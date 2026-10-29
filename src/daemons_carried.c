#include "global.h"
#include "daemons_carried.h"
#include "daemons_first_seen.h"
#include "event_data.h"
#include "overworld.h"
#include "party_menu.h"
#include "pokedex.h"
#include "pokemon.h"
#include "pokemon_storage_system.h"
#include "string_util.h"
#include "data.h"
#include "constants/abilities.h"
#include "constants/flags.h"
#include "constants/items.h"
#include "constants/moves.h"
#include "constants/region_map_sections.h"
#include "constants/species.h"

//  T-448 / T-450 (the user, 2026-10-24): THE LOAD-TIME FINISH. The companion (daemons-companion) writes into a save
//  while a daemon is carried, and leaves the rest for the game to do when the save is next continued:
//
//    * C-184: a carried daemon that evolved there comes home as its new species (and a default nickname renamed);
//    * T-450: one new HANDSHAKE per save -- a BoxPokemon in the PC's first empty slot with the save's trainer id and
//      name, level 5, metLocation METLOC_FATEFUL_ENCOUNTER (0xFF), and NO routines.
//
//  Either way it sets a MARKER, unusedRibbons bit 2 (bits 0-1 are the FEELINGS', daemons_feelings.h; bit 3 stays
//  free). On load (overworld.c, ContinueLoadedSave) every party slot and every box slot is walked, and a marked daemon
//  is finished the way EvolutionScene would have: routines for its level if it has none (with PP), its ability made
//  one its species has, the INDEX's seen and caught set (FIRST MET: THE COMPANION for a delivery), the party's stats
//  recomputed -- and the marker cleared, so it happens once. A boxed daemon has no stored stats; the box computes them
//  when it is withdrawn.
//
//  T-455 (the user, 2026-10-24): a daemon that comes home EVOLVED with routines of its own is offered the routines its
//  new species learns at its level, as the evolution scene offers them (MonTryLearningNewMove, the replace-a-routine
//  flow) -- the first time the player has it in hand: in the party, on the first step after the line (the same script),
//  or, if it waits in a box, on the first step after it is withdrawn. unusedRibbons bit 3 says the offer is owed and is
//  cleared when it is made (or when its level teaches nothing new), so it is made once.
//
//  The first one finished is remembered (in RAM: nothing is saved) and said on the first step after the load, in one
//  line (Daemons_TryCarriedArrives, field_control_avatar.c, after the dreams). DRAFT words, for the user.
//
//  ALSO ONCE: T-450 put the carried line at INDEX 395-397 and moved MISSINGNO from 395 to 398, so a save from before
//  carries MISSINGNO's seen and caught bits (and its FIRST MET byte) across. A new game sets the flag (new_game.c).

#define CARRIED_RIBBON_MARK     (1 << 2)
#define CARRIED_RIBBON_MOVES    (1 << 3)    // T-455: its new species' routines are still to be offered

#define CARRIED_OLD_MISSINGNO   395
#define CARRIED_NEW_MISSINGNO   398

enum {
    CARRIED_NONE,
    CARRIED_DELIVERED_BOX,
    CARRIED_DELIVERED_PARTY,
    CARRIED_CHANGED,
    CARRIED_TRADED,     // T-473
    CARRIED_GIFTED,     // T-492
};

static EWRAM_DATA u8 sCarriedKind = 0;
static EWRAM_DATA u8 sCarriedName[POKEMON_NAME_LENGTH + 1] = {0};
static EWRAM_DATA bool8 sCarriedMovesOwed = FALSE;    // some daemon, party or box, still has bit 3: look at the party

STATIC_ASSERT(NATIONAL_DEX_OLD_UNOWN_B == CARRIED_NEW_MISSINGNO, MissingnoIsWhereTheMoveSaysItIs);
//  T-473: LOAN and TRADED_IN took bits of the u16 at 0x1E -- the record the companion reads and writes is still 80 bytes
STATIC_ASSERT(sizeof(struct BoxPokemon) == 80, BoxPokemonIsStillEightyBytes);
//  T-474: the partner species has nine bits (2-10 of 0x1E)
STATIC_ASSERT(NUM_SPECIES <= 512, TradePartnerFitsNineBits);

