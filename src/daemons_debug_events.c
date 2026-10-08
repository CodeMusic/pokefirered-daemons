#include "global.h"
#include "gflib.h"
#include "event_data.h"
#include "fame_checker.h"
#include "item.h"
#include "list_menu.h"
#include "menu.h"
#include "menu_indicators.h"
#include "new_game.h"
#include "new_menu_helpers.h"
#include "notebook.h"
#include "script.h"
#include "string_util.h"
#include "task.h"
#include "teachy_tv.h"
#include "text_window.h"
#include "constants/items.h"
#include "constants/opponents.h"
#include "constants/songs.h"

//  T-394 (the user, 2026-10-08): "Debug mode starts with lots of things so you can't test some things." A new debug
//  game starts as a normal one does, and DEBUG -> GAME EVENTS hands the kit out one piece at a time, in the order the
//  story gives each piece. A row marked + is held, a row marked · is not; A on it gives or takes it away (rows without
//  a mark only give). MARKS, the NOTEBOOK, the DRIVERS and the UNDERSTANDINGS are pages of their own, each with ALL.
//  EVERYTHING is the old kit, exactly (new_game.c).
//
//  A script drives it (data/scripts/daemons_debug.inc), as the NOTEBOOK's lists are driven: DaemonsDebug_EventsChoose
//  puts the page up and hands the row back in VAR_RESULT, DaemonsDebug_EventsDo does it and leaves what it did in
//  gStringVar4. The list lives on the heap.

#if DAEMONS_DEBUG

enum { PG_EVENTS, PG_MARKS, PG_NOTEBOOK, PG_DRIVERS, PG_ROOTS, PG_COUNT };
enum { R_PAGE, R_GIVE, R_TOGGLE };

enum
{
    EV_EVERYTHING, EV_ROSTER, EV_EVERY_DAEMON, EV_STOCK,
    EV_OPENING, EV_SHOES, EV_INDEX, EV_OPUS, EV_SCHOOL, EV_TICKET, EV_TEA, EV_GATES, EV_REVEAL, EV_RESOLVER,
    EV_INTERRUPT, EV_GOTO_POINTS, EV_GUIDE, EV_GLOBAL_INDEX, EV_STREAM, EV_COMPANION,
    EV_MARKS_ALL, EV_MARK1, EV_MARK8 = EV_MARK1 + 7,
    EV_NOTEBOOK, EV_NOTEBOOK_FULL,
    EV_DRIVERS_ALL, EV_DRIVER1, EV_DRIVER8 = EV_DRIVER1 + 7,
    EV_ROOTS_ALL, EV_ROOT1, EV_ROOT7 = EV_ROOT1 + 6, EV_INSIGHTS_AGAIN,
};

struct DbgRow { const u8 *name; u8 kind; u8 arg; };

