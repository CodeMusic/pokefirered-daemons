#ifndef GUARD_DAEMONS_SPIRITS_H
#define GUARD_DAEMONS_SPIRITS_H

//  T-519 item 1: SPIRITS (vision.md 4.40, DRAFT). Every daemon drifts, slowly, between three classes -- in good
//  spirits, as it is, in low spirits -- held in the last four free bits of the u16 at box-mon 0x1E (include/pokemon.h:
//  daemonsSpirits, bits 12-13, and daemonsLean, bits 14-15). The game shows only the axis words; the companion reads
//  the class and its lean. Everything here is in src/daemons_spirits.c, which says what moves it and by how much.

enum {
    SPIRITS_AS_IT_IS,       // 0: every daemon starts here, and an old save's read as it (the bits were zero)
    SPIRITS_GOOD,           // 1: in good spirits
    SPIRITS_LOW,            // 2: in low spirits
                            // 3: never written; read as SPIRITS_AS_IT_IS
};

enum {
    SPIRITS_TONE_NONE,
    SPIRITS_TONE_LIGHT,     // good spirits: lighter over its type's colours
    SPIRITS_TONE_DARK,      // low spirits: darker over them
};

u8 Spirits_Class(const struct BoxPokemon *boxMon);
u8 Spirits_Lean(const struct BoxPokemon *boxMon);
u8 Spirits_ToneOf(const struct BoxPokemon *boxMon);
const u8 *Spirits_Words(const struct BoxPokemon *boxMon);   // "In good spirits." / "In low spirits." / NULL

//  Tone, never hue: each colour moves toward white or black, the same share of the way in each channel.
void Spirits_ToneBuffer(u16 *palette, u8 tone);                 // a 16-colour palette before it is loaded
void Spirits_ToneLoaded(u16 paletteOffset, u8 tone);            // one already loaded (both buffers)

//  What moves it (each a chance of one step; src/daemons_spirits.c has the weights).
void Spirits_OnHeal(void);                                      // KEPT: a CHECKPOINT, a bed (HealPlayerParty)
void Spirits_OnWalk(void);                                      // every 128 steps: left fainted, BOND, long in a box
void Spirits_OnFriendshipEvent(struct Pokemon *mon, u8 event);  // KEPT (a vitamin, the massage, a field faint), WORK (a leader faced)
void Spirits_OnOwnType(u8 battler);                             // WORK: a routine of its own type
void Spirits_OnFairFight(struct Pokemon *mon, u8 foeLevel);     // PORTION: won against a foe near its own level
void Spirits_OnFaint(struct Pokemon *mon, u8 foeLevel, u8 ownLevel);   // PORTION: sent against far worse odds
void Spirits_OnFed(struct Pokemon *mon);                        // KEPT: an item that restores HP
void Spirits_OnRareCandy(struct Pokemon *mon);                  // WORK, against: a level it did not earn

//  How it behaves (outside battle only): one in low spirits may refuse its food, one in good spirits thanks you.
bool8 Spirits_RefusesFood(struct Pokemon *mon);     // asked only for an item that restores HP
bool8 Spirits_Thanks(struct Pokemon *mon);
extern const u8 gText_SpiritsRefusesFood[];
extern const u8 gText_SpiritsHPRestoredThanks[];

//  The crossing: the first step after one, its screen (field_control_avatar.c; data/scripts/daemons_spirits.inc).
bool8 Daemons_TrySpiritsCrossing(void);     // a party daemon crossed: its line in gStringVar4
void Spirits_PlayCrossing(void);            // special, then waitstate: the screen, on the black of a fadescreen

#if DAEMONS_DEBUG
void Spirits_DebugLabel(u8 *label, u8 slot);
void Spirits_DebugStepLead(bool8 up);       // one step, now; a crossing plays on the next step
void Spirits_DebugSetLead(u8 class);        // straight to a class (lean 0), with its crossing
void Spirits_DebugToggleSure(void);
bool8 Spirits_DebugSure(void);              // the food's chances are certainties, to film it
#endif

#endif // GUARD_DAEMONS_SPIRITS_H
