#ifndef GUARD_DAEMONS_FEELINGS_H
#define GUARD_DAEMONS_FEELINGS_H

// T-431 P1 (planned 2026-10-21, docs/private/plan-feelings-2026-10-21.md; bible 4.9): FEELINGS, never named.
//
// Each daemon carries GLAD, MAD and SAD (0..100) and CALM (-100 calm .. 100, AFRAID below 0) in the six contest-
// condition bytes FireRed never uses (PokemonSubstruct2: cool, beauty, cute, smart; tough + sheen a u16 stamp in
// 10-minute ticks), inside its encrypted, checksummed record -- 0 B of save, 0 B of RAM. unusedRibbons bit 0 says the
// values are ours (an old save's daemon, or an in-game trade's, reads as zero until its first push); bit 1 says which
// clock stamped them (the cartridge's, or play time's).
//
// They fade as the companion's do (C-120): half gone in 3 hours (18 ticks), nothing left after a day. A push stores
// the faded values plus the push, clamped, and stamps them now. Friendship is the fifth value, and stays where it is.
// Eggs and AWAY daemons are never pushed. P2 draws them on the SUMMARY's fourth page (below), P3 makes the emoji.

#define FEELINGS_TICK_MINUTES   10
#define FEELINGS_HALF_LIFE      18      // ticks: 3 hours
#define FEELINGS_LOOK_BACK      144     // ticks: a day, after which a push is gone
#define FEELINGS_MAX            100
#define FEELINGS_CALM_MIN       (-100)

#define FEELINGS_RIBBON_INIT    (1 << 0)
#define FEELINGS_RIBBON_CLOCK   (1 << 1)

// What each event pushes: GLAD, MAD, SAD, CALM. ALL DRAFT (the plan's section 2, and the user's answers in section 8:
// MAD only from losses and hard faints; a small sad push back from a box).
#define FEEL_WON_GLAD           8       // a battle won: each daemon that took part
#define FEEL_LOST_SAD           12      // a battle lost (a white-out): the whole party
#define FEEL_LOST_CALM          (-10)
#define FEEL_LOST_MAD           6       // DRAFT, and a guess at the user's "MAD from losses": the plan gave a loss none
#define FEEL_FAINT_SAD          10      // fainted in battle
#define FEEL_FAINT_CALM         (-15)
#define FEEL_FAINT_HARD_MAD     6       // ... to a foe 30 or more levels above it (vanilla's FAINT_LARGE)
#define FEEL_POISON_SAD         12      // fainted from poison, walking
#define FEEL_POISON_CALM        (-10)
#define FEEL_HEAL_CALM          20      // the CHECKPOINT's machine, chosen (never after a white-out)
#define FEEL_WALK_GLAD          3       // every 128 steps, the daemon that follows
#define FEEL_WALK_CALM          2
#define FEEL_LEVEL_GLAD         6       // a level earned in battle (T-433: a candy's level earns none)
#define FEEL_VITAMIN_GLAD       5
#define FEEL_BATTLE_ITEM_GLAD   3
#define FEEL_TM_GLAD            3       // taught a routine from a TM or a DRIVER
#define FEEL_MASSAGE_GLAD       12
#define FEEL_MASSAGE_CALM       10
#define FEEL_HELD_ITEM_GLAD     5
#define FEEL_BOX_HOURS_SAD      6       // back from a box after 1 to 24 hours
#define FEEL_BOX_DAY_SAD        10      // back after more than a day
#define FEEL_BOX_DAY_CALM       (-5)
#define FEEL_BOX_HOURS_FROM     6       // ticks: an hour, under which coming back pushes nothing

// T-433 (planned and answered 2026-10-21, docs/private/plan-items-feelings-2026-10-21.md; bible 4.9): items and moments.
// What each item pushes is the table sItemFeelings in src/daemons_feelings.c; these are the moments. ALL DRAFT.
#define FEEL_FLAVOUR_LIKED_GLAD     2   // a flavour TRAP eaten in battle that it likes...
#define FEEL_FLAVOUR_DISLIKED_MAD   3   // ... or hates (vanilla confuses it; now it minds)
#define FEEL_TM_FORGOT_SAD          2   // a routine forgotten to make room for a PLUGIN's
#define FEEL_INTERRUPT_MAD          4   // woken by the INTERRUPT in battle: each sleeping daemon of yours
#define FEEL_INTERRUPT_CALM         (-4)
#define FEEL_UNVEILED_CALM          (-12)   // the RESOLVER unveils the REMNANT (DAEMON TOWER 6F): the party, once a game
#define FEEL_MASKED_CALM            (-4)    // a masked REMNANT met without the RESOLVER: the party
#define FEEL_REMNANT_DREAM_CALM     (-6)    // the REMNANT dream in DOLDRUM CAVE (T-254): the party
#define FEEL_SPIRIT_CALMED_CALM     10      // the REMNANT's spirit calmed (6F): the party
#define FEEL_SPIRIT_CALMED_SAD      4
#define FEEL_LEADER_GLAD            10      // a LEADER, the four or the champion beaten: the party (on top of a win's)
#define FEEL_EVOLVED_GLAD           8
#define FEEL_EVOLVED_CALM           (-5)
#define FEEL_EVO_STOPPED_MAD        3       // an evolution stopped with B
#define FEEL_RELEASED_SAD           6       // a partner released: the rest of the party
#define FEEL_HOF_GLAD               15      // the HALL OF FAME: the party
#define FEEL_HOF_CALM               10
#define FEEL_HOF_FRIENDSHIP         2
#define FEEL_TRADED_CALM            (-15)   // arriving by an in-game trade: a stranger's hands
#define FEEL_TRADED_SAD             4
#define FEEL_PLAYED_GLAD            3       // your instrument played to the lead (eight notes, or a song matched)...
#define FEEL_PLAYED_CALM            4
#define FEEL_PLAYED_EVERY           6       // ... at most once in this many ticks (an hour)
#define FEEL_PLAYED_NOTES           8       // notes in one playing that count, without a song
#define FEEL_WEIGHT_SHARE_GLAD      3       // WEIGHT SHARE held through a win it sat out