static const u8 sName_Everything[]   = _("EVERYTHING (THE OLD KIT)");
static const u8 sName_Roster[]       = _("THE ROSTER (SIX, LV50)");
static const u8 sName_EveryDaemon[]  = _("EVERY DAEMON, INDEX + PORT");
static const u8 sName_Stock[]        = _("THE STOCK (BAG, MONEY)");
static const u8 sName_Opening[]      = _("THE OPENING, OVER");
static const u8 sName_Shoes[]        = _("RUNNING SHOES");
static const u8 sName_Index[]        = _("THE INDEX");
static const u8 sName_Opus[]         = _("OPUS");
static const u8 sName_Marks[]        = _("MARKS >");
static const u8 sName_Notebook[]     = _("NOTEBOOK >");
static const u8 sName_School[]       = _("TEXTBOOK AND DIPLOMA");
static const u8 sName_Ticket[]       = _("S.S. ANNE TICKET");
static const u8 sName_Drivers[]      = _("DRIVERS >");
static const u8 sName_Tea[]          = _("TEA, FOR BRAZEN'S GUARDS");
static const u8 sName_Gates[]        = _("BRAZEN'S GATES OPEN");
static const u8 sName_Reveal[]       = _("REVEAL");
static const u8 sName_Resolver[]     = _("RESOLVER (HALFTONE)");
static const u8 sName_Interrupt[]    = _("INTERRUPT");
static const u8 sName_GotoPoints[]   = _("EVERY GOTO POINT");
static const u8 sName_Guide[]        = _("THE GUIDE");
static const u8 sName_GlobalIndex[]  = _("GLOBAL INDEX");
static const u8 sName_Roots[]        = _("UNDERSTANDINGS >");
static const u8 sName_Stream[]       = _("STREAM AND HEARSAY");
static const u8 sName_Companion[]    = _("COMPANION LINKED");
static const u8 sName_All[]          = _("ALL");
static const u8 sName_Mark1[] = _("SLATE (CAIRN)");
static const u8 sName_Mark2[] = _("SLOPE (BASIN)");
static const u8 sName_Mark3[] = _("SENSE (GAUGE)");
static const u8 sName_Mark4[] = _("FIT (TRELLIS)");
static const u8 sName_Mark5[] = _("SKEW (TILT)");
static const u8 sName_Mark6[] = _("FRAME (MATTE)");
static const u8 sName_Mark7[] = _("HEAT (ANNEAL)");
static const u8 sName_Mark8[] = _("TRUE (SCORN)");
static const u8 sName_TheNotebook[]  = _("THE NOTEBOOK, EMPTY");
static const u8 sName_EveryPage[]    = _("EVERY PAGE");
static const u8 sName_Driver1[] = _("PRUNE");
static const u8 sName_Driver2[] = _("GOTO");
static const u8 sName_Driver3[] = _("TRAVERSE");
static const u8 sName_Driver4[] = _("DISPLACE");
static const u8 sName_Driver5[] = _("VERBOSE");
static const u8 sName_Driver6[] = _("CRACK");
static const u8 sName_Driver7[] = _("ASCEND");
static const u8 sName_Driver8[] = _("DESCEND");
static const u8 sName_Root1[] = _("FIRST (TANOBY)");
static const u8 sName_Root2[] = _("THE SCHOOL");
static const u8 sName_Root3[] = _("THE READING ROOM");
static const u8 sName_Root4[] = _("THE NOTES");
static const u8 sName_Root5[] = _("SCORN");
static const u8 sName_Root6[] = _("THE RETURN");
static const u8 sName_Root7[] = _("THE GUIDE, READ");
static const u8 sName_InsightsAgain[] = _("LET INSIGHTS ARRIVE AGAIN");

