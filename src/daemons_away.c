#include "global.h"
#include "palette.h"
#include "pokemon.h"
#include "pokemon_icon.h"
#include "sprite.h"
#include "daemons_away.h"

//  T-358 (vision 9.25): AWAY, WASHED OUT. The daemon is on the player's device, so wherever the game draws it, it
//  is drawn paler -- still itself, only further off: each colour taken halfway to its own grey, then a third of the
//  way to white. Nothing here reads or changes the AWAY bit; the callers ask DaemonIsAway and call these.

#define AWAY_ICON_PAL_TAG 56016     // beside pokemon_icon.c's POKE_ICON_BASE_PAL_TAG (56000) and its three palettes

void DaemonsWashAway(u16 *palette, u16 count)
{
    u16 i;
    s32 r, g, b, grey;

    for (i = 0; i < count; i++)
    {
        r = palette[i] & 0x1F;
        g = (palette[i] >> 5) & 0x1F;
        b = (palette[i] >> 10) & 0x1F;
        grey = (r * 77 + g * 150 + b * 29) >> 8;
        r = (r + grey) / 2;
        g = (g + grey) / 2;
        b = (b + grey) / 2;
        r += (31 - r) / 3;
        g += (31 - g) / 3;
        b += (31 - b) / 3;
        palette[i] = RGB2(r, g, b);
    }
}

//  A palette already loaded -- a summary or PORT picture, after its streaks (T-132) are on it.
void DaemonsWashAwayLoaded(u16 paletteOffset, u16 count)
{
    DaemonsWashAway(&gPlttBufferUnfaded[paletteOffset], count);
    CpuCopy16(&gPlttBufferUnfaded[paletteOffset], &gPlttBufferFaded[paletteOffset], count * sizeof(u16));
}

//  An icon shares its palette with every other icon of the same colour group, so it cannot be washed in place: it is
//  pointed at a washed copy, loaded the first time one is needed and freed with the rest of the screen's palettes.
void DaemonsWashAwayIcon(struct Sprite *sprite, u16 species)
{
    u8 group, slot;

    if (sprite == NULL)
        return;
    group = gMonIconPaletteIndices[species > NUM_SPECIES ? SPECIES_NONE : species];
    slot = IndexOfSpritePaletteTag(AWAY_ICON_PAL_TAG + group);
    if (slot == 0xFF)
    {
        u16 washed[16];
        struct SpritePalette palette = { washed, AWAY_ICON_PAL_TAG + group };

        CpuCopy16(gMonIconPalettes[group], washed, sizeof(washed));
        DaemonsWashAway(washed, ARRAY_COUNT(washed));
        slot = LoadSpritePalette(&palette);
        if (slot == 0xFF)
            return;     // no palette slot free: the icon keeps its colour rather than borrow someone else's
    }
    sprite->oam.paletteNum = slot;
}

void DaemonsWashAwayIconIf(struct Sprite *sprite, bool8 away)
{
    if (away && sprite != NULL)
        DaemonsWashAwayIcon(sprite, sprite->data[0]);
}
