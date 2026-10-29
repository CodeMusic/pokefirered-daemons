#include "global.h"
#include "gpu_regs.h"
#include "palette.h"
#include "random.h"
#include "scanline_effect.h"
#include "task.h"
#include "daemons_glitch.h"

// T-486 (the user, 2026-10-29; bible 4.39): THE HERMETIC LAYER is the one place DAEMONS glitches on purpose -- the
// first page turned past OSRIC HATTER's warning (once a save, FLAG_HERMETIC_GLITCHED; book_reader.c), and, once T-485
// builds his house, at half past three there. Well under a second, and never a black frame: for GLITCH_FRAMES the
// chosen layer's scanlines are thrown sideways in bands (an HBlank DMA into its HOFS, the vanilla scanline effect),
// and on every other frame the chosen BG palettes are rotated in the faded buffer -- never toward black, only into
// each other's colours. Then the scanlines go straight and the palettes are copied back from the unfaded buffer, so
// nothing of it is left behind. It holds no RAM of its own: a task's data, and the scanline buffers vanilla keeps.

#define GLITCH_FRAMES 12            // a fifth of a second

#define tFrame    data[0]
#define tBg       data[1]
#define tPalettes data[2]
#define tHofs     data[3]     // the layer's own offset, put back after

static void Task_Glitch(u8 taskId);

static void Scramble(u8 bg, u16 base)
{
    u16 y = 0, band, offset, i;

    while (y < DISPLAY_HEIGHT)
    {
        band = 1 + Random() % 7;
        offset = (Random() % 4 == 0) ? 0 : (Random() % 48) - 24;
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

void Glitch_Start(u8 bg, u16 palettes)
{
    struct ScanlineEffectParams params;
    u8 taskId;

    if (Glitch_IsRunning() || gScanlineEffect.state != 0)
        return;                     // another scanline effect owns the HBlank: no glitch rather than a broken one
    taskId = CreateTask(Task_Glitch, 0);
    if (taskId == TASK_NONE)
        return;
    gTasks[taskId].tBg = bg & 3;
    gTasks[taskId].tPalettes = palettes;
    gTasks[taskId].tHofs = GetGpuReg(REG_OFFSET_BG0HOFS + (bg & 3) * 4);
    ScanlineEffect_Clear();
    Scramble(bg & 3, gTasks[taskId].tHofs);
    params.dmaDest = (vu16 *)(REG_ADDR_BG0HOFS + (bg & 3) * 4);
    params.dmaControl = SCANLINE_EFFECT_DMACNT_16BIT;
    params.initState = 1;
    params.unused9 = 0;
    ScanlineEffect_SetParams(params);
}

bool8 Glitch_IsRunning(void)
{
    return FuncIsActiveTask(Task_Glitch);
}

static void Task_Glitch(u8 taskId)
{
    s16 *data = gTasks[taskId].data;

    if (++tFrame >= GLITCH_FRAMES)
    {
        Restore(tBg, tHofs, tPalettes);
        DestroyTask(taskId);
        return;
    }
    Scramble(tBg, tHofs);
    if (tFrame & 1)
        RotatePalettes(tPalettes);
    else
        RestorePalettes(tPalettes);
}
