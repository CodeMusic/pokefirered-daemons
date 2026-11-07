#include "global.h"
#include "main.h"
#include "palette.h"
#include "event_data.h"
#include "overworld.h"
#include "fieldmap.h"
#include "battle.h"
#include "battle_main.h"
#include "constants/maps.h"
#include "constants/map_types.h"
#include "constants/flags.h"
#include "daemons_time.h"
#include "daemons_pulse.h"
#include "daemons_battle_bg.h"
#include "daemons_aurora.h"

// T-528 PART 1: THE AURORA, THE DAWN THAT COMES AT NIGHT (the user, 2026-11-07, by multiple choice; vision.md 4.40).
// "Once the unity book is restored it creates some effect surrounding it, and that illumination goes further."
//
// ONE ATOM, TWO COLOURS. Both are atomic oxygen: CONTENT's sky glows in the high red line (630.0 nm), CONTEXT's in the
// common green (557.7 nm). The colour says which cartridge is looking, and nothing else (invariant 5); north and south
// are the map's -- the mainland the north wind's side, the Sevii islands the south wind's -- in both editions.
//
// WHERE, in the order it was built:
//   (a) OVER DAEMON TOWER, every night once the book is mended: the tower's own pale stones (LAVENDER's palette row 7,
//       which in the town only the tower draws) breathe toward the edition's line.
//   (b) IN THE GROVES' BATTLE SKIES, on the light's nights, in a grove whose light is on: a band across T-527's sky
//       bands (T-481's, which the hour has already stepped toward navy) -- CONTENT's red high in the sky, CONTEXT's
//       green lower, as the two lines hang in a real aurora. A grove takes the light the first night it is entered
//       after the mending, and lights its pair across the sea (sGroves below: conjugate aurorae share a field line).
//   (c) OUTDOORS AT NIGHT, on the light's nights, once every grove found is lit: a faint breath over 9.21's night on the
//       map's own rows. Never in a building or a cave (map type), never in a timeless place.
//
// THE LIGHT'S NIGHTS: about one in three, from the date, so the game and the companion agree (Aurora_IsLightNight;
// DAEMONS docs/TODO.md T-528 states the rule as arithmetic). THE BREATH: the creator's hour's sixteen-second one
// (Pulse_GoldBreath, the pulse's clock), so a cartridge and the companion breathe together.
//
// HOW: laid on PLTT after the pulse, read back from what is there -- so the palette buffers are never touched, the
// watch's tint, the season, the weather's wash (T-415) and the pulse (T-482) stay underneath, and a fade takes the
// light with it. HALFTONE's grey outranks it (the mended town greys in the witching half-hour), as it outranks the
// pulse. Link battles keep their own light.
//
// COST: EWRAM 0 (1 in DEBUG), IWRAM 24, save 14 flags (FLAG_*_GROVE_LIT, the DAEMONS block 0x120-0x12D).

#ifdef FIRERED
static const u16 sLine = RGB(30, 7, 9);        // CONTENT: oxygen's red line, 630.0 nm
#else
static const u16 sLine = RGB(10, 30, 15);      // CONTEXT: oxygen's green line, 557.7 nm
#endif

// THE GROVES AND THEIR PAIRS: each mainland grove (the north wind's side, psychology's residents) lit with an island
// grove (the south wind's, machine learning's), by what their residents are -- DAEMONS docs/TODO.md T-528 gives each
// pair's reason. In FOUND-flag order; LIT is FOUND + 0x10.
struct AuroraGrove
{
    u16 map;
    u16 found;
    u16 lit;
    u8 pair;
};

enum
{
    G_VIRIDIAN, G_ROUTE25, G_ROUTE1, G_ROUTE24, G_ROUTE8, G_ROUTE11, G_ROUTE13,
    G_FIVE, G_SIX, G_THREE, G_ONE, G_TWO, G_FOUR, G_SEVEN,
};