//  THE STORY'S ORDER: the opening, the shoes and the INDEX, OPUS on THE REPO's shelf, then CAIRN's MARK and NOTEBOOK,
//  the school, the ship, the DRIVERS, BRAZEN's tea and REVEAL, HALFTONE, the sleeper, the GUIDE, and last what only
//  the end of the game gives.
static const struct DbgRow sRows_Events[] = {
    { sName_Everything,  R_GIVE,   EV_EVERYTHING },
    { sName_Roster,      R_GIVE,   EV_ROSTER },
    { sName_EveryDaemon, R_GIVE,   EV_EVERY_DAEMON },
    { sName_Stock,       R_GIVE,   EV_STOCK },
    { sName_Opening,     R_GIVE,   EV_OPENING },
    { sName_Shoes,       R_TOGGLE, EV_SHOES },
    { sName_Index,       R_TOGGLE, EV_INDEX },
    { sName_Opus,        R_TOGGLE, EV_OPUS },
    { sName_Marks,       R_PAGE,   PG_MARKS },
    { sName_Notebook,    R_PAGE,   PG_NOTEBOOK },
    { sName_School,      R_TOGGLE, EV_SCHOOL },
    { sName_Ticket,      R_TOGGLE, EV_TICKET },
    { sName_Drivers,     R_PAGE,   PG_DRIVERS },
    { sName_Tea,         R_TOGGLE, EV_TEA },
    { sName_Gates,       R_TOGGLE, EV_GATES },
    { sName_Reveal,      R_TOGGLE, EV_REVEAL },
    { sName_Resolver,    R_TOGGLE, EV_RESOLVER },
    { sName_Interrupt,   R_TOGGLE, EV_INTERRUPT },
    { sName_GotoPoints,  R_TOGGLE, EV_GOTO_POINTS },
    { sName_Guide,       R_TOGGLE, EV_GUIDE },
    { sName_GlobalIndex, R_TOGGLE, EV_GLOBAL_INDEX },
    { sName_Roots,       R_PAGE,   PG_ROOTS },
    { sName_Stream,      R_GIVE,   EV_STREAM },
    { sName_Companion,   R_TOGGLE, EV_COMPANION },
};
static const struct DbgRow sRows_Marks[] = {
    { sName_All, R_TOGGLE, EV_MARKS_ALL },
    { sName_Mark1, R_TOGGLE, EV_MARK1 + 0 }, { sName_Mark2, R_TOGGLE, EV_MARK1 + 1 },
    { sName_Mark3, R_TOGGLE, EV_MARK1 + 2 }, { sName_Mark4, R_TOGGLE, EV_MARK1 + 3 },
    { sName_Mark5, R_TOGGLE, EV_MARK1 + 4 }, { sName_Mark6, R_TOGGLE, EV_MARK1 + 5 },
    { sName_Mark7, R_TOGGLE, EV_MARK1 + 6 }, { sName_Mark8, R_TOGGLE, EV_MARK1 + 7 },
};
static const struct DbgRow sRows_Notebook[] = {
    { sName_TheNotebook, R_TOGGLE, EV_NOTEBOOK },
    { sName_EveryPage,   R_GIVE,   EV_NOTEBOOK_FULL },
};
static const struct DbgRow sRows_Drivers[] = {
    { sName_All, R_TOGGLE, EV_DRIVERS_ALL },
    { sName_Driver1, R_TOGGLE, EV_DRIVER1 + 0 }, { sName_Driver2, R_TOGGLE, EV_DRIVER1 + 1 },
    { sName_Driver3, R_TOGGLE, EV_DRIVER1 + 2 }, { sName_Driver4, R_TOGGLE, EV_DRIVER1 + 3 },
    { sName_Driver5, R_TOGGLE, EV_DRIVER1 + 4 }, { sName_Driver6, R_TOGGLE, EV_DRIVER1 + 5 },
    { sName_Driver7, R_TOGGLE, EV_DRIVER1 + 6 }, { sName_Driver8, R_TOGGLE, EV_DRIVER1 + 7 },
};
static const struct DbgRow sRows_Roots[] = {
    { sName_All, R_TOGGLE, EV_ROOTS_ALL },
    { sName_Root1, R_TOGGLE, EV_ROOT1 + 0 }, { sName_Root2, R_TOGGLE, EV_ROOT1 + 1 },
    { sName_Root3, R_TOGGLE, EV_ROOT1 + 2 }, { sName_Root4, R_TOGGLE, EV_ROOT1 + 3 },
    { sName_Root5, R_TOGGLE, EV_ROOT1 + 4 }, { sName_Root6, R_TOGGLE, EV_ROOT1 + 5 },
    { sName_Root7, R_TOGGLE, EV_ROOT1 + 6 },
    { sName_InsightsAgain, R_GIVE, EV_INSIGHTS_AGAIN },
};

static const struct { const struct DbgRow *rows; u8 count; } sPages[PG_COUNT] = {
    [PG_EVENTS]   = { sRows_Events,   ARRAY_COUNT(sRows_Events) },
    [PG_MARKS]    = { sRows_Marks,    ARRAY_COUNT(sRows_Marks) },
    [PG_NOTEBOOK] = { sRows_Notebook, ARRAY_COUNT(sRows_Notebook) },
    [PG_DRIVERS]  = { sRows_Drivers,  ARRAY_COUNT(sRows_Drivers) },
    [PG_ROOTS]    = { sRows_Roots,    ARRAY_COUNT(sRows_Roots) },
};

