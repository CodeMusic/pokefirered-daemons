#include "global.h"
#include "event_data.h"
#include "overworld.h"
#include "script.h"
#include "task.h"
#include "daemons_eudaimon.h"
#include "daemons_hours.h"
#include "daemons_time.h"
#include "daemons_aurora.h"     // T-528 (d): she sits again for the dawn chorus
#include "constants/flags.h"

// T-519 item 2 (the user, 2026-11-02, by multiple choice; the brief and vision 4.40, private detail in
// docs/private/): THE EUDAIMON LAYER, the third book's dawn half -- read in the same open book as the night half
// (book_reader.c, BOOK_EUDAIMON), its pages written by DAEMONS tools/gensubtle.py from docs/eudaimon-layer.md.
// It opens at any hour and is read only: its thirteen are the night half's spells, cast from the SPELLBOOK.
//
// WHERE IT IS FOUND (the brief): under the singing FIR, in THE CREATOR'S HOUR (DaemonsCreatorSecond, 4:24-5:12 on
// the cartridge's clock; DEBUG WATCH's three creator watches reach it). The LARK sits there with it -- in every
// grove, beside the fir (each grove's map.json, local id 3, hidden by FLAG_TEMP_1 when she is not there:
// EventScript_FirKeyFlags asks Eudaimon_LarkSits on every arrival). She gives it only to a player who sits with
// her and waits: about ten seconds with no button pressed (Eudaimon_LarkWait). Not on the S.S. ANNE's pier, whose
// fir stands on a boardwalk with no room beside it.
//
// THE NUDGE (the brief): with the dawn half held and the night half not, in the witching hour (3:00-3:59) a voice that
// is not the player's says one purple line, on the first step outdoors -- once in the hour (sNudged, RAM only: a
// reset inside the hour may say it again), and not in play's first minute (the catch-up, daemons_hours.c).

#define LARK_WAIT_FRAMES (60 * 10)

static bool8 sNudged;   // IWRAM, one byte: said in this witching hour

bool8 Eudaimon_Held(void)
{
    return FlagGet(FLAG_GOT_EUDAIMON_LAYER);
}

// T-528 (d): and once the dawn half is given, she comes back to the fir only for the dawn chorus, to sing along.
bool8 Eudaimon_LarkSits(void)
{
    return DaemonsCreatorSecond() >= 0 && (!Eudaimon_Held() || Aurora_DawnChorus());
}

#define tPhase  data[0]
#define tFrames data[1]

#define ANY_KEY (A_BUTTON | B_BUTTON | SELECT_BUTTON | START_BUTTON | DPAD_ANY | L_BUTTON | R_BUTTON)

// Sitting: first every button let go (the YES that sat you down is still under a thumb), then ten seconds of
// nothing. Any press, and you stirred.
static void Task_LarkWait(u8 taskId)
{
    struct Task *task = &gTasks[taskId];

    if (task->tPhase == 0)
    {
        if (!(gMain.heldKeys & ANY_KEY))
            task->tPhase = 1;
        return;
    }
    if (gMain.newKeys & ANY_KEY)
        gSpecialVar_Result = FALSE;
    else if (++task->tFrames >= LARK_WAIT_FRAMES)
        gSpecialVar_Result = TRUE;
    else
        return;
    DestroyTask(taskId);
    ScriptContext_Enable();
}

void Eudaimon_LarkWait(void)
{
    CreateTask(Task_LarkWait, 80);
}

#undef tPhase
#undef tFrames

bool8 Daemons_TryEudaimonNudge(void)
{
    if (!DaemonsIsWitchingHour())
    {
        sNudged = FALSE;
        return FALSE;
    }
    if (sNudged || !Eudaimon_Held() || FlagGet(FLAG_GOT_HERMETIC_LAYER))
        return FALSE;
    if (!IsMapTypeOutdoors(gMapHeader.mapType) || Hours_WitchingEase() == 0)
        return FALSE;
    sNudged = TRUE;
    return TRUE;
}
