#include "global.h"
#include "main.h"
#include "palette.h"
#include "overworld.h"
#include "fieldmap.h"
#include "daemons_pulse.h"
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

// IWRAM: how lit it is (0 dark .. 256), how fast VERBOSE's light rises, and whether the field was here last copy.
static u16 sLit;
static u16 sRise;
static bool8 sSeen;

bool8 Blackout_Here(void)
{
    return DaemonsFieldOnScreen() && gMapHeader.mapLayout != NULL
        && gMapHeader.mapLayout->secondaryTileset == &gTileset_RockTunnel;
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

void Blackout_Tick(void)
{
    if (!Blackout_Here())
        return;
    if (sRise != 0)
    {
        sLit += sRise;
        if (sLit >= BLACKOUT_LIT)
        {
            sLit = BLACKOUT_LIT;
            sRise = 0;
        }
    }
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

void Blackout_ApplyToPltt(void)
{
    u16 *pltt = (u16 *)PLTT;
    u32 e, phase;

    if (!Blackout_Here())
    {
        sSeen = FALSE;
        return;
    }
    if (!sSeen)
    {
        // The field has just come to (or back to) THE BLACKOUT: as dark or as lit as VERBOSE has left it.
        sLit = Overworld_GetFlashLevel() != 0 ? 0 : BLACKOUT_LIT;
        sRise = 0;
        sSeen = TRUE;
    }
    phase = Pulse_Phase();
    // lit, only the breath's rows change; dark or rising, every row of the map's
    for (e = sLit >= BLACKOUT_LIT ? BREATH_FIRST_ROW * 16 : 0; e < BLACKOUT_ROWS * 16; e++)
    {
        u16 c = pltt[e];
        s32 share = ThreadShare(e);
        u16 lit = (share < 0 && e >= BREATH_FIRST_ROW * 16 && (e & 15) != 0) ? Breathe(c, phase) : c;

        if (sLit >= BLACKOUT_LIT)
            pltt[e] = lit;
        else
            pltt[e] = Lerp(share < 0 ? 0 : Share(c, share), lit, sLit);
    }
}
