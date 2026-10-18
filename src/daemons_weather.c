#include "global.h"
#include "overworld.h"
#include "region_map.h"
#include "daemons_time.h"
#include "daemons_weather.h"
#include "daemons_holidays.h"   // T-416 P4
#include "constants/weather.h"
#include "constants/region_map_sections.h"
#include "constants/pokemon.h"   // T-415 P2
#include "field_weather.h"         // T-415 P2
#include "fieldmap.h"
#include "palette.h"
#include "blend_palette.h"

// T-415: WEATHER, BY SEASON (decided by the user, 2026-10-17). Ruby and Sapphire's overworld weather, which FireRed
// ships and never uses, brought back by the season: spring rain and sometimes a thunderstorm, summer clear and
// sometimes a thunderstorm (drought is broken in FireRed's overworld, so summer has no sun), autumn fog and wind (for
// now Ruby's sandstorm stands in for wind and leaves, to be recoloured), winter snow (Gen 3 has no heavy snow).
//
// SPECIAL, NOT OFTEN. Most of the time the sky is clear. A day holds nought, one or two EVENTS. Each starts at any
// minute of the day, lasts from ten minutes to a whole day (both ends rare; usually about one watch), and has a SIZE:
// one town at the smallest, both regions at the largest (both ends rare) -- so THE MARGINS can have other weather than
// GAMUT. Size is a radius in region-map cells around the event's centre section, measured to the nearest cell of the
// section the player stands in; the islands' three sheets never reach each other except as one region.
//
// NO SAVE COST. Everything is drawn from the date: a seed per day and edition (CONTEXT's year is not CONTENT's), from
// the cartridge clock, or from play time where there is none (an hour of play is a day, so there an event's ten
// minutes pass in twenty-five seconds). Yesterday's events are drawn again too, so one that began last night is still
// raining after midnight. Applies only to an ordinary outdoor map -- header weather SUNNY on an outdoor map type;
// forests (SHADE), caves, interiors and the few outdoor maps vanilla left without weather keep their own.
//
// Read by SetSavedWeatherFromCurrMapHeader on every map load, and once a second while standing outside
// (overworld.c), so an event starting or ending changes the sky in front of the player.

EWRAM_DATA u8 gDaemonsWeatherOverride = 0;

// T-417 P4: HARMONY IN BALANCE, played on an ordinary outdoor map, turns this section's sky for the rest of the watch:
// a clear sky brings the season's own weather, any weather clears. Not saved -- a watch is a few hours at most.
static EWRAM_DATA u8 sSongSky = 0;          // DAEMONS_SKY_* + 1, or 0 for none
static EWRAM_DATA u8 sSongSkyMapsec = 0;
static EWRAM_DATA u8 sSongSkyWatch = 0;
static EWRAM_DATA u16 sSongSkyDay = 0;

#define SKY_MAX_EVENTS   2
#define SKY_SIZE_REGION  254   // the whole of the centre's region (GAMUT, or every island)
#define SKY_SIZE_BOTH    255   // both regions

struct SkyRange
{
    u16 lo;
    u16 hi;
    u8 weight;
};

struct SkyEvent
{
    u16 start;    // minutes into its day
    u16 length;   // minutes
    u8 sky;       // DAEMONS_SKY_*
    u8 sheet;     // the centre's region-map sheet: 0 the mainland, 1-3 the islands
    u8 cx;
    u8 cy;
    u8 radius;    // region-map cells, or SKY_SIZE_REGION / SKY_SIZE_BOTH
};

// ---- THE TUNABLE NUMBERS -------------------------------------------------------------------------------------------
// Weights, not percentages, though each row happens to add to 100. With these, a given spot outdoors has weather about
// one hour in eighteen (5-6% of the time), one in twenty-eight in summer -- measured by replaying this generator over
// 400 days at every ordinary outdoor section (DAEMONS tools/skyodds.py reads these tables; rerun it after a change).

