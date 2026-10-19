#ifndef GUARD_DAEMONS_HOUSE_H
#define GUARD_DAEMONS_HOUSE_H

#include "constants/daemons_house.h"

// T-427: the house's take (gSaveBlock2Ptr->houseTake) -- see src/daemons_house.c
void DaemonsHouse_Add(s32 money);
void DaemonsHouse_AddPull(u16 bet, u16 payout);
void DaemonsHouse_ReadTake(void);
void DaemonsHouse_PlacePlants(void);
void DaemonsHouse_PlantHere(void);

#endif // GUARD_DAEMONS_HOUSE_H
