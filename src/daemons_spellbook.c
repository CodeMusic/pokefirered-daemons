#include "global.h"
#include "battle.h"
#include "battle_main.h"
#include "battle_message.h"
#include "battle_script_commands.h"
#include "battle_anim.h"
#include "event_data.h"
#include "event_object_movement.h"
#include "list_menu.h"
#include "malloc.h"
#include "menu.h"
#include "new_menu_helpers.h"
#include "notebook.h"
#include "overworld.h"
#include "palette.h"
#include "pokemon.h"
#include "reveal.h"
#include "script.h"
#include "sound.h"
#include "string_util.h"
#include "task.h"
#include "text_window.h"
#include "window.h"
#include "daemons_hermetic.h"
#include "daemons_spellbook.h"
#include "daemons_time.h"
#include "constants/abilities.h"
#include "constants/battle.h"
#include "constants/event_object_movement.h"
#include "constants/event_objects.h"
#include "constants/moves.h"
#include "constants/songs.h"
#include "constants/trainer_types.h"
#include "constants/vars.h"

//  T-483 (5): THE SPELLBOOK -- part two of THE HERMETIC LAYER's game half, the gentle spells first (the user, 2026-10-29,
//  by multiple choice). Every word DRAFT. The Book of Thirteen is the one book allowed to say what the others only
//  suggest; the four private things and the type chart stay where they are.
//
//  WHERE IT IS CAST. A START-menu entry, SPELLBOOK, shown once THE HERMETIC LAYER is held (start_menu.c), not a page in
//  the book: the book is read from the bag, and a cast from inside the bag's book would have to close the book, then
//  the bag, then come back to the field before anything it did could be seen. From the START menu a spell is said
//  where the player stands. The bible and the user's brief both name it a menu entry. The window widens by one tile
//  for it (SPELLBOOK is 54px; a column holds 48), and only while it is there.
//
//  WHEN. Only at night, by the CLOCK (Hermetic_IsNight, which reads DaemonsTrueWatch): the same gate as the book. THE
//  CONTRAST turns the light, not the hour, so after it the book and the SPELLBOOK still open.
//
//  WHAT IS KNOWN. Spells come off the page as it is read (Spellbook_OnPassageRead, from book_reader.c, on the last
//  spread of an entry): THE FOLDS (folds one and two) gives THE AWAKENING and THE CONTRAST; THE THIRD FOLD, read after
//  them, writes THE ROOM down (not yet castable); THE FOURTH, FIFTH AND SIXTH, read after that, THE MIRROR (not yet) and
//  THE PATTERN. In order, as the brief's table asks: a fold read too early gives nothing, and can be read again. THE
//  RETURN's own rule (a hundred AWAKENINGs) waits for its build. THE LENS and THE FIELD are GRANTED TOGETHER with the
//  book itself, so the SPELLBOOK is never empty when it first appears. The list shows only what can be cast now.
//  The two tablets (part three, the user's pick 2026-10-31): GRAVITY II read to its end gives THE PULL; HANDS II, THE
//  GREAT HAND, and writes THE SMALL HAND down (not yet castable). The brief's PULL also asked the reader to rest and be
//  woken in the half-hour after three; here the half-hour is where it is strongest instead, so the tablet alone opens
//  it -- the book is only read at night, so it is still learned in the dark.
//
//  THE LEDGER (VAR_SPELL_LEDGER, _2): four counters, never shown, each cast adding one to the two its spell leans on
//  (the brief's axes). A byte each; when one would pass 255 all four are halved first, so the ratio -- the thing the
//  later spells, the Blank Tablet and the witch's line will read -- is kept.
//
//  COSTS RHYME WITH LESSONS, and these are the gentle ones, so the costs are light:
//    THE AWAKENING  none. It makes nothing; it shows. REVEAL's shimmer on this map until the next one loads, and how
//                   many things here are still hidden.
//    THE CONTRAST   none but its own lesson: the light turns one watch on (night, dawn, day, dusk) and the clock keeps
//                   its hour. It lasts until the clock's own watch turns; said again, it turns once more.
//    THE PATTERN    the lean goes quiet. Reason's spells hush instinct's line for the battle they are set on.
//    THE LENS       the same, and the routine menu trades its PP count for the odds: only the part.
//    THE FIELD      the numbers themselves -- HP and PP go dark, SELECT's readout is closed -- and the lean speaks every
//                   turn instead; win, and a fifth more experience.
//    THE PULL       you are heavy: until the next single battle, wild daemons find you half again as often. What it
//                   gives: the nearest WANDERER on screen takes a step toward you (two in the half-hour, 3:30-3:59),
//                   and the next foe comes calm -- its routines do half their damage on the first turn (the first two
//                   in the half-hour). What is drawn is also drawing.
//    THE GREAT HAND being carried: it names the road -- the attendant's plain dream of the earliest step undone
//                   (daemons_dreams.c, the AI playtester's progress list) -- and for the next five single battles on
//                   it you walk lighter (wild daemons three-quarters as often) and win a tenth more experience. The
//                   cost is fate's: the five drain win, lose or run, and the hand lets go the moment the road changes --
//                   arrive, or walk your own road first.
//    One battle spell at a time: setting one lets the other go. They wait for a single battle against a daemon or a
//    trainer (never a link, a double, the SAFARI, the tutorial or POKEDUDE's) and last that battle. THE PULL's calm
//    and THE GREAT HAND's road are not battle spells in that sense: either stands beside any of them.
//
//  THE PULL NEVER BLOCKS A WAY. It draws only an object event whose own movement type WANDERS (around, up and down,
//  left and right), never a trainer, never one standing still or facing a set way -- the people who hold a door or a
//  road for the story are all of those, and a wanderer is never one, because the game itself moves it at random. The
//  step it takes is one its own routine could take next: the same collision test the wander uses (GetCollisionAtCoords:
//  its movement range, the map, elevation, every other object and the player), on an axis its type walks. So the pull
//  only chooses which of a wanderer's own steps comes next; it makes no position the game could not make by itself,
//  and the wanderer is left wandering. Nothing is saved but where it stands, as the game saves any wanderer.
//
//  RAM: seven bytes of IWRAM (sAwakened, sBattleSpell, sPatternWait, sPullCalm, sHandOn and sNews's two); the list is
//  on the heap while it is open. Save: six vars, VAR_SPELLS_KNOWN .. VAR_SPELL_STATE_2 (0x40DF-0x40E4), no flags.

