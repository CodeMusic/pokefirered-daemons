#ifndef GUARD_CONSTANTS_DAEMONS_HOUSE_H
#define GUARD_CONSTANTS_DAEMONS_HOUSE_H

// T-427: what the house keeps (src/daemons_house.c). Shared by the C and the scripts.

// The counter's price for a coin, in money: CeladonCity_GameCorner sells 50 coins for 1000 (and 500 for 10000)
#define HOUSE_COIN_PRICE   20

// One more plastic plant in the CORPUS HIDEOUT for every so much the house has kept (DRAFT)
#define HOUSE_PLANT_STEP   1000

// DaemonsHouse_ReadTake's VAR_RESULT
#define HOUSE_TAKE_NONE    0
#define HOUSE_TAKE_KEPT    1   // the house is ahead
#define HOUSE_TAKE_OWED    2   // the player is

// DaemonsHouse_PlantHere's VAR_RESULT
#define HOUSE_PLANT_NONE    0
#define HOUSE_PLANT_PLASTIC 1
#define HOUSE_PLANT_SCORNS  2

#endif // GUARD_CONSTANTS_DAEMONS_HOUSE_H
