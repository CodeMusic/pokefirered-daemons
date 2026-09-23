#include "global.h"
#include "gflib.h"
#include "event_data.h"
#include "menu.h"
#include "new_menu_helpers.h"
#include "overworld.h"
#include "quest_log.h"
#include "scanline_effect.h"
#include "script.h"
#include "string_util.h"
#include "task.h"
#include "text_window.h"
#include "school_exam.h"
#include "constants/songs.h"

//  T-217: THE EXAM (docs/school.md 7). Batch 3 -- LANGUAGE's section only, built for all seven.
//
//  WHAT IT IS AGAINST. 4.4: Scorn designed the standardised evaluations and rigged them so that rivals
//  failed, and did not say what they measured. This paper is the negative of every clause of that, and
//  nothing on it ever says so:
//    - the material is published: every question is drawn from a board topic and from nowhere else;
//    - the criterion is published: the pass mark is printed on the syllabus before the paper exists;
//    - leaving loses nothing: EXIT EXAM keeps every answer, in the save, for as long as it takes;
//    - it can be sat again, forever. Handing in clears the answers for the next attempt, and every
//      attempt's mark is kept in the NOTEBOOK.
//
//  THE SAVE holds ANSWERS, never a score, so a corrected question never strands an old save: three bits
//  each (0 unanswered, 1-4 for A-D), five to a var, from VAR_SCHOOL_EXAM_ANSWERS.
//
//  L AND R page the sections. engine.md's trap 19 said FireRed's help system owns them; T-179 took them
//  back -- HELP opens from the START menu only -- so they belong to this screen.

#define EXAM_PASS_MARK  80
#define QS_PER_VAR      5
#define ROWS            6

struct ExamQuestion
{
    const u8 *title;
    const u8 *text;
    const u8 *options[4];
    u8 answer;          // 1-4
};

struct ExamSection
{
    const u8 *name;
    const struct ExamQuestion *questions;
    u8 count;
};

//  ---- 1F LANGUAGE -- DRAFT WORDING, awaiting the user's approval. Every answer is on the board.
static const u8 sLang_T1[] = _("What a name is");
static const u8 sLang_Q1[] = _("The board calls a name a...");
static const u8 sLang_T2[] = _("Renaming");
static const u8 sLang_Q2[] = _("A thing is renamed.\nWhat has changed?");
static const u8 sLang_T3[] = _("Pointing");
static const u8 sLang_Q3[] = _("A name lets you point at a thing\nwithout having to...");
static const u8 sLang_T4[] = _("How many pieces");
static const u8 sLang_Q4[] = _("How many pieces are there\nin UN-READ-ABLE?");
static const u8 sLang_T5[] = _("Why learn the pieces");
static const u8 sLang_Q5[] = _("Learning the pieces lets you...");
static const u8 sLang_T6[] = _("A piece on its own");
static const u8 sLang_Q6[] = _("Taken on its own, each piece...");
static const u8 sLang_T7[] = _("The same words");
static const u8 sLang_Q7[] = _("USER READS DAEMON and\nDAEMON READS USER use...");
static const u8 sLang_T8[] = _("What changed");
static const u8 sLang_Q8[] = _("Between those two sentences,\nwhat changed?");
static const u8 sLang_T9[] = _("One word");
static const u8 sLang_Q9[] = _("One word can carry...");
static const u8 sLang_T10[] = _("Which BOX");
static const u8 sLang_Q10[] = _("What decides which BOX\nyou meant?");
static const u8 sLang_T11[] = _("What spelling is");
static const u8 sLang_Q11[] = _("Spelling is a rule for turning\na sound into...");
static const u8 sLang_T12[] = _("The corner of the board");
static const u8 sLang_Q12[] = _("What has someone written in the\ncorner, in smaller chalk?");

