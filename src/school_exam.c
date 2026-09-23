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

//  T-217: THE EXAM (docs/school.md 7). Seven sections, one per floor, 84 questions (batch 5, T-219).
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

//  T-219: 2F-7F's papers, generated by batch 5's question table (docs/school.md 7). DRAFT WORDING, awaiting
//  the user's approval. Every answer is on that floor's board; the options are shuffled so the right one moves.

static const u8 sAttention_T1[] = _("How much there is");
static const u8 sAttention_Q1[] = _("The board says attention is...");
static const u8 sAttention_O1_0[] = _("free");
static const u8 sAttention_O1_1[] = _("the same as memory");
static const u8 sAttention_O1_2[] = _("only so much");
static const u8 sAttention_O1_3[] = _("endless");
static const u8 sAttention_T2[] = _("More on one thing");
static const u8 sAttention_Q2[] = _("More attention on one thing\nmeans...");
static const u8 sAttention_O2_0[] = _("a headache");
static const u8 sAttention_O2_1[] = _("less on everything else");
static const u8 sAttention_O2_2[] = _("more everywhere");
static const u8 sAttention_O2_3[] = _("nothing changes");
static const u8 sAttention_T3[] = _("Never added");
static const u8 sAttention_Q3[] = _("Attention is never added.\nIt is only...");
static const u8 sAttention_O3_0[] = _("lost");
static const u8 sAttention_O3_1[] = _("spent");
static const u8 sAttention_O3_2[] = _("borrowed");
static const u8 sAttention_O3_3[] = _("moved");
static const u8 sAttention_T4[] = _("What a cue does");
static const u8 sAttention_Q4[] = _("A cue says where to look...");
static const u8 sAttention_O4_0[] = _("before the thing arrives");
static const u8 sAttention_O4_1[] = _("after it has gone");
static const u8 sAttention_O4_2[] = _("while you blink");
static const u8 sAttention_O4_3[] = _("never");
static const u8 sAttention_T5[] = _("A good cue");
static const u8 sAttention_Q5[] = _("A good cue makes you...");
static const u8 sAttention_O5_0[] = _("slower");
static const u8 sAttention_O5_1[] = _("curious");
static const u8 sAttention_O5_2[] = _("faster");
static const u8 sAttention_O5_3[] = _("certain");
static const u8 sAttention_T6[] = _("A wrong cue");
static const u8 sAttention_Q6[] = _("A wrong cue makes you...");
static const u8 sAttention_O6_0[] = _("faster");
static const u8 sAttention_O6_1[] = _("slower than no cue at all");
static const u8 sAttention_O6_2[] = _("no different");
static const u8 sAttention_O6_3[] = _("certain");
static const u8 sAttention_T7[] = _("The odd one out");
static const u8 sAttention_Q7[] = _("In a crowd, what finds you\nquickly?");
static const u8 sAttention_O7_0[] = _("the nearest thing");
static const u8 sAttention_O7_1[] = _("the biggest thing");
static const u8 sAttention_O7_2[] = _("nothing");
static const u8 sAttention_O7_3[] = _("the only thing that is different");
static const u8 sAttention_T8[] = _("A bigger crowd");
static const u8 sAttention_Q8[] = _("More things to search through\nusually means...");
static const u8 sAttention_O8_0[] = _("a longer search");
static const u8 sAttention_O8_1[] = _("no search");
static const u8 sAttention_O8_2[] = _("a louder search");
static const u8 sAttention_O8_3[] = _("a faster search");
static const u8 sAttention_T9[] = _("Switching");
static const u8 sAttention_Q9[] = _("Changing what you attend to\ncosts...");
static const u8 sAttention_O9_0[] = _("money");
static const u8 sAttention_O9_1[] = _("time, only once");
static const u8 sAttention_O9_2[] = _("time, every time");
static const u8 sAttention_O9_3[] = _("nothing");
static const u8 sAttention_T10[] = _("Two things at once");
static const u8 sAttention_Q10[] = _("Two things at once is nearly\nalways...");
static const u8 sAttention_O10_0[] = _("easier");
static const u8 sAttention_O10_1[] = _("one, then the other, fast");
static const u8 sAttention_O10_2[] = _("two at once");
static const u8 sAttention_O10_3[] = _("impossible");
static const u8 sAttention_T11[] = _("Watching closely");
static const u8 sAttention_Q11[] = _("Watch one thing closely enough\nand...");
static const u8 sAttention_O11_0[] = _("it moves");
static const u8 sAttention_O11_1[] = _("you fall asleep");
static const u8 sAttention_O11_2[] = _("you see everything");
static const u8 sAttention_O11_3[] = _("something walks right past");
static const u8 sAttention_T12[] = _("Missing it");
static const u8 sAttention_Q12[] = _("People who missed it believe...");
static const u8 sAttention_O12_0[] = _("that they did not");
static const u8 sAttention_O12_1[] = _("that they did");
static const u8 sAttention_O12_2[] = _("nothing");
static const u8 sAttention_O12_3[] = _("the board");

