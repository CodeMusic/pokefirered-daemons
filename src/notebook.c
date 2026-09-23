#include "global.h"
#include "gflib.h"
#include "event_data.h"
#include "list_menu.h"
#include "menu.h"
#include "menu_indicators.h"
#include "new_menu_helpers.h"
#include "script.h"
#include "string_util.h"
#include "task.h"
#include "text_window.h"
#include "school_exam.h"
#include "constants/songs.h"

//  T-216: THE RESEARCH NOTEBOOK (docs/school.md 9-10).
//
//  HEARSAY holds what people SAID about somebody. This holds what somebody WROTE -- primary sources, and
//  4.24's ending is an argument about how one was read. So this is the one screen in the game that asks the
//  player to do, for forty hours, the only thing that scene requires.
//
//  THE RULES THE CODE KEEPS.
//    1. A section appears only once it holds an entry. An empty shelf is a spoiler.
//    2. Entries list in the AUTHORED order of sEntries, never the order they were found -- so a page found
//       late can land between two found early. That is 4.10's device: dated paperwork, met out of sequence,
//       reading correctly once it is complete.
//    3. Every entry is a FLAG, not an item, so a document read before CAIRN hands this over is already in it.
//
//  THE SHAPE. A script drives it (data/scripts/notebook.inc): these specials put up a list, wait, and hand
//  the choice back in VAR_RESULT; an entry is then shown in the ordinary message box, so it pages the way
//  every sign does and R = AGAIN repeats it. The list lives on the HEAP -- EWRAM has under a kilobyte free.

enum
{
    NB_SCHOOL_NOTES,
    NB_LOOSE_PAGES,
    NB_LAB_NOTES,
    NB_RUN_LOGS,
    NB_CORRESPONDENCE,
    NB_THE_FILE,
    NB_PROSPECTUS,
    NB_PEER_REVIEW,
    NB_SECTION_COUNT
};

#define NB_CLOSE      0x7F
#define NB_MAX_SHOWN  6

struct NotebookEntry
{
    u8 section;
    u16 flag;
    const u8 *title;
    const u8 *text;
};

static const u8 sSection_SchoolNotes[]    = _("SCHOOL NOTES");
static const u8 sSection_LoosePages[]     = _("LOOSE PAGES");
static const u8 sSection_LabNotes[]       = _("LAB NOTES");
static const u8 sSection_RunLogs[]        = _("RUN LOGS");
static const u8 sSection_Correspondence[] = _("CORRESPONDENCE");
static const u8 sSection_TheFile[]        = _("THE FILE");
static const u8 sSection_Prospectus[]     = _("PROSPECTUS");
static const u8 sSection_PeerReview[]     = _("PEER REVIEW");

static const u8 *const sSectionNames[NB_SECTION_COUNT] =
{
    [NB_SCHOOL_NOTES]    = sSection_SchoolNotes,
    [NB_LOOSE_PAGES]     = sSection_LoosePages,
    [NB_LAB_NOTES]       = sSection_LabNotes,
    [NB_RUN_LOGS]        = sSection_RunLogs,
    [NB_CORRESPONDENCE]  = sSection_Correspondence,
    [NB_THE_FILE]        = sSection_TheFile,
    [NB_PROSPECTUS]      = sSection_Prospectus,
    [NB_PEER_REVIEW]     = sSection_PeerReview,
};

static const u8 sText_Close[] = _("CLOSE");
static const u8 sText_Back[]  = _("BACK");

//  The syllabus is the desk's own text, not a copy of it: what the player read is what the notebook holds.
extern const u8 ViridianCity_School_Text_Syllabus[];
extern const u8 ViridianCity_School_2F_Text_Syllabus[];
extern const u8 ViridianCity_School_3F_Text_Syllabus[];
extern const u8 ViridianCity_School_4F_Text_Syllabus[];


static const u8 sTitle_Syllabus_Language[] = _("1F LANGUAGE");
static const u8 sTitle_Syllabus_Attention[] = _("2F ATTENTION");
static const u8 sTitle_Syllabus_Memory[] = _("3F MEMORY");
static const u8 sTitle_Syllabus_Categories[] = _("4F CATEGORIES");

static const u8 sTitle_Loose_Stone[]       = _("THE STONE");
static const u8 sTitle_Paper[]             = _("THE PAPER");

//  LOOSE PAGES 1 -- DRAFT WORDING, awaiting the user's approval. A child's page, copied at the carving in
//  THE UNDERTONE (4.20, *Poly and Fields*): the rows are a type chart and the page never says so, and the
//  process word arrives as homework.
static const u8 sText_Loose_Stone[] = _(
    "A page torn from an exercise book.\n"
    "The writing is a child's.\p"
    "Copied off the stone in the wood,\n"
    "as near as I could.\p"
    "Rows of little figures. Each one\n"
    "set against every other one.\p"
    "Some rows push. Some give way.\n"
    "Some do nothing at all.\p"
    "It is the same rule everywhere on\n"
    "the stone. I checked every corner.\p"
    "At home nobody sees it.\n"
    "Here it sat still long enough.\p"
    "Tomorrow: integrating, page 40.\n"
    "I think I already know how.");

