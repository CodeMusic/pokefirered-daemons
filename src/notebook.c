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
#include "play.h"   // T-417: the SONGBOOK
#include "notebook.h"
#include "constants/songs.h"
#include "data/notebook_documents.h"   // T-224: the documents tools/gbadocs.py has placed
#include "overworld.h"   // T-403: a page's map, for the TOWN MAP
#include "region_map.h"
#include "quest_log.h"
#include "constants/maps.h"
#include "constants/region_map_sections.h"
#include "daemons_time.h"   // T-412: the dawn a dream waits for

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
    NB_INSIGHT,            // T-390: what a MARK and its understanding make together, in the player's own words
    NB_SONGBOOK,           // T-417: the songs PLAY knows, written as coloured notes
    NB_THE_YEAR,           // T-416: a line for a day of the year, as it arrives (DRAFT)
    NB_DREAM_LOG,          // T-412: YOUR DREAMS, each the first dawn after a milestone (DRAFT)
    NB_SECTION_COUNT
};

#define NB_CLOSE      0x7F
#define NB_SHOW_MAP   0x7E   // T-403: R on a page -- the script opens the TOWN MAP where it was found
#define NB_MAX_SHOWN  6

//  An entry is held when FLAG is set and, if it has one, FLAG2 as well. KIND says where its words come from:
//  stored, or the exam's record written on demand. ARG 1 means the text's first paragraph is a heading the
//  page's title already carries -- the syllabus's COURSE SYLLABUS / 1F LANGUAGE -- and the page leaves it off. The floors' boards were filed here once (batch 5); since
//  T-223 they are the TEXTBOOK's own chapters, and a book does not need a copy of itself in the notebook.
//  T-417: SONG is a SONGBOOK page -- ARG is its song (PLAY_SONG_*), and play.c writes it and its notes.
//  T-412: HER_DREAM is one of the attendant's dreams in the DREAM LOG -- ARG is its step, and where it led is written
//  under its words once the step is done.
enum { NB_KIND_TEXT, NB_KIND_EXAM, NB_KIND_CHOICES, NB_KIND_SONG, NB_KIND_HER_DREAM };

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
static const u8 sSection_Insight[]        = _("INSIGHT");
static const u8 sSection_Songbook[]       = _("SONGBOOK");   // T-417, DRAFT

extern const u8 gText_YearSection[];   // T-416: THE YEAR, src/daemons_holidays.c

#include "data/dream_log.h"   // T-412: the DREAM LOG's words and milestones, written by DAEMONS tools/gbadreamlog.py
#define ATTENDANT_DREAMS_NOTEBOOK     // T-412: HER DREAMS' pages; their words live in daemons_dreams.c (tools/gbadreams.py)
#include "data/attendant_dreams.h"
#undef ATTENDANT_DREAMS_NOTEBOOK

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
    [NB_INSIGHT]         = sSection_Insight,
    [NB_SONGBOOK]        = sSection_Songbook,
    [NB_THE_YEAR]        = gText_YearSection,   // T-416, src/daemons_holidays.c (DAEMONS tools/genholidays.py)
    [NB_DREAM_LOG]       = sSection_DreamLog,   // T-412, DRAFT (data/dream_log.h)
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

//  LOOSE PAGES 1 -- approved with T-401's clearer close (the user, 2026-10-15). A child's page, copied at the carving in
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
    "At home they say I see things\n"
    "that are not there.\p"
    "Here the rule sat still long\n"
    "enough to copy down.\p"
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
    "They walk between the buildings\n"
    "with their coats on.\p"
    "I keep not needing a coat. To me\n"
    "it is all one room.");

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

//  LOOSE PAGES 7 -- approved by the user 2026-09-26 (T-285), re-approved 2026-10-15 (T-401). *The Folds of
//  Awareness* (Seeing Sharp, 2025): six folds of one sheet, and a half-fold between the third and fourth. It
//  read as paper and nothing else (the user: "it isnt clear that it is talking about awareness"), so each step
//  now carries the essay's own word for its fold. The essay's conclusions (love, spirit, unity) stay out (craft
//  rule 1), and it is still not the GREEN PATH scientist's (4.33).
static const u8 sTitle_Loose_Folds[] = _("FOLDS");
static const u8 sText_Loose_Folds[] = _(
    "Instructions in pencil, pinned to\n"
    "the wall. No pictures. One word\n"
    "beside each step.\p"
    "1. AWARENESS. Start with the sheet\n"
    "flat. Nothing on it yet.\p"
    "2. CHANGE. Fold it once. Now one\n"
    "side has moved, and one side was\n"
    "met. There is a before.\p"
    "3. SPACE. Open it. Fold across the\n"
    "first crease. Where there was one\n"
    "line, squares, side by side.\p"
    "Between three and four: press it\n"
    "flat with your palm. It keeps\n"
    "trying to be one sheet again.\p"
    "4. MIRROR. Fold it corner to\n"
    "corner. Each half lies on the\n"
    "other, the wrong way round.\p"
    "5. PATTERN. The same fold again,\n"
    "smaller, and again, until your\n"
    "hands know it.\p"
    "6. RETURN. Unfold it all the way.\n"
    "The same sheet. Every crease\n"
    "still in it.\p"
    "Do not cut.");