// How many events a day holds: weights for 0, 1, 2.
static const u8 sSkyEventsPerDay[SEASON_COUNT][SKY_MAX_EVENTS + 1] = {
    [SEASON_WINTER] = { 30, 45, 25 },
    [SEASON_SPRING] = { 25, 45, 30 },
    [SEASON_SUMMER] = { 55, 35, 10 },   // summer is clear; a thunderstorm now and then
    [SEASON_AUTUMN] = { 30, 45, 25 },
};

// What an event is, by season: weights over DAEMONS_SKY_*.
static const u8 sSkyKinds[SEASON_COUNT][DAEMONS_SKY_COUNT] = {
    //                 CLEAR RAIN STORM SNOW FOG WIND
    [SEASON_WINTER] = {  0,    0,   0, 100,   0,  0 },
    [SEASON_SPRING] = {  0,   70,  30,   0,   0,  0 },
    [SEASON_SUMMER] = {  0,    0, 100,   0,   0,  0 },
    [SEASON_AUTUMN] = {  0,    0,   0,   0,  55, 45 },
};

// How long, in minutes (uniform inside a row): usually a watch or so, ten minutes and a whole day both rare.
static const struct SkyRange sSkyLengths[] = {
    {   10,   30,  6 },
    {   30,  120, 14 },
    {  120,  240, 22 },
    {  240,  480, 38 },   // about a watch -- the usual
    {  480,  960, 14 },
    {  960, 1440,  6 },
};

// How big, as a radius in region-map cells (uniform inside a row): one town and both regions both rare.
static const struct SkyRange sSkySizes[] = {
    { 0,               0,               6 },   // one town (or the one route)
    { 1,               2,              20 },   // a town and its roads
    { 3,               4,              30 },
    { 5,               7,              26 },   // much of a region
    { SKY_SIZE_REGION, SKY_SIZE_REGION, 13 },  // all of GAMUT, or all of THE MARGINS
    { SKY_SIZE_BOTH,   SKY_SIZE_BOTH,   5 },   // both
};

// Where the centre falls: weight of the mainland against the islands.
static const u8 sSkyCentreRegion[2] = { 60, 40 };
// ---------------------------------------------------------------------------------------------------------------------

static const u8 sSkyWeather[DAEMONS_SKY_COUNT] = {
    [DAEMONS_SKY_CLEAR] = WEATHER_SUNNY,
    [DAEMONS_SKY_RAIN]  = WEATHER_RAIN,
    [DAEMONS_SKY_STORM] = WEATHER_RAIN_THUNDERSTORM,
    [DAEMONS_SKY_SNOW]  = WEATHER_SNOW,
    [DAEMONS_SKY_FOG]   = WEATHER_FOG_HORIZONTAL,
    [DAEMONS_SKY_WIND]  = WEATHER_SANDSTORM,   // phase 1's stand-in for wind and leaves
};

#ifdef LEAFGREEN
#define SKY_EDITION_SALT 0x58544E43   // CONTEXT
#else
#define SKY_EDITION_SALT 0x544E4F43   // CONTENT
#endif

static u32 SkyHash(u32 x)
{
    x ^= x >> 16;
    x *= 0x7FEB352D;
    x ^= x >> 15;
    x *= 0x846CA68B;
    x ^= x >> 16;
    return x;
}

static u32 SkyNext(u32 *state)
{
    *state = SkyHash(*state + 0x6D2B79F5);
    return *state;
}

static u8 SkyPick(const u8 *weights, u8 count, u32 roll)
{
    u16 total = 0, at;
    u8 i;

    for (i = 0; i < count; i++)
        total += weights[i];
    if (total == 0)
        return 0;
    at = roll % total;
    for (i = 0; i < count; i++)
    {
        if (at < weights[i])
            return i;
        at -= weights[i];
    }
    return count - 1;
}

static u16 SkyRangePick(const struct SkyRange *rows, u8 count, u32 *state)
{
    u8 weights[8];
    u8 i;
    const struct SkyRange *row;

    for (i = 0; i < count; i++)
        weights[i] = rows[i].weight;
    row = &rows[SkyPick(weights, count, SkyNext(state))];
    return row->lo + SkyNext(state) % (row->hi - row->lo + 1);
}

