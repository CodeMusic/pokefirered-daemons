#include "global.h"
#include "field_message_box.h"
#include "gflib.h"
#include "new_menu_helpers.h"
#include "quest_log.h"
#include "script.h"
#include "text_window.h"

static EWRAM_DATA u8 sMessageBoxType = 0;

//  T-179, R = AGAIN. FRLG keeps no history at all: a mistimed A loses a line for good, and in a game
//  this dense with writing some of those lines are the only place a thing is said. The EXPANDED text is
//  kept, not the pointer -- a script's message is built in gStringVar4 from buffers that have moved on by
//  the time anyone asks to read it again.
//  240, not 500: EWRAM has about a kilobyte free in stock pokefirered (engine.md) and a 500-byte buffer
//  spends half of it on a convenience. A field box shows three lines of about thirty-nine characters, so
//  240 holds one screenful and change -- and AGAIN reprints one screen, not a whole conversation.
static EWRAM_DATA u8 sLastMessage[240] = {0};

const u8 *GetLastFieldMessage(void)
{
    return sLastMessage[0] ? sLastMessage : NULL;
}

static void ExpandStringAndStartDrawFieldMessageBox(const u8 *str);
static void StartDrawFieldMessageBox(void);

void InitFieldMessageBox(void)
{
    sMessageBoxType = FIELD_MESSAGE_BOX_HIDDEN;
    gTextFlags.canABSpeedUpPrint = FALSE;
    gTextFlags.useAlternateDownArrow = FALSE;
    gTextFlags.autoScroll = FALSE;
}

static void Task_DrawFieldMessageBox(u8 taskId)
{
    struct Task *task = &gTasks[taskId];
    switch (task->data[0])
    {
    case 0:
        if (gQuestLogState == QL_STATE_PLAYBACK)
        {
            gTextFlags.autoScroll = TRUE;
            LoadQuestLogWindowTiles(0, 0x200);
        }
        else if (!IsMsgSignpost())
            LoadStdWindowFrameGfx();
        else
            LoadSignpostWindowFrameGfx();
        task->data[0]++;
        break;
    case 1:
        DrawDialogueFrame(0, TRUE);
        task->data[0]++;
        break;
    case 2:
        if (RunTextPrinters_CheckPrinter0Active() != TRUE)
        {
            sMessageBoxType = FIELD_MESSAGE_BOX_HIDDEN;
            DestroyTask(taskId);
        }
        break;
    }
}

static void CreateTask_DrawFieldMessageBox(void)
{
    CreateTask(Task_DrawFieldMessageBox, 80);
}

static void DestroyTask_DrawFieldMessageBox(void)
{
    u8 taskId = FindTaskIdByFunc(Task_DrawFieldMessageBox);
    if (taskId != 0xFF)
        DestroyTask(taskId);
}

bool8 ShowFieldMessage(const u8 *str)
{
    if (sMessageBoxType != FIELD_MESSAGE_BOX_HIDDEN)
        return FALSE;
    ExpandStringAndStartDrawFieldMessageBox(str);
    sMessageBoxType = FIELD_MESSAGE_BOX_NORMAL;
    return TRUE;
}

bool8 ShowFieldAutoScrollMessage(const u8 *str)
{
    if (sMessageBoxType != FIELD_MESSAGE_BOX_HIDDEN)
        return FALSE;
    sMessageBoxType = FIELD_MESSAGE_BOX_AUTO_SCROLL;
    ExpandStringAndStartDrawFieldMessageBox(str);
    return TRUE;
}

// Unused
static bool8 ForceShowFieldAutoScrollMessage(const u8 *str)
{
    sMessageBoxType = FIELD_MESSAGE_BOX_AUTO_SCROLL;
    ExpandStringAndStartDrawFieldMessageBox(str);
    return TRUE;
}

// Unused
// Same as ShowFieldMessage, but instead of accepting a string argument,
// it just prints whatever that's already in gStringVar4
static bool8 ShowFieldMessageFromBuffer(void)
{
    if (sMessageBoxType != FIELD_MESSAGE_BOX_HIDDEN)
        return FALSE;
    sMessageBoxType = FIELD_MESSAGE_BOX_NORMAL;
    StartDrawFieldMessageBox();
    return TRUE;
}

static void ExpandStringAndStartDrawFieldMessageBox(const u8 *str)
{
    StringExpandPlaceholders(gStringVar4, str);
    //  StringCopyN takes a u8 count, so it cannot be handed 499 -- and it does not terminate what it
    //  truncates either. Bounded by hand, and always terminated.
    {
        u32 i;

        for (i = 0; i < sizeof(sLastMessage) - 1 && gStringVar4[i] != EOS; i++)
            sLastMessage[i] = gStringVar4[i];
        sLastMessage[i] = EOS;
    }
    AddTextPrinterDiffStyle(TRUE);
    CreateTask_DrawFieldMessageBox();
}

static void StartDrawFieldMessageBox(void)
{
    AddTextPrinterDiffStyle(TRUE);
    CreateTask_DrawFieldMessageBox();
}

void HideFieldMessageBox(void)
{
    DestroyTask_DrawFieldMessageBox();
    ClearDialogWindowAndFrame(0, TRUE);
    sMessageBoxType = FIELD_MESSAGE_BOX_HIDDEN;
}

u8 GetFieldMessageBoxType(void)
{
    return sMessageBoxType;
}

bool8 IsFieldMessageBoxHidden(void)
{
    if (sMessageBoxType == FIELD_MESSAGE_BOX_HIDDEN)
        return TRUE;
    else
        return FALSE;
}

// Unused
static void ReplaceFieldMessageWithFrame(void)
{
    DestroyTask_DrawFieldMessageBox();
    DrawStdWindowFrame(0, TRUE);
    sMessageBoxType = FIELD_MESSAGE_BOX_HIDDEN;
}