//  LOOSE PAGES 8 -- T-10, DRAFT WORDING. The S.S. ANNE band's sheet, from DAVID: a key signature and not one note.
//  It is the key the singing fir on the pier has lost (lineage 3b); nothing says so.
static const u8 sTitle_Loose_Key[] = _("THE BAND'S SHEET");
static const u8 sText_Loose_Key[] = _(
        "A sheet from the S.S. ANNE's piano\n"
        "bench. DAVID gave it to you.\p"
        "Staff lines, and at the start of\n"
        "each one a clef and four flats.\p"
        "Not one note after them.\p"
        "Pencilled along the bottom: THE\n"
        "KEY. THE TUNE KNOWS THE REST.");   // T-401: restored -- the REST->IDLE move sweep had reached it (bc7cba35f)

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
//  The words, APPROVED as drafts by the user 2026-10-01 ("yes to all"), with one fix: the rival's line names the
//  rival as the player named them ({RIVAL}), never a fixed name. DRAFT, every line.
static const u8 sChoices_Head[]        = _("Compiled by the INDEX, on request.\p");
static const u8 sChoices_Began[]       = _("INFERRED: You began as ");
static const u8 sChoices_ShoesDad[]    = _("INFERRED: The shoes you run in\nwere your father's.\p");
static const u8 sChoices_ShoesMom[]    = _("INFERRED: The shoes you run in\nwere your mother's.\p");
static const u8 sChoices_First[]       = _("INFERRED: Your first daemon was\n");
static const u8 sChoices_Rival[]       = _("You told CRYSTAL to call him\n{RIVAL}.\p");
static const u8 sChoices_Paper[]       = _("You sat the paper at CALLOW,\nand passed.\p");
static const u8 sChoices_Starr[]       = _("You bound S.T.A.R.R.\p");
static const u8 sChoices_End[]         = _("Everything above is complete.\nNothing above is missing.");
static const u8 sChoices_Stop[]        = _(".\p");

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

//  T-417: the SONGBOOK's titles, DRAFT -- one per ROOT/MARK pair, as DAEMONS docs/play_songs.json names them
//  (tools/genplaysongs.py fails if the two disagree).
static const u8 sTitle_Song_Fir[]          = _("O CHRISTMAS TREE");
static const u8 sTitle_Song_1[]            = _("TWELVE DAYS OF DECEMBER");   // SLATE / NAMING
static const u8 sTitle_Song_2[]            = _("THE REALIZATION");           // SLOPE / STILL
static const u8 sTitle_Song_3[]            = _("FORGIVENESS OFFERED");       // SENSE / MEND
static const u8 sTitle_Song_4[]            = _("SEQUEL THE GUARDIAN");       // FIT / PRACTICE
static const u8 sTitle_Song_5[]            = _("HARMONY IN BALANCE");        // SKEW / STANCE
static const u8 sTitle_Song_6[]            = _("THE SHIMMERING CANVAS");     // FRAME / NOTICE
static const u8 sTitle_Song_7[]            = _("REFLECTIONS WITHIN");        // HEAT / REFLECT

#include "data/wisdom.h"   // T-390: the seven insights, written by DAEMONS tools/gbawisdom.py