static const struct AuroraGrove sGroves[AURORA_GROVE_COUNT] = {
    [G_VIRIDIAN] = { MAP_VIRIDIAN_FOREST_GROVE,           FLAG_VIRIDIAN_FOREST_GROVE_FOUND,           FLAG_VIRIDIAN_FOREST_GROVE_LIT,           G_THREE },
    [G_ROUTE25]  = { MAP_ROUTE25_GROVE,                   FLAG_ROUTE25_GROVE_FOUND,                   FLAG_ROUTE25_GROVE_LIT,                   G_SEVEN },
    [G_ROUTE1]   = { MAP_ROUTE1_GROVE,                    FLAG_ROUTE1_GROVE_FOUND,                    FLAG_ROUTE1_GROVE_LIT,                    G_ONE },
    [G_ROUTE24]  = { MAP_ROUTE24_GROVE,                   FLAG_ROUTE24_GROVE_FOUND,                   FLAG_ROUTE24_GROVE_LIT,                   G_FOUR },
    [G_ROUTE8]   = { MAP_ROUTE8_GROVE,                    FLAG_ROUTE8_GROVE_FOUND,                    FLAG_ROUTE8_GROVE_LIT,                    G_SIX },
    [G_ROUTE11]  = { MAP_ROUTE11_GROVE,                   FLAG_ROUTE11_GROVE_FOUND,                   FLAG_ROUTE11_GROVE_LIT,                   G_TWO },
    [G_ROUTE13]  = { MAP_ROUTE13_GROVE,                   FLAG_ROUTE13_GROVE_FOUND,                   FLAG_ROUTE13_GROVE_LIT,                   G_FIVE },
    [G_FIVE]     = { MAP_FIVE_ISLAND_MEADOW_GROVE,        FLAG_FIVE_ISLAND_MEADOW_GROVE_FOUND,        FLAG_FIVE_ISLAND_MEADOW_GROVE_LIT,        G_ROUTE13 },
    [G_SIX]      = { MAP_SIX_ISLAND_WATER_PATH_GROVE,     FLAG_SIX_ISLAND_WATER_PATH_GROVE_FOUND,     FLAG_SIX_ISLAND_WATER_PATH_GROVE_LIT,     G_ROUTE8 },
    [G_THREE]    = { MAP_THREE_ISLAND_BERRY_FOREST_GROVE, FLAG_THREE_ISLAND_BERRY_FOREST_GROVE_FOUND, FLAG_THREE_ISLAND_BERRY_FOREST_GROVE_LIT, G_VIRIDIAN },
    [G_ONE]      = { MAP_ONE_ISLAND_GROVE,                FLAG_ONE_ISLAND_GROVE_FOUND,                FLAG_ONE_ISLAND_GROVE_LIT,                G_ROUTE1 },
    [G_TWO]      = { MAP_TWO_ISLAND_GROVE,                FLAG_TWO_ISLAND_GROVE_FOUND,                FLAG_TWO_ISLAND_GROVE_LIT,                G_ROUTE11 },
    [G_FOUR]     = { MAP_FOUR_ISLAND_GROVE,               FLAG_FOUR_ISLAND_GROVE_FOUND,               FLAG_FOUR_ISLAND_GROVE_LIT,               G_ROUTE24 },
    [G_SEVEN]    = { MAP_SEVEN_ISLAND_GROVE,              FLAG_SEVEN_ISLAND_GROVE_FOUND,              FLAG_SEVEN_ISLAND_GROVE_LIT,              G_ROUTE25 },
};

// (b) the band across the sky bands, in 256ths of its strength, top first: the red line high, the green one lower.
#define SKY_MAX 6
#ifdef FIRERED
static const u8 sBand[SKY_MAX] = { 255, 160, 64, 16, 0, 0 };
#else
static const u8 sBand[SKY_MAX] = { 96, 255, 160, 48, 16, 0 };
#endif

// (a) DAEMON TOWER's pale stones in LAVENDER's row 7 (the tower is the only thing in the town that draws the row),
// and each one's share of the light.
#define TOWER_ROW 7
static const u8 sTowerStones[][2] = { { 1, 255 }, { 9, 255 }, { 2, 192 }, { 6, 128 } };

// IWRAM, uninitialised: what this frame lays on PLTT.
static u32 sRowMask;            // (c): BG rows washed whole, colours 1-15
static u8 sRowWeight;
static u8 sCount;               // (a) and (b): single colours, each with its own weight
static u16 sEntry[SKY_MAX];
static u8 sWeight[SKY_MAX];

#if DAEMONS_DEBUG
EWRAM_DATA u8 gAuroraDebugNights = 0;
#endif

// THE LIGHT'S NIGHTS. N is the night's date as a day number: days since 2000-01-01 on the cartridge's clock (DEBUG's
// DATE moves it), or the hours played without one (an hour of play is a day, T-273) -- and a night belongs to the
// evening it began on, so before noon it is the day before's. Then two rounds of a 16-bit multiplicative hash, and
// one night in three: every product fits in 32 bits, so the companion's TypeScript gets the same answer.
u16 Aurora_NightNumber(void)
{
    u16 day, minute;

    DaemonsDayAndMinute(&day, &minute);
    if (minute < 12 * 60)
        day--;
    return day;
}

bool8 Aurora_IsLightNight(void)
{
    u32 x;

#if DAEMONS_DEBUG
    if (gAuroraDebugNights != 0)
        return gAuroraDebugNights == 1;
#endif
    x = ((u32)Aurora_NightNumber() * 40503) & 0xFFFF;
    x ^= x >> 7;
    x = (x * 40503) & 0xFFFF;
    return ((x >> 8) % 3) == 0;
}

static u16 MapHere(void)
{
    return gSaveBlock1Ptr->location.mapNum | (gSaveBlock1Ptr->location.mapGroup << 8);
}

static s8 GroveHere(void)
{
    u16 here = MapHere();
    u32 i;

    for (i = 0; i < AURORA_GROVE_COUNT; i++)
    {
        if (sGroves[i].map == here)
            return i;
    }
    return -1;
}