//  DRAFT (T-448): the one line on return
static const u8 sText_CarriedDeliveredBox[] = _("Something followed you home from the\nwalk. {STR_VAR_1} waits in the PC.");
static const u8 sText_CarriedDeliveredParty[] = _("Something followed you home from the\nwalk. {STR_VAR_1} is with you.");
static const u8 sText_CarriedChanged[] = _("{STR_VAR_1} changed while it was\ncarried.");
static const u8 sText_CarriedTraded[] = _("{STR_VAR_1} arrived in a trade.");   // T-473, DRAFT
static const u8 sText_CarriedGifted[] = _("{STR_VAR_1} arrived as a gift.");     // T-492, DRAFT

static bool8 DexBitGet(const u8 *flags, u16 nationalNum)
{
    return (flags[(nationalNum - 1) / 8] >> ((nationalNum - 1) % 8)) & 1;
}

static void DexBitMove(u8 *flags, u16 from, u16 to)
{
    if (DexBitGet(flags, from))
        flags[(to - 1) / 8] |= 1 << ((to - 1) % 8);
    flags[(from - 1) / 8] &= ~(1 << ((from - 1) % 8));
}

static void MoveMissingno(void)
{
    if (FlagGet(FLAG_CARRIED_DEX_MOVED))
        return;
    DexBitMove(gSaveBlock2Ptr->pokedex.seen, CARRIED_OLD_MISSINGNO, CARRIED_NEW_MISSINGNO);
    DexBitMove(gSaveBlock2Ptr->pokedex.owned, CARRIED_OLD_MISSINGNO, CARRIED_NEW_MISSINGNO);
    DexBitMove(gSaveBlock1Ptr->seen1, CARRIED_OLD_MISSINGNO, CARRIED_NEW_MISSINGNO);
    DexBitMove(gSaveBlock1Ptr->seen2, CARRIED_OLD_MISSINGNO, CARRIED_NEW_MISSINGNO);
    DaemonsFirstSeen_Move(CARRIED_OLD_MISSINGNO, CARRIED_NEW_MISSINGNO);
    FlagSet(FLAG_CARRIED_DEX_MOVED);
}

//  Where the save stands: the load has not read the map header yet, so FIRST MET is asked for by the save's own place.
static u8 SavedMapsec(void)
{
    return Overworld_GetMapHeaderByGroupAndId(gSaveBlock1Ptr->location.mapGroup, gSaveBlock1Ptr->location.mapNum)->regionMapSectionId;
}

//  the ability is read from the species by abilityNum: a second slot the species does not have reads as none
static void FitAbility(struct BoxPokemon *boxMon, u16 species)
{
    u8 abilityNum = GetBoxMonData(boxMon, MON_DATA_ABILITY_NUM, NULL);

    if (abilityNum && gSpeciesInfo[species].abilities[1] == ABILITY_NONE)
    {
        abilityNum = 0;
        SetBoxMonData(boxMon, MON_DATA_ABILITY_NUM, &abilityNum);
    }
}

//  T-479: FIRST MET is the first sighting, as FLAG_SET_SEEN keeps it (pokedex_screen.c) -- so a trade names it only
//  when the species had never been seen. A species seen before T-406's records has an empty byte that means BEFORE
//  RECORDS, not "never met", and a trade arriving later must not write A TRADE over it.
//  T-492: a GIFT (given, nothing back) is told apart from a trade -- FIRST MET: A GIFT -- under the same rule.
static void FirstMetByTrade(u16 nationalNum, bool8 gifted)
{
    if (GetSetPokedexFlag(nationalNum, FLAG_GET_SEEN))
        return;
    if (gifted)
        DaemonsFirstSeen_RecordGift(nationalNum);
    else
        DaemonsFirstSeen_RecordTrade(nationalNum);
}

