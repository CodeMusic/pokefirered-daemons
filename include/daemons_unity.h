#ifndef GUARD_DAEMONS_UNITY_H
#define GUARD_DAEMONS_UNITY_H

//  T-519 item 3: THE UNITY BOOK -- the two halves at DAEMON TOWER, and mended (src/daemons_unity.c; the brief is private).

enum { MENDING_NOT_HERE, MENDING_ONE_HALF, MENDING_BOTH_HALVES };

bool8 Unity_Mended(void);
bool8 Mending_TornHere(void);               // book_reader.c: a half opened here ends its last page torn at the spine
u8 Mending_AtTower(void);                   // specialvar: MENDING_* -- in DAEMON TOWER, not yet mended, halves held
void Mending_Deepen(void);                  // special, then waitstate: VAR_0x8004 0 the night half, 1 the dawn half
bool8 Mending_ShouldStart(void);            // field_control_avatar.c's step hook: both halves held, a step in the tower
void Mending_Mend(void);                    // special, then waitstate: the halves become THE UNITY BOOK, the burst
#if DAEMONS_DEBUG
void Mending_DebugSet(bool8 on);            // DEBUG -> GAME EVENTS -> MENDED
#endif

#endif // GUARD_DAEMONS_UNITY_H