// The centre: a region by sSkyCentreRegion, then any section of it the region map draws.
static void SkyCentre(struct SkyEvent *e, u32 *state)
{
    u8 islands = SkyPick(sSkyCentreRegion, 2, SkyNext(state));
    u8 tries, mapsec, sheet, x, y, w, h;

    for (tries = 0; tries < 48; tries++)
    {
        //  MAPSEC_NEXUS, not MAPSEC_SPECIAL_AREA: THE NEXUS's own section (2026-10-18) came after the sky was drawn, and
        //  the draw must not move every day's centre. It is drawn on no sheet, so it could never be picked anyway.
        mapsec = KANTO_MAPSEC_START + SkyNext(state) % (MAPSEC_NEXUS - KANTO_MAPSEC_START);
        if (DaemonsMapsecPlace(mapsec, &sheet, &x, &y, &w, &h) && (sheet != 0) == islands)
            break;
    }
    if (tries == 48)
        DaemonsMapsecPlace(islands ? MAPSEC_ONE_ISLAND : MAPSEC_SAFFRON_CITY, &sheet, &x, &y, &w, &h);
    e->sheet = sheet;
    e->cx = x + w / 2;
    e->cy = y + h / 2;
}

static u8 SkyDayEvents(u16 day, u8 season, struct SkyEvent *events)
{
    u32 state = SkyHash(day * 0x9E3779B9 ^ SKY_EDITION_SALT);
    u8 n = SkyPick(sSkyEventsPerDay[season], SKY_MAX_EVENTS + 1, SkyNext(&state));
    u8 i;

    for (i = 0; i < n; i++)
    {
        events[i].start = SkyNext(&state) % 1440;
        events[i].length = SkyRangePick(sSkyLengths, ARRAY_COUNT(sSkyLengths), &state);
        events[i].sky = SkyPick(sSkyKinds[season], DAEMONS_SKY_COUNT, SkyNext(&state));
        events[i].radius = SkyRangePick(sSkySizes, ARRAY_COUNT(sSkySizes), &state);
        SkyCentre(&events[i], &state);
    }
    return n;
}

static bool8 SkyCovers(const struct SkyEvent *e, u8 sheet, u8 x, u8 y, u8 w, u8 h)
{
    u8 dx, dy;

    if (e->radius == SKY_SIZE_BOTH)
        return TRUE;
    if ((sheet != 0) != (e->sheet != 0))
        return FALSE;
    if (e->radius == SKY_SIZE_REGION)
        return TRUE;
    if (sheet != e->sheet)
        return FALSE;
    dx = e->cx < x ? x - e->cx : (e->cx >= x + w ? e->cx - (x + w - 1) : 0);
    dy = e->cy < y ? y - e->cy : (e->cy >= y + h ? e->cy - (y + h - 1) : 0);
    return max(dx, dy) <= e->radius;
}

static u8 SkyHere(u8 mapsec)
{
    struct SkyEvent events[SKY_MAX_EVENTS];
    u16 day, minute;
    u8 sheet, x, y, w, h, season, n, i, back;
    s32 now, start;

    if (!DaemonsMapsecPlace(mapsec, &sheet, &x, &y, &w, &h))
        return DAEMONS_SKY_CLEAR;
    DaemonsDayAndMinute(&day, &minute);
    season = DaemonsSeason();
    now = minute + 1440;   // in a frame where yesterday began at 0
    // Today's first, so a new event wins over the tail of last night's.
    for (back = 0; back < 2; back++)
    {
        if (back == 1 && day == 0)
            break;
        n = SkyDayEvents(day - back, season, events);
        for (i = 0; i < n; i++)
        {
            start = events[i].start + (back ? 0 : 1440);
            if (now >= start && now < start + events[i].length && SkyCovers(&events[i], sheet, x, y, w, h))
                return events[i].sky;
        }
    }
    return DAEMONS_SKY_CLEAR;
}

