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
#include "book_reader.h"
#include "notebook.h"   // T-410: the ROOT's balance
#include "play.h"   // T-417: a SONGBOOK page's notes
#include "daemons_hermetic.h"   // T-483 (7): the card of the day
#include "daemons_pulse.h"   // T-483: THE HERMETIC LAYER breathes
#include "daemons_spellbook.h"   // T-483 (5): the folds' spells come off the page
#include "daemons_glitch.h"  // T-486: and glitches, once
#include "daemons_spirits.h"  // T-519 item 2: the dawn half's observer line
#include "daemons_unity.h"    // T-519 item 3: the torn page, and the mended book
#include "fieldmap.h"         // T-529: the halves' tones, for the mended book's lens
#include "constants/songs.h"
#include "data.h"         // T-509: the cards' daemons
#include "decompress.h"
#include "pokemon.h"
#include "random.h"

//  T-223: THE NOTEBOOK AND THE TEXTBOOK, READ LIKE BOOKS (docs/school.md 9, 6).
//
//  The first build showed an entry in the ordinary message box, and the user played it and said exactly what
//  was wrong: two lines at a time, and no idea how many pages are left. A sign may do that. A document may not
//  -- the ending (4.24) is about how a document was READ, and the one screen that holds documents was the one
//  screen that would not let you see one whole.
//
//  So both books open full screen, and the text is REFLOWED to the page rather than shown in the two-line
//  breaks it was written in: \n is a space, \p is a new paragraph, and the words are wrapped to the width.
//  Nothing is rewritten; the same bytes are laid out on a bigger surface.
//
//  THE NOTEBOOK is one ruled sheet on a spiral, in CONTEXT's hue: the title in the top margin, then the text on
//  the rules, and a page count only when there is more than one page. A turns on, B closes.
//
//  THE TEXTBOOK is an open book in CONTENT's hue, three levels deep, and B walks back up them one at a time:
//      CONTENTS  the seven floors as chapters, four on the left page and three on the right. The D-pad moves
//                in all four directions, as it would across a real contents page. A floor not yet stood on is
//                printed faintly and will not open -- the rule the batch-5 boards kept.
//      CHAPTER   the chapter's five topics on the left, its board's chalk mark drawn large on the right.
//                LEFT and RIGHT turn to the neighbouring chapter.
//      TOPIC     the topic's text across the spread, left page then right. LEFT and RIGHT turn to the
//                neighbouring topic.
//  Every chapter carries its floor's chalk mark (tools/genbookicons.py), so the book agrees with the building.
//
//  One window covers the screen and everything is drawn into it -- paper, rules, rings, pages, marks, text. The
//  text is printed with a transparent background, so the rules show through it as they would through ink.
//  Everything lives on the HEAP; EWRAM has under a kilobyte free.

enum { BOOK_NOTEBOOK, BOOK_TEXTBOOK, BOOK_GUIDE, BOOK_SUBTLE, BOOK_HERMETIC, BOOK_EUDAIMON, BOOK_UNITY,   // T-423: THE SUBTLE LAYER; T-483; T-519
       BOOK_ORIGIN };   // T-533: THE ORIGIN OF A DAEMON
enum { VIEW_PAGE, VIEW_CONTENTS, VIEW_CHAPTER, VIEW_TOPIC, VIEW_COVER, VIEW_GUIDE_CONTENTS, VIEW_GUIDE_ENTRY,
       VIEW_SUBTLE_CONTENTS, VIEW_SUBTLE_ENTRY, VIEW_THIRTEEN,     // T-483 (7): Halloween's card
       VIEW_CARDS, VIEW_CARDS_SELECT, VIEW_CARDS_SPREAD, VIEW_CARDS_BOARD, VIEW_CARDS_READ };   // T-509

#define WIN_BOOK        0
#define BR_MAX_LINES    96      // T-300: a Guide entry runs to three spreads
#define BR_TEXT_SIZE    1000
#define BR_LINES_SIZE   2200
#define BR_FONT         FONT_NORMAL
#define BR_SMALL        FONT_SMALL
#define BR_WIN_BYTES    (30 * 20 * TILE_SIZE_4BPP)

//  The notebook sheet
#define NB_PAPER_X      12
#define NB_MARGIN_X     30
#define NB_TEXT_X       34
#define NB_TEXT_W       196
#define NB_TOP          20
#define NB_PITCH        13
#define NB_LINES        10
#define SB_MIDDLE_C     112     // T-417: the SONGBOOK's staff -- step 0's y; a line every three pixels, E4 to F5

//  The textbook spread
#define TB_LEFT_X       8
#define TB_RIGHT_X      122
#define TB_PAGE_W       110
#define TB_TEXT_INSET   8
#define TB_TEXT_W       94
#define TB_PITCH        13
#define TB_FOLIO_Y      139
#define TB_CHAPTERS     7
#define TB_TOPICS       5

//  Palette indices, shared by both books; each book loads its own colours into them.
enum
{
    C_NONE, C_PAPER, C_INK, C_SHADOW, C_FAINT, C_RULE, C_MARGIN, C_EDGE,
    C_EDGE_DARK, C_HIGHLIGHT, C_RING, C_FRAME, C_SLATE, C_CHALK, C_CHALK_FAINT, C_COVER,
};

//  T-483: THE LAYER BOOKS. THE SUBTLE LAYER and THE HERMETIC LAYER are one kind of book -- a cover, a contents spread,
//  then each entry's picture on the left and its text on the right -- written by one tool, so the reader keeps one
//  set of views for both and asks which book's tables to read. (T-533: declared here, ahead of the reader, which
//  holds one -- THE ORIGIN OF A DAEMON as far as it has come back. The three types are the generated headers'.)
struct SubtleSection;
struct SubtleEntry;
struct SubtlePicture;
struct LayerBook
{
    const struct SubtleSection *sections;
    const struct SubtleEntry *entries;
    const struct SubtlePicture *pictures;
    const u32 *cover;
    u8 sectionCount;
    u8 entryCount;
};

struct BookReader
{
    u8 initState;
    u8 book;
    u8 view;
    u8 leaving;
    u8 page;
    u8 pageCount;
    u8 chapter;             // 1..7
    u8 cursor;              // CONTENTS: 0..6.  CHAPTER: 0..4.
    u8 topic;               // 0..4
    u8 lineCount;
    u8 section;             // GUIDE: the section the contents page is on
    u8 entry;               // GUIDE: the entry, counted through the whole book
    u8 spread;              // GUIDE: which spread of the entry
    u8 spreadCount;
    u8 bodyTop;             // GUIDE: where the first page starts its text, under the title
    bool8 focusRight;       // GUIDE contents: the cursor is in the chapters, not the sections
    bool8 torn;             // GUIDE: the REVIEW BOARD's copy (T-311)
    u8 qr;                  // T-315: a QR code on the last page (BOOK_QR_*), or none
    u8 song;                // T-417: a SONGBOOK page -- its song (PLAY_SONG_*) plus one, or 0
    u8 *blank;              // the empty sheet or spread, drawn once: filling it per pixel costs a quarter second
    u8 *blankPage;          // GUIDE: its reading page, which is not the contents' spread, so it is kept apart
    u16 lineStart[BR_MAX_LINES];
    u8 linePara[BR_MAX_LINES];  // T-423: each line's paragraph, for the asking voice
    bool8 voices;               // T-423: an entry with two voices, whose paragraphs are not indented
    u8 picEntry, picFrame, picTimer;   // T-423: the open picture, and which of its frames
    bool8 cardsFocus;                  // T-509: the cover's cursor is on the deck tile
    struct Cards *cards;               // T-509: the CARDS section, while it is open
    struct UnityLens *lens;            // T-529: THE UNITY BOOK's lens, while it is open
    u8 lensEntry, lensS, lensJ;        // T-529: the place kept across a turn of the lens (lensEntry 0xFF: look again)
    struct LayerBook origin;           // T-533: THE ORIGIN OF A DAEMON, cut after the last section to have come back
    u8 title[40];
    u8 text[BR_TEXT_SIZE];
    u8 lines[BR_LINES_SIZE];
};

static EWRAM_DATA struct BookReader *sBook = NULL;

static const struct BgTemplate sBgTemplates[] =
{
    { .bg = 0, .charBaseIndex = 0, .mapBaseIndex = 31, .screenSize = 0, .paletteMode = 0, .priority = 0, .baseTile = 0 },
    // T-509: the CARDS section's two layers, hidden until it opens -- the placed cards, and the spread
    { .bg = 1, .charBaseIndex = 2, .mapBaseIndex = 30, .screenSize = 0, .paletteMode = 0, .priority = 1, .baseTile = 0 },
    { .bg = 2, .charBaseIndex = 0, .mapBaseIndex = 29, .screenSize = 0, .paletteMode = 0, .priority = 1, .baseTile = 0 },
};

static const struct WindowTemplate sWindowTemplates[] =
{
    { .bg = 0, .tilemapLeft = 0, .tilemapTop = 0, .width = 30, .height = 20, .paletteNum = 15, .baseBlock = 1 },
    DUMMY_WIN_TEMPLATE
};

//  CONTEXT's hue (PSYCHIC, 176 86 158) on the cover; cream paper, blue rules, a red margin, pen ink.
static const u16 sNotebookPal[16] =
{
    [C_NONE]        = RGB(0, 0, 0),
    [C_PAPER]       = RGB(31, 30, 27),
    [C_INK]         = RGB(4, 5, 10),
    [C_SHADOW]      = RGB(25, 25, 23),
    [C_FAINT]       = RGB(18, 18, 17),
    [C_RULE]        = RGB(21, 25, 29),
    [C_MARGIN]      = RGB(28, 15, 15),
    [C_EDGE]        = RGB(27, 26, 23),
    [C_EDGE_DARK]   = RGB(22, 21, 19),
    [C_HIGHLIGHT]   = RGB(31, 29, 20),
    [C_RING]        = RGB(22, 22, 24),
    [C_FRAME]       = RGB(10, 10, 12),
    [C_SLATE]       = RGB(4, 6, 5),
    [C_CHALK]       = RGB(29, 28, 27),
    [C_CHALK_FAINT] = RGB(16, 17, 16),
    [C_COVER]       = RGB(17, 8, 15),
};

//  CONTENT's hue (NORMAL, 198 188 168) on the cover; white pages with a little age in them, and the boards'
//  own slate and chalk for the chapter marks.
static const u16 sTextbookPal[16] =
{
    [C_NONE]        = RGB(0, 0, 0),
    [C_PAPER]       = RGB(31, 31, 29),
    [C_INK]         = RGB(5, 5, 6),
    [C_SHADOW]      = RGB(25, 25, 23),
    [C_FAINT]       = RGB(18, 18, 17),
    [C_RULE]        = RGB(22, 21, 19),
    [C_MARGIN]      = RGB(15, 13, 10),
    [C_EDGE]        = RGB(27, 26, 24),
    [C_EDGE_DARK]   = RGB(22, 21, 19),
    [C_HIGHLIGHT]   = RGB(30, 28, 20),
    [C_RING]        = RGB(22, 22, 24),
    [C_FRAME]       = RGB(18, 13, 7),
    [C_SLATE]       = RGB(4, 6, 5),
    [C_CHALK]       = RGB(29, 28, 27),
    [C_CHALK_FAINT] = RGB(16, 17, 16),
    [C_COVER]       = RGB(15, 14, 12),
};

static const u8 sInk[3]    = { C_NONE, C_INK, C_SHADOW };
static const u8 sFaint[3]  = { C_NONE, C_FAINT, C_NONE };
static const u8 sPen[3]    = { C_NONE, C_MARGIN, C_NONE };   // T-304: a note in the margin, in the Guide's cyan

static const u32 sChapterMarks[] = INCBIN_U32("graphics/book/chapter_icons.4bpp");

//  ---- the textbook's matter: seven chapters, the floors, and each floor's board in five topics
static const u8 sChapter1[] = _("LANGUAGE");
static const u8 sChapter2[] = _("ATTENTION");
static const u8 sChapter3[] = _("MEMORY");
static const u8 sChapter4[] = _("CATEGORIES");
static const u8 sChapter5[] = _("LEARNING");
static const u8 sChapter6[] = _("BIAS");
static const u8 sChapter7[] = _("ERROR");
static const u8 *const sChapterNames[TB_CHAPTERS + 1] =
{
    NULL, sChapter1, sChapter2, sChapter3, sChapter4, sChapter5, sChapter6, sChapter7,
};

static const u16 sChapterFlags[TB_CHAPTERS + 1] =
{
    0, FLAG_SCHOOL_VISITED_1F, FLAG_SCHOOL_VISITED_2F, FLAG_SCHOOL_VISITED_3F, FLAG_SCHOOL_VISITED_4F,
    FLAG_SCHOOL_VISITED_5F, FLAG_SCHOOL_VISITED_6F, FLAG_SCHOOL_VISITED_7F,
};

//  DRAFT WORDING (T-223): the contents page's heading, and the floor label under each chapter.
static const u8 sText_Contents[] = _("CONTENTS");
static const u8 sText_Floor[]    = _("F");

//  The board is the building's own text, not a copy of it: what the player read on the wall is what the book
//  prints. Each topic opens with its own name and a \p, and the book takes the name from there.
extern const u8 ViridianCity_School_Text_Names[];
extern const u8 ViridianCity_School_Text_Pieces[];
extern const u8 ViridianCity_School_Text_Order[];
extern const u8 ViridianCity_School_Text_Ambiguity[];
extern const u8 ViridianCity_School_Text_Spelling_Board[];
extern const u8 ViridianCity_School_2F_Text_Limits[];
extern const u8 ViridianCity_School_2F_Text_Cues[];
extern const u8 ViridianCity_School_2F_Text_Search[];
extern const u8 ViridianCity_School_2F_Text_Switching[];
extern const u8 ViridianCity_School_2F_Text_Missed[];
extern const u8 ViridianCity_School_3F_Text_Storing[];
extern const u8 ViridianCity_School_3F_Text_Fading[];
extern const u8 ViridianCity_School_3F_Text_Crowding[];
extern const u8 ViridianCity_School_3F_Text_Recall[];
extern const u8 ViridianCity_School_3F_Text_Spacing[];
extern const u8 ViridianCity_School_4F_Text_Middles[];
extern const u8 ViridianCity_School_4F_Text_Edges[];
extern const u8 ViridianCity_School_4F_Text_Features[];
extern const u8 ViridianCity_School_4F_Text_Levels[];
extern const u8 ViridianCity_School_4F_Text_Other[];
extern const u8 ViridianCity_School_5F_Text_Connections[];
extern const u8 ViridianCity_School_5F_Text_Practice[];
extern const u8 ViridianCity_School_5F_Text_Threshold[];
extern const u8 ViridianCity_School_5F_Text_Feedback[];
extern const u8 ViridianCity_School_5F_Text_Pruning[];
extern const u8 ViridianCity_School_6F_Text_Framing[];
extern const u8 ViridianCity_School_6F_Text_Anchors[];
extern const u8 ViridianCity_School_6F_Text_Confirming[];
extern const u8 ViridianCity_School_6F_Text_Hindsight[];
extern const u8 ViridianCity_School_6F_Text_Fallacies[];
extern const u8 ViridianCity_School_7F_Text_Surprise[];
extern const u8 ViridianCity_School_7F_Text_Correction[];
extern const u8 ViridianCity_School_7F_Text_Steps[];
extern const u8 ViridianCity_School_7F_Text_Mistakes[];
extern const u8 ViridianCity_School_7F_Text_Certainty[];

static const u8 *const sTopics[TB_CHAPTERS + 1][TB_TOPICS] =
{
    [1] = { ViridianCity_School_Text_Names, ViridianCity_School_Text_Pieces, ViridianCity_School_Text_Order, ViridianCity_School_Text_Ambiguity, ViridianCity_School_Text_Spelling_Board },
    [2] = { ViridianCity_School_2F_Text_Limits, ViridianCity_School_2F_Text_Cues, ViridianCity_School_2F_Text_Search, ViridianCity_School_2F_Text_Switching, ViridianCity_School_2F_Text_Missed },
    [3] = { ViridianCity_School_3F_Text_Storing, ViridianCity_School_3F_Text_Fading, ViridianCity_School_3F_Text_Crowding, ViridianCity_School_3F_Text_Recall, ViridianCity_School_3F_Text_Spacing },
    [4] = { ViridianCity_School_4F_Text_Middles, ViridianCity_School_4F_Text_Edges, ViridianCity_School_4F_Text_Features, ViridianCity_School_4F_Text_Levels, ViridianCity_School_4F_Text_Other },
    [5] = { ViridianCity_School_5F_Text_Connections, ViridianCity_School_5F_Text_Practice, ViridianCity_School_5F_Text_Threshold, ViridianCity_School_5F_Text_Feedback, ViridianCity_School_5F_Text_Pruning },
    [6] = { ViridianCity_School_6F_Text_Framing, ViridianCity_School_6F_Text_Anchors, ViridianCity_School_6F_Text_Confirming, ViridianCity_School_6F_Text_Hindsight, ViridianCity_School_6F_Text_Fallacies },
    [7] = { ViridianCity_School_7F_Text_Surprise, ViridianCity_School_7F_Text_Correction, ViridianCity_School_7F_Text_Steps, ViridianCity_School_7F_Text_Mistakes, ViridianCity_School_7F_Text_Certainty },
};

//  ---- T-300: THE PROGRAMMER'S GUIDE TO THE HUMAN MIND. Its text, its palette and its cover are all written by DAEMONS
//  tools/genguide.py -- the words from docs/guide.md (DRAFT until the user approves them), the sixteen colours as this
//  file's palette roles, and the cover from the PDF's own. Its pages are the cover's charcoal, its ink near-white.
struct GuideSection { const u8 *label; const u8 *heading; u8 first; u8 count; };
struct GuideEntry { const u8 *shortTitle; const u8 *title; const u8 *text; };
#include "data/guide.h"
#include "data/subtle_layer.h"   // T-423: tools/gensubtle.py
#include "data/hermetic_layer.h" // T-483: the same tool, the same types
#include "data/eudaimon_layer.h" // T-519 item 2: the torn book's dawn half, the same again
#include "data/unity_layer.h"    // T-519 item 3: and the two mended
#include "data/origin_book.h"    // T-533: THE ORIGIN OF A DAEMON, the same tool and types
#include "data/arcana_cards.h"   // T-509: DAEMONS tools/port_arcana.py

//  T-483: the layer books' tables (struct LayerBook is declared above the reader).
static const struct LayerBook sLayerBooks[4] =
{
    { sSubtleSections, sSubtleEntries, sSubtlePictures, sSubtleCover, SUBTLE_SECTIONS, SUBTLE_ENTRIES },
    { sHermeticSections, sHermeticEntries, sHermeticPictures, sHermeticCover, HERMETIC_SECTIONS, HERMETIC_ENTRIES },
    { sEudaimonSections, sEudaimonEntries, sEudaimonPictures, sEudaimonCover, EUDAIMON_SECTIONS, EUDAIMON_ENTRIES },
    { sUnitySections, sUnityEntries, sUnityPictures, sUnityCover, UNITY_SECTIONS, UNITY_ENTRIES },
};

//  T-533: THE ORIGIN OF A DAEMON whole -- never read as it is: Origin_Open copies it into the reader cut after the
//  last section that has come back, and Layer() reads only that copy. Its sections are in order and their entries
//  contiguous (gensubtle writes them so), which is what lets a cut be two counts.
static const struct LayerBook sOriginWhole =
    { sOriginSections, sOriginEntries, sOriginPictures, sOriginCover, ORIGIN_SECTIONS, ORIGIN_ENTRIES };

