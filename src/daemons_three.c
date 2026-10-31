#include "global.h"
#include "battle.h"
#include "battle_anim.h"
#include "pokemon.h"
#include "string_util.h"
#include "characters.h"
#include "daemons_feelings.h"
#include "daemons_three.h"
#include "daemons_spellbook.h"   // T-483 (5)
#include "event_data.h"
#include "battle_main.h"
#include "palette.h"
#include "random.h"
#include "util.h"
#include "constants/vars.h"
#include "constants/battle.h"
#include "constants/flags.h"
#include "constants/pokemon.h"

// T-454: see include/daemons_three.h -- the rules, where the values live (nowhere saved), and the table
// (approved 2026-10-27). In effect only once THE SUBTLE LAYER is held (FLAG_GOT_SUBTLE_LAYER).

#define NOT_CARRIED (BATTLE_TYPE_LINK | BATTLE_TYPE_MULTI | BATTLE_TYPE_POKEDUDE | BATTLE_TYPE_OLD_MAN_TUTORIAL \
                   | BATTLE_TYPE_SAFARI)

#if DAEMONS_DEBUG
// Set by hand for the next battles: 0 AUTO, else the level + 1. Party slots, so a reorder keeps them on the slot.
// 19 bytes of EWRAM, the DEBUG build only.
static EWRAM_DATA u8 sDebugSet[PARTY_SIZE][THREE_COUNT] = {0};
static EWRAM_DATA u8 sDebugSlot = 0;
#endif

static u8 Up(u8 v, u8 by)
{
    return v + by > THREE_MAX ? THREE_MAX : v + by;
}

// What a daemon carries in to a battle, read from its friendship and its feelings now.
static void Derive(struct Pokemon *mon, u8 *out)
{
    struct DaemonFeelings f;
    u8 friendship = GetMonData(mon, MON_DATA_FRIENDSHIP);
    u8 peak;
    s8 v;

    Feelings_Get(&mon->box, &f);   // all zero for a daemon never pushed

    v = friendship < THREE_CLOSURE_FRIEND_1 ? 1 : friendship < THREE_CLOSURE_FRIEND_3 ? 2
      : friendship < THREE_CLOSURE_FRIEND_4 ? 3 : 4;
    if (f.mad >= THREE_CLOSURE_MAD_LINE)
        v--;
    out[THREE_CLOSURE] = v;

    v = 2;
    if (f.calm >= THREE_ATTUNE_CALM_3)
        v++;
    if (f.calm >= THREE_ATTUNE_CALM_4)
        v++;
    if (f.calm <= THREE_ATTUNE_CALM_1)
        v--;
    if (f.calm <= THREE_ATTUNE_CALM_0)
        v--;
    out[THREE_ATTUNEMENT] = v;

    peak = f.glad;
    if (f.mad > peak)
        peak = f.mad;
    if (f.sad > peak)
        peak = f.sad;
    if (f.calm < 0 && -f.calm > peak)
        peak = -f.calm;
    out[THREE_VOLTAGE] = peak < THREE_VOLTAGE_FEEL_2 ? 1 : peak < THREE_VOLTAGE_FEEL_3 ? 2
                       : peak < THREE_VOLTAGE_FEEL_4 ? 3 : 4;

#if DAEMONS_DEBUG
    {
        u8 slot = mon - gPlayerParty, i;

        if (slot < PARTY_SIZE)
            for (i = 0; i < THREE_COUNT; i++)
                if (sDebugSet[slot][i])
                    out[i] = sDebugSet[slot][i] - 1;
    }
#endif
}

void Three_BattleStart(void)
{
    u8 i;

    // The user, 2026-10-27: they wake with THE SUBTLE LAYER. Before the book is held a battle is vanilla's -- threeOn
    // stays FALSE, so nothing is derived and every hook below is a no-op: no read, no write, no leak.
    gBattleStruct->threeOn = !(gBattleTypeFlags & NOT_CARRIED) && FlagGet(FLAG_GOT_SUBTLE_LAYER);
    if (!gBattleStruct->threeOn)
        return;
    for (i = 0; i < PARTY_SIZE; i++)
        if (GetMonData(&gPlayerParty[i], MON_DATA_SANITY_HAS_SPECIES) && !GetMonData(&gPlayerParty[i], MON_DATA_IS_EGG))
            Derive(&gPlayerParty[i], gBattleStruct->three[i]);
}