//  THE AUTHORED ORDER. A new entry goes where it belongs in the finished file, not at the end.
static const struct NotebookEntry sEntries[] =
{
    { NB_SCHOOL_NOTES, FLAG_SCHOOL_SYLLABUS_LANGUAGE, sTitle_Syllabus_Language, ViridianCity_School_Text_Syllabus },
    { NB_SCHOOL_NOTES, FLAG_SCHOOL_SYLLABUS_ATTENTION, sTitle_Syllabus_Attention, ViridianCity_School_2F_Text_Syllabus },
    { NB_SCHOOL_NOTES, FLAG_SCHOOL_SYLLABUS_MEMORY, sTitle_Syllabus_Memory, ViridianCity_School_3F_Text_Syllabus },
    { NB_SCHOOL_NOTES, FLAG_SCHOOL_SYLLABUS_CATEGORIES, sTitle_Syllabus_Categories, ViridianCity_School_4F_Text_Syllabus },
    //  T-217: written on demand -- every sitting's mark, and a paper under way if there is one. NULL text
    //  means the page is generated rather than stored.
    { NB_SCHOOL_NOTES, FLAG_SCHOOL_EXAM_OPENED,       sTitle_Paper,             NULL },
    { NB_LOOSE_PAGES,  FLAG_NOTEBOOK_LOOSE_STONE,     sTitle_Loose_Stone,       sText_Loose_Stone },
};

static EWRAM_DATA struct ListMenuItem *sItems = NULL;
static EWRAM_DATA u16 sArrowScroll = 0;
static EWRAM_DATA u16 sSectionScroll = 0;
static EWRAM_DATA u16 sSectionRow = 0;
static EWRAM_DATA u16 sEntryScroll = 0;
static EWRAM_DATA u16 sEntryRow = 0;

#define tMaxShown  data[0]
#define tCount     data[1]
#define tWidth     data[4]
#define tHeight    data[5]
#define tIsEntries data[6]
#define tArrows    data[12]
#define tWindowId  data[13]
#define tListId    data[14]

static void Task_NotebookList(u8 taskId);

static bool8 SectionHasEntry(u8 section)
{
    u32 i;

    for (i = 0; i < ARRAY_COUNT(sEntries); i++)
    {
        if (sEntries[i].section == section && FlagGet(sEntries[i].flag))
            return TRUE;
    }
    return FALSE;
}

static void MoveCursor_Notebook(s32 itemIndex, bool8 onInit, struct ListMenu *list)
{
    if (!onInit)
        PlaySE(SE_SELECT);
    sArrowScroll = list->itemsAbove;
}

static void CreateNotebookList(u8 count, bool8 isEntries, u16 scroll, u16 row)
{
    struct ListMenuTemplate template = {0};
    struct WindowTemplate win;
    u8 taskId = CreateTask(Task_NotebookList, 80);
    struct Task *task = &gTasks[taskId];
    s32 i, width, widest = 0;

    for (i = 0; i < count; i++)
    {
        width = GetStringWidth(FONT_NORMAL, sItems[i].label, 1);
        if (width > widest)
            widest = width;
    }
    task->tCount = count;
    task->tMaxShown = count < NB_MAX_SHOWN ? count : NB_MAX_SHOWN;
    task->tWidth = (widest + 9) / 8 + 1;
    task->tHeight = task->tMaxShown * 2;
    task->tIsEntries = isEntries;

    win = SetWindowTemplateFields(0, 1, 1, task->tWidth, task->tHeight, 15, 0x038);
    task->tWindowId = AddWindow(&win);
    SetStdWindowBorderStyle(task->tWindowId, 0);

    template.items = sItems;
    template.moveCursorFunc = MoveCursor_Notebook;
    template.itemPrintFunc = NULL;
    template.totalItems = count;
    template.maxShowed = task->tMaxShown;
    template.windowId = task->tWindowId;
    template.header_X = 0;
    template.item_X = 8;
    template.cursor_X = 0;
    template.upText_Y = 0;
    template.cursorPal = 2;
    template.fillValue = 1;
    template.cursorShadowPal = 3;
    template.lettersSpacing = 1;
    template.itemVerticalPadding = 0;
    template.scrollMultiple = LIST_NO_MULTIPLE_SCROLL;
    template.fontId = FONT_NORMAL;
    template.cursorKind = 0;

    sArrowScroll = scroll;
    if (task->tCount > task->tMaxShown)
    {
        struct ScrollArrowsTemplate arrows = {
            .firstArrowType = SCROLL_ARROW_UP,
            .secondArrowType = SCROLL_ARROW_DOWN,
            .tileTag = 2000,
            .palTag = 100,
        };
        arrows.firstX = 4 * task->tWidth + 8;
        arrows.firstY = 8;
        arrows.secondX = 4 * task->tWidth + 8;
        arrows.secondY = 8 * task->tHeight + 10;
        arrows.fullyUpThreshold = 0;
        arrows.fullyDownThreshold = task->tCount - task->tMaxShown;
        task->tArrows = AddScrollIndicatorArrowPair(&arrows, &sArrowScroll);
    }
    task->tListId = ListMenuInit(&template, scroll, row);
    PutWindowTilemap(task->tWindowId);
    CopyWindowToVram(task->tWindowId, COPYWIN_FULL);
}

