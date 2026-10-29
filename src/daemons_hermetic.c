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
// beat it in name order, then the FNV-1a hash of "YYYY-MM-DD|<species>|costume" picks the type, and its high bits pick
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

// T-482: in battle the ground breathes in the hour once THE SUBTLE LAYER is held, as the hour's routine is offered;
// and whenever the world itself is pulsing.
bool8 Hermetic_BattlePulses(void)
{
    return (DaemonsIsWitchingHour() && FlagGet(FLAG_GOT_SUBTLE_LAYER)) || Hermetic_WorldPulses();
}

bool8 Hermetic_HandlessClock(void)
{
    return Hermetic_WorldPulses();
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

//  ---------------------------------------------------------------- Halloween's costume

static u32 Fnv(const char *s)
{
    u32 h = 2166136261u;

    for (; *s != 0; s++)
        h = (h ^ (u8)*s) * 16777619u;
    return h;
}

static char *Digits(char *p, u32 n, u8 width)
{
    char buf[10];
    u8 k = 0;

    do
    {
        buf[k++] = '0' + n % 10;
        n /= 10;
    } while (n != 0 && k < 10);
    while (k < width)
        buf[k++] = '0';
    while (k != 0)
        *p++ = buf[--k];
    return p;
}

// The types whose strike on TYPE is super effective, as a bit set (the chart, after FORESIGHT's rows too).
static u32 FearsOf(u8 type)
{
    u32 set = 0;
    u16 i;

    for (i = 0; i < sizeof(gTypeEffectiveness) && TYPE_EFFECT_ATK_TYPE(i) != TYPE_ENDTABLE; i += 3)
    {
        if (TYPE_EFFECT_ATK_TYPE(i) == TYPE_FORESIGHT)
            continue;
        if (TYPE_EFFECT_DEF_TYPE(i) == type && TYPE_EFFECT_MULTIPLIER(i) == TYPE_MUL_SUPER_EFFECTIVE
         && TYPE_EFFECT_ATK_TYPE(i) < NUMBER_OF_MON_TYPES)
            set |= 1u << TYPE_EFFECT_ATK_TYPE(i);
    }
    return set;
}

// The K-th type of SET in name order (the companion's fears.json is sorted by name).
static u8 NthByName(u32 set, u8 k)
{
    u8 t, best, n;
    u32 left = set;

    for (n = 0; left != 0; n++)
    {
        best = 0xFF;
        for (t = 0; t < NUMBER_OF_MON_TYPES; t++)
            if ((left & (1u << t)) && (best == 0xFF || StringCompare(gTypeNames[t], gTypeNames[best]) < 0))
                best = t;
        if (n == k)
            return best;
        left &= ~(1u << best);
    }
    return TYPE_NONE;
}

static bool8 CostumeCandidate(u16 s, u8 type, u16 own)
{
    // The companion's costumeOf skips only the empty slots, the ones with no picture (OLD_UNOWN_N..Z, 264-276):
    // MISSINGNO, REFLECTION, HANDSHAKE and the rest of 252-263 are daemons and may be worn.
    if (s == own || (s >= SPECIES_OLD_UNOWN_N && s <= SPECIES_OLD_UNOWN_Z))
        return FALSE;
    return gSpeciesInfo[s].types[0] == type;
}

static const char sCostumeTail[] = "|costume";

static u16 CostumeOn(u16 species, u16 year, u8 month, u8 day)
{
    char key[32], *p = key;
    u32 fears, h;
    u8 n = 0, t, type;
    u16 s, count = 0, pick;

    fears = FearsOf(gSpeciesInfo[species].types[0]);
    for (t = 0; t < NUMBER_OF_MON_TYPES; t++)
        if (fears & (1u << t))
            n++;
    if (n == 0)
        return SPECIES_NONE;            // nothing beats it: it goes as itself
    p = Digits(p, year, 4);
    *p++ = '-';
    p = Digits(p, month, 2);
    *p++ = '-';
    p = Digits(p, day, 2);
    *p++ = '|';
    p = Digits(p, species, 1);
    for (t = 0; sCostumeTail[t] != 0; t++)       // plain ASCII bytes, as the companion hashes them
        *p++ = sCostumeTail[t];
    *p = 0;
    h = Fnv(key);
    type = NthByName(fears, h % n);
    for (s = 1; s < NUM_SPECIES; s++)
        if (CostumeCandidate(s, type, species))
            count++;
    if (count == 0)
        return SPECIES_NONE;
    pick = (h >> 8) % count;
    for (s = 1; s < NUM_SPECIES; s++)
        if (CostumeCandidate(s, type, species) && pick-- == 0)
            return s;
    return SPECIES_NONE;
}

// IWRAM: the last answer, so the follower can ask every few frames without the chart being read each time.
static u16 sCostumeOf, sCostumeDay, sCostume;

u16 Hermetic_Costume(u16 species)
{
    u16 day, minute, year;
    u8 month, mday;

    if (!Hermetic_Held() || species == SPECIES_NONE || species >= NUM_SPECIES)
        return SPECIES_NONE;
    if (!Hermetic_IsHalloween())
    {
        if (FlagGet(FLAG_HERMETIC_UNMASKED))
            FlagClear(FLAG_HERMETIC_UNMASKED);
        return SPECIES_NONE;
    }
    if (FlagGet(FLAG_HERMETIC_UNMASKED))
        return SPECIES_NONE;
    DaemonsDayAndMinute(&day, &minute);
    if (sCostumeOf != species || sCostumeDay != day || sCostume == 0)
    {
        DaemonsDateOf(day, &year, &month, &mday);
        sCostumeOf = species;
        sCostumeDay = day;
        sCostume = CostumeOn(species, year, month, mday);
        if (sCostume == SPECIES_NONE)
            sCostume = 0xFFFF;          // asked, and nothing
    }
    return sCostume == 0xFFFF ? SPECIES_NONE : sCostume;
}

void Hermetic_Unmask(void)
{
    FlagSet(FLAG_HERMETIC_UNMASKED);
}