static const struct ExamQuestion sQuestions_Attention[] =
{
    { sAttention_T1, sAttention_Q1, { sAttention_O1_0, sAttention_O1_1, sAttention_O1_2, sAttention_O1_3 }, 3 },
    { sAttention_T2, sAttention_Q2, { sAttention_O2_0, sAttention_O2_1, sAttention_O2_2, sAttention_O2_3 }, 2 },
    { sAttention_T3, sAttention_Q3, { sAttention_O3_0, sAttention_O3_1, sAttention_O3_2, sAttention_O3_3 }, 4 },
    { sAttention_T4, sAttention_Q4, { sAttention_O4_0, sAttention_O4_1, sAttention_O4_2, sAttention_O4_3 }, 1 },
    { sAttention_T5, sAttention_Q5, { sAttention_O5_0, sAttention_O5_1, sAttention_O5_2, sAttention_O5_3 }, 3 },
    { sAttention_T6, sAttention_Q6, { sAttention_O6_0, sAttention_O6_1, sAttention_O6_2, sAttention_O6_3 }, 2 },
    { sAttention_T7, sAttention_Q7, { sAttention_O7_0, sAttention_O7_1, sAttention_O7_2, sAttention_O7_3 }, 4 },
    { sAttention_T8, sAttention_Q8, { sAttention_O8_0, sAttention_O8_1, sAttention_O8_2, sAttention_O8_3 }, 1 },
    { sAttention_T9, sAttention_Q9, { sAttention_O9_0, sAttention_O9_1, sAttention_O9_2, sAttention_O9_3 }, 3 },
    { sAttention_T10, sAttention_Q10, { sAttention_O10_0, sAttention_O10_1, sAttention_O10_2, sAttention_O10_3 }, 2 },
    { sAttention_T11, sAttention_Q11, { sAttention_O11_0, sAttention_O11_1, sAttention_O11_2, sAttention_O11_3 }, 4 },
    { sAttention_T12, sAttention_Q12, { sAttention_O12_0, sAttention_O12_1, sAttention_O12_2, sAttention_O12_3 }, 1 },
};

static const u8 sSection_Attention[] = _("ATTENTION");

static const u8 sMemory_T1[] = _("What is kept");
static const u8 sMemory_Q1[] = _("Memory keeps a thing...");
static const u8 sMemory_O1_0[] = _("not whole");
static const u8 sMemory_O1_1[] = _("only by name");
static const u8 sMemory_O1_2[] = _("for a day");
static const u8 sMemory_O1_3[] = _("whole, forever");
static const u8 sMemory_T2[] = _("Enough to...");
static const u8 sMemory_Q2[] = _("What is kept is enough to...");
static const u8 sMemory_O2_0[] = _("copy it exactly");
static const u8 sMemory_O2_1[] = _("prove it");
static const u8 sMemory_O2_2[] = _("build it again");
static const u8 sMemory_O2_3[] = _("forget it");
static const u8 sMemory_T3[] = _("Left alone");
static const u8 sMemory_Q3[] = _("Left alone, a memory...");
static const u8 sMemory_O3_0[] = _("moves");
static const u8 sMemory_O3_1[] = _("fades");
static const u8 sMemory_O3_2[] = _("grows");
static const u8 sMemory_O3_3[] = _("stays exactly as it was");
static const u8 sMemory_T4[] = _("How it fades");
static const u8 sMemory_Q4[] = _("It fades fastest...");
static const u8 sMemory_O4_0[] = _("never");
static const u8 sMemory_O4_1[] = _("on Fridays");
static const u8 sMemory_O4_2[] = _("at the end");
static const u8 sMemory_O4_3[] = _("at first");
static const u8 sMemory_T5[] = _("The last of it");
static const u8 sMemory_Q5[] = _("Fading goes on until...");
static const u8 sMemory_O5_0[] = _("never quite nothing");
static const u8 sMemory_O5_1[] = _("nothing is left");
static const u8 sMemory_O5_2[] = _("tomorrow");
static const u8 sMemory_O5_3[] = _("it is recalled");
static const u8 sMemory_T6[] = _("Old and new");
static const u8 sMemory_Q6[] = _("New things and old things...");
static const u8 sMemory_O6_0[] = _("never meet");
static const u8 sMemory_O6_1[] = _("agree");
static const u8 sMemory_O6_2[] = _("push on each other");
static const u8 sMemory_O6_3[] = _("swap places");
static const u8 sMemory_T7[] = _("Most confused");
static const u8 sMemory_Q7[] = _("The things most easily confused\nare...");
static const u8 sMemory_O7_0[] = _("the oldest");
static const u8 sMemory_O7_1[] = _("the most alike");
static const u8 sMemory_O7_2[] = _("the newest");
static const u8 sMemory_O7_3[] = _("the longest");
static const u8 sMemory_T8[] = _("Recalling");
static const u8 sMemory_Q8[] = _("Remembering is not reading a\ncopy. It is...");
static const u8 sMemory_O8_0[] = _("lending one");
static const u8 sMemory_O8_1[] = _("deleting one");
static const u8 sMemory_O8_2[] = _("naming one");
static const u8 sMemory_O8_3[] = _("building one");
static const u8 sMemory_T9[] = _("Saved over");
static const u8 sMemory_Q9[] = _("The rebuilt memory is saved...");
static const u8 sMemory_O9_0[] = _("over the old one");
static const u8 sMemory_O9_1[] = _("nowhere");
static const u8 sMemory_O9_2[] = _("in a box");
static const u8 sMemory_O9_3[] = _("beside the old one");
static const u8 sMemory_T10[] = _("Often enough");
static const u8 sMemory_Q10[] = _("Remember something often enough\nand you remember...");
static const u8 sMemory_O10_0[] = _("the future");
static const u8 sMemory_O10_1[] = _("a different thing");
static const u8 sMemory_O10_2[] = _("the remembering");
static const u8 sMemory_O10_3[] = _("nothing");
static const u8 sMemory_T11[] = _("Spacing");
static const u8 sMemory_Q11[] = _("Which works better?");
static const u8 sMemory_O11_0[] = _("never revising");
static const u8 sMemory_O11_1[] = _("once a day for five days");
static const u8 sMemory_O11_2[] = _("five times in one day");
static const u8 sMemory_O11_3[] = _("once, very hard");
static const u8 sMemory_T12[] = _("Believing it");
static const u8 sMemory_Q12[] = _("Who believes the spacing lesson?");
static const u8 sMemory_O12_0[] = _("the teacher only");
static const u8 sMemory_O12_1[] = _("nobody, ever");
static const u8 sMemory_O12_2[] = _("everyone, at once");
static const u8 sMemory_O12_3[] = _("those who have tried it");

