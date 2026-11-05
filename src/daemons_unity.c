#include "global.h"
#include "event_data.h"
#include "item.h"
#include "fieldmap.h"
#include "main.h"
#include "script.h"
#include "sound.h"
#include "task.h"
#include "daemons_unity.h"
#include "constants/flags.h"
#include "constants/items.h"
#include "constants/region_map_sections.h"
#include "constants/songs.h"

// T-519 item 3 (the user, 2026-11-02 and 2026-11-05, by multiple choice; vision 4.40, the private detail in
// docs/private/): THE UNITY BOOK, the third book whole. Its halves are THE HERMETIC LAYER (the night half,
// daemons_hermetic.c) and THE EUDAIMON LAYER (the dawn half, daemons_eudaimon.c); they are mended at DAEMON TOWER.
//
// BEFORE THE MENDING (item 2): a half opened from the bag inside DAEMON TOWER -- any floor -- deepens its tint for a
// moment, deep emerald or deep sapphire, then settles (fieldmap.c DaemonsHalftoneDeepen; it shows on a floor already
// back in colour too, over its own colours), and its book's last page ends torn mid-sentence at the spine (the torn
// text is DAEMONS tools/gensubtle.py's TORN, in each half's header). Inside the tower rather than the town: the
// tower is where the halves are mended, so it is where each one shows it is half of something.
//
// THE MENDING (item 3, the brief: "at DAEMON TOWER, with both halves held"): the first step taken inside the tower --
// any floor -- with both halves held, or either half opened there, mends them. A colourful burst (fieldmap.c
// DaemonsHalftoneBurst: the tower's tone turns through every colour and settles into the tower's own), and the two
// key items become one, THE UNITY BOOK (ITEM_UNITY_BOOK, was ITEM_0FA): the halves leave the bag, and both their
// flags stay set -- they mean "found", and the SPELLBOOK and every half's other rule still read them. Leaving the
// tower, the colour spreads from each step until HALFTONE is coloured (DaemonsHalftoneOnStep). In the save: one bit,
// FLAG_UNITY_MENDED (vanilla's unused FLAG_0x4BD). The spreading is RAM only; a reset during it leaves the town whole.

bool8 Unity_Mended(void)
{
    return FlagGet(FLAG_UNITY_MENDED);
}

static bool8 InTower(void)
{
    return gMapHeader.regionMapSectionId == MAPSEC_POKEMON_TOWER;
}

bool8 Mending_TornHere(void)
{
    return InTower() && !Unity_Mended();
}

u8 Mending_AtTower(void)
{
    bool8 night = FlagGet(FLAG_GOT_HERMETIC_LAYER), dawn = FlagGet(FLAG_GOT_EUDAIMON_LAYER);

    if (!InTower() || Unity_Mended() || (!night && !dawn))
        return MENDING_NOT_HERE;
    return night && dawn ? MENDING_BOTH_HALVES : MENDING_ONE_HALF;
}

static void Task_WaitForHalftone(u8 taskId)
{
    if (DaemonsHalftoneDeepening() || DaemonsHalftoneBursting())
        return;
    DestroyTask(taskId);
    ScriptContext_Enable();
}

void Mending_Deepen(void)
{
    bool8 dawn = gSpecialVar_0x8004 != 0;

    PlaySE(dawn ? SE_M_MORNING_SUN : SE_M_MOONLIGHT);
    DaemonsHalftoneDeepen(dawn);
    CreateTask(Task_WaitForHalftone, 80);
}

bool8 Mending_ShouldStart(void)
{
    return Mending_AtTower() == MENDING_BOTH_HALVES && !DaemonsHalftoneBursting();
}

static void Mend(void)
{
    FlagSet(FLAG_UNITY_MENDED);
    if (CheckBagHasItem(ITEM_HERMETIC_LAYER, 1))
        RemoveBagItem(ITEM_HERMETIC_LAYER, 1);
    if (CheckBagHasItem(ITEM_EUDAIMON_LAYER, 1))
        RemoveBagItem(ITEM_EUDAIMON_LAYER, 1);
    if (!CheckBagHasItem(ITEM_UNITY_BOOK, 1))
        AddBagItem(ITEM_UNITY_BOOK, 1);      // the key items' pocket never fills: two leave as one comes
}

void Mending_Mend(void)
{
    PlaySE(SE_M_HEAL_BELL);
    DaemonsHalftoneBurst();                  // first, so the burst starts from the tone the tower stood in
    Mend();
    CreateTask(Task_WaitForHalftone, 80);
}

#if DAEMONS_DEBUG
void Mending_DebugSet(bool8 on)
{
    if (on)
    {
        FlagSet(FLAG_GOT_HERMETIC_LAYER);
        FlagSet(FLAG_GOT_EUDAIMON_LAYER);
        Mend();
    }
    else
    {
        FlagClear(FLAG_UNITY_MENDED);
        if (CheckBagHasItem(ITEM_UNITY_BOOK, 1))
            RemoveBagItem(ITEM_UNITY_BOOK, 1);
        if (FlagGet(FLAG_GOT_HERMETIC_LAYER) && !CheckBagHasItem(ITEM_HERMETIC_LAYER, 1))
            AddBagItem(ITEM_HERMETIC_LAYER, 1);
        if (FlagGet(FLAG_GOT_EUDAIMON_LAYER) && !CheckBagHasItem(ITEM_EUDAIMON_LAYER, 1))
            AddBagItem(ITEM_EUDAIMON_LAYER, 1);
    }
    DaemonsHalftoneDebugMended(on);
}
#endif
