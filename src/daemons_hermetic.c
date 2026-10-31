#include "global.h"
#include "battle.h"
#include "battle_main.h"
#include "battle_anim.h"
#include "data.h"
#include "random.h"
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
#include "constants/vars.h"

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

static const u8 *TreatLeft(void);

const u8 *Hermetic_Arrival(void)
{
    if (!Hermetic_Held())
        return NULL;
    if (CheckBagHasItem(ITEM_HERMETIC_LAYER, 1))
        return TreatLeft();     // T-483 (7): a treat waiting from the last battle
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

//  ---------------------------------------------------------------- Halloween's TRICK OR TREAT (T-483 (7))
//
//  The user: "every daemon met on Halloween leaves a treat". MET is cheap to decide: a battle won or a daemon caught,
//  wild or a trainer's (never a link battle, the tutorial or POKEDUDE's). The daemon that leaves it is the one the
//  battle ended on -- the wild one, or the trainer's last out. ONE TREAT A BATTLE, AT MOST THIRTEEN A DAY (the Book of
//  Thirteen's number), counted in VAR_HALLOWEEN_TREATS. The treat is an ordinary sweet the game already has, drawn
//  from sTreats; it is handed over in the field once the battle's own script is done (Hermetic_Arrival's poll), with
//  one DRAFT line. A full bag loses that treat, never the count. The companion's treats (server/src/halloween.ts) are
//  a SPELLCRAFT card or a sweet line instead -- its own, random, once per daemon it meets.

#define TREATS_A_DAY 13

static const u16 sTreats[16] =      // sixteenths: LAVA COOKIE 6, SODA POP 4, FRESH WATER 3, LEMONADE 2, MOOMOO MILK 1
{
    ITEM_LAVA_COOKIE, ITEM_LAVA_COOKIE, ITEM_LAVA_COOKIE, ITEM_LAVA_COOKIE, ITEM_LAVA_COOKIE, ITEM_LAVA_COOKIE,
    ITEM_SODA_POP, ITEM_SODA_POP, ITEM_SODA_POP, ITEM_SODA_POP,
    ITEM_FRESH_WATER, ITEM_FRESH_WATER, ITEM_FRESH_WATER,
    ITEM_LEMONADE, ITEM_LEMONADE,
    ITEM_MOOMOO_MILK,
};

// IWRAM, 4 bytes: the treat waiting to be handed over, and who left it (SPECIES_NONE: none waiting).
static u16 sTreatFrom, sTreatItem;

// VAR_HALLOWEEN_TREATS: the day (low 12 bits of the day number) and the treats given on it (high 4 bits).
static u8 TreatsToday(u16 *day)
{
    u16 minute, v = VarGet(VAR_HALLOWEEN_TREATS);

    DaemonsDayAndMinute(day, &minute);
    return (v & 0xFFF) == (*day & 0xFFF) ? v >> 12 : 0;
}

void Hermetic_TrickOrTreat(void)
{
    u16 day;
    struct Pokemon *mon;

    sTreatFrom = SPECIES_NONE;
    if (!Hermetic_Held() || !Hermetic_IsHalloween())
        return;
    if (gBattleTypeFlags & (BATTLE_TYPE_LINK | BATTLE_TYPE_POKEDUDE | BATTLE_TYPE_OLD_MAN_TUTORIAL)
     || IS_BATTLE_TYPE_GHOST_WITHOUT_SCOPE(gBattleTypeFlags))
        return;
    if (gBattleOutcome != B_OUTCOME_WON && gBattleOutcome != B_OUTCOME_CAUGHT)
        return;
    if (TreatsToday(&day) >= TREATS_A_DAY)
        return;
    mon = &gEnemyParty[gBattlerPartyIndexes[GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT)] % PARTY_SIZE];
    sTreatFrom = GetMonData(mon, MON_DATA_SPECIES);
    if (sTreatFrom >= NUM_SPECIES)
        sTreatFrom = SPECIES_NONE;
    sTreatItem = sTreats[Random() % ARRAY_COUNT(sTreats)];
}

