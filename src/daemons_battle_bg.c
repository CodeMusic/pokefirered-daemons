#include "global.h"
#include "palette.h"
#include "fieldmap.h"
#include "overworld.h"
#include "constants/battle.h"
#include "constants/map_types.h"
#include "daemons_time.h"
#include "daemons_holidays.h"
#include "daemons_battle_bg.h"

// T-481 A: THE BATTLE BACKGROUNDS THAT KEEP THE CALENDAR (the user, 2026-10-29, by multiple choice; drafted on sheets
// by DAEMONS tools/gbabattlebg.py, which writes the tables below and draws what this does).
//
// A terrain is one tileset, one tilemap and three BG palette rows, 2-4: row 2 the sky bands (10-14), the field (15),
// the foe's platform (2-9) and the scan line (1); row 3 the player's platform, whose 10-14 no tile draws; row 4 the
// intro's tufts. In this order, after the palette loads and before the weather's wash (daemons_weather.c):
//
//   1. THE SEASON -- outdoors only: the ground, both platforms and the tufts from the tool's table (summer a light
//      sun-bleach; spring's platform glints in bloom). The sky is left to the hour.
//   2. THE HOLIDAY -- each platform's glint (its brightest sparse colour) in the month colour the holiday wears. Not
//      on a QUIET holiday; an ordinary day changes nothing.
//   3. THE WEEKDAY -- one line, the horizon: DaemonsBattleBgHorizon moves the backdrop's last scan line above the field
//      to colour 12 of row 3 (four bytes of one tile, and its row's map entries to row 3), and row 3's other free
//      colours copy row 2's, so the line and only the line takes the day's CHECKPOINT trim (day_trims.h).
//   4. THE HOUR -- the field's own light (daemons_time.c sWatchTint) on all 48 colours, and on the sky bands a step
//      toward the watch's sky (rose at dawn, amber at dusk, navy at night). Day is as drawn.
//
// THE REACH. Outdoors: all four. A building: the hour at a quarter ("through the windows"), no season. A cave keeps its
// own light, and so does any outdoor terrain met underground (a cave's water). The BENCHMARK arenas, the gyms and
// the Elite rooms: the weekday's line and the holiday's glints only. BREATHVERSE is timeless (T-417): no season and
// no hour there either. Then T-415's weather wash runs as it did, and T-482's pulse on top of everything, at the copy
// to PLTT (daemons_pulse.c); HALFTONE's grey over all of it.
//
// COST: EWRAM 0, IWRAM 0, save 0 -- every input is asked of the clock when the terrain loads, and the result goes into
// the palette buffers that already exist.

#include "data/battle_calendar.h"
#include "data/day_trims.h"

static u16 Step(u16 c, u16 target, u8 sixteenths)
{
    // tools/gbabattlebg.py's lerp: a + (b - a) * k // 16, floored as Python floors (an arithmetic shift)
    s32 r = GET_R(c), g = GET_G(c), b = GET_B(c);

    r += ((GET_R(target) - r) * sixteenths) >> 4;
    g += ((GET_G(target) - g) * sixteenths) >> 4;
    b += ((GET_B(target) - b) * sixteenths) >> 4;
    return RGB(r, g, b);
}

static u8 Reach(const struct BattleCalendarTerrain *t)
{
    u8 reach = t->reach;

    if (DaemonsIsTimeless())
        return BATTLE_CAL_KEPT;
    // An outdoor terrain met where the map is not outdoors: a cave's water keeps the cave's light, a room the room's.
    if (reach == BATTLE_CAL_FULL && !IsMapTypeOutdoors(gMapHeader.mapType))
        reach = (gMapHeader.mapType == MAP_TYPE_UNDERGROUND) ? BATTLE_CAL_KEPT : BATTLE_CAL_INDOOR;
    return reach;
}