static const u8 sText_Title_Events[] = _("GAME EVENTS, in story order.\nA gives or takes. + held, · not.");
static const u8 sText_Title_Marks[] = _("MARKS: each leader beaten, its\nMARK held. B goes back.");
static const u8 sText_Title_Notebook[] = _("NOTEBOOK: empty, or every page.\nB goes back.");
static const u8 sText_Title_Drivers[] = _("DRIVERS: held in the TOOLKIT,\nwhich is enough. B goes back.");
static const u8 sText_Title_Roots[] = _("UNDERSTANDINGS, each with what\nit needs. B goes back.");
static const u8 *const sTitles[PG_COUNT] = {
    sText_Title_Events, sText_Title_Marks, sText_Title_Notebook, sText_Title_Drivers, sText_Title_Roots,
};
static const u8 sText_Given[] = _("{STR_VAR_1}: given.");
static const u8 sText_Taken[] = _("{STR_VAR_1}: taken away.");
static const u8 sMark_Held[] = _("+ ");
static const u8 sMark_Not[]  = _("· ");
static const u8 sMark_Give[] = _("  ");

#define DBG_BACK  0x7E
#define DBG_CLOSE 0x7F
#define DBG_MAX_SHOWN 6
#define DBG_LABEL 32

static EWRAM_DATA u8 sPage = 0;
static EWRAM_DATA u16 sScroll[PG_COUNT] = {0};
static EWRAM_DATA u16 sRow[PG_COUNT] = {0};
static EWRAM_DATA struct ListMenuItem *sItems = NULL;
static EWRAM_DATA u8 *sLabels = NULL;
static EWRAM_DATA u16 sArrowScroll = 0;

//  ---------------------------------------------------------------- what each event is

static const u16 sMarkLeaders[8][2] = {
    { FLAG_DEFEATED_BROCK,    TRAINER_LEADER_BROCK },
    { FLAG_DEFEATED_MISTY,    TRAINER_LEADER_MISTY },
    { FLAG_DEFEATED_LT_SURGE, TRAINER_LEADER_LT_SURGE },
    { FLAG_DEFEATED_ERIKA,    TRAINER_LEADER_ERIKA },
    { FLAG_DEFEATED_KOGA,     TRAINER_LEADER_KOGA },
    { FLAG_DEFEATED_SABRINA,  TRAINER_LEADER_SABRINA },
    { FLAG_DEFEATED_BLAINE,   TRAINER_LEADER_BLAINE },
    { FLAG_DEFEATED_LEADER_GIOVANNI, TRAINER_LEADER_GIOVANNI },
};

//  Each understanding with the two things book_reader.c's DaemonsArriveAtUnderstandings asks for (the FIRST is set
//  by its own script and needs nothing). Taking one away takes its conditions too, or the next map load would bring
//  it straight back -- SCORN's is beating him, so taking it takes the eighth MARK.
static const u16 sRoots[7][3] = {
    { FLAG_UNDERSTANDING_FIRST,   0, 0 },
    { FLAG_UNDERSTANDING_SCHOOL,  FLAG_SCHOOL_GOT_TEXTBOOK, FLAG_GOT_DIPLOMA },
    { FLAG_UNDERSTANDING_READING, FLAG_GOT_DIPLOMA, FLAG_GOT_REVEAL },
    { FLAG_UNDERSTANDING_NOTES,   FLAG_NOTEBOOK_RUN_FATAL, FLAG_NOTEBOOK_FILE_COMPLETE },
    { FLAG_UNDERSTANDING_SCORN,   FLAG_BADGE08_GET, FLAG_DEFEATED_LEADER_GIOVANNI },
    { FLAG_UNDERSTANDING_RETURN,  FLAG_TY_GAVE_PAYLOAD, FLAG_CRYSTAL_READ_PAYLOAD },
    { FLAG_UNDERSTANDING_GUIDE,   FLAG_GOT_GUIDE, FLAG_GUIDE_READ },
};