static const u8 sO_Handle[] = _("a handle");
static const u8 sO_Spell[] = _("a spell");
static const u8 sO_Picture[] = _("a picture");
static const u8 sO_Rule[] = _("a rule");
static const u8 sO_TheThing[] = _("the thing itself");
static const u8 sO_OnlyHandle[] = _("only the handle");
static const u8 sO_Both[] = _("the thing and its name");
static const u8 sO_NothingAtAll[] = _("nothing at all");
static const u8 sO_SeeIt[] = _("see it");
static const u8 sO_SpellIt[] = _("spell it");
static const u8 sO_CarryIt[] = _("carry it");
static const u8 sO_TrustIt[] = _("trust it");
static const u8 sO_One[] = _("one");
static const u8 sO_Two[] = _("two");
static const u8 sO_Three[] = _("three");
static const u8 sO_Nine[] = _("nine");
static const u8 sO_SpellFaster[] = _("spell faster");
static const u8 sO_ReadUntaught[] = _("read words nobody taught you");
static const u8 sO_ForgetOld[] = _("forget the old words");
static const u8 sO_WinArguments[] = _("win arguments");
static const u8 sO_MeansNothing[] = _("means nothing");
static const u8 sO_MeansSomething[] = _("means something");
static const u8 sO_IsAName[] = _("is a name");
static const u8 sO_IsASpell[] = _("is a spell");
static const u8 sO_DifferentWords[] = _("different words");
static const u8 sO_SameWords[] = _("the same words");
static const u8 sO_NoWords[] = _("no words");
static const u8 sO_OneWord[] = _("one word, three times");
static const u8 sO_Nothing[] = _("nothing");
static const u8 sO_WordAdded[] = _("a word was added");
static const u8 sO_WordTaken[] = _("a word was taken away");
static const u8 sO_Everything[] = _("everything");
static const u8 sO_OneMeaning[] = _("one meaning, only ever");
static const u8 sO_Several[] = _("several meanings");
static const u8 sO_NoMeaning[] = _("no meaning at all");
static const u8 sO_ASpell[] = _("a spell");
static const u8 sO_ItsSpelling[] = _("its spelling");
static const u8 sO_WordsAround[] = _("the words around it");
static const u8 sO_ItsLength[] = _("how long it is");
static const u8 sO_WhoSaid[] = _("who said it");
static const u8 sO_Marks[] = _("marks");
static const u8 sO_Magic[] = _("magic");
static const u8 sO_Meaning[] = _("meaning");
static const u8 sO_Music[] = _("music");
static const u8 sO_Nobody[] = _("NOBODY");
static const u8 sO_Spelling[] = _("SPELLING");
static const u8 sO_UnderASpell[] = _("UNDER A SPELL");
static const u8 sO_PassMark[] = _("PASS MARK");

static const struct ExamQuestion sQuestions_Language[] =
{
    { sLang_T1,  sLang_Q1,  { sO_Handle, sO_Spell, sO_Picture, sO_Rule }, 1 },
    { sLang_T2,  sLang_Q2,  { sO_TheThing, sO_OnlyHandle, sO_Both, sO_NothingAtAll }, 2 },
    { sLang_T3,  sLang_Q3,  { sO_SeeIt, sO_SpellIt, sO_CarryIt, sO_TrustIt }, 3 },
    { sLang_T4,  sLang_Q4,  { sO_One, sO_Two, sO_Three, sO_Nine }, 3 },
    { sLang_T5,  sLang_Q5,  { sO_SpellFaster, sO_ReadUntaught, sO_ForgetOld, sO_WinArguments }, 2 },
    { sLang_T6,  sLang_Q6,  { sO_MeansNothing, sO_MeansSomething, sO_IsAName, sO_IsASpell }, 2 },
    { sLang_T7,  sLang_Q7,  { sO_DifferentWords, sO_SameWords, sO_NoWords, sO_OneWord }, 2 },
    { sLang_T8,  sLang_Q8,  { sO_Nothing, sO_WordAdded, sO_WordTaken, sO_Everything }, 4 },
    { sLang_T9,  sLang_Q9,  { sO_OneMeaning, sO_Several, sO_NoMeaning, sO_ASpell }, 2 },
    { sLang_T10, sLang_Q10, { sO_ItsSpelling, sO_WordsAround, sO_ItsLength, sO_WhoSaid }, 2 },
    { sLang_T11, sLang_Q11, { sO_Marks, sO_Magic, sO_Meaning, sO_Music }, 1 },
    { sLang_T12, sLang_Q12, { sO_Nobody, sO_Spelling, sO_UnderASpell, sO_PassMark }, 3 },
};

