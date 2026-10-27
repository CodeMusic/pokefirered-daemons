#ifndef GUARD_DAEMONS_THREE_H
#define GUARD_DAEMONS_THREE_H

// T-454 (the user, 2026-10-18: "per daemon, battle only"; filed 2026-10-24): THE SUBTLE LAYER's THREE -- CLOSURE (how
// whole a daemon is), ATTUNEMENT (how thin its seam) and VOLTAGE (feeling in the moment) -- tuning CONTEXT routines in
// battle, and NEVER making a read come true in the world (bible 4.33): nothing outside a battle reads them.
//
// WHERE THEY LIVE: nowhere saved. Each battle derives them, per party daemon, from what the daemon already carries --
// its friendship and T-431's feelings (src/daemons_feelings.c) -- and keeps them in gBattleStruct's tail padding
// (include/battle.h, 19 of its 28 spare bytes), which is on the heap and freed with the battle: 0 B of save, 0 B of
// EWRAM or IWRAM. In the battle they move with what happens in it, and are gone when it ends. A daemon "carries them as
// numbers it cannot see" because what it carries is the feelings and the bond they are read from.
//
// The book's formula is the spec (docs/subtle-layer.md THE THREE): reads rise with ATTUNEMENT and fall with VOLTAGE;
// writes need ATTUNEMENT and VOLTAGE at once, and only while CLOSURE holds; a flooded system with low CLOSURE does not
// write, it leaks. In battle: a READ is how well a CONTEXT routine finds its mark (its accuracy); a WRITE is how hard it
// lands (its damage). Only the player's own daemons carry them in effect -- a foe's routines are vanilla -- and never in
// a link battle (each cartridge would read its own player's feelings, and the two would disagree), the POKEDUDE's or
// the old man's. And only once the player holds THE SUBTLE LAYER (FLAG_GOT_SUBTLE_LAYER; the user, 2026-10-27):
// before the book, every battle is vanilla's.

// ---- THE TABLE (approved 2026-10-27): every number the user tunes, in one place ------------------------------------------------
// Each value is a level, 0..THREE_MAX.
#define THREE_MAX                   4

// CLOSURE at the battle's start, from friendship (the bond is the daemon whole with you) ...
#define THREE_CLOSURE_FRIEND_1      50      // friendship under this: 1
#define THREE_CLOSURE_FRIEND_3      150     // 50..149: 2 (a daemon just caught is 70); this and over: 3
#define THREE_CLOSURE_FRIEND_4      220     // this and over: 4
// ... and the shadow over the virtue: MAD at FEELINGS_MAD_LINE (40) or more takes one away.
#define THREE_CLOSURE_MAD_LINE      40
// In the battle, one away while it is confused and one while it is infatuated (two points of view), back when they end.

// ATTUNEMENT at the battle's start, from CALM (-100 afraid .. 100 calm): 2, and one more at each of the first two
// lines, one less at each of the last two.
#define THREE_ATTUNE_CALM_3         25
#define THREE_ATTUNE_CALM_4         60
#define THREE_ATTUNE_CALM_1         (-15)
#define THREE_ATTUNE_CALM_0         (-50)
// In the battle, stillness: each routine of no power it uses (REST among them, and every CONTEXT status routine).
#define THREE_ATTUNE_STILL          1

// VOLTAGE at the battle's start, from the strongest feeling it holds now -- GLAD, MAD, SAD, or AFRAID (CALM below 0):
#define THREE_VOLTAGE_FEEL_2        20      // under this (all quiet): 1
#define THREE_VOLTAGE_FEEL_3        45
#define THREE_VOLTAGE_FEEL_4        70
// In the battle, joy and grief: a foe it fells, each of its party fallen, and a critical hit taken.
#define THREE_VOLTAGE_JOY           1
#define THREE_VOLTAGE_GRIEF         1
#define THREE_VOLTAGE_JOLT          1

// READ = ATTUNEMENT - VOLTAGE (-4..+4). A CONTEXT routine's accuracy, in percent of itself, per step:
#define THREE_READ_UP               3       // up to +12%
#define THREE_READ_DOWN             2       // down to -8%
// WRITE: while CLOSURE is at least THREE_CLOSURE_HOLDS, damage + (ATTUNEMENT x VOLTAGE - THREE_WRITE_FROM) percent,
// at most THREE_WRITE_MAX: 3 and 3 is +5%, 4 and 3 +8%, 4 and 4 +10%. Nothing below.
#define THREE_CLOSURE_HOLDS         2
#define THREE_WRITE_FROM            4
#define THREE_WRITE_MAX             10
// LEAK: CLOSURE below THREE_CLOSURE_HOLDS with VOLTAGE at least THREE_LEAK_VOLTAGE: damage - (VOLTAGE - 2) x
// THREE_LEAK_STEP percent (3: -5%, 4: -10%).
#define THREE_LEAK_VOLTAGE          3
#define THREE_LEAK_STEP             5
// ----------------------------------------------------------------------------------------------------------------

enum { THREE_CLOSURE, THREE_ATTUNEMENT, THREE_VOLTAGE, THREE_COUNT };

struct BoxPokemon;

// The battle: each a no-op outside one, for a foe's daemon, and in a battle that does not carry them.
void Three_BattleStart(void);                                   // AllocateBattleResources, gBattleTypeFlags set
bool8 Three_Get(u8 battler, u8 *out);                           // THREE_COUNT values, CLOSURE as it is this turn
s8 Three_ReadScore(u8 battler);                                 // ATTUNEMENT - VOLTAGE, 0 if none (T-459 C's lean)
u16 Three_AdjustAccuracy(u8 attacker, u8 moveType, u16 calc);   // Cmd_accuracycheck
s32 Three_AdjustDamage(u8 attacker, u8 moveType, s32 damage);   // Cmd_damagecalc, and FUTURE SIGHT's
void Three_OnMoveUsed(u8 attacker, u16 move);                   // Cmd_ppreduce, when the PP is spent
void Three_OnHit(u8 target, u8 critMultiplier);                 // Cmd_damagecalc
void Three_OnFaint(u8 fainted, u8 attacker);                    // Cmd_tryfaintmon

#if DAEMONS_DEBUG
// DEBUG -> GAME EVENTS -> THE THREE: a party slot's three as the next battle would start them ("!" before one set
// here), the slot picked, and each value set by hand (AUTO, then 0..4, then AUTO again) for the next battles.
void Three_DebugLabel(u8 *label, u8 slot);
void Three_DebugPick(u8 slot);
u8 Three_DebugPicked(void);
void Three_DebugCycle(u8 which);
void Three_DebugAllAuto(void);
#endif

#endif // GUARD_DAEMONS_THREE_H
