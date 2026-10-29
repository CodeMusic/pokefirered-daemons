#include "global.h"
#include "battle.h"
#include "battle_anim.h"
#include "battle_main.h"
#include "event_data.h"
#include "item.h"
#include "palette.h"
#include "pokemon.h"
#include "util.h"
#include "main.h"
#include "daemons_three.h"
#include "daemons_time.h"
#include "daemons_witching.h"
#include "daemons_hermetic.h"   // T-483
#include "constants/battle.h"
#include "constants/flags.h"
#include "constants/hold_effects.h"
#include "constants/moves.h"
#include "constants/pokemon.h"

// T-480: THE WITCHING HOUR, the game's half (the user, 2026-10-29, by multiple choice; the companion's half is C-236).
//
// WHEN. 3:00 to 4:00 AM on the cartridge's own clock (DaemonsIsWitchingHour, src/daemons_time.c). Without a clock
// there is no witching hour: play time's night is a quarter of every hour, and the hour is the clock's. DEBUG's WATCH
// forces it as "3 AM".
//
// THE LOOK, to anyone awake then, book or no book. The party wears a thin shimmer of THE SUBTLE LAYER's purple -- the
// book's hue, lit (tools/gensubtle.py C_MARGIN, RGB(29, 17, 26)) -- a few sixteenths of it, flickering a little:
//   - the follower in the field (daemons_follower.c): its palette reloaded in the field's light and weather, then
//     blended in place every fourth frame; and it casts no reflection while the hour lasts -- "no shadow";
//   - the summary screen's picture and the party menu's icons: blended from the unfaded palette each frame;
//   - in battle, the player's daemons on the field (Witching_BattleShimmer), whenever no animation, fade or send-out
//     owns their palette, and put back the moment the hour ends.
// It costs no static RAM outside the follower's one byte (IWRAM): every weight is worked out from the frame counter.
//
// THE ROUTINE, only once THE SUBTLE LAYER is held (FLAG_GOT_SUBTLE_LAYER). Each daemon knows one more routine during
// the hour, named for its FIRST type (MOVE_WITCHING_FIRST + type, include/constants/moves.h): a plain strike of the
// type at 90, accuracy 100, five uses a battle. It is NEVER WRITTEN INTO A MOVESET. The battle menu has a second
// page -- DOWN from the bottom row, or R -- that shows it alone; choosing it answers with WITCHING_MENU_SLOT (4),
// which battle_main.c turns into the routine for that turn while chosenMovePositions stays on a real slot. So the
// moveset, its PP and the save are never touched: the hour's uses are a nibble per party slot in gBattleStruct (heap,
// gone with the battle). Not in link, multi, Safari, the old man's or the POKeDUDE's battles (as THE THREE).

#define NOT_CARRIED (BATTLE_TYPE_LINK | BATTLE_TYPE_MULTI | BATTLE_TYPE_POKEDUDE | BATTLE_TYPE_OLD_MAN_TUTORIAL \
                   | BATTLE_TYPE_SAFARI)

#define SHIMMER_COLOR RGB(29, 17, 26)   // gensubtle.py C_MARGIN: CONTEXT's hue, lit

// Sixteenths of the purple, a step every four frames: faint, and now and then a flicker up or down.
static const u8 sShimmerWeights[16] = { 3, 4, 3, 3, 5, 3, 4, 2, 3, 4, 6, 3, 3, 4, 3, 1 };

bool8 Witching_IsHour(void)
{
    return DaemonsIsWitchingHour();
}

u16 Witching_MoveForSpecies(u16 species)
{
    u8 type;

    if (species == SPECIES_NONE || species >= NUM_SPECIES)
        return MOVE_NONE;
    type = gSpeciesInfo[species].types[0];
    if (type >= NUMBER_OF_MON_TYPES)
        return MOVE_NONE;
    return MOVE_WITCHING_FIRST + type;
}

u8 Witching_ShimmerWeight(void)
{
    return sShimmerWeights[(gMain.vblankCounter2 >> 2) & 15];
}

