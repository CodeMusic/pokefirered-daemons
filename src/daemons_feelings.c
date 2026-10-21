#include "global.h"
#include "battle.h"
#include "battle_anim.h"
#include "event_data.h"
#include "pokemon.h"
#include "pokemon_storage_system.h"
#include "daemons_feelings.h"
#include "daemon_saturation.h"
#include "daemons_follower.h"
#include "daemons_time.h"
#include "constants/battle.h"
#include "constants/items.h"
#include "constants/pokemon.h"

// T-431 P1: see include/daemons_feelings.h. The four values and the stamp live in the daemon's own record, so they go
// wherever it goes -- a box, a trade, the companion and back -- and nothing here is resident: 0 B of EWRAM and IWRAM.

// 255 * 2^(-ticks / 18), rounded: what is left of a value after that many 10-minute ticks (a 3-hour half-life), out
// of 255. Past the last entry -- a day -- nothing is left.
static const u8 sFade[FEELINGS_LOOK_BACK + 1] = {
    255, 245, 236, 227, 219, 210, 202, 195, 187, 180, 174, 167, 161, 155, 149, 143,
    138, 133, 128, 123, 118, 114, 109, 105, 101,  97,  94,  90,  87,  83,  80,  77,
     74,  72,  69,  66,  64,  61,  59,  57,  55,  53,  51,  49,  47,  45,  43,  42,
     40,  39,  37,  36,  34,  33,  32,  31,  30,  28,  27,  26,  25,  24,  23,  23,
     22,  21,  20,  19,  19,  18,  17,  17,  16,  15,  15,  14,  14,  13,  13,  12,
     12,  11,  11,  10,  10,  10,   9,   9,   9,   8,   8,   8,   7,   7,   7,   7,
      6,   6,   6,   6,   5,   5,   5,   5,   5,   4,   4,   4,   4,   4,   4,   4,
      3,   3,   3,   3,   3,   3,   3,   3,   3,   2,   2,   2,   2,   2,   2,   2,
      2,   2,   2,   2,   2,   2,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,
      1,
};

// ---- T-431 P2 and P4: the SUMMARY's fourth page (src/pokemon_summary_screen.c draws it) ---------------------------

// The sixteen lines of behaviour (the plan's section 5), ALL APPROVED BY THE USER 2026-10-21 AND STILL DRAFT IN THE
// GAME. Each says what the daemon does, never what it feels (craft rule 3), and each is one line of at most 208px
// (tools/check_lexicon.py measures them). Chosen by Feelings_BehaviourLine below.
static const u8 sFeelingsLine_GladCalm[]      = _("It walks at your pace, unhurried.");       // DRAFT
static const u8 sFeelingsLine_GladMiddle[]    = _("It hums through its routines.");           // DRAFT
static const u8 sFeelingsLine_GladAfraid[]    = _("It bounds ahead, then darts back.");       // DRAFT
static const u8 sFeelingsLine_MiddleCalm[]    = _("It dozes the moment you stop.");           // DRAFT
static const u8 sFeelingsLine_MiddleWary[]    = _("It keeps a careful step away.");           // DRAFT
static const u8 sFeelingsLine_MiddleClose[]   = _("It leans on you whenever you stop.");      // DRAFT
static const u8 sFeelingsLine_MiddleAfraid[]  = _("It checks behind you at each turn.");      // DRAFT
static const u8 sFeelingsLine_SadCalm[]       = _("It trails a step behind, quiet.");         // DRAFT
static const u8 sFeelingsLine_SadWary[]       = _("It stops often, looking back.");           // DRAFT
static const u8 sFeelingsLine_SadClose[]      = _("It stays close, but will not play.");      // DRAFT
static const u8 sFeelingsLine_SadAfraidWary[] = _("It hides behind whatever is near.");       // DRAFT
static const u8 sFeelingsLine_SadAfraidClose[]= _("It presses close at every noise.");        // DRAFT
static const u8 sFeelingsLine_MadAfraid[]     = _("It snaps at the air when you turn.");      // DRAFT
static const u8 sFeelingsLine_MadSad[]        = _("It turns its back when you look.");        // DRAFT
static const u8 sFeelingsLine_Mad[]           = _("It glares back at the last fight.");       // DRAFT
static const u8 sFeelingsLine_Ever[]          = _("It goes where you go, as ever.");          // DRAFT

void Feelings_GridCell(const struct DaemonFeelings *f, u8 *col, u8 *row)
{
    s32 x = (s32)f->glad - (s32)f->sad;     // -100 .. 100, right is glad
    s32 y = f->calm;                        // -100 .. 100, up is calm

    *col = (x + 100) * FEELINGS_GRID / 201;
    *row = (100 - y) * FEELINGS_GRID / 201;
}