static const u16 sInsights[7] = {
    FLAG_INSIGHT_DILIGENCE, FLAG_INSIGHT_CHASTITY, FLAG_INSIGHT_CHARITY, FLAG_INSIGHT_KINDNESS,
    FLAG_INSIGHT_TEMPERANCE, FLAG_INSIGHT_PATIENCE, FLAG_INSIGHT_HUMILITY,
};

static void SetFlag(u16 flag, bool8 on)
{
    if (flag == 0)
        return;
    if (on)
        FlagSet(flag);
    else
        FlagClear(flag);
}

static void SetItem(u16 item, bool8 on)
{
    if (on && !CheckBagHasItem(item, 1))
        AddBagItem(item, 1);
    else if (!on && CheckBagHasItem(item, 1))
        RemoveBagItem(item, 1);
}

static bool8 MarkHeld(u8 i)
{
    return FlagGet(FLAG_BADGE01_GET + i);
}

static void SetMark(u8 i, bool8 on)
{
    SetFlag(FLAG_BADGE01_GET + i, on);
    SetFlag(sMarkLeaders[i][0], on);
    SetFlag(TRAINER_FLAGS_START + sMarkLeaders[i][1], on);
    if (on)
        TeachyTvMarkEveryUnlockedShowAsTold();   // a STREAM show per MARK, as the kit does, without eight announcements
}

static void SetRoot(u8 i, bool8 on)
{
    if (on && i == 1)
    {
        SetItem(ITEM_TEXTBOOK, TRUE);
        SetItem(ITEM_DIPLOMA, TRUE);
    }
    if (on && i == 2)
        SetItem(ITEM_DIPLOMA, TRUE);
    if (on && i == 6)
        SetItem(ITEM_GUIDE, TRUE);
    if (i == 4)
        SetMark(7, on);
    SetFlag(sRoots[i][1], on);
    SetFlag(sRoots[i][2], on);
    SetFlag(sRoots[i][0], on);
}

static bool8 IsHeld(u8 ev)
{
    u32 i;

    switch (ev)
    {
    case EV_SHOES:        return FlagGet(FLAG_SYS_B_DASH);
    case EV_INDEX:        return FlagGet(FLAG_SYS_POKEDEX_GET);
    case EV_OPUS:         return FlagGet(FLAG_GOT_OPUS);
    case EV_SCHOOL:       return FlagGet(FLAG_GOT_DIPLOMA);
    case EV_TICKET:       return FlagGet(FLAG_GOT_SS_TICKET);
    case EV_TEA:          return CheckBagHasItem(ITEM_TEA, 1);
    case EV_GATES:        return VarGet(VAR_MAP_SCENE_ROUTE5_ROUTE6_ROUTE7_ROUTE8_GATES) != 0;
    case EV_REVEAL:       return FlagGet(FLAG_GOT_REVEAL);
    case EV_RESOLVER:     return CheckBagHasItem(ITEM_SILPH_SCOPE, 1);
    case EV_INTERRUPT:    return CheckBagHasItem(ITEM_POKE_FLUTE, 1);
    case EV_GOTO_POINTS:  return FlagGet(FLAG_WORLD_MAP_BIRTH_ISLAND_EXTERIOR);
    case EV_GUIDE:        return FlagGet(FLAG_GOT_GUIDE);
    case EV_GLOBAL_INDEX: return IsNationalPokedexEnabled();
    case EV_COMPANION:    return FlagGet(FLAG_COMPANION_LINKED);
    case EV_NOTEBOOK:     return FlagGet(FLAG_GOT_NOTEBOOK);
    case EV_MARKS_ALL:
        for (i = 0; i < 8; i++)
            if (!MarkHeld(i))
                return FALSE;
        return TRUE;
    case EV_DRIVERS_ALL:
        for (i = 0; i < 8; i++)
            if (!CheckBagHasItem(ITEM_HM01 + i, 1))
                return FALSE;
        return TRUE;
    case EV_ROOTS_ALL:
        for (i = 0; i < 7; i++)
            if (!FlagGet(sRoots[i][0]))
                return FALSE;
        return TRUE;
    }
    if (ev >= EV_MARK1 && ev <= EV_MARK8)
        return MarkHeld(ev - EV_MARK1);
    if (ev >= EV_DRIVER1 && ev <= EV_DRIVER8)
        return CheckBagHasItem(ITEM_HM01 + (ev - EV_DRIVER1), 1);
    if (ev >= EV_ROOT1 && ev <= EV_ROOT7)
        return FlagGet(sRoots[ev - EV_ROOT1][0]);
    return FALSE;
}