// The faded colours, blended toward the purple where they stand -- for the field, whose faded palette carries the
// weather's light on top of the unfaded one. The caller reloads the palette first, so nothing compounds.
void Witching_ShimmerFadedInPlace(u16 palOffset, u8 weight)
{
    s32 i;
    s32 tr = GET_R(SHIMMER_COLOR), tg = GET_G(SHIMMER_COLOR), tb = GET_B(SHIMMER_COLOR);

    for (i = 1; i < 16; i++)   // 0 is the transparent colour
    {
        u16 c = gPlttBufferFaded[palOffset + i];
        s32 r = GET_R(c), g = GET_G(c), b = GET_B(c);

        r += ((tr - r) * weight) >> 4;
        g += ((tg - g) * weight) >> 4;
        b += ((tb - b) * weight) >> 4;
        gPlttBufferFaded[palOffset + i] = RGB(r, g, b);
    }
}

// From the unfaded palette, as BlendPalette does: for screens whose faded palette is only ever the unfaded one.
void Witching_ShimmerFromUnfaded(u16 palOffset)
{
    BlendPalette(palOffset, 16, Witching_ShimmerWeight(), SHIMMER_COLOR);
}

//  ---------------------------------------------------------------- in battle

static bool8 Carried(void)
{
    return gBattleStruct != NULL && !(gBattleTypeFlags & NOT_CARRIED);
}

static bool8 ShimmerCanTouch(u8 battler)
{
    struct BattleHealthboxInfo *info;
    u8 spriteId = gBattlerSpriteIds[battler];

    if (GetBattlerSide(battler) != B_SIDE_PLAYER || spriteId >= MAX_SPRITES || !gSprites[spriteId].inUse)
        return FALSE;
    // The healthbox shows once the daemon is out, so the trainer's back during the opening is never lit.
    if (gHealthboxSpriteIds[battler] >= MAX_SPRITES || gSprites[gHealthboxSpriteIds[battler]].invisible)
        return FALSE;
    info = &gBattleSpritesDataPtr->healthBoxesData[battler];
    return !info->ballAnimActive && !info->statusAnimActive && !info->specialAnimActive;
}

// Once a frame from BattleMainCB2, after the fade has run.
void Witching_BattleShimmer(void)
{
    u8 b;

    if (gBattleStruct == NULL || gBattleSpritesDataPtr == NULL || gPaletteFade.active || gAnimScriptActive)
        return;
    if ((gBattleTypeFlags & BATTLE_TYPE_LINK) || !Witching_IsHour())
    {
        if (gBattleStruct->witchFlags & WITCH_SHIMMERING)
        {
            for (b = 0; b < gBattlersCount; b++)
                if (ShimmerCanTouch(b))
                    BlendPalette(OBJ_PLTT_ID(gSprites[gBattlerSpriteIds[b]].oam.paletteNum), 16, 0, SHIMMER_COLOR);
            gBattleStruct->witchFlags &= ~WITCH_SHIMMERING;
        }
        return;
    }
    for (b = 0; b < gBattlersCount; b++)
        if (ShimmerCanTouch(b))
            Witching_ShimmerFromUnfaded(OBJ_PLTT_ID(gSprites[gBattlerSpriteIds[b]].oam.paletteNum));
    gBattleStruct->witchFlags |= WITCH_SHIMMERING;
}

void Witching_BattleStart(void)
{
    u8 i;

    gBattleStruct->witchFlags = 0;
    for (i = 0; i < ARRAY_COUNT(gBattleStruct->witchPpUsed); i++)
        gBattleStruct->witchPpUsed[i] = 0;
}

void Witching_TurnStart(void)
{
    if (gBattleStruct != NULL)
        gBattleStruct->witchFlags &= ~WITCH_CHOSEN_MASK;
}

u16 Witching_MoveFor(u8 battler)
{
    u16 move;

    if (!Carried() || battler >= gBattlersCount || GetBattlerSide(battler) != B_SIDE_PLAYER
     || !FlagGet(FLAG_GOT_SUBTLE_LAYER) || !Witching_IsHour())
        return MOVE_NONE;
    move = Witching_MoveForSpecies(GetMonData(&gPlayerParty[gBattlerPartyIndexes[battler]], MON_DATA_SPECIES));
    // ORACLE's strikes for nothing while vanilla's ??? rule stands (pokemon.c), and no daemon is ORACLE first.
    if (move == MOVE_SECOND_SIGHT)
        return MOVE_NONE;
    return move;
}