// The battler's own three, or NULL: outside a battle, for a foe, in a battle that does not carry them.
static u8 *Slot(u8 battler)
{
    if (gBattleStruct == NULL || !gBattleStruct->threeOn || battler >= gBattlersCount
     || GetBattlerSide(battler) != B_SIDE_PLAYER || gBattlerPartyIndexes[battler] >= PARTY_SIZE)
        return NULL;
    return gBattleStruct->three[gBattlerPartyIndexes[battler]];
}

bool8 Three_Get(u8 battler, u8 *out)
{
    u8 *three = Slot(battler);
    s8 closure;

    if (three == NULL)
        return FALSE;
    closure = three[THREE_CLOSURE];
    if (gBattleMons[battler].status2 & STATUS2_CONFUSION)
        closure--;
    if (gBattleMons[battler].status2 & STATUS2_INFATUATION)
        closure--;
    out[THREE_CLOSURE] = closure < 0 ? 0 : closure;
    out[THREE_ATTUNEMENT] = three[THREE_ATTUNEMENT];
    out[THREE_VOLTAGE] = three[THREE_VOLTAGE];
    return TRUE;
}

s8 Three_ReadScore(u8 battler)
{
    u8 v[THREE_COUNT];

    if (!Three_Get(battler, v))
        return 0;
    return v[THREE_ATTUNEMENT] - v[THREE_VOLTAGE];
}

u16 Three_AdjustAccuracy(u8 attacker, u8 moveType, u16 calc)
{
    s8 read;

    if (moveType != TYPE_PSYCHIC)   // CONTEXT
        return calc;
    read = Three_ReadScore(attacker);
    if (read > 0)
        calc += calc * read * THREE_READ_UP / 100;
    else if (read < 0)
        calc -= calc * -read * THREE_READ_DOWN / 100;
    return calc;
}

s32 Three_AdjustDamage(u8 attacker, u8 moveType, s32 damage)
{
    u8 v[THREE_COUNT];
    s32 pct = 0;

    if (moveType != TYPE_PSYCHIC || damage <= 0 || !Three_Get(attacker, v))
        return damage;
    if (v[THREE_CLOSURE] >= THREE_CLOSURE_HOLDS)
    {
        pct = v[THREE_ATTUNEMENT] * v[THREE_VOLTAGE] - THREE_WRITE_FROM;   // a write: both at once
        if (pct < 0)
            pct = 0;
        if (pct > THREE_WRITE_MAX)
            pct = THREE_WRITE_MAX;
    }
    else if (v[THREE_VOLTAGE] >= THREE_LEAK_VOLTAGE)
    {
        pct = -(v[THREE_VOLTAGE] - 2) * THREE_LEAK_STEP;                     // flooded, and not whole: a leak
    }
    damage += damage * pct / 100;
    return damage < 1 ? 1 : damage;
}

void Three_OnMoveUsed(u8 attacker, u16 move)
{
    u8 *three = Slot(attacker);

    if (three != NULL && gBattleMoves[move].power == 0)
        three[THREE_ATTUNEMENT] = Up(three[THREE_ATTUNEMENT], THREE_ATTUNE_STILL);
}

void Three_OnHit(u8 target, u8 critMultiplier)
{
    u8 *three = Slot(target);

    if (three != NULL && critMultiplier > 1)
        three[THREE_VOLTAGE] = Up(three[THREE_VOLTAGE], THREE_VOLTAGE_JOLT);
}