// MAD 0 is a cool slate (88, 120, 168), MAD 100 an ember (232, 88, 40); between, the straight line. Then friendship
// takes its colour toward grey by the rule the daemon's own picture follows (src/daemon_saturation.c).
#define DOT_COOL_R 11
#define DOT_COOL_G 15
#define DOT_COOL_B 21
#define DOT_HOT_R  29
#define DOT_HOT_G  11
#define DOT_HOT_B   5

u16 Feelings_DotColour(u8 mad, u8 friendship)
{
    u16 buf[16];
    s32 m = mad > FEELINGS_MAX ? FEELINGS_MAX : mad;

    buf[0] = 0;
    buf[1] = RGB(DOT_COOL_R + (DOT_HOT_R - DOT_COOL_R) * m / FEELINGS_MAX,
                 DOT_COOL_G + (DOT_HOT_G - DOT_COOL_G) * m / FEELINGS_MAX,
                 DOT_COOL_B + (DOT_HOT_B - DOT_COOL_B) * m / FEELINGS_MAX);
    Saturation_ApplyToBuffer(buf, friendship);
    return buf[1];
}

// The plan's scheme: MAD 40 or more speaks first; then the 3x3 reading of GLAD - SAD against CALM (20 either side of
// 0 is the middle); then, where the plan gives two lines, friendship picks (under 100 the wary one, 150 and over the
// close one; between them the nearer, split at 125 -- a choice the plan left open). All near zero and friendship 220
// or more is "It goes where you go, as ever." A daemon never stamped (an old save's, an in-game trade's) has no
// history to read: it takes that line at 220 and over, and otherwise "It dozes the moment you stop."
const u8 *Feelings_BehaviourLine(struct BoxPokemon *boxMon, u8 friendship)
{
    struct DaemonFeelings f;
    s32 x, quiet;
    bool8 close = friendship >= (FEELINGS_FRIEND_LOW + FEELINGS_FRIEND_HIGH) / 2;

    if (!Feelings_IsSet(boxMon))
        return friendship >= FEELINGS_FRIEND_EVER ? sFeelingsLine_Ever : sFeelingsLine_MiddleCalm;

    Feelings_Get(boxMon, &f);
    x = (s32)f.glad - (s32)f.sad;
    quiet = f.glad < FEELINGS_QUIET && f.mad < FEELINGS_QUIET && f.sad < FEELINGS_QUIET
         && f.calm < FEELINGS_QUIET && f.calm > -FEELINGS_QUIET;

    if (f.mad >= FEELINGS_MAD_LINE)
    {
        if (f.calm <= -FEELINGS_LEAN)
            return sFeelingsLine_MadAfraid;
        if (x <= -FEELINGS_LEAN)
            return sFeelingsLine_MadSad;
        return sFeelingsLine_Mad;
    }
    if (quiet && friendship >= FEELINGS_FRIEND_EVER)
        return sFeelingsLine_Ever;

    if (x >= FEELINGS_LEAN)
    {
        if (f.calm >= FEELINGS_LEAN)
            return sFeelingsLine_GladCalm;
        if (f.calm <= -FEELINGS_LEAN)
            return sFeelingsLine_GladAfraid;
        return sFeelingsLine_GladMiddle;
    }
    if (x <= -FEELINGS_LEAN)
    {
        if (f.calm >= FEELINGS_LEAN)
            return sFeelingsLine_SadCalm;
        if (f.calm <= -FEELINGS_LEAN)
            return close ? sFeelingsLine_SadAfraidClose : sFeelingsLine_SadAfraidWary;
        return close ? sFeelingsLine_SadClose : sFeelingsLine_SadWary;
    }
    if (f.calm >= FEELINGS_LEAN)
        return sFeelingsLine_MiddleCalm;
    if (f.calm <= -FEELINGS_LEAN)
        return sFeelingsLine_MiddleAfraid;
    return close ? sFeelingsLine_MiddleClose : sFeelingsLine_MiddleWary;
}

#if DAEMONS_DEBUG
// The theatre's route reads the clock the game last read (tools/theatre_route.py's `feelings`): now, and 1 for the
// cartridge's clock or 0 for play time. 4 bytes, the DEBUG build only.
EWRAM_DATA u16 gFeelingsDebugNow[2] = {0};
EWRAM_DATA u32 gFeelingsDebugShown[2] = {0};   // T-431 P2: the fourth page's line, cell and dot
#endif

