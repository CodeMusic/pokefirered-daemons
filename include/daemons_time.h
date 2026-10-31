#ifndef GUARD_DAEMONS_TIME_H
#define GUARD_DAEMONS_TIME_H

// T-268: the four watches (vision.md 9.21), from the cartridge's clock when it has one and from play time when not.
#define WATCH_DAY    0
#define WATCH_DUSK   1
#define WATCH_NIGHT  2
#define WATCH_DAWN   3
#define WATCH_COUNT  4

// The field tint that means "the watch's light" (after quest_log.h's QL_TINT_* 0-3).
#define DAEMONS_TINT_WATCH 4

extern u8 gDaemonsWatchOverride;   // DEBUG: 0 follows the clock, 1-4 is a watch + 1, 5-7 the witching hour
#define WATCH_OVERRIDE_WITCHING 5  // T-480: DEBUG's "3 AM" -- night, and the witching hour (3:00)
#define WATCH_OVERRIDE_WITCHING_15 6   // T-486: DEBUG's "3:15 AM", the witching hour at a quarter past
#define WATCH_OVERRIDE_WITCHING_30 7   // T-486: DEBUG's "3:30 AM", when OSRIC HATTER's clocks are right
extern u8 gDaemonsWeekdayOverride; // DEBUG (T-273): 0 follows the clock, 1-7 is Sunday..Saturday + 1

#define WEEKDAY_COUNT 7

// T-359: the four seasons (vision.md 9.21), the order tools/seasons.py and the companion use.
#define SEASON_WINTER  0
#define SEASON_SPRING  1
#define SEASON_SUMMER  2
#define SEASON_AUTUMN  3
#define SEASON_COUNT   4

extern u8 gDaemonsSeasonOverride;  // DEBUG: 0 follows the clock, 1-4 is a season + 1
u8 DaemonsSeason(void);

u8 DaemonsWatch(void);             // the watch the world is seen in (THE CONTRAST may have turned it, T-483)
u8 DaemonsTrueWatch(void);         // T-483: the clock's own watch, before any spell
bool8 DaemonsIsWitchingHour(void);   // T-480
s8 DaemonsWitchingMinute(void);      // T-486: 0-59 inside the witching hour (3:00-3:59), -1 outside it
u8 DaemonsWeekday(void);           // 0-6, Sunday first
bool8 DaemonsWatchIsFromClock(void);
void DaemonsDayAndMinute(u16 *day, u16 *minute);   // T-415
void DaemonsTintForWatch(u16 *palette, u16 count);
void DaemonsTintBattleForWatch(u16 *palette, u16 count, u8 watch, u8 quarters);   // T-481 A: the battle's light
// T-416: a fresh read for the holidays' lookups, a forced one for DEBUG's DATE, and the date as a day number.
void DaemonsTimeRefresh(void);
void DaemonsTimeInvalidate(void);
u16 DaemonsDayNumber(u16 year, u8 month, u8 day);
u16 DaemonsFeelingsNow(bool8 *fromClock);   // T-431: 10-minute ticks, from the clock or play time
bool8 DaemonsPulseClock(u32 *second, u32 *readAt);   // T-483: the clock's second of the day, and the frame it was read

#endif // GUARD_DAEMONS_TIME_H