static const struct ExamQuestion sQuestions_Memory[] =
{
    { sMemory_T1, sMemory_Q1, { sMemory_O1_0, sMemory_O1_1, sMemory_O1_2, sMemory_O1_3 }, 1 },
    { sMemory_T2, sMemory_Q2, { sMemory_O2_0, sMemory_O2_1, sMemory_O2_2, sMemory_O2_3 }, 3 },
    { sMemory_T3, sMemory_Q3, { sMemory_O3_0, sMemory_O3_1, sMemory_O3_2, sMemory_O3_3 }, 2 },
    { sMemory_T4, sMemory_Q4, { sMemory_O4_0, sMemory_O4_1, sMemory_O4_2, sMemory_O4_3 }, 4 },
    { sMemory_T5, sMemory_Q5, { sMemory_O5_0, sMemory_O5_1, sMemory_O5_2, sMemory_O5_3 }, 1 },
    { sMemory_T6, sMemory_Q6, { sMemory_O6_0, sMemory_O6_1, sMemory_O6_2, sMemory_O6_3 }, 3 },
    { sMemory_T7, sMemory_Q7, { sMemory_O7_0, sMemory_O7_1, sMemory_O7_2, sMemory_O7_3 }, 2 },
    { sMemory_T8, sMemory_Q8, { sMemory_O8_0, sMemory_O8_1, sMemory_O8_2, sMemory_O8_3 }, 4 },
    { sMemory_T9, sMemory_Q9, { sMemory_O9_0, sMemory_O9_1, sMemory_O9_2, sMemory_O9_3 }, 1 },
    { sMemory_T10, sMemory_Q10, { sMemory_O10_0, sMemory_O10_1, sMemory_O10_2, sMemory_O10_3 }, 3 },
    { sMemory_T11, sMemory_Q11, { sMemory_O11_0, sMemory_O11_1, sMemory_O11_2, sMemory_O11_3 }, 2 },
    { sMemory_T12, sMemory_Q12, { sMemory_O12_0, sMemory_O12_1, sMemory_O12_2, sMemory_O12_3 }, 4 },
};

static const u8 sSection_Memory[] = _("MEMORY");

static const u8 sCategories_T1[] = _("The middle");
static const u8 sCategories_Q1[] = _("The middle of a kind is...");
static const u8 sCategories_O1_0[] = _("its best example");
static const u8 sCategories_O1_1[] = _("its oldest member");
static const u8 sCategories_O1_2[] = _("its edge");
static const u8 sCategories_O1_3[] = _("its name");
static const u8 sCategories_T2[] = _("Everything else");
static const u8 sCategories_Q2[] = _("Everything else in a kind is...");
static const u8 sCategories_O2_0[] = _("exactly the same");
static const u8 sCategories_O2_1[] = _("the middle");
static const u8 sCategories_O2_2[] = _("more or less like the middle");
static const u8 sCategories_O2_3[] = _("not in it");
static const u8 sCategories_T3[] = _("A clean edge");
static const u8 sCategories_Q3[] = _("How many kinds have a clean\nedge?");
static const u8 sCategories_O3_0[] = _("all of them");
static const u8 sCategories_O3_1[] = _("none");
static const u8 sCategories_O3_2[] = _("most");
static const u8 sCategories_O3_3[] = _("exactly three");
static const u8 sCategories_T4[] = _("Half in");
static const u8 sCategories_Q4[] = _("Somewhere there is always a thing\nthat is...");
static const u8 sCategories_O4_0[] = _("on fire");
static const u8 sCategories_O4_1[] = _("fully out");
static const u8 sCategories_O4_2[] = _("already sorted");
static const u8 sCategories_O4_3[] = _("half in");
static const u8 sCategories_T5[] = _("What sorting uses");
static const u8 sCategories_Q5[] = _("Sorting looks at...");
static const u8 sCategories_O5_0[] = _("a handful of features");
static const u8 sCategories_O5_1[] = _("nothing");
static const u8 sCategories_O5_2[] = _("the name only");
static const u8 sCategories_O5_3[] = _("everything at once");
static const u8 sCategories_T6[] = _("The wrong handful");
static const u8 sCategories_Q6[] = _("Pick the wrong features, and...");
static const u8 sCategories_O6_0[] = _("it sorts faster");
static const u8 sCategories_O6_1[] = _("the drawer breaks");
static const u8 sCategories_O6_2[] = _("different things share a drawer");
static const u8 sCategories_O6_3[] = _("nothing happens");
static const u8 sCategories_T7[] = _("Several at once");
static const u8 sCategories_Q7[] = _("The same thing can sit in...");
static const u8 sCategories_O7_0[] = _("the OTHER drawer only");
static const u8 sCategories_O7_1[] = _("several kinds at once");
static const u8 sCategories_O7_2[] = _("one kind only");
static const u8 sCategories_O7_3[] = _("no kind");
static const u8 sCategories_T8[] = _("Heights");
static const u8 sCategories_Q8[] = _("A thing, a kind of thing, and...");
static const u8 sCategories_O8_0[] = _("a drawer");
static const u8 sCategories_O8_1[] = _("the end");
static const u8 sCategories_O8_2[] = _("a thing of kinds");
static const u8 sCategories_O8_3[] = _("a kind of kind");
static const u8 sCategories_T9[] = _("The last drawer");
static const u8 sCategories_Q9[] = _("Every way of sorting ends up with\na drawer called...");
static const u8 sCategories_O9_0[] = _("OTHER");
static const u8 sCategories_O9_1[] = _("MISC");
static const u8 sCategories_O9_2[] = _("SPARE");
static const u8 sCategories_O9_3[] = _("LATER");
static const u8 sCategories_T10[] = _("Watching it");
static const u8 sCategories_Q10[] = _("The board says to watch how fast\nOTHER...");
static const u8 sCategories_O10_0[] = _("empties");
static const u8 sCategories_O10_1[] = _("closes");
static const u8 sCategories_O10_2[] = _("fills");
static const u8 sCategories_O10_3[] = _("moves");
static const u8 sCategories_T11[] = _("First to mind");
static const u8 sCategories_Q11[] = _("The example that comes to mind\nfirst is...");
static const u8 sCategories_O11_0[] = _("the edge");
static const u8 sCategories_O11_1[] = _("the middle");
static const u8 sCategories_O11_2[] = _("the OTHER drawer");
static const u8 sCategories_O11_3[] = _("the last one");
static const u8 sCategories_T12[] = _("More or less");
static const u8 sCategories_Q12[] = _("Things in a kind belong to it...");
static const u8 sCategories_O12_0[] = _("never");
static const u8 sCategories_O12_1[] = _("exactly");
static const u8 sCategories_O12_2[] = _("alphabetically");
static const u8 sCategories_O12_3[] = _("more or less");