//  T-529 (the user, 2026-11-07, by multiple choice; vision 4.40, DAEMONS docs/TODO.md T-529): THE UNITY BOOK'S LENS.
//  Once the book is mended the reader holds only THE UNITY BOOK, and every page in it turns IN PLACE between three
//  readings: NIGHT, as the night half words it (THE HERMETIC LAYER), in the emerald sepia; WHOLE, the mended book's
//  own words, in full colour; and DAWN, as the dawn half words it (THE EUDAIMON LAYER), in the sapphire cyanotype --
//  the tints HALFTONE wears for each half (fieldmap.c DaemonsBookTone). L and R turn it, a switch of three settings
//  rather than a wheel (L toward the night, R toward the dawn); in this book the D-pad and A turn the pages, as in
//  every other. The rules are the companion's (C-304, server/src/unity.ts and app/screens/shared/Unity.tsx):
//    PAIRING  a section of the mended book whose label the night half does not have (BALANCE, written at the crease)
//             has no other reading; the other ten pair in order with each half's ten, so THE FOLDS meets the dawn
//             half's THE UNFOLDING.
//    PLACE    the page kept is a section and a place in it, clamped where the other reading's section is shorter;
//             turning the lens back returns to the same entry, unless a page was turned in between.
//    TONE     each page takes the tone of the reading it shows, with the half's own palette (its pictures were drawn
//             in it), so BALANCE stays in full colour with NIGHT or DAWN set: the tone always says which side is read.
//  The setting is kept for the session (sUnityLens, one byte of IWRAM), not in the save. The reading is built as a
//  book of its own on the heap -- the eleven sections, each from the book it is read from, and their entries and
//  pictures -- so every view the reader already has reads it unchanged. Every word DRAFT.
enum { LENS_WHOLE, LENS_NIGHT, LENS_DAWN };
#define LENS_MAX_ENTRIES 64
#define LAYER_HERMETIC   1      // sLayerBooks' order
#define LAYER_EUDAIMON   2
#define LAYER_UNITY      3

struct UnityLens
{
    struct LayerBook book;
    struct SubtleSection sections[UNITY_SECTIONS];
    struct SubtleEntry entries[LENS_MAX_ENTRIES];
    struct SubtlePicture pictures[LENS_MAX_ENTRIES];
    u8 from[UNITY_SECTIONS];            // which of sLayerBooks each section is read from
    u8 origin[LENS_MAX_ENTRIES];        // each entry's place in its own book
};

static u8 sUnityLens;                   // the session's setting: LENS_WHOLE until L or R turns it

static const u8 sText_LensNight[] = _("NIGHT");   // DRAFT
static const u8 sText_LensWhole[] = _("WHOLE");   // DRAFT
static const u8 sText_LensDawn[]  = _("DAWN");    // DRAFT
static const u8 *const sLensNames[] = { [LENS_WHOLE] = sText_LensWhole, [LENS_NIGHT] = sText_LensNight, [LENS_DAWN] = sText_LensDawn };

static bool8 LensOn(void)
{
    return sBook->book == BOOK_UNITY && sBook->lens != NULL && sUnityLens != LENS_WHOLE;
}

//  BALANCE: a section of the mended book the night half has no label for
static bool8 LensWholeOnly(const u8 *label)
{
    u8 k;

    for (k = 0; k < sLayerBooks[LAYER_HERMETIC].sectionCount; k++)
        if (StringCompare(sLayerBooks[LAYER_HERMETIC].sections[k].label, label) == 0)
            return FALSE;
    return TRUE;
}

static void LensBuild(void)
{
    struct UnityLens *l = sBook->lens;
    const struct LayerBook *whole = &sLayerBooks[LAYER_UNITY], *from;
    const struct SubtleSection *src;
    u8 halfIndex = sUnityLens == LENS_NIGHT ? LAYER_HERMETIC : LAYER_EUDAIMON;
    const struct LayerBook *half = &sLayerBooks[halfIndex];
    u8 s, j, k = 0, n = 0;

    for (s = 0; s < whole->sectionCount; s++)
    {
        if (!LensWholeOnly(whole->sections[s].label) && k < half->sectionCount)
        {
            from = half;
            src = &half->sections[k++];
            l->from[s] = halfIndex;
        }
        else
        {
            from = whole;
            src = &whole->sections[s];
            l->from[s] = LAYER_UNITY;
        }
        l->sections[s].label = src->label;
        l->sections[s].heading = src->heading;
        l->sections[s].first = n;
        for (j = 0; j < src->count && n < LENS_MAX_ENTRIES; j++, n++)
        {
            l->entries[n] = from->entries[src->first + j];
            l->pictures[n] = from->pictures[l->entries[n].picture];
            l->entries[n].picture = n;
            l->origin[n] = src->first + j;
        }
        l->sections[s].count = n - l->sections[s].first;
    }
    l->book.sections = l->sections;
    l->book.entries = l->entries;
    l->book.pictures = l->pictures;
    l->book.cover = whole->cover;
    l->book.sectionCount = whole->sectionCount;
    l->book.entryCount = n;
}

static const u16 sGuidePal[] = INCBIN_U16("graphics/book/guide_cover.gbapal");
static const u32 sGuideCover[] = INCBIN_U32("graphics/book/guide_cover.4bpp");

//  DRAFT WORDING (T-300): the Guide's contents heading, and the spread counter's separator.
static const u8 sText_GuideContents[] = _("CONTENTS");

static void Task_BookInit(u8 taskId);
static void Task_BookInput(u8 taskId);
static bool8 IsLayerBook(void);
static void Input_SubtleEntryTurn(void);

#include "data/qr_codes.h"

//  ---------------------------------------------------------------- drawing primitives

static void Rect(u8 color, s16 x, s16 y, s16 w, s16 h)
{
    if (w > 0 && h > 0)
        FillWindowPixelRect(WIN_BOOK, PIXEL_FILL(color), x, y, w, h);
}

//  T-315: A QR CODE, centred on (cx, cy): each module a 2x2 block, runs along a row filled at once, on a quiet zone of
//  four modules in the page's light colour. The codes are tools/gbaqr.py's; nothing on the page says what one is.
#define QR_MODULE 2
#define QR_QUIET  4

static void DrawQr(const u8 *qr, s16 cx, s16 cy, u8 light, u8 dark)
{
    u8 n = qr[0], rowBytes = (qr[0] + 7) / 8, r, c, run;
    //  x0 is kept EVEN: the 4-bit fill ORs the half-byte at an odd edge into what is there, so a run starting on an
    //  odd pixel came back one pixel wider in the shadow colour (1 | 2 = 3), and the code would not scan.
    s16 side = n * QR_MODULE, x0 = (cx - side / 2) & ~1, y0 = cy - side / 2, pad = QR_QUIET * QR_MODULE;
    const u8 *rows = qr + 1;

    FillWindowPixelRect(WIN_BOOK, PIXEL_FILL(light), x0 - pad, y0 - pad, side + 2 * pad, side + 2 * pad);
    for (r = 0; r < n; r++)
    {
        for (c = 0; c < n; c += run)
        {
            for (run = 0; c + run < n && (rows[r * rowBytes + (c + run) / 8] & (0x80 >> ((c + run) % 8))); run++)
                ;
            if (run)
                FillWindowPixelRect(WIN_BOOK, PIXEL_FILL(dark), x0 + c * QR_MODULE, y0 + r * QR_MODULE, run * QR_MODULE, QR_MODULE);
            else
                run = 1;
        }
    }
}

static void Print(u8 font, const u8 *colors, s16 x, s16 y, const u8 *str)
{
    AddTextPrinterParameterized3(WIN_BOOK, font, x, y, colors, TEXT_SKIP_DRAW, str);
}

static s16 Width(u8 font, const u8 *str)
{
    return GetStringWidth(font, str, 0);
}

static u8 MarkPixel(u8 chapter, u8 x, u8 y)
{
    const u8 *tiles = (const u8 *)sChapterMarks;
    u32 py = (chapter - 1) * 16 + y;
    u8 b = tiles[((py / 8) * 2 + x / 8) * 32 + (py % 8) * 4 + (x % 8) / 2];

    return (x & 1) ? (b >> 4) : (b & 0xF);
}

//  A chapter's chalk mark, at any whole-number scale. A chapter not yet attended is a ghost of its board.
static void DrawMark(u8 chapter, s16 x, s16 y, u8 scale, bool8 ghost)
{
    u8 px, py, c;

    for (py = 0; py < 16; py++)
    {
        for (px = 0; px < 16; px++)
        {
            c = MarkPixel(chapter, px, py);
            if (c == C_NONE)
                continue;
            if (ghost)
                c = (c == C_FRAME) ? C_EDGE_DARK : (c == C_SLATE ? C_EDGE : C_PAPER);
            Rect(c, x + px * scale, y + py * scale, scale, scale);
        }
    }
}

//  ---------------------------------------------------------------- reflow

//  Lays SRC out in lines no wider than WIDTH, into sBook->lines. A line break or a
//  scroll is a space; a new page (\p) starts a new line.
//  An ext-ctrl code rides along inside its word at no width. Returns the number of lines.
static u8 Reflow(const u8 *src, u16 width, u8 font)
{
    u8 word[64];
    u8 *out = sBook->lines, *end = sBook->lines + BR_LINES_SIZE - 70;
    u16 lineW = 0, w, spaceW, indentW;
    u8 n, len, count = 0, para = 0;
    bool8 started = FALSE, indent = FALSE;
    static const u8 sSpace[] = _(" ");
    static const u8 sIndent[] = _("   ");

    spaceW = Width(font, sSpace);
    indentW = Width(font, sIndent);
    sBook->lineStart[0] = 0;
    while (*src != EOS && out < end && count < BR_MAX_LINES - 1)
    {
        if (*src == CHAR_PROMPT_CLEAR)
        {
            src++;
            if (started)
            {
                *out++ = EOS;
                sBook->lineStart[++count] = out - sBook->lines;
                lineW = 0;
                started = FALSE;
            }
            para++;
            indent = (sBook->book == BOOK_GUIDE)    // the Guide's wide page: a paragraph opens indented
                  || (IsLayerBook() && !sBook->voices);   // and THE SUBTLE LAYER's, but not two voices
            continue;
        }
        if (*src == CHAR_SPACE || *src == CHAR_NEWLINE || *src == CHAR_PROMPT_SCROLL)
        {
            src++;
            continue;
        }
        n = 0;
        while (*src != EOS && *src != CHAR_SPACE && *src != CHAR_NEWLINE && *src != CHAR_PROMPT_SCROLL
               && *src != CHAR_PROMPT_CLEAR && n < 56)
        {
            if (*src == EXT_CTRL_CODE_BEGIN)
            {
                word[n++] = *src++;
                for (len = GetExtCtrlCodeLength(*src); len != 0 && *src != EOS && n < 62; len--)
                    word[n++] = *src++;
                continue;
            }
            word[n++] = *src++;
        }
        word[n] = EOS;
        w = Width(font, word);
        if (started && lineW + spaceW + w > width)
        {
            *out++ = EOS;
            sBook->lineStart[++count] = out - sBook->lines;
            lineW = 0;
            started = FALSE;
        }
        if (!started)
            sBook->linePara[count] = para;
        if (started)
        {
            *out++ = CHAR_SPACE;
            lineW += spaceW;
        }
        else if (indent)
        {
            out = StringCopy(out, sIndent);
            lineW += indentW;
        }
        indent = FALSE;
        out = StringCopy(out, word);
        lineW += w;
        started = TRUE;
    }
    *out = EOS;
    if (started)
        count++;
    sBook->lineCount = count;
    return count;
}

static const u8 *Line(u8 i)
{
    return sBook->lines + sBook->lineStart[i];
}

//  ---------------------------------------------------------------- THE NOTEBOOK: one ruled sheet on a spiral

static void DrawSheet(void)
{
    u8 i;
    s16 y;

    FillWindowPixelBuffer(WIN_BOOK, PIXEL_FILL(C_NONE));
    Rect(C_EDGE_DARK, NB_PAPER_X + 2, 6, 240 - NB_PAPER_X - 4, 152);       // the sheets under this one
    Rect(C_PAPER, NB_PAPER_X, 4, 240 - NB_PAPER_X - 6, 152);
    for (i = 0; i < NB_LINES; i++)                                          // the rules
        Rect(C_RULE, NB_PAPER_X, NB_TOP + i * NB_PITCH + NB_PITCH - 1, 240 - NB_PAPER_X - 6, 1);
    Rect(C_MARGIN, NB_MARGIN_X, 4, 1, 152);                                 // the margin
    for (y = 9; y < 150; y += 12)                                           // the spiral, through its holes
    {
        Rect(C_FRAME, NB_PAPER_X + 3, y + 1, 3, 3);
        Rect(C_RING, 4, y, NB_PAPER_X + 3, 2);
        Rect(C_EDGE_DARK, 4, y + 2, NB_PAPER_X + 2, 1);
    }
}

//  The empty page is drawn once, then copied: a redraw is a copy and the text.
static void Background(void (*draw)(void))
{
    if (sBook->blank == NULL)
    {
        draw();
        sBook->blank = Alloc(BR_WIN_BYTES);
        if (sBook->blank != NULL)   // T-421: no room to keep it -- draw it again next time
            CpuFastCopy(gWindows[WIN_BOOK].tileData, sBook->blank, BR_WIN_BYTES);
    }
    else
    {
        CpuFastCopy(sBook->blank, gWindows[WIN_BOOK].tileData, BR_WIN_BYTES);
    }
}

static void DrawNotebook(void)
{
    u8 buf[12], *p;
    u8 i, first;

    Background(DrawSheet);
    Print(BR_FONT, sInk, NB_TEXT_X, 4, sBook->title);
    if (sBook->pageCount > 1)
    {
        p = ConvertIntToDecimalStringN(buf, sBook->page + 1, STR_CONV_MODE_LEFT_ALIGN, 2);
        *p++ = CHAR_SLASH;
        ConvertIntToDecimalStringN(p, sBook->pageCount, STR_CONV_MODE_LEFT_ALIGN, 2);
        Print(BR_FONT, sFaint, 226 - Width(BR_FONT, buf), 4, buf);
    }

    first = sBook->page * NB_LINES;
    for (i = 0; i < NB_LINES && first + i < sBook->lineCount; i++)
        Print(BR_FONT, sInk, NB_TEXT_X, NB_TOP + i * NB_PITCH - 1, Line(first + i));
    if (sBook->qr != BOOK_QR_NONE && sBook->page + 1 == sBook->pageCount)       // T-315: the last page is the code
        DrawQr(sQrCodes[sBook->qr], (NB_MARGIN_X + 234) / 2, 86, C_PAPER, C_INK);
    if (sBook->song != 0 && sBook->page == 0)                                   // T-417: the song's staff
    {
        Rect(C_PAPER, NB_TEXT_X, SB_MIDDLE_C - 22, 226 - NB_TEXT_X, 30);
        for (i = 0; i < 5; i++)
            Rect(C_INK, NB_TEXT_X, SB_MIDDLE_C - 3 * (i + 1), 226 - NB_TEXT_X, 1);
    }

    if (sBook->page + 1 < sBook->pageCount)                                 // a dog-ear: there is more
    {
        for (i = 0; i < 8; i++)
        {
            Rect(C_NONE, 234 - i, 148 + i, i + 1, 1);
            Rect(C_EDGE_DARK, 226, 148 + i, 8 - i, 1);
        }
    }
    CopyWindowToVram(WIN_BOOK, COPYWIN_GFX);
}

//  ---------------------------------------------------------------- THE TEXTBOOK: an open book

static void DrawOpenBook(void)
{
    FillWindowPixelBuffer(WIN_BOOK, PIXEL_FILL(C_NONE));
    Rect(C_EDGE_DARK, TB_LEFT_X - 3, 8, TB_PAGE_W + 3, 149);                // the leaves beneath
    Rect(C_EDGE_DARK, TB_RIGHT_X, 8, TB_PAGE_W + 3, 149);
    Rect(C_EDGE, TB_LEFT_X - 2, 7, TB_PAGE_W + 2, 148);
    Rect(C_EDGE, TB_RIGHT_X, 7, TB_PAGE_W + 2, 148);
    Rect(C_PAPER, TB_LEFT_X, 5, TB_PAGE_W, 148);
    Rect(C_PAPER, TB_RIGHT_X, 5, TB_PAGE_W, 148);
    Rect(C_EDGE, TB_LEFT_X + TB_PAGE_W - 6, 5, 6, 148);                     // the gutter, into the spine
    Rect(C_EDGE_DARK, TB_LEFT_X + TB_PAGE_W - 2, 5, 2, 148);
    Rect(C_EDGE, TB_RIGHT_X, 5, 4, 148);
    Rect(C_EDGE_DARK, TB_RIGHT_X, 5, 1, 148);
    Rect(C_FRAME, TB_LEFT_X + TB_PAGE_W, 4, TB_RIGHT_X - TB_LEFT_X - TB_PAGE_W, 152);
}

static void DrawSpread(u16 folio)
{
    u8 buf[6];

    Background(DrawOpenBook);
    if (folio != 0)                                                         // even on the left, odd on the right
    {
        ConvertIntToDecimalStringN(buf, folio, STR_CONV_MODE_LEFT_ALIGN, 3);
        Print(BR_SMALL, sFaint, TB_LEFT_X + (TB_PAGE_W - Width(BR_SMALL, buf)) / 2, TB_FOLIO_Y, buf);
        ConvertIntToDecimalStringN(buf, folio + 1, STR_CONV_MODE_LEFT_ALIGN, 3);
        Print(BR_SMALL, sFaint, TB_RIGHT_X + (TB_PAGE_W - Width(BR_SMALL, buf)) / 2, TB_FOLIO_Y, buf);
    }
}

static bool8 ChapterOpen(u8 chapter)
{
    return FlagGet(sChapterFlags[chapter]);
}

//  Chapter 1 begins on page 2; each chapter is a spread for itself and one for each topic.
static u16 ChapterFolio(u8 chapter)
{
    return 2 + (chapter - 1) * (2 * (TB_TOPICS + 1));
}

static void FloorLabel(u8 *buf, u8 chapter)
{
    u8 *p = ConvertIntToDecimalStringN(buf, chapter, STR_CONV_MODE_LEFT_ALIGN, 1);
    StringCopy(p, sText_Floor);
}

//  A topic's heading is its first paragraph, as the board wrote it.
static void TopicHeading(u8 *dest, const u8 *topic, u8 max)
{
    u8 n = 0;

    while (*topic != EOS && *topic != CHAR_PROMPT_CLEAR && n < max - 1)
        dest[n++] = *topic++;
    dest[n] = EOS;
}

static const u8 *TopicBody(const u8 *topic)
{
    while (*topic != EOS && *topic != CHAR_PROMPT_CLEAR)
        topic++;
    if (*topic == CHAR_PROMPT_CLEAR)
        topic++;
    return topic;
}