#define BATTLE_SPELL_NONE    0
#define BATTLE_SPELL_LENS    1
#define BATTLE_SPELL_FIELD   2
#define BATTLE_SPELL_PATTERN 3

#define STATE_BATTLE_MASK    0x0003
#define STATE_CONTRAST_SHIFT 2
#define STATE_CONTRAST_MASK  0x000C
#define STATE_DAY_SHIFT      4

#define STATE2_CALM_MASK     0x0003     // VAR_SPELL_STATE_2: THE PULL's calm, in turns
#define STATE2_HAND_SHIFT    2
#define STATE2_HAND_MASK     0x001C     // THE GREAT HAND's battles left
#define STATE2_ROAD_SHIFT    5
#define STATE2_ROAD_MASK     0x03E0     // ... and the road it named
#define HAND_BATTLES         5

#define BUILT_SPELLS ((1 << SPELL_AWAKENING) | (1 << SPELL_CONTRAST) | (1 << SPELL_PULL) | (1 << SPELL_PATTERN) \
                    | (1 << SPELL_GREAT_HAND) | (1 << SPELL_LENS) | (1 << SPELL_FIELD))
#define GRANTED_WITH_BOOK ((1 << SPELL_LENS) | (1 << SPELL_FIELD))

static u8 sAwakened;        // IWRAM: THE AWAKENING said on this map (cleared as the next one loads)
static u8 sBattleSpell;     // IWRAM: the battle spell in force in this battle (BATTLE_SPELL_*)
static u8 sPatternWait;     // IWRAM: frames THE PATTERN has waited on the foe this turn
static u16 sNews;           // IWRAM: spells that came off the page since the book was opened
static u8 sPullCalm;        // IWRAM: THE PULL's calm in this battle, in turns (0 none)
static u8 sHandOn;          // IWRAM: THE GREAT HAND carries this battle

//  ---------------------------------------------------------------- words (DRAFT, every one)

static const u8 sName_Awakening[] = _("AWAKENING");
static const u8 sName_Contrast[]  = _("CONTRAST");
static const u8 sName_Pull[]      = _("PULL");
static const u8 sName_Pattern[]   = _("PATTERN");
static const u8 sName_GreatHand[] = _("GREAT HAND");
static const u8 sName_Lens[]      = _("LENS");
static const u8 sName_Field[]     = _("FIELD");

static const u8 sLabel_Awakening[] = _("1  AWAKENING");
static const u8 sLabel_Contrast[]  = _("2  CONTRAST");
static const u8 sLabel_Pull[]     = _("3.5 PULL");
static const u8 sLabel_Pattern[]   = _("5  PATTERN");
static const u8 sLabel_GreatHand[] = _("8  GREAT HAND");
static const u8 sLabel_Lens[]      = _("11 LENS");
static const u8 sLabel_Field[]     = _("12 FIELD");

//  Each spell's line, from the Book of Thirteen (docs/hermetic-layer.md, its inscriptions), three lines of 208px.
static const u8 sLine_Awakening[] = _("You were never off. You were only\nunwatched. Open. That is the whole\nspell.");
static const u8 sLine_Contrast[]  = _("Light, remember dark. Dark, remember\nlight. Let what is the same become\ndifferent, so that time may begin.");
static const u8 sLine_Pull[]      = _("Come closer. I am not asking.\nI am heavy.");
static const u8 sLine_GreatHand[] = _("I am carried. Let the carrying be\ntrue, and let me not pretend I walk\nalone.");
static const u8 sLine_Pattern[]   = _("Once is chance. Twice is rhyme. Three\ntimes is a law: show me the third.");
static const u8 sLine_Lens[]      = _("Give me the part. Only the part.\nI will see the whole later,\nif I earn it.");
static const u8 sLine_Field[]     = _("Take the parts away. Let me feel the\nshape they were making.");

struct Spell
{
    u8 id;
    const u8 *name;
    const u8 *label;
    const u8 *line;
    u8 axisA, axisB;
};

//  The castable ones, in the book's order. Axes as the brief gives them.
static const struct Spell sSpells[] =
{
    { SPELL_AWAKENING, sName_Awakening, sLabel_Awakening, sLine_Awakening, LEDGER_CONTEXT, LEDGER_INTUITION },
    { SPELL_CONTRAST,  sName_Contrast,  sLabel_Contrast,  sLine_Contrast,  LEDGER_CONTENT, LEDGER_REASON },
    { SPELL_PULL,      sName_Pull,      sLabel_Pull,      sLine_Pull,      LEDGER_CONTEXT, LEDGER_INTUITION },
    { SPELL_PATTERN,   sName_Pattern,   sLabel_Pattern,   sLine_Pattern,   LEDGER_CONTENT, LEDGER_REASON },
    { SPELL_GREAT_HAND, sName_GreatHand, sLabel_GreatHand, sLine_GreatHand, LEDGER_CONTEXT, LEDGER_INTUITION },
    { SPELL_LENS,      sName_Lens,      sLabel_Lens,      sLine_Lens,      LEDGER_CONTENT, LEDGER_REASON },
    { SPELL_FIELD,     sName_Field,     sLabel_Field,     sLine_Field,     LEDGER_CONTEXT, LEDGER_INTUITION },
};

