#include "global.h"
#include "gflib.h"
#include "random.h"
#include "overworld.h"
#include "constants/maps.h"
#include "load_save.h"
#include "item_menu.h"
#include "tm_case.h"
#include "berry_pouch.h"
#include "quest_log.h"
#include "wild_encounter.h"
#include "event_data.h"
#include "mail_data.h"
#include "play_time.h"
#include "money.h"
#include "battle_records.h"
#include "pokemon_size_record.h"
#include "pokemon_storage_system.h"
#include "roamer.h"
#include "item.h"
#include "player_pc.h"
#include "berry.h"
#include "easy_chat.h"
#include "union_room_chat.h"
#include "mystery_gift.h"
#if DAEMONS_DEBUG
#include "constants/items.h"
#include "constants/species.h"
#include "constants/pokemon.h"
#include "constants/flags.h"
#include "constants/moves.h"
#include "pokedex.h"
#endif
#include "renewable_hidden_items.h"
#include "trainer_tower.h"
#include "script.h"
#include "berry_powder.h"
#include "pokemon_jump.h"
#include "event_scripts.h"

// this file's functions
static void ResetMiniGamesResults(void);

// EWRAM vars
EWRAM_DATA bool8 gDifferentSaveFile = FALSE;

void SetTrainerId(u32 trainerId, u8 *dst)
{
    dst[0] = trainerId;
    dst[1] = trainerId >> 8;
    dst[2] = trainerId >> 16;
    dst[3] = trainerId >> 24;
}

void CopyTrainerId(u8 *dst, u8 *src)
{
    s32 i;
    for (i = 0; i < 4; i++)
        dst[i] = src[i];
}

static void InitPlayerTrainerId(void)
{
    u32 trainerId = (Random() << 0x10) | GetGeneratedTrainerIdLower();
    SetTrainerId(trainerId, gSaveBlock2Ptr->playerTrainerId);
}

static void SetDefaultOptions(void)
{
    gSaveBlock2Ptr->optionsTextSpeed = OPTIONS_TEXT_SPEED_MID;
    gSaveBlock2Ptr->optionsWindowFrameType = 0;
    gSaveBlock2Ptr->optionsSound = OPTIONS_SOUND_MONO;
    gSaveBlock2Ptr->optionsBattleStyle = OPTIONS_BATTLE_STYLE_SHIFT;
    gSaveBlock2Ptr->optionsBattleSceneOff = FALSE;
    gSaveBlock2Ptr->regionMapZoom = FALSE;
    gSaveBlock2Ptr->optionsButtonMode = OPTIONS_BUTTON_MODE_HELP;
}

static void ClearPokedexFlags(void)
{
    memset(&gSaveBlock2Ptr->pokedex.owned, 0, sizeof(gSaveBlock2Ptr->pokedex.owned));
    memset(&gSaveBlock2Ptr->pokedex.seen, 0, sizeof(gSaveBlock2Ptr->pokedex.seen));
}

static void ClearBattleTower(void)
{
    CpuFill32(0, &gSaveBlock2Ptr->battleTower, sizeof(gSaveBlock2Ptr->battleTower));
}

static void WarpToPlayersRoom(void)
{
    SetWarpDestination(MAP_GROUP(MAP_PALLET_TOWN_PLAYERS_HOUSE_2F), MAP_NUM(MAP_PALLET_TOWN_PLAYERS_HOUSE_2F), -1, 6, 6);
    WarpIntoMap();
}

void Sav2_ClearSetDefault(void)
{
    ClearSav2();
    SetDefaultOptions();
}

void ResetMenuAndMonGlobals(void)
{
    gDifferentSaveFile = FALSE;
    ZeroPlayerPartyMons();
    ZeroEnemyPartyMons();
    ResetBagCursorPositions();
    ResetTMCaseCursorPos();
    BerryPouch_CursorResetToTop();
    ResetQuestLog();
    SeedWildEncounterRng(Random());
    ResetSpecialVars();
}

