#ifndef GUARD_DAEMONS_BATTLE_BG_H
#define GUARD_DAEMONS_BATTLE_BG_H

// T-481 A: THE BATTLE BACKGROUNDS THAT KEEP THE CALENDAR (src/daemons_battle_bg.c).

// After the terrain's tiles and tilemap are in VRAM: the horizon's 1px line moved to palette row 3's colour 12.
void DaemonsBattleBgHorizon(u16 terrain);
// After the terrain's palette loads and BEFORE the weather's wash (DaemonsTintBattleTerrain): the season, the holiday,
// the weekday and the hour, in that order, into both palette buffers.
void DaemonsBattleBgCalendar(u16 terrain);

#endif // GUARD_DAEMONS_BATTLE_BG_H