static const u8 sSection_Language[] = _("LANGUAGE");

//  One row per floor. A section added here is a section on the paper; nothing else changes.
static const struct ExamSection sSections[] =
{
    { sSection_Language, sQuestions_Language, ARRAY_COUNT(sQuestions_Language) },
};

//  ---- the screen's words
static const u8 sText_Finish[]     = _("FINISH EXAM");
static const u8 sText_Exit[]       = _("EXIT EXAM");
static const u8 sText_Of[]         = _(" OF ");
static const u8 sText_Answered[]   = _(" ANSWERED");
static const u8 sText_Section[]    = _("SECTION ");
static const u8 sText_PassMark[]   = _("PASS MARK 80");
static const u8 sText_LR[]         = _("L  R");
static const u8 sText_Dash[]       = _("-");
static const u8 sText_Cursor[]     = _("▶");
static const u8 sText_Dot[]        = _(".");
static const u8 sText_Question[]   = _("QUESTION ");
static const u8 sText_ChooseBack[] = _("A  CHOOSE          B  BACK");
static const u8 sText_Left1[]      = _(" still unanswered.");
static const u8 sText_Left2[]      = _("EXIT EXAM keeps every answer you gave.");
static const u8 sText_Confirm1[]   = _("Hand the paper in?");
static const u8 sText_Confirm2[]   = _("A  YES          B  NO");
static const u8 sText_LetterA[] = _("A");
static const u8 sText_LetterB[] = _("B");
static const u8 sText_LetterC[] = _("C");
static const u8 sText_LetterD[] = _("D");
static const u8 *const sLetters[4] = { sText_LetterA, sText_LetterB, sText_LetterC, sText_LetterD };

//  ---- the save
static u16 TotalQuestions(void)
{
    u32 i, n = 0;

    for (i = 0; i < ARRAY_COUNT(sSections); i++)
        n += sSections[i].count;
    return n;
}

static u16 SectionBase(u8 section)
{
    u32 i, n = 0;

    for (i = 0; i < section; i++)
        n += sSections[i].count;
    return n;
}

static u8 GetAnswer(u16 q)
{
    return (VarGet(VAR_SCHOOL_EXAM_ANSWERS + q / QS_PER_VAR) >> ((q % QS_PER_VAR) * 3)) & 7;
}

static void SetAnswer(u16 q, u8 answer)
{
    u16 var = VAR_SCHOOL_EXAM_ANSWERS + q / QS_PER_VAR;
    u16 shift = (q % QS_PER_VAR) * 3;
    u16 value = VarGet(var);

    value &= ~(7 << shift);
    value |= (answer & 7) << shift;
    VarSet(var, value);
}

static u16 CountAnswered(void)
{
    u16 q, n = 0, total = TotalQuestions();

    for (q = 0; q < total; q++)
    {
        if (GetAnswer(q) != 0)
            n++;
    }
    return n;
}

static const u8 *GradeLetter(u16 percent)
{
    static const u8 sAPlus[] = _("A+"), sA[] = _("A"), sAMinus[] = _("A-"), sBPlus[] = _("B+"), sB[] = _("B"),
                    sBMinus[] = _("B-"), sCPlus[] = _("C+"), sC[] = _("C"), sCMinus[] = _("C-"), sD[] = _("D"),
                    sF[] = _("F");

    if (percent >= 97) return sAPlus;
    if (percent >= 93) return sA;
    if (percent >= 90) return sAMinus;
    if (percent >= 87) return sBPlus;
    if (percent >= 83) return sB;
    if (percent >= 80) return sBMinus;
    if (percent >= 77) return sCPlus;
    if (percent >= 73) return sC;
    if (percent >= 70) return sCMinus;
    if (percent >= 60) return sD;
    return sF;
}

