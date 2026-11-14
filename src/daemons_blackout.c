#include "global.h"
#include "main.h"
#include "palette.h"
#include "overworld.h"
#include "fieldmap.h"
#include "daemons_pulse.h"
#include "daemons_aurora.h"
#include "daemons_blackout.h"

// T-546 (the user, 2026-11-11): THE BLACKOUT, ROCK TUNNEL, in digital purple with threads of green. "before verbose you
// see black with some dim green threads -- and after you use it bright green threads with the rest as a life like
// breathing purple". The tiles are tools/gbacaves.py's direction B, LIGHT IN THE SEAMS; this is the dark.
//
// WHY NOT THE WINDOW. FRLG's dark is WIN0, a circle drawn per scanline (field_screen_effect.c); outside it WINOUT
// showed only BG0, so the map was not drawn there at all and no thread could be seen. A hardware darken keeps every
// colour's ratio, so it can never make black-with-threads. So the dark is a PALETTE, laid at the copy to PLTT as the
// pulse (T-483) and the aurora (T-528) are: the buffers are never touched, so the fades, the battle transition and
// every palette reload work unchanged, and both PLTT paths (engine.md trap 30) already call Pulse_ApplyToPltt, which
// calls this last. On these two maps WINOUT also shows BG1-3 (never the sprites): the map is drawn everywhere in the
// dark palette, and WIN0's circle still decides which sprites show, as vanilla hides a trainer in the dark.
//
// BEFORE VERBOSE (flashLevel != 0): every BG entry of rows 0..12 black, the backdrop too, but the six thread slots, each
// at a share of its own colour (core, mid, halo). VERBOSE: AnimateFlash(0) calls Blackout_Illuminate with the circle's
// own growth, and each entry rises from its dark to its lit colour in step. AFTER: the threads as loaded (bright), and
// every other entry of rows 8..12 breathing -- scaled between 70% and 100% and lifted a little toward red and blue at
// the top -- on Pulse_Phase(), the pulse's eight-second breath, so the cave breathes with the companion and the boards.
//
// TWO DECISIONS (2026-11-11, the coordinator's to make, recorded here): (1) THE DARK OUTRANKS HALFTONE -- it is laid
// over whatever reached PLTT, so in grey the threads would be grey in the black; the two never meet on these maps.
// (2) THE THREADS STAY STEADY IN THE DARK -- only the lit purple breathes.
//
// The slot table, the shares and the breath below are read by tools/gbacaves.py --blackout-proof, which mocks the dark
// from them, so the sheet cannot drift from this file: keep each on its own line in this form.

extern const struct Tileset gTileset_RockTunnel;

#define BLACKOUT_ROWS        13      // BG rows 0..12: the map's; 13-15 are the windows and the text
#define BLACKOUT_LIT         256
// the six thread slots (palette row, index) and their share of their colour in the dark, in 256ths
#define THREAD_CORE_DIM      102     // 40%
#define THREAD_MID_DIM       77      // 30%
#define THREAD_HALO_DIM      46      // 18%
// the breath: the purple at 179/256 (70%) at the exhale, full at the inhale, lifted in 5-bit red and blue at the top
#define BREATH_LOW           179
#define BREATH_LIFT_R        2
#define BREATH_LIFT_B        3
#define BREATH_FIRST_ROW     8       // the breath is the secondary's own rows, 8..12; the primary's are left as loaded

static const u8 sThreadSlots[][3] = {   // row, index, share
    { 8, 6, THREAD_CORE_DIM },
    { 8, 7, THREAD_MID_DIM },
    { 8, 8, THREAD_HALO_DIM },
    { 10, 1, THREAD_CORE_DIM },
    { 10, 2, THREAD_MID_DIM },
    { 10, 3, THREAD_HALO_DIM },
};

// IWRAM: how lit it is (0 dark .. 256), how fast VERBOSE's light rises, and whether the field was here last frame.
static u16 sLit;
static u16 sRise;
static bool8 sSeen;