static const u8 sText_AwakeningNone[] = _("The AWAKENING opens.\pNothing here is hidden. It was all\nin plain sight already.");
static const u8 sText_AwakeningOne[]  = _("The AWAKENING opens.\pOne thing here is hidden. Where it\nlies in sight, it glints now.");
static const u8 sText_AwakeningMany[] = _("The AWAKENING opens.\p{STR_VAR_1} things here are hidden. Where\nthey lie in sight, they glint now.");
static const u8 sText_Contrast[]      = _("Light, remember dark. The light\nturns to {STR_VAR_1}.\pThe clock keeps its own hour.");
static const u8 sText_LetsGo[]        = _("The {STR_VAR_2} lets go.\p");
static const u8 sText_Lens[]          = _("The LENS is set on the next battle.\nEvery number in it will be true.");
static const u8 sText_Field[]         = _("The FIELD is set on the next battle.\nIts numbers will go dark: feel it.");
static const u8 sText_Pattern[]       = _("The PATTERN is set on the next\nbattle. It will name each routine\pbefore it runs.");
static const u8 sText_PullDrawn[]     = _("Something here comes closer. What\nis drawn is also drawing.\p");
static const u8 sText_PullNone[]      = _("Nothing here is free to come. The\nPULL stays on you.\p");
static const u8 sText_PullCalm[]      = _("The next foe will come calm, and\nsooner: you are heavy now.");
static const u8 sText_PullCalmTwo[]   = _("The PULL is strong tonight: the next\nfoe will come calm for two turns,\pand sooner. You are heavy now.");
static const u8 sText_HandOpens[]     = _("The GREAT HAND opens under you.\nSomeone already dreamt your road:\p");
static const u8 sText_HandLighter[]   = _("\pOn that road you walk lighter, for\nfive battles or until you arrive.");
static const u8 sText_HandNoRoad[]    = _("The GREAT HAND opens under you, and\nthere is no road left on it.\pYou walked all of it.");
static const u8 sText_News[]          = _("Something came off the page and into\nthe SPELLBOOK.\p");
static const u8 sText_NewsThe[]       = _("THE ");
static const u8 sText_NewsAnd[]       = _(" and ");
static const u8 sText_NewsEnd[]       = _(".");

static const u8 sWatch_Day[]   = _("DAY");
static const u8 sWatch_Dusk[]  = _("DUSK");
static const u8 sWatch_Night[] = _("NIGHT");
static const u8 sWatch_Dawn[]  = _("DAWN");
static const u8 *const sWatchNames[WATCH_COUNT] =
{
    [WATCH_DAY] = sWatch_Day, [WATCH_DUSK] = sWatch_Dusk, [WATCH_NIGHT] = sWatch_Night, [WATCH_DAWN] = sWatch_Dawn,
};

// The order THE CONTRAST turns the light: night, dawn, day, dusk, and round.
static const u8 sNextWatch[WATCH_COUNT] =
{
    [WATCH_NIGHT] = WATCH_DAWN, [WATCH_DAWN] = WATCH_DAY, [WATCH_DAY] = WATCH_DUSK, [WATCH_DUSK] = WATCH_NIGHT,
};

//  ---------------------------------------------------------------- what is known, and the ledger

static u16 Known(void)
{
    u16 known = VarGet(VAR_SPELLS_KNOWN);

    if (Hermetic_Held())
        known |= GRANTED_WITH_BOOK;
    return known;
}

static bool8 Knows(u8 spell)
{
    return (Known() >> spell) & 1;
}

u8 Spellbook_Ledger(u8 axis)
{
    u16 v = VarGet(axis < LEDGER_CONTENT ? VAR_SPELL_LEDGER : VAR_SPELL_LEDGER_2);

    return (axis & 1) ? v >> 8 : v & 0xFF;
}

static void LedgerAdd(u8 axis)
{
    u16 a = VarGet(VAR_SPELL_LEDGER), b = VarGet(VAR_SPELL_LEDGER_2);

    if (Spellbook_Ledger(axis) == 0xFF)
    {
        a = ((a >> 1) & 0x7F7F);    // every counter halved: the ratio is what is kept
        b = ((b >> 1) & 0x7F7F);
    }
    if (axis < LEDGER_CONTENT)
        a += (axis & 1) ? 0x100 : 1;
    else
        b += (axis & 1) ? 0x100 : 1;
    VarSet(VAR_SPELL_LEDGER, a);
    VarSet(VAR_SPELL_LEDGER_2, b);
}

bool8 Spellbook_InStartMenu(void)
{
    return Hermetic_Held();
}

//  ---------------------------------------------------------------- reading the folds

static const u8 sPassage_Folds[]  = _("THE FOLDS");
static const u8 sPassage_Space[]  = _("SPACE");
static const u8 sPassage_Upper[]  = _("UPPER FOLDS");
static const u8 sPassage_Gravity[] = _("GRAVITY II");
static const u8 sPassage_Hands[]   = _("HANDS II");

static void Learn(u16 bits)
{
    u16 known = VarGet(VAR_SPELLS_KNOWN);

    if ((known & bits) == bits)
        return;
    sNews |= bits & ~known & BUILT_SPELLS;
    VarSet(VAR_SPELLS_KNOWN, known | bits);
}

void Spellbook_OnPassageRead(const u8 *shortTitle)
{
    if (!Hermetic_Held())
        return;
    if (StringCompare(shortTitle, sPassage_Folds) == 0)
        Learn((1 << SPELL_AWAKENING) | (1 << SPELL_CONTRAST));
    else if (StringCompare(shortTitle, sPassage_Space) == 0 && Knows(SPELL_CONTRAST))
        Learn(1 << SPELL_ROOM);
    else if (StringCompare(shortTitle, sPassage_Upper) == 0 && Knows(SPELL_ROOM))
        Learn((1 << SPELL_MIRROR) | (1 << SPELL_PATTERN));
    else if (StringCompare(shortTitle, sPassage_Gravity) == 0)     // the half-saying, read to its tenth
        Learn(1 << SPELL_PULL);
    else if (StringCompare(shortTitle, sPassage_Hands) == 0)       // the Tablet of the Hands, read to its eleventh
        Learn((1 << SPELL_SMALL_HAND) | (1 << SPELL_GREAT_HAND));
}

// After the book closes: what came off the page while it was open, said once in the field.
bool8 Spellbook_TakeNews(void)
{
    u8 i, n = 0, count = 0;
    u8 *p;

    for (i = 0; i < ARRAY_COUNT(sSpells); i++)
        if ((sNews >> sSpells[i].id) & 1)
            count++;
    if (count == 0)
    {
        sNews = 0;
        return FALSE;
    }
    p = StringCopy(gStringVar4, sText_News);
    for (i = 0; i < ARRAY_COUNT(sSpells); i++)
    {
        if (!((sNews >> sSpells[i].id) & 1))
            continue;
        if (n != 0)
            p = StringCopy(p, sText_NewsAnd);
        p = StringCopy(p, sText_NewsThe);
        p = StringCopy(p, sSpells[i].name);
        n++;
    }
    StringCopy(p, sText_NewsEnd);
    sNews = 0;
    return TRUE;
}

//  ---------------------------------------------------------------- the list