static u8 PpUsed(u8 slot)
{
    return (gBattleStruct->witchPpUsed[slot / 2] >> ((slot & 1) * 4)) & 0xF;
}

u8 Witching_PpLeft(u8 battler)
{
    u8 used;

    if (gBattleStruct == NULL || battler >= gBattlersCount)
        return 0;
    used = PpUsed(gBattlerPartyIndexes[battler] % PARTY_SIZE);
    return used >= WITCHING_PP ? 0 : WITCHING_PP - used;
}

bool8 Witching_CanChoose(u8 battler)
{
    u16 move = Witching_MoveFor(battler);
    u16 choiced;

    if (move == MOVE_NONE || Witching_PpLeft(battler) == 0)
        return FALSE;
    // Held to one routine by a CHOICE BAND: the hour does not undo it.
    choiced = gBattleStruct->choicedMove[battler];
    if (ItemId_GetHoldEffect(gBattleMons[battler].item) == HOLD_EFFECT_CHOICE_BAND
     && choiced != MOVE_NONE && choiced != MOVE_UNAVAILABLE && choiced != move)
        return FALSE;
    // NO REPLAY: not the same routine twice running.
    if ((gBattleMons[battler].status2 & STATUS2_TORMENT) && gLastMoves[battler] == move)
        return FALSE;
    return TRUE;
}

void Witching_SetChosen(u8 battler, bool8 chosen)
{
    if (gBattleStruct == NULL || battler >= MAX_BATTLERS_COUNT)
        return;
    if (chosen)
        gBattleStruct->witchFlags |= 1 << battler;
    else
        gBattleStruct->witchFlags &= ~(1 << battler);
}

bool8 Witching_IsChosen(u8 battler)
{
    return gBattleStruct != NULL && battler < MAX_BATTLERS_COUNT && (gBattleStruct->witchFlags & (1 << battler));
}

// Cmd_ppreduce's first question. The hour's routine spends one of its own uses (and PRESSURE's), never a slot's PP --
// chosenMovePositions still names a real slot, and nothing may be taken from it.
bool8 Witching_PpReduce(s32 ppToDeduct)
{
    u8 b = gBattlerAttacker;
    u8 slot, used;

    if (!IS_WITCHING_MOVE(gCurrentMove) || !Witching_IsChosen(b))
        return FALSE;
    if (!(gHitMarker & (HITMARKER_NO_PPDEDUCT | HITMARKER_NO_ATTACKSTRING)))
    {
        gProtectStructs[b].notFirstStrike = 1;
        slot = gBattlerPartyIndexes[b] % PARTY_SIZE;
        used = PpUsed(slot) + ppToDeduct;
        if (used > WITCHING_PP)
            used = WITCHING_PP;
        gBattleStruct->witchPpUsed[slot / 2] &= ~(0xF << ((slot & 1) * 4));
        gBattleStruct->witchPpUsed[slot / 2] |= used << ((slot & 1) * 4);
        Three_OnMoveUsed(b, gCurrentMove);
        Hermetic_OnWitchingRoutine();   // T-483: the first time, with the GUIDE and THE SUBTLE LAYER held, the book comes
    }
    gHitMarker &= ~HITMARKER_NO_PPDEDUCT;
    gBattlescriptCurrInstr++;
    return TRUE;
}

bool8 Witching_MenuPage(void)
{
    return gBattleStruct != NULL && (gBattleStruct->witchFlags & WITCH_MENU_PAGE);
}

void Witching_SetMenuPage(bool8 on)
{
    if (gBattleStruct == NULL)
        return;
    if (on)
        gBattleStruct->witchFlags |= WITCH_MENU_PAGE;
    else
        gBattleStruct->witchFlags &= ~WITCH_MENU_PAGE;
}