static void DrawContents(void)
{
    u8 buf[20];
    u8 k, row;
    s16 x, y;
    bool8 open;

    DrawSpread(0);
    Print(BR_FONT, sInk, TB_LEFT_X + (TB_PAGE_W - Width(BR_FONT, sText_Contents)) / 2, 9, sText_Contents);
    Rect(C_RULE, TB_LEFT_X + TB_TEXT_INSET, 25, TB_TEXT_W, 1);
    Rect(C_RULE, TB_RIGHT_X + TB_TEXT_INSET, 25, TB_TEXT_W, 1);
    for (k = 1; k <= TB_CHAPTERS; k++)
    {
        row = (k <= 4) ? k - 1 : k - 5;
        x = ((k <= 4) ? TB_LEFT_X : TB_RIGHT_X) + TB_TEXT_INSET;
        y = 30 + row * 27;
        open = ChapterOpen(k);
        if (sBook->cursor == k - 1)
            Rect(C_HIGHLIGHT, x - 3, y - 1, TB_TEXT_W + 4, 26);
        DrawMark(k, x - 1, y + 3, 1, !open);
        Print(BR_FONT, open ? sInk : sFaint, x + 18, y - 1, sChapterNames[k]);
        FloorLabel(buf, k);
        Print(BR_SMALL, sFaint, x + 18, y + 11, buf);
        if (open)
        {
            ConvertIntToDecimalStringN(buf, ChapterFolio(k), STR_CONV_MODE_LEFT_ALIGN, 3);
            Print(BR_SMALL, sFaint, x + TB_TEXT_W - 2 - Width(BR_SMALL, buf), y + 11, buf);
        }
    }
    CopyWindowToVram(WIN_BOOK, COPYWIN_GFX);
}

static void DrawChapter(void)
{
    u8 buf[24];
    u8 t;
    s16 x = TB_LEFT_X + TB_TEXT_INSET, y;

    DrawSpread(ChapterFolio(sBook->chapter));
    FloorLabel(buf, sBook->chapter);
    Print(BR_SMALL, sFaint, x, 8, buf);
    Print(BR_FONT, sInk, x, 18, sChapterNames[sBook->chapter]);
    Rect(C_RULE, x, 34, TB_TEXT_W, 1);
    for (t = 0; t < TB_TOPICS; t++)
    {
        y = 40 + t * 18;
        if (sBook->cursor == t)
            Rect(C_HIGHLIGHT, x - 3, y - 1, TB_TEXT_W + 4, 16);
        TopicHeading(buf, sTopics[sBook->chapter][t], sizeof(buf));
        Print(BR_FONT, sInk, x + 2, y, buf);
    }
    DrawMark(sBook->chapter, TB_RIGHT_X + (TB_PAGE_W - 64) / 2, 34, 4, FALSE);
    CopyWindowToVram(WIN_BOOK, COPYWIN_GFX);
}

#define TB_TOPIC_LEFT_TOP    40
#define TB_TOPIC_RIGHT_TOP   12
#define TB_TOPIC_LEFT_LINES  ((TB_FOLIO_Y - 4 - TB_TOPIC_LEFT_TOP) / TB_PITCH)
#define TB_TOPIC_RIGHT_LINES ((TB_FOLIO_Y - 4 - TB_TOPIC_RIGHT_TOP) / TB_PITCH)

static void DrawTopic(void)
{
    u8 buf[24];
    u8 i, n;
    s16 x = TB_LEFT_X + TB_TEXT_INSET;
    const u8 *topic = sTopics[sBook->chapter][sBook->topic];

    DrawSpread(ChapterFolio(sBook->chapter) + 2 * (sBook->topic + 1));
    Print(BR_SMALL, sFaint, x, 8, sChapterNames[sBook->chapter]);
    TopicHeading(buf, topic, sizeof(buf));
    Print(BR_FONT, sInk, x, 18, buf);
    Rect(C_RULE, x, 34, TB_TEXT_W, 1);

    n = Reflow(TopicBody(topic), TB_TEXT_W, BR_FONT);
    for (i = 0; i < n && i < TB_TOPIC_LEFT_LINES; i++)
        Print(BR_FONT, sInk, x, TB_TOPIC_LEFT_TOP + i * TB_PITCH, Line(i));
    if (n > TB_TOPIC_LEFT_LINES)
    {
        for (; i < n && i < TB_TOPIC_LEFT_LINES + TB_TOPIC_RIGHT_LINES; i++)
            Print(BR_FONT, sInk, TB_RIGHT_X + TB_TEXT_INSET, TB_TOPIC_RIGHT_TOP + (i - TB_TOPIC_LEFT_LINES) * TB_PITCH, Line(i));
    }
    else
    {
        DrawMark(sBook->chapter, TB_RIGHT_X + (TB_PAGE_W - 48) / 2, 40, 3, FALSE);   // a plate, where the text ends
    }
    CopyWindowToVram(WIN_BOOK, COPYWIN_GFX);
}

//  ---------------------------------------------------------------- THE GUIDE: its cover, then an open book in its colours
//
//  COVER     the cover itself, drawn by the tool from the PDF's: A opens the book, B puts it away.
//  CONTENTS  the sections on the left page -- INTRODUCTION, the six PARTS, CONCLUSION, APPENDICES -- and the chapters
//            of the one under the cursor on the right. UP and DOWN move within a page; A or RIGHT crosses to the
//            chapters, LEFT or B comes back. A on a chapter opens it.
//  ENTRY     one wide page, not a spread (the user, 2026-09-25: three words a line was cramped): the chapter's full
//            title across the head of it, then its text, page after page. A, RIGHT or R turn on (past the last page,
//            RIGHT goes on to the next chapter); LEFT or L turn back. B returns to the contents.

static void DrawGuideBook(void)
{
    u8 i;
    s16 x;

    DrawOpenBook();
    for (i = 0; i < 2; i++)                                                 // a trace along the head of each page
    {
        x = (i == 0 ? TB_LEFT_X : TB_RIGHT_X) + TB_TEXT_INSET;
        Rect(C_RULE, x, 7, TB_TEXT_W, 1);
        Rect(C_MARGIN, x - 2, 6, 2, 3);
        Rect(C_MARGIN, x + TB_TEXT_W, 6, 2, 3);
    }
}

static u8 GuideSectionOf(u8 entry)
{
    u8 k;

    for (k = GUIDE_SECTIONS - 1; k > 0; k--)
    {
        if (entry >= sGuideSections[k].first)
            break;
    }
    return k;
}

static void DrawGuideCover(void)
{
    CpuFastCopy(sGuideCover, gWindows[WIN_BOOK].tileData, BR_WIN_BYTES);
    CopyWindowToVram(WIN_BOOK, COPYWIN_GFX);
}

#define GC_SECTION_TOP   30
#define GC_SECTION_PITCH 12
#define GC_ENTRY_TOP     37
#define GC_ENTRY_PITCH   18

static void DrawGuideContents(void)
{
    const struct GuideSection *sec = &sGuideSections[sBook->section];
    u8 k, n;
    s16 x = TB_LEFT_X + TB_TEXT_INSET, rx = TB_RIGHT_X + TB_TEXT_INSET, y;

    Background(DrawGuideBook);
    Print(BR_FONT, sInk, TB_LEFT_X + (TB_PAGE_W - Width(BR_FONT, sText_GuideContents)) / 2, 10, sText_GuideContents);
    Rect(C_RULE, x, 26, TB_TEXT_W, 1);
    for (k = 0; k < GUIDE_SECTIONS; k++)
    {
        y = GC_SECTION_TOP + k * GC_SECTION_PITCH;
        if (k == sBook->section)
        {
            if (sBook->focusRight)
                Rect(C_MARGIN, x - 5, y + 4, 2, 3);                        // the section the chapters belong to
            else
                Rect(C_HIGHLIGHT, x - 3, y - 1, TB_TEXT_W + 4, GC_SECTION_PITCH);
        }
        Print(BR_FONT, sInk, x, y - 2, sGuideSections[k].label);
    }

    n = Reflow(sec->heading, TB_TEXT_W, BR_SMALL);                         // the section's own heading, small
    for (k = 0; k < n && k < 2; k++)
        Print(BR_SMALL, sFaint, rx, 9 + k * 9, Line(k));
    Rect(C_RULE, rx, 31, TB_TEXT_W, 1);
    for (k = 0; k < sec->count; k++)
    {
        y = GC_ENTRY_TOP + k * GC_ENTRY_PITCH;
        if (sBook->focusRight && k == sBook->cursor)
            Rect(C_HIGHLIGHT, rx - 3, y - 1, TB_TEXT_W + 4, GC_ENTRY_PITCH - 2);
        Print(BR_FONT, sInk, rx, y, sGuideEntries[sec->first + k].shortTitle);
    }
    CopyWindowToVram(WIN_BOOK, COPYWIN_GFX);
}

//  The reading page: one sheet across the screen, the cover's charcoal, a trace along its head.
#define GP_X           10
#define GP_W           220
#define GP_TEXT_X      18
#define GP_TEXT_W      204
#define GP_PITCH       12
#define GP_BOTTOM      150
#define GE_TITLE_TOP   19
#define GE_TITLE_PITCH 10
#define GE_PAGE_TOP    20
#define GE_PAGE_LINES  ((GP_BOTTOM - GE_PAGE_TOP) / GP_PITCH)

static void DrawGuidePage(void)
{
    FillWindowPixelBuffer(WIN_BOOK, PIXEL_FILL(C_NONE));
    Rect(C_EDGE_DARK, GP_X + 2, 7, GP_W, 150);                              // the pages beneath
    Rect(C_EDGE, GP_X + 1, 6, GP_W, 150);
    Rect(C_PAPER, GP_X, 4, GP_W, 150);
    Rect(C_FRAME, GP_X, 4, 3, 150);                                         // the spine's edge
    Rect(C_RULE, GP_TEXT_X, 7, GP_TEXT_W, 1);                               // the trace, with a node at each end
    Rect(C_MARGIN, GP_TEXT_X - 2, 6, 2, 3);
    Rect(C_MARGIN, GP_TEXT_X + GP_TEXT_W, 6, 2, 3);
}

static void GuidePageBackground(void)
{
    if (sBook->blankPage == NULL)
    {
        DrawGuidePage();
        sBook->blankPage = Alloc(BR_WIN_BYTES);
        if (sBook->blankPage != NULL)   // T-421: no room to keep it -- draw it again next time
            CpuFastCopy(gWindows[WIN_BOOK].tileData, sBook->blankPage, BR_WIN_BYTES);
    }
    else
    {
        CpuFastCopy(sBook->blankPage, gWindows[WIN_BOOK].tileData, BR_WIN_BYTES);
    }
}

//  Where the text starts under the title, and how many pages it needs: the first page gives the title its room,
//  and every page after it is text from the trace down.
static u8 GuideFirstLines(void)
{
    return (GP_BOTTOM - sBook->bodyTop) / GP_PITCH;
}

//  T-304: UNDERSTANDINGS ARRIVE IN THE GUIDE'S MARGINS (decided by the user 2026-09-25). Reading the Guide never grants
//  one -- it is method, and 4.3's rule is that method survives a handoff and understanding does not -- but once an
//  understanding is arrived at elsewhere, a note appears under the text of the chapter it concerns, indented, in the
//  Guide's cyan, as if written there. Nothing counts them; a chapter simply has a note it did not have.
//  WHICH CHAPTER, and the words, are T-252's question with where the understanding is earned: the first is DREAMS
//  (the user, 2026-10-01): it is earned reading TANOBY's last chamber, and the chapter's reality check is reading a
//  line twice.
#define GUIDE_ENTRY_DREAMS 19
#define GM_INDENT 28
static const u8 sText_MarginFirst[] = _("Read it twice. It stayed.");   // the user's (T-304, T-252)
//  DRAFT (T-252, 2026-09-26): the six the user named, each in the chapter it answers, in the player's own hand.
static const u8 sText_MarginSchool[]  = _("They taught me the names. The rest I will have to find.");
static const u8 sText_MarginReading[] = _("It was there before. I was not looking.");
static const u8 sText_MarginNotes[]   = _("Every step made sense on its own.");
static const u8 sText_MarginScorn[]   = _("He kept score. He was very good at it.");
static const u8 sText_MarginReturn[]  = _("It took him years to come back. He came back.");
static const u8 sText_MarginGuide[]   = _("Read to the end. Now the part a book cannot do.");
static const struct { u16 flag; u8 entry; const u8 *note; } sGuideMarginNotes[] = {
    { FLAG_UNDERSTANDING_FIRST,   GUIDE_ENTRY_DREAMS, sText_MarginFirst },   // DREAMS, earned at TANOBY
    { FLAG_UNDERSTANDING_SCHOOL,  1,  sText_MarginSchool },    // THE TOOLKIT
    { FLAG_UNDERSTANDING_READING, 9,  sText_MarginReading },   // MONITORING
    { FLAG_UNDERSTANDING_NOTES,   6,  sText_MarginNotes },     // THE LOOP
    { FLAG_UNDERSTANDING_SCORN,   8,  sText_MarginScorn },     // DEBUGGING
    { FLAG_UNDERSTANDING_RETURN,  14, sText_MarginReturn },    // CRASHES
    { FLAG_UNDERSTANDING_GUIDE,   24, sText_MarginGuide },     // THE PATH FORWARD
};

//  T-252: WHEN EACH IS ARRIVED AT. Asked on every map load (overworld.c): an understanding whose conditions all hold
//  is set, silently -- nothing is announced and nothing counts them. Conditions, not scripts, so every way into a
//  moment (the story, a debug JUMP, an old save) arrives at the same place.
static const struct { u16 flag; u16 needs[2]; } sUnderstandingConditions[] = {
    { FLAG_UNDERSTANDING_SCHOOL,  { FLAG_SCHOOL_GOT_TEXTBOOK, FLAG_GOT_DIPLOMA } },
    { FLAG_UNDERSTANDING_READING, { FLAG_GOT_DIPLOMA, FLAG_GOT_REVEAL } },
    { FLAG_UNDERSTANDING_NOTES,   { FLAG_NOTEBOOK_RUN_FATAL, FLAG_NOTEBOOK_FILE_COMPLETE } },
    { FLAG_UNDERSTANDING_SCORN,   { FLAG_BADGE08_GET, FLAG_BADGE08_GET } },
    { FLAG_UNDERSTANDING_RETURN,  { FLAG_TY_GAVE_PAYLOAD, FLAG_CRYSTAL_READ_PAYLOAD } },
    { FLAG_UNDERSTANDING_GUIDE,   { FLAG_GOT_GUIDE, FLAG_GUIDE_READ } },
};

void DaemonsArriveAtUnderstandings(void)
{
    u8 i;

    for (i = 0; i < ARRAY_COUNT(sUnderstandingConditions); i++)
        if (!FlagGet(sUnderstandingConditions[i].flag)
         && FlagGet(sUnderstandingConditions[i].needs[0]) && FlagGet(sUnderstandingConditions[i].needs[1]))
            FlagSet(sUnderstandingConditions[i].flag);
}

//  T-235 (the user, 2026-09-27): HOLDING EVERY UNDERSTANDING UNLOCKS THE FIVE WITNESSES' REWARD. VAR_RESULT TRUE once
//  all of them are held -- the margin notes' table is the list, so a new understanding joins the lock by joining it.
void DaemonsHoldsEveryUnderstanding(void)
{
    u8 i;

    gSpecialVar_Result = TRUE;
    for (i = 0; i < ARRAY_COUNT(sGuideMarginNotes); i++)
        if (!FlagGet(sGuideMarginNotes[i].flag))
            gSpecialVar_Result = FALSE;
}

//  T-317: how many understandings are still to come -- for the TOWN MAP's clarity, never shown as a number.
u8 DaemonsUnderstandingsMissing(void)
{
    u8 i, missing = 0;

    for (i = 0; i < ARRAY_COUNT(sGuideMarginNotes); i++)
        if (!FlagGet(sGuideMarginNotes[i].flag))
            missing++;
    return missing;
}

//  T-317: the clarity rule, one place for the map and the INDEX: two by two while most understandings are still to
//  come, two by one while a few are, one by one (sharp) once every one is held.
void DaemonsClarity(u8 *w, u8 *h)
{
    u8 missing = DaemonsUnderstandingsMissing();

    *w = missing != 0 ? 2 : 1;
    *h = missing >= 4 ? 2 : 1;
}

static const u8 *GuideMarginNote(u8 entry)
{
    u8 i;

    if (sBook->torn)                                    // the notes are in the player's copy, not the Board's
        return NULL;
    for (i = 0; i < ARRAY_COUNT(sGuideMarginNotes); i++)
        if (sGuideMarginNotes[i].entry == entry && FlagGet(sGuideMarginNotes[i].flag))
            return sGuideMarginNotes[i].note;
    return NULL;
}

//  T-410 (the user, 2026-10-16; approved 2026-10-18): once the understanding's MARK is held too, how the MARK balances
//  it -- in the same hand, after one more blank line under the note (notebook.c holds the words, DRAFT).
static const u8 *GuideBalanceNote(u8 entry)
{
    u8 i;

    if (sBook->torn)
        return NULL;
    for (i = 0; i < ARRAY_COUNT(sGuideMarginNotes); i++)
        if (sGuideMarginNotes[i].entry == entry && FlagGet(sGuideMarginNotes[i].flag))
            return Daemons_RootBalance(sGuideMarginNotes[i].flag);
    return NULL;
}

//  How many lines the entry fills: its text, then -- when it has a margin note -- a blank line and the note, and
//  (T-410) another blank line and the balance.
static u8 GuideTextLines(const struct GuideEntry *e, u8 *textLines)
{
    const u8 *note = GuideMarginNote(sBook->entry);
    const u8 *balance = GuideBalanceNote(sBook->entry);
    u8 n = Reflow(e->text, GP_TEXT_W, BR_FONT);

    *textLines = n;
    if (note != NULL)
        n += 1 + Reflow(note, GP_TEXT_W - GM_INDENT, BR_FONT);
    if (note != NULL && balance != NULL)
        n += 1 + Reflow(balance, GP_TEXT_W - GM_INDENT, BR_FONT);
    return n;
}

//  T-311: THE REVIEW BOARD'S COPY. Every page is there but ETHICAL CONSIDERATIONS, which is cut out close to the
//  spine: the contents still list it, and turning to it finds a stub of paper and the page beneath. Nothing says so
//  (craft rule 1). THE FUTURE, the page before, ends "That is exactly why the next section matters."
#define GUIDE_ENTRY_ETHICS 23

static bool8 GuideEntryTornOut(void)
{
    return sBook->torn && sBook->entry == GUIDE_ENTRY_ETHICS;
}

static void DrawGuideTornPage(void)
{
    //  the stub's ragged edge, row by row: how far the paper still reaches from the spine
    static const u8 sStub[] = { 7, 8, 6, 9, 8, 7, 10, 8, 6, 7, 9, 11, 8, 7, 6, 8, 10, 9, 7, 8 };
    u8 y;

    FillWindowPixelBuffer(WIN_BOOK, PIXEL_FILL(C_NONE));
    Rect(C_EDGE_DARK, GP_X + 2, 7, GP_W, 150);                              // the pages beneath, one nearer now
    Rect(C_EDGE, GP_X + 1, 6, GP_W, 150);
    for (y = 0; y < 150; y++)
        Rect(C_PAPER, GP_X, 4 + y, sStub[(y / 3 + y / 17) % ARRAY_COUNT(sStub)], 1);
    Rect(C_FRAME, GP_X, 4, 3, 150);                                         // the spine's edge
    CopyWindowToVram(WIN_BOOK, COPYWIN_GFX);
}

#define GUIDE_ENTRY_LAST (ARRAY_COUNT(sGuideEntries) - 1)     // FURTHER READING, which ends on the code (T-315)