static const struct ExamQuestion sQuestions_Categories[] =
{
    { sCategories_T1, sCategories_Q1, { sCategories_O1_0, sCategories_O1_1, sCategories_O1_2, sCategories_O1_3 }, 1 },
    { sCategories_T2, sCategories_Q2, { sCategories_O2_0, sCategories_O2_1, sCategories_O2_2, sCategories_O2_3 }, 3 },
    { sCategories_T3, sCategories_Q3, { sCategories_O3_0, sCategories_O3_1, sCategories_O3_2, sCategories_O3_3 }, 2 },
    { sCategories_T4, sCategories_Q4, { sCategories_O4_0, sCategories_O4_1, sCategories_O4_2, sCategories_O4_3 }, 4 },
    { sCategories_T5, sCategories_Q5, { sCategories_O5_0, sCategories_O5_1, sCategories_O5_2, sCategories_O5_3 }, 1 },
    { sCategories_T6, sCategories_Q6, { sCategories_O6_0, sCategories_O6_1, sCategories_O6_2, sCategories_O6_3 }, 3 },
    { sCategories_T7, sCategories_Q7, { sCategories_O7_0, sCategories_O7_1, sCategories_O7_2, sCategories_O7_3 }, 2 },
    { sCategories_T8, sCategories_Q8, { sCategories_O8_0, sCategories_O8_1, sCategories_O8_2, sCategories_O8_3 }, 4 },
    { sCategories_T9, sCategories_Q9, { sCategories_O9_0, sCategories_O9_1, sCategories_O9_2, sCategories_O9_3 }, 1 },
    { sCategories_T10, sCategories_Q10, { sCategories_O10_0, sCategories_O10_1, sCategories_O10_2, sCategories_O10_3 }, 3 },
    { sCategories_T11, sCategories_Q11, { sCategories_O11_0, sCategories_O11_1, sCategories_O11_2, sCategories_O11_3 }, 2 },
    { sCategories_T12, sCategories_Q12, { sCategories_O12_0, sCategories_O12_1, sCategories_O12_2, sCategories_O12_3 }, 4 },
};

static const u8 sSection_Categories[] = _("CATEGORIES");