// THE FIELD STAGE (T-546's second pass, 2026-11-12). Laying the dark by CPU after the vblank's DMA was too late: the
// theatre filmed screen rows 0-3 of every dark frame in the LIT purples, drawn before the CPU reached them. So the
// map's rows 0..12 are built here, OUTSIDE the vblank, at the end of the main loop (Blackout_Tick, the last thing
// before WaitForVBlank) -- the faded buffer, then the pulse and the aurora on it as they always laid themselves, then
// the dark or the breath -- and palette.c DMAs this stage over PLTT's first 208 entries instead of the buffer's, so
// nothing is written while the screen draws. Built whenever the field is on screen and THE BLACKOUT, the pulse or the
// aurora touches those rows: the pulse and the aurora wrote them by CPU after the DMA too, so they tore the same
// way, more faintly. The rest of PLTT is as before.
// A vblank can land while the stage is being built (a long frame: the theatre caught one in three while entering).
// So the stage is built a ROW at a time in a 16-entry scratch on the stack -- the buffer, the pulse, the aurora, the
// dark -- and only a finished row is copied in: whenever the vblank comes, every entry is this frame's or the last's
// finished colour, never a raw one, and the stage is always copied once it has been built on this visit. IWRAM, 416
// bytes (statics land there; EWRAM had 668 bytes left).
#define STAGE_ENTRIES   (BLACKOUT_ROWS * 16)
static u16 sStage[STAGE_ENTRIES];
static bool8 sStageReady;

bool8 Blackout_Here(void)
{
    return DaemonsFieldOnScreen() && gMapHeader.mapLayout != NULL
        && gMapHeader.mapLayout->secondaryTileset == &gTileset_RockTunnel;
}

static void Arrive(void)
{
    if (!sSeen)
    {
        // The field has just come to (or back to) THE BLACKOUT: as dark or as lit as VERBOSE has left it.
        sLit = Overworld_GetFlashLevel() != 0 ? 0 : BLACKOUT_LIT;
        sRise = 0;
        sSeen = TRUE;
    }
}

bool8 Blackout_IsDark(void)
{
    return Blackout_Here() && (!sSeen || sLit < BLACKOUT_LIT);
}

u16 Blackout_WinOut(u16 field)
{
    if (gMapHeader.mapLayout != NULL && gMapHeader.mapLayout->secondaryTileset == &gTileset_RockTunnel)
        return field | WINOUT_WIN01_BG1 | WINOUT_WIN01_BG2 | WINOUT_WIN01_BG3;
    return field;
}

void Blackout_Illuminate(u16 frames)
{
    if (!Blackout_Here())
        return;
    if (frames == 0)
        frames = 1;
    sRise = (BLACKOUT_LIT + frames - 1) / frames;
}

static u16 Share(u16 c, u32 share)
{
    return RGB((GET_R(c) * share) >> 8, (GET_G(c) * share) >> 8, (GET_B(c) * share) >> 8);
}

static u16 Breathe(u16 c, u32 phase)
{
    u32 f = BREATH_LOW + ((256 - BREATH_LOW) * phase) / 255;
    u32 r = (GET_R(c) * f + BREATH_LIFT_R * phase + 128) >> 8;
    u32 g = (GET_G(c) * f + 128) >> 8;
    u32 b = (GET_B(c) * f + BREATH_LIFT_B * phase + 128) >> 8;

    return RGB(min(r, 31), min(g, 31), min(b, 31));
}

static u16 Lerp(u16 a, u16 b, u32 k)    // k in 256ths, from a to b
{
    s32 r = GET_R(a) + (((s32)GET_R(b) - GET_R(a)) * (s32)k) / 256;
    s32 g = GET_G(a) + (((s32)GET_G(b) - GET_G(a)) * (s32)k) / 256;
    s32 bl = GET_B(a) + (((s32)GET_B(b) - GET_B(a)) * (s32)k) / 256;

    return RGB(r, g, bl);
}

