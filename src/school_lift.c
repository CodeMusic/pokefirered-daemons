#include "global.h"
#include "fieldmap.h"
#include "palette.h"
#include "school_lift.h"

//  THE LIFT'S LAMP (the user, 2026-10-03: "above the elevator in that little white panel there should be green
//  illuminated in the middle ... between the selection and when you are at the new floor ready to move ... it should
//  appear blue"). tools/gbainterior.py draws the lamp in one colour nothing else in the building uses, LAMP_GREEN,
//  so the game finds the lamp's palette slot by that colour in the school's own tileset palette rather than by a
//  number a redraw could move. Green is the lamp at rest. Choosing a floor turns it blue; the floor you arrive on
//  loads it blue while the ride is still going; and the first frame the player can move again turns it green.

#define LAMP_GREEN  RGB(5, 29, 12)      // gbainterior.py's C_LIFTLAMP, (40, 232, 96), at five bits
#define LAMP_BLUE   RGB(6, 16, 31)

static EWRAM_DATA bool8 sRiding = FALSE;
static EWRAM_DATA u16 sLampAtRest = 0;   // the lamp's colour as this floor loaded it (a faded print fades it too)

//  The lamp's slot in the BG palettes, or -1 when this map's secondary tileset has no lamp (it is not the school).
static s16 FindLampSlot(void)
{
    const struct Tileset *ts = gMapHeader.mapLayout ? gMapHeader.mapLayout->secondaryTileset : NULL;
    u32 row, i;

    if (ts == NULL || ts->palettes == NULL)
        return -1;
    for (row = NUM_PALS_IN_PRIMARY; row < NUM_PALS_TOTAL; row++)
    {
        for (i = 1; i < 16; i++)
        {
            if ((ts->palettes[row][i] & 0x7FFF) == LAMP_GREEN)
                return row * 16 + i;
        }
    }
    return -1;
}

//  The colour at rest was faded with the rest of the floor as it loaded (T-317's faded print); the blue is faded here
//  the same way, or it is the one thing in a grey building at full colour (found in the theatre, T-357).
static void SetLamp(s16 slot, u16 color, bool8 tint)
{
    gPlttBufferUnfaded[slot] = color;
    if (tint)
        DaemonsWashTileColour(slot);
    if (!gPaletteFade.active)
        gPlttBufferFaded[slot] = gPlttBufferUnfaded[slot];
}

//  special: a floor has been chosen.
void School_LiftLampBlue(void)
{
    s16 slot = FindLampSlot();

    if (slot < 0)
        return;
    sLampAtRest = gPlttBufferUnfaded[slot];
    SetLamp(slot, LAMP_BLUE, TRUE);
    sRiding = TRUE;
}

//  fieldmap.c, after a map's tileset palettes load: the floor being arrived at shows the ride still going.
void SchoolLift_OnTilesetPalettesLoaded(void)
{
    s16 slot;

    if (!sRiding)
        return;
    slot = FindLampSlot();
    if (slot < 0)
    {
        sRiding = FALSE;            // arrived somewhere with no lamp: nothing to light
        return;
    }
    sLampAtRest = gPlttBufferUnfaded[slot];
    SetLamp(slot, LAMP_BLUE, TRUE);
}

//  field_control_avatar.c, every frame the player has control: the ride is over.
void SchoolLift_OnPlayerReady(void)
{
    s16 slot;

    if (!sRiding)
        return;
    sRiding = FALSE;
    slot = FindLampSlot();
    if (slot >= 0)
        SetLamp(slot, sLampAtRest, FALSE);
}