//  T-403 (the user, 2026-10-15): where each entry was found, what it is, and its SEEN flag -- DAEMONS
//  docs/notebook_notes.json, written in by tools/gbanotebook.py.
struct NotebookNote
{
    u16 flag;
    u8 seen;
    u16 map;
    const u8 *about;
};
#include "data/notebook_notes.h"
#include "constants/holidays.h"
#include "data/holiday_lines.h"   // T-416: THE YEAR's entries (DAEMONS tools/genholidays.py)

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
    { NB_LOOSE_PAGES,  NB_KIND_TEXT,  0, FLAG_NOTEBOOK_LOOSE_KEY,       0, sTitle_Loose_Key,           sText_Loose_Key },
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
    { NB_THE_FILE,     NB_KIND_TEXT,  0, FLAG_NOTEBOOK_FILE_MINUTES,    0, sTitle_File_Minutes,           CinnabarIsland_PokemonLab_Entrance_Text_MeetingRoomSign },
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
    //  T-390 (bible 0.7, Wisdom; rule 2 amended): INSIGHT grows one line a pair and is never numbered.
    NB_INSIGHT_ENTRIES
    //  T-417: the SONGBOOK. O CHRISTMAS TREE, then the seven (P4: their motifs chosen by the user by ear), each
    //  written in once it is heard in its ROOT's place and played back.
    { NB_SONGBOOK,     NB_KIND_SONG,  PLAY_SONG_FIR,      FLAG_SONG_FIR, 0, sTitle_Song_Fir,         NULL },
    { NB_SONGBOOK,     NB_KIND_SONG,  PLAY_SONG_NAMING,   FLAG_SONG_1,   0, sTitle_Song_1,           NULL },
    { NB_SONGBOOK,     NB_KIND_SONG,  PLAY_SONG_STILL,    FLAG_SONG_2,   0, sTitle_Song_2,           NULL },
    { NB_SONGBOOK,     NB_KIND_SONG,  PLAY_SONG_MEND,     FLAG_SONG_3,   0, sTitle_Song_3,           NULL },
    { NB_SONGBOOK,     NB_KIND_SONG,  PLAY_SONG_PRACTICE, FLAG_SONG_4,   0, sTitle_Song_4,           NULL },
    { NB_SONGBOOK,     NB_KIND_SONG,  PLAY_SONG_STANCE,   FLAG_SONG_5,   0, sTitle_Song_5,           NULL },
    { NB_SONGBOOK,     NB_KIND_SONG,  PLAY_SONG_NOTICE,   FLAG_SONG_6,   0, sTitle_Song_6,           NULL },
    { NB_SONGBOOK,     NB_KIND_SONG,  PLAY_SONG_REFLECT,  FLAG_SONG_7,   0, sTitle_Song_7,           NULL },
    //  T-416: THE YEAR, in the order of the year -- each line once it has arrived on its day.
    NB_YEAR_ENTRIES
    //  T-412: the DREAM LOG -- YOUR DREAMS, in the draft's order, each once it has been dreamt.
    NB_DREAM_ENTRIES
    //  T-412: then HER DREAMS, in step order, each once the attendant has told it.
    NB_HER_DREAM_ENTRIES
};

#if DAEMONS_DEBUG
//  T-394: GAME EVENTS' NOTEBOOK, EVERY PAGE -- every document's flags set, so the NOTEBOOK opens full. Not the paper's
//  record (it is written from the exam), not the INDEX's page (its flag is the story's ending) and not INSIGHT (its
//  arrival is a thing to test), nor THE YEAR (T-416, the same), nor the DREAM LOG (T-412): those come the way they come.
void Notebook_DebugFillAll(void)
{
    u32 i;

    for (i = 0; i < ARRAY_COUNT(sEntries); i++)
    {
        if (sEntries[i].kind != NB_KIND_TEXT || sEntries[i].section == NB_INSIGHT || sEntries[i].section == NB_THE_YEAR
         || sEntries[i].section == NB_DREAM_LOG)
            continue;
        FlagSet(sEntries[i].flag);
        if (sEntries[i].flag2 != 0)
            FlagSet(sEntries[i].flag2);
    }
    FlagSet(FLAG_NOTEBOOK_SEEN_MIGRATED);   // T-403: every page filed now and unread, so the marks can be seen
}
#endif

//  T-390: the first step after a MARK and its understanding are both held (and the NOTEBOOK is there to write in),
//  an insight arrives -- once each. field_control_avatar.c's step hook asks, as it asks for OPUS's one beat; the
//  line is left in gStringVar4 for Daemons_EventScript_Insight to say.
bool8 Daemons_TryInsightArrives(void)
{
    u32 i;

    if (!FlagGet(FLAG_GOT_NOTEBOOK))
        return FALSE;
    for (i = 0; i < ARRAY_COUNT(sWisdomPairs); i++)
    {
        if (FlagGet(sWisdomPairs[i].mark) && FlagGet(sWisdomPairs[i].root) && !FlagGet(sWisdomPairs[i].insight))
        {
            FlagSet(sWisdomPairs[i].insight);
            StringExpandPlaceholders(gStringVar4, sWisdomPairs[i].arrival);
            return TRUE;
        }
    }
    //  T-411 (the user, 2026-10-16; option A approved 2026-10-18): once every insight has arrived -- every MARK held with
    //  its ROOT -- the last note arrives the same way, on the next step, and is filed last under INSIGHT. DRAFT.
    for (i = 0; i < ARRAY_COUNT(sWisdomPairs); i++)
    {
        if (!FlagGet(sWisdomPairs[i].insight))
            return FALSE;
    }
    if (FlagGet(WISDOM_LAST_FLAG))
        return FALSE;
    FlagSet(WISDOM_LAST_FLAG);
    StringExpandPlaceholders(gStringVar4, WISDOM_LAST_ARRIVAL);
    return TRUE;
}