static s32 ThreadShare(u32 e)
{
    u32 i;

    for (i = 0; i < ARRAY_COUNT(sThreadSlots); i++)
        if (e == sThreadSlots[i][0] * 16u + sThreadSlots[i][1])
            return sThreadSlots[i][2];
    return -1;
}

// The dark or the breath over entries lo..hi of `pal` (PLTT, or a row of the stage, indexed as PLTT is), from what is
// already in them.
static void Transform(u16 *pal, u32 lo, u32 hi)
{
    u32 e, phase = Pulse_Phase();

    // lit, only the breath's rows change; dark or rising, every row of the map's
    if (sLit >= BLACKOUT_LIT && lo < BREATH_FIRST_ROW * 16)
        lo = BREATH_FIRST_ROW * 16;
    for (e = lo; e < hi; e++)
    {
        u16 c = pal[e];
        s32 share = ThreadShare(e);
        u16 lit = (share < 0 && e >= BREATH_FIRST_ROW * 16 && (e & 15) != 0) ? Breathe(c, phase) : c;

        if (sLit >= BLACKOUT_LIT)
            pal[e] = lit;
        else
            pal[e] = Lerp(share < 0 ? 0 : Share(c, share), lit, sLit);
    }
}

// The stage, from the faded buffer as it stands, a finished row at a time: FALSE (and not ready) when nothing lays
// anything on these rows.
bool8 Blackout_BuildStage(void)
{
    bool8 here = Blackout_Here();
    u16 row[16];
    u16 *at;    // `row` indexed as PLTT is, for the layers that work on PLTT's entry numbers
    u32 p;

    if (!DaemonsFieldOnScreen() || !(here || Pulse_Touches(STAGE_ENTRIES) || Aurora_Touches(STAGE_ENTRIES)))
    {
        sStageReady = FALSE;
        return FALSE;
    }
    if (here)
        Arrive();
    for (p = 0; p < BLACKOUT_ROWS; p++)
    {
        at = row - p * 16;
        CpuCopy16(&gPlttBufferFaded[p * 16], row, sizeof(row));
        Pulse_LayTo(at, p * 16, p * 16 + 16);
        Aurora_LayTo(at, p * 16, p * 16 + 16);
        if (here)
            Transform(at, p * 16, p * 16 + 16);
        CpuCopy16(row, &sStage[p * 16], sizeof(row));
    }
    sStageReady = TRUE;
    return TRUE;
}

void Blackout_Tick(void)
{
    if (!Blackout_Here())
        sSeen = FALSE;
    else if (sRise != 0)
    {
        sLit += sRise;
        if (sLit >= BLACKOUT_LIT)
        {
            sLit = BLACKOUT_LIT;
            sRise = 0;
        }
    }
    Blackout_BuildStage();
}

// palette.c, both paths to PLTT: the stage over the map's rows (DMA, or a CPU copy outside the vblank), the faded buffer
// over the rest, then the pulse and the aurora on the rest only. FALSE: the stage is not ready, the old path is taken.
bool8 Blackout_CopyStage(bool8 dma)
{
    if (!sStageReady)
        return FALSE;
    if (dma)
    {
        DmaCopy16(3, sStage, (void *)PLTT, sizeof(sStage));
        DmaCopy16(3, &gPlttBufferFaded[STAGE_ENTRIES], (void *)(PLTT + sizeof(sStage)), PLTT_SIZE - sizeof(sStage));
    }
    else
    {
        CpuCopy16(sStage, (void *)PLTT, sizeof(sStage));
        CpuCopy16(&gPlttBufferFaded[STAGE_ENTRIES], (void *)(PLTT + sizeof(sStage)), PLTT_SIZE - sizeof(sStage));
    }
    Pulse_ApplyToPlttFrom(STAGE_ENTRIES);
    return TRUE;
}

// The old path's last step, when the stage was not ready: laid by CPU on PLTT, as before.
void Blackout_ApplyToPltt(void)
{
    if (!Blackout_Here())
    {
        sSeen = FALSE;
        return;
    }
    Arrive();
    Transform((u16 *)PLTT, 0, STAGE_ENTRIES);
}