//  ---- the screen
enum { MODE_LIST, MODE_QUESTION, MODE_UNANSWERED, MODE_CONFIRM, MODE_LEAVING };
enum { WIN_HEADER, WIN_BODY, WIN_FOOTER, WIN_COUNT };

struct SchoolExam
{
    u8 initState;
    u8 mode;
    u8 section;
    u8 cursor;      // 0..count-1 a question; count is the FINISH / EXIT row
    u8 scroll;
    u8 footer;      // 0 FINISH, 1 EXIT
    u8 option;
    u8 handedIn;
};

static EWRAM_DATA struct SchoolExam *sExam = NULL;

static const struct BgTemplate sBgTemplates[] =
{
    { .bg = 0, .charBaseIndex = 0, .mapBaseIndex = 31, .screenSize = 0, .paletteMode = 0, .priority = 0, .baseTile = 0 },
};

static const struct WindowTemplate sWindowTemplates[WIN_COUNT + 1] =
{
    [WIN_HEADER] = { .bg = 0, .tilemapLeft = 1, .tilemapTop = 0,  .width = 28, .height = 2,  .paletteNum = 15, .baseBlock = 1 },
    [WIN_BODY]   = { .bg = 0, .tilemapLeft = 1, .tilemapTop = 3,  .width = 28, .height = 12, .paletteNum = 15, .baseBlock = 57 },
    [WIN_FOOTER] = { .bg = 0, .tilemapLeft = 1, .tilemapTop = 16, .width = 28, .height = 4,  .paletteNum = 15, .baseBlock = 393 },
    [WIN_COUNT]  = DUMMY_WIN_TEMPLATE
};

//  Paper and ink, and nothing else. The one screen in the game that tests you is the plainest one in it.
static const u16 sPaperPal[16] =
{
    RGB(0, 0, 0), RGB(29, 28, 25), RGB(3, 3, 4), RGB(23, 22, 19), RGB(14, 14, 13),
};
static const u8 sInk[3]   = { 1, 2, 3 };
static const u8 sFaint[3] = { 1, 4, 1 };

static void Task_ExamInit(u8 taskId);
static void Task_ExamInput(u8 taskId);

static void Print(u8 win, const u8 *colors, u8 x, u8 y, const u8 *str)
{
    AddTextPrinterParameterized3(win, FONT_NORMAL, x, y, colors, TEXT_SKIP_DRAW, str);
}

static const struct ExamSection *Section(void)
{
    return &sSections[sExam->section];
}

static void DrawHeader(void)
{
    u8 buf[32], *p;

    FillWindowPixelBuffer(WIN_HEADER, PIXEL_FILL(1));
    Print(WIN_HEADER, sInk, 0, 1, Section()->name);
    if (ARRAY_COUNT(sSections) > 1)
    {
        p = StringCopy(buf, sText_Section);
        p = ConvertIntToDecimalStringN(p, sExam->section + 1, STR_CONV_MODE_LEFT_ALIGN, 1);
        p = StringCopy(p, sText_Of);
        ConvertIntToDecimalStringN(p, ARRAY_COUNT(sSections), STR_CONV_MODE_LEFT_ALIGN, 1);
        Print(WIN_HEADER, sInk, 96, 1, buf);
        Print(WIN_HEADER, sFaint, 196, 1, sText_LR);
    }
    else
    {
        Print(WIN_HEADER, sFaint, 224 - GetStringWidth(FONT_NORMAL, sText_PassMark, 0), 1, sText_PassMark);
    }
    CopyWindowToVram(WIN_HEADER, COPYWIN_FULL);
}

