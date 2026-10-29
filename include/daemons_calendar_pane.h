#ifndef GUARD_DAEMONS_CALENDAR_PANE_H
#define GUARD_DAEMONS_CALENDAR_PANE_H

// T-481 B: THE MENUS AND PANES THAT KEEP THE CALENDAR (src/daemons_calendar_pane.c).
#define CALENDAR_PANE_TEXT_BOX  0   // the dialogue frame, BG palette 15 (LoadStdWindowFrameGfx)
#define CALENDAR_PANE_START     1   // the START menu's frame, BG palette 14 (DoDrawStartMenu)
#define CALENDAR_PANE_SUMMARY   2   // the SUMMARY's backdrop, BG palette 0 (PokeSum_HandleLoadBgGfx and a switch)
#define CALENDAR_PANE_INDEX     3   // the INDEX's page, BG palette 0 (DexScreen_LoadResources)
#define CALENDAR_PANE_COUNT     4

// After the pane's palette row `pal` (a BG row, 0-15) loads: the day, a holiday and the hour on its frame or backdrop
// slots, into both palette buffers. Never the paper and never an ink.
void DaemonsCalendarPane(u8 pal, u8 surface);

#endif // GUARD_DAEMONS_CALENDAR_PANE_H
