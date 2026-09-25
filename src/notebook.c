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
#include "constants/species.h"   // T-257
#include "pokemon.h"   // T-257
#include "pokedex.h"   // T-257
#include "data.h"   // T-257: gSpeciesNames
#include "book_reader.h"
#include "constants/songs.h"
#include "data/notebook_documents.h"   // T-224: the documents tools/gbadocs.py has placed

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
//  the choice back in VAR_RESULT. A chosen entry opens FULL SCREEN on a ruled sheet (book_reader.c, T-223) --
//  the message box showed two lines at a time and never said how many were left. The list lives on the HEAP --
//  EWRAM has under a kilobyte free.

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

//  An entry is held when FLAG is set and, if it has one, FLAG2 as well. KIND says where its words come from:
//  stored, or the exam's record written on demand. ARG 1 means the text's first paragraph is a heading the
//  page's title already carries -- the syllabus's COURSE SYLLABUS / 1F LANGUAGE -- and the page leaves it off. The floors' boards were filed here once (batch 5); since
//  T-223 they are the TEXTBOOK's own chapters, and a book does not need a copy of itself in the notebook.
enum { NB_KIND_TEXT, NB_KIND_EXAM, NB_KIND_CHOICES };

struct NotebookEntry
{
    u8 section;
    u8 kind;
    u8 arg;
    u16 flag;
    u16 flag2;
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
extern const u8 ViridianCity_School_5F_Text_Syllabus[];
extern const u8 ViridianCity_School_6F_Text_Syllabus[];
extern const u8 ViridianCity_School_7F_Text_Syllabus[];
extern const u8 ViridianCity_School_2F_Text_Syllabus[];
extern const u8 ViridianCity_School_3F_Text_Syllabus[];
extern const u8 ViridianCity_School_4F_Text_Syllabus[];



static const u8 sTitle_Syllabus_Language[] = _("1F LANGUAGE");
static const u8 sTitle_Syllabus_Attention[] = _("2F ATTENTION");
static const u8 sTitle_Syllabus_Memory[] = _("3F MEMORY");
static const u8 sTitle_Syllabus_Categories[] = _("4F CATEGORIES");

static const u8 sTitle_Loose_Stone[]       = _("THE STONE");
static const u8 sTitle_Paper[]             = _("THE PAPER");
static const u8 sTitle_Syllabus_Learning[] = _("5F LEARNING");
static const u8 sTitle_Syllabus_Bias[]     = _("6F BIAS");
static const u8 sTitle_Syllabus_Error[]    = _("7F ERROR");

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


//  T-218: THE READING ROOM's shelves. DRAFT WORDING, awaiting the user's approval. Each is a document and states
//  no conclusion (docs/school.md 9, rule 4); each is drawn from the rock opera (school.md 10).

//  LOOSE PAGES 3 -- *Poly and Fields*: the two meanings of the word, and a coat.
static const u8 sTitle_Loose_Fields[] = _("FIELDS");
static const u8 sText_Loose_Fields[] = _(
    "Not the exercise book this time.\n"
    "A loose sheet, a grown-up hand.\p"
    "Fields, the way a university means\n"
    "it: each one its own building.\p"
    "Fields, the way the maths means it:\n"
    "one surface, one set of rules.\p"
    "Nobody here believes the second.\n"
    "They walk between the buildings\p"
    "with their coats on.\p"
    "I keep not needing a coat.");

//  LOOSE PAGES 5 -- *Love Persists*: a letter that waits instead of arguing.
static const u8 sTitle_Loose_Unsent[] = _("UNSENT");
static const u8 sText_Loose_Unsent[] = _(
    "A letter, folded twice, never sent.\n"
    "No name at the top.\p"
    "I never thought you would break\n"
    "this. I still do not think you have.\p"
    "Some things wait better than they\n"
    "argue.\p"
    "I am going to be one of them.");

//  LOOSE PAGES 7 -- DRAFT WORDING, awaiting the user's approval. *The Folds of Awareness* (Seeing Sharp, 2025):
//  six folds of one sheet, and a half-fold between the third and fourth. Every step is only paper -- a fold, a
//  crease, a corner -- and it never says what it is a diagram of (craft rule 1). It is not the GREEN PATH
//  scientist's (4.33 rules that nothing may prove he was there), so a player who has heard him is the only one
//  who will see it as anything but instructions.
static const u8 sTitle_Loose_Folds[] = _("FOLDS");
static const u8 sText_Loose_Folds[] = _(
    "Instructions in pencil, pinned to\n"
    "the wall. No pictures.\p"
    "1. Start with the sheet flat.\n"
    "Nothing on it yet.\p"
    "2. Fold it once. Now one side has\n"
    "moved, and one side was met.\p"
    "3. Open it. Fold across the first\n"
    "crease. Where there was one line,\n"
    "squares, side by side.\p"
    "Between three and four: press it\n"
    "flat with your palm. It keeps\n"
    "trying to be one sheet again.\p"
    "4. Fold it corner to corner. Each\n"
    "half lies on the other, the wrong\n"
    "way round.\p"
    "5. The same fold again, smaller,\n"
    "and again, until your hands know\n"
    "it.\p"
    "6. Unfold it all the way. The same\n"
    "sheet. Every crease still in it.\p"
    "Do not cut.");

//  LAB NOTES 2 -- *Quantum Translations*, the first verse: what counts as an observer.
static const u8 sTitle_Lab_Observer[] = _("THE OBSERVER");
static const u8 sText_Lab_Observer[] = _(
    "LAB NOTES. A session with ARTSAI.\p"
    "Asked what counts as an observer.\n"
    "It said: one that observes.\p"
    "A duck, then? A tree? Alive, both.\p"
    "This rock? Not alive like us, but\n"
    "it keeps itself steady.\p"
    "It did not answer that.\n"
    "Nobody else has either, yet.\p"
    "Next: what if the one watching is\n"
    "not alive at all?");

//  LAB NOTES 3 -- *Quantum Translations*, the second verse: the box, read by something that is not a person.
static const u8 sTitle_Lab_Box[] = _("THE BOX");
static const u8 sText_Lab_Box[] = _(
    "LAB NOTES. The box.\p"
    "The usual sealed box. This time,\n"
    "the one reading it is not a person.\p"
    "It only prints ALIVE or DEAD.\p"
    "Does its reading settle the box,\n"
    "the way a person's is said to?\p"
    "If not, the only other answer is\n"
    "that mine does.\p"
    "And then the effect came before\n"
    "the cause.\p"
    "Neither is comfortable.\n"
    "That is usually a good sign.");

//  PEER REVIEW 1 -- *The Owl and the Code*, the first verse: a good paper, and short.
static const u8 sTitle_Review_Position[] = _("POSITION PAPER");
static const u8 sText_Review_Position[] = _(
    "A position paper, typed. Signed at\n"
    "the bottom in a careful hand.\p"
    "Machines process what is there.\n"
    "Inputs and outputs.\p"
    "Nothing breaks through, because\n"
    "nothing is behind them to break.\p"
    "It is well argued.\n"
    "It is also very short.");

//  PEER REVIEW 2 -- *The Owl and the Code*, the pre-chorus: the question in the margin, left open.
static const u8 sTitle_Review_Draft[] = _("SECOND DRAFT");
static const u8 sText_Review_Draft[] = _(
    "The same paper, a later draft.\n"
    "The same hand, less careful.\p"
    "The argument has not changed.\p"
    "One line has been added in pencil,\n"
    "in the margin:\p"
    "“But are we not shaped by\n"
    "the world that we see?”\p"
    "It has not been crossed out.\n"
    "It has not been answered either.");

//  T-219 (batch 8): the documents the game ALREADY SHOWS, filed when they are read. Like the syllabus, each entry
//  is the thing's own text and not a copy of it -- what the player read on the terminal is what the page holds.
//  The titles are the only new words, and they are DRAFT: a date where the document has one, and what the
//  document IS where it does not.
extern const u8 PokemonMansion_1F_Text_IterLog[];
extern const u8 PokemonMansion_1F_Text_NewMonDiscoveredInGuyanaJungle[];
extern const u8 PokemonMansion_1F_Text_ChristenedDiscoveredMonMew[];
extern const u8 PokemonMansion_1F_Text_MewGaveBirthToMewtwo[];
extern const u8 PokemonMansion_B1F_Text_MewtwoIsFarTooPowerful[];
extern const u8 CinnabarIsland_PokemonLab_Entrance_Text_MeetingRoomSign[];
extern const u8 CinnabarIsland_PokemonLab_Entrance_Text_RAndDRoomSign[];
extern const u8 CinnabarIsland_PokemonLab_Entrance_Text_PhotoOfLabFounderDrFuji[];
extern const u8 SaffronCity_MrPsychicsHouse_Text_OwlConcession[];
extern const u8 SaffronCity_MrPsychicsHouse_Text_NotMineToDeliver[];
extern const u8 SaffronCity_Text_SilphsLatestProduct[];

static const u8 sTitle_Lab_Iter[]          = _("ITER 35");
//  T-235: the session the station's log withholds, told by ARTSAI -- the Five Witnesses' reward. The words are the
//  user's to approve (on the private field-test page); until then the ROM holds only where they go.
static const u8 sTitle_Lab_Transcript[]    = _("TRANSCRIPT");

//  T-257: WHAT YOU CHOSE. The INDEX's one page about the PLAYER, written on demand from what the save already
//  records -- REASON or INSTINCT, whose shoes, the first daemon, the rival's name, the paper, S.T.A.R.R. -- so it
//  stays current and needs no save data of its own. INFERRED marks what happened before the INDEX was yours; the
//  word, not a bracket, because the charmap has none (trap 26). It is exactly as sure of you as it is of every
//  daemon (4.2), and says so in its last two lines, which is the point and is never remarked on.
extern const u8 gText_Boy[];    // "REASON" and "INSTINCT" (strings.c): the choice at the title
extern const u8 gText_Girl[];
static const u8 sTitle_Lab_Choices[]   = _("WHAT YOU CHOSE");
//  PLACEHOLDER labels: the page's words are a NOTEBOOK draft, and NOTEBOOK drafts stay on the private page until the
//  user approves them. The structure is final -- what it reads, when, and in what order -- and the words drop in.
static const u8 sChoices_Head[]        = _("LAB NOTES. The words of this page\nwait to be approved.\p");
static const u8 sChoices_Began[]       = _("TITLE: ");
static const u8 sChoices_ShoesDad[]    = _("SHOES: FATHER\p");
static const u8 sChoices_ShoesMom[]    = _("SHOES: MOTHER\p");
static const u8 sChoices_First[]       = _("FIRST: ");
static const u8 sChoices_Rival[]       = _("RIVAL: {RIVAL}\p");
static const u8 sChoices_Paper[]       = _("PAPER: PASSED\p");
static const u8 sChoices_Starr[]       = _("S.T.A.R.R.: BOUND\p");
static const u8 sChoices_End[]         = _("END");
static const u8 sChoices_Stop[]        = _("\p");

static void Notebook_WriteChoices(u8 *dest)
{
    static const u16 sStarters[] = { SPECIES_BULBASAUR, SPECIES_SQUIRTLE, SPECIES_CHARMANDER };
    u8 buf[400];
    u16 starter = VarGet(VAR_STARTER_MON);

    StringCopy(buf, sChoices_Head);
    StringAppend(buf, sChoices_Began);
    StringAppend(buf, gSaveBlock2Ptr->playerGender == MALE ? gText_Boy : gText_Girl);
    StringAppend(buf, sChoices_Stop);
    if (FlagGet(FLAG_SYS_B_DASH))
        StringAppend(buf, FlagGet(FLAG_SHOES_FROM_DAD) ? sChoices_ShoesDad : sChoices_ShoesMom);
    if (FlagGet(FLAG_SYS_POKEMON_GET) && starter < ARRAY_COUNT(sStarters))
    {
        StringAppend(buf, sChoices_First);
        StringAppend(buf, gSpeciesNames[sStarters[starter]]);
        StringAppend(buf, sChoices_Stop);
    }
    StringAppend(buf, sChoices_Rival);
    if (FlagGet(FLAG_GOT_DIPLOMA))
        StringAppend(buf, sChoices_Paper);
    if (GetSetPokedexFlag(SpeciesToNationalPokedexNum(SPECIES_MEWTWO), FLAG_GET_CAUGHT))
        StringAppend(buf, sChoices_Starr);
    StringAppend(buf, sChoices_End);
    StringExpandPlaceholders(dest, buf);
}
static const u8 sText_Lab_Transcript[]     = _("TRANSCRIPT. SEPT 3.\pThe page is here. Its words are\nwaiting to be approved.");
static const u8 sTitle_Run_Mar4[]          = _("MAR 4");
static const u8 sTitle_Run_Apr19[]         = _("APR 19");
static const u8 sTitle_Run_Aug12[]         = _("AUG 12");
static const u8 sTitle_Run_Fatal[]         = _("NO DATE");
static const u8 sTitle_File_Minutes[]      = _("MINUTES");
static const u8 sTitle_File_Requisition[]  = _("REQUISITION");
static const u8 sTitle_File_Complete[]     = _("PR-0903");
static const u8 sTitle_Review_Transcript[] = _("TRANSCRIPT");
static const u8 sTitle_Review_Sealed[]     = _("SEALED");
static const u8 sTitle_Prospectus_Scores[]  = _("REVIEW SCORES");

//  THE AUTHORED ORDER. A new entry goes where it belongs in the finished file, not at the end.
static const struct NotebookEntry sEntries[] =
{
    //  SCHOOL NOTES: the seven syllabi in floor order, then the paper.
    { NB_SCHOOL_NOTES, NB_KIND_TEXT,  1, FLAG_SCHOOL_SYLLABUS_LANGUAGE,   0, sTitle_Syllabus_Language,   ViridianCity_School_Text_Syllabus },
    { NB_SCHOOL_NOTES, NB_KIND_TEXT,  1, FLAG_SCHOOL_SYLLABUS_ATTENTION,  0, sTitle_Syllabus_Attention,  ViridianCity_School_2F_Text_Syllabus },
    { NB_SCHOOL_NOTES, NB_KIND_TEXT,  1, FLAG_SCHOOL_SYLLABUS_MEMORY,     0, sTitle_Syllabus_Memory,     ViridianCity_School_3F_Text_Syllabus },
    { NB_SCHOOL_NOTES, NB_KIND_TEXT,  1, FLAG_SCHOOL_SYLLABUS_CATEGORIES, 0, sTitle_Syllabus_Categories, ViridianCity_School_4F_Text_Syllabus },
    { NB_SCHOOL_NOTES, NB_KIND_TEXT,  1, FLAG_SCHOOL_SYLLABUS_LEARNING,   0, sTitle_Syllabus_Learning,   ViridianCity_School_5F_Text_Syllabus },
    { NB_SCHOOL_NOTES, NB_KIND_TEXT,  1, FLAG_SCHOOL_SYLLABUS_BIAS,       0, sTitle_Syllabus_Bias,       ViridianCity_School_6F_Text_Syllabus },
    { NB_SCHOOL_NOTES, NB_KIND_TEXT,  1, FLAG_SCHOOL_SYLLABUS_ERROR,      0, sTitle_Syllabus_Error,      ViridianCity_School_7F_Text_Syllabus },
    //  T-217: written on demand -- every sitting's mark, and a paper under way if there is one.
    { NB_SCHOOL_NOTES, NB_KIND_EXAM,  0, FLAG_SCHOOL_EXAM_OPENED,       0, sTitle_Paper,               NULL },
    { NB_LOOSE_PAGES,  NB_KIND_TEXT,  0, FLAG_NOTEBOOK_LOOSE_STONE,     0, sTitle_Loose_Stone,         sText_Loose_Stone },
    NB_DOC_LOOSE_PAGES_2
    { NB_LOOSE_PAGES,  NB_KIND_TEXT,  0, FLAG_NOTEBOOK_LOOSE_FIELDS,    0, sTitle_Loose_Fields,        sText_Loose_Fields },
    NB_DOC_LOOSE_PAGES_4
    { NB_LOOSE_PAGES,  NB_KIND_TEXT,  0, FLAG_NOTEBOOK_LOOSE_UNSENT,    0, sTitle_Loose_Unsent,        sText_Loose_Unsent },
    NB_DOC_LOOSE_PAGES_6
    { NB_LOOSE_PAGES,  NB_KIND_TEXT,  0, FLAG_NOTEBOOK_LOOSE_FOLDS,     0, sTitle_Loose_Folds,         sText_Loose_Folds },
    NB_DOC_LAB_NOTES_1
    { NB_LAB_NOTES,    NB_KIND_TEXT,  0, FLAG_NOTEBOOK_LAB_OBSERVER,    0, sTitle_Lab_Observer,        sText_Lab_Observer },
    { NB_LAB_NOTES,    NB_KIND_TEXT,  0, FLAG_NOTEBOOK_LAB_BOX,         0, sTitle_Lab_Box,             sText_Lab_Box },
    NB_DOC_LAB_NOTES_4
    { NB_LAB_NOTES,    NB_KIND_TEXT,  0, FLAG_NOTEBOOK_LAB_ITER,        0, sTitle_Lab_Iter,            PokemonMansion_1F_Text_IterLog },
    { NB_LAB_NOTES,    NB_KIND_TEXT,  0, FLAG_ARTSAI_PAGE,              0, sTitle_Lab_Transcript,      sText_Lab_Transcript },
    { NB_LAB_NOTES,    NB_KIND_CHOICES, 0, FLAG_INDEX_WRITES,           0, sTitle_Lab_Choices,         NULL },   // T-257
    NB_DOC_LAB_NOTES_6
    { NB_RUN_LOGS,     NB_KIND_TEXT,  0, FLAG_NOTEBOOK_RUN_MAR4,        0, sTitle_Run_Mar4,            PokemonMansion_1F_Text_NewMonDiscoveredInGuyanaJungle },
    { NB_RUN_LOGS,     NB_KIND_TEXT,  0, FLAG_NOTEBOOK_RUN_APR19,       0, sTitle_Run_Apr19,           PokemonMansion_1F_Text_ChristenedDiscoveredMonMew },
    { NB_RUN_LOGS,     NB_KIND_TEXT,  0, FLAG_NOTEBOOK_RUN_AUG12,       0, sTitle_Run_Aug12,           PokemonMansion_1F_Text_MewGaveBirthToMewtwo },
    { NB_RUN_LOGS,     NB_KIND_TEXT,  0, FLAG_NOTEBOOK_RUN_FATAL,       0, sTitle_Run_Fatal,           PokemonMansion_B1F_Text_MewtwoIsFarTooPowerful },
    NB_DOC_RUN_LOGS_5
    NB_DOC_RUN_LOGS_6
    NB_DOC_CORRESPONDENCE_1
    NB_DOC_CORRESPONDENCE_2
    NB_DOC_CORRESPONDENCE_3
    NB_DOC_CORRESPONDENCE_4
    NB_DOC_CORRESPONDENCE_5
    NB_DOC_CORRESPONDENCE_6
    { NB_THE_FILE,     NB_KIND_TEXT,  1, FLAG_NOTEBOOK_FILE_MINUTES,    0, sTitle_File_Minutes,        CinnabarIsland_PokemonLab_Entrance_Text_MeetingRoomSign },
    { NB_THE_FILE,     NB_KIND_TEXT,  1, FLAG_NOTEBOOK_FILE_REQUISITION, 0, sTitle_File_Requisition,   CinnabarIsland_PokemonLab_Entrance_Text_RAndDRoomSign },
    { NB_THE_FILE,     NB_KIND_TEXT,  0, FLAG_NOTEBOOK_FILE_COMPLETE,   0, sTitle_File_Complete,       CinnabarIsland_PokemonLab_Entrance_Text_PhotoOfLabFounderDrFuji },
    NB_DOC_THE_FILE_4
    NB_DOC_THE_FILE_5
    NB_DOC_THE_FILE_6
    NB_DOC_PROSPECTUS_1
    //  PROSPECTUS 2: the scores, and the notice's own congratulations -- the only commentary it has.
    { NB_PROSPECTUS,   NB_KIND_TEXT,  0, FLAG_NOTEBOOK_PROSPECTUS_SCORES, 0, sTitle_Prospectus_Scores, SaffronCity_Text_SilphsLatestProduct },
    NB_DOC_PROSPECTUS_3
    NB_DOC_PROSPECTUS_4
    NB_DOC_PROSPECTUS_5
    NB_DOC_PROSPECTUS_6
    { NB_PEER_REVIEW,  NB_KIND_TEXT,  0, FLAG_NOTEBOOK_REVIEW_POSITION, 0, sTitle_Review_Position,     sText_Review_Position },
    { NB_PEER_REVIEW,  NB_KIND_TEXT,  0, FLAG_NOTEBOOK_REVIEW_DRAFT,    0, sTitle_Review_Draft,        sText_Review_Draft },
    NB_DOC_PEER_REVIEW_3
    NB_DOC_PEER_REVIEW_4
    { NB_PEER_REVIEW,  NB_KIND_TEXT,  0, FLAG_NOTEBOOK_REVIEW_TRANSCRIPT, 0, sTitle_Review_Transcript, SaffronCity_MrPsychicsHouse_Text_OwlConcession },
    //  4.34 rule 1: a cover sheet, and the notebook refuses the rest -- which says it louder than no entry at all.
    { NB_PEER_REVIEW,  NB_KIND_TEXT,  0, FLAG_NOTEBOOK_REVIEW_SEALED,   0, sTitle_Review_Sealed,       SaffronCity_MrPsychicsHouse_Text_NotMineToDeliver },
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

static bool8 EntryHeld(u32 i)
{
    return FlagGet(sEntries[i].flag) && (sEntries[i].flag2 == 0 || FlagGet(sEntries[i].flag2));
}

static bool8 SectionHasEntry(u8 section)
{
    u32 i;

    for (i = 0; i < ARRAY_COUNT(sEntries); i++)
    {
        if (sEntries[i].section == section && EntryHeld(i))
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
        if (EntryHeld(i))
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
        if (sEntries[i].section == gSpecialVar_0x8005 && EntryHeld(i))
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

//  VAR_0x8006 is the entry. Its text goes to gStringVar4.
//  An entry must stay under 1000 bytes -- that is gStringVar4 -- and check_lexicon holds it there.
void Notebook_LoadEntry(void)
{
    if (gSpecialVar_0x8006 >= ARRAY_COUNT(sEntries))
        gStringVar4[0] = EOS;
    else if (sEntries[gSpecialVar_0x8006].kind == NB_KIND_EXAM)
        School_WriteExamRecord(gStringVar4);
    else if (sEntries[gSpecialVar_0x8006].kind == NB_KIND_CHOICES)
        Notebook_WriteChoices(gStringVar4);
    else
        StringExpandPlaceholders(gStringVar4, sEntries[gSpecialVar_0x8006].text);
}

//  T-223: VAR_0x8006 is the entry; it opens full screen on the NOTEBOOK's ruled sheet. The script fades first
//  and waits, and comes back to the list of pages.
//  T-224: a document found in the world opens where it is found, by its flag (VAR_0x8004), full screen as the
//  notebook would show it -- so reading one and filing it are the same page. tools/gbadocs.py calls this.
void Notebook_ReadFlagEntry(void)
{
    u16 i;

    for (i = 0; i < ARRAY_COUNT(sEntries); i++)
    {
        if (sEntries[i].flag == gSpecialVar_0x8004 && sEntries[i].kind == NB_KIND_TEXT)
        {
            StringExpandPlaceholders(gStringVar4, sEntries[i].text);   // {RIVAL} in a letter is the name the player gave
            BookReader_OpenNotebookPage(sEntries[i].title, gStringVar4);
            return;
        }
    }
    ScriptContext_Enable();
}

void Notebook_ReadEntry(void)
{
    const u8 *text = gStringVar4;

    if (gSpecialVar_0x8006 >= ARRAY_COUNT(sEntries))
        gSpecialVar_0x8006 = 0;
    Notebook_LoadEntry();
    if (sEntries[gSpecialVar_0x8006].arg == 1)
    {
        while (*text != EOS && *text != CHAR_PROMPT_CLEAR)
            text++;
        if (*text == CHAR_PROMPT_CLEAR)
            text++;
    }
    BookReader_OpenNotebookPage(sEntries[gSpecialVar_0x8006].title, text);
}