//  T-473 (C-219): a daemon another player traded, as the companion wrote it -- TRADED_IN set (one-shot, cleared here
//  whatever happens, so it is never left set), its own trainer and routines. The INDEX takes it, FIRST MET: A TRADE, and
//  it evolves as a trade evolves it in FireRed: GetEvolutionTargetSpecies(EVO_MODE_TRADE) on a copy, so an EVERSTONE
//  holds it and EVO_TRADE_ITEM's item is spent; only a default name follows the species; its new routines are owed
//  (T-455). LOAN wins: a borrowed daemon is never trade-evolved. T-474: the species it was traded FOR comes with it (bits
//  2-10 of 0x1E, cleared with TRADED_IN), so DOLPHIN and PENGUIN become PENPHIN only when traded for each other.
static u8 FinishTrade(struct BoxPokemon *boxMon, struct Pokemon *partyMon, u8 ribbons)
{
    struct Pokemon copy;
    u8 nickname[POKEMON_NAME_LENGTH + 1];
    u16 species, target, item;
    u32 personality;
    bool8 loan = DaemonBoxIsLoan(boxMon);
    u16 partner = boxMon->daemonsTradePartner;   // T-474
    bool8 gifted = DaemonBoxIsGifted(boxMon);    // T-492: given, nothing back -- it still trade-evolves (the user)

    boxMon->daemonsTradedIn = 0;
    boxMon->daemonsTradePartner = 0;
    boxMon->daemonsGifted = 0;
    if (partner >= NUM_SPECIES)
        partner = SPECIES_NONE;
    ribbons &= ~CARRIED_RIBBON_MARK;
    SetBoxMonData(boxMon, MON_DATA_UNUSED_RIBBONS, &ribbons);
    species = GetBoxMonData(boxMon, MON_DATA_SPECIES, NULL);
    if (species == SPECIES_NONE || species >= NUM_SPECIES
     || GetBoxMonData(boxMon, MON_DATA_IS_EGG, NULL) || GetBoxMonData(boxMon, MON_DATA_SANITY_IS_BAD_EGG, NULL))
    {
        DebugPrintf("T-473: TRADED_IN cleared on species %d, not finished", species);
        return CARRIED_NONE;
    }
    personality = GetBoxMonData(boxMon, MON_DATA_PERSONALITY, NULL);
    FirstMetByTrade(SpeciesToNationalPokedexNum(species), gifted);
    HandleSetPokedexFlag(SpeciesToNationalPokedexNum(species), FLAG_SET_SEEN, personality);
    HandleSetPokedexFlag(SpeciesToNationalPokedexNum(species), FLAG_SET_CAUGHT, personality);
    if (!loan)
    {
        BoxMonToMon(boxMon, &copy);
        target = GetEvolutionTargetSpecies(&copy, EVO_MODE_TRADE, partner);   // T-474: what it was traded for
        if (target != SPECIES_NONE && target < NUM_SPECIES)
        {
            item = GetMonData(&copy, MON_DATA_HELD_ITEM, NULL);
            SetBoxMonData(boxMon, MON_DATA_HELD_ITEM, &item);
            GetBoxMonData(boxMon, MON_DATA_NICKNAME, nickname);
            if (StringCompare(nickname, gSpeciesNames[species]) == 0)
                SetBoxMonData(boxMon, MON_DATA_NICKNAME, gSpeciesNames[target]);
            SetBoxMonData(boxMon, MON_DATA_SPECIES, &target);
            FirstMetByTrade(SpeciesToNationalPokedexNum(target), gifted);
            HandleSetPokedexFlag(SpeciesToNationalPokedexNum(target), FLAG_SET_SEEN, personality);
            HandleSetPokedexFlag(SpeciesToNationalPokedexNum(target), FLAG_SET_CAUGHT, personality);
            ribbons |= CARRIED_RIBBON_MOVES;
            SetBoxMonData(boxMon, MON_DATA_UNUSED_RIBBONS, &ribbons);
            sCarriedMovesOwed = TRUE;
            species = target;
        }
    }
    FitAbility(boxMon, species);
    if (partyMon != NULL)
        CalculateMonStats(partyMon);
    if (sCarriedKind == CARRIED_NONE)
    {
        sCarriedKind = gifted ? CARRIED_GIFTED : CARRIED_TRADED;
        GetBoxMonData(boxMon, MON_DATA_NICKNAME, sCarriedName);
        StringGet_Nickname(sCarriedName);
    }
    return gifted ? CARRIED_GIFTED : CARRIED_TRADED;
}