static void GuideLayout(void)
{
    const struct GuideEntry *e = &sGuideEntries[sBook->entry];
    u8 titleLines = Reflow(e->title, GP_TEXT_W, BR_SMALL);
    u8 n, textLines;

    sBook->bodyTop = GE_TITLE_TOP + titleLines * GE_TITLE_PITCH + 9;
    n = GuideTextLines(e, &textLines);
    sBook->spreadCount = 1;
    if (GuideEntryTornOut())
        return;
    if (n > GuideFirstLines())
        sBook->spreadCount += (n - GuideFirstLines() + GE_PAGE_LINES - 1) / GE_PAGE_LINES;
    if (sBook->entry == GUIDE_ENTRY_LAST)                                      // T-315: and a page for the code
        sBook->spreadCount++;
    if (sBook->spread >= sBook->spreadCount)
        sBook->spread = sBook->spreadCount - 1;
}

static void DrawGuideEntry(void)
{
    const struct GuideEntry *e = &sGuideEntries[sBook->entry];
    u8 buf[12], *p;
    u8 i, n, lines, start;
    s16 top;

    GuideLayout();
    if (GuideEntryTornOut())
    {
        DrawGuideTornPage();
        return;
    }
    GuidePageBackground();
    Print(BR_SMALL, sFaint, GP_TEXT_X, 9, sGuideSections[GuideSectionOf(sBook->entry)].label);
    if (sBook->spreadCount > 1)                                             // 2/3, at the head's right
    {
        p = ConvertIntToDecimalStringN(buf, sBook->spread + 1, STR_CONV_MODE_LEFT_ALIGN, 1);
        *p++ = CHAR_SLASH;
        ConvertIntToDecimalStringN(p, sBook->spreadCount, STR_CONV_MODE_LEFT_ALIGN, 1);
        Print(BR_SMALL, sFaint, GP_TEXT_X + GP_TEXT_W - Width(BR_SMALL, buf), 9, buf);
    }
    if (sBook->entry == GUIDE_ENTRY_LAST)
        FlagSet(FLAG_GUIDE_READ);                                              // T-252: read to its end
    if (sBook->entry == GUIDE_ENTRY_LAST && sBook->spread + 1 == sBook->spreadCount)
    {
        //  T-315: the last page of the last entry is the code, light on the Guide's dark page, where the book's own
        //  further reading would go. The Board's copy has it too: it is the same book.
        DrawQr(sQrCodes[BOOK_QR_GUIDE], GP_X + GP_W / 2, 86, C_INK, C_PAPER);
        CopyWindowToVram(WIN_BOOK, COPYWIN_GFX);
        return;
    }
    if (sBook->spread == 0)
    {
        n = Reflow(e->title, GP_TEXT_W, BR_SMALL);
        for (i = 0; i < n; i++)
            Print(BR_SMALL, sInk, GP_TEXT_X, GE_TITLE_TOP + i * GE_TITLE_PITCH, Line(i));
        Rect(C_RULE, GP_TEXT_X, sBook->bodyTop - 5, GP_TEXT_W, 1);
        top = sBook->bodyTop;
        lines = GuideFirstLines();
        start = 0;
    }
    else
    {
        top = GE_PAGE_TOP;
        lines = GE_PAGE_LINES;
        start = GuideFirstLines() + (sBook->spread - 1) * GE_PAGE_LINES;
    }

    n = Reflow(e->text, GP_TEXT_W, BR_FONT);
    for (i = 0; i < lines && start + i < n; i++)
        Print(BR_FONT, sInk, GP_TEXT_X, top + i * GP_PITCH, Line(start + i));
    if (GuideMarginNote(sBook->entry) != NULL)                               // T-304: the note, after one blank line
    {
        u8 textLines = n, k;

        n = Reflow(GuideMarginNote(sBook->entry), GP_TEXT_W - GM_INDENT, BR_FONT);
        for (k = 0; k < n; k++)
        {
            u8 at = textLines + 1 + k;                                      // its line in the whole entry
            if (at >= start && at < start + lines)
                Print(BR_FONT, sPen, GP_TEXT_X + GM_INDENT, top + (at - start) * GP_PITCH, Line(k));
        }
        if (GuideBalanceNote(sBook->entry) != NULL)                         // T-410: the balance, after one more
        {
            u8 noteEnd = textLines + 1 + n;

            n = Reflow(GuideBalanceNote(sBook->entry), GP_TEXT_W - GM_INDENT, BR_FONT);
            for (k = 0; k < n; k++)
            {
                u8 at = noteEnd + 1 + k;
                if (at >= start && at < start + lines)
                    Print(BR_FONT, sPen, GP_TEXT_X + GM_INDENT, top + (at - start) * GP_PITCH, Line(k));
            }
        }
    }

    if (sBook->spread + 1 < sBook->spreadCount)                             // a dog-ear while there is more
    {
        for (i = 0; i < 6; i++)
            Rect(C_MARGIN, GP_X + GP_W - 6 + i, 153 - i, 6 - i, 1);
    }
    CopyWindowToVram(WIN_BOOK, COPYWIN_GFX);
}


//  ---------------------------------------------------------------- T-423: THE SUBTLE LAYER, an open book with pictures
//
//  The user's psychic book, rewritten as a trainer's handbook (docs/subtle-layer.md, tools/gensubtle.py). It reads
//  as an open book (the user, 2026-10-18): a cover, a contents spread like the Guide's, then each entry opens with
//  its picture on the left page and its text on the right, continuing over both pages. The page is CONTEXT's hue on
//  purpose -- this book's colour carries the argument -- and the seven centres are never coloured. In TWO VOICES the
//  asking voice is printed faint: the generator marks which paragraphs ask, and Reflow notes each line's paragraph.

#define SB_PIC_X     (TB_LEFT_X + TB_TEXT_INSET + (TB_TEXT_W - 96) / 2)
#define SB_PIC_Y     13
#define SB_CAP_Y     103
#define SB_TOP       14
#define SB_BOTTOM    135
#define SB_PER_PAGE  ((SB_BOTTOM - SB_TOP) / TB_PITCH)
#define SB_ANIM      40          // frames between a picture's frames (SHADOWS, WHICH PATH)

static bool8 IsLayerBook(void)
{
    return sBook->book == BOOK_SUBTLE || sBook->book == BOOK_HERMETIC || sBook->book == BOOK_EUDAIMON
        || sBook->book == BOOK_UNITY || sBook->book == BOOK_ORIGIN;
}

static const struct LayerBook *Layer(void)
{
    if (LensOn())
        return &sBook->lens->book;      // T-529: the reading the lens is turned to
    if (sBook->book == BOOK_ORIGIN)
        return &sBook->origin;          // T-533: only the sections that have come back (Origin_Open)
    return &sLayerBooks[sBook->book == BOOK_UNITY ? 3 : sBook->book == BOOK_EUDAIMON ? 2 : sBook->book == BOOK_HERMETIC];
}

//  T-519 item 2: THE EUDAIMON LAYER has no CARDS section of its own yet (the dawn readings of the two decks are the
//  companion lane's), so its cover shows no deck tile. T-519 item 3 (vision 4.40): THE UNITY BOOK has the night half's
//  deck, which leaves the bag at the mending, and deals it Both as One only. T-533: THE ORIGIN OF A DAEMON has none.
static bool8 HasCards(void)
{
    return sBook->book == BOOK_SUBTLE || sBook->book == BOOK_HERMETIC || sBook->book == BOOK_UNITY;
}

//  T-533 (the user, 2026-11-10; bible 9.35): THE ORIGIN OF A DAEMON COMES BACK A SECTION AT A TIME. Its first section
//  (THE DISC, I and II) is there from the day it is taken off the bedroom shelf; each after it waits for a number of
//  understandings held, some for SCORN's TRUE MARK as well (the generated header's sOriginOpensRoots and
//  sOriginOpensMark), and for the one before it -- so what is open is always a run from the start. SCORN's own
//  understanding follows that MARK on the next map load, so winning it can bring one section back at once and another
//  after that load. Until a section comes back the book does not have it: no label, nothing faint, no count (the
//  reader's cut, Origin_Open).
u8 Origin_OpenSections(void)
{
    u8 held = ARRAY_COUNT(sGuideMarginNotes) - DaemonsUnderstandingsMissing();
    bool8 mark = FlagGet(FLAG_BADGE08_GET);
    u8 open;

    for (open = 0; open < ORIGIN_SECTIONS; open++)
        if (held < sOriginOpensRoots[open] || (sOriginOpensMark[open] && !mark))
            break;
    return open != 0 ? open : 1;        // the first is never held back
}

//  T-533: A PAGE FILLS. The first step after a section has come back (the book held), it is told -- once, every
//  section that came back at once in one line, and never which or how many. field_control_avatar.c's step hook asks
//  just after the insight's, so never on the step an insight arrives. Returns how many were told now (0, 1 or more);
//  the hook chooses the line by it. FLAG_ORIGIN_PAGE_1 is the second section's, and the nine run on in order.
STATIC_ASSERT(ORIGIN_SECTIONS == 10 && FLAG_ORIGIN_PAGE_9 == FLAG_ORIGIN_PAGE_1 + 8, OriginPageFlagsRunInOrder);

u8 Origin_TryPageFills(void)
{
    u8 open, i, told = 0;

    if (!FlagGet(FLAG_GOT_ORIGIN_BOOK))
        return 0;
    open = Origin_OpenSections();
    for (i = 1; i < open; i++)
    {
        if (!FlagGet(FLAG_ORIGIN_PAGE_1 + i - 1))
        {
            FlagSet(FLAG_ORIGIN_PAGE_1 + i - 1);
            told++;
        }
    }
    return told;
}

static u8 SubtleSectionOf(u8 entry);

//  T-529: the book and entry the open page is read from -- through the lens, the half's own (its observer line, its
//  spells); otherwise the reader's.
static void LensOrigin(u8 *book, u8 *entry)
{
    u8 from;

    *book = sBook->book;
    *entry = sBook->entry;
    if (!LensOn())
        return;
    from = sBook->lens->from[SubtleSectionOf(sBook->entry)];
    *book = from == LAYER_HERMETIC ? BOOK_HERMETIC : from == LAYER_EUDAIMON ? BOOK_EUDAIMON : BOOK_UNITY;
    *entry = sBook->lens->origin[sBook->entry];
}

//  T-529: the page's palette -- the book's own, or through the lens the half's own in its tone (the contents in the
//  setting's). Only the cover, the contents and the pages: the CARDS keep theirs.
static void LensPalette(void)
{
    const u16 *src = sUnityPal;
    u16 pal[16];
    u8 i, from = LAYER_UNITY;

    if (sBook->book != BOOK_UNITY)
        return;
    if (sBook->view != VIEW_COVER && sBook->view != VIEW_SUBTLE_CONTENTS && sBook->view != VIEW_SUBTLE_ENTRY)
        return;
    if (LensOn() && sBook->view == VIEW_SUBTLE_ENTRY)
        from = sBook->lens->from[SubtleSectionOf(sBook->entry)];
    else if (LensOn() && sBook->view == VIEW_SUBTLE_CONTENTS)
        from = sUnityLens == LENS_NIGHT ? LAYER_HERMETIC : LAYER_EUDAIMON;
    if (from == LAYER_HERMETIC)
        src = sHermeticPal;
    else if (from == LAYER_EUDAIMON)
        src = sEudaimonPal;
    for (i = 0; i < 16; i++)
        pal[i] = from == LAYER_UNITY ? src[i] : DaemonsBookTone(src[i], from == LAYER_EUDAIMON);
    LoadPalette(pal, BG_PLTT_ID(15), PLTT_SIZE_4BPP);
    LoadPalette(&pal[C_COVER], BG_PLTT_ID(0), PLTT_SIZEOF(1));          // the backdrop is the cover
}

//  T-519 item 2: an entry's text -- and in the dawn half's BLESSING, the OBSERVER's line chosen by who is looking: the
//  lead daemon's SPIRITS (0 as it is, 1 in good spirits, 2 in low spirits; an egg, or nobody, reads as it is).
static const u8 *LayerText(const struct SubtleEntry *e)
{
    u8 class = SPIRITS_AS_IT_IS, book, entry;

    LensOrigin(&book, &entry);   // T-529: through the lens, the book and entry the page is read from
    // T-519 item 2: a half opened in DAEMON TOWER before the mending ends its last page torn at the spine.
    if (book == BOOK_HERMETIC && entry == HERMETIC_TORN_ENTRY && Mending_TornHere())
        return sHermeticTorn;
    if (book == BOOK_EUDAIMON && entry == EUDAIMON_TORN_ENTRY && Mending_TornHere())
        return sEudaimonTorn;
    if (!(book == BOOK_EUDAIMON && entry == EUDAIMON_OBSERVER_ENTRY)
     && !(book == BOOK_UNITY && entry == UNITY_OBSERVER_ENTRY))   // T-519 item 3: the ATTESTATION's
        return e->text;
    if (GetMonData(&gPlayerParty[0], MON_DATA_SPECIES) != SPECIES_NONE && !GetMonData(&gPlayerParty[0], MON_DATA_IS_EGG))
        class = Spirits_Class(&gPlayerParty[0].box);
    if (class >= 3)
        class = SPIRITS_AS_IT_IS;
    return book == BOOK_UNITY ? sUnityObserver[class] : sEudaimonObserver[class];
}

//  T-483: an INSCRIPTION ('^ ' in docs/hermetic-layer.md) is printed in the tablet's emerald -- C_RING, which
//  BreatheHermetic moves with the pulse -- over the page's own shadow.
static const u8 sInscribed[3] = { C_NONE, C_RING, C_SHADOW };

static u8 SubtleSectionOf(u8 entry)
{
    u8 k;

    for (k = Layer()->sectionCount - 1; k > 0; k--)
    {
        if (entry >= Layer()->sections[k].first)
            break;
    }
    return k;
}

static void DrawCardsTile(void);

static void DrawSubtleCover(void)
{
    CpuFastCopy(Layer()->cover, gWindows[WIN_BOOK].tileData, BR_WIN_BYTES);
    if (HasCards())
        DrawCardsTile();    // T-509: the CARDS section's tile, beside the cover
    CopyWindowToVram(WIN_BOOK, COPYWIN_GFX);
}

//  T-483 (7): on Halloween, THE HERMETIC LAYER opens on the Book of Thirteen's card of the day (the companion's
//  thirteenOfDay, daemons_hermetic.c) before its warning: its number and name on the left page, its line in the
//  breathing emerald on the right, and the daemon it belongs to. Every word DRAFT.
static const u8 sText_Thirteen[]      = _("THE THIRTEEN");     // DRAFT
static const u8 sText_ThirteenNo[]    = _("No. ");             // DRAFT
static const u8 sText_ThirteenNight[] = _("Tonight's card");   // DRAFT

static void DrawThirteen(void)
{
    const struct ThirteenCard *c = Hermetic_CardOfDay();
    s16 x = TB_LEFT_X + TB_TEXT_INSET, rx = TB_RIGHT_X + TB_TEXT_INSET;
    u8 k, n, no[12];

    Background(DrawGuideBook);
    if (c != NULL)
    {
        Print(BR_SMALL, sFaint, x + (TB_TEXT_W - Width(BR_SMALL, sText_Thirteen)) / 2, 10, sText_Thirteen);
        Rect(C_RULE, x, 26, TB_TEXT_W, 1);
        StringCopy(no, sText_ThirteenNo);
        StringAppend(no, c->n);
        Print(BR_FONT, sInk, x + (TB_TEXT_W - Width(BR_FONT, no)) / 2, 52, no);
        Print(BR_FONT, sInscribed, x + (TB_TEXT_W - Width(BR_FONT, c->keyword)) / 2, 70, c->keyword);
        Print(BR_SMALL, sFaint, x + (TB_TEXT_W - Width(BR_SMALL, sText_ThirteenNight)) / 2, 118, sText_ThirteenNight);
        n = Reflow(c->phrase, TB_TEXT_W, BR_FONT);
        for (k = 0; k < n && k < 4; k++)
            Print(BR_FONT, sInscribed, rx, 44 + k * TB_PITCH, Line(k));
        Rect(C_RULE, rx, 104, TB_TEXT_W, 1);
        Print(BR_SMALL, sFaint, rx + TB_TEXT_W - Width(BR_SMALL, c->species), 108, c->species);
    }
    CopyWindowToVram(WIN_BOOK, COPYWIN_GFX);
}

//  T-519 item 3: THE UNITY BOOK has eleven sections (BALANCE at the crease), so its contents are set at an 11px
//  pitch; every other book keeps 12.
#define UNITY_SECTION_PITCH 11

static void DrawSubtleContents(void)
{
    const struct SubtleSection *sec = &Layer()->sections[sBook->section];
    u8 k, n, pitch = sBook->book == BOOK_UNITY ? UNITY_SECTION_PITCH : GC_SECTION_PITCH;
    s16 x = TB_LEFT_X + TB_TEXT_INSET, rx = TB_RIGHT_X + TB_TEXT_INSET, y;

    Background(DrawGuideBook);
    Print(BR_FONT, sInk, TB_LEFT_X + (TB_PAGE_W - Width(BR_FONT, sText_GuideContents)) / 2, 10, sText_GuideContents);
    Rect(C_RULE, x, 26, TB_TEXT_W, 1);
    for (k = 0; k < Layer()->sectionCount; k++)
    {
        y = GC_SECTION_TOP + k * pitch;
        if (k == sBook->section)
        {
            if (sBook->focusRight)
                Rect(C_MARGIN, x - 5, y + 4, 2, 3);
            else
                Rect(C_HIGHLIGHT, x - 3, y - 1, TB_TEXT_W + 4, pitch);
        }
        Print(BR_FONT, sInk, x, y - 2, Layer()->sections[k].label);
    }
    n = Reflow(sec->heading, TB_TEXT_W, BR_SMALL);
    for (k = 0; k < n && k < 2; k++)
        Print(BR_SMALL, sFaint, rx, 9 + k * 9, Line(k));
    Rect(C_RULE, rx, 31, TB_TEXT_W, 1);
    for (k = 0; k < sec->count; k++)
    {
        y = GC_ENTRY_TOP + k * GC_ENTRY_PITCH;
        if (sBook->focusRight && k == sBook->cursor)
            Rect(C_HIGHLIGHT, rx - 3, y - 1, TB_TEXT_W + 4, GC_ENTRY_PITCH - 2);
        Print(BR_FONT, sInk, rx, y, Layer()->entries[sec->first + k].shortTitle);
    }
    CopyWindowToVram(WIN_BOOK, COPYWIN_GFX);
}

static void DrawSubtlePicture(void)
{
    const struct SubtlePicture *p = &Layer()->pictures[Layer()->entries[sBook->entry].picture];

    BlitBitmapRectToWindow(WIN_BOOK, (const u8 *)p->frame[sBook->picFrame % p->frames], 0, 0, p->w, p->h,
                           SB_PIC_X, SB_PIC_Y, p->w, p->h);
}

//  The entry's title, then its text, reflowed; returns where the text starts on the opening page. The text's lines
//  are left in sBook->lines, and the spread count is set.
static u8 SubtleLayout(u8 *titleLines, u8 *firstLines)
{
    const struct SubtleEntry *e = &Layer()->entries[sBook->entry];
    u8 start, n;

    *titleLines = Reflow(e->title, TB_TEXT_W, BR_SMALL);
    start = 19 + *titleLines * 10 + 9;
    *firstLines = (SB_BOTTOM - start) / TB_PITCH;
    sBook->voices = (e->faint | e->inscribed) != 0;   // T-483: an inscription starts its own line, as a voice does
    n = Reflow(LayerText(e), TB_TEXT_W, BR_FONT);
    sBook->voices = FALSE;
    sBook->spreadCount = 1;
    if (n > *firstLines)
        sBook->spreadCount += ((n - *firstLines + SB_PER_PAGE - 1) / SB_PER_PAGE + 1) / 2;
    if (sBook->spread >= sBook->spreadCount)
        sBook->spread = sBook->spreadCount - 1;
    return start;
}