u16 Feelings_Now(bool8 *fromClock)
{
    u16 now = DaemonsFeelingsNow(fromClock);

#if DAEMONS_DEBUG
    gFeelingsDebugNow[0] = now;
    gFeelingsDebugNow[1] = *fromClock;
#endif
    return now;
}

static bool8 IsPushable(struct BoxPokemon *boxMon)
{
    return GetBoxMonData(boxMon, MON_DATA_SANITY_HAS_SPECIES)
        && !GetBoxMonData(boxMon, MON_DATA_IS_EGG)
        && !DaemonBoxIsAway(boxMon);
}

bool8 Feelings_IsSet(struct BoxPokemon *boxMon)
{
    return GetBoxMonData(boxMon, MON_DATA_SANITY_HAS_SPECIES)
        && !GetBoxMonData(boxMon, MON_DATA_IS_EGG)
        && (GetBoxMonData(boxMon, MON_DATA_UNUSED_RIBBONS) & FEELINGS_RIBBON_INIT);
}

static u16 Stamp(struct BoxPokemon *boxMon)
{
    return GetBoxMonData(boxMon, MON_DATA_TOUGH) | (GetBoxMonData(boxMon, MON_DATA_SHEEN) << 8);
}

static void SetStamp(struct BoxPokemon *boxMon, u16 stamp)
{
    u8 lo = stamp, hi = stamp >> 8;

    SetBoxMonData(boxMon, MON_DATA_TOUGH, &lo);
    SetBoxMonData(boxMon, MON_DATA_SHEEN, &hi);
}

// How many ticks ago it was stamped, or a day and more when the clock that stamped it is not the clock now (a save
// moved between a cartridge with a clock and one without): that reads as long gone.
static u16 TicksSince(struct BoxPokemon *boxMon)
{
    bool8 fromClock;
    u16 now = Feelings_Now(&fromClock);
    bool8 stampedByClock = (GetBoxMonData(boxMon, MON_DATA_UNUSED_RIBBONS) & FEELINGS_RIBBON_CLOCK) != 0;

    if (stampedByClock != fromClock)
        return 0xFFFF;
    return (u16)(now - Stamp(boxMon));
}

static u8 Faded(u8 value, u16 ticks)
{
    if (ticks > FEELINGS_LOOK_BACK)
        return 0;
    return (value * sFade[ticks] + 127) / 255;
}

static s8 FadedSigned(s8 value, u16 ticks)
{
    return value < 0 ? -(s8)Faded(-value, ticks) : (s8)Faded(value, ticks);
}

void Feelings_Get(struct BoxPokemon *boxMon, struct DaemonFeelings *out)
{
    u16 ticks;

    out->glad = out->mad = out->sad = 0;
    out->calm = 0;
    if (!Feelings_IsSet(boxMon))
        return;
    ticks = TicksSince(boxMon);
    out->glad = Faded(GetBoxMonData(boxMon, MON_DATA_COOL), ticks);
    out->mad = Faded(GetBoxMonData(boxMon, MON_DATA_BEAUTY), ticks);
    out->sad = Faded(GetBoxMonData(boxMon, MON_DATA_CUTE), ticks);
    out->calm = FadedSigned((s8)GetBoxMonData(boxMon, MON_DATA_SMART), ticks);
}

static u8 Clamp(s16 v)
{
    return v < 0 ? 0 : v > FEELINGS_MAX ? FEELINGS_MAX : v;
}

static s8 ClampCalm(s16 v)
{
    return v < FEELINGS_CALM_MIN ? FEELINGS_CALM_MIN : v > FEELINGS_MAX ? FEELINGS_MAX : v;
}

// The values as they are now, plus the push, stamped now with the clock it came from.
static void Store(struct BoxPokemon *boxMon, s16 glad, s16 mad, s16 sad, s16 calm)
{
    bool8 fromClock;
    u16 now = Feelings_Now(&fromClock);
    u8 g = Clamp(glad), m = Clamp(mad), s = Clamp(sad);
    s8 c = ClampCalm(calm);
    u8 ribbons = (GetBoxMonData(boxMon, MON_DATA_UNUSED_RIBBONS) & ~FEELINGS_RIBBON_CLOCK) | FEELINGS_RIBBON_INIT;

    if (fromClock)
        ribbons |= FEELINGS_RIBBON_CLOCK;
    SetBoxMonData(boxMon, MON_DATA_COOL, &g);
    SetBoxMonData(boxMon, MON_DATA_BEAUTY, &m);
    SetBoxMonData(boxMon, MON_DATA_CUTE, &s);
    SetBoxMonData(boxMon, MON_DATA_SMART, &c);
    SetStamp(boxMon, now);
    SetBoxMonData(boxMon, MON_DATA_UNUSED_RIBBONS, &ribbons);
}