// T-416 P4: A HOLIDAY'S SKY, over the day's draw (src/data/daemons_holidays.h's sHolidaySkies): BEACH DAY holds the
// islands clear whatever the draw said; SNOW CARVING snows on SLATE CITY at night and MASK NIGHT hazes HALFTONE and
// its roads, each only where the draw left the sky clear. Centred on the row's section, covering as an event would.
// tools/skyodds.py does not count them (holidays are not in its odds).
static u8 HolidaySky(u8 mapsec, u8 sky)
{
    struct SkyEvent e;
    u8 k, kind, centre, radius, sheet, x, y, w, h, cx, cy, cw, chh;
    bool8 onlyIfClear;

    if (!DaemonsMapsecPlace(mapsec, &sheet, &x, &y, &w, &h))
        return sky;
    for (k = 0; DaemonsHolidaySkyRow(k, &kind, &centre, &radius, &onlyIfClear); k++)
    {
        if (onlyIfClear && sky != DAEMONS_SKY_CLEAR)
            continue;
        if (!DaemonsMapsecPlace(centre, &e.sheet, &cx, &cy, &cw, &chh))
            continue;
        e.cx = cx + cw / 2;
        e.cy = cy + chh / 2;
        e.radius = radius;
        if (SkyCovers(&e, sheet, x, y, w, h))
            return kind;
    }
    return sky;
}

// T-417 P4: the song's sky holds in its own section, until the watch turns or the day does.
static bool8 SongSkyHolds(u8 mapsec)
{
    u16 day, minute;

    if (sSongSky == 0 || mapsec != sSongSkyMapsec || DaemonsWatch() != sSongSkyWatch)
        return FALSE;
    DaemonsDayAndMinute(&day, &minute);
    return day == sSongSkyDay;
}

u8 DaemonsWeatherForMap(u8 headerWeather, u8 mapType, u8 mapsec)
{
    u8 sky;

    if (headerWeather != WEATHER_SUNNY || !IsMapTypeOutdoors(mapType))
        return headerWeather;
    if (gDaemonsWeatherOverride)
        sky = (gDaemonsWeatherOverride - 1) % DAEMONS_SKY_COUNT;
    else if (SongSkyHolds(mapsec))
        sky = sSongSky - 1;
    else
        sky = HolidaySky(mapsec, SkyHere(mapsec));
    return sSkyWeather[sky];
}

// T-416: the sky over a section now, as an ordinary outdoor map there would show it (DAEMONS_SKY_*) -- a holiday gift
// asks it of the CHECKPOINT's own section, whatever the interior's header says.
u8 DaemonsSkyAt(u8 mapsec)
{
    if (gDaemonsWeatherOverride)
        return (gDaemonsWeatherOverride - 1) % DAEMONS_SKY_COUNT;
    if (SongSkyHolds(mapsec))
        return sSongSky - 1;
    return HolidaySky(mapsec, SkyHere(mapsec));
}

// T-416: BEACH DAY's BIG PEARL -- did a thunderstorm touch the islands today (any hour, any of their sheets)?
bool8 DaemonsStormOnIslandsToday(void)
{
    struct SkyEvent events[SKY_MAX_EVENTS];
    u16 day, minute;
    u8 n, i;

    DaemonsDayAndMinute(&day, &minute);
    n = SkyDayEvents(day, DaemonsSeason(), events);
    for (i = 0; i < n; i++)
    {
        if (events[i].sky == DAEMONS_SKY_STORM && (events[i].sheet != 0 || events[i].radius == SKY_SIZE_BOTH))
            return TRUE;
    }
    return FALSE;
}

// T-417 P4: HARMONY IN BALANCE (src/play.c). TRUE if the sky here was turned; FALSE where the day decides nothing (a
// cave, a forest, indoors), and nothing changes.
bool8 DaemonsSongSky(u8 headerWeather, u8 mapType, u8 mapsec)
{
    u16 minute;
    u8 season, sky, k;

    if (headerWeather != WEATHER_SUNNY || !IsMapTypeOutdoors(mapType))
        return FALSE;
    sky = SongSkyHolds(mapsec) ? sSongSky - 1 : HolidaySky(mapsec, SkyHere(mapsec));
    if (sky == DAEMONS_SKY_CLEAR)
    {
        season = DaemonsSeason();   // the season's own: the kind it brings most often
        for (k = 1; k < DAEMONS_SKY_COUNT; k++)
            if (sSkyKinds[season][k] > sSkyKinds[season][sky])
                sky = k;
    }
    else
    {
        sky = DAEMONS_SKY_CLEAR;
    }
    sSongSky = sky + 1;
    sSongSkyMapsec = mapsec;
    sSongSkyWatch = DaemonsWatch();
    DaemonsDayAndMinute(&sSongSkyDay, &minute);
    return TRUE;
}