// ---- T-459 C: THE LEAN ------------------------------------------------------------------------------------------------
// Before some turns, where "What will X do?" stood, a purple line: what the foe will do this turn. It is read from what
// the foe has really chosen -- the AI chooses without seeing the player's choice, in the frame or two after the menu is
// asked for, so the menu waits for it -- and it is that more often as READ (ATTUNEMENT - VOLTAGE) rises; otherwise it
// is something else the foe could have done. Counted in the NOTEBOOK's log once the foe's turn comes, and landed by what
// the foe DID, not what it meant: the log decides, and the game never vouches (bible 4.33). Singles only.

enum { LEAN_NONE, LEAN_STRIKE, LEAN_HOLD_BACK, LEAN_TURN_AWAY };

#define LEAN_DECIDED    (1 << 0)    // this turn's roll is made (the menu opens again after a B, the lean with it)
#define LEAN_WAITING    (1 << 1)    // the roll said yes: waiting on the foe's choice
#define LEAN_LAST_TURN  (1 << 2)    // a lean came last turn, so none this one
#define LEAN_FOE_ACTED  (1 << 3)    // the foe's turn came, and it was standing
#define LEAN_FOE_STRUCK (1 << 4)    // ... it used a routine with power
#define LEAN_FOE_AWAY   (1 << 5)    // ... it switched out, or fled

#define LEAN_GIVE_UP    30          // frames to wait on the foe before letting this turn's lean go

// The book's purple on the battle's dark box: gensubtle.py's C_MARGIN, the hue lit (C_RULE, its ink, reads at 1.8:1 on
// it), the shadow the box's own. BG palette 0's slot 12 is a magenta placeholder in textbox.gbapal that nothing draws.
#define LEAN_SLOT       12
static const u16 sLeanPurple[] = { RGB(29, 17, 26) };
static const u8 sText_LeanStrike[]   = _("{COLOR 12}{SHADOW 6}A lean: this turn,\nit will strike.");      // DRAFT
static const u8 sText_LeanHoldBack[] = _("{COLOR 12}{SHADOW 6}A lean: this turn,\nit will hold back.");   // DRAFT
static const u8 sText_LeanTurnAway[] = _("{COLOR 12}{SHADOW 6}A lean: this turn,\nit will turn away.");   // DRAFT
static const u8 *const sLeans[] = { NULL, sText_LeanStrike, sText_LeanHoldBack, sText_LeanTurnAway };

#if DAEMONS_DEBUG
EWRAM_DATA u8 gThreeDebugLean = 0;
#endif

static u8 Foe(void)
{
    return GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT);
}

bool8 Three_LeanWanted(u8 battler)
{
    bool8 want;

    if (gBattleStruct == NULL || !gBattleStruct->threeOn || gBattleStruct->leanState & LEAN_DECIDED)
        return FALSE;
    gBattleStruct->leanState |= LEAN_DECIDED;
    gBattleStruct->lean = LEAN_NONE;
    gBattleStruct->leanWait = 0;
    if (gBattleTypeFlags & BATTLE_TYPE_DOUBLE || battler != GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)
     || Slot(battler) == NULL)
        return FALSE;
    if (Spellbook_LeanHushed())
        return FALSE;                   // T-483 (5): THE LENS or THE PATTERN is set -- reason speaks, the lean does not
    want = !(gBattleStruct->leanState & LEAN_LAST_TURN) && Random() % 100 < THREE_LEAN_CHANCE;
    if (Spellbook_FieldOn())
        want = TRUE;                    // T-483 (5): THE FIELD -- no numbers, so the lean comes every turn
#if DAEMONS_DEBUG
    if (gThreeDebugLean)
        want = TRUE;
#endif
    if (want)
        gBattleStruct->leanState |= LEAN_WAITING;
    return want;
}

// A trainer's daemon with another able to come in, or a wild one that flees: whether TURN AWAY could be a wrong lean.
static bool8 CanTurnAway(u8 foe)
{
    u8 i;
    u16 species;

    if (gBattleTypeFlags & BATTLE_TYPE_ROAMER)
        return TRUE;
    if (!(gBattleTypeFlags & BATTLE_TYPE_TRAINER))
        return FALSE;
    for (i = 0; i < PARTY_SIZE; i++)
    {
        species = GetMonData(&gEnemyParty[i], MON_DATA_SPECIES_OR_EGG);
        if (i != gBattlerPartyIndexes[foe] && species != SPECIES_NONE && species != SPECIES_EGG
         && GetMonData(&gEnemyParty[i], MON_DATA_HP) != 0)
            return TRUE;
    }
    return FALSE;
}

