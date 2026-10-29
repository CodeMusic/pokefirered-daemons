#include "global.h"
#include "gpu_regs.h"
#include "palette.h"
#include "random.h"
#include "scanline_effect.h"
#include "task.h"
#include "daemons_glitch.h"
#include "daemons_time.h"

// T-486 (the user, 2026-10-29; bible 4.39): THE HERMETIC LAYER is the one place DAEMONS glitches on purpose -- the
// first page turned past OSRIC HATTER's warning (once a save, FLAG_HERMETIC_GLITCHED; book_reader.c), and, once T-485
// builds his house, on every entry from 3:00 to 3:59 there. Well under a second, and never a black frame: for its frames the
// chosen layer's scanlines are thrown sideways in bands (an HBlank DMA into its HOFS, the vanilla scanline effect),
// and on some of them the chosen BG palettes are rotated in the faded buffer -- never toward black, only into
// each other's colours. Then the scanlines go straight and the palettes are copied back from the unfaded buffer, so
// nothing of it is left behind. It holds no RAM of its own: a task's data, and the scanline buffers vanilla keeps.

// T-486 (the user, 2026-10-29): THE STRENGTH FOLLOWS THE CLOCK. A strength is 25-100 (a percentage): 100 at 3:30, the
// Tablet of Gravity's hour, and 25 at 3:00 and toward 4:00 -- s = 1 - |minute - 30| / 30 x 0.75. The strongest glitch
// is 12 frames (a fifth of a second), its scanlines thrown up to 24px, its palettes rotated every other frame; the
// weakest 4 frames, 10px, rotated every 4th -- and the first glitched frame always rotates, so even the weakest keeps
// its colour swap. Between, each is drawn straight from the strength (qa 29Oct2026 T486 51, the user's samples).
#define tFrame    data[0]
#define tBg       data[1]
#define tPalettes data[2]
#define tHofs     data[3]     // the layer's own offset, put back after
#define tFrames   data[4]     // how many frames, from the strength
#define tReach    data[5]     // how far a band is thrown, in pixels either way
#define tEvery    data[6]     // the palettes rotate on every tEvery-th frame

static void Task_Glitch(u8 taskId);

static void Scramble(u8 bg, u16 base, s16 reach)
{
    u16 y = 0, band, offset, i;

    while (y < DISPLAY_HEIGHT)
    {
        band = 1 + Random() % 7;
        offset = (Random() % 4 == 0) ? 0 : (Random() % (reach * 2 + 1)) - reach;
        for (i = 0; i < band && y < DISPLAY_HEIGHT; i++, y++)
            gScanlineEffectRegBuffers[0][y] = gScanlineEffectRegBuffers[1][y] = base + offset;
    }
}

static void RotatePalettes(u16 palettes)
{
    u8 p, e, shift;
    u16 row[16];

    for (p = 0; p < 16; p++)
    {
        if (!(palettes & (1 << p)))
            continue;
        shift = 1 + Random() % 14;
        for (e = 1; e < 16; e++)
            row[e] = gPlttBufferUnfaded[BG_PLTT_ID(p) + 1 + (e - 1 + shift) % 15];
        for (e = 1; e < 16; e++)
            gPlttBufferFaded[BG_PLTT_ID(p) + e] = row[e];
    }
}

static void RestorePalettes(u16 palettes)
{
    u8 p;

    for (p = 0; p < 16; p++)
        if (palettes & (1 << p))
            CpuCopy16(&gPlttBufferUnfaded[BG_PLTT_ID(p)], &gPlttBufferFaded[BG_PLTT_ID(p)], PLTT_SIZE_4BPP);
}

static void Restore(u8 bg, u16 hofs, u16 palettes)
{
    ScanlineEffect_Stop();
    SetGpuReg(REG_OFFSET_BG0HOFS + bg * 4, hofs);
    RestorePalettes(palettes);
}

void Glitch_Start(u8 bg, u16 palettes, u8 strength)
{
    struct ScanlineEffectParams params;
    u8 taskId;
    s16 over;

    if (Glitch_IsRunning() || gScanlineEffect.state != 0)
        return;                     // another scanline effect owns the HBlank: no glitch rather than a broken one
    taskId = CreateTask(Task_Glitch, 0);
    if (taskId == TASK_NONE)
        return;
    if (strength < GLITCH_WEAKEST)
        strength = GLITCH_WEAKEST;
    if (strength > GLITCH_STRONGEST)
        strength = GLITCH_STRONGEST;
    over = strength - GLITCH_WEAKEST;                                   // 0-75
    gTasks[taskId].tFrames = 4 + over * 8 / 75;                          // 4-12
    gTasks[taskId].tReach = 10 + over * 14 / 75;                         // 10-24
    gTasks[taskId].tEvery = 2 + ((75 - over) * 2 + 37) / 75;             // 4-2, rounded
    gTasks[taskId].tBg = bg & 3;
    gTasks[taskId].tPalettes = palettes;
    gTasks[taskId].tHofs = GetGpuReg(REG_OFFSET_BG0HOFS + (bg & 3) * 4);
    ScanlineEffect_Clear();
    Scramble(bg & 3, gTasks[taskId].tHofs, gTasks[taskId].tReach);
    params.dmaDest = (vu16 *)(REG_ADDR_BG0HOFS + (bg & 3) * 4);
    params.dmaControl = SCANLINE_EFFECT_DMACNT_16BIT;
    params.initState = 1;
    params.unused9 = 0;
    ScanlineEffect_SetParams(params);
}

// T-486: the strength for now -- the witching hour's minute on the curve, and the weakest outside it (the book's
// warning page glitches once a save, so it must still happen at any hour of the night).
u8 Glitch_StrengthNow(void)
{
    s8 minute = DaemonsWitchingMinute();
    s8 off;

    if (minute < 0)
        return GLITCH_WEAKEST;
    off = minute - 30;
    if (off < 0)
        off = -off;
    return GLITCH_STRONGEST - off * 5 / 2;                               // 100 at :30, 25 at :00, 28 at :59
}

bool8 Glitch_IsRunning(void)
{
    return FuncIsActiveTask(Task_Glitch);
}

static void Task_Glitch(u8 taskId)
{
    s16 *data = gTasks[taskId].data;

    if (++tFrame >= tFrames)
    {
        Restore(tBg, tHofs, tPalettes);
        DestroyTask(taskId);
        return;
    }
    Scramble(tBg, tHofs, tReach);
    if ((tFrame - 1) % tEvery == 0)
        RotatePalettes(tPalettes);
    else
        RestorePalettes(tPalettes);
}