static void PrintSubtleLines(u8 from, u8 count, s16 x, s16 y)
{
    const struct SubtleEntry *e = &Layer()->entries[sBook->entry];
    u8 i;

    for (i = 0; i < count && from + i < sBook->lineCount; i++)
        Print(BR_FONT, (e->faint >> sBook->linePara[from + i]) & 1 ? sFaint
                     : (e->inscribed >> sBook->linePara[from + i]) & 1 ? sInscribed : sInk, x, y + i * TB_PITCH, Line(from + i));
}

static void DrawSubtleEntry(void)
{
    const struct SubtleEntry *e = &Layer()->entries[sBook->entry];
    u8 buf[6], titleLines, firstLines, i, n, from;
    s16 x = TB_LEFT_X + TB_TEXT_INSET, rx = TB_RIGHT_X + TB_TEXT_INSET, start;
    u16 folio = e->folio + 2 * sBook->spread;

    if (sBook->picEntry != sBook->entry)
    {
        sBook->picEntry = sBook->entry;
        sBook->picFrame = 0;
        sBook->picTimer = 0;
    }
    Background(DrawGuideBook);
    ConvertIntToDecimalStringN(buf, folio, STR_CONV_MODE_LEFT_ALIGN, 3);
    Print(BR_SMALL, sFaint, TB_LEFT_X + (TB_PAGE_W - Width(BR_SMALL, buf)) / 2, TB_FOLIO_Y, buf);
    ConvertIntToDecimalStringN(buf, folio + 1, STR_CONV_MODE_LEFT_ALIGN, 3);
    Print(BR_SMALL, sFaint, TB_RIGHT_X + (TB_PAGE_W - Width(BR_SMALL, buf)) / 2, TB_FOLIO_Y, buf);
    if (sBook->spread == 0)
    {
        DrawSubtlePicture();
        n = Reflow(e->caption, TB_TEXT_W, BR_SMALL);
        for (i = 0; i < n && i < 3; i++)
            Print(BR_SMALL, sFaint, x + (TB_TEXT_W - Width(BR_SMALL, Line(i))) / 2, SB_CAP_Y + i * 10, Line(i));
        Print(BR_SMALL, sFaint, rx, 9, Layer()->sections[SubtleSectionOf(sBook->entry)].label);
        n = Reflow(e->title, TB_TEXT_W, BR_SMALL);
        for (i = 0; i < n; i++)
            Print(BR_SMALL, sInk, rx, 19 + i * 10, Line(i));
    }
    start = SubtleLayout(&titleLines, &firstLines);
    LensOrigin(&from, &i);                          // T-529: the dawn half only words the spells, as it does alone
    if ((from == BOOK_HERMETIC || from == BOOK_UNITY) && sBook->spread + 1 == sBook->spreadCount)
        Spellbook_OnPassageRead(e->shortTitle);     // T-483 (5): a fold read to its end lets its spell off the page
    if (sBook->book == BOOK_UNITY && sBook->lens != NULL)   // T-529: the lens's setting, under the picture's page
        Print(BR_SMALL, sFaint, TB_LEFT_X + TB_TEXT_INSET, TB_FOLIO_Y, sLensNames[sUnityLens]);
    if (sBook->spread == 0)
    {
        Rect(C_RULE, rx, start - 5, TB_TEXT_W, 1);
        PrintSubtleLines(0, firstLines, rx, start);
    }
    else
    {
        from = firstLines + (2 * sBook->spread - 2) * SB_PER_PAGE;
        PrintSubtleLines(from, SB_PER_PAGE, x, SB_TOP);
        PrintSubtleLines(from + SB_PER_PAGE, SB_PER_PAGE, rx, SB_TOP);
    }
    if (sBook->spread + 1 < sBook->spreadCount)                             // more to come: the corner, lit
    {
        for (i = 0; i < 5; i++)
            Rect(C_MARGIN, TB_RIGHT_X + TB_PAGE_W - 8 + (4 - i), 145 + i, i + 1, 1);
    }
    CopyWindowToVram(WIN_BOOK, COPYWIN_GFX);
}

//  A picture with frames turns over while its spread is open.
static void AnimateSubtlePicture(void)
{
    const struct SubtlePicture *p;

    if (!IsLayerBook() || sBook->view != VIEW_SUBTLE_ENTRY || sBook->spread != 0)
        return;
    p = &Layer()->pictures[Layer()->entries[sBook->entry].picture];
    if (p->frames < 2 || ++sBook->picTimer < SB_ANIM)
        return;
    sBook->picTimer = 0;
    sBook->picFrame++;
    DrawSubtlePicture();
    CopyWindowToVram(WIN_BOOK, COPYWIN_GFX);
}

static void Draw(void);
static void Turn(void);
static void Move(void);

//  ---------------------------------------------------------------- T-509: THE CARDS
//
//  The user, 2026-11-01 (DAEMONS docs/TODO.md T-509, bible 4.39): each layer book ends on a CARDS section, collapsed at
//  first -- the book opens on its cover, and beside it a tile shaped like a tarot deck; DOWN or RIGHT reaches it, A
//  expands the section and B collapses it again. Expanded, THE CARD OF THE DAY lies face down (A turns it) beside the
//  three ways to draw: A SINGLE DRAW, PAST / PRESENT / FUTURE and MIND / BODY / SPIRIT. Choosing one clears the last
//  and asks which deck, the selector read down as a sentence -- THE SUBTLE LAYER's "The Deck of Reason / The Deck of
//  Instinct / A Combined Deck", THE HERMETIC LAYER's "as The Deck Above, so The Deck Below, or Both as One" (Within
//  and Without in the thin half-hour). Then the deck shuffles and spreads, LEFT and RIGHT run along it, A pulls the
//  card at that place, and the deck moves away while the card lands face down on its place on the board. A on a
//  placed card turns it; A on a turned card opens its words, larger; B goes back. The backs never say which deck.
//  A card changes nothing in play, and nothing is saved: the day's card is the hash of the day and the trainer, the
//  same card the companion deals (arcana.ts cardOfDay; daemons_hermetic.c Hermetic_TodayHash), and every draw is
//  forgotten when the book closes.
//
//  THE SCREEN. BG0 is the book's own window, painted as the board; BG1 holds three 72x104 card windows, one per
//  place, each on its own palette row (12-14, the frame's sixteen colours, its pulse on index 14 for the hermetic
//  pair); BG2 is the spread, nothing but a tilemap: the back's 117 tiles once in VRAM, each card in the spread one
//  tile column of it, the last whole -- a fan of 22 is exactly 240px -- and the chosen card lifted a tile. The daemon
//  on a turned card is a sprite at the frame's window (4,14), its sheet decompressed through a buffer freed at once.
//  Everything is on the heap and freed when the section collapses or the book closes. Every word DRAFT.

#define CARD_W          72
#define CARD_H          104
#define CARD_TCOLS      9
#define CARD_TROWS      13
#define CARD_TILES      (CARD_TCOLS * CARD_TROWS)
#define CARD_BYTES      (CARD_TILES * TILE_SIZE_4BPP)
#define CARD_PLACES     3
#define CARD_PAL        12              // BG palette rows 12, 13, 14: one per place
#define CARD_BACK_PAL   11              // the spread's backs
#define CARD_FAN_TILE   601             // BG2 shares BG0's character base, after the book's 600 tiles
#define CARD_FAN_ROW    2
#define CARD_TAG_TILE   0x5090          // the cover's deck tile (sheet and palette)
#define CARD_TAG_PIC    0x5091          // + 0..3: the day's card, and the three places
#define CARD_OTID       0
#define CARD_PERSONALITY 0xFFFF0000     // never a shiny daemon: the card is what shines
#define CARD_INK        12              // the frames' palette roles (gfx/drafts/t509_cards/draw_cards.py)
#define CARD_PULSE      14
#define CARD_BAND_Y     6
#define CARD_PANEL_Y    79
#define CARD_PANEL_H    21
#define CARD_LINE_CHARS 15
#define CR_TEXT_X       88              // the reading page's column: the generator measures every phrase against it
#define CR_TEXT_W       140

enum { CARDS_DAY, CARDS_SINGLE, CARDS_PAST, CARDS_MIND, CARDS_MODES };
enum { DECK_FIRST, DECK_SECOND, DECK_BOTH };
enum { FAN_SHUFFLE, FAN_DEAL, FAN_CHOOSE, FAN_AWAY, FAN_LAND, FAN_RETURN };

struct CardDraw
{
    u8 n;
    u8 side;
    bool8 shiny;
    bool8 used;
    bool8 turned;
    u8 sprite;                  // MAX_SPRITES: none
    const struct ThirteenCard *thirteen;    // Halloween's card of the day, with THE HERMETIC LAYER
};

struct Cards
{
    u8 win[CARD_PLACES];
    u16 *fanMap;
    u8 cursor;                  // the expanded view: 0 the day's card, then the three modes
    u8 mode;
    u8 deck;
    u8 phase, timer;
    u8 place, places;           // the place being filled (the spread), or looked at (the board, the page)
    u8 fanCount, fanCursor, dealt;
    u8 fan[ARCANA_CARDS];
    u8 pulseRows;               // palette rows 12-14 that breathe (bit 0 is row 12)
    bool8 read;                 // the page is the day's card's
    struct CardDraw day;
    struct CardDraw drawn[CARD_PLACES];
};

static const u8 sCardsMine[3]  = { C_NONE, C_INK, C_SHADOW };
static const u8 sCardsLit[3]   = { C_NONE, C_MARGIN, C_SHADOW };
static const u8 sCardsDim[3]   = { C_NONE, C_FAINT, C_NONE };
static const u8 sCardsEmer[3]  = { C_NONE, C_RING, C_SHADOW };

static const u8 *const sCardsModeNames[CARDS_MODES] =
{
    sCardsText_CardOfDay, sCardsText_ModeSingle, sCardsText_ModePast, sCardsText_ModeMind,
};
static const u8 *const sCardsPlaceNames[2][CARD_PLACES] =
{
    { sCardsText_Past, sCardsText_Present, sCardsText_Future },
    { sCardsText_Mind, sCardsText_Body, sCardsText_Spirit },
};

static const struct OamData sCardsOam64 =
{
    .shape = SPRITE_SHAPE(64x64),
    .size = SPRITE_SIZE(64x64),
    .priority = 0,
};

//  The hermetic deck: THE HERMETIC LAYER's, and THE UNITY BOOK's (T-519 item 3) -- the same cards, the same day's card
//  (the "spellcraft" hash and its side bit, as the companion's C-299 deals it), Above and Below.
static bool8 CardsHermetic(void)
{
    return sBook->book == BOOK_HERMETIC || sBook->book == BOOK_UNITY;
}

//  T-519 item 3: THE UNITY BOOK deals Both as One only -- its selector holds that one entry.
static bool8 CardsBothOnly(void)
{
    return sBook->book == BOOK_UNITY;
}

static const u8 sCardsText_UnityTitle[] = _("THE UNITY BOOK");   // DRAFT

//  ---- the board, the card, its letters

//  The board is one 8x8 pattern, so it is built once as a tile and copied over the window.
static void CardsBoard(void)
{
    u32 tile[8];
    u8 x, y, *b = (u8 *)tile, c;
    u16 i;

    for (y = 0; y < 8; y++)
    {
        for (x = 0; x < 8; x += 2)
        {
            c = (((x / 2 + y / 2) % 2 == 0) && ((x + y) % 4 == 0)) ? C_PAPER : C_COVER;
            b[y * 4 + x / 2] = c | ((((((x + 1) / 2 + y / 2) % 2 == 0) && ((x + 1 + y) % 4 == 0)) ? C_PAPER : C_COVER) << 4);
        }
    }
    for (i = 0; i < 30 * 20; i++)
        CpuCopy32(tile, gWindows[WIN_BOOK].tileData + i * TILE_SIZE_4BPP, TILE_SIZE_4BPP);
}

static void PrintBold(const u8 *colors, s16 x, s16 y, const u8 *str)
{
    Print(BR_FONT, colors, x, y, str);
    Print(BR_FONT, colors, x + 1, y, str);
}

static s16 BoldWidth(const u8 *str)
{
    return Width(BR_FONT, str) + 1;     // the whole line struck again a pixel to the right
}

static void Frame(u8 color, s16 x, s16 y, s16 w, s16 h)
{
    Rect(color, x, y, w, 1);
    Rect(color, x, y + h - 1, w, 1);
    Rect(color, x, y, 1, h);
    Rect(color, x + w - 1, y, 1, h);
}

static void CardPixel(u8 *tiles, s16 x, s16 y, u8 c)
{
    u8 *b;

    if ((u16)x >= CARD_W || (u16)y >= CARD_H)
        return;
    b = tiles + ((y / 8) * CARD_TCOLS + x / 8) * TILE_SIZE_4BPP + (y % 8) * 4 + (x % 8) / 2;
    *b = (x & 1) ? ((*b & 0x0F) | (c << 4)) : ((*b & 0xF0) | c);
}

//  LEN characters of S in the frames' 3x5 letters (lower case drawn as upper), from X.
static void CardLetters(u8 *tiles, const u8 *s, u8 len, s16 x, s16 y)
{
    u8 i, r, k;
    u16 g;

    for (i = 0; i < len && s[i] != EOS; i++)
    {
        g = sCardGlyphs[s[i]];
        for (r = 0; r < 5; r++)
            for (k = 0; k < 3; k++)
                if (g & (1 << (14 - r * 3 - k)))
                    CardPixel(tiles, x + i * 4 + k, y + r, CARD_INK);
    }
}

static void CardLettersCentred(u8 *tiles, const u8 *s, u8 len, s16 y)
{
    CardLetters(tiles, s, len, CARD_W / 2 - (len * 4 - 1) / 2, y);
}

//  The phrase, wrapped to the panel: at most three lines of fifteen (tools/port_arcana.py checks every face fits).
static void CardPhrase(u8 *tiles, const u8 *s)
{
    const u8 *start[3];
    u8 len[3], n = 0, i, w;
    const u8 *p = s, *line = s, *lastSpace = NULL;

    while (*line != EOS && n < 3)
    {
        lastSpace = NULL;
        for (p = line, w = 0; *p != EOS && w <= CARD_LINE_CHARS; p++, w++)
            if (*p == CHAR_SPACE)
                lastSpace = p;
        if (*p == EOS && w <= CARD_LINE_CHARS)
        {
            start[n] = line;
            len[n++] = w;
            break;
        }
        if (lastSpace == NULL)
            lastSpace = line + CARD_LINE_CHARS;
        start[n] = line;
        len[n++] = lastSpace - line;
        line = (*lastSpace == CHAR_SPACE) ? lastSpace + 1 : lastSpace;
    }
    for (i = 0; i < n; i++)
        CardLettersCentred(tiles, start[i], len[i], CARD_PANEL_Y + (CARD_PANEL_H - (n * 6 - 1)) / 2 + i * 6);
}

static const u8 *CardKeyword(const struct CardDraw *d)
{
    return d->thirteen != NULL ? d->thirteen->keyword : sArcana[d->n].face[d->side].keyword;
}

static const u8 *CardWords(const struct CardDraw *d)
{
    return d->thirteen != NULL ? d->thirteen->phrase : sArcana[d->n].face[d->side].phrase;
}

static const u8 *CardRoman(const struct CardDraw *d)
{
    return d->thirteen != NULL ? d->thirteen->n : sArcana[d->n].roman;
}

static u16 CardSpecies(const struct CardDraw *d)
{
    u16 s;

    if (d->thirteen == NULL)
        return sArcana[d->n].face[d->side].species;
    for (s = 1; s < NUM_SPECIES; s++)
        if (StringCompare(gSpeciesNames[s], d->thirteen->species) == 0)
            return s;
    return SPECIES_NONE;
}

//  A card into place K's window: its back, or its face -- the frame, the numeral and keyword in the band (the
//  keyword alone when both will not fit), the phrase in the panel. Its palette goes into the place's row.
static void CardRender(u8 k, const struct CardDraw *d, bool8 face)
{
    u8 win = sBook->cards->win[k], *tiles = gWindows[win].tileData, band[24], *p;
    u8 len;

    if (!face)
    {
        CpuCopy32(sCardBack, tiles, CARD_BYTES);
        LoadPalette(sCardBackPal, BG_PLTT_ID(CARD_PAL + k), PLTT_SIZE_4BPP);
        sBook->cards->pulseRows &= ~(1 << k);
    }
    else
    {
        CpuCopy32(sCardFrames[d->side][d->shiny], tiles, CARD_BYTES);
        LoadPalette(sCardPals[d->side][d->shiny], BG_PLTT_ID(CARD_PAL + k), PLTT_SIZE_4BPP);
        if (d->side >= CARD_ABOVE)
            sBook->cards->pulseRows |= 1 << k;
        else
            sBook->cards->pulseRows &= ~(1 << k);
        p = StringCopy(band, CardRoman(d));
        *p++ = CHAR_SPACE;
        StringCopy(p, CardKeyword(d));
        len = StringLength(band);
        if (len * 4 - 1 > 60)
            CardLettersCentred(tiles, CardKeyword(d), StringLength(CardKeyword(d)), CARD_BAND_Y);
        else
            CardLettersCentred(tiles, band, len, CARD_BAND_Y);
        CardPhrase(tiles, CardWords(d));
    }
    CopyWindowToVram(win, COPYWIN_GFX);
}

//  ---- the daemons

static void CardPicFree(struct CardDraw *d, u16 tag)
{
    u16 palTag;
    u8 i;
    struct CardDraw *other;

    if (d->sprite == MAX_SPRITES)
        return;
    palTag = gSprites[d->sprite].template->paletteTag;
    DestroySprite(&gSprites[d->sprite]);
    FreeSpriteTilesByTag(tag);
    d->sprite = MAX_SPRITES;
    for (i = 0; i <= CARD_PLACES; i++)       // another card's daemon may share the palette
    {
        other = (i == CARD_PLACES) ? &sBook->cards->day : &sBook->cards->drawn[i];
        if (other->sprite != MAX_SPRITES && gSprites[other->sprite].template->paletteTag == palTag)
            return;
    }
    FreeSpritePaletteByTag(palTag);
}

static void CardPicMake(struct CardDraw *d, u16 tag)
{
    struct SpriteSheet sheet;
    struct SpriteTemplate *t;
    const struct CompressedSpritePalette *pal;
    u16 species = CardSpecies(d);
    u8 *buf;

    if (d->sprite != MAX_SPRITES || species == SPECIES_NONE || species >= NUM_SPECIES)
        return;
    buf = Alloc(MON_PIC_SIZE);
    if (buf == NULL)
        return;
    LoadSpecialPokePic(&gMonFrontPicTable[species], buf, species, CARD_PERSONALITY, TRUE);
    sheet.data = buf;
    sheet.size = MON_PIC_SIZE;
    sheet.tag = tag;
    LoadSpriteSheet(&sheet);
    Free(buf);
    pal = GetMonSpritePalStructFromOtIdPersonality(species, CARD_OTID, CARD_PERSONALITY);
    LoadCompressedSpritePalette(pal);
    t = Alloc(sizeof(*t));          // the template outlives this call: the sprite keeps a pointer to it
    if (t == NULL)
        return;
    t->tileTag = tag;
    t->paletteTag = pal->tag;
    t->oam = &sCardsOam64;
    t->anims = gDummySpriteAnimTable;
    t->images = NULL;
    t->affineAnims = gDummySpriteAffineAnimTable;
    t->callback = SpriteCallbackDummy;
    d->sprite = CreateSprite(t, 0, 0, 0);
    if (d->sprite == MAX_SPRITES)
        Free(t);
    else
        gSprites[d->sprite].invisible = TRUE;
}

