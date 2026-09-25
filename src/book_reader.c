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
#include "constants/songs.h"

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

enum { BOOK_NOTEBOOK, BOOK_TEXTBOOK, BOOK_GUIDE };
enum { VIEW_PAGE, VIEW_CONTENTS, VIEW_CHAPTER, VIEW_TOPIC, VIEW_COVER, VIEW_GUIDE_CONTENTS, VIEW_GUIDE_ENTRY };

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
    u8 *blank;              // the empty sheet or spread, drawn once: filling it per pixel costs a quarter second
    u8 *blankPage;          // GUIDE: its reading page, which is not the contents' spread, so it is kept apart
    u16 lineStart[BR_MAX_LINES];
    u8 title[40];
    u8 text[BR_TEXT_SIZE];
    u8 lines[BR_LINES_SIZE];
};

static EWRAM_DATA struct BookReader *sBook = NULL;

static const struct BgTemplate sBgTemplates[] =
{
    { .bg = 0, .charBaseIndex = 0, .mapBaseIndex = 31, .screenSize = 0, .paletteMode = 0, .priority = 0, .baseTile = 0 },
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

static const u16 sGuidePal[] = INCBIN_U16("graphics/book/guide_cover.gbapal");
static const u32 sGuideCover[] = INCBIN_U32("graphics/book/guide_cover.4bpp");

//  DRAFT WORDING (T-300): the Guide's contents heading, and the spread counter's separator.
static const u8 sText_GuideContents[] = _("CONTENTS");

static void Task_BookInit(u8 taskId);
static void Task_BookInput(u8 taskId);

//  ---------------------------------------------------------------- drawing primitives

static void Rect(u8 color, s16 x, s16 y, s16 w, s16 h)
{
    if (w > 0 && h > 0)
        FillWindowPixelRect(WIN_BOOK, PIXEL_FILL(color), x, y, w, h);
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
    u8 n, len, count = 0;
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
            indent = (sBook->book == BOOK_GUIDE);   // the Guide's wide page: a paragraph opens indented
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

static void GuideLayout(void)
{
    const struct GuideEntry *e = &sGuideEntries[sBook->entry];
    u8 titleLines = Reflow(e->title, GP_TEXT_W, BR_SMALL);
    u8 n;

    sBook->bodyTop = GE_TITLE_TOP + titleLines * GE_TITLE_PITCH + 9;
    n = Reflow(e->text, GP_TEXT_W, BR_FONT);
    sBook->spreadCount = 1;
    if (n > GuideFirstLines())
        sBook->spreadCount += (n - GuideFirstLines() + GE_PAGE_LINES - 1) / GE_PAGE_LINES;
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
    GuidePageBackground();
    Print(BR_SMALL, sFaint, GP_TEXT_X, 9, sGuideSections[GuideSectionOf(sBook->entry)].label);
    if (sBook->spreadCount > 1)                                             // 2/3, at the head's right
    {
        p = ConvertIntToDecimalStringN(buf, sBook->spread + 1, STR_CONV_MODE_LEFT_ALIGN, 1);
        *p++ = CHAR_SLASH;
        ConvertIntToDecimalStringN(p, sBook->spreadCount, STR_CONV_MODE_LEFT_ALIGN, 1);
        Print(BR_SMALL, sFaint, GP_TEXT_X + GP_TEXT_W - Width(BR_SMALL, buf), 9, buf);
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

    if (sBook->spread + 1 < sBook->spreadCount)                             // a dog-ear while there is more
    {
        for (i = 0; i < 6; i++)
            Rect(C_MARGIN, GP_X + GP_W - 6 + i, 153 - i, 6 - i, 1);
    }
    CopyWindowToVram(WIN_BOOK, COPYWIN_GFX);
}

static void Draw(void)
{
    switch (sBook->view)
    {
    case VIEW_PAGE:           DrawNotebook();      break;
    case VIEW_CONTENTS:       DrawContents();      break;
    case VIEW_CHAPTER:        DrawChapter();       break;
    case VIEW_TOPIC:          DrawTopic();         break;
    case VIEW_COVER:          DrawGuideCover();    break;
    case VIEW_GUIDE_CONTENTS: DrawGuideContents(); break;
    case VIEW_GUIDE_ENTRY:    DrawGuideEntry();    break;
    }
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

static void Input_Cover(void)
{
    if (JOY_NEW(B_BUTTON))
    {
        Leave();
    }
    else if (JOY_NEW(A_BUTTON))
    {
        sBook->view = VIEW_GUIDE_CONTENTS;
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

static void Task_BookInput(u8 taskId)
{
    if (sBook->leaving)
    {
        if (gPaletteFade.active)
            return;
        DestroyTask(taskId);
        FreeAllWindowBuffers();
        if (sBook->blank != NULL)
            Free(sBook->blank);
        if (sBook->blankPage != NULL)
            Free(sBook->blankPage);
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
    }
}

//  ---------------------------------------------------------------- the screen

static void VBlankCB_Book(void)
{
    LoadOam();
    ProcessSpriteCopyRequests();
    TransferPlttBuffer();
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
        Draw();
        PutWindowTilemap(WIN_BOOK);
        CopyBgTilemapBufferToVram(0);
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

static void OpenBook(u8 book, u8 view)
{
    sBook = AllocZeroed(sizeof(*sBook));
    sBook->book = book;
    sBook->view = view;
    QuestLog_CutRecording();
    SetMainCallback2(CB2_OpenBook);
    LockPlayerFieldControls();
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
    OpenBook(BOOK_NOTEBOOK, VIEW_PAGE);
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

    OpenBook(BOOK_TEXTBOOK, VIEW_CONTENTS);
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
