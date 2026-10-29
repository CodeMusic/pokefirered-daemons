#include "global.h"
#include "main.h"
#include "palette.h"
#include "overworld.h"
#include "battle.h"
#include "battle_main.h"
#include "fieldmap.h"
#include "daemons_time.h"
#include "daemons_hermetic.h"
#include "daemons_pulse.h"

// T-483 / T-482: THE PULSE (the user, 2026-10-29; bible 4.39). One breath for the app, the watches and the game, so
// boards side by side and a cartridge beside them breathe together: CONTENT's red to CONTEXT's blue through THE SUBTLE
// LAYER's purple -- red and blue made one -- in THREE STOPS, never a straight blend (a straight one passes through a
// muddy grey-violet, not the book's purple).
//
//   red    #C8405A  GBA (25,  8, 11)
//   purple #B0569E  GBA (22, 10, 19)    tools/gensubtle.py's C_RULE
//   blue   #4F6BD8  GBA ( 9, 13, 27)
//
// THE BREATH. s = (1 - cos(2 pi t / 8000)) / 2, t the milliseconds since midnight on the cartridge's clock: one breath
// every eight seconds, red at s = 0 and blue at s = 1 (the companion's server/src/witching.ts breathOf, and the
// boards' breathNow). Eight seconds divide a day, so only t mod 8000 matters. The clock gives whole seconds, read once
// a second (daemons_time.c); between reads the frame counter carries it, at the GBA's 59.73 frames a second, and each
// fresh read nudges it back inside the second the clock says. Without a clock it breathes on play time. No float: the
// cosine is a table of 64 steps (one every 125 ms), interpolated.
//
// WHERE IT SHOWS. The book's backdrop and its inscriptions (book_reader.c); the party's shimmer (daemons_witching.c);
// and, through Pulse_ApplyToPltt, the overworld's map palettes (with THE HERMETIC LAYER held, from 3 to 4 AM on the
// clock, and all Halloween) and the battle's ground (T-482: in the hour with THE SUBTLE LAYER held). People, daemons,
// the battlers and the text keep their own colours; HALFTONE stays grey.

static const u8 sBreath[65] = {
    0, 1, 2, 5, 10, 15, 21, 29, 37, 47, 57, 67, 79, 90, 103, 115, 127, 140, 152, 165, 176, 188, 198, 208, 218, 226,
    234, 240, 245, 250, 253, 254, 255, 254, 253, 250, 245, 240, 234, 226, 218, 208, 198, 188, 176, 165, 152, 140,
    128, 115, 103, 90, 79, 67, 57, 47, 37, 29, 21, 15, 10, 5, 2, 1, 0,
};

static const u8 sStops[3][3] = {
    { 25,  8, 11 },     // red
    { 22, 10, 19 },     // purple
    {  9, 13, 27 },     // blue
};

#define FRAME_US   16742    // a GBA frame, in microseconds (59.73 Hz)

// IWRAM, uninitialised: the anchor between clock reads, and this frame's pulse (worked out once in the main loop, so
// the vblank only blends).
static u32 sAnchorFrame;
static u32 sAnchorRead;
static u16 sAnchorMs;
static bool8 sAnchored;
static u32 sPulseMask;      // palettes pulsing this frame: bit p is BG palette p, bit 16 + p OBJ palette p
static u16 sPulseColour;
static u8 sPulseWeight;

static u16 Since(u16 ms, u32 frames)
{
    return (ms + frames * FRAME_US / 1000) % 8000;
}

u16 Pulse_Ms(void)
{
    u32 second, readAt, now = gMain.vblankCounter2, rtc, atRead, diff;

    if (!DaemonsPulseClock(&second, &readAt))
        return (gSaveBlock2Ptr->playTimeSeconds % 8) * 1000 + gSaveBlock2Ptr->playTimeVBlanks * 1000 / 60;
    if (!sAnchored || readAt != sAnchorRead)
    {
        rtc = (second % 8) * 1000;
        if (!sAnchored || now - sAnchorFrame > 600)
        {
            atRead = rtc + 500;                     // the first read: the middle of the second it names
        }
        else
        {
            atRead = Since(sAnchorMs, readAt - sAnchorFrame);
            diff = (atRead + 8000 - rtc) % 8000;
            if (diff >= 1000)                       // outside the second the clock names: back to its nearer edge
                atRead = (diff > 4500) ? rtc : rtc + 999;
        }
        sAnchorMs = atRead % 8000;
        sAnchorFrame = readAt;
        sAnchorRead = readAt;
        sAnchored = TRUE;
    }
    return Since(sAnchorMs, now - sAnchorFrame);
}