static void CardPicFreeAll(void)
{
    u8 k;
    struct SpriteTemplate *t;

    for (k = 0; k <= CARD_PLACES; k++)
    {
        struct CardDraw *d = (k == 0) ? &sBook->cards->day : &sBook->cards->drawn[k - 1];

        if (d->sprite == MAX_SPRITES)
            continue;
        t = (struct SpriteTemplate *)gSprites[d->sprite].template;
        CardPicFree(d, CARD_TAG_PIC + k);
        Free(t);
    }
}

static void CardPicShow(struct CardDraw *d, s16 x, s16 y)
{
    if (d->sprite == MAX_SPRITES)
        return;
    gSprites[d->sprite].x = x + 32;
    gSprites[d->sprite].y = y + 32;
    gSprites[d->sprite].invisible = FALSE;
}

static void CardPicsHide(void)
{
    u8 k;

    if (sBook->cards->day.sprite != MAX_SPRITES)
        gSprites[sBook->cards->day.sprite].invisible = TRUE;
    for (k = 0; k < CARD_PLACES; k++)
        if (sBook->cards->drawn[k].sprite != MAX_SPRITES)
            gSprites[sBook->cards->drawn[k].sprite].invisible = TRUE;
}

//  ---- the cover's tile

static void CardsTileShow(bool8 show)
{
    u8 i;

    for (i = 0; i < MAX_SPRITES; i++)
    {
        if (gSprites[i].inUse && gSprites[i].template->tileTag == CARD_TAG_TILE)
        {
            gSprites[i].invisible = !show;
            return;
        }
    }
}

static const struct SpriteTemplate sCardsTileTemplate =
{
    .tileTag = CARD_TAG_TILE,
    .paletteTag = CARD_TAG_TILE,
    .oam = &sCardsOam64,
    .anims = gDummySpriteAnimTable,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCallbackDummy,
};

#define CT_X 196                // the deck tile, right of the cover: three backs, 40x56 at half size
#define CT_Y 86

static void CardsTileMake(void)
{
    static const struct SpriteSheet sheet = { sCardDeckTile, 64 * 64 / 2, CARD_TAG_TILE };
    static const struct SpritePalette pal = { sCardBackPal, CARD_TAG_TILE };

    LoadSpriteSheet(&sheet);
    LoadSpritePalette(&pal);
    CreateSprite(&sCardsTileTemplate, CT_X + 32, CT_Y + 32, 0);
}

//  The cover, and the tile's label under it; lit when the cursor is on it.
static void DrawCardsTile(void)
{
    s16 lw = Width(BR_SMALL, sCardsText_CardsTile);

    if (sBook->cardsFocus)
        Frame(C_MARGIN, CT_X - 1, CT_Y - 1, 44, 60);
    Print(BR_SMALL, sBook->cardsFocus ? sCardsLit : sCardsDim, CT_X + 21 - lw / 2, CT_Y + 59, sCardsText_CardsTile);
}

//  ---- the section opened and closed

static void CardsBgs(bool8 cards)
{
    if (cards)
    {
        SetBgAttribute(0, BG_ATTR_PRIORITY, 2);
        ShowBg(0);                  // SetBgAttribute keeps the value; ShowBg writes BG0CNT with it
        ShowBg(1);
        ShowBg(2);
    }
    else
    {
        HideBg(1);
        HideBg(2);
        SetBgAttribute(0, BG_ATTR_PRIORITY, 0);
        ShowBg(0);
    }
}

static bool8 CardsOpen(void)
{
    struct Cards *c;
    struct WindowTemplate t = { .bg = 1, .tilemapTop = CARD_FAN_ROW, .width = CARD_TCOLS, .height = CARD_TROWS };
    u8 k;
    u32 h;

    c = AllocZeroed(sizeof(*c));
    if (c == NULL)
        return FALSE;
    sBook->cards = c;
    c->fanMap = AllocZeroed(BG_SCREEN_SIZE);
    for (k = 0; k < CARD_PLACES; k++)
    {
        t.tilemapLeft = 10 * k;
        t.paletteNum = CARD_PAL + k;
        t.baseBlock = 1 + CARD_TILES * k;
        c->win[k] = AddWindow(&t);
        c->drawn[k].sprite = MAX_SPRITES;
    }
    c->day.sprite = MAX_SPRITES;
    if (c->fanMap == NULL || c->win[0] == WINDOW_NONE || c->win[1] == WINDOW_NONE || c->win[2] == WINDOW_NONE)
        return FALSE;           // what was made is freed with the rest
    SetBgTilemapBuffer(2, c->fanMap);
    LoadBgTiles(2, sCardBack, CARD_BYTES, CARD_FAN_TILE);
    LoadPalette(sCardBackPal, BG_PLTT_ID(CARD_BACK_PAL), PLTT_SIZE_4BPP);
    // the day's card: the companion's cardOfDay (arcana.ts; THE HERMETIC LAYER's rule agreed with it, C-273), or on
    // Halloween with THE HERMETIC LAYER the Book of Thirteen's card instead
    c->day.used = TRUE;
    if (CardsHermetic() && (c->day.thirteen = Hermetic_CardOfDay()) != NULL)
    {
        c->day.side = CARD_ABOVE;
    }
    else
    {
        h = Hermetic_TodayHash(CardsHermetic() ? "spellcraft" : "arcana");
        c->day.n = h % ARCANA_CARDS;
        c->day.side = ((h >> 8) % 2) ? (CardsHermetic() ? CARD_ABOVE : CARD_INSTINCT)
                                     : (CardsHermetic() ? CARD_BELOW : CARD_REASON);
        c->day.shiny = ((h >> 12) % 16) == 0;
    }
    return TRUE;
}

static void CardsClose(void)
{
    struct Cards *c = sBook->cards;
    u8 k;

    if (c == NULL)
        return;
    CardPicFreeAll();
    for (k = 0; k < CARD_PLACES; k++)
        if (c->win[k] != WINDOW_NONE && c->win[k] != 0)
            RemoveWindow(c->win[k]);
    if (c->fanMap != NULL)
    {
        SetBgTilemapBuffer(2, NULL);
        Free(c->fanMap);
    }
    FREE_AND_SET_NULL(sBook->cards);
    ChangeBgX(1, 0, BG_COORD_SET);
    ChangeBgY(1, 0, BG_COORD_SET);
    CardsBgs(FALSE);
}

//  Which windows show, and where BG1 puts them: the windows sit at x 0, 80 and 160, y 16.
static void CardsPlaceWindows(u8 mask, s16 dx, s16 dy)
{
    u8 k;

    for (k = 0; k < CARD_PLACES; k++)
    {
        if (mask & (1 << k))
            PutWindowTilemap(sBook->cards->win[k]);
        else
            ClearWindowTilemap(sBook->cards->win[k]);
    }
    CopyBgTilemapBufferToVram(1);
    ChangeBgX(1, (u32)(-dx) << 8, BG_COORD_SET);
    ChangeBgY(1, (u32)(CARD_FAN_ROW * 8 - dy) << 8, BG_COORD_SET);
}

//  ---- the spread: the backs as tile columns on BG2

static void FanColumn(u8 x, u8 col, u8 top)
{
    u8 r;

    for (r = 0; r < CARD_TROWS; r++)
        sBook->cards->fanMap[(top + r) * 32 + x] = (CARD_FAN_TILE + r * CARD_TCOLS + col) | (CARD_BACK_PAL << 12);
}

//  DEALT cards laid out from the left, the rest still stacked on the last of them; the chosen card lifted a tile and
//  shown whole over its neighbours, whose feet still show under it.
static void DrawFan(bool8 lift)
{
    struct Cards *c = sBook->cards;
    u8 n = c->fanCount, width = n + CARD_TCOLS - 1, x0 = (30 - width) / 2, k, col, x;

    CpuFill16(0, c->fanMap, BG_SCREEN_SIZE);
    for (k = 0; k < n; k++)
    {
        x = x0 + (k < c->dealt ? k : c->dealt);
        if (k + 1 < n && k < c->dealt)
        {
            FanColumn(x, 0, CARD_FAN_ROW);
        }
        else
        {
            for (col = 0; col < CARD_TCOLS; col++)
                FanColumn(x + col, col, CARD_FAN_ROW);
            break;                                  // the last, or the undealt rest under it
        }
    }
    if (lift && n != 0)
    {
        x = x0 + c->fanCursor;
        c->fanMap[(CARD_FAN_ROW + CARD_TROWS - 1) * 32 + x] = c->fanCursor == 0 ? 0     // what it uncovers
            : (CARD_FAN_TILE + (CARD_TROWS - 1) * CARD_TCOLS + 1) | (CARD_BACK_PAL << 12);
        for (col = 0; col < CARD_TCOLS; col++)
            FanColumn(x + col, col, CARD_FAN_ROW - 1);
        for (col = 1; col < CARD_TCOLS && c->fanCursor + 1 == n; col++)   // the last card: nothing under it
            c->fanMap[(CARD_FAN_ROW + CARD_TROWS - 1) * 32 + x + col] = 0;
    }
    ChangeBgX(2, ((30 - width) % 2) ? (u32)(-4) << 8 : 0, BG_COORD_SET);
    CopyBgTilemapBufferToVram(2);
}

static void CardsShuffle(void)
{
    struct Cards *c = sBook->cards;
    u8 k, j, t;

    for (k = 0; k < ARCANA_CARDS; k++)
        c->fan[k] = k;
    for (k = ARCANA_CARDS - 1; k > 0; k--)
    {
        j = Random() % (k + 1);
        t = c->fan[k];
        c->fan[k] = c->fan[j];
        c->fan[j] = t;
    }
    c->fanCount = ARCANA_CARDS;
}

//  The card at the cursor comes out of the spread: its number from the shuffle, its face from the deck chosen.
static void CardsPull(void)
{
    struct Cards *c = sBook->cards;
    struct CardDraw *d = &c->drawn[c->place];
    u8 k;

    d->n = c->fan[c->fanCursor];
    if (c->deck == DECK_BOTH)
        d->side = Random() % 2;
    else
        d->side = c->deck;
    if (CardsHermetic())
        d->side += CARD_ABOVE;
    d->shiny = (Random() % 16) == 0;
    d->used = TRUE;
    d->turned = FALSE;
    d->thirteen = NULL;
    for (k = c->fanCursor; k + 1 < c->fanCount; k++)
        c->fan[k] = c->fan[k + 1];
    c->fanCount--;
    if (c->fanCursor >= c->fanCount)
        c->fanCursor = c->fanCount - 1;
}

//  ---- the views

//  The place each card window shows at on the board: one card centred, or three in a row.
static s16 PlaceX(u8 k)
{
    return 4 + 80 * k;
}
#define PLACE_Y 15

static u8 PlaceOf(u8 i)                 // the i-th place's window: a single draw lies in the middle
{
    return sBook->cards->places == 1 ? 1 : i;
}

static void DrawCards(void)
{
    struct Cards *c = sBook->cards;
    u8 k;
    s16 y;

    CardsBgs(TRUE);
    CardPicsHide();
    CpuFill16(0, c->fanMap, BG_SCREEN_SIZE);
    CopyBgTilemapBufferToVram(2);
    ChangeBgX(2, 0, BG_COORD_SET);
    ChangeBgY(2, 0, BG_COORD_SET);
    CardsBoard();
    Print(BR_SMALL, sCardsMine, 48 - Width(BR_SMALL, sCardsText_CardOfDay) / 2, 5, sCardsText_CardOfDay);
    if (c->cursor == CARDS_DAY)
        Frame(C_MARGIN, 10, 18, CARD_W + 4, CARD_H + 4);
    for (k = CARDS_SINGLE; k < CARDS_MODES; k++)
    {
        y = 22 + (k - 1) * 28;
        if (k == c->cursor)
            Rect(C_HIGHLIGHT, 94, y - 2, 142, 18);
        Print(BR_FONT, k == c->cursor ? sCardsLit : sCardsMine, 98, y, sCardsModeNames[k]);
    }
    if (c->cursor == CARDS_DAY)
        Print(BR_SMALL, sCardsDim, 120 - Width(BR_SMALL, sCardsText_TurnBack) / 2, 146, sCardsText_TurnBack);
    else
        Print(BR_SMALL, sCardsDim, 120 - Width(BR_SMALL, sCardsText_Draw) / 2, 146, sCardsText_Draw);
    CopyWindowToVram(WIN_BOOK, COPYWIN_GFX);
    CardRender(0, &c->day, c->day.turned);
    CardsPlaceWindows(1, 12, 20);
    if (c->day.turned)
    {
        CardPicMake(&c->day, CARD_TAG_PIC);
        CardPicShow(&c->day, 12 + 4, 20 + 14);
    }
}

//  The selector, read down as the sentence it is: the titles struck twice and selectable, the words between plain.
static void DrawCardsSelect(void)
{
    struct Cards *c = sBook->cards;
    const u8 *titles[3], *between[3] = { NULL, NULL, NULL };
    const u8 *book = CardsBothOnly() ? sCardsText_UnityTitle : CardsHermetic() ? sCardsText_HermeticTitle : sCardsText_SubtleTitle;
    const u8 *ink = CardsHermetic() ? sCardsEmer : sCardsMine;
    u8 k, row = 0;
    s16 y, gap;

    CardsBgs(TRUE);
    CardPicsHide();
    CpuFill16(0, c->fanMap, BG_SCREEN_SIZE);
    CopyBgTilemapBufferToVram(2);
    CardsBoard();
    if (CardsHermetic())
    {
        bool8 thin = Spellbook_HalfHour();

        titles[0] = thin ? sCardsText_DeckWithin : sCardsText_DeckAbove;
        titles[1] = thin ? sCardsText_DeckWithout : sCardsText_DeckBelow;
        titles[2] = sCardsText_DeckBoth;
        between[0] = sCardsText_As;
        between[1] = sCardsText_So;
        between[2] = sCardsText_Or;
    }
    else
    {
        titles[0] = sCardsText_DeckReason;
        titles[1] = sCardsText_DeckInstinct;
        titles[2] = sCardsText_DeckCombined;
    }
    gap = CardsHermetic() ? 16 : 24;
    Rect(C_RULE, 86, 3, 151, CardsHermetic() ? 108 : 84);              // the menu's box
    Rect(C_PAPER, 88, 5, 147, CardsHermetic() ? 104 : 80);
    for (k = CardsBothOnly() ? DECK_BOTH : 0; k < 3; k++)
    {
        if (between[k] != NULL && !CardsBothOnly())
        {
            Print(BR_FONT, sCardsDim, 94, 6 + row * gap, between[k]);
            row++;
        }
        y = 6 + row * gap;
        if (k == c->deck)
        {
            Rect(ink[1], 92, y + 4, 2, 7);
            Rect(ink[1], 94, y + 5, 1, 5);
            Rect(ink[1], 95, y + 6, 1, 3);
            Rect(ink[1], 96, y + 7, 1, 1);
        }
        PrintBold(ink, 100, y, titles[k]);
        if (between[k] != NULL && k < 2 && !CardsBothOnly())   // the comma after a title is one of the plain words
            Print(BR_FONT, sCardsDim, 100 + BoldWidth(titles[k]), y, sCardsText_Comma);
        row++;
    }
    Rect(C_FAINT, 8, 3, CARD_W, CARD_H);                                // the deck: two more backs under the one shown
    Rect(C_RULE, 6, 5, CARD_W, CARD_H);
    Rect(C_RULE, 8, 120, 224, 32);                                      // and the book it is
    Rect(C_PAPER, 10, 122, 220, 28);
    PrintBold(ink, 120 - BoldWidth(book) / 2, 129, book);
    CopyWindowToVram(WIN_BOOK, COPYWIN_GFX);
    CardRender(0, NULL, FALSE);
    CardsPlaceWindows(1, 4, 7);
}

static void DrawCardsSpread(void)
{
    struct Cards *c = sBook->cards;
    const u8 *label;

    CardsBgs(TRUE);
    CardPicsHide();
    CardsBoard();
    if (c->places == CARD_PLACES)
    {
        label = sCardsPlaceNames[c->mode == CARDS_MIND][c->place];
        Print(BR_SMALL, sCardsLit, 120 - Width(BR_SMALL, label) / 2, 0, label);
    }
    if (c->phase == FAN_CHOOSE)
        Print(BR_SMALL, sCardsDim, 120 - Width(BR_SMALL, sCardsText_Draw) / 2, 146, sCardsText_Draw);
    CopyWindowToVram(WIN_BOOK, COPYWIN_GFX);
    CardsPlaceWindows(0, 4, PLACE_Y);
    DrawFan(c->phase == FAN_CHOOSE);
}

static void DrawCardsBoard(void)
{
    struct Cards *c = sBook->cards;
    u8 i, k, mask = 0;
    s16 px;
    const u8 *label;

    CardsBgs(TRUE);
    CardPicsHide();
    CpuFill16(0, c->fanMap, BG_SCREEN_SIZE);
    CopyBgTilemapBufferToVram(2);
    ChangeBgY(2, 0, BG_COORD_SET);
    CardsBoard();
    for (i = 0; i < c->places; i++)
    {
        k = PlaceOf(i);
        px = PlaceX(k) - 3;
        Frame(i == c->place && c->phase != FAN_LAND ? C_MARGIN : C_RULE, px, PLACE_Y - 3, 78, 110);
        Rect(C_FAINT, px, PLACE_Y - 3, 1, 1);
        if (c->places == CARD_PLACES)
        {
            label = sCardsPlaceNames[c->mode == CARDS_MIND][i];
            Print(BR_SMALL, i == c->place && c->phase != FAN_LAND ? sCardsLit : sCardsMine,
                  px + 39 - Width(BR_SMALL, label) / 2, 126, label);
        }
        if (c->drawn[i].used)
        {
            mask |= 1 << k;
            CardRender(k, &c->drawn[i], c->drawn[i].turned);
            if (c->drawn[i].turned)
            {
                CardPicMake(&c->drawn[i], CARD_TAG_PIC + 1 + i);
                CardPicShow(&c->drawn[i], PlaceX(k) + 4, PLACE_Y + 14);
            }
        }
    }
    if (c->phase != FAN_LAND)
        Print(BR_SMALL, sCardsDim, 120 - Width(BR_SMALL, sCardsText_TurnBack) / 2, 146, sCardsText_TurnBack);
    CopyWindowToVram(WIN_BOOK, COPYWIN_GFX);
    CardsPlaceWindows(mask, 4, PLACE_Y);
}

