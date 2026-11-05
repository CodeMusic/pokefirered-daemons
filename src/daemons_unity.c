#include "global.h"
#include "event_data.h"
#include "fieldmap.h"
#include "main.h"
#include "script.h"
#include "sound.h"
#include "task.h"
#include "daemons_unity.h"
#include "constants/flags.h"
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
