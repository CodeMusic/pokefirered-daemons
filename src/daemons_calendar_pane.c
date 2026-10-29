#include "global.h"
#include "palette.h"
#include "fieldmap.h"
#include "daemons_time.h"
#include "daemons_holidays.h"
#include "daemons_calendar_pane.h"

// T-481 B: THE MENUS AND PANES THAT KEEP THE CALENDAR (the user, 2026-10-29, by multiple choice on the draft's sheets,
// DAEMONS tools/gbacalendar_ui.py, which measures each pane's drawing and writes the table below).
//
// Four panes -- the text box, the START menu's frame, the SUMMARY and the INDEX -- each take, after its palette loads:
//
//   THE WEEKDAY  one sparse slot in the day's CHECKPOINT trim (day_trims.h): the text box's outer-corner flecks,
//                START's eight inner-corner pixels (T-318's wash of the whole frame is retired: the frame keeps its
//                style's own colours), the SUMMARY's 1px line under the title bar, the INDEX's box corners.
//   A HOLIDAY    one sparse slot in the month colour the holiday wears, never on a QUIET one: the text box's inner
//                corner flecks, START's inner bevel, the SUMMARY's title pip; the INDEX has no other free colour, so
//                its corners take the month for that day and the day yields.
//   THE HOUR     sWatchTint at HALF strength on the frame or backdrop slots only -- never the paper, never an ink, so
//                the text's contrast is the day's. Day is as drawn; BREATHVERSE is timeless (T-417).
//
// No season in the menus (a near-white paper is boxed in between ICE's hue and NORMAL's -- the draft's report). Every
// colour added is a day trim or a month colour itself, each 22+ from every type hue (9.4); a blend of either fails.
// START's day and holiday slots exist only in the one drawing frame styles 1, 3, 4 and 5 share; the other styles are
// other drawings and take the hour alone.
//
// COST: EWRAM 0, IWRAM 0, save 0 -- the clock is asked when the pane loads, and the result goes into the palette
// buffers that already exist. T-483's pulse, the fades and HALFTONE's grey come after, at the copy to PLTT.

#include "data/calendar_panes.h"
#include "data/day_trims.h"

void DaemonsCalendarPane(u8 pal, u8 surface)
{
    const struct CalendarPane *p;
    u16 *row = &gPlttBufferUnfaded[BG_PLTT_ID(pal)];
    u8 holiday, watch, i;

    if (surface >= CALENDAR_PANE_COUNT)
        return;
    p = &sCalendarPanes[surface];
    if (surface != CALENDAR_PANE_START || ((p->styleMask >> gSaveBlock2Ptr->optionsWindowFrameType) & 1))
    {
        row[p->day] = sDayTrims[DaemonsWeekday()];
        holiday = DaemonsHoliday();
        if (holiday != HOLIDAY_NONE && !(DaemonsHolidayFlags(holiday) & HF_QUIET))
            row[p->holiday] = DaemonsMonthColour(DaemonsHolidayMonth(holiday));
    }
    watch = DaemonsWatch();
    if (watch != WATCH_DAY && !DaemonsIsTimeless())
    {
        for (i = 1; i < 16; i++)
        {
            if ((p->hourMask >> i) & 1)
                DaemonsTintBattleForWatch(&row[i], 1, watch, 2);
        }
    }
    CpuCopy16(row, &gPlttBufferFaded[BG_PLTT_ID(pal)], PLTT_SIZE_4BPP);
}