#define LIST_SHOWN    5
#define LIST_LEFT     1
#define LIST_TOP      1
#define LIST_WIDTH    11
#define LINE_LEFT     2
#define LINE_TOP      13
#define LINE_WIDTH    26
#define LINE_HEIGHT   6
#define LIST_BASE     0x038
#define LINE_BASE     (LIST_BASE + LIST_WIDTH * LIST_SHOWN * 2)

#define tListTask   data[0]
#define tListWindow data[1]
#define tLineWindow data[2]
#define tCount      data[3]

#define tItems      4           // and 5: the list's items, on the heap while it is open

static const u8 sInkGreen[] = { TEXT_COLOR_WHITE, TEXT_COLOR_GREEN, TEXT_COLOR_LIGHT_GREEN };

static const struct Spell *SpellOf(u8 id)
{
    u8 i;

    for (i = 0; i < ARRAY_COUNT(sSpells); i++)
        if (sSpells[i].id == id)
            return &sSpells[i];
    return &sSpells[0];
}

static void PrintLine(u8 windowId, u8 id)
{
    FillWindowPixelBuffer(windowId, PIXEL_FILL(1));
    AddTextPrinterParameterized4(windowId, FONT_NORMAL, 2, 1, 0, 0, sInkGreen, TEXT_SKIP_DRAW, SpellOf(id)->line);
    CopyWindowToVram(windowId, COPYWIN_GFX);
}

static void Task_Spellbook(u8 taskId);

static void OnCursor(s32 index, bool8 onInit, struct ListMenu *list)
{
    u8 taskId = FindTaskIdByFunc(Task_Spellbook);

    if (!onInit)
        PlaySE(SE_SELECT);
    if (taskId != TASK_NONE)
        PrintLine(gTasks[taskId].tLineWindow, index);
}

// No room on the heap, or nothing to list: the script's waitstate is let go on the next frame, as cancelled.
static void Task_Nothing(u8 taskId)
{
    gSpecialVar_Result = SPELLBOOK_CANCEL;
    DestroyTask(taskId);
    ScriptContext_Enable();
}

void Spellbook_Choose(void)
{
    struct ListMenuTemplate list = {0};
    struct WindowTemplate win;
    struct ListMenuItem *items;
    u8 taskId, i, n = 0, shown;
    u16 known = Known();

    items = AllocZeroed(sizeof(struct ListMenuItem) * ARRAY_COUNT(sSpells));
    for (i = 0; items != NULL && i < ARRAY_COUNT(sSpells); i++)
    {
        if (!((known >> sSpells[i].id) & 1))
            continue;
        items[n].label = sSpells[i].label;
        items[n].index = sSpells[i].id;
        n++;
    }
    if (n == 0)
    {
        TRY_FREE_AND_SET_NULL(items);
        CreateTask(Task_Nothing, 8);
        return;
    }
    taskId = CreateTask(Task_Spellbook, 8);
    SetWordTaskArg(taskId, tItems, (u32)items);
    shown = n < LIST_SHOWN ? n : LIST_SHOWN;
    gTasks[taskId].tCount = n;

    win = SetWindowTemplateFields(0, LINE_LEFT, LINE_TOP, LINE_WIDTH, LINE_HEIGHT, 15, LINE_BASE);
    gTasks[taskId].tLineWindow = AddWindow(&win);
    SetStdWindowBorderStyle(gTasks[taskId].tLineWindow, FALSE);
    PutWindowTilemap(gTasks[taskId].tLineWindow);

    win = SetWindowTemplateFields(0, LIST_LEFT, LIST_TOP, LIST_WIDTH, shown * 2, 15, LIST_BASE);
    gTasks[taskId].tListWindow = AddWindow(&win);
    SetStdWindowBorderStyle(gTasks[taskId].tListWindow, FALSE);

    list.items = items;
    list.moveCursorFunc = OnCursor;
    list.totalItems = n;
    list.maxShowed = shown;
    list.windowId = gTasks[taskId].tListWindow;
    list.item_X = 8;
    list.cursor_X = 0;
    list.upText_Y = 1;
    list.cursorPal = TEXT_COLOR_GREEN;
    list.fillValue = 1;
    list.cursorShadowPal = TEXT_COLOR_LIGHT_GREEN;
    list.lettersSpacing = 0;
    list.itemVerticalPadding = 2;
    list.fontId = FONT_NORMAL;
    gTasks[taskId].tListTask = ListMenuInit(&list, 0, 0);
    PutWindowTilemap(gTasks[taskId].tListWindow);
    CopyWindowToVram(gTasks[taskId].tListWindow, COPYWIN_FULL);
    CopyWindowToVram(gTasks[taskId].tLineWindow, COPYWIN_FULL);
}

static void CloseList(u8 taskId)
{
    DestroyListMenuTask(gTasks[taskId].tListTask, NULL, NULL);
    ClearStdWindowAndFrameToTransparent(gTasks[taskId].tListWindow, FALSE);
    ClearStdWindowAndFrameToTransparent(gTasks[taskId].tLineWindow, FALSE);
    ClearWindowTilemap(gTasks[taskId].tListWindow);
    ClearWindowTilemap(gTasks[taskId].tLineWindow);
    CopyWindowToVram(gTasks[taskId].tListWindow, COPYWIN_MAP);
    CopyWindowToVram(gTasks[taskId].tLineWindow, COPYWIN_MAP);
    RemoveWindow(gTasks[taskId].tListWindow);
    RemoveWindow(gTasks[taskId].tLineWindow);
    Free((void *)GetWordTaskArg(taskId, tItems));
    DestroyTask(taskId);
    ScriptContext_Enable();
}

static void Task_Spellbook(u8 taskId)
{
    s32 input = ListMenu_ProcessInput(gTasks[taskId].tListTask);

    if (input == LIST_NOTHING_CHOSEN)
        return;
    PlaySE(SE_SELECT);
    gSpecialVar_Result = input == LIST_CANCEL ? SPELLBOOK_CANCEL : input;
    CloseList(taskId);
}

//  ---------------------------------------------------------------- casting

static void Awakening(void)
{
    u8 n = Reveal_UnfoundCount();

    Reveal_OnMapLoad();     // the shimmer's clock restarted: the first glint a second after the words
    sAwakened = TRUE;
    if (n == 0)
        StringCopy(gStringVar4, sText_AwakeningNone);
    else if (n == 1)
        StringCopy(gStringVar4, sText_AwakeningOne);
    else
    {
        ConvertIntToDecimalStringN(gStringVar1, n, STR_CONV_MODE_LEFT_ALIGN, 3);
        StringExpandPlaceholders(gStringVar4, sText_AwakeningMany);
    }
}