// What the foe has really chosen this turn.
static u8 Intent(u8 foe)
{
    u16 move;

    switch (gChosenActionByBattler[foe])
    {
    case B_ACTION_SWITCH:
    case B_ACTION_RUN:
        return LEAN_TURN_AWAY;
    case B_ACTION_USE_MOVE:
        break;
    default:
        return LEAN_HOLD_BACK;          // an item, or watching
    }
    if (gBattleMons[foe].status2 & STATUS2_RECHARGE)
        return LEAN_HOLD_BACK;
    if (gBattleMons[foe].status2 & STATUS2_MULTIPLETURNS)
        move = gLockedMoves[foe];
    else if (gProtectStructs[foe].noValidMoves)
        return LEAN_STRIKE;             // STRUGGLE
    else
        move = gChosenMoveByBattler[foe];
    return gBattleMoves[move].power != 0 ? LEAN_STRIKE : LEAN_HOLD_BACK;
}

bool8 Three_LeanReady(void)
{
    u8 foe = Foe(), truth, others[LEAN_TURN_AWAY], n = 0, k;
    s16 right;

    if (!(gBattleStruct->leanState & LEAN_WAITING))
        return TRUE;
    if (!BattleMain_ActionSettled(foe) && ++gBattleStruct->leanWait < LEAN_GIVE_UP)
        return FALSE;
    gBattleStruct->leanState &= ~LEAN_WAITING;
    if (!BattleMain_ActionSettled(foe))
        return TRUE;                    // the foe never settled: no lean this turn
    truth = Intent(foe);
    right = THREE_LEAN_RIGHT + Three_ReadScore(GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)) * THREE_LEAN_RIGHT_STEP;
    if (Random() % 100 < right)
    {
        gBattleStruct->lean = truth;
        return TRUE;
    }
    for (k = LEAN_STRIKE; k <= LEAN_TURN_AWAY; k++)
        if (k != truth && (k != LEAN_TURN_AWAY || CanTurnAway(foe)))
            others[n++] = k;
    gBattleStruct->lean = others[Random() % n];
    return TRUE;
}

const u8 *Three_LeanText(void)
{
    if (gBattleStruct == NULL || gBattleStruct->lean == LEAN_NONE)
        return NULL;
    LoadPalette(sLeanPurple, BG_PLTT_ID(0) + LEAN_SLOT, PLTT_SIZEOF(1));   // every time: a menu's return reloads 0
    return sLeans[gBattleStruct->lean];
}

void Three_LeanOnAction(u8 battler, u8 action)
{
    if (gBattleStruct == NULL || gBattleStruct->lean == LEAN_NONE || battler != Foe()
     || gBattleMons[battler].hp == 0 || gAbsentBattlerFlags & gBitTable[battler])
        return;
    if (action == B_ACTION_USE_MOVE || action == B_ACTION_USE_ITEM)
        gBattleStruct->leanState |= LEAN_FOE_ACTED;
    else if (action == B_ACTION_SWITCH || action == B_ACTION_RUN)
        gBattleStruct->leanState |= LEAN_FOE_ACTED | LEAN_FOE_AWAY;
}

void Three_LeanOnAttackString(u8 attacker, u16 move)
{
    if (gBattleStruct != NULL && gBattleStruct->lean != LEAN_NONE && attacker == Foe() && gBattleMoves[move].power != 0)
        gBattleStruct->leanState |= LEAN_FOE_STRUCK;
}

static void Count(u16 var)
{
    u16 n = VarGet(var);

    if (n < 9999)
        VarSet(var, n + 1);
}