static const u8 sLearning_T1[] = _("What a mind is");
static const u8 sLearning_Q1[] = _("The board says a mind is...");
static const u8 sLearning_O1_0[] = _("one big cell");
static const u8 sLearning_O1_1[] = _("cells joined to cells");
static const u8 sLearning_O1_2[] = _("a box of words");
static const u8 sLearning_O1_3[] = _("a river");
static const u8 sLearning_T2[] = _("A join's strength");
static const u8 sLearning_Q2[] = _("A join's strength says how hard\none cell...");
static const u8 sLearning_O2_0[] = _("sleeps");
static const u8 sLearning_O2_1[] = _("remembers");
static const u8 sLearning_O2_2[] = _("sings");
static const u8 sLearning_O2_3[] = _("pushes the next");
static const u8 sLearning_T3[] = _("Using a join");
static const u8 sLearning_Q3[] = _("Use a join, and it...");
static const u8 sLearning_O3_0[] = _("grows stronger");
static const u8 sLearning_O3_1[] = _("moves");
static const u8 sLearning_O3_2[] = _("disappears");
static const u8 sLearning_O3_3[] = _("wears out");
static const u8 sLearning_T4[] = _("The saying");
static const u8 sLearning_Q4[] = _("Cells that fire together...");
static const u8 sLearning_O4_0[] = _("never meet");
static const u8 sLearning_O4_1[] = _("cancel out");
static const u8 sLearning_O4_2[] = _("wire together");
static const u8 sLearning_O4_3[] = _("tire together");
static const u8 sLearning_T5[] = _("When it fires");
static const u8 sLearning_Q5[] = _("A cell fires when the pushes on\nit...");
static const u8 sLearning_O5_0[] = _("are loud");
static const u8 sLearning_O5_1[] = _("add up past a line");
static const u8 sLearning_O5_2[] = _("stop");
static const u8 sLearning_O5_3[] = _("are equal");
static const u8 sLearning_T6[] = _("Near the line");
static const u8 sLearning_Q6[] = _("Cells close to their line fire\nat...");
static const u8 sLearning_O6_0[] = _("exactly one push");
static const u8 sLearning_O6_1[] = _("only at night");
static const u8 sLearning_O6_2[] = _("nothing, ever");
static const u8 sLearning_O6_3[] = _("almost nothing");
static const u8 sLearning_T7[] = _("Finding out");
static const u8 sLearning_Q7[] = _("Learning needs to find out...");
static const u8 sLearning_O7_0[] = _("how it went");
static const u8 sLearning_O7_1[] = _("who is watching");
static const u8 sLearning_O7_2[] = _("the time");
static const u8 sLearning_O7_3[] = _("nothing");
static const u8 sLearning_T8[] = _("Without feedback");
static const u8 sLearning_Q8[] = _("Practice without feedback makes...");
static const u8 sLearning_O8_0[] = _("a skill, not a habit");
static const u8 sLearning_O8_1[] = _("perfect");
static const u8 sLearning_O8_2[] = _("a habit, not a skill");
static const u8 sLearning_O8_3[] = _("nothing at all");
static const u8 sLearning_T9[] = _("Unused joins");
static const u8 sLearning_Q9[] = _("Joins that are never used are...");
static const u8 sLearning_O9_0[] = _("made stronger");
static const u8 sLearning_O9_1[] = _("taken away");
static const u8 sLearning_O9_2[] = _("doubled");
static const u8 sLearning_O9_3[] = _("kept forever");
static const u8 sLearning_T10[] = _("Growing up");
static const u8 sLearning_Q10[] = _("Growing up means losing most\njoins...");
static const u8 sLearning_O10_0[] = _("never");
static const u8 sLearning_O10_1[] = _("by accident");
static const u8 sLearning_O10_2[] = _("twice");
static const u8 sLearning_O10_3[] = _("on purpose");
static const u8 sLearning_T11[] = _("Pushing");
static const u8 sLearning_Q11[] = _("When one cell fires, it...");
static const u8 sLearning_O11_0[] = _("pushes the next");
static const u8 sLearning_O11_1[] = _("sleeps");
static const u8 sLearning_O11_2[] = _("forgets");
static const u8 sLearning_O11_3[] = _("stops the mind");
static const u8 sLearning_T12[] = _("Stronger");
static const u8 sLearning_Q12[] = _("What makes a join stronger?");
static const u8 sLearning_O12_0[] = _("naming it");
static const u8 sLearning_O12_1[] = _("pruning it");
static const u8 sLearning_O12_2[] = _("using it");
static const u8 sLearning_O12_3[] = _("ignoring it");

static const struct ExamQuestion sQuestions_Learning[] =
{
    { sLearning_T1, sLearning_Q1, { sLearning_O1_0, sLearning_O1_1, sLearning_O1_2, sLearning_O1_3 }, 2 },
    { sLearning_T2, sLearning_Q2, { sLearning_O2_0, sLearning_O2_1, sLearning_O2_2, sLearning_O2_3 }, 4 },
    { sLearning_T3, sLearning_Q3, { sLearning_O3_0, sLearning_O3_1, sLearning_O3_2, sLearning_O3_3 }, 1 },
    { sLearning_T4, sLearning_Q4, { sLearning_O4_0, sLearning_O4_1, sLearning_O4_2, sLearning_O4_3 }, 3 },
    { sLearning_T5, sLearning_Q5, { sLearning_O5_0, sLearning_O5_1, sLearning_O5_2, sLearning_O5_3 }, 2 },
    { sLearning_T6, sLearning_Q6, { sLearning_O6_0, sLearning_O6_1, sLearning_O6_2, sLearning_O6_3 }, 4 },
    { sLearning_T7, sLearning_Q7, { sLearning_O7_0, sLearning_O7_1, sLearning_O7_2, sLearning_O7_3 }, 1 },
    { sLearning_T8, sLearning_Q8, { sLearning_O8_0, sLearning_O8_1, sLearning_O8_2, sLearning_O8_3 }, 3 },
    { sLearning_T9, sLearning_Q9, { sLearning_O9_0, sLearning_O9_1, sLearning_O9_2, sLearning_O9_3 }, 2 },
    { sLearning_T10, sLearning_Q10, { sLearning_O10_0, sLearning_O10_1, sLearning_O10_2, sLearning_O10_3 }, 4 },
    { sLearning_T11, sLearning_Q11, { sLearning_O11_0, sLearning_O11_1, sLearning_O11_2, sLearning_O11_3 }, 1 },
    { sLearning_T12, sLearning_Q12, { sLearning_O12_0, sLearning_O12_1, sLearning_O12_2, sLearning_O12_3 }, 3 },
};

static const u8 sSection_Learning[] = _("LEARNING");

