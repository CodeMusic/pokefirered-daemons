#include "global.h"
#include "daemons_glitch.h"
#include "daemons_time.h"

// T-485 (the user, 2026-10-29; bible 4.39): OSRIC HATTER's house on QUICKSILVER, the leaning top hat on the south
// sand. Inside every clock is stopped at half past three, so from 3:30 to 3:59 by the cartridge's clock his clocks are
// right, and he says so (Hatter_ClocksAreRight; his lines are the map's, data/maps/CinnabarIsland_HattersHouse). And
// it is the second of the two places DAEMONS glitches on purpose (T-486): every entry from 3:00 to 3:59 glitches once,
// at that minute's strength (Hatter_Glitch, from the map's frame script on entering) -- and his 3:30 line once more between its
// boxes. The room is drawn on its bottom layer, which the field puts on BG2; its palettes are the map's thirteen.

#define HATTER_MAP_PALETTES 0x1FFF    // BG palettes 0-12: the building's seven and the room's six

// specialvar: TRUE from 3:30 to 3:59 (DEBUG WATCH's 3:30 AM too)
bool8 Hatter_ClocksAreRight(void)
{
    return DaemonsWitchingMinute() >= 30;
}

// specialvar: the glitch, at the clock's strength, when it is the witching hour and not otherwise; TRUE when one began
// (over in 12 frames at most: the script then waits 16, and does not wait at all when it is FALSE)
bool8 Hatter_Glitch(void)
{
    if (DaemonsWitchingMinute() < 0)
        return FALSE;
    Glitch_Start(2, HATTER_MAP_PALETTES, Glitch_StrengthNow());
    return Glitch_IsRunning();
}
