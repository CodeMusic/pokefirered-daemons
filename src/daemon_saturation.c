#include "global.h"
#include "battle.h"
#include "battle_anim.h"
#include "palette.h"
#include "pokemon.h"
#include "daemon_saturation.h"

// T-424: SATURATION IS FRIENDSHIP -- see include/daemon_saturation.h. DRAFT, its curve too:
//
//   keep(f) = 31 + 225 * (1 - (1 - f/255)^2)      in 256ths: an ease-out, quick at first and settling at the top
//
//   friendship    0    35    70   100   150   200   255
//   kept         12%   34%   54%   67%   85%   96%  100%
//
// so the first friendship a daemon earns shows most, a caught or hatched one (70, most species' base) is about half
// way, and a Corpus daemon (0, battle_main.c) keeps only the hint of its type. Each colour moves toward its own grey
// (the GBA's five-bit channels weighted 77 : 150 : 29, as luminance is), the same amount in each channel.

#define KEEP_LOW  31
#define KEEP_SPAN 225

u16 Saturation_OfFriendship(u8 friendship)
{
    u32 rest = 255 - friendship;

    return KEEP_LOW + KEEP_SPAN * (65025 - rest * rest) / 65025;
}

static u16 Desaturate(u16 colour, u16 keep)
{
    s32 r = colour & 31, g = (colour >> 5) & 31, b = (colour >> 10) & 31;
    s32 grey = (r * 77 + g * 150 + b * 29 + 128) >> 8;

    r = grey + (r - grey) * keep / 256;
    g = grey + (g - grey) * keep / 256;
    b = grey + (b - grey) * keep / 256;
    return RGB(r, g, b) | (colour & 0x8000);
}

void Saturation_ApplyToBuffer(u16 *palette, u8 friendship)
{
    u16 keep = Saturation_OfFriendship(friendship);
    s32 i;

    if (keep >= 256)
        return;
    for (i = 1; i < 16; i++)
        palette[i] = Desaturate(palette[i], keep);
}

void Saturation_ApplyToLoadedRange(u16 paletteOffset, u8 first, u8 count, u8 friendship)
{
    u16 keep = Saturation_OfFriendship(friendship);
    s32 i;

    if (keep >= 256)
        return;
    for (i = first; i < first + count && i < 16; i++)
    {
        u16 colour = Desaturate(gPlttBufferUnfaded[paletteOffset + i], keep);

        gPlttBufferUnfaded[paletteOffset + i] = colour;
        gPlttBufferFaded[paletteOffset + i] = colour;
    }
}

void Saturation_ApplyToLoaded(u16 paletteOffset, u8 friendship)
{
    Saturation_ApplyToLoadedRange(paletteOffset, 1, 15, friendship);
}

u8 Saturation_OfBattler(u8 battlerId)
{
    struct Pokemon *party = GetBattlerSide(battlerId) == B_SIDE_PLAYER ? gPlayerParty : gEnemyParty;

    return GetMonData(&party[gBattlerPartyIndexes[battlerId]], MON_DATA_FRIENDSHIP);
}
