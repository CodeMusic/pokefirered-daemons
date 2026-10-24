#ifndef GUARD_DAEMONS_CARRIED_H
#define GUARD_DAEMONS_CARRIED_H

//  T-448 / T-450: a daemon the companion grew or delivered is finished when the save loads (src/daemons_carried.c).
void DaemonsCarried_OnLoad(void);        // ContinueLoadedSave: walk the party and every box for the marker
void DaemonsCarried_NewGame(void);       // new_game.c: nothing to carry across
bool8 Daemons_TryCarriedArrives(void);   // the first step after: the one line, in gStringVar4
bool8 Daemons_TryCarriedMoves(void);     // T-455: a party daemon owed its new species' routines, in VAR_0x8004

#endif // GUARD_DAEMONS_CARRIED_H
