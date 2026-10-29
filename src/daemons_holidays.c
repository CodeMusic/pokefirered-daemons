#include "global.h"
#include "gflib.h"
#include "event_data.h"
#include "m4a.h"
#include "overworld.h"
#include "pokemon.h"
#include "string_util.h"
#include "task.h"
#include "daemons_time.h"
#include "daemons_weather.h"
#include "daemons_holidays.h"
#include "play.h"
#include "event_scripts.h"
#include "field_weather.h"
#include "fieldmap.h"
#include "map_name_popup.h"
#include "quest_log.h"
#include "random.h"
#include "region_map.h"
#include "script.h"
#include "constants/weather.h"
#include "constants/flags.h"
#include "constants/items.h"
#include "constants/maps.h"
#include "constants/moves.h"
#include "constants/region_map_sections.h"
#include "constants/species.h"
#include "constants/vars.h"

// T-416 HOLIDAYS ON THE REAL CALENDAR, AND A COLOUR AND A NOTE FOR EACH MONTH (decided by the user 2026-10-17/18).
//
// THE DAY. Three holidays a month: one COMMON to both editions, one for CONTENT (the northern year), one for CONTEXT
// (the southern). A ROM carries 24: the COMMON twelve (ids 1-12) and its own (13-24). With the cartridge's clock, a
// holiday holds its date (and ROLLOVER the evening before, from dusk); with no clock, play time's 28-hour year holds
// each one for a single watch (15 minutes of play). Two on one day: the edition's shows, and the COMMON one's gift
// still waits at the CHECKPOINT. SCIENCE FAIR steps off FLEDGLING DAY (it is not on a day a COMMON one holds).
//
// THE MONTH. Each month is a note of the chromatic scale, C (JAN) to B (DEC), in the user's colours. The day banner
// wears the month's note as its mark, in the month's colour, on the 1st and under a holiday, and plays the note on
// the edition's instrument; the trainer card's title wears the month's colour. Nothing says what either means.
//
// THE GIFTS (P2). The giver a holiday names -- the CHECKPOINT's attendant, MOM, the sand on FINDERS BEACH -- has
// something: an item once per holiday per year (a flag each, cleared when VAR_HOLIDAY_YEAR is not this year), or a
// line for THE YEAR (a NOTEBOOK section), once ever. Some are dynamic: the watch, the sky, the weekday, the lead's
// type, a storm on the islands today. Every word is DRAFT. The table is DAEMONS tools/genholidays.py's.

#include "data/daemons_holidays.h"

#define PLAY_YEAR_MINUTES (HOLIDAY_PLAY_YEAR_WATCHES * 15)   // 1680: an hour of play is a day, 28 hours a year
#define PLAY_YEAR_STARTS  171                                // June 21, as days after January 1 (a common year)

static EWRAM_DATA u8 sMonth = 0;            // 1-12; 0 before the first read
static EWRAM_DATA u8 sMonthDay = 0;
static EWRAM_DATA u8 sHoliday = 0;          // the one that shows
static EWRAM_DATA u8 sHolidayCommon = 0;    // the COMMON one, when the edition's shows over it
static EWRAM_DATA u16 sYearKey = 0;         // VAR_HOLIDAY_YEAR's value for now
static EWRAM_DATA u8 sGiver = 0;            // P2: the walk through today's gifts at one giver
static EWRAM_DATA u8 sGiftStage = 0;
static EWRAM_DATA u8 sGiftIdx = 0;
static EWRAM_DATA u8 sGiftOpen = 0;
static EWRAM_DATA u8 sGiftPending = 0;

