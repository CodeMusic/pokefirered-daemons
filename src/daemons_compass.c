#include "global.h"
#include "gflib.h"
#include "event_data.h"
#include "menu.h"
#include "random.h"
#include "script.h"
#include "sound.h"
#include "task.h"
#include "trig.h"
#include "daemons_compass.h"
#include "constants/flags.h"
#include "constants/songs.h"

//  T-586: DAVID's COMPASS (the user, 2026-11-17; DRAFT, drawn in code).
//
//  Given on the S.S. ANNE when DAVID looks out the porthole instead of reading on. Its rose is lettered in the mirror
//  alphabet of the ship's painting (T-493: A for Z), so where a compass says N E S W it says M V H D. Used from the bag,
//  it opens over the field: the rose on graph paper, its four letters, and a needle. Up and down turn the chosen letter
//  through the alphabet, left and right choose which. Nothing is written on the screen. While the rose is wrong the
//  needle spins; set to N E S W it holds -- the letters stop turning, the needle settles north with one quiet sound, and
//  the compass is READ (FLAG_COMPASS_READ), which from then on brings a player out of a grove's fir beside that grove's
//  own tree in THE NEXUS (EventScript_SingingFir, DAEMONS tools/gbanexusmap.py). Read, it opens already holding.
//
//  RAM: one heap struct and two 4.6 KB heap buffers while it is open (engine.md trap 22: the paper and the letters are
//  drawn once and copied, so only the needle is drawn each frame); nothing in EWRAM or IWRAM, one flag in the save.

#define WIN_W      12                       // tiles: 96x96 px, centred over the field
#define WIN_H      12
#define WIN_LEFT   9
#define WIN_TOP    2
#define WIN_TILE   0x050                    // play.c's staff: past the map name popup, short of the YES/NO box's 0x125
#define WIN_PAL    13                       // the popup's own; it loads its palette again when it next shows
#define PX_W       (WIN_W * 8)
#define PX_H       (WIN_H * 8)
#define BUF_SIZE   (WIN_W * WIN_H * TILE_SIZE_4BPP)
#define CX         48
#define CY         48
#define RING       30
#define NEEDLE     21

enum { C_NONE, C_PAPER, C_GRID, C_RULE, C_INK, C_INK_MID, C_RED, C_TEAL, C_DARK, C_SHADOW };

static const u16 sCompassPal[16] = {
    [C_NONE]    = RGB(0, 0, 0),
    [C_PAPER]   = RGB(30, 30, 27),
    [C_GRID]    = RGB(23, 27, 26),
    [C_RULE]    = RGB(18, 23, 22),
    [C_INK]     = RGB(5, 8, 9),
    [C_INK_MID] = RGB(11, 15, 16),
    [C_RED]     = RGB(25, 7, 6),
    [C_TEAL]    = RGB(3, 18, 17),
    [C_DARK]    = RGB(7, 10, 11),
    [C_SHADOW]  = RGB(23, 25, 23),
};

static const u8 sInk[]  = { C_PAPER, C_INK, C_SHADOW };
static const u8 sTeal[] = { C_PAPER, C_TEAL, C_SHADOW };

// The four points, N E S W, and the letters a compass would print there. The rose starts in the mirror: 25 - each.
static const u8 sTrue[4] = { 'N' - 'A', 'E' - 'A', 'S' - 'A', 'W' - 'A' };

enum { ST_SPIN, ST_SETTLE, ST_HELD };

struct Compass
{
    u8 *bg;          // the paper, the grid and the rose
    u8 *base;        // that, with the four letters on it
    u8 windowId;
    u8 letters[4];
    u8 cursor;
    u8 state;
    u8 angle;        // 0 is north, 64 east: a quarter turn is 64
    s8 speed;
    u8 wobble;
    s16 pos;         // settling: the angle from north in 16ths of a step, signed, and its swing
    s16 vel;
};

static void Task_Compass(u8 taskId);

// ---------------------------------------------------------------------------------------------------------- drawing

// One pixel of a 4bpp window buffer, either half of its byte (engine.md trap 36 is FillWindowPixelRect's, not this).
static void Px(u8 *buf, s16 x, s16 y, u8 c)
{
    u8 *p;

    if (x < 0 || y < 0 || x >= PX_W || y >= PX_H)
        return;
    p = buf + ((y >> 3) * WIN_W + (x >> 3)) * TILE_SIZE_4BPP + (y & 7) * 4 + ((x & 7) >> 1);
    if (x & 1)
        *p = (*p & 0x0F) | (c << 4);
    else
        *p = (*p & 0xF0) | c;
}

static void Circle(u8 *buf, s16 r, u8 c)
{
    s16 x = r, y = 0, err = 1 - r;

    while (x >= y)
    {
        Px(buf, CX + x, CY + y, c); Px(buf, CX - x, CY + y, c);
        Px(buf, CX + x, CY - y, c); Px(buf, CX - x, CY - y, c);
        Px(buf, CX + y, CY + x, c); Px(buf, CX - y, CY + x, c);
        Px(buf, CX + y, CY - x, c); Px(buf, CX - y, CY - x, c);
        y++;
        if (err < 0)
            err += 2 * y + 1;
        else
        {
            x--;
            err += 2 * (y - x) + 1;
        }
    }
}