static void DestroyNotebookList(u8 taskId)
{
    struct Task *task = &gTasks[taskId];
    u16 scroll, row;

    ListMenuGetScrollAndRow(task->tListId, &scroll, &row);
    if (task->tIsEntries)
    {
        sEntryScroll = scroll;
        sEntryRow = row;
    }
    else
    {
        sSectionScroll = scroll;
        sSectionRow = row;
    }
    if (task->tCount > task->tMaxShown)
        RemoveScrollIndicatorArrowPair(task->tArrows);
    DestroyListMenuTask(task->tListId, NULL, NULL);
    Free(sItems);
    sItems = NULL;
    ClearStdWindowAndFrameToTransparent(task->tWindowId, TRUE);
    FillWindowPixelBuffer(task->tWindowId, PIXEL_FILL(0));
    ClearWindowTilemap(task->tWindowId);
    CopyWindowToVram(task->tWindowId, COPYWIN_GFX);
    RemoveWindow(task->tWindowId);
    DestroyTask(taskId);
    ScriptContext_Enable();
}

static void Task_NotebookList(u8 taskId)
{
    s32 input = ListMenu_ProcessInput(gTasks[taskId].tListId);

    switch (input)
    {
    case LIST_NOTHING_CHOSEN:
        break;
    case LIST_CANCEL:
        gSpecialVar_Result = NB_CLOSE;
        PlaySE(SE_SELECT);
        DestroyNotebookList(taskId);
        break;
    default:
        gSpecialVar_Result = input;
        PlaySE(SE_SELECT);
        DestroyNotebookList(taskId);
        break;
    }
}

//  ---------------------------------------------------------------- the specials

//  VAR_RESULT: TRUE if anything at all is written in it.
void Notebook_HasEntries(void)
{
    u32 i;

    gSpecialVar_Result = FALSE;
    for (i = 0; i < ARRAY_COUNT(sEntries); i++)
    {
        if (FlagGet(sEntries[i].flag))
            gSpecialVar_Result = TRUE;
    }
    sSectionScroll = sSectionRow = 0;
}

//  The sections that hold something, in their fixed order, then CLOSE. VAR_RESULT: the section, or 0x7F.
void Notebook_ChooseSection(void)
{
    u8 s, n = 0;

    sItems = AllocZeroed((NB_SECTION_COUNT + 1) * sizeof(struct ListMenuItem));
    for (s = 0; s < NB_SECTION_COUNT; s++)
    {
        if (SectionHasEntry(s))
        {
            sItems[n].label = sSectionNames[s];
            sItems[n].index = s;
            n++;
        }
    }
    sItems[n].label = sText_Close;
    sItems[n].index = NB_CLOSE;
    n++;
    if (sSectionRow + sSectionScroll >= n)
        sSectionScroll = sSectionRow = 0;
    sEntryScroll = sEntryRow = 0;
    CreateNotebookList(n, FALSE, sSectionScroll, sSectionRow);
}

//  VAR_0x8005 is the section. The entries held in it, in AUTHORED order, then BACK.
//  VAR_RESULT: the entry's index in sEntries, or 0x7F.
void Notebook_ChooseEntry(void)
{
    u32 i;
    u8 n = 0;

    sItems = AllocZeroed((ARRAY_COUNT(sEntries) + 1) * sizeof(struct ListMenuItem));
    for (i = 0; i < ARRAY_COUNT(sEntries); i++)
    {
        if (sEntries[i].section == gSpecialVar_0x8005 && FlagGet(sEntries[i].flag))
        {
            sItems[n].label = sEntries[i].title;
            sItems[n].index = i;
            n++;
        }
    }
    sItems[n].label = sText_Back;
    sItems[n].index = NB_CLOSE;
    n++;
    if (sEntryRow + sEntryScroll >= n)
        sEntryScroll = sEntryRow = 0;
    CreateNotebookList(n, TRUE, sEntryScroll, sEntryRow);
}

//  VAR_0x8006 is the entry. Its text goes to gStringVar4 for the script's msgbox.
//  An entry must stay under 1000 bytes -- that is gStringVar4 -- and check_lexicon holds it there.
void Notebook_LoadEntry(void)
{
    if (gSpecialVar_0x8006 < ARRAY_COUNT(sEntries) && sEntries[gSpecialVar_0x8006].text == NULL)
        School_WriteExamRecord(gStringVar4);
    else if (gSpecialVar_0x8006 < ARRAY_COUNT(sEntries))
        StringExpandPlaceholders(gStringVar4, sEntries[gSpecialVar_0x8006].text);
    else
        gStringVar4[0] = EOS;
}