static const u8 sBias_T1[] = _("Ninety or ten");
static const u8 sBias_Q1[] = _("Ninety survive and ten die are...");
static const u8 sBias_O1_0[] = _("both too low");
static const u8 sBias_O1_1[] = _("the same number");
static const u8 sBias_O1_2[] = _("different numbers");
static const u8 sBias_O1_3[] = _("both false");
static const u8 sBias_T2[] = _("What chooses");
static const u8 sBias_Q2[] = _("In FRAMING, what is doing the\nchoosing?");
static const u8 sBias_O2_0[] = _("chance");
static const u8 sBias_O2_1[] = _("the chooser alone");
static const u8 sBias_O2_2[] = _("the number");
static const u8 sBias_O2_3[] = _("the way it is put");
static const u8 sBias_T3[] = _("Put two ways");
static const u8 sBias_Q3[] = _("Put two ways, the same numbers\nmake people...");
static const u8 sBias_O3_0[] = _("choose differently");
static const u8 sBias_O3_1[] = _("choose the same");
static const u8 sBias_O3_2[] = _("refuse");
static const u8 sBias_O3_3[] = _("laugh");
static const u8 sBias_T4[] = _("The first number");
static const u8 sBias_Q4[] = _("The first number heard pulls later\nguesses...");
static const u8 sBias_O4_0[] = _("away from itself");
static const u8 sBias_O4_1[] = _("upward only");
static const u8 sBias_O4_2[] = _("toward itself");
static const u8 sBias_O4_3[] = _("nowhere");
static const u8 sBias_T5[] = _("Picked at random");
static const u8 sBias_Q5[] = _("Does an anchor still pull if it was\npicked at random?");
static const u8 sBias_O5_0[] = _("no");
static const u8 sBias_O5_1[] = _("yes");
static const u8 sBias_O5_2[] = _("only on Fridays");
static const u8 sBias_O5_3[] = _("only if large");
static const u8 sBias_T6[] = _("What agrees");
static const u8 sBias_Q6[] = _("People look for what agrees with\nthem, and...");
static const u8 sBias_O6_0[] = _("ignore it");
static const u8 sBias_O6_1[] = _("never find it");
static const u8 sBias_O6_2[] = _("argue");
static const u8 sBias_O6_3[] = _("find it");
static const u8 sBias_T7[] = _("What disagrees");
static const u8 sBias_Q7[] = _("What disagrees gets checked...");
static const u8 sBias_O7_0[] = _("twice as hard");
static const u8 sBias_O7_1[] = _("once");
static const u8 sBias_O7_2[] = _("by someone else");
static const u8 sBias_O7_3[] = _("not at all");
static const u8 sBias_T8[] = _("Knowing the end");
static const u8 sBias_Q8[] = _("Once you know how it ended, it\nlooks...");
static const u8 sBias_O8_0[] = _("unfinished");
static const u8 sBias_O8_1[] = _("wrong");
static const u8 sBias_O8_2[] = _("as if it had to end that way");
static const u8 sBias_O8_3[] = _("surprising");
static const u8 sBias_T9[] = _("Had to?");
static const u8 sBias_Q9[] = _("Did it have to end that way?");
static const u8 sBias_O9_0[] = _("the board does not say");
static const u8 sBias_O9_1[] = _("no");
static const u8 sBias_O9_2[] = _("yes");
static const u8 sBias_O9_3[] = _("always");
static const u8 sBias_T10[] = _("A fallacy");
static const u8 sBias_Q10[] = _("A fallacy is an argument that...");
static const u8 sBias_O10_0[] = _("is true");
static const u8 sBias_O10_1[] = _("is long");
static const u8 sBias_O10_2[] = _("works");
static const u8 sBias_O10_3[] = _("looks as if it works");
static const u8 sBias_T11[] = _("The speaker");
static const u8 sBias_Q11[] = _("Attacking the speaker instead of\nthe argument is...");
static const u8 sBias_O11_0[] = _("a fallacy");
static const u8 sBias_O11_1[] = _("a proof");
static const u8 sBias_O11_2[] = _("polite");
static const u8 sBias_O11_3[] = _("framing");
static const u8 sBias_T12[] = _("More than two");
static const u8 sBias_Q12[] = _("Offering two choices where there\nare more is...");
static const u8 sBias_O12_0[] = _("an anchor");
static const u8 sBias_O12_1[] = _("a cue");
static const u8 sBias_O12_2[] = _("a fallacy");
static const u8 sBias_O12_3[] = _("fair");

static const struct ExamQuestion sQuestions_Bias[] =
{
    { sBias_T1, sBias_Q1, { sBias_O1_0, sBias_O1_1, sBias_O1_2, sBias_O1_3 }, 2 },
    { sBias_T2, sBias_Q2, { sBias_O2_0, sBias_O2_1, sBias_O2_2, sBias_O2_3 }, 4 },
    { sBias_T3, sBias_Q3, { sBias_O3_0, sBias_O3_1, sBias_O3_2, sBias_O3_3 }, 1 },
    { sBias_T4, sBias_Q4, { sBias_O4_0, sBias_O4_1, sBias_O4_2, sBias_O4_3 }, 3 },
    { sBias_T5, sBias_Q5, { sBias_O5_0, sBias_O5_1, sBias_O5_2, sBias_O5_3 }, 2 },
    { sBias_T6, sBias_Q6, { sBias_O6_0, sBias_O6_1, sBias_O6_2, sBias_O6_3 }, 4 },
    { sBias_T7, sBias_Q7, { sBias_O7_0, sBias_O7_1, sBias_O7_2, sBias_O7_3 }, 1 },
    { sBias_T8, sBias_Q8, { sBias_O8_0, sBias_O8_1, sBias_O8_2, sBias_O8_3 }, 3 },
    { sBias_T9, sBias_Q9, { sBias_O9_0, sBias_O9_1, sBias_O9_2, sBias_O9_3 }, 2 },
    { sBias_T10, sBias_Q10, { sBias_O10_0, sBias_O10_1, sBias_O10_2, sBias_O10_3 }, 4 },
    { sBias_T11, sBias_Q11, { sBias_O11_0, sBias_O11_1, sBias_O11_2, sBias_O11_3 }, 1 },
    { sBias_T12, sBias_Q12, { sBias_O12_0, sBias_O12_1, sBias_O12_2, sBias_O12_3 }, 3 },
};