void Three_LeanTurnEnds(void)
{
    u8 lean, state, did;

    if (gBattleStruct == NULL)
        return;
    lean = gBattleStruct->lean;
    state = gBattleStruct->leanState;
    gBattleStruct->lean = LEAN_NONE;
    gBattleStruct->leanState = lean != LEAN_NONE ? LEAN_LAST_TURN : 0;
    if (lean == LEAN_NONE || !(state & LEAN_FOE_ACTED))
        return;                         // no lean, or the foe's turn never came: nothing to count
    did = state & LEAN_FOE_AWAY ? LEAN_TURN_AWAY : state & LEAN_FOE_STRUCK ? LEAN_STRIKE : LEAN_HOLD_BACK;
    Count(VAR_LEANS_TOLD);
    if (did == lean)
        Count(VAR_LEANS_LANDED);
}

void Three_OnFaint(u8 fainted, u8 attacker)
{
    u8 i, *three;

    if (gBattleStruct == NULL || !gBattleStruct->threeOn)
        return;
    if (GetBattlerSide(fainted) == B_SIDE_PLAYER)
    {
        for (i = 0; i < PARTY_SIZE; i++)   // grief: the rest of the party
            if (i != gBattlerPartyIndexes[fainted] && GetMonData(&gPlayerParty[i], MON_DATA_HP) != 0)
                gBattleStruct->three[i][THREE_VOLTAGE] = Up(gBattleStruct->three[i][THREE_VOLTAGE], THREE_VOLTAGE_GRIEF);
    }
    else if ((three = Slot(attacker)) != NULL)   // joy: the one that felled it
    {
        three[THREE_VOLTAGE] = Up(three[THREE_VOLTAGE], THREE_VOLTAGE_JOY);
    }
}

#if DAEMONS_DEBUG

//  "1 C2 A2 V1 R+1" -- slot 1 as the next battle would start it; "!" before a value set by hand; ">" on the slot
//  picked. "-" for an empty slot, EGG for an egg.
void Three_DebugLabel(u8 *label, u8 slot)
{
    static const u8 sEgg[] = _("EGG");
    static const u8 sLetters[THREE_COUNT] = { CHAR_C, CHAR_A, CHAR_V };
    struct Pokemon *mon = &gPlayerParty[slot];
    u8 v[THREE_COUNT], i, *p = label;
    s8 read;

    *p++ = slot == sDebugSlot ? CHAR_GREATER_THAN : CHAR_SPACE;
    *p++ = CHAR_1 + slot;
    *p++ = CHAR_SPACE;
    *p = EOS;
    if (!GetMonData(mon, MON_DATA_SANITY_HAS_SPECIES))
    {
        *p++ = CHAR_HYPHEN;
        *p = EOS;
        return;
    }
    if (GetMonData(mon, MON_DATA_IS_EGG))
    {
        StringCopy(p, sEgg);
        return;
    }
    Derive(mon, v);
    for (i = 0; i < THREE_COUNT; i++)
    {
        if (sDebugSet[slot][i])
            *p++ = CHAR_EXCL_MARK;
        *p++ = sLetters[i];
        *p++ = CHAR_0 + v[i];
        *p++ = CHAR_SPACE;
    }
    read = v[THREE_ATTUNEMENT] - v[THREE_VOLTAGE];
    *p++ = CHAR_R;
    *p++ = read < 0 ? CHAR_HYPHEN : CHAR_PLUS;
    *p++ = CHAR_0 + (read < 0 ? -read : read);
    *p = EOS;
}

void Three_DebugPick(u8 slot)
{
    if (slot < PARTY_SIZE)
        sDebugSlot = slot;
}

u8 Three_DebugPicked(void)
{
    return sDebugSlot;
}

void Three_DebugCycle(u8 which)
{
    u8 *set = &sDebugSet[sDebugSlot][which];

    *set = *set > THREE_MAX ? 0 : *set + 1;   // AUTO, 0, 1, 2, 3, 4, AUTO
}

void Three_DebugAllAuto(void)
{
    u8 i, j;

    for (i = 0; i < PARTY_SIZE; i++)
        for (j = 0; j < THREE_COUNT; j++)
            sDebugSet[i][j] = 0;
}

#endif