u8 Pulse_Phase(void)
{
    u16 ms = Pulse_Ms();
    u8 i = ms / 125, frac = ms % 125;

    return sBreath[i] + ((s32)(sBreath[i + 1] - sBreath[i]) * frac) / 125;
}

static u16 Lerp(const u8 *a, const u8 *b, u32 k)   // k in 256ths
{
    s32 r = a[0] + (((s32)b[0] - a[0]) * (s32)k) / 256;
    s32 g = a[1] + (((s32)b[1] - a[1]) * (s32)k) / 256;
    s32 bl = a[2] + (((s32)b[2] - a[2]) * (s32)k) / 256;

    return RGB(r, g, bl);
}

u16 Pulse_Colour(u8 phase)
{
    if (phase < 128)
        return Lerp(sStops[0], sStops[1], phase * 2);
    return Lerp(sStops[1], sStops[2], (phase - 128) * 2 + 2);
}

u16 Pulse_Now(void)
{
    return Pulse_Colour(Pulse_Phase());
}

// The tablet's emerald, #1F8F5F (3, 17, 11), rising to #50C878 (10, 25, 15) as the breath reaches blue.
u16 Pulse_Emerald(u8 phase)
{
    static const u8 deep[3] = { 3, 17, 11 }, lit[3] = { 10, 25, 15 };

    return Lerp(deep, lit, phase);
}

u16 Pulse_Blend(u16 base, u16 target, u8 weight)
{
    s32 r = GET_R(base), g = GET_G(base), b = GET_B(base);

    r += ((GET_R(target) - r) * weight) >> 8;
    g += ((GET_G(target) - g) * weight) >> 8;
    b += ((GET_B(target) - b) * weight) >> 8;
    return RGB(r, g, b);
}

#define BG_ROWS(first, last) ((((u32)1 << ((last) + 1)) - 1) & ~(((u32)1 << (first)) - 1))

void Pulse_Tick(void)
{
    u32 mask = 0;

    if (DaemonsFieldOnScreen())
    {
        // The map's own rows (0-12); 13-15 are the windows and the text. Never HALFTONE: grey outranks the pulse.
        if (Hermetic_WorldPulses() && !DaemonsIsHalftone())
            mask = BG_ROWS(0, 12);
    }
    sPulseMask = mask;
    if (mask == 0)
        return;
    sPulseColour = Pulse_Now();
    sPulseWeight = PULSE_BG_WEIGHT;
    // A fade takes the pulse with it, so a fade to black is black and the field comes back into the breath.
    if (gPaletteFade.active || gPaletteFade.y != 0)
        sPulseWeight = PULSE_BG_WEIGHT * (16 - min(gPaletteFade.y, 16)) / 16;
}

void Pulse_ApplyToPltt(void)
{
    u32 mask = sPulseMask, p, e;
    u16 *pltt = (u16 *)PLTT;
    const u16 *faded = gPlttBufferFaded;
    s32 tr, tg, tb, w = sPulseWeight;

    if (mask == 0 || w == 0)
        return;
    tr = GET_R(sPulseColour);
    tg = GET_G(sPulseColour);
    tb = GET_B(sPulseColour);
    for (p = 0; mask != 0; p++, mask >>= 1)
    {
        if (!(mask & 1))
            continue;
        for (e = p * 16; e < p * 16 + 16; e++)
        {
            u16 c = faded[e];
            s32 r = GET_R(c), g = GET_G(c), b = GET_B(c);

            r += ((tr - r) * w) >> 8;
            g += ((tg - g) * w) >> 8;
            b += ((tb - b) * w) >> 8;
            pltt[e] = RGB(r, g, b);
        }
    }
}