// Along a direction from the centre, from radius r0 to r1 (angle in 256ths of a turn, 0 north, clockwise).
static void Ray(u8 *buf, u8 angle, s16 r0, s16 r1, u8 c, bool8 thick)
{
    s16 r, x, y;
    s32 sn = gSineTable[angle], cs = gSineTable[(u8)(angle + 64)];

    for (r = r0; r <= r1; r++)
    {
        x = CX + (sn * r + (sn < 0 ? -128 : 128)) / 256;
        y = CY - (cs * r + (cs < 0 ? -128 : 128)) / 256;
        Px(buf, x, y, c);
        if (thick)
        {
            Px(buf, x + 1, y, c);
            Px(buf, x, y + 1, c);
        }
    }
}

// Graph paper, a rule round the edge, and the rose: two rings, a cross of hairlines, a tick at each half-point.
static void DrawPaper(u8 *buf)
{
    s16 x, y, dx, dy;
    u8 i;

    for (y = 0; y < PX_H; y++)
        for (x = 0; x < PX_W; x++)
        {
            dx = x - CX;
            dy = y - CY;
            if (dx * dx + dy * dy <= (RING - 2) * (RING - 2))
                Px(buf, x, y, C_PAPER);
            else if (x == 0 || y == 0 || x == PX_W - 1 || y == PX_H - 1)
                Px(buf, x, y, C_INK_MID);
            else if ((x & 7) == 0 || (y & 7) == 0)
                Px(buf, x, y, C_GRID);
            else
                Px(buf, x, y, C_PAPER);
        }
    for (x = CX - RING + 3; x <= CX + RING - 3; x++)
        Px(buf, x, CY, C_GRID);
    for (y = CY - RING + 3; y <= CY + RING - 3; y++)
        Px(buf, CX, y, C_GRID);
    Circle(buf, RING, C_INK);
    Circle(buf, RING - 2, C_RULE);
    for (i = 0; i < 8; i++)
        Ray(buf, i * 32, RING - (i & 1 ? 5 : 7), RING - 3, i & 1 ? C_RULE : C_INK_MID, FALSE);
}

static void LetterAt(u8 point, s16 *x, s16 *y)
{
    switch (point)
    {
    case 0:  *x = CX - 3; *y = 1;        break;
    case 1:  *x = PX_W - 11; *y = CY - 7; break;
    case 2:  *x = CX - 3; *y = PX_H - 16; break;
    default: *x = 5; *y = CY - 7;         break;
    }
}

// The letters onto a fresh copy of the paper, and the result kept as the needle's base.
static void DrawLetters(struct Compass *c)
{
    u8 *tiles = (u8 *)GetWindowAttribute(c->windowId, WINDOW_TILE_DATA);
    u8 str[2];
    s16 x, y, i;
    u8 p;

    CpuFastCopy(c->bg, tiles, BUF_SIZE);
    for (p = 0; p < 4; p++)
    {
        bool8 chosen = (c->state == ST_SPIN && p == c->cursor);

        str[0] = CHAR_A + c->letters[p];
        str[1] = EOS;
        LetterAt(p, &x, &y);
        x += (6 - GetStringWidth(FONT_NORMAL, str, 0)) / 2;
        AddTextPrinterParameterized4(c->windowId, FONT_NORMAL, x, y, 0, 0, chosen ? sTeal : sInk, TEXT_SKIP_DRAW, str);
        if (chosen)
        {
            LetterAt(p, &x, &y);
            for (i = -1; i < 8; i++)
                Px(tiles, x + i, y + 14, C_TEAL);
        }
    }
    CpuFastCopy(tiles, c->base, BUF_SIZE);
}

static void DrawNeedle(struct Compass *c)
{
    u8 *tiles = (u8 *)GetWindowAttribute(c->windowId, WINDOW_TILE_DATA);
    s16 x, y;

    CpuFastCopy(c->base, tiles, BUF_SIZE);
    Ray(tiles, c->angle + 128, 0, NEEDLE - 6, C_DARK, TRUE);
    Ray(tiles, c->angle, 0, NEEDLE, C_RED, TRUE);
    for (y = -1; y <= 2; y++)
        for (x = -1; x <= 2; x++)
            Px(tiles, CX + x, CY + y, (x == -1 || x == 2 || y == -1 || y == 2) ? C_INK : C_PAPER);
    CopyWindowToVram(c->windowId, COPYWIN_GFX);
}

static bool8 IsTrue(struct Compass *c)
{
    u8 p;

    for (p = 0; p < 4; p++)
        if (c->letters[p] != sTrue[p])
            return FALSE;
    return TRUE;
}

// ------------------------------------------------------------------------------------------------------------ open