static void Apply(u8 ev, bool8 on)
{
    u32 i;

    switch (ev)
    {
    case EV_EVERYTHING:   DaemonsDebug_GrantTestKit(); return;
    case EV_ROSTER:       DaemonsDebug_GiveRoster(); return;
    case EV_EVERY_DAEMON: DaemonsDebug_FillIndexAndPort(); FlagSet(FLAG_SYS_POKEDEX_GET); return;
    case EV_STOCK:        DaemonsDebug_GiveStock(); return;
    case EV_OPENING:      DaemonsDebug_FinishOpening(); return;
    case EV_SHOES:        SetFlag(FLAG_SYS_B_DASH, on); return;
    case EV_INDEX:        SetFlag(FLAG_SYS_POKEDEX_GET, on); return;
    case EV_OPUS:         SetFlag(FLAG_GOT_OPUS, on); SetItem(ITEM_OPUS, on); return;
    case EV_SCHOOL:
        SetFlag(FLAG_SCHOOL_GOT_TEXTBOOK, on); SetFlag(FLAG_GOT_DIPLOMA, on);
        SetItem(ITEM_TEXTBOOK, on); SetItem(ITEM_DIPLOMA, on);
        return;
    case EV_TICKET:       SetFlag(FLAG_GOT_SS_TICKET, on); SetItem(ITEM_SS_TICKET, on); return;
    case EV_TEA:          SetItem(ITEM_TEA, on); return;
    case EV_GATES:
        if (on)
            DaemonsDebug_OpenBrazenGates();
        else
            VarSet(VAR_MAP_SCENE_ROUTE5_ROUTE6_ROUTE7_ROUTE8_GATES, 0);
        return;
    case EV_REVEAL:       SetFlag(FLAG_GOT_REVEAL, on); return;
    case EV_RESOLVER:     SetItem(ITEM_SILPH_SCOPE, on); return;
    case EV_INTERRUPT:    SetFlag(FLAG_GOT_POKE_FLUTE, on); SetItem(ITEM_POKE_FLUTE, on); return;
    case EV_GOTO_POINTS:  DaemonsDebug_EveryGotoPoint(on); return;
    case EV_GUIDE:        SetFlag(FLAG_GOT_GUIDE, on); SetItem(ITEM_GUIDE, on); return;
    case EV_GLOBAL_INDEX:
        if (on)
            EnableNationalPokedex();
        else
            DisableNationalPokedex();
        return;
    case EV_STREAM:
        SetItem(ITEM_TEACHY_TV, TRUE); SetItem(ITEM_FAME_CHECKER, TRUE);
        FullyUnlockFameChecker();
        TeachyTvMarkEveryUnlockedShowAsTold();
        return;
    case EV_COMPANION:    SetFlag(FLAG_COMPANION_LINKED, on); return;
    case EV_NOTEBOOK:     SetFlag(FLAG_GOT_NOTEBOOK, on); SetItem(ITEM_NOTEBOOK, on); return;
    case EV_NOTEBOOK_FULL:
        FlagSet(FLAG_GOT_NOTEBOOK); SetItem(ITEM_NOTEBOOK, TRUE);
        Notebook_DebugFillAll();
        return;
    case EV_MARKS_ALL:
        for (i = 0; i < 8; i++)
            SetMark(i, on);
        return;
    case EV_DRIVERS_ALL:
        for (i = 0; i < 8; i++)
            SetItem(ITEM_HM01 + i, on);
        return;
    case EV_ROOTS_ALL:
        for (i = 0; i < 7; i++)
            SetRoot(i, on);
        return;
    case EV_INSIGHTS_AGAIN:
        for (i = 0; i < ARRAY_COUNT(sInsights); i++)
            FlagClear(sInsights[i]);
        return;
    }
    if (ev >= EV_MARK1 && ev <= EV_MARK8)
        SetMark(ev - EV_MARK1, on);
    else if (ev >= EV_DRIVER1 && ev <= EV_DRIVER8)
        SetItem(ITEM_HM01 + (ev - EV_DRIVER1), on);
    else if (ev >= EV_ROOT1 && ev <= EV_ROOT7)
        SetRoot(ev - EV_ROOT1, on);
}