static void DrawList(void)
{
    u8 buf[8];
    u16 base = SectionBase(sExam->section);
    u8 row, i, answer;

    FillWindowPixelBuffer(WIN_BODY, PIXEL_FILL(1));
    for (row = 0; row < ROWS; row++)
    {
        i = sExam->scroll + row;
        if (i >= Section()->count)
            break;
        if (sExam->cursor == i)
            Print(WIN_BODY, sInk, 0, row * 16, sText_Cursor);
        ConvertIntToDecimalStringN(buf, i + 1, STR_CONV_MODE_RIGHT_ALIGN, 2);
        StringAppend(buf, sText_Dot);
        Print(WIN_BODY, sInk, 10, row * 16, buf);
        Print(WIN_BODY, sInk, 34, row * 16, Section()->questions[i].title);
        answer = GetAnswer(base + i);
        if (answer != 0)
            Print(WIN_BODY, sInk, 212, row * 16, sLetters[answer - 1]);
        else
            Print(WIN_BODY, sFaint, 212, row * 16, sText_Dash);
    }
    CopyWindowToVram(WIN_BODY, COPYWIN_FULL);
}

static void DrawQuestion(void)
{
    const struct ExamQuestion *q = &Section()->questions[sExam->cursor];
    u8 buf[48], i;

    FillWindowPixelBuffer(WIN_BODY, PIXEL_FILL(1));
    Print(WIN_BODY, sInk, 0, 0, q->text);
    for (i = 0; i < 4; i++)
    {
        if (sExam->option == i)
            Print(WIN_BODY, sInk, 0, 36 + i * 14, sText_Cursor);
        StringCopy(buf, sLetters[i]);
        Print(WIN_BODY, sInk, 10, 36 + i * 14, buf);
        Print(WIN_BODY, sInk, 24, 36 + i * 14, q->options[i]);
    }
    CopyWindowToVram(WIN_BODY, COPYWIN_FULL);
}

static void DrawFooter(void)
{
    u8 buf[48], *p;
    bool8 onFooter = (sExam->cursor == Section()->count);

    FillWindowPixelBuffer(WIN_FOOTER, PIXEL_FILL(1));
    switch (sExam->mode)
    {
    case MODE_QUESTION:
        Print(WIN_FOOTER, sFaint, 0, 0, sText_ChooseBack);
        p = StringCopy(buf, sText_Question);
        p = ConvertIntToDecimalStringN(p, sExam->cursor + 1, STR_CONV_MODE_LEFT_ALIGN, 2);
        p = StringCopy(p, sText_Of);
        ConvertIntToDecimalStringN(p, Section()->count, STR_CONV_MODE_LEFT_ALIGN, 2);
        Print(WIN_FOOTER, sFaint, 0, 16, buf);
        break;
    case MODE_UNANSWERED:
        p = ConvertIntToDecimalStringN(buf, TotalQuestions() - CountAnswered(), STR_CONV_MODE_LEFT_ALIGN, 3);
        StringCopy(p, sText_Left1);
        Print(WIN_FOOTER, sInk, 0, 0, buf);
        Print(WIN_FOOTER, sInk, 0, 16, sText_Left2);
        break;
    case MODE_CONFIRM:
        Print(WIN_FOOTER, sInk, 0, 0, sText_Confirm1);
        Print(WIN_FOOTER, sInk, 0, 16, sText_Confirm2);
        break;
    default:
        if (onFooter && sExam->footer == 0)
            Print(WIN_FOOTER, sInk, 0, 0, sText_Cursor);
        Print(WIN_FOOTER, sInk, 10, 0, sText_Finish);
        if (onFooter && sExam->footer == 1)
            Print(WIN_FOOTER, sInk, 120, 0, sText_Cursor);
        Print(WIN_FOOTER, sInk, 130, 0, sText_Exit);
        p = ConvertIntToDecimalStringN(buf, CountAnswered(), STR_CONV_MODE_LEFT_ALIGN, 3);
        p = StringCopy(p, sText_Of);
        p = ConvertIntToDecimalStringN(p, TotalQuestions(), STR_CONV_MODE_LEFT_ALIGN, 3);
        StringCopy(p, sText_Answered);
        Print(WIN_FOOTER, sFaint, 10, 16, buf);
        break;
    }
    CopyWindowToVram(WIN_FOOTER, COPYWIN_FULL);
}

static void DrawAll(void)
{
    DrawHeader();
    if (sExam->mode == MODE_QUESTION)
        DrawQuestion();
    else
        DrawList();
    DrawFooter();
}

