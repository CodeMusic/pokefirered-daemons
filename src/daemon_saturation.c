#include "global.h"
#include "battle.h"
#include "battle_anim.h"
#include "palette.h"
#include "pokemon.h"
#include "daemon_saturation.h"
#include "daemon_song.h"
#include "data.h"
#include "constants/opponents.h"
#include "constants/trainers.h"

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

// ------------------------------------------------------------------------------------------------- T-597
// COLOUR BY SONG replaces friendship as the key (the user, 2026-11-19: approach A, "grey until sung"): each slot keeps
// as much of its colour as its notes are learned (DaemonSong_ColourKeeps, src/daemon_song.c). Friendship still scales
// over the song for the two parties whose grey tells a story -- the Corpus's (friendship 0) and AL's fading one
// (T-559's RivalFriendship) -- so those keep exactly the curve they had: at a whole song, the very same colours.

static void KeepsToBuffer(u16 *palette, u8 first, u8 count, const u16 *keeps)
{
    s32 i;

    for (i = first; i < first + count && i < 16; i++)
        if (keeps[i] < 256)
            palette[i] = Desaturate(palette[i], keeps[i]);
}

static void KeepsToLoaded(u16 paletteOffset, u8 first, u8 count, const u16 *keeps)
{
    s32 i;

    for (i = first; i < first + count && i < 16; i++)
    {
        if (keeps[i] < 256)
        {
            u16 colour = Desaturate(gPlttBufferUnfaded[paletteOffset + i], keeps[i]);

            gPlttBufferUnfaded[paletteOffset + i] = colour;
            gPlttBufferFaded[paletteOffset + i] = colour;
        }
    }
}

bool8 Saturation_IsStoryParty(struct Pokemon *mon)
{
    u8 class;

    if (mon < &gEnemyParty[0] || mon >= &gEnemyParty[PARTY_SIZE])
        return FALSE;
    if (!(gBattleTypeFlags & BATTLE_TYPE_TRAINER)
     || (gBattleTypeFlags & (BATTLE_TYPE_LINK | BATTLE_TYPE_BATTLE_TOWER | BATTLE_TYPE_EREADER_TRAINER | BATTLE_TYPE_TRAINER_TOWER)))
        return FALSE;
    if (gTrainerBattleOpponent_A == TRAINER_SECRET_BASE || gTrainerBattleOpponent_A >= NUM_TRAINERS)
        return FALSE;
    // the same classes battle_main.c's CreateNPCTrainerParty sets the friendship of
    class = gTrainers[gTrainerBattleOpponent_A].trainerClass;
    return class == TRAINER_CLASS_TEAM_ROCKET || class == TRAINER_CLASS_BOSS
        || class == TRAINER_CLASS_RIVAL_EARLY || class == TRAINER_CLASS_RIVAL_LATE || class == TRAINER_CLASS_CHAMPION;
}

void Saturation_SongToBuffer(u16 *palette, u16 species, u8 share)
{
    u16 keeps[16];

    DaemonSong_ColourKeeps(species, share, keeps);
    KeepsToBuffer(palette, 1, 15, keeps);
}

// A daemon's keeps for the picture of SHOWN: its own form, or a form after it (the evolution scene), each slot by its
// notes at its share of that form's part; another species' picture (PERSPECTIVE, a transform) all at its own body's.
static void MonKeeps(struct Pokemon *mon, u16 shown, u16 *keeps)
{
    u16 species = GetMonData(mon, MON_DATA_SPECIES);
    u32 exp = GetMonData(mon, MON_DATA_EXP);
    s32 i;

    if (shown == species || DaemonSong_Continues(species, shown))
    {
        DaemonSong_ColourKeeps(shown, DaemonSong_ShareOf(shown, exp), keeps);
    }
    else
    {
        u16 body;

        DaemonSong_ColourKeeps(species, DaemonSong_ShareOf(species, exp), keeps);
        body = keeps[SATURATION_BODY_SLOT];
        for (i = 0; i < 16; i++)
            keeps[i] = body;
    }
    if (Saturation_IsStoryParty(mon))
    {
        u16 kept = Saturation_OfFriendship(GetMonData(mon, MON_DATA_FRIENDSHIP));

        for (i = 0; i < 16; i++)
            keeps[i] = keeps[i] * kept / 256;
    }
}

void Saturation_MonToBuffer(u16 *palette, struct Pokemon *mon, u16 shown)
{
    u16 keeps[16];

    MonKeeps(mon, shown, keeps);
    KeepsToBuffer(palette, 1, 15, keeps);
}

void Saturation_MonToLoaded(u16 paletteOffset, struct Pokemon *mon, u16 shown)
{
    Saturation_MonToLoadedRange(paletteOffset, 1, 15, mon, shown);
}

void Saturation_MonToLoadedRange(u16 paletteOffset, u8 first, u8 count, struct Pokemon *mon, u16 shown)
{
    u16 keeps[16];

    MonKeeps(mon, shown, keeps);
    KeepsToLoaded(paletteOffset, first, count, keeps);
}

u16 Saturation_MonKeep(struct Pokemon *mon)
{
    u16 keeps[16];

    MonKeeps(mon, GetMonData(mon, MON_DATA_SPECIES), keeps);
    return keeps[SATURATION_BODY_SLOT];
}

void Saturation_KeepToBuffer(u16 *palette, u16 keep)
{
    s32 i;

    if (keep >= 256)
        return;
    for (i = 1; i < 16; i++)
        palette[i] = Desaturate(palette[i], keep);
}

struct Pokemon *Saturation_MonOfBattler(u8 battlerId)
{
    struct Pokemon *party = GetBattlerSide(battlerId) == B_SIDE_PLAYER ? gPlayerParty : gEnemyParty;

    return &party[gBattlerPartyIndexes[battlerId]];
}