//  A turned card's words, larger: the daemon on the left, the numeral and keyword, the deck, the phrase.
static void DrawCardsRead(void)
{
    struct Cards *c = sBook->cards;
    struct CardDraw *d = c->read ? &c->day : &c->drawn[c->place];
    const u8 *side;
    bool8 thin = CardsHermetic() && Spellbook_HalfHour();
    u8 buf[24], *p, n, i;
    s16 cx;

    CardsBgs(TRUE);
    CardPicsHide();
    CardsPlaceWindows(0, 0, 0);
    CpuFill16(0, c->fanMap, BG_SCREEN_SIZE);
    CopyBgTilemapBufferToVram(2);
    CardsBoard();
    Rect(C_RULE, 4, 4, 232, 134);
    Rect(C_PAPER, 6, 6, 228, 130);
    CardPicShow(d, 14, 30);
    p = StringCopy(buf, CardRoman(d));
    *p++ = CHAR_SPACE;
    StringCopy(p, CardKeyword(d));
    PrintBold(CardsHermetic() ? sCardsEmer : sCardsMine, CR_TEXT_X, 14, BoldWidth(buf) <= CR_TEXT_W ? buf : CardKeyword(d));
    side = d->thirteen != NULL ? sText_Thirteen
         : d->side == CARD_REASON ? sCardsText_SideReason
         : d->side == CARD_INSTINCT ? sCardsText_SideInstinct
         : d->side == CARD_ABOVE ? (thin ? sCardsText_SideWithin : sCardsText_SideAbove)
         : (thin ? sCardsText_SideWithout : sCardsText_SideBelow);
    Print(BR_SMALL, sCardsDim, CR_TEXT_X, 32, side);
    if (d->shiny)
        Print(BR_SMALL, sCardsLit, CR_TEXT_X + Width(BR_SMALL, side) + 6, 32, sCardsText_Shiny);
    Rect(C_RULE, CR_TEXT_X, 46, CR_TEXT_W, 1);
    n = Reflow(CardWords(d), CR_TEXT_W, BR_FONT);
    for (i = 0; i < n && i < 4; i++)
        Print(BR_FONT, sCardsMine, CR_TEXT_X, 54 + i * 14, Line(i));
    if (CardSpecies(d) != SPECIES_NONE)
        Print(BR_SMALL, sCardsDim, 46 - Width(BR_SMALL, gSpeciesNames[CardSpecies(d)]) / 2, 112,
              gSpeciesNames[CardSpecies(d)]);
    if (c->read)
    {
        Print(BR_SMALL, sCardsLit, 120 - Width(BR_SMALL, sCardsText_CardOfDay) / 2, 144, sCardsText_CardOfDay);
    }
    else if (c->places == CARD_PLACES)
    {
        for (i = 0; i < CARD_PLACES; i++)
        {
            cx = 40 + 80 * i;
            Print(BR_SMALL, i == c->place ? sCardsLit : sCardsDim,
                  cx - Width(BR_SMALL, sCardsPlaceNames[c->mode == CARDS_MIND][i]) / 2, 144,
                  sCardsPlaceNames[c->mode == CARDS_MIND][i]);
        }
    }
    CopyWindowToVram(WIN_BOOK, COPYWIN_GFX);
}

//  ---- input

static void CardsCollapse(void)
{
    CardsClose();
    sBook->cardsFocus = TRUE;
    sBook->view = VIEW_COVER;
    Turn();
}

static void CardsClear(void)
{
    struct Cards *c = sBook->cards;
    u8 k;
    struct SpriteTemplate *t;

    for (k = 0; k < CARD_PLACES; k++)
    {
        if (c->drawn[k].sprite != MAX_SPRITES)
        {
            t = (struct SpriteTemplate *)gSprites[c->drawn[k].sprite].template;
            CardPicFree(&c->drawn[k], CARD_TAG_PIC + 1 + k);
            Free(t);
        }
        c->drawn[k].used = FALSE;
        c->drawn[k].turned = FALSE;
    }
}

static void Input_Cards(void)
{
    struct Cards *c = sBook->cards;

    if (JOY_NEW(B_BUTTON))
    {
        CardsCollapse();
    }
    else if (JOY_NEW(DPAD_UP | DPAD_LEFT) && c->cursor != 0)
    {
        c->cursor--;
        Move();
    }
    else if (JOY_NEW(DPAD_DOWN | DPAD_RIGHT) && c->cursor + 1 < CARDS_MODES)
    {
        c->cursor++;
        Move();
    }
    else if (JOY_NEW(A_BUTTON) && c->cursor == CARDS_DAY)
    {
        if (!c->day.turned)
        {
            c->day.turned = TRUE;
            PlaySE(SE_BALL_TRAY_EXIT);
            Draw();
        }
        else
        {
            c->read = TRUE;
            sBook->view = VIEW_CARDS_READ;
            Turn();
        }
    }
    else if (JOY_NEW(A_BUTTON))
    {
        CardsClear();                       // a new way to draw clears the last
        c->mode = c->cursor;
        c->places = (c->mode == CARDS_SINGLE) ? 1 : CARD_PLACES;
        c->deck = CardsBothOnly() ? DECK_BOTH : DECK_FIRST;
        sBook->view = VIEW_CARDS_SELECT;
        Turn();
    }
}

static void Input_CardsSelect(void)
{
    struct Cards *c = sBook->cards;

    if (JOY_NEW(B_BUTTON))
    {
        sBook->view = VIEW_CARDS;
        Turn();
    }
    else if (JOY_NEW(DPAD_UP | DPAD_LEFT) && c->deck != 0 && !CardsBothOnly())
    {
        c->deck--;
        Move();
    }
    else if (JOY_NEW(DPAD_DOWN | DPAD_RIGHT) && c->deck < DECK_BOTH)
    {
        c->deck++;
        Move();
    }
    else if (JOY_NEW(A_BUTTON))
    {
        CardsShuffle();
        c->place = 0;
        c->phase = FAN_SHUFFLE;
        c->timer = 0;
        c->dealt = 0;
        c->fanCursor = ARCANA_CARDS / 2;
        sBook->view = VIEW_CARDS_SPREAD;
        PlaySE(SE_SELECT);
        Draw();
        ChangeBgY(2, 0, BG_COORD_SET);      // BG2's scroll is still the field's until it is set
    }
}

//  The spread runs on frames: the shuffle (the stacked deck jostled), the deal (a card a frame), the choice, then
//  the deck away up the screen, the card landing on the board, and the deck back for the next place.
static void Input_CardsSpread(void)
{
    struct Cards *c = sBook->cards;

    switch (c->phase)
    {
    case FAN_SHUFFLE:
        if (c->timer == 0)
            DrawCardsSpread();
        ChangeBgX(2, (u32)((c->timer & 4) ? 3 : -3) << 8, BG_COORD_SET);
        if (c->timer % 8 == 0)
            PlaySE(SE_BALL_TRAY_EXIT);
        if (++c->timer >= 48)
        {
            ChangeBgX(2, 0, BG_COORD_SET);
            c->phase = FAN_DEAL;
            c->timer = 0;
        }
        break;
    case FAN_DEAL:
        if (++c->timer % 3 == 0)
        {
            c->dealt++;
            DrawFan(FALSE);
            if (c->dealt + 1 >= c->fanCount)
            {
                c->dealt = c->fanCount;
                c->phase = FAN_CHOOSE;
                if (c->fanCursor >= c->fanCount)
                    c->fanCursor = c->fanCount - 1;
                DrawCardsSpread();
            }
        }
        break;
    case FAN_CHOOSE:
        if (JOY_NEW(B_BUTTON))
        {
            ChangeBgX(2, 0, BG_COORD_SET);
            CardsClear();
            sBook->view = VIEW_CARDS;
            Turn();
        }
        else if (JOY_REPT(DPAD_LEFT) && c->fanCursor != 0)
        {
            c->fanCursor--;
            PlaySE(SE_SELECT);
            DrawFan(TRUE);
        }
        else if (JOY_REPT(DPAD_RIGHT) && c->fanCursor + 1 < c->fanCount)
        {
            c->fanCursor++;
            PlaySE(SE_SELECT);
            DrawFan(TRUE);
        }
        else if (JOY_NEW(A_BUTTON))
        {
            CardsPull();
            PlaySE(SE_BALL_TRAY_EXIT);
            c->dealt = c->fanCount;
            DrawFan(FALSE);
            c->phase = FAN_AWAY;
            c->timer = 0;
        }
        break;
    case FAN_AWAY:
        c->timer++;
        ChangeBgY(2, (u32)(c->timer * 10) << 8, BG_COORD_SET);
        if (c->timer >= 14)
        {
            c->phase = FAN_LAND;
            c->timer = 0;
            DrawCardsBoard();
            PlaySE(SE_BALL_TRAY_ENTER);
        }
        break;
    case FAN_LAND:
        if (++c->timer < 30)
            break;
        c->timer = 0;
        if (++c->place < c->places)
        {
            c->phase = FAN_RETURN;
            c->dealt = c->fanCount;
            ChangeBgY(2, (u32)(-140) << 8, BG_COORD_SET);
            DrawCardsSpread();
        }
        else
        {
            c->place = 0;
            c->phase = FAN_CHOOSE;
            sBook->view = VIEW_CARDS_BOARD;
            DrawCardsBoard();
        }
        break;
    case FAN_RETURN:
        c->timer++;
        ChangeBgY(2, (u32)(-140 + c->timer * 10) << 8, BG_COORD_SET);
        if (c->timer >= 14)
        {
            ChangeBgY(2, 0, BG_COORD_SET);
            c->phase = FAN_CHOOSE;
            DrawCardsSpread();
        }
        break;
    }
}

static void Input_CardsBoard(void)
{
    struct Cards *c = sBook->cards;
    struct CardDraw *d = &c->drawn[c->place];

    if (JOY_NEW(B_BUTTON))
    {
        sBook->view = VIEW_CARDS;
        Turn();
    }
    else if (JOY_NEW(DPAD_LEFT) && c->place != 0)
    {
        c->place--;
        Move();
    }
    else if (JOY_NEW(DPAD_RIGHT) && c->place + 1 < c->places)
    {
        c->place++;
        Move();
    }
    else if (JOY_NEW(A_BUTTON) && !d->turned)
    {
        d->turned = TRUE;
        PlaySE(SE_BALL_TRAY_EXIT);
        Draw();
    }
    else if (JOY_NEW(A_BUTTON))
    {
        c->read = FALSE;
        sBook->view = VIEW_CARDS_READ;
        Turn();
    }
}

static void Input_CardsRead(void)
{
    struct Cards *c = sBook->cards;

    if (JOY_NEW(B_BUTTON | A_BUTTON))
    {
        sBook->view = c->read ? VIEW_CARDS : VIEW_CARDS_BOARD;
        Turn();
    }
    else if (!c->read && JOY_NEW(DPAD_LEFT) && c->place != 0 && c->drawn[c->place - 1].turned)
    {
        c->place--;
        Turn();
    }
    else if (!c->read && JOY_NEW(DPAD_RIGHT) && c->place + 1 < c->places && c->drawn[c->place + 1].turned)
    {
        c->place++;
        Turn();
    }
}

//  The hermetic pair's frames breathe on index 14, with the book.
static void BreatheCards(u8 phase)
{
    u8 k;
    u16 colour;

    if (sBook->cards == NULL || sBook->cards->pulseRows == 0)
        return;
    colour = Pulse_Colour(phase);
    for (k = 0; k < CARD_PLACES; k++)
        if (sBook->cards->pulseRows & (1 << k))
            gPlttBufferUnfaded[BG_PLTT_ID(CARD_PAL + k) + CARD_PULSE] = gPlttBufferFaded[BG_PLTT_ID(CARD_PAL + k) + CARD_PULSE] = colour;
}

static void Draw(void)
{
    switch (sBook->view)
    {
    case VIEW_PAGE:           DrawNotebook();      break;
    case VIEW_CONTENTS:       DrawContents();      break;
    case VIEW_CHAPTER:        DrawChapter();       break;
    case VIEW_TOPIC:          DrawTopic();         break;
    case VIEW_COVER:          if (IsLayerBook()) DrawSubtleCover(); else DrawGuideCover(); break;
    case VIEW_CARDS:           DrawCards();          break;
    case VIEW_CARDS_SELECT:    DrawCardsSelect();    break;
    case VIEW_CARDS_SPREAD:    DrawCardsSpread();    break;
    case VIEW_CARDS_BOARD:     DrawCardsBoard();     break;
    case VIEW_CARDS_READ:      DrawCardsRead();      break;
    case VIEW_SUBTLE_CONTENTS: DrawSubtleContents(); break;
    case VIEW_SUBTLE_ENTRY:    DrawSubtleEntry();    break;
    case VIEW_THIRTEEN:        DrawThirteen();       break;
    case VIEW_GUIDE_CONTENTS: DrawGuideContents(); break;
    case VIEW_GUIDE_ENTRY:    DrawGuideEntry();    break;
    }
    if (IsLayerBook() && HasCards())
        CardsTileShow(sBook->view == VIEW_COVER);
    LensPalette();                      // T-529
}

//  ---------------------------------------------------------------- input

static void Leave(void)
{
    PlaySE(SE_SELECT);
    sBook->leaving = TRUE;
    BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 16, RGB_BLACK);
}

static void Turn(void)
{
    PlaySE(SE_BALL_TRAY_EXIT);
    Draw();
}

static void Move(void)
{
    PlaySE(SE_SELECT);
    Draw();
}

static void Input_Page(void)
{
    if (JOY_NEW(B_BUTTON))
        Leave();
    else if (JOY_NEW(A_BUTTON | DPAD_RIGHT | DPAD_DOWN | R_BUTTON))
    {
        if (sBook->page + 1 < sBook->pageCount)
        {
            sBook->page++;
            Turn();
        }
        else if (JOY_NEW(A_BUTTON))
        {
            Leave();
        }
    }
    else if (JOY_NEW(DPAD_LEFT | DPAD_UP | L_BUTTON) && sBook->page != 0)
    {
        sBook->page--;
        Turn();
    }
}

//  Four chapters on the left page, three on the right: UP and DOWN move within a page, LEFT and RIGHT across.
static void Input_Contents(void)
{
    u8 c = sBook->cursor;

    if (JOY_NEW(B_BUTTON))
    {
        Leave();
        return;
    }
    if (JOY_NEW(A_BUTTON))
    {
        if (!ChapterOpen(c + 1))
        {
            PlaySE(SE_FAILURE);
            return;
        }
        sBook->chapter = c + 1;
        sBook->cursor = 0;
        sBook->view = VIEW_CHAPTER;
        Turn();
        return;
    }
    if (JOY_NEW(DPAD_UP) && c != 0 && c != 4)
        c--;
    else if (JOY_NEW(DPAD_DOWN) && c != 3 && c != TB_CHAPTERS - 1)
        c++;
    else if (JOY_NEW(DPAD_RIGHT) && c < 4)
        c = (c + 4 < TB_CHAPTERS) ? c + 4 : TB_CHAPTERS - 1;
    else if (JOY_NEW(DPAD_LEFT) && c >= 4)
        c -= 4;
    if (c != sBook->cursor)
    {
        sBook->cursor = c;
        Move();
    }
}

//  The next chapter in DIR that has been attended, or 0.
static u8 NeighbourChapter(u8 chapter, s8 dir)
{
    s8 k;

    for (k = chapter + dir; k >= 1 && k <= TB_CHAPTERS; k += dir)
    {
        if (ChapterOpen(k))
            return k;
    }
    return 0;
}

static void Input_Chapter(void)
{
    u8 k;

    if (JOY_NEW(B_BUTTON))
    {
        sBook->cursor = sBook->chapter - 1;
        sBook->view = VIEW_CONTENTS;
        Turn();
    }
    else if (JOY_NEW(A_BUTTON))
    {
        sBook->topic = sBook->cursor;
        sBook->view = VIEW_TOPIC;
        Turn();
    }
    else if (JOY_NEW(DPAD_UP) && sBook->cursor != 0)
    {
        sBook->cursor--;
        Move();
    }
    else if (JOY_NEW(DPAD_DOWN) && sBook->cursor + 1 < TB_TOPICS)
    {
        sBook->cursor++;
        Move();
    }
    else if (JOY_NEW(DPAD_LEFT | DPAD_RIGHT | L_BUTTON | R_BUTTON))
    {
        k = NeighbourChapter(sBook->chapter, JOY_NEW(DPAD_LEFT | L_BUTTON) ? -1 : 1);
        if (k != 0)
        {
            sBook->chapter = k;
            sBook->cursor = 0;
            Turn();
        }
    }
}

static void Input_Topic(void)
{
    if (JOY_NEW(A_BUTTON | B_BUTTON))
    {
        sBook->cursor = sBook->topic;
        sBook->view = VIEW_CHAPTER;
        Turn();
    }
    else if (JOY_NEW(DPAD_LEFT | L_BUTTON) && sBook->topic != 0)
    {
        sBook->topic--;
        Turn();
    }
    else if (JOY_NEW(DPAD_RIGHT | R_BUTTON) && sBook->topic + 1 < TB_TOPICS)
    {
        sBook->topic++;
        Turn();
    }
}

static bool8 CardsOpen(void);
static void CardsClose(void);

static void Input_Cover(void)
{
    if (JOY_NEW(B_BUTTON))
    {
        Leave();
    }
    else if (IsLayerBook() && HasCards() && !sBook->cardsFocus && JOY_NEW(DPAD_DOWN | DPAD_RIGHT))
    {
        sBook->cardsFocus = TRUE;           // T-509: onto the deck tile
        Move();
    }
    else if (IsLayerBook() && sBook->cardsFocus && JOY_NEW(DPAD_UP | DPAD_LEFT))
    {
        sBook->cardsFocus = FALSE;
        Move();
    }
    else if (IsLayerBook() && sBook->cardsFocus && JOY_NEW(A_BUTTON))
    {
        if (!CardsOpen())
        {
            CardsClose();                   // no room on the heap: the section stays shut
            PlaySE(SE_FAILURE);
            return;
        }
        sBook->cards->cursor = CARDS_DAY;
        sBook->view = VIEW_CARDS;
        Turn();
    }
    else if (JOY_NEW(A_BUTTON) && sBook->book == BOOK_HERMETIC && Hermetic_CardOfDay() != NULL)
    {
        sBook->view = VIEW_THIRTEEN;     // T-483 (7): Halloween's card first
        Turn();
    }
    else if (JOY_NEW(A_BUTTON) && (sBook->book == BOOK_HERMETIC || sBook->book == BOOK_EUDAIMON || sBook->book == BOOK_UNITY))
    {
        // T-483: THE HERMETIC LAYER opens with its warning, as written, every time; its contents come after.
        // T-519 item 2: and THE EUDAIMON LAYER with its blessing.
        sBook->entry = 0;
        sBook->spread = 0;
        sBook->view = VIEW_SUBTLE_ENTRY;
        Turn();
    }
    else if (JOY_NEW(A_BUTTON))
    {
        // THE SUBTLE LAYER and THE ORIGIN OF A DAEMON (T-533) open on their contents, as the Guide does.
        sBook->view = IsLayerBook() ? VIEW_SUBTLE_CONTENTS : VIEW_GUIDE_CONTENTS;
        Turn();
    }
}

static void Input_Thirteen(void)
{
    if (JOY_NEW(B_BUTTON))
    {
        sBook->view = VIEW_COVER;
        Turn();
    }
    else if (JOY_NEW(A_BUTTON))
    {
        sBook->entry = 0;               // then the warning, as written
        sBook->spread = 0;
        sBook->view = VIEW_SUBTLE_ENTRY;
        Turn();
    }
}

