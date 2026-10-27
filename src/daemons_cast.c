#include "global.h"
#include "characters.h"
#include "event_data.h"
#include "event_object_movement.h"
#include "pokemon.h"
#include "string_util.h"
#include "battle_main.h"
#include "item.h"
#include "teachy_tv.h"
#include "daemons_time.h"
#include "daemons_follower.h"
#include "constants/items.h"
#include "constants/event_objects.h"
#include "constants/species.h"

// T-439 (the user, 2026-10-22; DAEMONS docs/private/plan-cast-2026-10-22.md): the specials behind the eleven unused
// sprites' small jobs. Every word they lead to is in the maps' text.inc, and every one of those is DRAFT.

// The season, for a script: 0 winter, 1 spring, 2 summer, 3 autumn (daemons_time.h's SEASON_*). The GREBE swims in
// spring only; the PTARMIGAN is white in winter and says so.
void DaemonsCast_Season(void)
{
    gSpecialVar_Result = DaemonsSeason();
}

// HALFTONE's KINGFISHER photographs your FOLLOWER: its species into VAR_0x8004 (showmonpic reads the var) and TRUE,
// or FALSE when nothing follows (an egg, HALTED, AWAY, or the option off).
void DaemonsCast_FollowerSpecies(void)
{
    s8 slot = Follower_LeadSlot();

    gSpecialVar_Result = FALSE;
    if (slot < 0)
        return;
    gSpecialVar_0x8004 = GetMonData(&gPlayerParty[slot], MON_DATA_SPECIES);
    gSpecialVar_Result = gSpecialVar_0x8004 != SPECIES_NONE;
}

// CALLOW's MYNAH has seen every STREAM twice. VAR_RESULT: 0 you hold no STREAM, 1 a lecture you hold and have not
// watched (its name in STR_VAR_1: a MARK's word without "The " and " MARK", an understanding's as it is), 2 none.
static const u8 sMarkPrefix[] = _("The ");
static const u8 sMarkSuffix[] = _(" MARK");

void DaemonsCast_MynahTip(void)
{
    s8 show;
    const u8 *title;
    u16 n, p = StringLength(sMarkPrefix), q = StringLength(sMarkSuffix);

    if (!CheckBagHasItem(ITEM_TEACHY_TV, 1))
    {
        gSpecialVar_Result = 0;
        return;
    }
    show = TeachyTvFirstUnwatchedLecture();
    if (show < 0)
    {
        gSpecialVar_Result = 2;
        return;
    }
    title = TeachyTvLectureTitle(show);
    n = StringLength(title);
    if (n > p + q && StringCompareN(title, sMarkPrefix, p) == 0 && StringCompare(title + n - q, sMarkSuffix) == 0)
    {
        StringCopyN(gStringVar1, title + p, n - p - q);
        gStringVar1[n - p - q] = EOS;
    }
    else
    {
        StringCopy(gStringVar1, title);
    }
    gSpecialVar_Result = 1;
}

// RESORT SUBLIME's young ERMINE wants, once a season, to see a daemon of the season's type: FROZEN in winter, GROWTH
// in spring, FLOW in summer, ENTROPY in autumn. The type's name into STR_VAR_1, and VAR_RESULT: 0 he has paid this
// season already, 1 a daemon of it is in the party (not an egg), 2 none is. DaemonsCast_ErminePaid stamps the season.
static const u8 sErmineTypes[SEASON_COUNT] = {
    [SEASON_WINTER] = TYPE_ICE,
    [SEASON_SPRING] = TYPE_GRASS,
    [SEASON_SUMMER] = TYPE_WATER,
    [SEASON_AUTUMN] = TYPE_FIRE,
};

void DaemonsCast_ErmineAsks(void)
{
    u8 season = DaemonsSeason() % SEASON_COUNT, type = sErmineTypes[season];
    s32 i;

    StringCopy(gStringVar1, gTypeNames[type]);
    if (VarGet(VAR_ERMINE_SEASON) == season + 1)
    {
        gSpecialVar_Result = 0;
        return;
    }
    gSpecialVar_Result = 2;
    for (i = 0; i < PARTY_SIZE; i++)
    {
        u16 species = GetMonData(&gPlayerParty[i], MON_DATA_SPECIES_OR_EGG);

        if (species == SPECIES_NONE || species == SPECIES_EGG)
            continue;
        if (gSpeciesInfo[species].types[0] == type || gSpeciesInfo[species].types[1] == type)
        {
            gSpecialVar_Result = 1;
            return;
        }
    }
}

void DaemonsCast_ErminePaid(void)
{
    VarSet(VAR_ERMINE_SEASON, DaemonsSeason() % SEASON_COUNT + 1);
}

// THE ANNEX: PROTEUS shows each edition its own shape -- CONTENT's all edges, CONTEXT's a shell -- and talking to it
// shifts it to the other, until on the third touch it holds still in its own shape and the battle starts -- in that
// shape (T-440, the user 2026-10-23: a wild PROTEUS battles in its edition's own shape, matching the overworld).
// DaemonsCast_ProteusShape sets VAR_OBJ_GFX_ID_0 to the edition's shape (OnTransition). DaemonsCast_ProteusShift takes
// the touch in VAR_0x8004 (1, 2, 3) and the object's local id in VAR_0x8005, redraws it, and returns VAR_RESULT:
// 0 it turned all edges, 1 it curled into a shell, 2 it holds still.
#if GAME_VERSION == VERSION_FIRE_RED
#define PROTEUS_OWN   OBJ_EVENT_GFX_DEOXYS_A
#define PROTEUS_OTHER OBJ_EVENT_GFX_DEOXYS_D
#else
#define PROTEUS_OWN   OBJ_EVENT_GFX_DEOXYS_D
#define PROTEUS_OTHER OBJ_EVENT_GFX_DEOXYS_A
#endif

void DaemonsCast_ProteusShape(void)
{
    VarSet(VAR_OBJ_GFX_ID_0, PROTEUS_OWN);
}

void DaemonsCast_ProteusShift(void)
{
    u16 gfx;
    u8 objectEventId;

    if (gSpecialVar_0x8004 >= 3)
        gfx = PROTEUS_OWN;          // T-440: it holds still in its own shape, the one it battles in
    else if (gSpecialVar_0x8004 & 1)
        gfx = PROTEUS_OTHER;
    else
        gfx = PROTEUS_OWN;
    if (!TryGetObjectEventIdByLocalIdAndMap(gSpecialVar_0x8005, gSaveBlock1Ptr->location.mapNum,
                                            gSaveBlock1Ptr->location.mapGroup, &objectEventId))
    {
        // a new sheet is only copied in when an animation starts: turn it where it faces, or the old frame's tiles stay
        ObjectEventSetGraphicsId(&gObjectEvents[objectEventId], gfx);
        ObjectEventTurn(&gObjectEvents[objectEventId], gObjectEvents[objectEventId].facingDirection);
    }
    if (gSpecialVar_0x8004 >= 3)
        gSpecialVar_Result = 2;
    else
        gSpecialVar_Result = gfx == OBJ_EVENT_GFX_DEOXYS_D;
}
