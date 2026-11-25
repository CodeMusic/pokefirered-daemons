#include "global.h"
#include "event_data.h"
#include "script.h"
#include "string_util.h"
#include "characters.h"
#include "task.h"
#include "daemons_wire.h"

//  T-621: THE WIRE -- a spike (the user, 2026-11-24): can a local model write what a person in the town says, with
//  the line handed into the running game? docs/wire-spike.md has the answer and the measurements.
//
//  THE HANDSHAKE. A host outside the cartridge (mGBA's Lua, tools/wire_host.lua; a browser's EmulatorJS; RetroArch on
//  a Pi) writes WIRE_HOST_MAGIC into gDaemonsWire.host every frame it is listening. Talking to a wired person, the
//  script calls this special and waits (waitstate):
//    - no host (a real cartridge, Delta, an emulator with no script): VAR_RESULT FALSE at once -- the written line.
//    - a host: the game marks the ask, clears .host, and waits. The host reads who and where, asks its model, writes
//      the line into gStringVar4 (charmap bytes, already wrapped to the box), then sets WIRE_ANSWERED.
//    - the host stops writing .host for half a second, says WIRE_FAILED, or takes longer than WIRE_PATIENCE:
//      VAR_RESULT FALSE -- the written line, never a silence.
//
//  DEBUG ONLY for now: a release ROM has no gDaemonsWire (EWRAM is 99.75% full; engine.md 1), and the special
//  answers FALSE without waiting. Nothing here is saved.

#define WIRE_PATIENCE   (60 * 20)   // a cold model loads for about 13 s
#define WIRE_HOST_GONE  30          // frames without a heartbeat

#if DAEMONS_DEBUG

EWRAM_DATA struct DaemonsWire gDaemonsWire = {0};

static void Task_WaitForWire(u8 taskId)
{
    s16 *data = gTasks[taskId].data;
    bool8 done = FALSE, said = FALSE;

    gDaemonsWire.waited++;
    if (gDaemonsWire.host == WIRE_HOST_MAGIC)
    {
        data[0] = 0;
        gDaemonsWire.host = 0;      // it must come back next frame
    }
    else if (++data[0] > WIRE_HOST_GONE)
    {
        done = TRUE;
    }

    if (gDaemonsWire.state == WIRE_ANSWERED)
        done = said = TRUE;
    else if (gDaemonsWire.state == WIRE_FAILED || gDaemonsWire.waited > WIRE_PATIENCE)
        done = TRUE;

    if (done)
    {
        gDaemonsWire.state = WIRE_IDLE;
        gSpecialVar_Result = said;
        DestroyTask(taskId);
        ScriptContext_Enable();
    }
}

void DaemonsWire_Ask(void)
{
    bool8 host = gDaemonsWire.host == WIRE_HOST_MAGIC;

    gSpecialVar_Result = FALSE;
    if (!host)
    {
        // nothing is listening: waitstate still waits for a task, so one that ends at once
        CreateTask(Task_WaitForWire, 80);
        gDaemonsWire.state = WIRE_FAILED;
        gDaemonsWire.waited = 0;
        return;
    }
    gStringVar4[0] = EOS;
    gDaemonsWire.speaker = gSpecialVar_LastTalked;
    gDaemonsWire.mapGroup = gSaveBlock1Ptr->location.mapGroup;
    gDaemonsWire.mapNum = gSaveBlock1Ptr->location.mapNum;
    gDaemonsWire.seq++;
    gDaemonsWire.waited = 0;
    gDaemonsWire.host = 0;
    gDaemonsWire.state = WIRE_ASKED;
    CreateTask(Task_WaitForWire, 80);
}

#else

static void Task_NoWire(u8 taskId)
{
    DestroyTask(taskId);
    ScriptContext_Enable();
}

void DaemonsWire_Ask(void)
{
    gSpecialVar_Result = FALSE;
    CreateTask(Task_NoWire, 80);
}

#endif