void Feelings_Push(struct BoxPokemon *boxMon, s8 glad, s8 mad, s8 sad, s8 calm)
{
    struct DaemonFeelings f;

    if (!IsPushable(boxMon))
        return;
    Feelings_Get(boxMon, &f);
    Store(boxMon, f.glad + glad, f.mad + mad, f.sad + sad, f.calm + calm);
}

static bool8 IsPlayerPartyMon(struct Pokemon *mon)
{
    return mon >= &gPlayerParty[0] && mon < &gPlayerParty[PARTY_SIZE];
}

static void PushMon(struct Pokemon *mon, s8 glad, s8 mad, s8 sad, s8 calm)
{
    if (IsPlayerPartyMon(mon))
        Feelings_Push(&mon->box, glad, mad, sad, calm);
}

//  ---------------------------------------------------------------- the events

// AdjustFriendship's events, after vanilla's own change: a level in battle, a TM, the massage, a faint. Walking is
// Feelings_OnWalk's (the lead only, not vanilla's whole party at even odds), and a league battle pushes nothing.
void Feelings_OnFriendshipEvent(struct Pokemon *mon, u8 event)
{
    switch (event)
    {
    case FRIENDSHIP_EVENT_GROW_LEVEL:
        PushMon(mon, FEEL_LEVEL_GLAD, 0, 0, 0);
        break;
    case FRIENDSHIP_EVENT_LEARN_TMHM:
        PushMon(mon, FEEL_TM_GLAD, 0, 0, 0);
        break;
    case FRIENDSHIP_EVENT_MASSAGE:
        PushMon(mon, FEEL_MASSAGE_GLAD, 0, 0, FEEL_MASSAGE_CALM);
        break;
    case FRIENDSHIP_EVENT_FAINT_SMALL:
        PushMon(mon, 0, 0, FEEL_FAINT_SAD, FEEL_FAINT_CALM);
        break;
    case FRIENDSHIP_EVENT_FAINT_LARGE:
        PushMon(mon, 0, FEEL_FAINT_HARD_MAD, FEEL_FAINT_SAD, FEEL_FAINT_CALM);
        break;
    case FRIENDSHIP_EVENT_FAINT_OUTSIDE_BATTLE:
        PushMon(mon, 0, 0, FEEL_POISON_SAD, FEEL_POISON_CALM);
        break;
    }
}

// An item whose effect changed its friendship (PokemonUseItemEffects): a vitamin, a battle item, or a candy's level.
// The bitter herbs lower friendship and push nothing here.
void Feelings_OnItemFriendship(struct Pokemon *mon, u16 item)
{
    switch (item)
    {
    case ITEM_HP_UP:
    case ITEM_PROTEIN:
    case ITEM_IRON:
    case ITEM_CARBOS:
    case ITEM_CALCIUM:
    case ITEM_ZINC:
        PushMon(mon, FEEL_VITAMIN_GLAD, 0, 0, 0);
        break;
    case ITEM_RARE_CANDY:
        PushMon(mon, FEEL_LEVEL_GLAD, 0, 0, 0);
        break;
    case ITEM_GUARD_SPEC:
    case ITEM_DIRE_HIT:
    case ITEM_X_ATTACK:
    case ITEM_X_DEFEND:
    case ITEM_X_SPEED:
    case ITEM_X_ACCURACY:
    case ITEM_X_SPECIAL:
        PushMon(mon, FEEL_BATTLE_ITEM_GLAD, 0, 0, 0);
        break;
    }
}

// The battle is over. Won: each daemon that took part (gBattleStruct->feelingsTookPart, a bit a party slot, set as
// each is sent out). Lost: the whole party. A link battle, the old man's lesson and the POKéDUDE's are not the
// player's own party's, and running, a capture or a draw by time push nothing.
void Feelings_OnBattleEnd(void)
{
    u8 i, tookPart;

    if (gBattleTypeFlags & (BATTLE_TYPE_LINK | BATTLE_TYPE_OLD_MAN_TUTORIAL | BATTLE_TYPE_POKEDUDE | BATTLE_TYPE_SAFARI))
        return;
    tookPart = gBattleStruct->feelingsTookPart;
    for (i = 0; i < gBattlersCount; i++)
        if (GetBattlerSide(i) == B_SIDE_PLAYER)
            tookPart |= 1 << gBattlerPartyIndexes[i];
    for (i = 0; i < PARTY_SIZE; i++)
    {
        if (gBattleOutcome == B_OUTCOME_WON && (tookPart & (1 << i)))
            Feelings_Push(&gPlayerParty[i].box, FEEL_WON_GLAD, 0, 0, 0);
        else if (gBattleOutcome == B_OUTCOME_LOST)
            Feelings_Push(&gPlayerParty[i].box, 0, FEEL_LOST_MAD, FEEL_LOST_SAD, FEEL_LOST_CALM);
    }
}