enum { FEEL_GLAD, FEEL_MAD, FEEL_SAD, FEEL_CALM, FEEL_COUNT };

struct DaemonFeelings
{
    u8 glad;
    u8 mad;
    u8 sad;
    s8 calm;
};

struct Pokemon;
struct BoxPokemon;

// Now, faded. A daemon never pushed (an old save, a trade, an egg) reads as zero.
void Feelings_Get(struct BoxPokemon *boxMon, struct DaemonFeelings *out);
bool8 Feelings_IsSet(struct BoxPokemon *boxMon);
// A push: skipped for an empty slot, an egg or an AWAY daemon.
void Feelings_Push(struct BoxPokemon *boxMon, s8 glad, s8 mad, s8 sad, s8 calm);
// The clock, in ticks, and which one it is (DaemonsFeelingsNow, src/daemons_time.c).
u16 Feelings_Now(bool8 *fromClock);

// The events (each a no-op on a daemon that is not the player's own).
void Feelings_OnFriendshipEvent(struct Pokemon *mon, u8 event);   // the end of AdjustFriendship
void Feelings_OnItemUsed(struct Pokemon *mon, u16 item);           // T-433: the end of PokemonUseItemEffects, when it worked
void Feelings_OnBattleEnd(void);                                   // HandleEndTurn_FinishBattle
void Feelings_OnWalk(void);                                        // every 128 steps
void Feelings_OnHeldItemGiven(struct Pokemon *mon);
void Feelings_OnDeposit(struct BoxPokemon *boxMon);                // into a box: restamped
void Feelings_OnWithdraw(struct Pokemon *mon);                     // out of one: a small sad push, by how long
void Feelings_CheckpointHeal(void);                                // special, after HealPlayerParty at a CHECKPOINT

// T-433: the moments (each a no-op outside the player's own party, and on an egg or an AWAY daemon).
void Feelings_OnHeldItemActed(u8 battler, u16 item);               // a held item did its work in battle (once a battle)
void Feelings_OnPokeFluteInBattle(void);                           // the INTERRUPT, before it wakes anyone
void Feelings_OnGhostBattleIntro(void);                            // a REMNANT met: unveiled, masked, or the dream
void Feelings_RemnantCalmed(void);                                 // special: DAEMON TOWER 6F, the spirit calmed
void Feelings_OnRoutineForgotten(struct Pokemon *mon);             // a PLUGIN's routine took an old one's place
void Feelings_OnEvolved(struct Pokemon *mon);
void Feelings_OnEvolutionStopped(struct Pokemon *mon);
void Feelings_OnRelease(void);                                     // a daemon released from the PC
void Feelings_OnHallOfFame(void);
void Feelings_OnTradeReceived(struct Pokemon *mon);                // an in-game trade's daemon, just arrived
void Feelings_OnPlayed(void);                                      // the instrument, played to the lead

// T-431 P2 and P4: what the SUMMARY's fourth page shows (docs/private/plan-feelings-2026-10-21.md sections 3 and 5).
#define FEELINGS_GRID           9       // cells a side
#define FEELINGS_LEAN           20      // GLAD - SAD or CALM this far from 0 leaves the middle of the grid's 3x3 reading
#define FEELINGS_MAD_LINE       40      // MAD this high speaks first
#define FEELINGS_QUIET          20      // every value under this is "all near zero"
#define FEELINGS_FRIEND_LOW     100     // the plan's friendship variants: under this the wary line...
#define FEELINGS_FRIEND_HIGH    150     // ... at or over this the close one (between them: the nearer, split at 125)
#define FEELINGS_FRIEND_EVER    220     // all near zero and this friendly: "It goes where you go, as ever."

// The dot's cell, 0..8 each way: across by GLAD - SAD (right is glad), down from CALM (row 0 calm, row 8 afraid).
void Feelings_GridCell(const struct DaemonFeelings *f, u8 *col, u8 *row);
// The dot's colour: its hue from MAD (a cool slate at 0 to an ember at 100), then greyed by friendship as the
// daemon's own picture is (T-424).
u16 Feelings_DotColour(u8 mad, u8 friendship);
// The line of behaviour under the grid -- one of the sixteen approved lines (DRAFT in the game). A daemon never
// stamped reads the quiet line for its friendship.
const u8 *Feelings_BehaviourLine(struct BoxPokemon *boxMon, u8 friendship);

#if DAEMONS_DEBUG
void Feelings_DebugAge(u16 ticks);                                 // every stamp, party and boxes, made older
void Feelings_DebugPushLead(u8 which);                             // FEEL_GLAD, MAD, SAD, or FEEL_CALM for AFRAID
void Feelings_DebugUseItem(void);                                  // T-433: gSpecialVar_ItemId used on party slot 1
// What the SUMMARY's fourth page last drew, for the theatre (tools/theatre_route.py's `summary`): [0] the line's
// address, [1] its cell (col | row << 8) and the dot's colour << 16. 8 bytes, the DEBUG build only.
extern u32 gFeelingsDebugShown[2];
#endif

#endif // GUARD_DAEMONS_FEELINGS_H