static void HandIn(void)
{
    u16 q, correct = 0, total = TotalQuestions(), percent;
    u32 s, i;

    q = 0;
    for (s = 0; s < ARRAY_COUNT(sSections); s++)
    {
        for (i = 0; i < sSections[s].count; i++, q++)
        {
            if (GetAnswer(q) == sSections[s].questions[i].answer)
                correct++;
        }
    }
    percent = (correct * 100 + total / 2) / total;
    VarSet(VAR_SCHOOL_EXAM_LAST, percent);
    if (VarGet(VAR_SCHOOL_EXAM_ATTEMPTS) == 0 || percent > VarGet(VAR_SCHOOL_EXAM_BEST))
        VarSet(VAR_SCHOOL_EXAM_BEST, percent);
    VarSet(VAR_SCHOOL_EXAM_ATTEMPTS, VarGet(VAR_SCHOOL_EXAM_ATTEMPTS) + 1);
    //  A fresh paper for the next sitting. The mark is kept; the answers are not, or a retake would be
    //  handing the same paper in twice.
    for (q = 0; q < (total + QS_PER_VAR - 1) / QS_PER_VAR; q++)
        VarSet(VAR_SCHOOL_EXAM_ANSWERS + q, 0);
    sExam->handedIn = TRUE;
}

static void Leave(void)
{
    sExam->mode = MODE_LEAVING;
    BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 16, RGB_BLACK);
}

static void Task_ExamInput(u8 taskId)
{
    u8 count = Section()->count;

    switch (sExam->mode)
    {
    case MODE_LIST:
        if (JOY_NEW(DPAD_UP) && sExam->cursor > 0)
        {
            sExam->cursor--;
            if (sExam->cursor < sExam->scroll)
                sExam->scroll = sExam->cursor;
            PlaySE(SE_SELECT);
            DrawAll();
        }
        else if (JOY_NEW(DPAD_DOWN) && sExam->cursor < count)
        {
            sExam->cursor++;
            if (sExam->cursor < count && sExam->cursor >= sExam->scroll + ROWS)
                sExam->scroll = sExam->cursor - ROWS + 1;
            PlaySE(SE_SELECT);
            DrawAll();
        }
        else if (JOY_NEW(DPAD_LEFT | DPAD_RIGHT) && sExam->cursor == count)
        {
            sExam->footer ^= 1;
            PlaySE(SE_SELECT);
            DrawFooter();
        }
        else if (JOY_NEW(L_BUTTON | R_BUTTON) && ARRAY_COUNT(sSections) > 1)
        {
            if (JOY_NEW(R_BUTTON))
                sExam->section = (sExam->section + 1) % ARRAY_COUNT(sSections);
            else
                sExam->section = (sExam->section + ARRAY_COUNT(sSections) - 1) % ARRAY_COUNT(sSections);
            sExam->cursor = sExam->scroll = 0;
            PlaySE(SE_SELECT);
            DrawAll();
        }
        else if (JOY_NEW(A_BUTTON))
        {
            PlaySE(SE_SELECT);
            if (sExam->cursor < count)
            {
                u8 answer = GetAnswer(SectionBase(sExam->section) + sExam->cursor);
                sExam->option = answer ? answer - 1 : 0;
                sExam->mode = MODE_QUESTION;
            }
            else if (sExam->footer == 1)
            {
                Leave();
                return;
            }
            else if (CountAnswered() < TotalQuestions())
            {
                sExam->mode = MODE_UNANSWERED;
            }
            else
            {
                sExam->mode = MODE_CONFIRM;
            }
            DrawAll();
        }
        else if (JOY_NEW(B_BUTTON))
        {
            //  B never leaves by itself: it walks the cursor to EXIT EXAM, so leaving is always a choice.
            sExam->cursor = count;
            sExam->footer = 1;
            PlaySE(SE_SELECT);
            DrawAll();
        }
        break;
    case MODE_QUESTION:
        if (JOY_NEW(DPAD_UP) && sExam->option > 0)
        {
            sExam->option--;
            PlaySE(SE_SELECT);
            DrawQuestion();
        }
        else if (JOY_NEW(DPAD_DOWN) && sExam->option < 3)
        {
            sExam->option++;
            PlaySE(SE_SELECT);
            DrawQuestion();
        }
        else if (JOY_NEW(A_BUTTON))
        {
            SetAnswer(SectionBase(sExam->section) + sExam->cursor, sExam->option + 1);
            PlaySE(SE_SELECT);
            sExam->mode = MODE_LIST;
            if (sExam->cursor + 1 < count)
            {
                sExam->cursor++;
                if (sExam->cursor >= sExam->scroll + ROWS)
                    sExam->scroll = sExam->cursor - ROWS + 1;
            }
            DrawAll();
        }
        else if (JOY_NEW(B_BUTTON))
        {
            PlaySE(SE_SELECT);
            sExam->mode = MODE_LIST;
            DrawAll();
        }
        break;
    case MODE_UNANSWERED:
        if (JOY_NEW(A_BUTTON | B_BUTTON))
        {
            PlaySE(SE_SELECT);
            sExam->mode = MODE_LIST;
            DrawAll();
        }
        break;
    case MODE_CONFIRM:
        if (JOY_NEW(A_BUTTON))
        {
            PlaySE(SE_SELECT);
            HandIn();
            Leave();
        }
        else if (JOY_NEW(B_BUTTON))
        {
            PlaySE(SE_SELECT);
            sExam->mode = MODE_LIST;
            DrawAll();
        }
        break;
    case MODE_LEAVING:
        if (gPaletteFade.active)
            break;
        gSpecialVar_Result = sExam->handedIn;
        DestroyTask(taskId);
        FreeAllWindowBuffers();
        FREE_AND_SET_NULL(sExam);
        SetMainCallback2(CB2_ReturnToFieldContinueScriptPlayMapMusic);
        break;
    }
}