void Aurora_OnArrive(void)
{
    s8 grove;

    if (!FlagGet(FLAG_UNITY_MENDED) || DaemonsWatch() != WATCH_NIGHT || (grove = GroveHere()) < 0)
        return;
    FlagSet(sGroves[grove].lit);
    FlagSet(sGroves[sGroves[grove].pair].lit);
}

// (c)'s gate: every grove found is lit, and at least one is.
static bool8 EveryFoundGroveLit(void)
{
    u32 i;
    bool8 any = FALSE;

    for (i = 0; i < AURORA_GROVE_COUNT; i++)
    {
        if (FlagGet(sGroves[i].lit))
            any = TRUE;
        else if (FlagGet(sGroves[i].found))
            return FALSE;
    }
    return any;
}

static void AddEntry(u16 entry, u32 weight)
{
    if (sCount < SKY_MAX && weight != 0)
    {
        sEntry[sCount] = entry;
        sWeight[sCount] = min(weight, 255);
        sCount++;
    }
}

void Aurora_Tick(void)
{
    const u8 *sky;
    u32 breath, fade, i, n;
    s8 grove;

    sRowMask = 0;
    sCount = 0;
    if (!FlagGet(FLAG_UNITY_MENDED) || DaemonsIsHalftone() || DaemonsWatch() != WATCH_NIGHT)
        return;
    breath = Pulse_GoldBreath();
    if (DaemonsFieldOnScreen())
    {
        // (a) over the tower, every night
        if (MapHere() == MAP_LAVENDER_TOWN)
        {
            for (i = 0; i < ARRAY_COUNT(sTowerStones); i++)
                AddEntry(BG_PLTT_ID(TOWER_ROW) + sTowerStones[i][0], (64 + breath * 112 / 255) * sTowerStones[i][1] / 255);
        }
        // (c) outdoors, on the light's nights, once every grove found is lit
        if (IsMapTypeOutdoors(gMapHeader.mapType) && !DaemonsIsTimeless() && EveryFoundGroveLit() && Aurora_IsLightNight())
        {
            sRowMask = (1 << 13) - 1;       // the map's own rows, 0-12; 13-15 are the windows and the text
            sRowWeight = 10 + breath * 22 / 255;
        }
    }
    else if (gMain.inBattle && gMain.callback2 == BattleMainCB2 && !(gBattleTypeFlags & BATTLE_TYPE_LINK))
    {
        // (b) a lit grove's battle sky, on the light's nights
        grove = GroveHere();
        if (grove >= 0 && FlagGet(sGroves[grove].lit) && Aurora_IsLightNight()
         && (n = DaemonsBattleBgSkySlots(&sky)) != 0)
        {
            for (i = 0; i < n; i++)
                AddEntry(BG_PLTT_ID(2) + sky[i], (72 + breath * 96 / 255) * sBand[i] / 255);
        }
    }
    // A fade takes the light with it, as it takes the pulse.
    if ((sRowMask != 0 || sCount != 0) && (gPaletteFade.active || gPaletteFade.y != 0))
    {
        fade = 16 - min(gPaletteFade.y, 16);
        sRowWeight = sRowWeight * fade / 16;
        for (i = 0; i < sCount; i++)
            sWeight[i] = sWeight[i] * fade / 16;
    }
}

static void Lay(u16 *pltt, u32 e, s32 w)
{
    u16 c = pltt[e];
    s32 r = GET_R(c), g = GET_G(c), b = GET_B(c);

    r += ((GET_R(sLine) - r) * w) >> 8;
    g += ((GET_G(sLine) - g) * w) >> 8;
    b += ((GET_B(sLine) - b) * w) >> 8;
    pltt[e] = RGB(r, g, b);
}

void Aurora_ApplyToPltt(void)
{
    u16 *pltt = (u16 *)PLTT;
    u32 mask = sRowMask, p, e, i;

    if (mask != 0 && sRowWeight != 0)
    {
        for (p = 0; mask != 0; p++, mask >>= 1)
        {
            if (mask & 1)
            {
                for (e = p * 16 + 1; e < p * 16 + 16; e++)
                    Lay(pltt, e, sRowWeight);
            }
        }
    }
    for (i = 0; i < sCount; i++)
        Lay(pltt, sEntry[i], sWeight[i]);
}

#if DAEMONS_DEBUG
void Aurora_DebugLightAll(bool8 on)    // DEBUG -> GAME EVENTS -> EVERY GROVE LIT
{
    u32 i;

    for (i = 0; i < AURORA_GROVE_COUNT; i++)
    {
        if (on)
            FlagSet(sGroves[i].lit);
        else
            FlagClear(sGroves[i].lit);
    }
}

bool8 Aurora_DebugAllLit(void)
{
    u32 i;

    for (i = 0; i < AURORA_GROVE_COUNT; i++)
    {
        if (!FlagGet(sGroves[i].lit))
            return FALSE;
    }
    return TRUE;
}
#endif