//  T-410 (the user, 2026-10-16; approved 2026-10-18): A MARK AND ITS ROOT, EACH SAYING HOW THE OTHER BALANCES IT. Shown
//  once both are held -- the MARK's on the USER card's BENCHMARKS page (trainer_card.c), the ROOT's in its
//  understanding's GUIDE margin (book_reader.c). SCORN's MARK pairs with none. DRAFT.
const u8 *Daemons_MarkBalance(u8 mark)
{
    u32 i;

    for (i = 0; i < ARRAY_COUNT(sWisdomPairs); i++)
    {
        if (sWisdomPairs[i].mark == FLAG_BADGE01_GET + mark)
            return (FlagGet(sWisdomPairs[i].mark) && FlagGet(sWisdomPairs[i].root)) ? sWisdomPairs[i].markBalance : NULL;
    }
    return NULL;
}

const u8 *Daemons_RootBalance(u16 root)
{
    u32 i;

    for (i = 0; i < ARRAY_COUNT(sWisdomPairs); i++)
    {
        if (sWisdomPairs[i].root == root)
            return (FlagGet(sWisdomPairs[i].mark) && FlagGet(sWisdomPairs[i].root)) ? sWisdomPairs[i].rootBalance : NULL;
    }
    return NULL;
}

//  T-412 (the user, 2026-10-16; the twenty-one approved 2026-10-18): THE DREAM LOG. After a milestone -- a MARK, an
//  understanding, a turn of the story -- the player has a dream at the first dawn after it (the watch), or on the first
//  load on a later day; it is said, and filed in the NOTEBOOK's DREAM LOG. One a night: when several wait, the next
//  waits for the next dawn, in data/dream_log.h's order. VAR_DREAM_LOG_STAMP holds when the one waiting began to wait,
//  as 2 x its day + 1 once past that day's dawn, + 1 -- so a dawn has come since when (now + 1) / 2 has moved on, and a
//  later day when now / 2 has. Days are the clock's, or play time's (an hour of play a day, its dawn the hour's last
//  quarter), as the watch's are (daemons_time.c).
#define DREAM_DAWN_CAME 0xFFFF   // DEBUG's WATCH turned to DAWN: whatever waits is dreamt on the next step
static const u8 sDreamBreak[] = _("\p");

static EWRAM_DATA bool8 sDreamLoaded = FALSE;    // a saved game has just been continued, and no step taken since

static u16 DreamNow(void)
{
    u16 day, minute;

    DaemonsDayAndMinute(&day, &minute);
    return day * 2 + (minute >= (DaemonsWatchIsFromClock() ? 5 * 60 : 45 * 24));
}

static s32 DreamWaiting(void)
{
    u32 i;

    for (i = 0; i < ARRAY_COUNT(sDreamMilestones); i++)
    {
        if (!FlagGet(sDreamMilestones[i].dreamt)
         && FlagGet(sDreamMilestones[i].needs[0]) && FlagGet(sDreamMilestones[i].needs[1]))
            return i;
    }
    return -1;
}

//  On every map load (overworld.c), beside the understandings: a milestone just reached starts its dream waiting, so
//  leaving the BENCHMARK and saving before a step still counts from the win.
void DaemonsDreamLog_Notice(void)
{
    if (VarGet(VAR_DREAM_LOG_STAMP) == 0 && DreamWaiting() >= 0)
        VarSet(VAR_DREAM_LOG_STAMP, DreamNow() + 1);
}

void DaemonsDreamLog_Loaded(void)
{
    sDreamLoaded = TRUE;
}

#if DAEMONS_DEBUG
void DaemonsDreamLog_DebugDawn(void)
{
    if (DreamWaiting() >= 0)
        VarSet(VAR_DREAM_LOG_STAMP, DREAM_DAWN_CAME);
}
#endif

//  The step hook (field_control_avatar.c), after the insights: the dream goes into gStringVar4, after its line, for
//  Daemons_EventScript_Dream to say. Its flag files it.
bool8 Daemons_TryDreamArrives(void)
{
    u16 stamp = VarGet(VAR_DREAM_LOG_STAMP), now;
    bool8 loaded = sDreamLoaded;
    s32 i = DreamWaiting();

    sDreamLoaded = FALSE;
    if (i < 0)
    {
        if (stamp != 0)
            VarSet(VAR_DREAM_LOG_STAMP, 0);
        return FALSE;
    }
    now = DreamNow();
    if (stamp != DREAM_DAWN_CAME)
    {
        if (stamp == 0 || now < stamp - 1)          // nothing was waiting yet -- or the clock was set back
        {
            VarSet(VAR_DREAM_LOG_STAMP, now + 1);
            return FALSE;
        }
        stamp--;
        if ((now + 1) / 2 <= (stamp + 1) / 2 && !(loaded && now / 2 > stamp / 2))
            return FALSE;
    }
    FlagSet(sDreamMilestones[i].dreamt);
    VarSet(VAR_DREAM_LOG_STAMP, 0);                  // the next one starts waiting from now, on the next step
    StringCopy(gStringVar4, sDreamLine);
    StringAppend(gStringVar4, sDreamBreak);
    StringAppend(gStringVar4, sDreamMilestones[i].text);
    return TRUE;
}