void Compass_Open(void)
{
    struct WindowTemplate template = {
        .bg = 0, .tilemapLeft = WIN_LEFT, .tilemapTop = WIN_TOP, .width = WIN_W, .height = WIN_H,
        .paletteNum = WIN_PAL, .baseBlock = WIN_TILE,
    };
    struct Compass *c = AllocZeroed(sizeof(*c));
    u8 taskId, p;

    if (c != NULL)
    {
        c->bg = AllocZeroed(BUF_SIZE);
        c->base = AllocZeroed(BUF_SIZE);
        c->windowId = AddWindow(&template);
    }
    if (c == NULL || c->bg == NULL || c->base == NULL || c->windowId == WINDOW_NONE)
    {
        if (c != NULL)
        {
            if (c->windowId != WINDOW_NONE && c->bg != NULL && c->base != NULL)
                RemoveWindow(c->windowId);
            TRY_FREE_AND_SET_NULL(c->bg);
            TRY_FREE_AND_SET_NULL(c->base);
            Free(c);
        }
        ScriptContext_Enable();
        return;
    }
    LoadPalette(sCompassPal, BG_PLTT_ID(WIN_PAL), PLTT_SIZE_4BPP);
    DrawPaper(c->bg);
    if (FlagGet(FLAG_COMPASS_READ))
    {
        for (p = 0; p < 4; p++)
            c->letters[p] = sTrue[p];
        c->state = ST_HELD;
        c->angle = 0;
    }
    else
    {
        for (p = 0; p < 4; p++)
            c->letters[p] = 25 - sTrue[p];   // M V H D
        c->state = ST_SPIN;
        c->angle = Random();
        c->speed = 7;
    }
    PutWindowTilemap(c->windowId);
    DrawLetters(c);
    DrawNeedle(c);
    CopyWindowToVram(c->windowId, COPYWIN_FULL);
    taskId = CreateTask(Task_Compass, 80);
    SetWordTaskArg(taskId, 0, (u32)c);
}

static void Close(u8 taskId, struct Compass *c)
{
    ClearWindowTilemap(c->windowId);
    CopyWindowToVram(c->windowId, COPYWIN_MAP);
    RemoveWindow(c->windowId);
    Free(c->bg);
    Free(c->base);
    Free(c);
    DestroyTask(taskId);
    ScriptContext_Enable();
}

// A wrong rose: the needle never rests -- its speed drifts, now and then it swings back.
static void Spin(struct Compass *c)
{
    if (++c->wobble >= 24)
    {
        c->wobble = Random() % 12;
        c->speed = 3 + Random() % 9;
        if (Random() % 4 == 0)
            c->speed = -c->speed;
    }
    c->angle += c->speed;
}

// A true rose: the needle swings to north as a needle does -- carried on by its spin, pulled back, a little less each
// time -- and rests, about two thirds of a second and three swings later.
static bool8 Settle(struct Compass *c)
{
    c->vel -= c->pos / 10;
    c->vel = c->vel * 13 / 16;
    c->pos += c->vel;
    if (c->pos > -16 && c->pos < 16 && c->vel > -16 && c->vel < 16)
    {
        c->angle = 0;
        return TRUE;
    }
    c->angle = (u8)(c->pos >> 4);
    return FALSE;
}

static void Task_Compass(u8 taskId)
{
    struct Compass *c = (struct Compass *)GetWordTaskArg(taskId, 0);
    bool8 relettered = FALSE;

    if (JOY_NEW(A_BUTTON | B_BUTTON))
    {
        Close(taskId, c);
        return;
    }
    if (c->state == ST_SPIN)
    {
        if (JOY_NEW(DPAD_LEFT))
            c->cursor = (c->cursor + 3) & 3, relettered = TRUE;
        else if (JOY_NEW(DPAD_RIGHT))
            c->cursor = (c->cursor + 1) & 3, relettered = TRUE;
        else if (JOY_REPT(DPAD_UP))
            c->letters[c->cursor] = (c->letters[c->cursor] + 1) % 26, relettered = TRUE;
        else if (JOY_REPT(DPAD_DOWN))
            c->letters[c->cursor] = (c->letters[c->cursor] + 25) % 26, relettered = TRUE;
        if (relettered && IsTrue(c))
        {
            c->state = ST_SETTLE;      // the rose holds: no letter turns from here
            c->pos = (s8)c->angle * 16;
            c->vel = c->speed * 16;
            FlagSet(FLAG_COMPASS_READ);
        }
        if (relettered)
            DrawLetters(c);
    }
    switch (c->state)
    {
    case ST_SPIN:
        Spin(c);
        DrawNeedle(c);
        break;
    case ST_SETTLE:
        if (Settle(c))
        {
            c->state = ST_HELD;
            PlaySE(SE_PIN);
        }
        DrawNeedle(c);
        break;
    }
}

// The map the player stands on, as a MAP_* constant ((group << 8) | num), in VAR_RESULT: called with `special`, then
// VAR_RESULT read (engine.md trap 37). The singing fir asks it, to know which grove it is leaving.
void Compass_CurrentMap(void)
{
    gSpecialVar_Result = (gSaveBlock1Ptr->location.mapGroup << 8) | gSaveBlock1Ptr->location.mapNum;
}