#if DAEMONS_DEBUG
// The testing kit. pokefirered ships no debug build at all -- unlike pokeemerald
// it has no menu to port -- so this is the smallest thing that answers the
// question 9.3 actually asked: does the GBA give this design room it did not
// have?
//
// So the party is picked for ABILITIES rather than for power (Levitate,
// Intimidate, Static, Thick Fat, Synchronize, Flash Fire), and the bag holds one
// of each KIND of item rather than a pile of one kind -- a ball, a medicine, a
// berry, a held item, a TM -- because the thing being evaluated is the
// description window, which Gen 1 does not have at all.
static void DaemonsDebug_GrantTestKit(void)
{
    // Every one of these is a species we have RENAMED. The first version picked
    // six for their abilities and four of them still read as vanilla on the
    // party screen, which made the debug build look like it contained none of
    // our work. Abilities are still the point -- Flash Fire, Water Absorb,
    // Volt Absorb, Synchronize, Pressure, Thick Fat -- but a test kit that
    // shows nothing we changed is not a test kit.
    static const u16 sParty[] = {
        SPECIES_FLAREON,    // CODEMUSAI  -- Flash Fire
        SPECIES_VAPOREON,   // CAREMUSAI  -- Water Absorb
        SPECIES_JOLTEON,    // SEEKMUSAI  -- Volt Absorb
        SPECIES_MEW,        // ARTSAI     -- Synchronize
        SPECIES_MEWTWO,     // S.T.A.R.R. -- Pressure
        SPECIES_SNORLAX,    // DEADLOCK   -- Immunity / Thick Fat
    };
    static const u16 sBag[][2] = {
        { ITEM_ULTRA_BALL,   20 },
        { ITEM_HYPER_POTION, 20 },
        { ITEM_RARE_CANDY,   30 },
        { ITEM_ORAN_BERRY,   10 },
        { ITEM_LEFTOVERS,     1 },
        { ITEM_TM01,          1 },
        // A KEY item, which is its own pocket and its own description pane --
        // and it is the one thing whose absence actually blocked a test run:
        // without it the ship at Ardor cannot be boarded, so a debug save
        // could not reach HM01 or anything behind it.
        { ITEM_SS_TICKET,     1 },
    };
    struct Pokemon mon;
    u32 i;

    // The last two carry the HMs between them. The Game Boy debug build had a
    // party member who could move you around the map, and losing that on the
    // port made testing much slower: four move slots each, eight field moves,
    // so it takes two daemons and there is no room for anything else on them.
    static const u16 sFieldMoves[] = {
        MOVE_CUT, MOVE_FLY, MOVE_SURF, MOVE_STRENGTH,
        MOVE_ROCK_SMASH, MOVE_WATERFALL, MOVE_FLASH, MOVE_DIVE,
    };
    u32 m;

    for (i = 0; i < ARRAY_COUNT(sParty); i++)
    {
        CreateMon(&mon, sParty[i], 50, 31, FALSE, 0, OT_ID_PLAYER_ID, 0);
        if (i >= ARRAY_COUNT(sParty) - 2)
        {
            u32 base = (i == ARRAY_COUNT(sParty) - 2) ? 0 : 4;
            for (m = 0; m < 4; m++)
                SetMonMoveSlot(&mon, sFieldMoves[base + m], m);
        }
        GiveMonToPlayer(&mon);
    }

    // The Index is the thing most worth testing, and it opens empty without
    // these. Every entry marked seen and caught, so every one can be read.
    for (i = 1; i <= NATIONAL_DEX_COUNT; i++)
    {
        GetSetPokedexFlag(i, FLAG_SET_SEEN);
        GetSetPokedexFlag(i, FLAG_SET_CAUGHT);
    }
    for (i = 0; i < ARRAY_COUNT(sBag); i++)
        AddBagItem(sBag[i][0], sBag[i][1]);

    for (i = FLAG_BADGE01_GET; i <= FLAG_BADGE08_GET; i++)
        FlagSet(i);

    // THE ITEM IS NOT THE PERMISSION. Putting ITEM_SS_TICKET in the bag was not
    // enough and could not be: the sailor at Ardor runs
    //     goto_if_unset FLAG_GOT_SS_TICKET, ...DontHaveSSTicket
    // so he checks a FLAG and never looks in the bag at all. A debug save with
    // the ticket in hand was still turned away at the gangway.
    //
    // Both are set, deliberately. The flag is what opens the ship; the item is
    // what the player can read a description of, which is what 9.3 is here to
    // evaluate.
    FlagSet(FLAG_GOT_SS_TICKET);

    // Without these the party is in memory and unreachable: start_menu.c only
    // draws the POKeMON entry when FLAG_SYS_POKEMON_GET is set, and the DEX
    // entry when FLAG_SYS_POKEDEX_GET is. Handing someone six daemons and no
    // way to open the menu is worse than handing them none, because it looks
    // like the kit did not run.
    FlagSet(FLAG_SYS_POKEMON_GET);
    FlagSet(FLAG_SYS_POKEDEX_GET);

    // Fly opens the map but every destination is dead until it has been
    // visited: region_map.c returns MAPSECTYPE_NOT_VISITED for any mapsec whose
    // FLAG_WORLD_MAP_* is clear, and the cursor will not settle on one of
    // those. The block is contiguous, PALLET_TOWN through BIRTH_ISLAND, so mark
    // the whole world seen -- a debug build that can reach one town is not a
    // debug build.
    for (i = FLAG_WORLD_MAP_PALLET_TOWN; i <= FLAG_WORLD_MAP_BIRTH_ISLAND_EXTERIOR; i++)
        FlagSet(i);

    SetMoney(&gSaveBlock1Ptr->money, 999999);
}
#endif

