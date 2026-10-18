#ifndef GUARD_DAEMONS_HOLIDAYS_H
#define GUARD_DAEMONS_HOLIDAYS_H

#include "constants/holidays.h"

// T-416 HOLIDAYS (the user, 2026-10-17/18): the real calendar's holidays, three a month, and a note and a colour for
// each month -- see src/daemons_holidays.c. The table is DAEMONS tools/genholidays.py's (src/data/daemons_holidays.h).

// daemons_time.c's ReadClock hands over the date it read (or the debug date), once a second at most.
u16 DaemonsHolidays_FromDate(u16 year, u8 month, u8 day, u8 hour, u16 dayNumber);   // returns the shown one's HF_*
void DaemonsHolidays_FromPlay(u32 playMinutes);
u8 DaemonsHolidays_WatchFromHour(u8 hour, u16 flags);   // the watch, with LONGEST DAY / NIGHT's hours

u8 DaemonsHoliday(void);             // the one that shows (the edition's first): HOLIDAY_*, or HOLIDAY_NONE
u8 DaemonsHolidayCommon(void);       // the COMMON one, when the edition's holds the same day
u8 DaemonsHolidayForBanner(void);    // DaemonsHoliday(), if the banner names it in this watch
u16 DaemonsHolidayFlags(u8 holiday); // HF_*
const u8 *DaemonsHolidayName(u8 holiday);
u8 DaemonsHolidayMonth(u8 holiday);  // 0-11: the month whose note and colour it wears
u8 DaemonsMonth(void);               // 0-11
bool8 DaemonsIsFirstOfMonth(void);
u16 DaemonsMonthColour(u8 month);    // 0-11
const u8 *DaemonsMonthNote(u8 month);
void DaemonsPlayMonthNote(u8 month, bool8 soft);

// P3: a holiday's daemons -- the slot's species, or the one a holiday gives that slot of this map now (wild_encounter.c)
u16 DaemonsHolidayShiftSpecies(u16 species, u8 area, u8 slot);   // area: HOLIDAY_AREA_*

// P4: the overlays. The light a holiday puts over the watch's, outdoors (daemons_time.c's DaemonsTintForWatch); the
// caller has already ruled out indoors, HALFTONE's grey and BREATHVERSE.
enum {
    HOLIDAY_LIFT_NONE,
    HOLIDAY_LIFT_LANTERN,   // LANTERN NIGHTS, dusk and night
    HOLIDAY_LIFT_FROST,     // FIRST FROST, dawn
    HOLIDAY_LIFT_SNOW,      // the solstice nights and FIR LIGHTS, while it snows
    HOLIDAY_LIFT_BRIGHT,    // BEACH DAY, by day, on the islands
    HOLIDAY_LIFT_COUNT
};
u8 DaemonsHolidayLift(void);
u16 DaemonsHolidayBloomColour(u8 shade);   // VIOLET BLOOM's petals, 0 light .. 2 dark
bool8 DaemonsHolidayFirGlow(void);         // WINTER FIR, FIR LIGHTS
void DaemonsHolidayGlow(u16 *unfaded, u16 *faded);   // the found fir's palette, lifted a step
// The k-th sky row of today's holidays that holds this watch (daemons_weather.c covers the map with it); FALSE past the last.
bool8 DaemonsHolidaySkyRow(u8 k, u8 *sky, u8 *mapsec, u8 *radius, bool8 *onlyIfClear);
// Once a second in the field (overworld.c): a line that arrives on its own -- HALFTONE's at dusk, DAWN VIGIL's at dawn.
const u8 *DaemonsHolidayArrival(void);
bool8 DaemonsHolidayVisitorComes(void);   // special: MARIGOLD DAYS' visitor in DAEMON TOWER
// Every frame in the field (overworld.c): ROLLOVER's and SUMMER LIGHTS' fireworks, on a clear night outdoors.
void DaemonsFireworks_Update(void);
void DaemonsFireworks_Stop(void);    // at once, and the art given back (PLAY takes it out)

// P2: the gifts (specials; data/scripts/holidays.inc)
void DaemonsHolidayGiftBegin(void);  // VAR_0x8006 the giver (GIVER_*); clears the year's gift flags on a new year
u16 DaemonsHolidayGift(void);        // specialvar: 0 nothing, 1 an item (VAR_0x8004/5, gStringVar4 the giver's line), 2 a line (gStringVar4)
void DaemonsHolidayGiftTaken(void);  // after giveitem succeeded
bool8 DaemonsHolidayGiftWaits(void); // specialvar: this giver (VAR_0x8006) has something today

#if DAEMONS_DEBUG
// DEBUG -> ENCOUNTER -> DATE: 0 the clock (or play time); 1-24 a holiday's own date; then the 1st of each month; then
// an ordinary day. It overrides the DATE, not the answer -- the lookup still runs.
#define DAEMONS_DEBUG_DATE_FIRSTS  (HOLIDAY_COUNT + 1)
#define DAEMONS_DEBUG_DATE_QUIET   (DAEMONS_DEBUG_DATE_FIRSTS + 12)
#define DAEMONS_DEBUG_DATE_COUNT   (DAEMONS_DEBUG_DATE_QUIET + 1)
extern u8 gDaemonsDateOverride;
bool8 DaemonsHolidays_DebugDate(u16 *year, u8 *month, u8 *day);
void DaemonsHolidays_DebugDateName(u8 *dest);
void DaemonsHolidays_DebugGiftsAgain(void);
bool8 DaemonsHolidays_DebugLineHeld(u8 line);
void DaemonsHolidays_DebugSetLine(u8 line, bool8 on);
#endif

#endif // GUARD_DAEMONS_HOLIDAYS_H