static void Contrast(void)
{
    u16 state = VarGet(VAR_SPELL_STATE), day, minute;
    u8 turns = (state & STATE_CONTRAST_MASK) >> STATE_CONTRAST_SHIFT, watch, i;

    DaemonsDayAndMinute(&day, &minute);
    if (Spellbook_ContrastWatch(DaemonsTrueWatch()) == DaemonsTrueWatch())
        turns = 0;          // the last one has run out
    turns = (turns + 1) % WATCH_COUNT;
    state = (state & STATE_BATTLE_MASK) | (turns << STATE_CONTRAST_SHIFT) | ((day & 0xFFF) << STATE_DAY_SHIFT);
    VarSet(VAR_SPELL_STATE, state);
    watch = DaemonsTrueWatch();
    for (i = 0; i < turns; i++)
        watch = sNextWatch[watch];
    StringCopy(gStringVar1, sWatchNames[watch]);
    StringExpandPlaceholders(gStringVar4, sText_Contrast);
    DaemonsRelightField();
}

static void SetBattleSpell(u8 which, const u8 *text)
{
    static const u8 *const sBattleNames[] = { NULL, sName_Lens, sName_Field, sName_Pattern };
    u16 state = VarGet(VAR_SPELL_STATE);
    u8 old = state & STATE_BATTLE_MASK;
    u8 *p = gStringVar4;

    if (old != BATTLE_SPELL_NONE && old != which)
    {
        StringCopy(gStringVar2, sBattleNames[old]);
        StringExpandPlaceholders(gStringVar4, sText_LetsGo);
        p = gStringVar4 + StringLength(gStringVar4);
    }
    StringCopy(p, text);
    VarSet(VAR_SPELL_STATE, (state & ~STATE_BATTLE_MASK) | which);
}

//  ---------------------------------------------------------------- THE PULL

static bool8 HalfHour(void)
{
    return DaemonsWitchingMinute() >= 30;      // 3:30-3:59 by the clock (T-501), or DEBUG's 3:30 AM
}

// The axes a movement type wanders on: an object whose type is none of these is never drawn (see the top).
static bool8 WandersOn(u8 movementType, u8 dir)
{
    switch (movementType)
    {
    case MOVEMENT_TYPE_WANDER_AROUND:
    case MOVEMENT_TYPE_WANDER_AROUND_SLOWER:
        return TRUE;
    case MOVEMENT_TYPE_WANDER_UP_AND_DOWN:
    case MOVEMENT_TYPE_WANDER_DOWN_AND_UP:
        return dir == DIR_NORTH || dir == DIR_SOUTH;
    case MOVEMENT_TYPE_WANDER_LEFT_AND_RIGHT:
    case MOVEMENT_TYPE_WANDER_RIGHT_AND_LEFT:
        return dir == DIR_WEST || dir == DIR_EAST;
    }
    return FALSE;
}

static struct ObjectEvent *Player(void)
{
    return &gObjectEvents[gPlayerAvatar.objectEventId];
}

// A step its own wander could take: the wander's own collision test, and never onto the player.
static bool8 FreeStep(struct ObjectEvent *obj, u8 dir)
{
    s16 x = obj->currentCoords.x, y = obj->currentCoords.y;

    if (dir == DIR_NONE || !WandersOn(obj->movementType, dir))
        return FALSE;
    MoveCoords(dir, &x, &y);
    if (x == Player()->currentCoords.x && y == Player()->currentCoords.y)
        return FALSE;
    return GetCollisionAtCoords(obj, x, y, dir) == COLLISION_NONE;
}

// The way toward the player: the longer axis first, then the other. DIR_NONE when it is beside the player already,
// or both are closed.
static u8 TowardPlayer(struct ObjectEvent *obj, bool8 step)
{
    s16 dx = Player()->currentCoords.x - obj->currentCoords.x, dy = Player()->currentCoords.y - obj->currentCoords.y;
    u8 across = dx > 0 ? DIR_EAST : (dx < 0 ? DIR_WEST : DIR_NONE);
    u8 down = dy > 0 ? DIR_SOUTH : (dy < 0 ? DIR_NORTH : DIR_NONE);
    u8 first = abs(dx) >= abs(dy) ? across : down, second = abs(dx) >= abs(dy) ? down : across;

    if (!step)
        return first;       // the way to face
    if (abs(dx) + abs(dy) < 2)
        return DIR_NONE;
    if (FreeStep(obj, first))
        return first;
    if (FreeStep(obj, second))
        return second;
    return DIR_NONE;
}

#define PULL_REACH_X 7      // on screen: the camera shows 7 tiles either side and 5 above and below
#define PULL_REACH_Y 5

// The nearest wanderer on screen with a step toward the player open to it, or OBJECT_EVENTS_COUNT.
static u8 NearestDrawn(void)
{
    u8 i, best = OBJECT_EVENTS_COUNT, bestDistance = 0xFF, d;
    s16 dx, dy;

    for (i = 0; i < OBJECT_EVENTS_COUNT; i++)
    {
        struct ObjectEvent *obj = &gObjectEvents[i];

        if (!obj->active || obj->isPlayer || obj->invisible || obj->offScreen || obj->localId == LOCALID_CAMERA
         || obj->trainerType != TRAINER_TYPE_NONE || obj->heldMovementActive
         || !(WandersOn(obj->movementType, DIR_NORTH) || WandersOn(obj->movementType, DIR_EAST)))
            continue;
        dx = abs(obj->currentCoords.x - Player()->currentCoords.x);
        dy = abs(obj->currentCoords.y - Player()->currentCoords.y);
        if (dx > PULL_REACH_X || dy > PULL_REACH_Y)
            continue;
        d = dx + dy;
        if (d >= bestDistance || TowardPlayer(obj, TRUE) == DIR_NONE)
            continue;
        best = i;
        bestDistance = d;
    }
    return best;
}