static void VBlankCB_Exam(void)
{
    LoadOam();
    ProcessSpriteCopyRequests();
    TransferPlttBuffer();
}

static void CB2_Exam(void)
{
    RunTasks();
    AnimateSprites();
    BuildOamBuffer();
    UpdatePaletteFade();
}

static void Task_ExamInit(u8 taskId)
{
    switch (sExam->initState)
    {
    case 0:
        SetVBlankCallback(NULL);
        break;
    case 1:
        DmaClearLarge16(3, (void *)VRAM, VRAM_SIZE, 0x1000);
        DmaClear32(3, (void *)OAM, OAM_SIZE);
        DmaClear16(3, (void *)PLTT, PLTT_SIZE);
        SetGpuReg(REG_OFFSET_DISPCNT, 0);
        ResetBgsAndClearDma3BusyFlags(0);
        InitBgsFromTemplates(0, sBgTemplates, ARRAY_COUNT(sBgTemplates));
        ChangeBgX(0, 0, BG_COORD_SET);
        ChangeBgY(0, 0, BG_COORD_SET);
        InitWindows(sWindowTemplates);
        DeactivateAllTextPrinters();
        SetGpuReg(REG_OFFSET_DISPCNT, DISPCNT_OBJ_1D_MAP | DISPCNT_OBJ_ON);
        ShowBg(0);
        break;
    case 2:
        LoadPalette(sPaperPal, BG_PLTT_ID(15), PLTT_SIZE_4BPP);
        LoadPalette(&sPaperPal[1], BG_PLTT_ID(0), PLTT_SIZEOF(1));   // the backdrop is the paper
        break;
    case 3:
        DrawAll();
        PutWindowTilemap(WIN_HEADER);
        PutWindowTilemap(WIN_BODY);
        PutWindowTilemap(WIN_FOOTER);
        CopyBgTilemapBufferToVram(0);
        break;
    case 4:
        BeginNormalPaletteFade(PALETTES_ALL, 0, 16, 0, RGB_BLACK);
        break;
    case 5:
        SetVBlankCallback(VBlankCB_Exam);
        break;
    default:
        if (gPaletteFade.active)
            return;
        gTasks[taskId].func = Task_ExamInput;
        return;
    }
    sExam->initState++;
}

