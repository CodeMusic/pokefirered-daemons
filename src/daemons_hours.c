#include "global.h"
#include "main.h"
#include "daemons_hours.h"
#include "daemons_time.h"

// T-519 item 4: THE TWO HOURS (the user, 2026-11-02 and 2026-11-04, by multiple choice).
//
// THE CATCH-UP. The player may be anywhere in an hour when play begins -- a new game, a save continued after power on
// or a soft reset -- so neither hour's effect jumps to the clock's level: nothing for the first minute of play, then a
// smoothstep over two minutes up to wherever the clock has it, and from then on the clock alone. Passing into an hour
// while playing is not a beginning: three minutes in, the catch-up is whole and the hour's own shape applies. What it
// eases: the witching hour's glitch (daemons_glitch.c), its pulse over the map and the battle's ground
// (daemons_pulse.c) and the party's shimmer (daemons_witching.c). The moment play began is one EWRAM word and a byte,
// never saved, and lost on a reset as it should be.

#define FRAMES_PER_MINUTE   3584    // 59.73 frames a second
#define CATCHUP_QUIET       FRAMES_PER_MINUTE
#define CATCHUP_EASE        (2 * FRAMES_PER_MINUTE)

static EWRAM_DATA u32 sPlayBeganAt = 0;
static EWRAM_DATA bool8 sPlayBegan = FALSE;

//  ---------------------------------------------------------------- the catch-up

void Hours_PlayBegan(void)
{
    sPlayBeganAt = gMain.vblankCounter2;
    sPlayBegan = TRUE;
}

void Hours_DebugRestart(void)
{
    Hours_PlayBegan();
}

// 0-256 from NUM / DEN, eased at both ends: 3x^2 - 2x^3.
static u16 Smooth(u32 num, u32 den)
{
    u32 x;

    if (num >= den)
        return 256;
    x = num * 256 / den;
    return x * x * (768 - 2 * x) / 65536;
}

u16 Hours_CatchUp(void)
{
    u32 frames;

    if (!sPlayBegan)
        return 0;
    frames = gMain.vblankCounter2 - sPlayBeganAt;
    if (frames < CATCHUP_QUIET)
        return 0;
    return Smooth(frames - CATCHUP_QUIET, CATCHUP_EASE);
}

u16 Hours_WitchingEase(void)
{
    if (!DaemonsIsWitchingHour())
        return 256;
    return Hours_CatchUp();
}