static u8 FinishOne(struct BoxPokemon *boxMon, struct Pokemon *partyMon)
{
    u8 ribbons, kind;
    u16 species, nationalNum;
    u32 personality;

    if (!GetBoxMonData(boxMon, MON_DATA_SANITY_HAS_SPECIES, NULL))
        return CARRIED_NONE;
    ribbons = GetBoxMonData(boxMon, MON_DATA_UNUSED_RIBBONS, NULL);
    if (ribbons & CARRIED_RIBBON_MOVES)
        sCarriedMovesOwed = TRUE;                   // owed from an earlier load (it waited in a box)
    if (boxMon->daemonsTradedIn || boxMon->daemonsGifted)   // T-473: by trade (T-492: or a gift), finished its own way
        return FinishTrade(boxMon, partyMon, ribbons);
    if (!(ribbons & CARRIED_RIBBON_MARK))
        return CARRIED_NONE;
    ribbons &= ~CARRIED_RIBBON_MARK;
    SetBoxMonData(boxMon, MON_DATA_UNUSED_RIBBONS, &ribbons);
    if (GetBoxMonData(boxMon, MON_DATA_IS_EGG, NULL) || GetBoxMonData(boxMon, MON_DATA_SANITY_IS_BAD_EGG, NULL))
        return CARRIED_NONE;

    species = GetBoxMonData(boxMon, MON_DATA_SPECIES, NULL);
    kind = CARRIED_CHANGED;
    if (GetBoxMonData(boxMon, MON_DATA_MOVE1, NULL) == MOVE_NONE)
    {
        DaemonsGiveBoxMonInitialMoveset(boxMon);
        kind = partyMon != NULL ? CARRIED_DELIVERED_PARTY : CARRIED_DELIVERED_BOX;
    }
    else
    {
        //  T-455: it came home evolved, with routines of its own -- the new species' are offered when it is in hand
        ribbons |= CARRIED_RIBBON_MOVES;
        SetBoxMonData(boxMon, MON_DATA_UNUSED_RIBBONS, &ribbons);
        sCarriedMovesOwed = TRUE;
    }
    FitAbility(boxMon, species);
    nationalNum = SpeciesToNationalPokedexNum(species);
    personality = GetBoxMonData(boxMon, MON_DATA_PERSONALITY, NULL);
    if (GetSetPokedexFlag(nationalNum, FLAG_GET_SEEN))
        ;   // T-479: already met -- its FIRST MET stands, even an empty one (BEFORE RECORDS)
    else if (GetBoxMonData(boxMon, MON_DATA_MET_LOCATION, NULL) == METLOC_FATEFUL_ENCOUNTER)
        DaemonsFirstSeen_RecordCompanion(nationalNum);
    else
        DaemonsFirstSeen_RecordAt(nationalNum, SavedMapsec());
    HandleSetPokedexFlag(nationalNum, FLAG_SET_SEEN, personality);
    HandleSetPokedexFlag(nationalNum, FLAG_SET_CAUGHT, personality);
    if (partyMon != NULL)
        CalculateMonStats(partyMon);
    if (sCarriedKind == CARRIED_NONE)
    {
        sCarriedKind = kind;
        GetBoxMonData(boxMon, MON_DATA_NICKNAME, sCarriedName);
        StringGet_Nickname(sCarriedName);
    }
    return kind;
}