static const u8 sSection_Bias[] = _("BIAS");

static const u8 sError_T1[] = _("Guessing");
static const u8 sError_Q1[] = _("A mind is always guessing...");
static const u8 sError_O1_0[] = _("its own name");
static const u8 sError_O1_1[] = _("nothing");
static const u8 sError_O1_2[] = _("what comes next");
static const u8 sError_O1_3[] = _("the weather");
static const u8 sError_T2[] = _("Surprise");
static const u8 sError_Q2[] = _("Surprise is the size of...");
static const u8 sError_O2_0[] = _("the room");
static const u8 sError_O2_1[] = _("the miss");
static const u8 sError_O2_2[] = _("the prize");
static const u8 sError_O2_3[] = _("the answer");
static const u8 sError_T3[] = _("Mostly");
static const u8 sError_Q3[] = _("Learning is mostly...");
static const u8 sError_O3_0[] = _("waiting");
static const u8 sError_O3_1[] = _("memorising");
static const u8 sError_O3_2[] = _("guessing right");
static const u8 sError_O3_3[] = _("correcting");
static const u8 sError_T4[] = _("A big miss");
static const u8 sError_Q4[] = _("A bigger miss means a bigger...");
static const u8 sError_O4_0[] = _("correction");
static const u8 sError_O4_1[] = _("prize");
static const u8 sError_O4_2[] = _("silence");
static const u8 sError_O4_3[] = _("mistake next time");
static const u8 sError_T5[] = _("Too much at once");
static const u8 sError_Q5[] = _("Correct too much at once and\nyou...");
static const u8 sError_O5_0[] = _("stop");
static const u8 sError_O5_1[] = _("forget");
static const u8 sError_O5_2[] = _("overshoot");
static const u8 sError_O5_3[] = _("get it exactly");
static const u8 sError_T6[] = _("Getting closer");
static const u8 sError_Q6[] = _("What gets closer?");
static const u8 sError_O6_0[] = _("guessing again");
static const u8 sError_O6_1[] = _("small corrections, many times");
static const u8 sError_O6_2[] = _("one huge correction");
static const u8 sError_O6_3[] = _("no correction");
static const u8 sError_T7[] = _("Many times");
static const u8 sError_Q7[] = _("Small corrections should be made...");
static const u8 sError_O7_0[] = _("all at once");
static const u8 sError_O7_1[] = _("never");
static const u8 sError_O7_2[] = _("once");
static const u8 sError_O7_3[] = _("many times");
static const u8 sError_T8[] = _("Noticed");
static const u8 sError_Q8[] = _("A mistake you notice teaches more\nthan...");
static const u8 sError_O8_0[] = _("an answer you were given");
static const u8 sError_O8_1[] = _("nothing");
static const u8 sError_O8_2[] = _("a second mistake");
static const u8 sError_O8_3[] = _("a book");
static const u8 sError_T9[] = _("Being sure");
static const u8 sError_Q9[] = _("Being sure is...");
static const u8 sError_O9_0[] = _("proof");
static const u8 sError_O9_1[] = _("the answer");
static const u8 sError_O9_2[] = _("not evidence");
static const u8 sError_O9_3[] = _("a skill");
static const u8 sError_T10[] = _("The surest");
static const u8 sError_Q10[] = _("The surest people learn...");
static const u8 sError_O10_0[] = _("the most");
static const u8 sError_O10_1[] = _("the least");
static const u8 sError_O10_2[] = _("the same");
static const u8 sError_O10_3[] = _("first");
static const u8 sError_T11[] = _("Why");
static const u8 sError_Q11[] = _("Why do the surest learn least?");
static const u8 sError_O11_0[] = _("they sleep less");
static const u8 sError_O11_1[] = _("they read more");
static const u8 sError_O11_2[] = _("the board does not say");
static const u8 sError_O11_3[] = _("nothing surprises them");
static const u8 sError_T12[] = _("No miss");
static const u8 sError_Q12[] = _("No miss means...");
static const u8 sError_O12_0[] = _("no surprise");
static const u8 sError_O12_1[] = _("a correction");
static const u8 sError_O12_2[] = _("a lesson");
static const u8 sError_O12_3[] = _("a big surprise");