// ---- T-415 PHASE 2 (the user, 2026-10-17): what the weather does outside the sky ------------------------------------

// The sky a field weather is, whatever put it there -- the day's draw, a song, DEBUG, or a map's own header (FIVE
// ISLAND's LOST CAVE keeps vanilla's fog, and is fog for all of this). Anything else is CLEAR.
u8 DaemonsSkyOfWeather(u8 weather)
{
    switch (weather)
    {
    case WEATHER_RAIN:
    case WEATHER_DOWNPOUR:
        return DAEMONS_SKY_RAIN;
    case WEATHER_RAIN_THUNDERSTORM:
        return DAEMONS_SKY_STORM;
    case WEATHER_SNOW:
        return DAEMONS_SKY_SNOW;
    case WEATHER_FOG_HORIZONTAL:
    case WEATHER_FOG_DIAGONAL:
        return DAEMONS_SKY_FOG;
    case WEATHER_SANDSTORM:
        return DAEMONS_SKY_WIND;
    }
    return DAEMONS_SKY_CLEAR;
}

u8 DaemonsSkyNow(void)
{
    return DaemonsSkyOfWeather(GetCurrentWeather());
}

// The field weathers' own boosts, x1.25 while they last (pokemon.c's CalculateBaseDamage, inside Cloud Nine's check).
// Rain's FLOW and ENTROPY are the battle's rain, which a thunderstorm brings too; these are the sky's: a thunderstorm
// lifts SIGNAL, fog LATENT (DRAFT, 2026-10-18: what runs unobserved, in weather that hides things), wind VECTOR. None
// has a weakened side of its own -- the storm's is rain's ENTROPY. The type chart is untouched (invariant 3).
bool8 DaemonsSkyBoostsType(u8 type)
{
    switch (DaemonsSkyNow())
    {
    case DAEMONS_SKY_STORM:
        return type == TYPE_ELECTRIC;
    case DAEMONS_SKY_FOG:
        return type == TYPE_GHOST;
    case DAEMONS_SKY_WIND:
        return type == TYPE_FLYING;
    }
    return FALSE;
}

// THE BATTLE TINT: the weather outside carried into the battle as a light wash over the terrain's three palettes
// (BG 2-4), in the unfaded buffer so every fade and every redrawn background starts from it. Subtle -- a few
// sixteenths. Never in HALFTONE (grey) or BREATHVERSE (timeless). Called after each load of the terrain's palette.
static const struct { u16 color; u8 coeff; } sSkyBattleTint[DAEMONS_SKY_COUNT] = {
    [DAEMONS_SKY_RAIN]  = { RGB( 8, 12, 20), 3 },
    [DAEMONS_SKY_STORM] = { RGB( 5,  7, 15), 4 },
    [DAEMONS_SKY_SNOW]  = { RGB(28, 30, 31), 3 },
    [DAEMONS_SKY_FOG]   = { RGB(24, 25, 27), 4 },
    [DAEMONS_SKY_WIND]  = { RGB(28, 21, 12), 2 },
};

void DaemonsTintBattleTerrain(void)
{
    u8 sky = DaemonsSkyNow();

    if (sky == DAEMONS_SKY_CLEAR || DaemonsIsHalftone() || DaemonsIsTimeless())
        return;
    BlendPalette(BG_PLTT_ID(2), 3 * 16, sSkyBattleTint[sky].coeff, sSkyBattleTint[sky].color);
    CpuCopy16(&gPlttBufferFaded[BG_PLTT_ID(2)], &gPlttBufferUnfaded[BG_PLTT_ID(2)], PLTT_SIZEOF(3 * 16));
}

// THE GLINT: may a boosted routine's name glint here? Not in HALFTONE or BREATHVERSE, as the tint.
bool8 DaemonsSkyGlintAllowed(void)
{
    return !DaemonsIsHalftone() && !DaemonsIsTimeless();
}