static void Input_GuideContents(void)
{
    u8 count = sGuideSections[sBook->section].count;

    if (!sBook->focusRight)
    {
        if (JOY_NEW(B_BUTTON))
        {
            sBook->view = VIEW_COVER;
            Turn();
        }
        else if (JOY_NEW(A_BUTTON | DPAD_RIGHT))
        {
            sBook->focusRight = TRUE;
            sBook->cursor = 0;
            Move();
        }
        else if (JOY_NEW(DPAD_UP) && sBook->section != 0)
        {
            sBook->section--;
            Move();
        }
        else if (JOY_NEW(DPAD_DOWN) && sBook->section + 1 < GUIDE_SECTIONS)
        {
            sBook->section++;
            Move();
        }
        return;
    }
    if (JOY_NEW(B_BUTTON | DPAD_LEFT))
    {
        sBook->focusRight = FALSE;
        Move();
    }
    else if (JOY_NEW(A_BUTTON))
    {
        sBook->entry = sGuideSections[sBook->section].first + sBook->cursor;
        sBook->spread = 0;
        sBook->view = VIEW_GUIDE_ENTRY;
        Turn();
    }
    else if (JOY_NEW(DPAD_UP) && sBook->cursor != 0)
    {
        sBook->cursor--;
        Move();
    }
    else if (JOY_NEW(DPAD_DOWN) && sBook->cursor + 1 < count)
    {
        sBook->cursor++;
        Move();
    }
}

//  Back to the contents with the cursor on the chapter just read.
static void GuideBackToContents(void)
{
    sBook->section = GuideSectionOf(sBook->entry);
    sBook->cursor = sBook->entry - sGuideSections[sBook->section].first;
    sBook->focusRight = TRUE;
    sBook->view = VIEW_GUIDE_CONTENTS;
    Turn();
}

static void Input_GuideEntry(void)
{
    if (JOY_NEW(B_BUTTON))
    {
        GuideBackToContents();
    }
    else if (JOY_NEW(A_BUTTON | DPAD_RIGHT | R_BUTTON))
    {
        if (sBook->spread + 1 < sBook->spreadCount)
        {
            sBook->spread++;
            Turn();
        }
        else if (JOY_NEW(A_BUTTON))
        {
            GuideBackToContents();
        }
        else if (sBook->entry + 1 < GUIDE_ENTRIES)
        {
            sBook->entry++;
            sBook->spread = 0;
            Turn();
        }
    }
    else if (JOY_NEW(DPAD_LEFT | L_BUTTON))
    {
        if (sBook->spread != 0)
        {
            sBook->spread--;
            Turn();
        }
        else if (sBook->entry != 0)
        {
            sBook->entry--;
            sBook->spread = 0;
            Turn();
        }
    }
}


//  T-529: L and R turn THE UNITY BOOK's lens, on its contents and its pages; TRUE when they did (or met the end).
static bool8 LensInput(void)
{
    u8 to = sUnityLens, s, count;

    if (sBook->book != BOOK_UNITY || sBook->lens == NULL || !JOY_NEW(L_BUTTON | R_BUTTON))
        return FALSE;
    if (JOY_NEW(L_BUTTON))
        to = to == LENS_DAWN ? LENS_WHOLE : LENS_NIGHT;
    else
        to = to == LENS_NIGHT ? LENS_WHOLE : LENS_DAWN;
    if (to == sUnityLens)
        return TRUE;                    // a switch, not a wheel: already at that end
    if (sBook->view == VIEW_SUBTLE_ENTRY && sBook->entry != sBook->lensEntry)
    {
        sBook->lensS = SubtleSectionOf(sBook->entry);       // the place: a section, and a place in it
        sBook->lensJ = sBook->entry - Layer()->sections[sBook->lensS].first;
    }
    sUnityLens = to;
    if (to != LENS_WHOLE)
        LensBuild();
    if (sBook->view == VIEW_SUBTLE_ENTRY)
    {
        s = sBook->lensS;
        count = Layer()->sections[s].count;
        sBook->entry = Layer()->sections[s].first + (sBook->lensJ < count ? sBook->lensJ : count - 1);
        sBook->lensEntry = sBook->entry;
        sBook->picEntry = 0xFF;
    }
    else
    {
        count = Layer()->sections[sBook->section].count;
        if (sBook->cursor >= count)
            sBook->cursor = count - 1;
    }
    Turn();
    return TRUE;
}

static void Input_SubtleContents(void)
{
    u8 count = Layer()->sections[sBook->section].count;

    if (LensInput())
        return;

    if (!sBook->focusRight)
    {
        if (JOY_NEW(B_BUTTON))
        {
            sBook->view = VIEW_COVER;
            Turn();
        }
        else if (JOY_NEW(A_BUTTON | DPAD_RIGHT))
        {
            sBook->focusRight = TRUE;
            sBook->cursor = 0;
            Move();
        }
        else if (JOY_NEW(DPAD_UP) && sBook->section != 0)
        {
            sBook->section--;
            Move();
        }
        else if (JOY_NEW(DPAD_DOWN) && sBook->section + 1 < Layer()->sectionCount)
        {
            sBook->section++;
            Move();
        }
        return;
    }
    if (JOY_NEW(B_BUTTON | DPAD_LEFT))
    {
        sBook->focusRight = FALSE;
        Move();
    }
    else if (JOY_NEW(A_BUTTON))
    {
        sBook->entry = Layer()->sections[sBook->section].first + sBook->cursor;
        sBook->spread = 0;
        sBook->view = VIEW_SUBTLE_ENTRY;
        Turn();
    }
    else if (JOY_NEW(DPAD_UP) && sBook->cursor != 0)
    {
        sBook->cursor--;
        Move();
    }
    else if (JOY_NEW(DPAD_DOWN) && sBook->cursor + 1 < count)
    {
        sBook->cursor++;
        Move();
    }
}

static void SubtleBackToContents(void)
{
    sBook->section = SubtleSectionOf(sBook->entry);
    sBook->cursor = sBook->entry - Layer()->sections[sBook->section].first;
    sBook->focusRight = TRUE;
    sBook->view = VIEW_SUBTLE_CONTENTS;
    Turn();
}

//  T-486 (the user, 2026-10-29): THE HERMETIC LAYER is the one place DAEMONS glitches on purpose -- the first page
//  turned past OSRIC HATTER's warning, once a save: a fifth of a second of thrown scanlines and shuffled colours on
//  the book's own layer and palette (src/daemons_glitch.c), then the page as it should be.
static void GlitchPastTheWarning(bool8 wasWarning)
{
    if (!wasWarning || sBook->book != BOOK_HERMETIC || FlagGet(FLAG_HERMETIC_GLITCHED))
        return;
    if (sBook->view == VIEW_SUBTLE_ENTRY && sBook->entry == 0)
        return;                                 // still in the warning
    if (sBook->view != VIEW_SUBTLE_ENTRY && !JOY_NEW(A_BUTTON))
        return;                                 // B, back out: not past it
    FlagSet(FLAG_HERMETIC_GLITCHED);
    Glitch_Start(0, 1 << 15, Glitch_StrengthNow());   // T-486: at the night minute it is turned (the weakest outside 3-4)
}

static void Input_SubtleEntry(void)
{
    bool8 wasWarning = (sBook->book == BOOK_HERMETIC && sBook->entry == 0);
    u8 entry = sBook->entry;

    if (LensInput())
        return;
    Input_SubtleEntryTurn();
    if (sBook->entry != entry || sBook->view != VIEW_SUBTLE_ENTRY)
        sBook->lensEntry = 0xFF;        // T-529: a page turned, so the lens keeps this place now
    GlitchPastTheWarning(wasWarning);
}

static void Input_SubtleEntryTurn(void)
{
    u16 lr = sBook->book == BOOK_UNITY && sBook->lens != NULL ? 0 : (L_BUTTON | R_BUTTON);   // T-529: the lens's

    if (JOY_NEW(B_BUTTON))
    {
        SubtleBackToContents();
    }
    else if (JOY_NEW(A_BUTTON | DPAD_RIGHT | (lr & R_BUTTON)))
    {
        if (sBook->spread + 1 < sBook->spreadCount)
        {
            sBook->spread++;
            Turn();
        }
        else if (JOY_NEW(A_BUTTON))
        {
            SubtleBackToContents();
        }
        else if (sBook->entry + 1 < Layer()->entryCount)
        {
            sBook->entry++;
            sBook->spread = 0;
            Turn();
        }
    }
    else if (JOY_NEW(DPAD_LEFT | (lr & L_BUTTON)))
    {
        if (sBook->spread != 0)
        {
            sBook->spread--;
            Turn();
        }
        else if (sBook->entry != 0)
        {
            sBook->entry--;
            sBook->spread = 0;
            Turn();
        }
    }
}

//  T-483: THE HERMETIC LAYER BREATHES -- the backdrop round the open book takes about 30% of the pulse, and every
//  INSCRIPTION's emerald rises and falls with it (src/daemons_pulse.c, the app's and the watches' breath). Both are
//  worked out from the book's own palette each frame and written whole, so nothing compounds; never during a fade.
static void BreatheHermetic(void)
{
    u8 phase;
    u16 backdrop;

    if (sBook->book != BOOK_HERMETIC || gPaletteFade.active)
        return;
    phase = Pulse_Phase();
    backdrop = Pulse_Blend(sHermeticPal[C_COVER], Pulse_Colour(phase), PULSE_BG_WEIGHT);
    gPlttBufferUnfaded[0] = gPlttBufferFaded[0] = backdrop;
    gPlttBufferUnfaded[BG_PLTT_ID(15) + C_COVER] = gPlttBufferFaded[BG_PLTT_ID(15) + C_COVER] = backdrop;
    gPlttBufferUnfaded[BG_PLTT_ID(15) + C_RING] = gPlttBufferFaded[BG_PLTT_ID(15) + C_RING] = Pulse_Emerald(phase);
    BreatheCards(phase);    // T-509: and the hermetic frames, on their index 14
}

static void Task_BookInput(u8 taskId)
{
    if (sBook->leaving)
    {
        if (gPaletteFade.active)
            return;
        DestroyTask(taskId);
        CardsClose();                       // T-509: the section's windows, map and daemons
        FreeAllWindowBuffers();
        if (IsLayerBook())
        {
            ResetSpriteData();              // T-509: the deck tile (the field loads its own on return)
            FreeAllSpritePalettes();
        }
        if (sBook->song != 0)
        {
            ResetSpriteData();   // T-417: the song's notes (the field loads its own on return)
            Play_BookSongFree();
        }
        if (sBook->blank != NULL)
            Free(sBook->blank);
        if (sBook->blankPage != NULL)
            Free(sBook->blankPage);
        if (sBook->lens != NULL)
            Free(sBook->lens);              // T-529
        FREE_AND_SET_NULL(sBook);
        SetMainCallback2(CB2_ReturnToFieldContinueScriptPlayMapMusic);
        return;
    }
    switch (sBook->view)
    {
    case VIEW_PAGE:     Input_Page();     break;
    case VIEW_CONTENTS: Input_Contents(); break;
    case VIEW_CHAPTER:  Input_Chapter();  break;
    case VIEW_TOPIC:    Input_Topic();    break;
    case VIEW_COVER:          Input_Cover();         break;
    case VIEW_GUIDE_CONTENTS: Input_GuideContents(); break;
    case VIEW_GUIDE_ENTRY:    Input_GuideEntry();    break;
    case VIEW_SUBTLE_CONTENTS: Input_SubtleContents(); break;
    case VIEW_SUBTLE_ENTRY:    Input_SubtleEntry();    break;
    case VIEW_THIRTEEN:        Input_Thirteen();       break;
    case VIEW_CARDS:           Input_Cards();          break;
    case VIEW_CARDS_SELECT:    Input_CardsSelect();    break;
    case VIEW_CARDS_SPREAD:    Input_CardsSpread();    break;
    case VIEW_CARDS_BOARD:     Input_CardsBoard();     break;
    case VIEW_CARDS_READ:      Input_CardsRead();      break;
    }
    if (sBook != NULL && !sBook->leaving)
    {
        AnimateSubtlePicture();
        BreatheHermetic();
    }
}

//  ---------------------------------------------------------------- the screen

static void VBlankCB_Book(void)
{
    LoadOam();
    ProcessSpriteCopyRequests();
    TransferPlttBuffer();
    ScanlineEffect_InitHBlankDmaTransfer();   // T-486: the glitch's scanlines (nothing, when none runs)
}

static void CB2_Book(void)
{
    RunTasks();
    AnimateSprites();
    BuildOamBuffer();
    UpdatePaletteFade();
}

static void Task_BookInit(u8 taskId)
{
    const u16 *pal = (sBook->book == BOOK_NOTEBOOK) ? sNotebookPal
                   : (sBook->book == BOOK_GUIDE)    ? sGuidePal
                   : (sBook->book == BOOK_SUBTLE)   ? sSubtlePal
                   : (sBook->book == BOOK_HERMETIC) ? sHermeticPal
                   : (sBook->book == BOOK_EUDAIMON) ? sEudaimonPal
                   : (sBook->book == BOOK_UNITY)    ? sUnityPal
                   : (sBook->book == BOOK_ORIGIN)   ? sOriginPal      // T-533
                   : sTextbookPal;

    switch (sBook->initState)
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
        LoadPalette(pal, BG_PLTT_ID(15), PLTT_SIZE_4BPP);
        LoadPalette(&pal[C_COVER], BG_PLTT_ID(0), PLTT_SIZEOF(1));        // the backdrop is the cover
        break;
    case 3:
        if (IsLayerBook() && HasCards())
            CardsTileMake();                // T-509: the deck tile, shown on the cover only
        Draw();
        PutWindowTilemap(WIN_BOOK);
        CopyBgTilemapBufferToVram(0);
        if (sBook->song != 0)
            Play_BookSong(sBook->song - 1, NB_TEXT_X, SB_MIDDLE_C);   // T-417: its notes, in their colours
        break;
    case 4:
        BeginNormalPaletteFade(PALETTES_ALL, 0, 16, 0, RGB_BLACK);
        break;
    case 5:
        SetVBlankCallback(VBlankCB_Book);
        break;
    default:
        if (gPaletteFade.active)
            return;
        gTasks[taskId].func = Task_BookInput;
        return;
    }
    sBook->initState++;
}

static void CB2_OpenBook(void)
{
    ResetSpriteData();
    ResetPaletteFade();
    FreeAllSpritePalettes();
    ResetTasks();
    ScanlineEffect_Stop();
    CreateTask(Task_BookInit, 0);
    SetMainCallback2(CB2_Book);
}

//  T-421: FALSE when there is no room for the book -- the field comes back (the script waits on it, already faded, as
//  it would for a book closed) and the caller opens nothing, as play.c's Play_Start does.
static bool8 OpenBook(u8 book, u8 view)
{
    sBook = AllocZeroed(sizeof(*sBook));
    if (sBook == NULL)
    {
        SetMainCallback2(CB2_ReturnToFieldContinueScriptPlayMapMusic);
        return FALSE;
    }
    sBook->book = book;
    sBook->view = view;
    QuestLog_CutRecording();
    SetMainCallback2(CB2_OpenBook);
    LockPlayerFieldControls();
    return TRUE;
}

//  ---------------------------------------------------------------- the ways in

//  StringCopyN copies a fixed count and does not stop at EOS; this stops at either.
static void CopyBounded(u8 *dest, const u8 *src, u16 size)
{
    u16 i;

    for (i = 0; i < size - 1 && src[i] != EOS; i++)
        dest[i] = src[i];
    dest[i] = EOS;
}

//  A NOTEBOOK page: TITLE over TEXT, already expanded. The script fades to black first and waits.
void BookReader_OpenNotebookPage(const u8 *title, const u8 *text)
{
    if (!OpenBook(BOOK_NOTEBOOK, VIEW_PAGE))
        return;
    CopyBounded(sBook->title, title, sizeof(sBook->title));
    CopyBounded(sBook->text, text, BR_TEXT_SIZE);
    Reflow(sBook->text, NB_TEXT_W, BR_FONT);
    sBook->pageCount = (sBook->lineCount + NB_LINES - 1) / NB_LINES;
    if (sBook->pageCount == 0)
        sBook->pageCount = 1;
}

//  The TEXTBOOK, open at CONTENTS with the cursor on the first chapter attended. The script fades first.
void Textbook_Open(void)
{
    u8 k;

    if (!OpenBook(BOOK_TEXTBOOK, VIEW_CONTENTS))
        return;
    for (k = 1; k <= TB_CHAPTERS; k++)
    {
        if (ChapterOpen(k))
        {
            sBook->cursor = k - 1;
            break;
        }
    }
}

//  T-300: THE GUIDE, open at its cover. The script fades first.
void Guide_Open(void)
{
    OpenBook(BOOK_GUIDE, VIEW_COVER);
}

//  T-423: THE SUBTLE LAYER, open at its cover. The script fades first.
void Subtle_Open(void)
{
    if (OpenBook(BOOK_SUBTLE, VIEW_COVER))
        sBook->picEntry = 0xFF;
}

//  T-483: THE HERMETIC LAYER, open at its cover -- only ever at night (its script asks Hermetic_IsNight first).
void Hermetic_Open(void)
{
    if (OpenBook(BOOK_HERMETIC, VIEW_COVER))
        sBook->picEntry = 0xFF;
}

//  T-519 item 2: THE EUDAIMON LAYER, open at its cover -- at any hour, and read only: its thirteen are the night
//  half's own spells, cast from the SPELLBOOK, and this half only words them as gifts (the brief, item 5).
void Eudaimon_Open(void)
{
    if (OpenBook(BOOK_EUDAIMON, VIEW_COVER))
        sBook->picEntry = 0xFF;
}

//  T-519 item 3: THE UNITY BOOK, open at its cover -- at any hour ("It was written for no hour"). Its folds read to
//  their end let their spells off the page as the night half's did (Spellbook_OnPassageRead), since the night half
//  is in it now.
void Unity_Open(void)
{
    if (!OpenBook(BOOK_UNITY, VIEW_COVER))
        return;
    sBook->picEntry = 0xFF;
    sBook->lensEntry = 0xFF;
    sBook->lens = AllocZeroed(sizeof(*sBook->lens));   // T-529: no room, no lens -- the book reads whole
    if (sBook->lens != NULL && sUnityLens != LENS_WHOLE)
        LensBuild();                    // the session's setting, kept from the last time it was open
}

//  T-533: THE ORIGIN OF A DAEMON, open at its cover -- at any hour, and only as far as it has come back. The reader
//  is given the book cut after its last open section (Origin_OpenSections), and every view reads that copy through
//  Layer(): the contents list only those sections, an entry's turns stop at their last entry, and the way back to the
//  contents finds its section among them. Nothing can change while it is open, so the cut is made once, here.
void Origin_Open(void)
{
    u8 open;

    if (!OpenBook(BOOK_ORIGIN, VIEW_COVER))
        return;
    sBook->picEntry = 0xFF;
    open = Origin_OpenSections();
    sBook->origin = sOriginWhole;
    sBook->origin.sectionCount = open;
    sBook->origin.entryCount = sOriginWhole.sections[open - 1].first + sOriginWhole.sections[open - 1].count;
}

//  T-315: a NOTEBOOK page that ends on a QR code -- one more page, after the text. Called just after it is opened.
void BookReader_AddQr(u8 which)
{
    if (sBook == NULL)   // T-421: the book could not be opened
        return;
    sBook->qr = which;
    sBook->pageCount++;
}

//  T-417: a SONGBOOK page -- its notes on a staff under the words, and the key's note at its left once known.
void BookReader_AddSong(u8 song)
{
    if (sBook == NULL)   // T-421: the book could not be opened
        return;
    sBook->song = song + 1;
}

//  T-311: the REVIEW BOARD's copy, on the shelf in its lobby.
void Guide_OpenBoardCopy(void)
{
    if (OpenBook(BOOK_GUIDE, VIEW_COVER))
        sBook->torn = TRUE;
}