static const u8 sMonthLen[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

// ------------------------------------------------------------------------------------------------------------ dates

static void DateOf(u16 dn, u16 *year, u8 *month, u8 *day)
{
    u16 y = 2000, len;
    u8 m, ml;

    for (;;)
    {
        len = (y % 4 == 0) ? 366 : 365;
        if (dn < len)
            break;
        dn -= len;
        y++;
    }
    for (m = 1; m < 12; m++)
    {
        ml = sMonthLen[m - 1] + (m == 2 && y % 4 == 0);
        if (dn < ml)
            break;
        dn -= ml;
    }
    *year = y;
    *month = m;
    *day = dn + 1;
}

// A holiday's first day in YEAR, as a day number (daemons_time.c's: days since 2000-01-01, a Saturday).
static u16 HolidayStart(const struct HolidayRow *row, u16 year)
{
    u16 first;
    u8 wd1;
    bool8 inTable = (year >= HOLIDAY_FIRST_YEAR && year <= HOLIDAY_LAST_YEAR);

    switch (row->rule)
    {
    case HOLIDAY_RULE_NTH_WEEKDAY:
        first = DaemonsDayNumber(year, row->month, 1);
        wd1 = (first + 6) % 7;
        return first + (row->weekday + 7 - wd1) % 7 + 7 * (row->arg - 1);
    case HOLIDAY_RULE_EASTER:
        if (inTable)
            return DaemonsDayNumber(year, 3, 21) + sEasterAfterMar21[year - HOLIDAY_FIRST_YEAR] + row->arg;
        return DaemonsDayNumber(year, row->anchorMonth, row->anchorDay);
    case HOLIDAY_RULE_MIDAUTUMN:
        if (inTable)
            return DaemonsDayNumber(year, 9, 1) + sMidAutumnAfterSep1[year - HOLIDAY_FIRST_YEAR];
        return DaemonsDayNumber(year, row->anchorMonth, row->anchorDay);
    default:
        return DaemonsDayNumber(year, row->month, row->arg);
    }
}

static bool8 HolidayOn(const struct HolidayRow *row, u16 year, u16 dn, u8 hour)
{
    u16 start = HolidayStart(row, year);

    if (dn >= start && dn - start < row->days)
        return TRUE;
    if ((row->flags & HF_EVE_FROM_DUSK) && hour >= 18)
    {
        if (dn + 1 == start || dn + 1 == HolidayStart(row, year + 1))
            return TRUE;
    }
    return FALSE;
}

u16 DaemonsHolidays_FromDate(u16 year, u8 month, u8 day, u8 hour, u16 dn)
{
    u8 i, own = 0, common = 0;

    sMonth = month;
    sMonthDay = day;
    sYearKey = year - 2000;
    for (i = 0; i < HOLIDAY_COUNT; i++)
    {
        if (!HolidayOn(&sHolidays[i], year, dn, hour))
            continue;
        if (i < HOLIDAY_COUNT / 2)
        {
            if (common == 0)
                common = i + 1;
        }
        else if (own == 0)
        {
            own = i + 1;
        }
    }
    // SCIENCE FAIR steps off FLEDGLING DAY (the user, 2026-10-18): not on a day a COMMON one holds
    if (own != 0 && (sHolidays[own - 1].flags & HF_STEPS_OFF) && common != 0)
        own = 0;
    if (own != 0)
    {
        sHoliday = own;
        sHolidayCommon = common;
    }
    else
    {
        sHoliday = common;
        sHolidayCommon = 0;
    }
    return sHoliday ? sHolidays[sHoliday - 1].flags : 0;
}

// With no clock: the play-time year starts at June 21; a holiday holds the one watch genholidays gave it.
void DaemonsHolidays_FromPlay(u32 playMinutes)
{
    u32 inYear = playMinutes % PLAY_YEAR_MINUTES;
    u16 doy = (inYear * 365 / PLAY_YEAR_MINUTES + PLAY_YEAR_STARTS) % 365;
    u8 watch = (playMinutes / 15) % HOLIDAY_PLAY_YEAR_WATCHES, i;
    u16 year;

    DateOf(DaemonsDayNumber(2001, 1, 1) + doy, &year, &sMonth, &sMonthDay);
    sYearKey = 0x8000 | (playMinutes / PLAY_YEAR_MINUTES);
    sHoliday = 0;
    sHolidayCommon = 0;
    for (i = 0; i < HOLIDAY_COUNT; i++)
    {
        if (sHolidays[i].playWatch == watch)
        {
            sHoliday = i + 1;
            break;
        }
    }
}

// The clock's hours (daemons_time.c: dawn 5-7, day 7-18, dusk 18-20, night 20-5), and LONGEST DAY's and NIGHT's.
u8 DaemonsHolidays_WatchFromHour(u8 hour, u16 flags)
{
    u8 dayEnd = 18, duskEnd = 20;

    if (flags & HF_LONG_DAY)
    {
        dayEnd = 21;
        duskEnd = 22;
    }
    else if (flags & HF_LONG_NIGHT)
    {
        dayEnd = 16;
        duskEnd = 17;
    }
    if (hour >= 5 && hour < 7)
        return WATCH_DAWN;
    if (hour >= 7 && hour < dayEnd)
        return WATCH_DAY;
    if (hour >= dayEnd && hour < duskEnd)
        return WATCH_DUSK;
    return WATCH_NIGHT;
}

// ------------------------------------------------------------------------------------------------------- the answers

u8 DaemonsHoliday(void)
{
    DaemonsTimeRefresh();
    return sHoliday;
}

u8 DaemonsHolidayCommon(void)
{
    DaemonsTimeRefresh();
    return sHolidayCommon;
}

u8 DaemonsHolidayForBanner(void)
{
    u8 h = DaemonsHoliday();

    if (h != HOLIDAY_NONE && ((sHolidays[h - 1].bannerWatches >> DaemonsWatch()) & 1))
        return h;
    return HOLIDAY_NONE;
}

u16 DaemonsHolidayFlags(u8 holiday)
{
    return (holiday == HOLIDAY_NONE || holiday > HOLIDAY_COUNT) ? 0 : sHolidays[holiday - 1].flags;
}

const u8 *DaemonsHolidayName(u8 holiday)
{
    return (holiday == HOLIDAY_NONE || holiday > HOLIDAY_COUNT) ? NULL : sHolidays[holiday - 1].name;
}

u8 DaemonsHolidayMonth(u8 holiday)
{
    return (holiday == HOLIDAY_NONE || holiday > HOLIDAY_COUNT) ? DaemonsMonth() : sHolidays[holiday - 1].month - 1;
}

u8 DaemonsMonth(void)
{
    DaemonsTimeRefresh();
    return sMonth != 0 ? sMonth - 1 : 5;
}

bool8 DaemonsIsFirstOfMonth(void)
{
    DaemonsTimeRefresh();
    return sMonthDay == 1;
}

//  T-460: one day of the year by the calendar the holidays read -- the cartridge clock, the play-time year without
//  one, or the DEBUG date. FALSE before the first read.
// T-483: a day number (daemons_time.c's, from 2000-01-01) as a date -- Halloween's costume hashes the date.
void DaemonsDateOf(u16 dayNumber, u16 *year, u8 *month, u8 *day)
{
    DateOf(dayNumber, year, month, day);
}

bool8 DaemonsIsCalendarDay(u8 month, u8 day)
{
    DaemonsTimeRefresh();
    return sMonth == month && sMonthDay == day;
}

u16 DaemonsMonthColour(u8 month)
{
    return sMonthColours[month % 12];
}

const u8 *DaemonsMonthNote(u8 month)
{
    return sMonthNotes[month % 12];
}

// The month's note on the edition's instrument (PLAY's sounds, src/play.c): the HARP plucks and dies away by itself;
// the FLUTE's note is a held breath, so a task lets it out after a beat.
#define tTimer data[0]
#define tSoft  data[1]
#define MONTH_NOTE_HOLD 36
#define MONTH_NOTE_FADE 10

static void Task_MonthNoteBreath(u8 taskId)
{
    s16 *data = gTasks[taskId].data;
    u16 full = tSoft ? 96 : 256;

    if (++tTimer <= MONTH_NOTE_HOLD)
        return;
    if (tTimer >= MONTH_NOTE_HOLD + MONTH_NOTE_FADE)
    {
        m4aMPlayStop(&gMPlayInfo_SE2);
        DestroyTask(taskId);
        return;
    }
    m4aMPlayVolumeControl(&gMPlayInfo_SE2, TRACKS_ALL, full * (MONTH_NOTE_HOLD + MONTH_NOTE_FADE - tTimer) / MONTH_NOTE_FADE);
}

void DaemonsPlayMonthNote(u8 month, bool8 soft)
{
    u8 taskId;

    Play_SoundNote(PLAY_INSTRUMENT, month % 12, FALSE);
    m4aMPlayVolumeControl(&gMPlayInfo_SE2, TRACKS_ALL, soft ? 96 : 256);
    if (PLAY_INSTRUMENT == PLAY_FLUTE)
    {
        taskId = FindTaskIdByFunc(Task_MonthNoteBreath);
        if (taskId == TASK_NONE)
            taskId = CreateTask(Task_MonthNoteBreath, 90);
        gTasks[taskId].tTimer = 0;
        gTasks[taskId].tSoft = soft;
    }
}

#undef tTimer
#undef tSoft

// ----------------------------------------------------------------------------------------------------- the daemons

// T-416 P3: A HOLIDAY'S DAEMONS. A row of sHolidayShifts gives one wild slot of one map to another daemon while its
// holiday holds, in the watches it names -- on land (wild_encounter.c's WildShiftSpecies asks this first, so a holiday
// wins over the season's or the hour's row on that slot, that day) or on a rod (STREAMERS' in DOLDRUM CITY). Only the
// species moves: the slot keeps its level and its odds. PAIRING DAY and FIR LIGHTS bring the other edition's daemons in
// with ordinary rows -- those two days are the only ones they come. Both of the day's holidays count (the edition's and
// the COMMON one under it). The INDEX's area map does not learn them: it shows homes, not holidays.
u16 DaemonsHolidayShiftSpecies(u16 species, u8 area, u8 slot)
{
    u32 i;
    u8 h, common, watchBit;

    h = DaemonsHoliday();
    common = sHolidayCommon;
    if (h == HOLIDAY_NONE)
        return species;
    watchBit = 1 << DaemonsWatch();
    for (i = 0; i < ARRAY_COUNT(sHolidayShifts); i++)
    {
        if ((sHolidayShifts[i].holiday == h || (common != HOLIDAY_NONE && sHolidayShifts[i].holiday == common))
         && sHolidayShifts[i].area == area
         && sHolidayShifts[i].slot == slot
         && (sHolidayShifts[i].watches & watchBit)
         && sHolidayShifts[i].mapGroup == gSaveBlock1Ptr->location.mapGroup
         && sHolidayShifts[i].mapNum == gSaveBlock1Ptr->location.mapNum)
            return sHolidayShifts[i].species;
    }
    return species;
}

// ---------------------------------------------------------------------------------------------------- the routines

// T-392 P3: A HOLIDAY'S ROUTINES. On its day a routine's signature takes the look its row names (daemons_signature.c
// plays it): the holiday that shows and the COMMON one under it both count, as for the daemons. A QUIET day dresses
// none -- its signatures keep their ink and play soft.
const struct HolidayRoutineLook *DaemonsHolidayRoutineLook(u16 move)
{
    u32 i;
    u8 h = DaemonsHoliday(), common = sHolidayCommon;

    if (h == HOLIDAY_NONE || (DaemonsHolidayFlags(h) & HF_QUIET))
        return NULL;
    for (i = 0; i < ARRAY_COUNT(sHolidayRoutineLooks); i++)
        if (sHolidayRoutineLooks[i].move == move
         && (sHolidayRoutineLooks[i].holiday == h || (common != HOLIDAY_NONE && sHolidayRoutineLooks[i].holiday == common)))
            return &sHolidayRoutineLooks[i];
    return NULL;
}

bool8 DaemonsHolidayIsQuiet(void)
{
    return (DaemonsHolidayFlags(DaemonsHoliday()) & HF_QUIET) != 0;
}

// ---------------------------------------------------------------------------------------------------- the overlays

// T-416 P4: WHAT A HOLIDAY DOES TO THE FIELD. Only the holiday that shows (the edition's, when two hold) dresses it;
// only what its row's HF_* name, so a QUIET day is quiet -- no colour, no fireworks -- save LANTERN NIGHTS' own lantern.
// Outdoors only, never in HALFTONE (grey outranks everything) or BREATHVERSE (timeless): the callers rule those out.
u8 DaemonsHolidayLift(void)
{
    u16 flags = DaemonsHolidayFlags(DaemonsHoliday());
    u8 watch, sheet, x, y, w, h;

    if (!(flags & (HF_LANTERN | HF_FROST | HF_SNOW_LIGHT | HF_BRIGHT_DAY)))
        return HOLIDAY_LIFT_NONE;
    watch = DaemonsWatch();
    if ((flags & HF_LANTERN) && (watch == WATCH_DUSK || watch == WATCH_NIGHT))
        return HOLIDAY_LIFT_LANTERN;
    if ((flags & HF_FROST) && watch == WATCH_DAWN)
        return HOLIDAY_LIFT_FROST;
    if ((flags & HF_SNOW_LIGHT) && watch == WATCH_NIGHT && DaemonsSkyAt(gMapHeader.regionMapSectionId) == DAEMONS_SKY_SNOW)
        return HOLIDAY_LIFT_SNOW;
    if ((flags & HF_BRIGHT_DAY) && watch == WATCH_DAY
     && DaemonsMapsecPlace(gMapHeader.regionMapSectionId, &sheet, &x, &y, &w, &h) && sheet != 0)
        return HOLIDAY_LIFT_BRIGHT;
    return HOLIDAY_LIFT_NONE;
}

// VIOLET BLOOM: the month's own purple (NOV's) for the darkest petal, and two steps of it toward white for the others.
u16 DaemonsHolidayBloomColour(u8 shade)
{
    u16 c = sMonthColours[10];
    s32 r = GET_R(c), g = GET_G(c), b = GET_B(c), toward = shade == 0 ? 9 : shade == 1 ? 5 : 0;   // sixteenths

    r += (31 - r) * toward / 16;
    g += (31 - g) * toward / 16;
    b += (31 - b) * toward / 16;
    return RGB(r, g, b);
}

bool8 DaemonsHolidayFirGlow(void)
{
    return (DaemonsHolidayFlags(DaemonsHoliday()) & HF_FIR_GLOW) != 0;
}

// The found fir's palette (NPC_GREEN, its own slot), a step brighter and a touch warmer -- before the watch's tint, so
// the night still falls on it.
void DaemonsHolidayGlow(u16 *unfaded, u16 *faded)
{
    u8 i;
    s32 r, g, b;

    for (i = 1; i < 16; i++)
    {
        r = GET_R(unfaded[i]);
        g = GET_G(unfaded[i]);
        b = GET_B(unfaded[i]);
        r += (31 - r) * 5 / 16;
        g += (30 - g) * 4 / 16;
        b += (18 - b) * 2 / 16;
        unfaded[i] = RGB(r, g, b);
        faded[i] = unfaded[i];
    }
}

bool8 DaemonsHolidaySkyRow(u8 k, u8 *sky, u8 *mapsec, u8 *radius, bool8 *onlyIfClear)
{
    u32 i;
    u8 h = DaemonsHoliday(), common = sHolidayCommon, watchBit;

    if (h == HOLIDAY_NONE)
        return FALSE;
    watchBit = 1 << DaemonsWatch();
    for (i = 0; i < ARRAY_COUNT(sHolidaySkies); i++)
    {
        if (sHolidaySkies[i].holiday != h && (common == HOLIDAY_NONE || sHolidaySkies[i].holiday != common))
            continue;
        if (!(sHolidaySkies[i].watches & watchBit))
            continue;
        if (k-- != 0)
            continue;
        *sky = sHolidaySkies[i].sky;
        *mapsec = sHolidaySkies[i].mapsec;
        *radius = sHolidaySkies[i].radius;
        *onlyIfClear = sHolidaySkies[i].onlyIfClear;
        return TRUE;
    }
    return FALSE;
}

// THE LINES THAT ARRIVE ON THEIR OWN: LANTERN NIGHTS' in HALFTONE TOWN after dusk, DAWN VIGIL's anywhere outdoors at
// dawn. Asked once a second while the player has the field (overworld.c), after the day's banner has gone; the gift
// walk decides (the line once ever, only with the NOTEBOOK, only in the watches its row names).
const u8 *DaemonsHolidayArrival(void)
{
    u8 stage, h, giver;

    if (!IsMapTypeOutdoors(gMapHeader.mapType) || IsMapNamePopupTaskActive() || Play_IsActive())
        return NULL;
    DaemonsTimeRefresh();
    for (stage = 0; stage < 2; stage++)
    {
        h = stage == 0 ? sHoliday : sHolidayCommon;
        if (h == HOLIDAY_NONE)
            continue;
        giver = sHolidays[h - 1].giver;
        if (giver == GIVER_HALFTONE && gSaveBlock1Ptr->location.mapGroup == MAP_GROUP(MAP_LAVENDER_TOWN)
         && gSaveBlock1Ptr->location.mapNum == MAP_NUM(MAP_LAVENDER_TOWN))
        {
            gSpecialVar_0x8006 = GIVER_HALFTONE;
            if (DaemonsHolidayGiftWaits())
                return EventScript_HolidayArrives_Halftone;
        }
        else if (giver == GIVER_DAWN_OUTDOORS)
        {
            gSpecialVar_0x8006 = GIVER_DAWN_OUTDOORS;
            if (DaemonsHolidayGiftWaits())
                return EventScript_HolidayArrives_Dawn;
        }
    }
    return NULL;
}

// MARIGOLD DAYS' visitor at the stones (DAEMON TOWER 2F): a special for the floor's transition.
bool8 DaemonsHolidayVisitorComes(void)
{
    return DaemonsHoliday() == HOLIDAY_MARIGOLD_DAYS || DaemonsHolidayCommon() == HOLIDAY_MARIGOLD_DAYS;
}

// THE FIREWORKS (ROLLOVER, SUMMER LIGHTS): outdoors, at night, under a clear sky, never in HALFTONE or BREATHVERSE,
// never while the player is held (a message, a menu, a script) or the instrument is out. A task keeps the sky: it is
// made when they may begin (once a second at most) and ends the moment they may not. PLAY draws them (src/play.c).
#define tPalSlot data[0]
#define tWait    data[1]

static bool8 FireworksAllowed(void)
{
    u8 weather;

    if (!(DaemonsHolidayFlags(DaemonsHoliday()) & HF_FIREWORKS) || DaemonsWatch() != WATCH_NIGHT)
        return FALSE;
    if (!IsMapTypeOutdoors(gMapHeader.mapType) || DaemonsIsHalftone() || DaemonsIsTimeless())
        return FALSE;
    weather = GetCurrentWeather();
    if (weather != WEATHER_SUNNY && weather != WEATHER_NONE)
        return FALSE;
    return !ArePlayerFieldControlsLocked() && !Play_IsActive() && gQuestLogState != QL_STATE_PLAYBACK;
}

static void Task_Fireworks(u8 taskId)
{
    s16 *data = gTasks[taskId].data;

    Play_FireworksShine(tPalSlot);
    if (gPaletteFade.active || --tWait > 0)
        return;
    tWait = 18 + Random() % 48;
    if (Play_FireworksRockets() < 4)
        Play_FireworksLaunch(tPalSlot, 24 + Random() % (DISPLAY_WIDTH - 48), 20 + Random() % 52, Random() % 12);
}

void DaemonsFireworks_Stop(void)
{
    u8 taskId = FindTaskIdByFunc(Task_Fireworks);

    if (taskId == TASK_NONE)
        return;
    Play_FireworksEnd();
    DestroyTask(taskId);
}

void DaemonsFireworks_Update(void)
{
    u8 taskId = FindTaskIdByFunc(Task_Fireworks), palSlot;

    if (taskId != TASK_NONE)
    {
        if (!FireworksAllowed())
            DaemonsFireworks_Stop();
        return;
    }
    if ((gMain.vblankCounter2 % 60) != 0 || !FireworksAllowed())
        return;
    palSlot = Play_FireworksBegin();
    if (palSlot == 0xFF)
        return;
    taskId = CreateTask(Task_Fireworks, 91);
    gTasks[taskId].tPalSlot = palSlot;
    gTasks[taskId].tWait = 1;
}

#undef tPalSlot
#undef tWait

// ------------------------------------------------------------------------------------------------------- the gifts

static u16 GiftFlag(u8 holiday)
{
    return FLAG_HOLIDAY_GIFT_COMMON_JAN + holiday - 1;
}

// A new year (real, or play time's): every holiday's item can be given again.
static void YearCheck(void)
{
    u32 i;

    DaemonsTimeRefresh();
    if (VarGet(VAR_HOLIDAY_YEAR) != sYearKey)
    {
        for (i = 0; i < HOLIDAY_GIFT_FLAG_COUNT; i++)
            FlagClear(FLAG_HOLIDAY_GIFT_COMMON_JAN + i);
        VarSet(VAR_HOLIDAY_YEAR, sYearKey);
    }
}

// The shown holiday first, then the COMMON one under it -- each only if this is its giver.
static u8 StageHoliday(u8 stage)
{
    u8 h = stage == 0 ? sHoliday : sHolidayCommon;

    if (h == HOLIDAY_NONE || sHolidays[h - 1].giver != sGiver)
        return HOLIDAY_NONE;
    return h;
}

void DaemonsHolidayGiftBegin(void)
{
    u8 stage, h;

    YearCheck();
    sGiver = gSpecialVar_0x8006;
    sGiftStage = 0;
    sGiftIdx = 0;
    sGiftOpen = 0;
    sGiftPending = 0;
    for (stage = 0; stage < 2; stage++)
    {
        h = StageHoliday(stage);
        if (h != HOLIDAY_NONE && !FlagGet(GiftFlag(h)))
            sGiftOpen |= 1 << stage;
    }
}

// DEN DAY: the GAIN item of the lead's first type. A lead that is ORACLE as EITHER type has none (ITEM_NONE), and its
// list reads on to THE YEAR's line (the user, 2026-10-18: no daemon is ORACLE first; OLD_UNOWN_B is ORACLE second).
static u16 LeadTypeGain(void)
{
    u32 i;
    u16 species;
    u8 type;

    for (i = 0; i < PARTY_SIZE; i++)
    {
        species = GetMonData(&gPlayerParty[i], MON_DATA_SPECIES);
        if (species == SPECIES_NONE)
            break;
        if (GetMonData(&gPlayerParty[i], MON_DATA_IS_EGG))
            continue;
        if (gSpeciesInfo[species].types[0] == TYPE_MYSTERY || gSpeciesInfo[species].types[1] == TYPE_MYSTERY)
            return ITEM_NONE;
        type = gSpeciesInfo[species].types[0];
        return type < NUMBER_OF_MON_TYPES ? sHolidayTypeGain[type] : ITEM_NONE;
    }
    return ITEM_NONE;
}

static bool8 GiftWhen(const struct HolidayGift *g)
{
    if (g->watches != 0 && !((g->watches >> DaemonsWatch()) & 1))
        return FALSE;
    if (g->weekdays != 0 && !((g->weekdays >> DaemonsWeekday()) & 1))
        return FALSE;
    if (g->skies != 0 && !((g->skies >> DaemonsSkyAt(gMapHeader.regionMapSectionId)) & 1))
        return FALSE;
    if ((g->flags & HG_STORM_ISLANDS) && !DaemonsStormOnIslandsToday())
        return FALSE;
    return TRUE;
}

// One gift at a time, until it returns 0: the script gives what it is handed and asks again, so a day that holds two
// (ROLLOVER's line and its STARDUST; the edition's and the COMMON one's) gives both.
static u16 GiftWalk(bool8 dry)
{
    const struct HolidayRow *row;
    const struct HolidayGift *g;
    u16 item;
    u8 h;

    for (; sGiftStage < 2; sGiftStage++, sGiftIdx = 0)
    {
        h = StageHoliday(sGiftStage);
        if (h == HOLIDAY_NONE)
            continue;
        row = &sHolidays[h - 1];
        while (sGiftIdx < row->giftCount)
        {
            g = &sHolidayGifts[row->giftFirst + sGiftIdx++];
            if (!GiftWhen(g))
                continue;
            item = g->item;
            if (g->flags & HG_LEAD_TYPE_GAIN)
            {
                item = LeadTypeGain();
                if (item == ITEM_NONE)
                    continue;
            }
            if (g->flags & HG_YEAR_DRAW)
                item = sHolidayPresents[((sYearKey & 0x7FFF) + HOLIDAY_EDITION) % ARRAY_COUNT(sHolidayPresents)];
            if (g->flags & HG_LINE)
            {
                // once ever, and only into a NOTEBOOK there is to write in
                if (FlagGet(sYearLineFlags[g->arg]) || !FlagGet(FLAG_GOT_NOTEBOOK))
                {
                    if (g->flags & HG_GO_ON)
                        continue;
                    break;
                }
                if (dry)
                    return 2;
                FlagSet(sYearLineFlags[g->arg]);
                StringExpandPlaceholders(gStringVar4, sYearArrivals[g->arg]);
                if (!(g->flags & HG_GO_ON))
                    sGiftIdx = row->giftCount;
                return 2;
            }
            if (!((sGiftOpen >> sGiftStage) & 1))   // this year's already given
            {
                if (g->flags & HG_GO_ON)
                    continue;
                break;
            }
            if (dry)
                return 1;
            gSpecialVar_0x8004 = item;
            gSpecialVar_0x8005 = g->count;
            sGiftPending = h;
            StringCopy(gStringVar1, row->name);
            if (sHolidaySay[sGiver] != NULL)
                StringExpandPlaceholders(gStringVar4, sHolidaySay[sGiver]);
            else
                gStringVar4[0] = EOS;
            if (!(g->flags & HG_GO_ON))
                sGiftIdx = row->giftCount;
            return 1;
        }
    }
    return 0;
}

u16 DaemonsHolidayGift(void)
{
    return GiftWalk(FALSE);
}

// Whether this giver has anything today, without giving it (the sand by the palm says only "a palm" otherwise).
bool8 DaemonsHolidayGiftWaits(void)
{
    u16 result;

    DaemonsHolidayGiftBegin();
    result = GiftWalk(TRUE);
    sGiftStage = 0;
    sGiftIdx = 0;
    return result != 0;
}

void DaemonsHolidayGiftTaken(void)
{
    if (sGiftPending != HOLIDAY_NONE)
        FlagSet(GiftFlag(sGiftPending));
}

// ------------------------------------------------------------------------------------------------------------ DEBUG

#if DAEMONS_DEBUG
EWRAM_DATA u8 gDaemonsDateOverride = 0;

static const u8 sDbgDateAuto[] = _("AUTO DATE");
static const u8 sDbgDateQuiet[] = _("A QUIET DAY");
static const u8 sDbgDateFirst[] = _("1ST ");
static const u8 sDbgDateOct3[] = _("3RD OCT");   // T-460
static const u8 sDbgMonth_Jan[] = _("JAN");
static const u8 sDbgMonth_Feb[] = _("FEB");
static const u8 sDbgMonth_Mar[] = _("MAR");
static const u8 sDbgMonth_Apr[] = _("APR");
static const u8 sDbgMonth_May[] = _("MAY");
static const u8 sDbgMonth_Jun[] = _("JUN");
static const u8 sDbgMonth_Jul[] = _("JUL");
static const u8 sDbgMonth_Aug[] = _("AUG");
static const u8 sDbgMonth_Sep[] = _("SEP");
static const u8 sDbgMonth_Oct[] = _("OCT");
static const u8 sDbgMonth_Nov[] = _("NOV");
static const u8 sDbgMonth_Dec[] = _("DEC");
static const u8 *const sDbgMonths[12] = {
    sDbgMonth_Jan, sDbgMonth_Feb, sDbgMonth_Mar, sDbgMonth_Apr, sDbgMonth_May, sDbgMonth_Jun,
    sDbgMonth_Jul, sDbgMonth_Aug, sDbgMonth_Sep, sDbgMonth_Oct, sDbgMonth_Nov, sDbgMonth_Dec,
};

// The year a holiday's own date is taken from: where its known double falls, so the two-on-one-day rule is exercised
// (DEN DAY on the solstice and PAIRING DAY in PARADE DAYS are both in 2026).
static u16 DebugYear(u8 holiday)
{
#ifdef FIRERED
    if (holiday == HOLIDAY_EGG_HUNT)
        return 2029;            // EGG HUNT on FOOLS' DAY
#else
    if (holiday == HOLIDAY_VIOLET_BLOOM)
        return 2024;            // VIOLET BLOOM in MARIGOLD DAYS
#endif
    return 2026;
}

bool8 DaemonsHolidays_DebugDate(u16 *year, u8 *month, u8 *day)
{
    u8 o = gDaemonsDateOverride;

    if (o == 0 || o >= DAEMONS_DEBUG_DATE_COUNT)
        return FALSE;
    if (o <= HOLIDAY_COUNT)
    {
        DateOf(HolidayStart(&sHolidays[o - 1], DebugYear(o)), year, month, day);
    }
    else if (o < DAEMONS_DEBUG_DATE_QUIET)
    {
        *year = 2026;
        *month = o - DAEMONS_DEBUG_DATE_FIRSTS + 1;
        *day = 1;
    }
    else if (o == DAEMONS_DEBUG_DATE_OCT3)
    {
        *year = 2026;     // T-460: the flowers on CRYSTAL's desk
        *month = 10;
        *day = 3;
    }
    else
    {
        *year = 2026;     // a Tuesday in March: no holiday in either edition
        *month = 3;
        *day = 10;
    }
    return TRUE;
}

void DaemonsHolidays_DebugDateName(u8 *dest)
{
    u8 o = gDaemonsDateOverride;

    if (o == 0 || o >= DAEMONS_DEBUG_DATE_COUNT)
        StringCopy(dest, sDbgDateAuto);
    else if (o <= HOLIDAY_COUNT)
        StringCopy(dest, sHolidays[o - 1].name);
    else if (o < DAEMONS_DEBUG_DATE_QUIET)
        StringCopy(StringCopy(dest, sDbgDateFirst), sDbgMonths[o - DAEMONS_DEBUG_DATE_FIRSTS]);
    else if (o == DAEMONS_DEBUG_DATE_OCT3)
        StringCopy(dest, sDbgDateOct3);
    else
        StringCopy(dest, sDbgDateQuiet);
}

void DaemonsHolidays_DebugGiftsAgain(void)
{
    u32 i;

    for (i = 0; i < HOLIDAY_GIFT_FLAG_COUNT; i++)
        FlagClear(FLAG_HOLIDAY_GIFT_COMMON_JAN + i);
    VarSet(VAR_HOLIDAY_YEAR, 0);
}

bool8 DaemonsHolidays_DebugLineHeld(u8 line)
{
    return line < HOLIDAY_YEAR_LINE_COUNT && FlagGet(sYearLineFlags[line]);
}

void DaemonsHolidays_DebugSetLine(u8 line, bool8 on)
{
    if (line >= HOLIDAY_YEAR_LINE_COUNT)
        return;
    if (on)
        FlagSet(sYearLineFlags[line]);
    else
        FlagClear(sYearLineFlags[line]);
}
#endif
