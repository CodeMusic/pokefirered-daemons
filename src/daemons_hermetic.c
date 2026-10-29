#include "global.h"
#include "battle_main.h"
#include "event_data.h"
#include "event_scripts.h"
#include "item.h"
#include "pokemon.h"
#include "string_util.h"
#include "constants/pokemon.h"
#include "daemons_time.h"
#include "daemons_holidays.h"
#include "daemons_hermetic.h"
#include "constants/flags.h"
#include "constants/items.h"
#include "constants/species.h"

// T-483: THE HERMETIC LAYER, the game's half, first part (the user, 2026-10-29, by multiple choice; bible 4.39).
//
// THE BOOK is a key item (HERMETIC), read in the same open book as THE SUBTLE LAYER (book_reader.c, BOOK_HERMETIC),
// its pages written by DAEMONS tools/gensubtle.py from docs/hermetic-layer.md. It is READ ONLY AT NIGHT -- the
// game's night watch, whichever clock is keeping it (Hermetic_IsNight); by day the bag says so and it stays shut.
//
// THE UNLOCK, two ways, one flag (FLAG_GOT_HERMETIC_LAYER):
//   - in the game, the first time a daemon uses its witching-hour routine (T-480) while the save holds the GUIDE and
//     THE SUBTLE LAYER (Hermetic_OnWitchingRoutine, from daemons_witching.c's Witching_PpReduce);
//   - from the companion, which writes the flag at a SYNC (C-238).
// Either way only the flag is set; the book itself arrives in the field (Hermetic_Arrival, once a second, from
// overworld.c), with one DRAFT line -- so the book appears however the flag came to be set, and only once.
//
// THE PULSE (src/daemons_pulse.c): with the book held, from 3:00 to 4:00 AM on the clock (DaemonsIsWitchingHour) the
// map breathes red to blue through purple, and all of Halloween too. THE HANDLESS CLOCK in the same window: the
// game's one clock, the terminal's CLOCK code, gives the date and no time.
//
// HALLOWEEN'S COSTUME: with the book held, on 31 October the follower wears the face of a daemon whose first type
// beats its own first type -- what it fears, read from the chart itself (gTypeEffectiveness, super effective only;
// the chart is never touched). The same choice as the companion's server/src/halloween.ts costumeOf: the types that
// beat it in name order, then an FNV-1a hash of "YYYY-MM-DD|<species>|costume" picks the type, and its high bits pick
// a daemon of that first type (the same one all day, for the same daemon). A on the follower takes the mask off for
// the rest of the day (FLAG_HERMETIC_UNMASKED, cleared the first time it is looked at on any other day).

bool8 Hermetic_Held(void)
{
    return FlagGet(FLAG_GOT_HERMETIC_LAYER);
}

bool8 Hermetic_IsHalloween(void)
{
    return DaemonsIsCalendarDay(10, 31);
}

bool8 Hermetic_WorldPulses(void)
{
    return Hermetic_Held() && (DaemonsIsWitchingHour() || Hermetic_IsHalloween());
}

bool8 Hermetic_IsNight(void)
{
    return DaemonsWatch() == WATCH_NIGHT;
}

void Hermetic_OnWitchingRoutine(void)
{
    if (!Hermetic_Held() && FlagGet(FLAG_GOT_GUIDE) && FlagGet(FLAG_GOT_SUBTLE_LAYER))
        FlagSet(FLAG_GOT_HERMETIC_LAYER);
}

const u8 *Hermetic_Arrival(void)
{
    if (!Hermetic_Held() || CheckBagHasItem(ITEM_HERMETIC_LAYER, 1))
        return NULL;
    if (!AddBagItem(ITEM_HERMETIC_LAYER, 1))
        return NULL;    // no room: asked again a second later
    return EventScript_HermeticArrives;
}