void DaemonsCarried_OnLoad(void)
{
    u32 i, j;

    sCarriedKind = CARRIED_NONE;
    sCarriedMovesOwed = FALSE;
    MoveMissingno();
    for (i = 0; i < PARTY_SIZE; i++)
        FinishOne(&gPlayerParty[i].box, &gPlayerParty[i]);
    for (i = 0; i < TOTAL_BOXES_COUNT; i++)
        for (j = 0; j < IN_BOX_COUNT; j++)
            FinishOne(GetBoxedMonPtr(i, j), NULL);
}

void DaemonsCarried_NewGame(void)
{
    FlagSet(FLAG_CARRIED_DEX_MOVED);   // a new game has no MISSINGNO at 395 to carry across
    sCarriedKind = CARRIED_NONE;
    sCarriedMovesOwed = FALSE;
}

//  The step hook: the line goes into gStringVar4 for Daemons_EventScript_Dream (a msgbox of gStringVar4) to say.
bool8 Daemons_TryCarriedArrives(void)
{
    const u8 *text;

    switch (sCarriedKind)
    {
    case CARRIED_DELIVERED_BOX:
        text = sText_CarriedDeliveredBox;
        break;
    case CARRIED_DELIVERED_PARTY:
        text = sText_CarriedDeliveredParty;
        break;
    case CARRIED_CHANGED:
        text = sText_CarriedChanged;
        break;
    case CARRIED_TRADED:
        text = sText_CarriedTraded;
        break;
    case CARRIED_GIFTED:
        text = sText_CarriedGifted;
        break;
    default:
        return FALSE;
    }
    sCarriedKind = CARRIED_NONE;
    StringCopy(gStringVar1, sCarriedName);
    StringExpandPlaceholders(gStringVar4, text);
    return TRUE;
}

//  T-455: does its species learn, at exactly its level, a routine it does not know? (MonTryLearningNewMove's question,
//  asked first so the party menu never opens on nothing.)
static bool8 LearnsSomethingAtItsLevel(struct Pokemon *mon)
{
    u16 species = GetMonData(mon, MON_DATA_SPECIES, NULL);
    u8 level = GetMonData(mon, MON_DATA_LEVEL, NULL);
    const u16 *learnset = gLevelUpLearnsets[species];
    u32 i;

    for (i = 0; learnset[i] != LEVEL_UP_END; i++)
        if ((learnset[i] & LEVEL_UP_MOVE_LV) == (level << 9) && !MonKnowsMove(mon, learnset[i] & LEVEL_UP_MOVE_ID))
            return TRUE;
    return FALSE;
}

//  T-455, the step hook and the line's script (specialvar): the first party daemon whose offer is owed, in VAR_0x8004
//  for Daemons_OfferCarriedMoves. Its bit is cleared here, whether its level teaches it anything or not.
bool8 Daemons_TryCarriedMoves(void)
{
    u32 i;
    u8 ribbons;
    struct Pokemon *mon;

    if (!sCarriedMovesOwed)
        return FALSE;
    for (i = 0; i < PARTY_SIZE; i++)
    {
        mon = &gPlayerParty[i];
        if (!GetMonData(mon, MON_DATA_SANITY_HAS_SPECIES, NULL) || GetMonData(mon, MON_DATA_IS_EGG, NULL))
            continue;
        ribbons = GetMonData(mon, MON_DATA_UNUSED_RIBBONS, NULL);
        if (!(ribbons & CARRIED_RIBBON_MOVES))
            continue;
        ribbons &= ~CARRIED_RIBBON_MOVES;
        SetMonData(mon, MON_DATA_UNUSED_RIBBONS, &ribbons);
        if (LearnsSomethingAtItsLevel(mon))
        {
            gSpecialVar_0x8004 = i;
            return TRUE;
        }
    }
    //  sCarriedMovesOwed stays set: one still in a box is offered on the first step after it is withdrawn
    return FALSE;
}