static const struct ExamQuestion sQuestions_Error[] =
{
    { sError_T1, sError_Q1, { sError_O1_0, sError_O1_1, sError_O1_2, sError_O1_3 }, 3 },
    { sError_T2, sError_Q2, { sError_O2_0, sError_O2_1, sError_O2_2, sError_O2_3 }, 2 },
    { sError_T3, sError_Q3, { sError_O3_0, sError_O3_1, sError_O3_2, sError_O3_3 }, 4 },
    { sError_T4, sError_Q4, { sError_O4_0, sError_O4_1, sError_O4_2, sError_O4_3 }, 1 },
    { sError_T5, sError_Q5, { sError_O5_0, sError_O5_1, sError_O5_2, sError_O5_3 }, 3 },
    { sError_T6, sError_Q6, { sError_O6_0, sError_O6_1, sError_O6_2, sError_O6_3 }, 2 },
    { sError_T7, sError_Q7, { sError_O7_0, sError_O7_1, sError_O7_2, sError_O7_3 }, 4 },
    { sError_T8, sError_Q8, { sError_O8_0, sError_O8_1, sError_O8_2, sError_O8_3 }, 1 },
    { sError_T9, sError_Q9, { sError_O9_0, sError_O9_1, sError_O9_2, sError_O9_3 }, 3 },
    { sError_T10, sError_Q10, { sError_O10_0, sError_O10_1, sError_O10_2, sError_O10_3 }, 2 },
    { sError_T11, sError_Q11, { sError_O11_0, sError_O11_1, sError_O11_2, sError_O11_3 }, 4 },
    { sError_T12, sError_Q12, { sError_O12_0, sError_O12_1, sError_O12_2, sError_O12_3 }, 1 },
};

static const u8 sSection_Error[] = _("ERROR");

//  One row per floor. A section added here is a section on the paper; nothing else changes.
static const struct ExamSection sSections[] =
{
    { sSection_Language, sQuestions_Language, ARRAY_COUNT(sQuestions_Language) },
    { sSection_Attention, sQuestions_Attention, ARRAY_COUNT(sQuestions_Attention) },
    { sSection_Memory, sQuestions_Memory, ARRAY_COUNT(sQuestions_Memory) },
    { sSection_Categories, sQuestions_Categories, ARRAY_COUNT(sQuestions_Categories) },
    { sSection_Learning, sQuestions_Learning, ARRAY_COUNT(sQuestions_Learning) },
    { sSection_Bias, sQuestions_Bias, ARRAY_COUNT(sQuestions_Bias) },
    { sSection_Error, sQuestions_Error, ARRAY_COUNT(sQuestions_Error) },
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
enum { MODE_LIST, MODE_QUESTION, MODE_UNANSWERED, MODE_CONFIRM, MODE_LEAVING, MODE_DIPLOMA };
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
static EWRAM_DATA bool8 sShowDiploma = FALSE;   // T-218: open the screen as the certificate, not the paper

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

//  T-218: THE DIPLOMA. The school's own paper and ink, and the best mark on it. No hue: a diploma has no type,
//  and the one place in the game that tests you gave its certificate the same plainness.
static void DrawDiploma(void)
{
    static const u8 sSchool[]   = _("CALLOW SCHOOL");
    static const u8 sCertify[]  = _("This is to certify that");
    static const u8 sSat[]      = _("sat the paper, all seven parts,");
    static const u8 sPassed[]   = _("and passed it.");
    static const u8 sMark[]     = _("MARK  ");
    static const u8 sGap[]      = _("   ");
    static const u8 sPutAway[]  = _("A  PUT IT AWAY");
    u8 buf[32], *p;
    u16 best = VarGet(VAR_SCHOOL_EXAM_BEST);

    FillWindowPixelBuffer(WIN_HEADER, PIXEL_FILL(1));
    Print(WIN_HEADER, sInk, (224 - GetStringWidth(FONT_NORMAL, sSchool, 0)) / 2, 1, sSchool);
    CopyWindowToVram(WIN_HEADER, COPYWIN_FULL);
    FillWindowPixelBuffer(WIN_BODY, PIXEL_FILL(1));
    Print(WIN_BODY, sInk, (224 - GetStringWidth(FONT_NORMAL, sCertify, 0)) / 2, 8, sCertify);
    Print(WIN_BODY, sInk, (224 - GetStringWidth(FONT_NORMAL, gSaveBlock2Ptr->playerName, 0)) / 2, 30, gSaveBlock2Ptr->playerName);
    Print(WIN_BODY, sInk, (224 - GetStringWidth(FONT_NORMAL, sSat, 0)) / 2, 52, sSat);
    Print(WIN_BODY, sInk, (224 - GetStringWidth(FONT_NORMAL, sPassed, 0)) / 2, 66, sPassed);
    CopyWindowToVram(WIN_BODY, COPYWIN_FULL);
    FillWindowPixelBuffer(WIN_FOOTER, PIXEL_FILL(1));
    p = StringCopy(buf, sMark);
    p = ConvertIntToDecimalStringN(p, best, STR_CONV_MODE_LEFT_ALIGN, 3);
    p = StringCopy(p, sGap);
    StringCopy(p, GradeLetter(best));
    Print(WIN_FOOTER, sInk, (224 - GetStringWidth(FONT_NORMAL, buf, 0)) / 2, 0, buf);
    Print(WIN_FOOTER, sFaint, (224 - GetStringWidth(FONT_NORMAL, sPutAway, 0)) / 2, 16, sPutAway);
    CopyWindowToVram(WIN_FOOTER, COPYWIN_FULL);
}

static void DrawAll(void)
{
    if (sExam->mode == MODE_DIPLOMA)
    {
        DrawDiploma();
        return;
    }
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
    case MODE_DIPLOMA:
        if (JOY_NEW(A_BUTTON | B_BUTTON))
        {
            PlaySE(SE_SELECT);
            Leave();
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
    if (sShowDiploma)
        sExam->mode = MODE_DIPLOMA;
    sShowDiploma = FALSE;
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

//  T-218: the DIPLOMA, shown on the same paper. The script fades first and waits.
void School_ShowDiploma(void)
{
    QuestLog_CutRecording();
    sShowDiploma = TRUE;
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
    u8 *p = dest;                       // the page's own title says THE PAPER

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