void DaemonsBattleBgHorizon(u16 terrain)
{
    const struct BattleCalendarTerrain *t;
    u16 *map = (u16 *)BG_SCREEN_ADDR(26);
    u32 i, row;

    if (terrain >= NELEMS(sBattleCalendar))
        terrain = BATTLE_TERRAIN_PLAIN;
    t = &sBattleCalendar[terrain];
    // The line's pixel row of the horizon tile, colour 1 -> 12 (a 32-bit write: VRAM takes no single bytes).
    *(vu32 *)((u8 *)BG_CHAR_ADDR(2) + t->horizonTile * TILE_SIZE_4BPP + t->horizonLine * 4) = 0x11111111u * BATTLE_CAL_ACCENT;
    // That tile's map entries, in both screens (BG3_X is 256 in the evolution scene), to palette row 3.
    for (row = t->horizonRow; row < 64; row += 32)
    {
        for (i = row * 32; i < row * 32 + 32; i++)
        {
            if ((map[i] & 0x3FF) == t->horizonTile)
                map[i] = (map[i] & 0x0FFF) | (3 << 12);
        }
    }
}

void DaemonsBattleBgCalendar(u16 terrain)
{
    const struct BattleCalendarTerrain *t;
    u16 *pal = &gPlttBufferUnfaded[BG_PLTT_ID(2)];
    u8 reach, holiday, watch, i, row, k;
    u16 colour;

    if (terrain >= NELEMS(sBattleCalendar))
        terrain = BATTLE_TERRAIN_PLAIN;
    t = &sBattleCalendar[terrain];
    reach = Reach(t);

    // 1. the season
    if (reach == BATTLE_CAL_FULL && t->index != BATTLE_CAL_NO_SEASON)
        CpuCopy16(sBattleSeasonPals[t->index][DaemonsSeason()], pal, PLTT_SIZEOF(3 * 16));

    // 2. the holiday
    holiday = DaemonsHoliday();
    if (holiday != HOLIDAY_NONE && !(DaemonsHolidayFlags(holiday) & HF_QUIET))
    {
        colour = DaemonsMonthColour(DaemonsHolidayMonth(holiday));
        for (row = 0; row < 2; row++)
        {
            if (t->glint[row])
                pal[row * 16 + t->glint[row]] = colour;
        }
    }

    // 3. the weekday: row 3's free colours copy row 2's, and the accent takes the day's trim
    for (i = 1; i < 16; i++)
    {
        if (t->freeMask & (1 << i))
            pal[16 + i] = pal[i];
    }
    pal[16 + BATTLE_CAL_ACCENT] = sDayTrims[DaemonsWeekday()];

    // 4. the hour
    watch = DaemonsWatch();
    if (watch != WATCH_DAY && (reach == BATTLE_CAL_FULL || reach == BATTLE_CAL_INDOOR))
    {
        DaemonsTintBattleForWatch(pal, 3 * 16, watch, reach == BATTLE_CAL_FULL ? 4 : 1);
        k = sBattleSkyStep[watch];
        if (reach == BATTLE_CAL_FULL && t->index != BATTLE_CAL_NO_SEASON && k != 0)
        {
            for (i = 0; i < NELEMS(t->sky) && t->sky[i] != 0; i++)
            {
                pal[t->sky[i]] = Step(pal[t->sky[i]], sBattleSkyTargets[t->index][watch][i], k);
                pal[16 + t->sky[i]] = Step(pal[16 + t->sky[i]], sBattleSkyTargets[t->index][watch][i], k);
            }
            // the day's line a quarter step toward the horizon's sky, so it sits in the sky it is drawn across
            pal[16 + BATTLE_CAL_ACCENT] = Step(pal[16 + BATTLE_CAL_ACCENT], sBattleSkyLow[watch], k / 4);
        }
    }
    CpuCopy16(pal, &gPlttBufferFaded[BG_PLTT_ID(2)], PLTT_SIZEOF(3 * 16));
}
