#include "global.h"
#include "main.h"
#include "daemons_rtc.h"
#include "daemons_time.h"

// T-268: DAY AND NIGHT (vision.md 9.21; decided by the user, 2026-09-25).
//
// WHICH CLOCK. The cartridge's own, when it has one (T-265): night falls when it is night where the player is.
// Without one -- a retail board, an emulator with its clock off -- 9.21's clock made of play time takes over, so
// every player still has nights: an hour of play is a day, fifteen minutes a watch. A new game starts in daylight.
//
// WHAT IT CHANGES. The light on outdoor maps (fieldmap.c's DaemonsFieldTint -> DaemonsTintForWatch, applied as each
// palette loads, so fades and weather start from it and nothing compounds), and on a few routes the wild table
// (wild_encounter.c). Interiors, caves and battles keep their own light; HALFTONE is grey at every hour, because
// grey outranks time. Nothing in the game says any of this.

EWRAM_DATA u8 gDaemonsWatchOverride = 0;
static EWRAM_DATA u8 sWatch = WATCH_DAY;
static EWRAM_DATA bool8 sWatchFromClock = FALSE;
static EWRAM_DATA u32 sWatchReadAt = 0;
static EWRAM_DATA bool8 sWatchRead = FALSE;

// The clock's hours: dawn 5-7, day 7-18, dusk 18-20, night 20-5.
static u8 WatchFromHour(u8 hour)
{
    if (hour >= 5 && hour < 7)
        return WATCH_DAWN;
    if (hour >= 7 && hour < 18)
        return WATCH_DAY;
    if (hour >= 18 && hour < 20)
        return WATCH_DUSK;
    return WATCH_NIGHT;
}

// Play time's: day, dusk, night, dawn, fifteen minutes each -- day first, so the opening is in daylight.
static const u8 sPlayTimeWatches[WATCH_COUNT] = { WATCH_DAY, WATCH_DUSK, WATCH_NIGHT, WATCH_DAWN };

u8 DaemonsWatch(void)
{
    struct DaemonsClock clock;

    if (gDaemonsWatchOverride)
        return (gDaemonsWatchOverride - 1) % WATCH_COUNT;
    // Asked on every step through grass and every palette load, and a clock read is a few thousand cycles of
    // bit-banging, so the answer is kept for a second.
    if (!sWatchRead || gMain.vblankCounter2 - sWatchReadAt >= 60)
    {
        sWatchFromClock = DaemonsRtc_Read(&clock);
        if (sWatchFromClock)
            sWatch = WatchFromHour(clock.hour);
        else
            sWatch = sPlayTimeWatches[(gSaveBlock2Ptr->playTimeMinutes / 15) % WATCH_COUNT];
        sWatchReadAt = gMain.vblankCounter2;
        sWatchRead = TRUE;
    }
    return sWatch;
}

bool8 DaemonsWatchIsFromClock(void)
{
    DaemonsWatch();
    return sWatchFromClock;
}

// Each channel scaled and lifted: out = c * mul / 256 + add. Dusk warm and dimmer, night a blue-dark that keeps
// shapes readable (white becomes a pale navy, black a deep one), dawn a cool, pale half-light.
static const u8 sWatchTint[WATCH_COUNT][3][2] = {
    [WATCH_DAY]   = { { 255, 0 }, { 255, 0 }, { 255, 0 } },
    [WATCH_DUSK]  = { { 250, 1 }, { 200, 0 }, { 160, 1 } },
    [WATCH_NIGHT] = { { 112, 1 }, { 128, 2 }, { 176, 4 } },
    [WATCH_DAWN]  = { { 232, 1 }, { 226, 1 }, { 240, 3 } },
};

void DaemonsTintForWatch(u16 *palette, u16 count)
{
    const u8 (*t)[2] = sWatchTint[DaemonsWatch()];
    u16 i;
    s32 r, g, b;

    for (i = 0; i < count; i++)
    {
        r = ((palette[i] & 0x1F) * t[0][0] >> 8) + t[0][1];
        g = (((palette[i] >> 5) & 0x1F) * t[1][0] >> 8) + t[1][1];
        b = (((palette[i] >> 10) & 0x1F) * t[2][0] >> 8) + t[2][1];
        palette[i] = RGB2(min(r, 31), min(g, 31), min(b, 31));
    }
}