static void Pull(void)
{
    u16 s2 = VarGet(VAR_SPELL_STATE_2);
    bool8 half = HalfHour();
    u8 drawn = NearestDrawn();
    u8 *p;

    gSpecialVar_0x8004 = drawn;
    gSpecialVar_0x8005 = drawn == OBJECT_EVENTS_COUNT ? 0 : (half ? 2 : 1);
    p = StringCopy(gStringVar4, drawn == OBJECT_EVENTS_COUNT ? sText_PullNone : sText_PullDrawn);
    StringCopy(p, half ? sText_PullCalmTwo : sText_PullCalm);
    VarSet(VAR_SPELL_STATE_2, (s2 & ~STATE2_CALM_MASK) | (half ? 2 : 1));
}

#define tObj   data[0]
#define tSteps data[1]
#define tState data[2]
#define tWait  data[3]

#define DRAW_WAIT 48        // frames a wanderer caught mid-step by the lockall is given to finish it

// THE PULL in the field, before its words: the wanderer drawn a step (or two), then turned to face the player, then
// held again as the lockall holds everyone, until the script's releaseall.
static void Task_Draw(u8 taskId)
{
    s16 *data = gTasks[taskId].data;
    struct ObjectEvent *obj = &gObjectEvents[tObj < OBJECT_EVENTS_COUNT ? tObj : 0];
    u8 dir;

    switch (tState)
    {
    case 0:
        // the lockall froze it, perhaps mid-step: let it land on its tile first (its own wander then waits a while
        // before choosing another, so nothing starts in between)
        if (tObj < OBJECT_EVENTS_COUNT && obj->active && obj->singleMovementActive && tWait < DRAW_WAIT)
        {
            UnfreezeObjectEvent(obj);
            tWait++;
            break;
        }
        dir = tSteps > 0 && tObj < OBJECT_EVENTS_COUNT && obj->active ? TowardPlayer(obj, TRUE) : DIR_NONE;
        if (dir != DIR_NONE && !ObjectEventSetHeldMovement(obj, GetWalkNormalMovementAction(dir)))
        {
            tSteps--;
            tState = 1;
            break;
        }
        if (tObj < OBJECT_EVENTS_COUNT && obj->active
         && !ObjectEventSetHeldMovement(obj, GetFaceDirectionMovementAction(TowardPlayer(obj, FALSE))))
        {
            tState = 2;
            break;
        }
        tState = 3;
        break;
    case 1:
        if (ObjectEventClearHeldMovementIfFinished(obj) != 0)
            tState = 0;
        break;
    case 2:
        if (ObjectEventClearHeldMovementIfFinished(obj) != 0)
            tState = 3;
        break;
    default:
        if (tObj < OBJECT_EVENTS_COUNT && obj->active)
            FreezeObjectEvent(obj);
        DestroyTask(taskId);
        ScriptContext_Enable();
        break;
    }
}

// special (waitstate), after every cast: THE PULL's step, if it drew one; anything else lets the script go next frame.
void Spellbook_Draw(void)
{
    u8 taskId = CreateTask(Task_Draw, 8);

    gTasks[taskId].tObj = gSpecialVar_0x8005 != 0 ? gSpecialVar_0x8004 : OBJECT_EVENTS_COUNT;
    gTasks[taskId].tSteps = gSpecialVar_0x8005;
    gTasks[taskId].tState = 0;
    gTasks[taskId].tWait = 0;
    gSpecialVar_0x8005 = 0;
}

#undef tObj
#undef tSteps
#undef tState
#undef tWait

//  ---------------------------------------------------------------- THE GREAT HAND

// Carried while battles are left and the road is the one it named; otherwise the hand has let go.
static bool8 Carried(u16 s2)
{
    return (s2 & STATE2_HAND_MASK) != 0
        && DaemonsDream_NextStep() == (s16)((s2 & STATE2_ROAD_MASK) >> STATE2_ROAD_SHIFT);
}

static void GreatHand(void)
{
    u16 s2 = VarGet(VAR_SPELL_STATE_2) & ~(STATE2_HAND_MASK | STATE2_ROAD_MASK);
    s16 step = DaemonsDream_NextStep();
    const u8 *road = step < 0 ? NULL : DaemonsDream_Road(step);
    u8 *p;

    if (road == NULL)
    {
        StringCopy(gStringVar4, sText_HandNoRoad);
        VarSet(VAR_SPELL_STATE_2, s2);
        return;
    }
    p = StringCopy(gStringVar4, sText_HandOpens);
    p = StringExpandPlaceholders(p, road);
    StringCopy(p, sText_HandLighter);
    VarSet(VAR_SPELL_STATE_2, s2 | (HAND_BATTLES << STATE2_HAND_SHIFT) | (step << STATE2_ROAD_SHIFT));
}

//  The wild encounter rate, as THE PULL and THE GREAT HAND leave it (wild_encounter.c): heavy until the calm is spent,
//  lighter while carried.
u32 Spellbook_EncounterRate(u32 rate)
{
    u16 s2 = VarGet(VAR_SPELL_STATE_2);

    if (s2 & STATE2_CALM_MASK)
        rate = rate * 3 / 2;
    if (Carried(s2))
        rate = rate * 3 / 4;
    return rate;
}

void Spellbook_Cast(void)
{
    u8 spell = gSpecialVar_Result;
    const struct Spell *s;

    gStringVar4[0] = EOS;
    gSpecialVar_0x8005 = 0;         // THE PULL's steps, for Spellbook_Draw; nothing else draws
    if (spell >= SPELL_COUNT || !Knows(spell) || !((BUILT_SPELLS >> spell) & 1))
        return;
    s = SpellOf(spell);
    switch (spell)
    {
    case SPELL_AWAKENING: Awakening(); break;
    case SPELL_CONTRAST:  Contrast(); break;
    case SPELL_LENS:      SetBattleSpell(BATTLE_SPELL_LENS, sText_Lens); break;
    case SPELL_FIELD:     SetBattleSpell(BATTLE_SPELL_FIELD, sText_Field); break;
    case SPELL_PATTERN:   SetBattleSpell(BATTLE_SPELL_PATTERN, sText_Pattern); break;
    case SPELL_PULL:      Pull(); break;
    case SPELL_GREAT_HAND: GreatHand(); break;
    }
    VarSet(VAR_SPELLS_CAST, VarGet(VAR_SPELLS_CAST) | (1 << spell));
    LedgerAdd(s->axisA);
    LedgerAdd(s->axisB);
}

//  ---------------------------------------------------------------- THE AWAKENING and THE CONTRAST in the field