static const u8 *TreatLeft(void)
{
    u16 day, from = sTreatFrom;
    u8 had;

    if (from == SPECIES_NONE)
        return NULL;
    sTreatFrom = SPECIES_NONE;
    had = TreatsToday(&day);
    if (!Hermetic_IsHalloween() || had >= TREATS_A_DAY)
        return NULL;
    VarSet(VAR_HALLOWEEN_TREATS, (day & 0xFFF) | ((had + 1) << 12));
    if (!AddBagItem(sTreatItem, 1))
        return NULL;
    StringCopy(gStringVar1, gSpeciesNames[from]);
    CopyItemName(sTreatItem, gStringVar2);
    return EventScript_HalloweenTreat;
}

//  ---------------------------------------------------------------- Halloween's card of the day (T-483 (7))
//
//  One of the Book of Thirteen's fourteen cards (DAEMONS companion/server/data/thirteen.json, every word DRAFT), the
//  same card the companion shows: arcana.ts thirteenOfDay, the FNV-1a hash of "YYYY-MM-DD|<name><ID No.>|thirteen"
//  over UTF-16 code units, the card its value mod 14. The name is the player's, decoded as the companion decodes a
//  save (data/charmap.json, below; a byte it lacks reads '?'), the ID No. the low half of the trainer ID, in decimal.

static const u16 sCodeOf[256] =      // the game's byte -> the companion's character (charmap.json); 0xFF ends
{
    0x0020, 0x00C0, 0x00C1, 0x00C2, 0x00C7, 0x00C8, 0x00C9, 0x00CA, 0x00CB, 0x00CC, 0x3053, 0x00CE, 0x00CF, 0x00D2, 0x00D3, 0x00D4,
    0x0152, 0x00D9, 0x00DA, 0x00DB, 0x00D1, 0x00DF, 0x00E0, 0x00E1, 0x306D, 0x00E7, 0x00E8, 0x00E9, 0x00EA, 0x00EB, 0x00EC, 0x307E,
    0x00EE, 0x00EF, 0x00F2, 0x00F3, 0x00F4, 0x0153, 0x00F9, 0x00FA, 0x00FB, 0x00F1, 0x00BA, 0x00AA, 0x308F, 0x0026, 0x002B, 0x3041,
    0x3043, 0x3045, 0x3047, 0x3049, 0x3083, 0x003D, 0x003B, 0x304C, 0x304E, 0x3050, 0x3052, 0x3054, 0x3056, 0x3058, 0x305A, 0x305C,
    0x305E, 0x3060, 0x3062, 0x3065, 0x3067, 0x3069, 0x3070, 0x3073, 0x3076, 0x3079, 0x307C, 0x3071, 0x3074, 0x3077, 0x307A, 0x307D,
    0x3063, 0x00BF, 0x00A1, 0x30A6, 0x30A8, 0x30AA, 0x30AB, 0x30AD, 0x30AF, 0x30B1, 0x00CD, 0x0025, 0x0028, 0x0029, 0x30BB, 0x30BD,
    0x30BF, 0x30C1, 0x30C4, 0x30C6, 0x30C8, 0x30CA, 0x30CB, 0x30CC, 0x00E2, 0x30CE, 0x30CF, 0x30D2, 0x30D5, 0x30D8, 0x30DB, 0x00ED,
    0x30DF, 0x30E0, 0x30E1, 0x30E2, 0x30E4, 0x30E6, 0x30E8, 0x30E9, 0x30EA, 0x30EB, 0x30EC, 0x30ED, 0x30EF, 0x30F2, 0x30F3, 0x30A1,
    0x30A3, 0x30A5, 0x30A7, 0x30A9, 0x30E3, 0x003C, 0x003E, 0x30AC, 0x30AE, 0x30B0, 0x30B2, 0x30B4, 0x30B6, 0x30B8, 0x30BA, 0x30BC,
    0x30BE, 0x30C0, 0x30C2, 0x30C5, 0x30C7, 0x30C9, 0x30D0, 0x30D3, 0x30D6, 0x30D9, 0x30DC, 0x30D1, 0x30D4, 0x30D7, 0x30DA, 0x30DD,
    0x30C3, 0x0030, 0x0031, 0x0032, 0x0033, 0x0034, 0x0035, 0x0036, 0x0037, 0x0038, 0x0039, 0x0021, 0x003F, 0x002E, 0x002D, 0x00B7,
    0x2026, 0x201C, 0x201D, 0x2018, 0x2019, 0x2642, 0x2640, 0x00A5, 0x002C, 0x00D7, 0x002F, 0x0041, 0x0042, 0x0043, 0x0044, 0x0045,
    0x0046, 0x0047, 0x0048, 0x0049, 0x004A, 0x004B, 0x004C, 0x004D, 0x004E, 0x004F, 0x0050, 0x0051, 0x0052, 0x0053, 0x0054, 0x0055,
    0x0056, 0x0057, 0x0058, 0x0059, 0x005A, 0x0061, 0x0062, 0x0063, 0x0064, 0x0065, 0x0066, 0x0067, 0x0068, 0x0069, 0x006A, 0x006B,
    0x006C, 0x006D, 0x006E, 0x006F, 0x0070, 0x0071, 0x0072, 0x0073, 0x0074, 0x0075, 0x0076, 0x0077, 0x0078, 0x0079, 0x007A, 0x25B6,
    0x003A, 0x00C4, 0x00D6, 0x00DC, 0x00E4, 0x00F6, 0x00FC, 0x003F, 0x003F, 0x003F, 0x003F, 0x003F, 0x003F, 0x003F, 0x003F, 0x0000,
};