// The ID number is a record of what you chose.
//
// Vanilla rolls 32 random bits and shows you the low sixteen, which is a number
// that means nothing. Ours derives the visible half from the choices actually
// made at the front of the game -- your name, your rival's name, which edition
// this is, and whether it is a testing build -- and leaves the last five bits
// to chance.
//
// So two players who chose the same things land within 32 of each other and
// everyone else lands somewhere else entirely: the number tells you what was
// chosen, and the last two digits do not. Nobody is told this, and nothing in
// the game remarks on it.
//
// The high sixteen bits stay random. They are never displayed, and they are
// half of what decides whether a daemon is shiny -- deriving those from a name
// would quietly make shininess a function of what you typed.
static void DaemonsDeriveTrainerId(void)
{
    u32 sig = 2166136261u;                 // FNV-1a, for no reason but that it is short
    const u8 *p;
    u32 i;

    for (p = gSaveBlock2Ptr->playerName, i = 0; i < PLAYER_NAME_LENGTH && p[i] != EOS; i++)
        sig = (sig ^ p[i]) * 16777619u;
    for (p = gSaveBlock1Ptr->rivalName, i = 0; i < PLAYER_NAME_LENGTH && p[i] != EOS; i++)
        sig = (sig ^ p[i]) * 16777619u;
#if defined(FIRERED)
    sig = (sig ^ 'C') * 16777619u;         // CONTENT
#else
    sig = (sig ^ 'X') * 16777619u;         // CONTEXT
#endif
#if DAEMONS_DEBUG
    sig = (sig ^ 'D') * 16777619u;
#endif

    SetTrainerId(((Random() << 16) | (((sig & 0x7FF) << 5) | (Random() & 0x1F))),
                 gSaveBlock2Ptr->playerTrainerId);
}

void NewGameInitData(void)
{
    u8 rivalName[PLAYER_NAME_LENGTH + 1];

    StringCopy(rivalName, gSaveBlock1Ptr->rivalName);
    gDifferentSaveFile = TRUE;
    gSaveBlock2Ptr->encryptionKey = 0;
    ZeroPlayerPartyMons();
    ZeroEnemyPartyMons();
    ClearBattleTower();
    ClearSav1();
    ClearMailData();
    gSaveBlock2Ptr->specialSaveWarpFlags = 0;
    gSaveBlock2Ptr->gcnLinkFlags = 0;
    gSaveBlock2Ptr->unkFlag1 = TRUE;
    gSaveBlock2Ptr->unkFlag2 = FALSE;
    InitPlayerTrainerId();
    PlayTimeCounter_Reset();
    ClearPokedexFlags();
    InitEventData();
    ResetFameChecker();
    SetMoney(&gSaveBlock1Ptr->money, 3000);
    ResetGameStats();
    ClearPlayerLinkBattleRecords();
    InitHeracrossSizeRecord();
    InitMagikarpSizeRecord();
    EnableNationalPokedex_RSE();
    gPlayerPartyCount = 0;
    ZeroPlayerPartyMons();
    ResetPokemonStorageSystem();
    ClearRoamerData();
    gSaveBlock1Ptr->registeredItem = 0;
    ClearBag();
    NewGameInitPCItems();
    ClearEnigmaBerries();
    InitEasyChatPhrases();
    ResetTrainerFanClub();
    UnionRoomChat_InitializeRegisteredTexts();
    ResetMiniGamesResults();
    ClearMysteryGift();
    SetAllRenewableItemFlags();
    WarpToPlayersRoom();
    RunScriptImmediately(EventScript_ResetAllMapFlags);
    StringCopy(gSaveBlock1Ptr->rivalName, rivalName);
    ResetTrainerTowerResults();
    DaemonsDeriveTrainerId();
#if DAEMONS_DEBUG
    DaemonsDebug_GrantTestKit();
#endif
}

static void ResetMiniGamesResults(void)
{
    CpuFill16(0, &gSaveBlock2Ptr->berryCrush, sizeof(struct BerryCrush));
    SetBerryPowder(&gSaveBlock2Ptr->berryCrush.berryPowderAmount, 0);
    ResetPokemonJumpRecords();
    CpuFill16(0, &gSaveBlock2Ptr->berryPick, sizeof(struct BerryPickingResults));
}
