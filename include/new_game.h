#ifndef GUARD_NEW_GAME_H
#define GUARD_NEW_GAME_H

#include "global.h"

extern bool8 gDifferentSaveFile;

void SetTrainerId(u32 trainerId, u8 *dst);
void CopyTrainerId(u8 *dst, u8 *src);
void NewGameInitData(void);
void ResetMenuAndMonGlobals(void);
void Sav2_ClearSetDefault(void);

#if DAEMONS_DEBUG
//  T-394: the testing kit, in the pieces the DEBUG menu's GAME EVENTS hands out (daemons_debug_events.c).
void DaemonsDebug_GiveRoster(void);
void DaemonsDebug_FillIndexAndPort(void);
void DaemonsDebug_GiveStock(void);
void DaemonsDebug_FinishOpening(void);
void DaemonsDebug_OpenBrazenGates(void);
void DaemonsDebug_EveryGotoPoint(bool8 on);
void DaemonsDebug_GrantTestKit(void);
#endif

#endif // GUARD_NEW_GAME_H