//  ---------------------------------------------------------------- the list

static void Task_DbgEventsList(u8 taskId);

#define tListId   data[0]
#define tWindowId data[1]
#define tCount    data[2]
#define tShown    data[3]
#define tArrows   data[4]
#define tWidth    data[5]

static void MoveCursor_DbgEvents(s32 itemIndex, bool8 onInit, struct ListMenu *list)
{
    if (!onInit)
        PlaySE(SE_SELECT);
    sArrowScroll = list->itemsAbove;
}

static void BuildLabels(void)
{
    const struct DbgRow *rows = sPages[sPage].rows;
    u8 n = sPages[sPage].count, i;

    sItems = AllocZeroed(n * sizeof(struct ListMenuItem));
    sLabels = AllocZeroed(n * DBG_LABEL);
    for (i = 0; i < n; i++)
    {
        u8 *label = sLabels + i * DBG_LABEL;
        const u8 *mark = rows[i].kind == R_TOGGLE ? (IsHeld(rows[i].arg) ? sMark_Held : sMark_Not) : sMark_Give;

        StringCopy(StringCopy(label, mark), rows[i].name);
        sItems[i].label = label;
        sItems[i].index = i;
    }
}

void DaemonsDebug_EventsStart(void)
{
    sPage = PG_EVENTS;
    StringCopy(gStringVar4, sTitles[sPage]);
}

//  VAR_RESULT: the row chosen on page sPage, DBG_BACK (B on a page under GAME EVENTS) or DBG_CLOSE (B on it).
void DaemonsDebug_EventsChoose(void)
{
    struct ListMenuTemplate template = {0};
    struct WindowTemplate win;
    u8 taskId = CreateTask(Task_DbgEventsList, 80);
    struct Task *task = &gTasks[taskId];
    s32 i, width, widest = 0;

    BuildLabels();
    task->tCount = sPages[sPage].count;
    for (i = 0; i < task->tCount; i++)
    {
        width = GetStringWidth(FONT_NORMAL, sItems[i].label, 1);
        if (width > widest)
            widest = width;
    }
    task->tShown = task->tCount < DBG_MAX_SHOWN ? task->tCount : DBG_MAX_SHOWN;
    task->tWidth = (widest + 9) / 8 + 1;
    win = SetWindowTemplateFields(0, 1, 1, task->tWidth, task->tShown * 2, 15, 0x038);
    task->tWindowId = AddWindow(&win);
    SetStdWindowBorderStyle(task->tWindowId, 0);

    template.items = sItems;
    template.moveCursorFunc = MoveCursor_DbgEvents;
    template.totalItems = task->tCount;
    template.maxShowed = task->tShown;
    template.windowId = task->tWindowId;
    template.item_X = 8;
    template.cursorPal = 2;
    template.fillValue = 1;
    template.cursorShadowPal = 3;
    template.lettersSpacing = 1;
    template.scrollMultiple = LIST_NO_MULTIPLE_SCROLL;
    template.fontId = FONT_NORMAL;

    if (sRow[sPage] + sScroll[sPage] >= task->tCount)
        sRow[sPage] = sScroll[sPage] = 0;
    sArrowScroll = sScroll[sPage];
    if (task->tCount > task->tShown)
    {
        struct ScrollArrowsTemplate arrows = {
            .firstArrowType = SCROLL_ARROW_UP, .secondArrowType = SCROLL_ARROW_DOWN, .tileTag = 2000, .palTag = 100,
        };
        arrows.firstX = arrows.secondX = 4 * task->tWidth + 8;
        arrows.firstY = 8;
        arrows.secondY = 16 * task->tShown + 10;
        arrows.fullyDownThreshold = task->tCount - task->tShown;
        task->tArrows = AddScrollIndicatorArrowPair(&arrows, &sArrowScroll);
    }
    task->tListId = ListMenuInit(&template, sScroll[sPage], sRow[sPage]);
    PutWindowTilemap(task->tWindowId);
    CopyWindowToVram(task->tWindowId, COPYWIN_FULL);
}

