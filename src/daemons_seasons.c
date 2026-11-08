#include "global.h"
#include "battle.h"
#include "battle_anim.h"   // GetBattlerPosition
#include "pokemon.h"
#include "daemons_time.h"
#include "daemons_seasons.h"
#include "constants/species.h"

// T-532 (vision 9.34; the user, 2026-11-08): the six MUSAI wear the season, Unown's way -- one species, its PICTURE
// chosen when it is drawn. The forms are picture-only ids past SPECIES_UNOWN_QMARK (tools/genmusaiseasons.py writes
// them and this table); the species never changes, so the save, the INDEX's counts, trades, evolution and stats are
// untouched. Look only, and silent: nothing says it changed.
#include "data/pokemon/musai_seasons.h"

// The 24 INDEX entries, one per form, both editions (DAEMONS tools/port_index.py writes them, DRAFT).
struct MusaiSeasonText
{
    u16 species;
    const u8 *text[SEASON_COUNT];
};
#include "data/pokemon/pokedex_text_seasons.h"

// A battle reads the season once, as it begins, and keeps it: a battle crossing midnight on an equinox (or seven
// hours of play without a clock) would otherwise switch a daemon in wearing the next season while the background kept
// the old one. gBattleStruct lives from AllocateBattleResources to FreeBattleResources, so the byte is read first by
// whatever draws first -- the background (daemons_battle_bg.c) -- and the INDEX or summary opened inside the battle
// agree with it.
u8 DaemonsDisplaySeason(void)
{
    if (gBattleStruct != NULL)
    {
        if (gBattleStruct->daemonsSeason == 0)
            gBattleStruct->daemonsSeason = DaemonsSeason() % SEASON_COUNT + 1;
        return gBattleStruct->daemonsSeason - 1;
    }
    return DaemonsSeason() % SEASON_COUNT;
}

static s32 BranchIndex(u16 species)
{
    s32 i;

    for (i = 0; i < (s32)ARRAY_COUNT(sMusaiBranch); i++)
        if (sMusaiBranch[i] == species)
            return i;
    return -1;
}

bool8 DaemonsIsMusaiBranch(u16 species)
{
    return BranchIndex(species) >= 0;
}

u16 DaemonsSeasonalSpecies(u16 species)
{
    s32 i = BranchIndex(species);

    if (i < 0)
        return species;
    return sMusaiSeasonForms[i][DaemonsDisplaySeason()];
}

const u8 *DaemonsSeasonalIndexText(u16 species)
{
    s32 i;

    for (i = 0; i < (s32)ARRAY_COUNT(sMusaiSeasonTexts); i++)
        if (sMusaiSeasonTexts[i].species == species)
            return sMusaiSeasonTexts[i].text[DaemonsDisplaySeason()];
    return NULL;
}

// CRYPTOPHASIA (vision 9.34): words only its family can follow. Each OTHER daemon of the branch in the user's party
// that can still fight -- not an egg, not fainted -- adds 20 to its 40. Read in Cmd_damagecalc and in the AI's estimate,
// as RETURN's power is read from the user.
u16 Cryptophasia_Power(u8 battler)
{
    struct Pokemon *party = GET_BATTLER_SIDE(battler) == B_SIDE_PLAYER ? gPlayerParty : gEnemyParty;
    u8 own = gBattlerPartyIndexes[battler];
    u16 power = 40;
    s32 i;

    for (i = 0; i < PARTY_SIZE; i++)
    {
        if (i == own)
            continue;
        if (DaemonsIsMusaiBranch(GetMonData(&party[i], MON_DATA_SPECIES_OR_EGG))
         && GetMonData(&party[i], MON_DATA_HP) != 0)
            power += 20;
    }
    return power > 140 ? 140 : power;
}