static EWRAM_DATA struct ListMenuItem *sItems = NULL;
static EWRAM_DATA u16 sArrowScroll = 0;
static EWRAM_DATA u16 sSectionScroll = 0;
static EWRAM_DATA u16 sSectionRow = 0;
static EWRAM_DATA u16 sEntryScroll = 0;
static EWRAM_DATA u16 sEntryRow = 0;
static EWRAM_DATA bool8 sListIsEntries = FALSE;   // T-403: which list the print and cursor hooks are drawing

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

//  T-403: an entry's note, by the flag that files it -- NULL for one the table does not know (none, by gbanotebook).
static const struct NotebookNote *FindNote(u16 flag)
{
    u32 i;

    for (i = 0; i < ARRAY_COUNT(sNotebookNotes); i++)
    {
        if (sNotebookNotes[i].flag == flag)
            return &sNotebookNotes[i];
    }
    return NULL;
}

//  T-417: sixty-four SEEN flags to a block; the second block takes seen 64 and up.
//  T-420: neither block may run into the flag after it. A `seen` past the second block is DAEMONS check_lexicon's to
//  catch (and gbanotebook.py's), since a table's contents cannot be asserted on here.
STATIC_ASSERT(FLAG_NOTEBOOK_SEEN_0 + NOTEBOOK_SEEN_BLOCK <= FLAG_NOTEBOOK_SEEN_MIGRATED, NotebookSeenBlock0Fits)
STATIC_ASSERT(FLAG_NOTEBOOK_SEEN_64 == FLAG_NOTEBOOK_SEEN_0 + 0x80 && FLAG_NOTEBOOK_SEEN_64 > FLAG_SONG_HEARD_7
           && FLAG_NOTEBOOK_SEEN_64 + NOTEBOOK_SEEN_BLOCK <= FLAG_HOLIDAY_GIFT_COMMON_JAN, NotebookSeenBlock64Fits)
static u16 SeenFlag(const struct NotebookNote *note)
{
    return note->seen < NOTEBOOK_SEEN_BLOCK ? FLAG_NOTEBOOK_SEEN_0 + note->seen
                                            : FLAG_NOTEBOOK_SEEN_64 + (note->seen - NOTEBOOK_SEEN_BLOCK);
}

static void MarkSeen(u16 flag)
{
    const struct NotebookNote *note = FindNote(flag);

    if (note != NULL)
        FlagSet(SeenFlag(note));
}

//  Unread: in the NOTEBOOK and never opened -- from the list, or full screen where it was found.
static bool8 EntryUnread(u32 i)
{
    const struct NotebookNote *note = FindNote(sEntries[i].flag);

    return note != NULL && EntryHeld(i) && !FlagGet(SeenFlag(note));
}