static void DestroyDbgEventsList(u8 taskId)
{
    struct Task *task = &gTasks[taskId];

    ListMenuGetScrollAndRow(task->tListId, &sScroll[sPage], &sRow[sPage]);
    if (task->tCount > task->tShown)
        RemoveScrollIndicatorArrowPair(task->tArrows);
    DestroyListMenuTask(task->tListId, NULL, NULL);
    Free(sItems);
    Free(sLabels);
    sItems = NULL;
    sLabels = NULL;
    ClearStdWindowAndFrameToTransparent(task->tWindowId, TRUE);
    FillWindowPixelBuffer(task->tWindowId, PIXEL_FILL(0));
    ClearWindowTilemap(task->tWindowId);
    CopyWindowToVram(task->tWindowId, COPYWIN_GFX);
    RemoveWindow(task->tWindowId);
    DestroyTask(taskId);
    ScriptContext_Enable();
}

static void Task_DbgEventsList(u8 taskId)
{
    s32 input = ListMenu_ProcessInput(gTasks[taskId].tListId);

    if (input == LIST_NOTHING_CHOSEN)
        return;
    gSpecialVar_Result = input == LIST_CANCEL ? (sPage == PG_EVENTS ? DBG_CLOSE : DBG_BACK) : input;
    PlaySE(SE_SELECT);
    DestroyDbgEventsList(taskId);
}

//  VAR_RESULT is the row (or DBG_BACK). It is done -- a page opened, an event given or taken -- and gStringVar4 says
//  what happened, for the script to show above the next list.
void DaemonsDebug_EventsDo(void)
{
    const struct DbgRow *row;
    bool8 on;

    if (gSpecialVar_Result == DBG_BACK)
    {
        sPage = PG_EVENTS;
        StringCopy(gStringVar4, sTitles[sPage]);
        return;
    }
    row = &sPages[sPage].rows[gSpecialVar_Result];
    if (row->kind == R_PAGE)
    {
        sPage = row->arg;
        StringCopy(gStringVar4, sTitles[sPage]);
        return;
    }
    on = row->kind == R_GIVE ? TRUE : !IsHeld(row->arg);
    Apply(row->arg, on);
    StringCopy(gStringVar1, row->name);
    StringExpandPlaceholders(gStringVar4, on ? sText_Given : sText_Taken);
}

#undef tListId
#undef tWindowId
#undef tCount
#undef tShown
#undef tArrows
#undef tWidth

#else

//  The release ROMs have no DEBUG menu; the specials table still names these.
void DaemonsDebug_EventsStart(void) {}
void DaemonsDebug_EventsChoose(void) {}
void DaemonsDebug_EventsDo(void) {}

#endif
