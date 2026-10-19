#include "global.h"
#include "event_data.h"
#include "fieldmap.h"
#include "field_player_avatar.h"
#include "string_util.h"
#include "daemons_house.h"
#include "constants/maps.h"

// T-427 WHAT THE HOUSE KEEPS GOES TO CORPUS (the user, 2026-10-18; confirmed by phone 2026-10-19). One number, the
// house's take, in money: what the GAME CORNER's reels kept -- bets less payouts, at the price the counter sells coins
// for -- and, at SYNC, what the companion's CASINO kept (its C-167). It lives in SaveBlock2 (gSaveBlock2Ptr->houseTake),
// plain, never XORed; negative while the player is ahead. The game only ever ADDS to it.
//
// It shows, never explained: a ledger in the CORPUS HIDEOUT, cheerful and exact; plastic plants (T-381's) that multiply
// down there with it, the finest in SCORN's office; and, after CORPUS falls, a line in VERDIGRIS saying it came back.

void DaemonsHouse_Add(s32 money)
{
    s32 take = gSaveBlock2Ptr->houseTake;

    if (money > 0 && take > 0x7FFFFFFF - money)
        take = 0x7FFFFFFF;
    else if (money < 0 && take < (s32)0x80000000 - money)
        take = (s32)0x80000000;
    else
        take += money;
    gSaveBlock2Ptr->houseTake = take;
}

// One pull at the reels: the coins bet, the coins paid back -- the difference at the counter's price
void DaemonsHouse_AddPull(u16 bet, u16 payout)
{
    DaemonsHouse_Add(((s32)bet - (s32)payout) * HOUSE_COIN_PRICE);
}

// special: gStringVar1 = the take's size; VAR_RESULT = HOUSE_TAKE_NONE (zero), HOUSE_TAKE_KEPT, HOUSE_TAKE_OWED
void DaemonsHouse_ReadTake(void)
{
    s32 take = gSaveBlock2Ptr->houseTake;
    s32 size = take < 0 ? -(take + 1) : take;   // -INT_MIN would not fit an s32

    if (take < 0 && size < 0x7FFFFFFF)
        size++;
    ConvertIntToDecimalStringN(gStringVar1, size, STR_CONV_MODE_LEFT_ALIGN, 10);
    gSpecialVar_Result = take > 0 ? HOUSE_TAKE_KEPT : take < 0 ? HOUSE_TAKE_OWED : HOUSE_TAKE_NONE;
}

// ------------------------------------------------------------------------------------------------------- the plants
// One more plastic plant every HOUSE_PLANT_STEP the house keeps, in this order: the first (and the fifth) in SCORN's
// office on B4F, the rest about the hideout's corners where nobody needs to walk. Each is two tiles, gTileset_SilphCo's
// floor plant (0x300 above 0x308) or the one against a left wall (0x301 above 0x309), as the hideout's own are drawn.
// DRAFT: the step and the places.

struct HousePlant
{
    u16 map;          // MAP_*
    u8 x, y;          // the plant's top tile
    bool8 wall;       // against a left wall
    bool8 scorns;     // one of SCORN's
};

static const struct HousePlant sHousePlants[] =
{
    {MAP_ROCKET_HIDEOUT_B4F, 22,  8, FALSE, TRUE},
    {MAP_ROCKET_HIDEOUT_B1F, 26, 13, FALSE, FALSE},
    {MAP_ROCKET_HIDEOUT_B1F,  1,  8, TRUE,  FALSE},
    {MAP_ROCKET_HIDEOUT_B1F, 26,  8, FALSE, FALSE},
    {MAP_ROCKET_HIDEOUT_B4F, 16,  8, TRUE,  TRUE},
    {MAP_ROCKET_HIDEOUT_B4F,  1,  6, TRUE,  FALSE},
    {MAP_ROCKET_HIDEOUT_B1F,  1, 32, TRUE,  FALSE},
    {MAP_ROCKET_HIDEOUT_B1F,  1, 14, TRUE,  FALSE},
};

static bool8 PlantShows(u32 i)
{
    return gSaveBlock2Ptr->houseTake >= (s32)(HOUSE_PLANT_STEP * (i + 1));
}

static bool8 PlantOnThisMap(u32 i)
{
    return gSaveBlock1Ptr->location.mapGroup == MAP_GROUP(sHousePlants[i].map)
        && gSaveBlock1Ptr->location.mapNum == MAP_NUM(sHousePlants[i].map);
}

// special, on the hideout's load: the plants the take has paid for
void DaemonsHouse_PlacePlants(void)
{
    u32 i;

    for (i = 0; i < ARRAY_COUNT(sHousePlants); i++)
    {
        const struct HousePlant *p = &sHousePlants[i];

        if (!PlantOnThisMap(i) || !PlantShows(i))
            continue;
        MapGridSetMetatileIdAt(p->x + MAP_OFFSET, p->y + MAP_OFFSET, p->wall ? 0x301 : 0x300);
        MapGridSetMetatileIdAt(p->x + MAP_OFFSET, p->y + 1 + MAP_OFFSET, (p->wall ? 0x309 : 0x308) | MAPGRID_COLLISION_MASK);
    }
}

// special, from a plant's sign: VAR_RESULT = HOUSE_PLANT_NONE when the plant there has not come yet, else which
void DaemonsHouse_PlantHere(void)
{
    s16 x, y;
    u32 i;

    GetXYCoordsOneStepInFrontOfPlayer(&x, &y);
    x -= MAP_OFFSET;
    y -= MAP_OFFSET;
    gSpecialVar_Result = HOUSE_PLANT_NONE;
    for (i = 0; i < ARRAY_COUNT(sHousePlants); i++)
    {
        const struct HousePlant *p = &sHousePlants[i];

        if (PlantOnThisMap(i) && PlantShows(i) && x == p->x && (y == p->y || y == p->y + 1))
        {
            gSpecialVar_Result = p->scorns ? HOUSE_PLANT_SCORNS : HOUSE_PLANT_PLASTIC;
            return;
        }
    }
}