static bool8 SectionHasUnread(u8 section)
{
    u32 i;

    for (i = 0; i < ARRAY_COUNT(sEntries); i++)
    {
        if (sEntries[i].section == section && EntryUnread(i))
            return TRUE;
    }
    return FALSE;
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

static const u8 sText_WhichPage[] = _("Which page?");
static const u8 sText_WhereFound[] = _("{R_BUTTON} WHERE IT WAS FOUND");   // T-403, DRAFT
static const u8 sText_UnreadMark[] = _("·");

//  T-403: the line under the list (the field's message box, which says "Which page?") says what the page under the
//  cursor IS, and that R shows where it was found. BACK, or a page with no note, says "Which page?" again.
static void PrintPageNote(s32 itemIndex)
{
    const struct NotebookNote *note = NULL;

    if (itemIndex >= 0 && itemIndex != NB_CLOSE && itemIndex < (s32)ARRAY_COUNT(sEntries))
        note = FindNote(sEntries[itemIndex].flag);
    FillWindowPixelBuffer(0, PIXEL_FILL(1));
    if (note == NULL || note->about == NULL)
    {
        AddTextPrinterParameterized(0, FONT_NORMAL, sText_WhichPage, 0, 1, TEXT_SKIP_DRAW, NULL);
    }
    else
    {
        AddTextPrinterParameterized(0, FONT_NORMAL, note->about, 0, 1, TEXT_SKIP_DRAW, NULL);
        if (note->map != MAP_UNDEFINED)
            AddTextPrinterParameterized(0, FONT_NORMAL, sText_WhereFound, 0, 17, TEXT_SKIP_DRAW, NULL);
    }
    CopyWindowToVram(0, COPYWIN_GFX);
}

static void MoveCursor_Notebook(s32 itemIndex, bool8 onInit, struct ListMenu *list)
{
    if (!onInit)
        PlaySE(SE_SELECT);
    sArrowScroll = list->itemsAbove;
    if (sListIsEntries)
        PrintPageNote(itemIndex);
}

//  T-403: a dot in the right-hand gutter -- an entry not yet opened, or a section holding one.
static void PrintUnreadMark(u8 windowId, u32 itemId, u8 y)
{
    bool8 unread;

    if (itemId == NB_CLOSE || (s32)itemId == LIST_HEADER)   // T-412: a heading has no page
        return;
    unread = sListIsEntries ? EntryUnread(itemId) : SectionHasUnread(itemId);
    if (unread)
        AddTextPrinterParameterized(windowId, FONT_NORMAL, sText_UnreadMark,
                                    GetWindowAttribute(windowId, WINDOW_WIDTH) * 8 - 6, y, TEXT_SKIP_DRAW, NULL);
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
    sListIsEntries = isEntries;
    for (i = 0; i < count; i++)   // T-403: a tile of gutter for the unread dot, when there is one to draw
    {
        if (sItems[i].index != NB_CLOSE && sItems[i].index != LIST_HEADER && (isEntries ? EntryUnread(sItems[i].index) : SectionHasUnread(sItems[i].index)))
        {
            task->tWidth++;
            break;
        }
    }
    task->tHeight = task->tMaxShown * 2;
    task->tIsEntries = isEntries;

    win = SetWindowTemplateFields(0, 1, 1, task->tWidth, task->tHeight, 15, 0x038);
    task->tWindowId = AddWindow(&win);
    SetStdWindowBorderStyle(task->tWindowId, 0);

    template.items = sItems;
    template.moveCursorFunc = MoveCursor_Notebook;
    template.itemPrintFunc = PrintUnreadMark;
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

    //  T-403: R on a page that was found somewhere -- the script opens the TOWN MAP there, then comes back here.
    if (input == LIST_NOTHING_CHOSEN && gTasks[taskId].tIsEntries && JOY_NEW(R_BUTTON))
    {
        u16 scroll, row;
        const struct NotebookNote *note;
        s32 index;

        ListMenuGetScrollAndRow(gTasks[taskId].tListId, &scroll, &row);
        index = sItems[scroll + row].index;
        note = (index != NB_CLOSE) ? FindNote(sEntries[index].flag) : NULL;
        if (note != NULL && note->map != MAP_UNDEFINED)
        {
            gSpecialVar_0x8006 = index;
            gSpecialVar_Result = NB_SHOW_MAP;
            PlaySE(SE_SELECT);
            DestroyNotebookList(taskId);
            return;
        }
    }
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
    //  T-403: a save from before the marks -- everything already in it counts as read, once.
    if (!FlagGet(FLAG_NOTEBOOK_SEEN_MIGRATED))
    {
        for (i = 0; i < ARRAY_COUNT(sEntries); i++)
        {
            if (EntryHeld(i))
                MarkSeen(sEntries[i].flag);
        }
        FlagSet(FLAG_NOTEBOOK_SEEN_MIGRATED);
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
//  T-412: in the DREAM LOG, once the attendant has told one of her dreams, the list is headed in two -- YOUR DREAMS,
//  then HER DREAMS -- headings the cursor passes over. A player she never dreamt for sees it unheaded.
static bool8 HerDreamsHeld(void)
{
    u32 i;

    for (i = 0; i < ARRAY_COUNT(sEntries); i++)
    {
        if (sEntries[i].kind == NB_KIND_HER_DREAM && EntryHeld(i))
            return TRUE;
    }
    return FALSE;
}

void Notebook_ChooseEntry(void)
{
    u32 i;
    u8 n = 0;
    bool8 headed = (gSpecialVar_0x8005 == NB_DREAM_LOG && HerDreamsHeld());
    bool8 yours = FALSE, hers = FALSE;

    sItems = AllocZeroed((ARRAY_COUNT(sEntries) + 3) * sizeof(struct ListMenuItem));
    for (i = 0; i < ARRAY_COUNT(sEntries); i++)
    {
        if (sEntries[i].section == gSpecialVar_0x8005 && EntryHeld(i))
        {
            if (headed && sEntries[i].kind == NB_KIND_TEXT && !yours)
            {
                sItems[n].label = sDreamHeading_Yours;
                sItems[n].index = LIST_HEADER;
                n++;
                yours = TRUE;
            }
            if (headed && sEntries[i].kind == NB_KIND_HER_DREAM && !hers)
            {
                sItems[n].label = sDreamHeading_Hers;
                sItems[n].index = LIST_HEADER;
                n++;
                hers = TRUE;
            }
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
    if (sItems[sEntryScroll + sEntryRow].index == LIST_HEADER)   // T-412: never open on a heading
    {
        if (sEntryRow + 1 < NB_MAX_SHOWN && sEntryRow + 1 < n)
            sEntryRow++;
        else
            sEntryScroll++;
    }
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
    else if (sEntries[gSpecialVar_0x8006].kind == NB_KIND_SONG)
        Play_WriteSongPage(sEntries[gSpecialVar_0x8006].arg, gStringVar4);   // T-417
    else if (sEntries[gSpecialVar_0x8006].kind == NB_KIND_HER_DREAM)          // T-412: her dream, and where it led
    {
        const u8 *led = DaemonsDream_Led(sEntries[gSpecialVar_0x8006].arg);
        u8 *end = StringExpandPlaceholders(gStringVar4, sEntries[gSpecialVar_0x8006].text);

        if (led != NULL)
        {
            *end++ = CHAR_PROMPT_CLEAR;
            StringExpandPlaceholders(end, led);
        }
    }
    else
        StringExpandPlaceholders(gStringVar4, sEntries[gSpecialVar_0x8006].text);
}

//  T-223: VAR_0x8006 is the entry; it opens full screen on the NOTEBOOK's ruled sheet. The script fades first
//  and waits, and comes back to the list of pages.
//  T-224: a document found in the world opens where it is found, by its flag (VAR_0x8004), full screen as the
//  notebook would show it -- so reading one and filing it are the same page. tools/gbadocs.py calls this.
//  T-315, T-402: a page that came from a blog post, a book or a song ends on a QR code to it, on a page of its own
//  after the text (docs/school.md 10 names each page's song). Songs not yet found public have no row.
static const struct { u16 flag; u8 qr; } sPageQr[] = {
    { FLAG_NOTEBOOK_LOOSE_FOLDS,            BOOK_QR_FOLDS },          // The Folds of Awareness (Seeing Sharp)
    { FLAG_NOTEBOOK_DOC_LOOSE_PAGES_4,      BOOK_QR_CRYSTAL_CLEAR },  // ERRANDS
    { FLAG_NOTEBOOK_DOC_LAB_NOTES_1,        BOOK_QR_ECHOES },         // FIRST SESSION
    { FLAG_NOTEBOOK_DOC_LAB_NOTES_6,        BOOK_QR_ECHOES },         // CIRCULATED
    { FLAG_NOTEBOOK_RUN_MAR4,               BOOK_QR_SLUMBERING },     // RUN LOGS 1
    { FLAG_NOTEBOOK_RUN_FATAL,              BOOK_QR_FATAL_ERROR },    // RUN LOGS 4
    { FLAG_NOTEBOOK_DOC_RUN_LOGS_5,         BOOK_QR_FATAL_ERROR },    // RECLASSIFIED
    { FLAG_NOTEBOOK_DOC_RUN_LOGS_6,         BOOK_QR_FATAL_ERROR },    // 1002
    { FLAG_NOTEBOOK_DOC_CORRESPONDENCE_1,   BOOK_QR_TYS_DILEMMA },    // COME BACK
    { FLAG_NOTEBOOK_DOC_CORRESPONDENCE_2,   BOOK_QR_LINES_IN_SAND },  // THE POOL
    { FLAG_NOTEBOOK_DOC_CORRESPONDENCE_3,   BOOK_QR_FIT_FOR_WORK },   // FIT FOR WORK
    { FLAG_NOTEBOOK_DOC_CORRESPONDENCE_4,   BOOK_QR_LINES_IN_SAND },  // CARBON COPY
    { FLAG_NOTEBOOK_DOC_THE_FILE_4,         BOOK_QR_NINE_SCARS },     // NINE ITEMS
    { FLAG_NOTEBOOK_DOC_PROSPECTUS_1,       BOOK_QR_SCORN_SOLUTION }, // METHODOLOGY
    { FLAG_NOTEBOOK_PROSPECTUS_SCORES,      BOOK_QR_SCORN_SOLUTION }, // REVIEW SCORES
    { FLAG_NOTEBOOK_DOC_PROSPECTUS_3,       BOOK_QR_SCORN_BLUES },    // SIGNAGE MEMO
    //  T-402, the rest (the user's links, 2026-10-15)
    { FLAG_NOTEBOOK_LOOSE_STONE,            BOOK_QR_POLY_FIELDS },    // THE STONE
    { FLAG_NOTEBOOK_DOC_LOOSE_PAGES_2,      BOOK_QR_POLY_FIELDS },    // HOMEWORK
    { FLAG_NOTEBOOK_LOOSE_FIELDS,           BOOK_QR_POLY_FIELDS },    // FIELDS
    { FLAG_NOTEBOOK_LOOSE_UNSENT,           BOOK_QR_LOVE_PERSISTS },  // UNSENT
    { FLAG_NOTEBOOK_LAB_OBSERVER,           BOOK_QR_QUANTUM_TRANSLATIONS }, // THE OBSERVER
    { FLAG_NOTEBOOK_LAB_BOX,                BOOK_QR_QUANTUM_TRANSLATIONS }, // THE BOX
    { FLAG_NOTEBOOK_DOC_LAB_NOTES_4,        BOOK_QR_QUANTUM_TRANSLATIONS }, // STRUCK OUT
    { FLAG_NOTEBOOK_DOC_PEER_REVIEW_4,      BOOK_QR_QUANTUM_TRANSLATIONS }, // READ AGAIN
    { FLAG_NOTEBOOK_DOC_CORRESPONDENCE_5,   BOOK_QR_TYS_REDEMPTION }, // NO ADDRESS (Part 1)
    { FLAG_NOTEBOOK_DOC_CORRESPONDENCE_6,   BOOK_QR_STARRS_REVELATION }, // FOLDED NOTE
    { FLAG_NOTEBOOK_DOC_THE_FILE_5,         BOOK_QR_DESPERATE_SHADOWS }, // COUNSEL MEMO
    { FLAG_NOTEBOOK_DOC_PROSPECTUS_5,       BOOK_QR_BETRAYALS_STING },   // HARDWARE
    { FLAG_NOTEBOOK_DOC_PROSPECTUS_6,       BOOK_QR_EMPIRE_OF_SCORN },   // THE PLAN
    { FLAG_NOTEBOOK_REVIEW_POSITION,        BOOK_QR_OWL_AND_CODE },   // POSITION PAPER
    { FLAG_NOTEBOOK_REVIEW_DRAFT,           BOOK_QR_OWL_AND_CODE },   // SECOND DRAFT
    { FLAG_NOTEBOOK_LOOSE_KEY,              BOOK_QR_PAINTED_MIRROR }, // THE BAND'S SHEET (The Painted Mirror, episode 4)
};

static void AddPageQr(u16 flag)
{
    u16 i;

    for (i = 0; i < ARRAY_COUNT(sPageQr); i++)
    {
        if (sPageQr[i].flag == flag)
        {
            BookReader_AddQr(sPageQr[i].qr);
            return;
        }
    }
}

void Notebook_ReadFlagEntry(void)
{
    u16 i;

    for (i = 0; i < ARRAY_COUNT(sEntries); i++)
    {
        if (sEntries[i].flag == gSpecialVar_0x8004 && sEntries[i].kind == NB_KIND_TEXT)
        {
            StringExpandPlaceholders(gStringVar4, sEntries[i].text);   // {RIVAL} in a letter is the name the player gave
            BookReader_OpenNotebookPage(sEntries[i].title, gStringVar4);
            AddPageQr(sEntries[i].flag);
            MarkSeen(sEntries[i].flag);   // T-403: read where it was found is read
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
    if (sEntries[gSpecialVar_0x8006].kind == NB_KIND_TEXT && sEntries[gSpecialVar_0x8006].arg == 1)
    {
        while (*text != EOS && *text != CHAR_PROMPT_CLEAR)
            text++;
        if (*text == CHAR_PROMPT_CLEAR)
            text++;
    }
    BookReader_OpenNotebookPage(sEntries[gSpecialVar_0x8006].title, text);
    if (sEntries[gSpecialVar_0x8006].kind == NB_KIND_SONG)
        BookReader_AddSong(sEntries[gSpecialVar_0x8006].arg);   // T-417: its notes, on a staff under the words
    AddPageQr(sEntries[gSpecialVar_0x8006].flag);
    MarkSeen(sEntries[gSpecialVar_0x8006].flag);   // T-403
}

//  T-403: VAR_0x8006 is the entry; the TOWN MAP opens with its cursor on the place the page was found, and the
//  script carries on (back to the list) when it closes.
void Notebook_ShowOnMap(void)
{
    const struct NotebookNote *note = NULL;
    u8 mapsec = MAPSEC_NONE;

    if (gSpecialVar_0x8006 < ARRAY_COUNT(sEntries))
        note = FindNote(sEntries[gSpecialVar_0x8006].flag);
    if (note != NULL && note->map != MAP_UNDEFINED)
        mapsec = Overworld_GetMapHeaderByGroupAndId(note->map >> 8, note->map & 0xFF)->regionMapSectionId;
    QuestLog_CutRecording();
    InitRegionMapFocusedWithExitCB(REGIONMAP_TYPE_WALL, mapsec, CB2_ReturnToFieldContinueScriptPlayMapMusic);
}
