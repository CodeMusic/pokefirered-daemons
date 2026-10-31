#ifndef GUARD_DAEMONS_SPELLBOOK_H
#define GUARD_DAEMONS_SPELLBOOK_H

// T-483 (5): THE SPELLBOOK -- the Book of Thirteen's spells, cast from the START menu at night with THE HERMETIC
// LAYER held (src/daemons_spellbook.c). Part two: the gentle ones, which change only what is seen, or the clock's
// light: THE AWAKENING, THE CONTRAST, THE PATTERN, THE LENS and THE FIELD. Every word DRAFT.

// The thirteen, and the half, in the book's order. A spell's bit in VAR_SPELLS_KNOWN and VAR_SPELLS_CAST is its value.
enum
{
    SPELL_AWAKENING,    // 1
    SPELL_CONTRAST,     // 2
    SPELL_ROOM,         // 3
    SPELL_PULL,         // 3 1/2
    SPELL_MIRROR,       // 4
    SPELL_PATTERN,      // 5
    SPELL_RETURN,       // 6
    SPELL_SMALL_HAND,   // 7
    SPELL_GREAT_HAND,   // 8
    SPELL_PROCESS,      // 9
    SPELL_SEAM,         // 10
    SPELL_LENS,         // 11
    SPELL_FIELD,        // 12
    SPELL_WITCHING,     // 13
    SPELL_COUNT
};

#define SPELLBOOK_CANCEL 127    // VAR_RESULT when the list is closed with B (data/scripts/spellbook.inc says 127)

// The ledger's four counters (never shown): every cast adds to the two its spell leans on.
enum { LEDGER_REASON, LEDGER_INTUITION, LEDGER_CONTENT, LEDGER_CONTEXT, LEDGER_COUNT };
u8 Spellbook_Ledger(u8 axis);

bool8 Spellbook_InStartMenu(void);          // the START menu shows SPELLBOOK: THE HERMETIC LAYER is held
void Spellbook_Choose(void);                // special (waitstate): the list; VAR_RESULT the spell, or SPELLBOOK_CANCEL
void Spellbook_Cast(void);                  // special: casts VAR_RESULT's spell; what it did is left in gStringVar4
bool8 Spellbook_TakeNews(void);             // specialvar: a spell came off the page while the book was open
void Spellbook_OnPassageRead(const u8 *shortTitle);   // book_reader.c: a page of THE HERMETIC LAYER read to its end

// THE AWAKENING: REVEAL's shimmer on this map until the next one loads.
bool8 Spellbook_Awakened(void);
void Spellbook_OnMapLoad(void);

// THE CONTRAST: the watch the world is seen in, given the clock's own (daemons_time.c DaemonsWatch).
u8 Spellbook_ContrastWatch(u8 trueWatch);

// THE LENS, THE FIELD and THE PATTERN: one of them is set on the next single battle, and lasts that battle.
void Spellbook_BattleStart(void);           // battle_util2.c, as the battle's resources are made
bool8 Spellbook_LensOn(void);
bool8 Spellbook_FieldOn(void);
bool8 Spellbook_PatternOn(void);
bool8 Spellbook_LeanHushed(void);           // THE LENS and THE PATTERN are reason's: the lean (T-459 C) says nothing
bool8 Spellbook_PatternWanted(u8 battler);  // the menu should wait for the foe's choice, to name it
bool8 Spellbook_PatternReady(void);         // ... and it has come, or will not
const u8 *Spellbook_PromptText(void);       // THE PATTERN's or THE LENS's line where "What will X do?" stood, or NULL
bool8 Spellbook_LensMoveText(u16 move, u8 *effect, u8 *hit);   // THE LENS in the routine menu: x2, and 95%

// THE PULL and THE GREAT HAND (part three): the wanderer drawn, the calm, the road and its lightness.
void Spellbook_Draw(void);                  // special (waitstate), after every cast: THE PULL's step, if it drew one
u32 Spellbook_EncounterRate(u32 rate);      // wild_encounter.c: heavier after THE PULL, lighter while carried
s32 Spellbook_PullDamage(u8 attacker, s32 damage);   // Cmd_damagecalc: the calmed foe's first turn (or two), halved
bool8 Spellbook_HandOn(void);               // the exp: THE GREAT HAND carries this battle, a tenth more

#if DAEMONS_DEBUG
void Spellbook_DebugAll(bool8 on);          // DEBUG -> GAME EVENTS: every spell known (or none)
#endif

#endif // GUARD_DAEMONS_SPELLBOOK_H
