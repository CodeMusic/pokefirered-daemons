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
// Eggs and AWAY daemons are never pushed. Nothing here is shown to the player: P2 draws them, P3 makes the emoji.

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
#define FEEL_LEVEL_GLAD         6       // a level, in battle or from a candy
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
void Feelings_OnItemFriendship(struct Pokemon *mon, u16 item);     // a vitamin, a battle item, a candy
void Feelings_OnBattleEnd(void);                                   // HandleEndTurn_FinishBattle
void Feelings_OnWalk(void);                                        // every 128 steps
void Feelings_OnHeldItemGiven(struct Pokemon *mon);
void Feelings_OnDeposit(struct BoxPokemon *boxMon);                // into a box: restamped
void Feelings_OnWithdraw(struct Pokemon *mon);                     // out of one: a small sad push, by how long
void Feelings_CheckpointHeal(void);                                // special, after HealPlayerParty at a CHECKPOINT

#if DAEMONS_DEBUG
void Feelings_DebugAge(u16 ticks);                                 // every stamp, party and boxes, made older
void Feelings_DebugPushLead(u8 which);                             // FEEL_GLAD, MAD, SAD, or FEEL_CALM for AFRAID
#endif

#endif // GUARD_DAEMONS_FEELINGS_H