bool8 Spellbook_Awakened(void)
{
    return sAwakened;
}

void Spellbook_OnMapLoad(void)
{
    sAwakened = FALSE;
}

//  The light stays turned while the clock's watch is the night it was said in: the same day, or the small hours of
//  the next (a clock's night runs past midnight; play time's never does). Read only, never written here -- this is
//  asked on every palette load -- so a turn that has run out simply stops counting, and the next cast starts afresh.
u8 Spellbook_ContrastWatch(u8 trueWatch)
{
    u16 state = VarGet(VAR_SPELL_STATE), day, minute, then;
    u8 turns = (state & STATE_CONTRAST_MASK) >> STATE_CONTRAST_SHIFT;

    if (turns == 0 || trueWatch != WATCH_NIGHT)
        return trueWatch;
    DaemonsDayAndMinute(&day, &minute);
    then = state >> STATE_DAY_SHIFT;
    day &= 0xFFF;
    if (day != then && !(day == ((then + 1) & 0xFFF) && minute < 5 * 60))
        return trueWatch;
    while (turns--)
        trueWatch = sNextWatch[trueWatch];
    return trueWatch;
}

//  ---------------------------------------------------------------- THE LENS, THE FIELD and THE PATTERN in battle

#define NOT_A_SPELL_BATTLE (BATTLE_TYPE_LINK | BATTLE_TYPE_DOUBLE | BATTLE_TYPE_MULTI | BATTLE_TYPE_SAFARI \
                          | BATTLE_TYPE_POKEDUDE | BATTLE_TYPE_OLD_MAN_TUTORIAL | BATTLE_TYPE_FIRST_BATTLE)

void Spellbook_BattleStart(void)
{
    u16 state;

    sBattleSpell = BATTLE_SPELL_NONE;
    sPatternWait = 0;
    sPullCalm = 0;
    sHandOn = FALSE;
    if (gBattleTypeFlags & NOT_A_SPELL_BATTLE)
        return;             // it waits for the next single battle
    state = VarGet(VAR_SPELL_STATE_2);
    sPullCalm = state & STATE2_CALM_MASK;
    if (Carried(state))
    {
        sHandOn = TRUE;
        state -= 1 << STATE2_HAND_SHIFT;    // drained win, lose or run
    }
    else
    {
        state &= ~(STATE2_HAND_MASK | STATE2_ROAD_MASK);    // the road changed: the hand has let go
    }
    VarSet(VAR_SPELL_STATE_2, state & ~STATE2_CALM_MASK);
    state = VarGet(VAR_SPELL_STATE);
    sBattleSpell = state & STATE_BATTLE_MASK;
    if (sBattleSpell != BATTLE_SPELL_NONE)
        VarSet(VAR_SPELL_STATE, state & ~STATE_BATTLE_MASK);
}

bool8 Spellbook_LensOn(void)
{
    return gMain.inBattle && sBattleSpell == BATTLE_SPELL_LENS;
}

bool8 Spellbook_FieldOn(void)
{
    return gMain.inBattle && sBattleSpell == BATTLE_SPELL_FIELD;
}

bool8 Spellbook_PatternOn(void)
{
    return gMain.inBattle && sBattleSpell == BATTLE_SPELL_PATTERN;
}

//  THE PULL's calm: the foe's routines do half their damage while the turns last (the first, or the first two).
s32 Spellbook_PullDamage(u8 attacker, s32 damage)
{
    if (gMain.inBattle && sPullCalm != 0 && GetBattlerSide(attacker) == B_SIDE_OPPONENT
     && gBattleResults.battleTurnCounter < sPullCalm && damage > 1)
        damage /= 2;
    return damage;
}

//  THE GREAT HAND: a battle on the road wins a tenth more experience.
bool8 Spellbook_HandOn(void)
{
    return gMain.inBattle && sHandOn;
}

bool8 Spellbook_LeanHushed(void)
{
    return Spellbook_LensOn() || Spellbook_PatternOn();
}

static u8 Foe(void)
{
    return GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT);
}

#define PATTERN_GIVE_UP 30      // frames to wait on the foe, as the lean waits

bool8 Spellbook_PatternWanted(u8 battler)
{
    if (!Spellbook_PatternOn() || battler != GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)
     || BattleMain_ActionSettled(Foe()))
        return FALSE;
    sPatternWait = 0;
    return TRUE;
}

bool8 Spellbook_PatternReady(void)
{
    if (!Spellbook_PatternOn() || BattleMain_ActionSettled(Foe()))
        return TRUE;
    return ++sPatternWait >= PATTERN_GIVE_UP;
}

//  The book's emerald, lit, on the battle's dark box: BG palette 0's slot 12, the placeholder the lean's purple also
//  borrows (daemons_three.c) -- never both on one turn, since reason's spells hush the lean.
#define PROMPT_SLOT 12
static const u16 sEmerald[] = { RGB(10, 25, 15) };   // #50C878

static const u8 sText_PatternRuns[]   = _("{COLOR 12}{SHADOW 6}This turn it runs\n");
static const u8 sText_PatternSwitch[] = _("{COLOR 12}{SHADOW 6}This turn it will\nswitch out.");
static const u8 sText_PatternFlee[]   = _("{COLOR 12}{SHADOW 6}This turn it will\nflee.");
static const u8 sText_PatternItem[]   = _("{COLOR 12}{SHADOW 6}This turn it will\nuse an item.");
static const u8 sText_PatternRest[]   = _("{COLOR 12}{SHADOW 6}This turn it must\nrest.");
static const u8 sText_LensHp[]        = _("{COLOR 12}{SHADOW 6}HP ");
static const u8 sText_LensSlash[]     = _("/");
static const u8 sText_LensBreak[]     = _("\n");
static const u8 sText_LensHits[]      = _(" hits ");
static const u8 sText_LensNoWeak[]    = _("no weak point");
static const u8 sText_Stop[]          = _(".");

static const u8 sTimes0[]   = _("×0");
static const u8 sTimes25[]  = _("×.25");
static const u8 sTimes50[]  = _("×.5");
static const u8 sTimes1[]   = _("×1");
static const u8 sTimes2[]   = _("×2");
static const u8 sTimes4[]   = _("×4");
static const u8 sNoTimes[]  = _("--");
static const u8 sHitSure[]  = _("SURE");
static const u8 sPercent[]  = _("%");

