#ifndef GUARD_DAEMONS_HERMETIC_H
#define GUARD_DAEMONS_HERMETIC_H

// T-483: THE HERMETIC LAYER in the game -- the book's gates, its unlock, the world's pulse, the handless clock and
// Halloween's costume (src/daemons_hermetic.c). The book itself is read in book_reader.c.

bool8 Hermetic_Held(void);
bool8 Hermetic_IsHalloween(void);       // 31 October, by the calendar the holidays read (MASK NIGHT)
bool8 Hermetic_WorldPulses(void);       // the book held, and 3-4 AM on the clock or all Halloween
bool8 Hermetic_BattlePulses(void);      // T-482: the hour with THE SUBTLE LAYER held, or the world's pulse
bool8 Hermetic_HandlessClock(void);     // every clock face without its hands
bool8 Hermetic_IsNight(void);           // specialvar: the book is read only at night

void Hermetic_OnWitchingRoutine(void);  // a daemon has used its hour's routine (daemons_witching.c)
const u8 *Hermetic_Arrival(void);       // once a second in the field: the book arrives in the bag, once

// Halloween's costume: the species the follower goes as today, or SPECIES_NONE (no book, not Halloween, unmasked,
// or nothing beats its first type).
u16 Hermetic_Costume(u16 species);
void Hermetic_Unmask(void);

// T-483 (7): Halloween with the book -- trick or treat, and the Book of Thirteen's card of the day (DRAFT)
void Hermetic_TrickOrTreat(void);       // at a battle's end: a treat waits for the field (Hermetic_Arrival)
#define THIRTEEN_CARDS 14
struct ThirteenCard { u8 n[6]; u8 keyword[11]; u8 phrase[40]; u8 species[11]; };
const struct ThirteenCard *Hermetic_CardOfDay(void);

#endif // GUARD_DAEMONS_HERMETIC_H
