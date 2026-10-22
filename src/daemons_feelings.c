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
#include "berry.h"
#include "fieldmap.h"
#include "item.h"
#include "item_menu.h"
#include "constants/abilities.h"
#include "constants/battle.h"
#include "constants/hold_effects.h"
#include "constants/items.h"
#include "constants/pokemon.h"
#include "constants/trainers.h"

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

// T-433: AFFINITY (vanilla's Soothe Bell) already makes friendship rise half again as fast; now GLAD does too.
void Feelings_Push(struct BoxPokemon *boxMon, s8 glad, s8 mad, s8 sad, s8 calm)
{
    struct DaemonFeelings f;
    s16 g = glad;

    if (!IsPushable(boxMon))
        return;
    if (g > 0 && ItemId_GetHoldEffect(GetBoxMonData(boxMon, MON_DATA_HELD_ITEM)) == HOLD_EFFECT_FRIENDSHIP_UP)
        g = g * 3 / 2;
    Feelings_Get(boxMon, &f);
    Store(boxMon, f.glad + g, f.mad + mad, f.sad + sad, f.calm + calm);
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

//  ---------------------------------------------------------------- T-433: items

// What each item pushes when it works -- hand-given (PokemonUseItemEffects, in the field or in battle) or, for the
// berries, herbs and bands, held and acting in battle by itself (Feelings_OnHeldItemActed) -- and the friendship it
// adds beyond vanilla's own (the drinks, the LAVA COOKIE and the revives). The user's answers, 2026-10-21: everything as
// the plan proposed it, with the RARE CANDY's MAD and the bitter herbs' MAD. ALL DRAFT. Vanilla's friendship changes
// (vitamins, MP UP and MAX, the herbs, the candy's -- cut to +1/+1/0 in item_effects.h) still happen where they did;
// the TMs and DRIVERs push from AdjustFriendship (Feelings_OnFriendshipEvent). The flavour TRAPs are the code's:
// liked or hated, by the daemon's nature (Feelings_OnHeldItemActed). An item not here pushes nothing.
struct ItemFeelings
{
    u16 item;
    s8 glad, mad, sad, calm;
    s8 friendship;
};

static const struct ItemFeelings sItemFeelings[] = {
    //  healing: by strength; the drinks and the cookie are treats from your hand
    { ITEM_POTION,          2, 0, 0, 0, 0 },    // RECOVER
    { ITEM_SUPER_POTION,    3, 0, 0, 0, 0 },    // DEEP RECOVER
    { ITEM_HYPER_POTION,    5, 0, 0, 0, 0 },    // FULL RECOVER
    { ITEM_MAX_POTION,      6, 0, 0, 0, 0 },    // REBUILD
    { ITEM_FULL_RESTORE,    7, 0, 0, 4, 0 },    // SNAPSHOT
    { ITEM_FRESH_WATER,     3, 0, 0, 0, 1 },    // FRESH WATER
    { ITEM_SODA_POP,        4, 0, 0, 0, 1 },    // SODA POP
    { ITEM_LEMONADE,        4, 0, 0, 0, 1 },    // LEMONADE
    { ITEM_MOOMOO_MILK,     5, 0, 0, 0, 1 },    // MOOMOO MILK
    { ITEM_BERRY_JUICE,     2, 0, 0, 0, 0 },    // RESERVE (held: drunk by itself)
    { ITEM_ENERGY_POWDER,   0, 3, 0, 0, 0 },    // FIELD PATCH: it works, and it is bitter
    { ITEM_ENERGY_ROOT,     0, 5, 0, 0, 0 },    // WORKAROUND
    //  revives: the way back from a faint
    { ITEM_REVIVE,          6, 0, 0, 3, 1 },    // RESTART
    { ITEM_MAX_REVIVE,     10, 0, 0, 5, 2 },    // REBOOT
    { ITEM_SACRED_ASH,     10, 0, 0, 5, 2 },    // RESTART ALL (each)
    { ITEM_REVIVAL_HERB,    4, 6, 0, 0, 0 },    // COLD START: back, the hard way
    //  status cures: the trouble taken away
    { ITEM_ANTIDOTE,        1, 0, 0, 3, 0 },    // PATCH
    { ITEM_BURN_HEAL,       1, 0, 0, 3, 0 },    // COOLANT
    { ITEM_ICE_HEAL,        1, 0, 0, 3, 0 },    // WATCHDOG
    { ITEM_AWAKENING,       0, 0, 0, 2, 0 },    // RESUME: woken gently
    { ITEM_PARALYZE_HEAL,   1, 0, 0, 3, 0 },    // PRIORITY
    { ITEM_FULL_HEAL,       2, 0, 0, 5, 0 },    // ROLLBACK
    { ITEM_HEAL_POWDER,     0, 3, 0, 3, 0 },    // HOTFIX: cured, and bitter
    { ITEM_LAVA_COOKIE,     5, 0, 0, 2, 1 },    // LAVA COOKIE
    { ITEM_BLUE_FLUTE,      0, 0, 0, 3, 0 },    // WAKE TONE
    { ITEM_YELLOW_FLUTE,    0, 0, 0, 6, 0 },    // CALM TONE
    { ITEM_RED_FLUTE,       0, 0, 0, 4, 0 },    // CLEAR TONE
    //  routines refilled
    { ITEM_ETHER,           2, 0, 0, 0, 0 },    // CHARGE
    { ITEM_MAX_ETHER,       3, 0, 0, 0, 0 },    // FULL CHARGE
    { ITEM_ELIXIR,          3, 0, 0, 0, 0 },    // CHARGE ALL
    { ITEM_MAX_ELIXIR,      4, 0, 0, 0, 0 },    // FULL RECHARGE
    { ITEM_PP_UP,           4, 0, 0, 0, 0 },    // MP UP
    { ITEM_PP_MAX,          5, 0, 0, 0, 0 },    // MP MAX
    //  vitamins: training it chose to take (T-431 P1's push, kept)
    { ITEM_HP_UP,           FEEL_VITAMIN_GLAD, 0, 0, 0, 0 },    // MEMORY
    { ITEM_PROTEIN,         FEEL_VITAMIN_GLAD, 0, 0, 0, 0 },    // WATTAGE
    { ITEM_IRON,            FEEL_VITAMIN_GLAD, 0, 0, 0, 0 },    // SHIELDING
    { ITEM_CARBOS,          FEEL_VITAMIN_GLAD, 0, 0, 0, 0 },    // CLOCK RATE
    { ITEM_CALCIUM,         FEEL_VITAMIN_GLAD, 0, 0, 0, 0 },    // BANDWIDTH
    { ITEM_ZINC,            FEEL_VITAMIN_GLAD, 0, 0, 0, 0 },    // INSULATION
    //  the user's own: robbed of its natural growth, and its level earns no level's GLAD
    { ITEM_RARE_CANDY,      0, 5, 0, 0, 0 },    // INCREMENT
    //  battle items: backed up mid-fight (P1's push), LOCK STATS held steady
    { ITEM_X_ATTACK,        FEEL_BATTLE_ITEM_GLAD, 0, 0, 0, 0 },    // TUNE ATTACK
    { ITEM_X_DEFEND,        FEEL_BATTLE_ITEM_GLAD, 0, 0, 0, 0 },    // TUNE DEFENSE
    { ITEM_X_SPEED,         FEEL_BATTLE_ITEM_GLAD, 0, 0, 0, 0 },    // TUNE SPEED
    { ITEM_X_ACCURACY,      FEEL_BATTLE_ITEM_GLAD, 0, 0, 0, 0 },    // TUNE AIM
    { ITEM_X_SPECIAL,       FEEL_BATTLE_ITEM_GLAD, 0, 0, 0, 0 },    // TUNE SP. ATK
    { ITEM_DIRE_HIT,        FEEL_BATTLE_ITEM_GLAD, 0, 0, 0, 0 },    // TUNE CRIT
    { ITEM_GUARD_SPEC,      1, 0, 0, 3, 0 },    // LOCK STATS
    //  berries, hand-fed or eaten while held: about half a shop item's push
    { ITEM_CHERI_BERRY,     0, 0, 0, 2, 0 },    // THROTTLE TRAP
    { ITEM_CHESTO_BERRY,    0, 0, 0, 2, 0 },    // SUSPEND TRAP
    { ITEM_PECHA_BERRY,     0, 0, 0, 2, 0 },    // LEAK TRAP
    { ITEM_RAWST_BERRY,     0, 0, 0, 2, 0 },    // HEAT TRAP
    { ITEM_ASPEAR_BERRY,    0, 0, 0, 2, 0 },    // HANG TRAP
    { ITEM_PERSIM_BERRY,    0, 0, 0, 3, 0 },    // THRASH TRAP
    { ITEM_LUM_BERRY,       1, 0, 0, 4, 0 },    // CATCH ALL
    { ITEM_LEPPA_BERRY,     1, 0, 0, 0, 0 },    // EXHAUST TRAP
    { ITEM_ORAN_BERRY,      1, 0, 0, 0, 0 },    // LOW MARK
    { ITEM_SITRUS_BERRY,    2, 0, 0, 0, 0 },    // HIGH MARK
    { ITEM_LIECHI_BERRY,    0, 0, 0, 3, 0 },    // AUTO ATTACK: a reserve, when cornered
    { ITEM_GANLON_BERRY,    0, 0, 0, 3, 0 },    // AUTO DEFENSE
    { ITEM_SALAC_BERRY,     0, 0, 0, 3, 0 },    // AUTO SPEED
    { ITEM_PETAYA_BERRY,    0, 0, 0, 3, 0 },    // AUTO SP. ATK
    { ITEM_APICOT_BERRY,    0, 0, 0, 3, 0 },    // AUTO SP. DEF
    { ITEM_LANSAT_BERRY,    0, 0, 0, 3, 0 },    // AUTO CRIT
    { ITEM_STARF_BERRY,     0, 0, 0, 3, 0 },    // AUTO ANY
    //  held, acting in battle (once a battle)
    { ITEM_FOCUS_BAND,      2, 0, 0, 5, 0 },    // FAILSAFE: it held on
    { ITEM_WHITE_HERB,      0, 0, 0, 3, 0 },    // REVERT: its stats put back
    { ITEM_MENTAL_HERB,     0, 0, 0, 4, 0 },    // UNPAIR: let go of a pull
    { ITEM_SMOKE_BALL,      0, 0, 0, 2, 0 },    // ESCAPE HATCH: out, safely
};

static const struct ItemFeelings *FindItemFeelings(u16 item)
{
    u32 i;

    for (i = 0; i < ARRAY_COUNT(sItemFeelings); i++)
        if (sItemFeelings[i].item == item)
            return &sItemFeelings[i];
    return NULL;
}

// Friendship beyond vanilla's: half again under AFFINITY for a rise, as vanilla's own item changes are.
static void AddFriendship(struct Pokemon *mon, s8 delta)
{
    s32 friendship;

    if (delta == 0 || !IsPushable(&mon->box))
        return;
    friendship = GetMonData(mon, MON_DATA_FRIENDSHIP);
    if (delta > 0 && ItemId_GetHoldEffect(GetMonData(mon, MON_DATA_HELD_ITEM)) == HOLD_EFFECT_FRIENDSHIP_UP)
        friendship += 150 * delta / 100;
    else
        friendship += delta;
    if (friendship < 0)
        friendship = 0;
    if (friendship > MAX_FRIENDSHIP)
        friendship = MAX_FRIENDSHIP;
    SetMonData(mon, MON_DATA_FRIENDSHIP, &friendship);
}

// PokemonUseItemEffects worked on the player's own daemon (the AI's items never come here). The INTERRUPT in the
// field wakes your party through AWAKENING's effect, and pushes nothing: it plays for the sleeper, not for yours.
void Feelings_OnItemUsed(struct Pokemon *mon, u16 item)
{
    const struct ItemFeelings *row;

    if (!IsPlayerPartyMon(mon))
        return;
    if (item == ITEM_AWAKENING && gSpecialVar_ItemId == ITEM_POKE_FLUTE)
        return;
    row = FindItemFeelings(item);
    if (row == NULL)
        return;
    PushMon(mon, row->glad, row->mad, row->sad, row->calm);
    AddFriendship(mon, row->friendship);
}

// The battles whose player side is the player's own party, as Feelings_OnBattleEnd counts them.
static bool8 BattleIsOurs(void)
{
    return !(gBattleTypeFlags & (BATTLE_TYPE_LINK | BATTLE_TYPE_OLD_MAN_TUTORIAL | BATTLE_TYPE_POKEDUDE | BATTLE_TYPE_SAFARI));
}

// A held item did its work in battle: a berry or a herb eaten, a FAILSAFE held, an ESCAPE HATCH used. Once a battle
// for each daemon (gBattleStruct->feelingsHeldActed, a bit a party slot). A flavour TRAP is liked or hated by the
// daemon's nature, as vanilla reads it (a hated one confuses it); a flavour it neither likes nor hates pushes nothing.
void Feelings_OnHeldItemActed(u8 battler, u16 item)
{
    const struct ItemFeelings *row;
    u8 slot, holdEffect;
    s8 glad = 0, mad = 0, calm = 0;
    s8 relation;

    if (GetBattlerSide(battler) != B_SIDE_PLAYER || !BattleIsOurs() || item == ITEM_ENIGMA_BERRY)
        return;
    slot = gBattlerPartyIndexes[battler];
    if (slot >= PARTY_SIZE || (gBattleStruct->feelingsHeldActed & (1 << slot)))
        return;
    holdEffect = ItemId_GetHoldEffect(item);
    if (holdEffect >= HOLD_EFFECT_CONFUSE_SPICY && holdEffect <= HOLD_EFFECT_CONFUSE_SOUR)
    {
        relation = GetFlavorRelationByPersonality(gBattleMons[battler].personality, holdEffect - HOLD_EFFECT_CONFUSE_SPICY);
        if (relation > 0)
            glad = FEEL_FLAVOUR_LIKED_GLAD;
        else if (relation < 0)
            mad = FEEL_FLAVOUR_DISLIKED_MAD;
        else
            return;
    }
    else
    {
        row = FindItemFeelings(item);
        if (row == NULL)
            return;
        glad = row->glad;
        mad = row->mad;
        calm = row->calm;
    }
    gBattleStruct->feelingsHeldActed |= 1 << slot;
    Feelings_Push(&gPlayerParty[slot].box, glad, mad, 0, calm);
}

// The INTERRUPT, in battle, before it wakes anyone (VARIOUS_CHECK_POKEFLUTE): each daemon of yours asleep, in battle or
// not, and not SOUNDPROOF (it does not hear it, and sleeps on), is woken with a jolt -- the companion's "woken", halved.
void Feelings_OnPokeFluteInBattle(void)
{
    u8 i;
    u16 species;

    if (!BattleIsOurs())
        return;
    for (i = 0; i < PARTY_SIZE; i++)
    {
        species = GetMonData(&gPlayerParty[i], MON_DATA_SPECIES_OR_EGG);
        if (species == SPECIES_NONE || species == SPECIES_EGG
         || !(GetMonData(&gPlayerParty[i], MON_DATA_STATUS) & STATUS1_SLEEP)
         || GetAbilityBySpecies(species, GetMonData(&gPlayerParty[i], MON_DATA_ABILITY_NUM)) == ABILITY_SOUNDPROOF)
            continue;
        Feelings_Push(&gPlayerParty[i].box, 0, FEEL_INTERRUPT_MAD, 0, FEEL_INTERRUPT_CALM);
    }
}

static void PushParty(s8 glad, s8 mad, s8 sad, s8 calm)
{
    u8 i;

    for (i = 0; i < PARTY_SIZE; i++)
        Feelings_Push(&gPlayerParty[i].box, glad, mad, sad, calm);
}

// A REMNANT met (BattleIntroPrintWildMonAttacked). With the RESOLVER, DAEMON TOWER 6F's COREFILE is unveiled -- the
// one battle that plays "RESOLVER unveiled the REMNANT's identity!" -- and the whole party is afraid, once in a game.
// Without it, a masked REMNANT on any floor; in DOLDRUM CAVE before it is understood, the dream (T-254).
void Feelings_OnGhostBattleIntro(void)
{
    if (!(gBattleTypeFlags & BATTLE_TYPE_GHOST))
        return;
    if (IS_BATTLE_TYPE_GHOST_WITH_SCOPE(gBattleTypeFlags))
    {
        if (FlagGet(FLAG_FEELINGS_REMNANT_UNVEILED))
            return;
        FlagSet(FLAG_FEELINGS_REMNANT_UNVEILED);
        PushParty(0, 0, 0, FEEL_UNVEILED_CALM);
    }
    else if (DaemonsRemnantDream())
    {
        PushParty(0, 0, 0, FEEL_REMNANT_DREAM_CALM);
    }
    else
    {
        PushParty(0, 0, 0, FEEL_MASKED_CALM);
    }
}

// Special, DAEMON TOWER 6F: "the mother's spirit was calmed".
void Feelings_RemnantCalmed(void)
{
    PushParty(0, 0, FEEL_SPIRIT_CALMED_SAD, FEEL_SPIRIT_CALMED_CALM);
}

void Feelings_OnRoutineForgotten(struct Pokemon *mon)
{
    PushMon(mon, 0, 0, FEEL_TM_FORGOT_SAD, 0);
}

// Evolved: glad, and a little unsettled. An evolution a RARE CANDY's level set off still gets this -- growth
// happened; it was only hurried.
void Feelings_OnEvolved(struct Pokemon *mon)
{
    PushMon(mon, FEEL_EVOLVED_GLAD, 0, 0, FEEL_EVOLVED_CALM);
}

void Feelings_OnEvolutionStopped(struct Pokemon *mon)
{
    PushMon(mon, 0, FEEL_EVO_STOPPED_MAD, 0, 0);
}

// A daemon released from the PC (ReleaseMon, after it is gone): the party sees one go.
void Feelings_OnRelease(void)
{
    PushParty(0, 0, FEEL_RELEASED_SAD, 0);
}

// The HALL OF FAME, before its save: the whole party, glad and calm, and a little friendlier.
void Feelings_OnHallOfFame(void)
{
    u8 i;

    PushParty(FEEL_HOF_GLAD, 0, 0, FEEL_HOF_CALM);
    for (i = 0; i < PARTY_SIZE; i++)
        AddFriendship(&gPlayerParty[i], FEEL_HOF_FRIENDSHIP);
}

// An in-game trade's daemon, just put in the party (its condition bytes were the trade's; this is its first push).
void Feelings_OnTradeReceived(struct Pokemon *mon)
{
    PushMon(mon, 0, 0, FEEL_TRADED_SAD, FEEL_TRADED_CALM);
}

// The instrument (T-417), played to the lead: eight notes or more, or a song matched. At most once an hour --
// VAR_FEELINGS_PLAYED holds when, + 1 (0: never).
void Feelings_OnPlayed(void)
{
    bool8 fromClock;
    u16 now = Feelings_Now(&fromClock);
    u16 last = VarGet(VAR_FEELINGS_PLAYED);
    s8 slot;

    if (last != 0 && (u16)(now - (last - 1)) < FEEL_PLAYED_EVERY)
        return;
    slot = Follower_LeadSlot();
    if (slot < 0)
        return;
    VarSet(VAR_FEELINGS_PLAYED, now + 1);
    Feelings_Push(&gPlayerParty[slot].box, FEEL_PLAYED_GLAD, 0, 0, FEEL_PLAYED_CALM);
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
        else if (gBattleOutcome == B_OUTCOME_WON
              && ItemId_GetHoldEffect(GetMonData(&gPlayerParty[i], MON_DATA_HELD_ITEM)) == HOLD_EFFECT_EXP_SHARE
              && GetMonData(&gPlayerParty[i], MON_DATA_HP) != 0)
            Feelings_Push(&gPlayerParty[i].box, FEEL_WEIGHT_SHARE_GLAD, 0, 0, 0);   // T-433: the win it sat out
        else if (gBattleOutcome == B_OUTCOME_LOST)
            Feelings_Push(&gPlayerParty[i].box, 0, FEEL_LOST_MAD, FEEL_LOST_SAD, FEEL_LOST_CALM);
    }
    // T-433: a LEADER, one of the four or the champion beaten -- the whole party, on top of the win's own push.
    if (gBattleOutcome == B_OUTCOME_WON && (gBattleTypeFlags & BATTLE_TYPE_TRAINER)
     && !(gBattleTypeFlags & (BATTLE_TYPE_BATTLE_TOWER | BATTLE_TYPE_EREADER_TRAINER | BATTLE_TYPE_TRAINER_TOWER)))
    {
        switch (gTrainers[gTrainerBattleOpponent_A].trainerClass)
        {
        case TRAINER_CLASS_LEADER:
        case TRAINER_CLASS_ELITE_FOUR:
        case TRAINER_CLASS_CHAMPION:
            PushParty(FEEL_LEADER_GLAD, 0, 0, 0);
            break;
        }
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

// An item given to hold -- only when it was holding nothing (T-433), so swapping two back and forth earns nothing.
// GiveItemToMon (party_menu.c) checks.
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

// T-433: USE THE ITEM ON SLOT 1 -- gSpecialVar_ItemId (the theatre pokes it) used on the first party daemon as the
// party menu would (its first routine for an item that asks for one), so the route reads what the item pushes.
void Feelings_DebugUseItem(void)
{
    ExecuteTableBasedItemEffect(&gPlayerParty[0], gSpecialVar_ItemId, 0, 0);
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
