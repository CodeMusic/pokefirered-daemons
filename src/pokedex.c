#include "global.h"
#include "pokedex.h"
#include "pokedex_screen.h"

// Unused
const u8 *GetPokedexCategoryName(u16 dexNum)
{
    return gPokedexEntries[dexNum].categoryName;
}

u16 GetPokedexHeightWeight(u16 dexNum, u8 data)
{
    switch (data)
    {
    case 0:  // height
        return gPokedexEntries[dexNum].height;
    case 1:  // weight
        return gPokedexEntries[dexNum].weight;
    default:
        return 1;
    }
}

s8 GetSetPokedexFlag(u16 nationalDexNo, u8 caseID)
{
    return DexScreen_GetSetPokedexFlag(nationalDexNo, caseID, 0);
}

u16 GetNationalPokedexCount(u8 caseID)
{
    u16 count = 0;
    u16 i;

    for (i = 0; i < NATIONAL_DEX_COUNT; i++)
    {
        switch (caseID)
        {
        case FLAG_GET_SEEN:
            if (GetSetPokedexFlag(i + 1, FLAG_GET_SEEN))
                count++;
            break;
        case FLAG_GET_CAUGHT:
            if (GetSetPokedexFlag(i + 1, FLAG_GET_CAUGHT))
                count++;
            break;
        }
    }
    return count;
}

/*
u16 GetHoennPokedexCount(u8 caseID)
{
    u16 count = 0;
    u16 i;

    for (i = 0; i < HOENN_DEX_COUNT; i++)
    {
        switch (caseID)
        {
        case FLAG_GET_SEEN:
            if (GetSetPokedexFlag(HoennToNationalOrder(i + 1), FLAG_GET_SEEN))
                count++;
            break;
        case FLAG_GET_CAUGHT:
            if (GetSetPokedexFlag(HoennToNationalOrder(i + 1), FLAG_GET_CAUGHT))
                count++;
            break;
        }
    }
    return count;
}
*/

u16 GetKantoPokedexCount(u8 caseID)
{
    u16 count = 0;
    u16 i;

    for (i = 0; i < KANTO_DEX_COUNT; i++)
    {
        switch (caseID)
        {
        case FLAG_GET_SEEN:
            if (GetSetPokedexFlag(i + 1, FLAG_GET_SEEN))
                count++;
            break;
        case FLAG_GET_CAUGHT:
            if (GetSetPokedexFlag(i + 1, FLAG_GET_CAUGHT))
                count++;
            break;
        }
    }
    return count;
}

bool16 HasAllHoennMons(void)
{
    u16 i;

    // -2 excludes Jirachi and Deoxys
    for (i = 0; i < HOENN_DEX_COUNT - 2; i++)
    {
        if (!GetSetPokedexFlag(HoennToNationalOrder(i + 1), FLAG_GET_CAUGHT))
            return FALSE;
    }
    return TRUE;
}

bool16 HasAllKantoMons(void)
{
    u16 i;

    // -1 excludes Mew
    for (i = 0; i < KANTO_DEX_COUNT - 1; i++)
    {
        if (!GetSetPokedexFlag(i + 1, FLAG_GET_CAUGHT))
            return FALSE;
    }
    return TRUE;
}

//  T-469 (the user, 2026-10-28: "the cart alone earns it"): the GLOBAL diploma counts every daemon the cartridge can
//  give and nothing only the companion can. Named, not counted back from the end of the INDEX: "-2 excludes Jirachi
//  and Deoxys" went stale twice (T-395 put THE NEXUS after them, T-450 the carried line), and the range came to end at
//  HANDSHAKE, which the cart never gives. LYNX and BASTET count (THE NEXUS's grass; a MOON STONE), as does every NEXUS
//  resident. The six vanilla left out stay out: each is a single meeting that, once lost, is lost for good.
static bool8 IsOutsideGlobalDiploma(u16 nationalNum)
{
    switch (nationalNum)
    {
    case NATIONAL_DEX_MEW:
    case NATIONAL_DEX_LUGIA:
    case NATIONAL_DEX_HO_OH:
    case NATIONAL_DEX_CELEBI:
    case NATIONAL_DEX_JIRACHI:
    case NATIONAL_DEX_DEOXYS:
    //  The carried line (T-450): it grows only on the companion and the cart never gives it -- a bonus, not a debt.
    case NATIONAL_DEX_HANDSHAKE:
    case NATIONAL_DEX_TETHERING:
    case NATIONAL_DEX_COROUTINE:
        return TRUE;
    }
    return FALSE;
}

bool16 HasAllMons(void)
{
    u16 n;

    for (n = 1; n <= NATIONAL_DEX_COUNT; n++)
    {
        if (IsOutsideGlobalDiploma(n))
            continue;
        if (!GetSetPokedexFlag(n, FLAG_GET_CAUGHT))
            return FALSE;
    }
    return TRUE;
}
