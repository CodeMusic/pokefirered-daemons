#ifndef GUARD_DAEMONS_HERMETIC_H
#define GUARD_DAEMONS_HERMETIC_H

// T-483: THE HERMETIC LAYER in the game -- the book's gates, its unlock, the world's pulse, the handless clock and
// Halloween's costume (src/daemons_hermetic.c). The book itself is read in book_reader.c.

bool8 Hermetic_Held(void);
bool8 Hermetic_IsHalloween(void);       // 31 October, by the calendar the holidays read (MASK NIGHT)
bool8 Hermetic_WorldPulses(void);       // the book held, and 3-4 AM on the clock or all Halloween
bool8 Hermetic_IsNight(void);           // specialvar: the book is read only at night

void Hermetic_OnWitchingRoutine(void);  // a daemon has used its hour's routine (daemons_witching.c)
const u8 *Hermetic_Arrival(void);       // once a second in the field: the book arrives in the bag, once


#endif // GUARD_DAEMONS_HERMETIC_H