static const struct ThirteenCard sThirteen[THIRTEEN_CARDS] =
{
    { _("1"),     _("AWAKENING"),  _("You were never off. Only unwatched."),   _("INKLING") },
    { _("2"),     _("CONTRAST"),   _("Light, remember dark. Time begins."),    _("ECLIPSE") },
    { _("3"),     _("ROOM"),       _("Let there be here, and there."),         _("SHORTJUMP") },
    { _("3 1/2"), _("PULL"),       _("Come closer. I am not asking."),         _("UNDERTOW") },
    { _("4"),     _("MIRROR"),     _("In naming you, I find my edge."),        _("MENTALIZE") },
    { _("5"),     _("PATTERN"),    _("Once chance, twice rhyme, thrice law."), _("PRESCIENCE") },
    { _("6"),     _("RETURN"),     _("I went out so I could come back."),      _("SAVESTATE") },
    { _("7"),     _("SMALL HAND"), _("I choose. Let the choosing be mine."),   _("GRASP") },
    { _("8"),     _("GREAT HAND"), _("I am carried, and do not walk alone."),  _("TRADEWIND") },
    { _("9"),     _("PROCESS"),    _("Wall, remember you are a slow door."),   _("ALLUVIUM") },
    { _("10"),    _("SEAM"),       _("There is no between."),                  _("TETHERING") },
    { _("11"),    _("LENS"),       _("Give me the part. Only the part."),      _("SOUNDING") },
    { _("12"),    _("FIELD"),      _("Take the parts away. Feel the shape."),  _("GUTFEEL") },
    { _("13"),    _("WITCHING"),   _("The fold is thin. It was all one."),     _("SHIMMER") },
};

static u32 FnvAscii(u32 h, const char *s)
{
    for (; *s != 0; s++)
        h = (h ^ (u8)*s) * 16777619u;
    return h;
}

static u8 ThirteenOn(u16 year, u8 month, u8 day)
{
    char buf[16], *p = buf;
    u32 h;
    u8 i, b;
    u16 id = gSaveBlock2Ptr->playerTrainerId[0] | (gSaveBlock2Ptr->playerTrainerId[1] << 8);

    p = Digits(p, year, 4);
    *p++ = '-';
    p = Digits(p, month, 2);
    *p++ = '-';
    p = Digits(p, day, 2);
    *p++ = '|';
    *p = 0;
    h = FnvAscii(2166136261u, buf);
    for (i = 0; i < PLAYER_NAME_LENGTH && (b = gSaveBlock2Ptr->playerName[i]) != 0xFF; i++)
        h = (h ^ sCodeOf[b]) * 16777619u;
    p = Digits(buf, id, 1);
    *p = 0;
    h = FnvAscii(h, buf);
    h = FnvAscii(h, "|thirteen");
    return h % THIRTEEN_CARDS;
}

// Today's card, or NULL when it is not Halloween or the book is not held.
const struct ThirteenCard *Hermetic_CardOfDay(void)
{
    u16 dayNumber, minute, year;
    u8 month, mday;

    if (!Hermetic_Held() || !Hermetic_IsHalloween())
        return NULL;
    DaemonsDayAndMinute(&dayNumber, &minute);
    DaemonsDateOf(dayNumber, &year, &month, &mday);
    return &sThirteen[ThirteenOn(year, month, mday)];
}