// Every 128 steps (UpdateHappinessStepCounter): the daemon walking behind you, when one is.
void Feelings_OnWalk(void)
{
    s8 slot;

    if (FlagGet(FLAG_FOLLOWER_OFF))
        return;
    slot = Follower_LeadSlot();
    if (slot >= 0)
        Feelings_Push(&gPlayerParty[slot].box, FEEL_WALK_GLAD, 0, 0, FEEL_WALK_CALM);
}

void Feelings_OnHeldItemGiven(struct Pokemon *mon)
{
    PushMon(mon, FEEL_HELD_ITEM_GLAD, 0, 0, 0);
}

// Into a box: what it feels now is kept and stamped, so the stamp says when it went in.
void Feelings_OnDeposit(struct BoxPokemon *boxMon)
{
    struct DaemonFeelings f;

    if (!IsPushable(boxMon) || !Feelings_IsSet(boxMon))
        return;
    Feelings_Get(boxMon, &f);
    Store(boxMon, f.glad, f.mad, f.sad, f.calm);
}

// Out of a box: how long it was in is the stamp's age (nothing pushes a boxed daemon). Under an hour, nothing; up to a
// day, a little sad; longer, sadder and a little afraid. A daemon never stamped (boxed before this build, or caught
// straight into one) has no known time in there and is left as it is; one stamped by the other clock reads as gone.
void Feelings_OnWithdraw(struct Pokemon *mon)
{
    u16 ticks;

    if (!IsPushable(&mon->box) || !Feelings_IsSet(&mon->box))
        return;
    ticks = TicksSince(&mon->box);
    if (ticks > FEELINGS_LOOK_BACK)
        Feelings_Push(&mon->box, 0, 0, FEEL_BOX_DAY_SAD, FEEL_BOX_DAY_CALM);
    else if (ticks >= FEEL_BOX_HOURS_FROM)
        Feelings_Push(&mon->box, 0, 0, FEEL_BOX_HOURS_SAD, 0);
}

// The CHECKPOINT's machine, when the player asked for it (EventScript_PkmnCenterNurse_HealPkmn): the whole party,
// calmer. The white-out's heal goes through the same machine (white_out.inc calls TakeAndHealPkmn directly) and pushes
// nothing -- the user, 2026-10-21: only a heal the player chooses calms.
void Feelings_CheckpointHeal(void)
{
    u8 i;

    for (i = 0; i < PARTY_SIZE; i++)
        Feelings_Push(&gPlayerParty[i].box, 0, 0, 0, FEEL_HEAL_CALM);
}

//  ---------------------------------------------------------------- DEBUG

#if DAEMONS_DEBUG
static void Age(struct BoxPokemon *boxMon, u16 ticks)
{
    if (Feelings_IsSet(boxMon))
        SetStamp(boxMon, Stamp(boxMon) - ticks);
}

// +3 HOURS: every stamp in the party and the boxes made older, as if the time had passed.
void Feelings_DebugAge(u16 ticks)
{
    u32 box, pos;

    for (pos = 0; pos < PARTY_SIZE; pos++)
        Age(&gPlayerParty[pos].box, ticks);
    for (box = 0; box < TOTAL_BOXES_COUNT; box++)
        for (pos = 0; pos < IN_BOX_COUNT; pos++)
            Age(GetBoxedMonPtr(box, pos), ticks);
}

// PUSH GLAD / MAD / SAD / AFRAID: a large push (+30, or calm -30) on the first daemon that is not an egg.
void Feelings_DebugPushLead(u8 which)
{
    u8 i;

    for (i = 0; i < PARTY_SIZE; i++)
    {
        if (!IsPushable(&gPlayerParty[i].box))
            continue;
        Feelings_Push(&gPlayerParty[i].box, which == FEEL_GLAD ? 30 : 0, which == FEEL_MAD ? 30 : 0,
                      which == FEEL_SAD ? 30 : 0, which == FEEL_CALM ? -30 : 0);
        return;
    }
}
#endif
