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

extern u8 gDaemonsWatchOverride;   // DEBUG: 0 follows the clock, 1-4 is a watch + 1
extern u8 gDaemonsWeekdayOverride; // DEBUG (T-273): 0 follows the clock, 1-7 is Sunday..Saturday + 1

#define WEEKDAY_COUNT 7

u8 DaemonsWatch(void);
u8 DaemonsWeekday(void);           // 0-6, Sunday first
bool8 DaemonsWatchIsFromClock(void);
void DaemonsTintForWatch(u16 *palette, u16 count);

#endif // GUARD_DAEMONS_TIME_H