static void CB2_OpenExam(void)
{
    sExam = AllocZeroed(sizeof(*sExam));
    ResetSpriteData();
    ResetPaletteFade();
    FreeAllSpritePalettes();
    ResetTasks();
    ScanlineEffect_Stop();
    CreateTask(Task_ExamInit, 0);
    SetMainCallback2(CB2_Exam);
}

//  ---------------------------------------------------------------- the specials

//  Opens the paper. The script fades to black first and waits; VAR_RESULT comes back TRUE if it was handed in.
void School_OpenExam(void)
{
    QuestLog_CutRecording();
    FlagSet(FLAG_SCHOOL_EXAM_OPENED);
    SetMainCallback2(CB2_OpenExam);
    LockPlayerFieldControls();
}

//  VAR_RESULT: TRUE if a paper is under way -- any answer given and not yet handed in.
void School_ExamInProgress(void)
{
    gSpecialVar_Result = (CountAnswered() != 0);
}

//  STR_VAR_1: the last mark. STR_VAR_2: its letter. VAR_RESULT: TRUE if it passes.
void School_BufferExamResult(void)
{
    u16 percent = VarGet(VAR_SCHOOL_EXAM_LAST);

    ConvertIntToDecimalStringN(gStringVar1, percent, STR_CONV_MODE_LEFT_ALIGN, 3);
    StringCopy(gStringVar2, GradeLetter(percent));
    gSpecialVar_Result = (percent >= EXAM_PASS_MARK);
}

//  The NOTEBOOK's page for the paper: every sitting's mark, and one under way if there is one.
void School_WriteExamRecord(u8 *dest)
{
    static const u8 sHead[]       = _("THE PAPER\n");
    static const u8 sNone[]       = _("Not handed in yet.");
    static const u8 sSat[]        = _("Handed in: ");
    static const u8 sTimes[]      = _(" times.");
    static const u8 sOnce[]       = _(" time.");
    static const u8 sBest[]       = _("Best: ");
    static const u8 sLast[]       = _(".  Last: ");
    static const u8 sComma[]      = _(", ");
    static const u8 sFullStop[]   = _(".");
    static const u8 sUnderWay[]   = _("Under way: ");
    static const u8 sOf[]         = _(" of ");
    static const u8 sAnswered[]   = _(" answered.");
    u16 attempts = VarGet(VAR_SCHOOL_EXAM_ATTEMPTS);
    u16 best = VarGet(VAR_SCHOOL_EXAM_BEST), last = VarGet(VAR_SCHOOL_EXAM_LAST);
    u16 answered = CountAnswered();
    u8 *p = StringCopy(dest, sHead);

    if (attempts == 0)
    {
        p = StringCopy(p, sNone);
    }
    else
    {
        p = StringCopy(p, sSat);
        p = ConvertIntToDecimalStringN(p, attempts, STR_CONV_MODE_LEFT_ALIGN, 3);
        p = StringCopy(p, attempts == 1 ? sOnce : sTimes);
        *p++ = CHAR_PROMPT_CLEAR;
        p = StringCopy(p, sBest);
        p = ConvertIntToDecimalStringN(p, best, STR_CONV_MODE_LEFT_ALIGN, 3);
        p = StringCopy(p, sComma);
        p = StringCopy(p, GradeLetter(best));
        p = StringCopy(p, sLast);
        p = ConvertIntToDecimalStringN(p, last, STR_CONV_MODE_LEFT_ALIGN, 3);
        p = StringCopy(p, sComma);
        p = StringCopy(p, GradeLetter(last));
        p = StringCopy(p, sFullStop);
    }
    if (answered != 0)
    {
        *p++ = CHAR_PROMPT_CLEAR;
        p = StringCopy(p, sUnderWay);
        p = ConvertIntToDecimalStringN(p, answered, STR_CONV_MODE_LEFT_ALIGN, 3);
        p = StringCopy(p, sOf);
        p = ConvertIntToDecimalStringN(p, TotalQuestions(), STR_CONV_MODE_LEFT_ALIGN, 3);
        StringCopy(p, sAnswered);
    }
}