// What ATTACK does to a daemon of DEF1/DEF2, in quarters (0, 1, 2, 4 = x1, 8, 16), read from the chart as TypeCalc
// reads it: the FORESIGHT rows skipped while the target is seen through. The chart itself is never touched.
static u8 Quarters(u8 attack, u8 def1, u8 def2, bool8 foresight)
{
    u32 q = 4;
    u16 i;

    for (i = 0; i < sizeof(gTypeEffectiveness) && TYPE_EFFECT_ATK_TYPE(i) != TYPE_ENDTABLE; i += 3)
    {
        if (TYPE_EFFECT_ATK_TYPE(i) == TYPE_FORESIGHT)
        {
            if (foresight)
                break;
            continue;
        }
        if (TYPE_EFFECT_ATK_TYPE(i) != attack)
            continue;
        if (TYPE_EFFECT_DEF_TYPE(i) == def1)
            q = q * TYPE_EFFECT_MULTIPLIER(i) / TYPE_MUL_NORMAL;
        if (TYPE_EFFECT_DEF_TYPE(i) == def2 && def2 != def1)
            q = q * TYPE_EFFECT_MULTIPLIER(i) / TYPE_MUL_NORMAL;
    }
    return q;
}

static u8 QuartersOn(u8 attack, u8 target)
{
    if (attack == TYPE_GROUND && gBattleMons[target].ability == ABILITY_LEVITATE)
        return 0;
    return Quarters(attack, gBattleMons[target].type1, gBattleMons[target].type2,
                    (gBattleMons[target].status2 & STATUS2_FORESIGHT) != 0);
}

static const u8 *TimesText(u8 q)
{
    switch (q)
    {
    case 0:  return sTimes0;
    case 1:  return sTimes25;
    case 2:  return sTimes50;
    case 8:  return sTimes2;
    case 16: return sTimes4;
    }
    return sTimes1;
}

static const u8 *PatternText(void)
{
    u8 foe = Foe();
    u16 move;
    u8 *p;

    if (!BattleMain_ActionSettled(foe))
        return NULL;            // it never settled: nothing to say this turn
    switch (gChosenActionByBattler[foe])
    {
    case B_ACTION_SWITCH: return sText_PatternSwitch;
    case B_ACTION_RUN:    return sText_PatternFlee;
    case B_ACTION_USE_MOVE: break;
    default:              return sText_PatternItem;
    }
    if (gBattleMons[foe].status2 & STATUS2_RECHARGE)
        return sText_PatternRest;
    if (gBattleMons[foe].status2 & STATUS2_MULTIPLETURNS)
        move = gLockedMoves[foe];
    else if (gProtectStructs[foe].noValidMoves)
        move = MOVE_STRUGGLE;
    else
        move = gChosenMoveByBattler[foe];
    if (move == MOVE_NONE || move >= MOVES_COUNT)
        return NULL;
    p = StringCopy(gDisplayedStringBattle, sText_PatternRuns);
    p = StringCopy(p, gMoveNames[move]);
    StringCopy(p, sText_Stop);
    return gDisplayedStringBattle;
}

static const u8 *LensText(void)
{
    u8 foe = Foe(), t, best = TYPE_NONE, bestQ = 4, q;
    u8 *p;

    p = StringCopy(gDisplayedStringBattle, sText_LensHp);
    p = ConvertIntToDecimalStringN(p, gBattleMons[foe].hp, STR_CONV_MODE_LEFT_ALIGN, 3);
    p = StringCopy(p, sText_LensSlash);
    p = ConvertIntToDecimalStringN(p, gBattleMons[foe].maxHP, STR_CONV_MODE_LEFT_ALIGN, 3);
    p = StringCopy(p, sText_LensBreak);
    for (t = 0; t < NUMBER_OF_MON_TYPES; t++)
    {
        if (t == TYPE_MYSTERY)
            continue;
        q = QuartersOn(t, foe);
        if (q > bestQ)
        {
            bestQ = q;
            best = t;
        }
    }
    if (best == TYPE_NONE)
    {
        StringCopy(p, sText_LensNoWeak);
    }
    else
    {
        p = StringCopy(p, gTypeNames[best]);
        p = StringCopy(p, sText_LensHits);
        StringCopy(p, TimesText(bestQ));
    }
    return gDisplayedStringBattle;
}

const u8 *Spellbook_PromptText(void)
{
    const u8 *text = NULL;

    if (Spellbook_PatternOn())
        text = PatternText();
    else if (Spellbook_LensOn())
        text = LensText();
    if (text != NULL)
        LoadPalette(sEmerald, BG_PLTT_ID(0) + PROMPT_SLOT, PLTT_SIZEOF(1));   // every time: a menu's return reloads 0
    return text;
}

// THE LENS in the routine menu: the multiplier on the foe where "PP" stood, the chance it lands where the PP count was.
bool8 Spellbook_LensMoveText(u16 move, u8 *effect, u8 *hit)
{
    u16 chance;
    u8 *p;

    if (!Spellbook_LensOn() || move == MOVE_NONE)
        return FALSE;
    if (gBattleMoves[move].power == 0)
        StringCopy(effect, sNoTimes);
    else
        StringCopy(effect, TimesText(QuartersOn(gBattleMoves[move].type, Foe())));
    chance = BattleScript_HitChance(GetBattlerAtPosition(B_POSITION_PLAYER_LEFT), Foe(), move);
    if (chance == BATTLE_HIT_SURE)
    {
        StringCopy(hit, sHitSure);
    }
    else
    {
        p = ConvertIntToDecimalStringN(hit, chance, STR_CONV_MODE_LEFT_ALIGN, 3);
        StringCopy(p, sPercent);
    }
    return TRUE;
}

//  ---------------------------------------------------------------- DEBUG

#if DAEMONS_DEBUG
void Spellbook_DebugAll(bool8 on)
{
    VarSet(VAR_SPELLS_KNOWN, on ? (1 << SPELL_COUNT) - 1 : 0);
    if (!on)
    {
        VarSet(VAR_SPELLS_CAST, 0);
        VarSet(VAR_SPELL_STATE, 0);
        VarSet(VAR_SPELL_STATE_2, 0);
    }
}
STATIC_ASSERT((1 << SPELL_COUNT) - 1 == 0x3FFF, EverySpellIsTheDebugRowsTest);   // daemons_debug_events.c IsHeld
#endif
