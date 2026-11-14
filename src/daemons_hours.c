#include "global.h"
#include "gpu_regs.h"
#include "m4a.h"
#include "main.h"
#include "palette.h"
#include "random.h"
#include "sound.h"
#include "sprite.h"
#include "trig.h"
#include "fieldmap.h"
#include "overworld.h"
#include "daemons_hours.h"
#include "daemons_aurora.h"     // T-528 (d): the dawn chorus
#include "daemons_time.h"
#include "daemons_pulse.h"   // the gold breath
#include "daemons_blackout.h"   // T-546: no bloom in THE BLACKOUT's dark
#include "constants/songs.h"

// T-519 item 4: THE TWO HOURS (the user, 2026-11-02 and 2026-11-04, by multiple choice).
//
// THE CATCH-UP. The player may be anywhere in an hour when play begins -- a new game, a save continued after power on
// or a soft reset -- so neither hour's effect jumps to the clock's level: nothing for the first minute of play, then a
// smoothstep over two minutes up to wherever the clock has it, and from then on the clock alone. Passing into an hour
// while playing is not a beginning: three minutes in, the catch-up is whole and the hour's own shape applies. What it
// eases: the witching hour's glitch (daemons_glitch.c), its pulse over the map and the battle's ground
// (daemons_pulse.c) and the party's shimmer (daemons_witching.c); and all of the creator's hour, below. The moment play
// began is one EWRAM word and a byte, never saved, and lost on a reset as it should be.
//
// THE CREATOR'S HOUR, every day on the cartridge's own clock (DaemonsCreatorSecond, daemons_time.c): a softer touch
// than the witching hour's -- light, not noise. Nothing flickers; it rises over seven minutes, holds, and resolves over
// the last seven, and only in the field (the overworld: never in battle, which it cannot change, and never in a save).
//   - THE LIGHT: the hardware's brighten (BLDCNT effect 2) on the map's layers and the backdrop, BLDY 0 to 4 -- never
//     BG0, which carries every text box and menu, so the words stay plain -- and a pale gold laid over the map's own
//     palettes (rows 0-12; the pulse's path, so fades and weather stay underneath), breathing once every sixteen
//     seconds on the clock, between 0.85 and 1.0 of its level. The brighten is taken only while BLDCNT is the
//     field's own (not a fog's, a fade's, the FLASH's): it waits, and gives it back as it found it.
//   - THE MOTES: up to six points of light rising slowly through the field like dust in a sunbeam, a few OAM sprites
//     under BG0's priority, so a text box or a menu always covers them. They take a palette only when two are free (the field reserves twelve).
//   - THE FOLLOWER wears a faint steady gold instead of the witching shimmer (daemons_follower.c).
//   - THE TONE, in the middle minutes only: a choir's sustained "aah" (voicegroup191's choir, GM 52) held on one soft
//     note, on the third sound-effect player (the weather's, one track), faded in and out over thirty seconds. It
//     plays only while the map's own music is what is playing -- never over a battle's or a scene's: it waits -- and
//     never over the weather's sound, which it gives way to.

#define FRAMES_PER_MINUTE   3584    // 59.73 frames a second
#define CATCHUP_QUIET       FRAMES_PER_MINUTE
#define CATCHUP_EASE        (2 * FRAMES_PER_MINUTE)

// The hour's shape, in seconds from its first: a seven-minute rise, a hold, a seven-minute resolve.
#define CREATOR_SECONDS     (48 * 60)
#define CREATOR_EDGE        (7 * 60)
// The tone's minutes, the middle seven (4:44:30 to 4:51:30): fading in from the first second, out to the last.
#define TONE_FIRST          (20 * 60 + 30)
#define TONE_LAST           (27 * 60 + 30)
#define TONE_FADE           (30 * 60)        // thirty seconds, in frames (near enough)
#define TONE_FADE_FAST      32               // a step a frame when it must give way: about a second

// The field's own BLDCNT (overworld.c's InitOverworldGraphicsRegisters): no first target, its layers and the sprites
// as the second, so a semi-transparent sprite (a reflection) blends onto the map. The bloom keeps those second targets
// -- a semi-transparent sprite still blends, whatever the effect -- and brightens BG1-3 and the backdrop.
#define FIELD_BLDCNT (BLDCNT_TGT2_BG1 | BLDCNT_TGT2_BG2 | BLDCNT_TGT2_BG3 | BLDCNT_TGT2_OBJ | BLDCNT_EFFECT_BLEND)
#define BLOOM_BLDCNT (BLDCNT_TGT2_BG1 | BLDCNT_TGT2_BG2 | BLDCNT_TGT2_BG3 | BLDCNT_TGT2_OBJ \
                    | BLDCNT_TGT1_BG1 | BLDCNT_TGT1_BG2 | BLDCNT_TGT1_BG3 | BLDCNT_TGT1_BD | BLDCNT_EFFECT_LIGHTEN)
// The field's WIN0 covers the whole screen (the FLASH's circle is WIN0 too) with its colour effects off: the bloom
// turns them on inside the windows only -- so a dark cave outside its FLASH circle stays dark.
#define FIELD_WININ  (WININ_WIN0_BG_ALL | WININ_WIN0_OBJ | WININ_WIN1_BG_ALL | WININ_WIN1_OBJ)
#define BLOOM_WININ  (FIELD_WININ | WININ_WIN0_CLR | WININ_WIN1_CLR)
#define BLOOM_MAX_Y         4                // a fifth more would wash the gold out to grey (filmed, 4 Nov)
#define GOLD                RGB(31, 22, 8)    // a warm gold, saturated enough to read over the dawn's pale light
#define TINT_FULL           77               // 256ths of gold at full (30%), at the top of the breath
// The light breathes between 0.85 and 1.0 of its level: 0.85 + 0.15 x goldBreath, in 256ths (the companion's).
#define BREATH_LOW          218
#define BREATH_SPAN         38
#define GLOW_MAX            3                // the follower's sixteenths

#define MOTES_MAX           6
#define MOTE_TAG            0x5190
#define MOTE_EVERY          40               // a new mote at most every 40 frames
#define MOTE_PALETTES_FREE  2                // take a palette only when this many are free (the field reserves 12)

static EWRAM_DATA u32 sPlayBeganAt = 0;
static EWRAM_DATA bool8 sPlayBegan = FALSE;
static EWRAM_DATA bool8 sBloomOwned = FALSE;
static EWRAM_DATA bool8 sSe3Dirty = FALSE;
static EWRAM_DATA u16 sToneLevel = 0;
static EWRAM_DATA u16 sToneVolume = 0;

//  ---------------------------------------------------------------- the catch-up

void Hours_PlayBegan(void)
{
    sPlayBeganAt = gMain.vblankCounter2;
    sPlayBegan = TRUE;
}

void Hours_DebugRestart(void)
{
    Hours_PlayBegan();
}

// 0-256 from NUM / DEN, eased at both ends: 3x^2 - 2x^3.
static u16 Smooth(u32 num, u32 den)
{
    u32 x;

    if (num >= den)
        return 256;
    x = num * 256 / den;
    return x * x * (768 - 2 * x) / 65536;
}

u16 Hours_CatchUp(void)
{
    u32 frames;

    if (!sPlayBegan)
        return 0;
    frames = gMain.vblankCounter2 - sPlayBeganAt;
    if (frames < CATCHUP_QUIET)
        return 0;
    return Smooth(frames - CATCHUP_QUIET, CATCHUP_EASE);
}

u16 Hours_WitchingEase(void)
{
    if (!DaemonsIsWitchingHour())
        return 256;
    return Hours_CatchUp();
}

//  ---------------------------------------------------------------- the creator's hour

u16 Hours_CreatorStrength(void)
{
    s16 second = DaemonsCreatorSecond();
    u16 shape;

    if (second < 0)
        return 0;
    if (second < CREATOR_EDGE)
        shape = Smooth(second, CREATOR_EDGE);
    else if (second >= CREATOR_SECONDS - CREATOR_EDGE)
        shape = Smooth(CREATOR_SECONDS - second, CREATOR_EDGE);
    else
        shape = 256;
    return shape * Hours_CatchUp() / 256;
}

// The light's share of its level now: 0.85 to 1.0 on the clock's sixteen-second gold breath (src/daemons_pulse.c,
// the same clock as the witching pulse's eight), in 256ths.
static u16 Breath(void)
{
    return BREATH_LOW + BREATH_SPAN * Pulse_GoldBreath() / 255;
}

// The field, as the player sees it: the overworld itself, not HALFTONE (grey outranks every light but the bloom's).
static u16 FieldStrength(bool8 *grey)
{
    *grey = DaemonsIsHalftone();
    if (!DaemonsFieldOnScreen())
        return 0;
    return Hours_CreatorStrength();
}

bool8 Hours_CreatorTint(u16 *colour, u8 *weight)
{
    bool8 grey;
    u16 s = FieldStrength(&grey);

    if (s == 0 || grey)
        return FALSE;
    *colour = GOLD;
    *weight = s * Breath() / 256 * TINT_FULL / 256;
    if (DaemonsHalftoneVividHere())
        *weight = *weight * 2 / 5;      // T-519: HALFTONE mended, its own colours show through the gold (the user)
    return *weight != 0;
}

// T-519: the field's light eased from the watch's toward the dawn's, in eighths of the hour's level (0-8) -- put on
// the palettes as they load (DaemonsTintForWatch), so the field relights when the step moves (overworld.c).
static EWRAM_DATA u8 sDawnStepLit = 0;

u8 Hours_DawnStep(void)
{
    bool8 grey;
    u16 s = FieldStrength(&grey);

    if (grey || !sPlayBegan)
        return 0;
    if (!DaemonsFieldOnScreen())
        s = Hours_CreatorStrength();     // a map loading: the step it will be seen in
    if (DaemonsHalftoneVividHere())
        return (s * 6 + 128) / 256;     // T-519: HALFTONE mended -- most of the dawn, not all, so its colours are not paled out
    return (s * 8 + 128) / 256;
}

void Hours_NoteDawnStepLit(u8 step)
{
    sDawnStepLit = step;
}

bool8 Hours_DawnStepMoved(void)
{
    return Hours_DawnStep() != sDawnStepLit;
}

u8 Hours_FollowerGlow(void)
{
    bool8 grey;
    u16 s = FieldStrength(&grey);

    if (grey)
        return 0;
    return (s * GLOW_MAX + 128) / 256;
}

void Hours_GlowFadedInPlace(u16 palOffset, u8 weight)
{
    s32 i, tr = GET_R(GOLD), tg = GET_G(GOLD), tb = GET_B(GOLD);

    for (i = 1; i < 16; i++)   // 0 is the transparent colour
    {
        u16 c = gPlttBufferFaded[palOffset + i];
        s32 r = GET_R(c), g = GET_G(c), b = GET_B(c);

        r += ((tr - r) * weight) >> 4;
        g += ((tg - g) * weight) >> 4;
        b += ((tb - b) * weight) >> 4;
        gPlttBufferFaded[palOffset + i] = RGB(r, g, b);
    }
}

//  ---------------------------------------------------------------- the bloom

static void Bloom(u16 s)
{
    u16 y = (s * Breath() / 256 * (DaemonsHalftoneVividHere() ? BLOOM_MAX_Y / 2 : BLOOM_MAX_Y) + 128) / 256;   // T-519
    u16 now = GetGpuReg(REG_OFFSET_BLDCNT), win = GetGpuReg(REG_OFFSET_WININ);
    bool8 ours = (now == BLOOM_BLDCNT);

    if (gPaletteFade.active || gPaletteFade.y != 0)
        y = 0;                                  // a fade to black stays black
    if (Blackout_IsDark())
        y = 0;                                  // T-546: it would lift VERBOSE's circle out of THE BLACKOUT's black
    if (!DaemonsFieldOnScreen())
    {
        // Left for a battle, the BAG, a screen of its own: the brighten let go at once (BLDY 0 changes nothing), and
        // the field sets its own registers again when it comes back.
        if (sBloomOwned && ours)
            SetGpuReg(REG_OFFSET_BLDY, 0);
        sBloomOwned = FALSE;
        return;
    }
    if ((now != FIELD_BLDCNT && !ours) || (win != FIELD_WININ && win != BLOOM_WININ))
    {
        sBloomOwned = FALSE;                    // a fog, a fade, the HELP: theirs, untouched -- it waits
        return;
    }
    if (y == 0)
    {
        if (sBloomOwned || ours)
        {
            SetGpuReg(REG_OFFSET_BLDCNT, FIELD_BLDCNT);
            SetGpuReg(REG_OFFSET_WININ, FIELD_WININ);
            SetGpuReg(REG_OFFSET_BLDY, 0);
        }
        sBloomOwned = FALSE;
        return;
    }
    if (!ours)
        SetGpuReg(REG_OFFSET_BLDCNT, BLOOM_BLDCNT);
    if (win != BLOOM_WININ)
        SetGpuReg(REG_OFFSET_WININ, BLOOM_WININ);
    SetGpuReg(REG_OFFSET_BLDY, y);
    sBloomOwned = TRUE;
}

//  ---------------------------------------------------------------- the motes

static const u32 sMoteTiles[] = {
    0x00000000, 0x00000000, 0x00000000, 0x00022000, 0x00022000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00002000, 0x00023200, 0x00002000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00001000, 0x00022200, 0x00123210, 0x00022200, 0x00001000, 0x00000000, 0x00000000,
};

// A warm gold, never the night's: written back over whatever the field's light or the weather laid on the slot.
static const u16 sMotePalette[16] = { RGB(0, 0, 0), RGB(28, 16, 2), RGB(31, 23, 5), RGB(31, 30, 16) };

static const struct SpriteSheet sMoteSheet = { sMoteTiles, sizeof(sMoteTiles), MOTE_TAG };
static const struct SpritePalette sMoteSpritePalette = { sMotePalette, MOTE_TAG };

static const struct OamData sMoteOam = {
    .affineMode = ST_OAM_AFFINE_OFF,
    .objMode = ST_OAM_OBJ_NORMAL,
    .shape = SPRITE_SHAPE(8x8),
    .size = SPRITE_SIZE(8x8),
    .priority = 1,                              // under BG0: a text box or a menu covers it
};

static void SpriteCB_Mote(struct Sprite *sprite);

static const struct SpriteTemplate sMoteTemplate = {
    .tileTag = MOTE_TAG,
    .paletteTag = MOTE_TAG,
    .oam = &sMoteOam,
    .anims = gDummySpriteAnimTable,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCB_Mote,
};

#define mAge   data[0]
#define mLife  data[1]
#define mX     data[2]
#define mPhase data[3]
#define mY16   data[4]
#define mTile  data[5]

static void SpriteCB_Mote(struct Sprite *sprite)
{
    s16 age = ++sprite->mAge, left = sprite->mLife - age;
    u8 frame;

    sprite->mY16 -= 4;                          // a quarter of a pixel a frame: fifteen a second
    sprite->mPhase = (sprite->mPhase + 1) & 0xFF;
    sprite->x = sprite->mX + Sin(sprite->mPhase, 6);
    sprite->y = sprite->mY16 >> 4;
    if (left <= 0 || sprite->y < -8)
    {
        DestroySprite(sprite);
        return;
    }
    if (age < 45 || left < 45)
        frame = 0;
    else if (age < 90 || left < 90)
        frame = 1;
    else
        frame = 2;
    sprite->oam.tileNum = sprite->mTile + frame;
}

static u8 CountMotes(void)
{
    u8 i, n = 0;

    for (i = 0; i < MAX_SPRITES; i++)
        if (gSprites[i].inUse && gSprites[i].callback == SpriteCB_Mote)
            n++;
    return n;
}

static u8 FreeSpritePalettes(void)
{
    u8 i, n = 0;

    for (i = gReservedSpritePaletteCount; i < 16; i++)
        if (GetSpritePaletteTagByPaletteNum(i) == TAG_NONE)
            n++;
    return n;
}

static void Motes(u16 s, bool8 grey)
{
    u8 want, have, spriteId;
    struct Sprite *sprite;

    if (!DaemonsFieldOnScreen() || (gMain.vblankCounter2 % MOTE_EVERY) != 0)
        return;
    want = grey ? 0 : (s * MOTES_MAX + 128) / 256;
    have = CountMotes();
    // The field's relight and the weather retint the sprites' slots; the motes' gold is put back over it.
    if (!gPaletteFade.active && (spriteId = IndexOfSpritePaletteTag(MOTE_TAG)) != 0xFF)
    {
        CpuCopy16(sMotePalette, &gPlttBufferUnfaded[OBJ_PLTT_ID(spriteId)], PLTT_SIZE_4BPP);
        CpuCopy16(sMotePalette, &gPlttBufferFaded[OBJ_PLTT_ID(spriteId)], PLTT_SIZE_4BPP);
    }
    // A reset of the field's sprites (a battle, the BAG) takes the motes and their tiles; they are asked for again.
    // The last one gone with the hour, its tiles and palette are given back.
    if (want == 0 || have >= want)
    {
        if (want == 0 && have == 0)
        {
            if (GetSpriteTileStartByTag(MOTE_TAG) != 0xFFFF)
                FreeSpriteTilesByTag(MOTE_TAG);
            if (IndexOfSpritePaletteTag(MOTE_TAG) != 0xFF)
                FreeSpritePaletteByTag(MOTE_TAG);
        }
        return;
    }
    if (gPaletteFade.active)
        return;
    if (IndexOfSpritePaletteTag(MOTE_TAG) == 0xFF)
    {
        if (FreeSpritePalettes() < MOTE_PALETTES_FREE)
            return;
        if (LoadSpritePalette(&sMoteSpritePalette) == 0xFF)
            return;
    }
    if (GetSpriteTileStartByTag(MOTE_TAG) == 0xFFFF)
    {
        LoadSpriteSheet(&sMoteSheet);
        if (GetSpriteTileStartByTag(MOTE_TAG) == 0xFFFF)
            return;
    }
    spriteId = CreateSprite(&sMoteTemplate, 0, 0, 0xFF);
    if (spriteId == MAX_SPRITES)
        return;
    sprite = &gSprites[spriteId];
    sprite->mLife = 360 + Random() % 180;       // six to nine seconds
    sprite->mX = 8 + Random() % (DISPLAY_WIDTH - 16);
    sprite->mPhase = Random() & 0xFF;
    sprite->mY16 = (140 + Random() % 20) << 4;  // from low on the screen, like light under a door
    sprite->mTile = GetSpriteTileStartByTag(MOTE_TAG);
    sprite->x = sprite->mX;
    sprite->y = sprite->mY16 >> 4;
    sprite->oam.tileNum = sprite->mTile;
}

//  ---------------------------------------------------------------- the tone

// T-528 (d): in a lit grove on the morning after a night of light, the tone is SE_DAWN_CHORUS -- the same pad, with
// the rising whistles under it (DAEMONS tools/genchorus.py) -- faded exactly as the pad is.
static u16 ToneSong(void)
{
    return Aurora_DawnChorus() ? SE_DAWN_CHORUS : SE_AAH_PAD;
}

static bool8 ToneIsPlaying(void)
{
    return (gMPlayInfo_SE3.status & MUSICPLAYER_STATUS_TRACK) && !(gMPlayInfo_SE3.status & MUSICPLAYER_STATUS_PAUSE)
        && (gMPlayInfo_SE3.songHeader == gSongTable[SE_AAH_PAD].header
         || gMPlayInfo_SE3.songHeader == gSongTable[SE_DAWN_CHORUS].header);
}

// The map's own music is what is playing: not a battle's, not a scene's.
static bool8 MapMusicPlaying(void)
{
    u16 song = GetCurrentMapMusic();

    if (gMain.inBattle || song == 0 || song == MUS_NONE || song > SE_AAH_PAD)
        return FALSE;
    return (gMPlayInfo_BGM.status & MUSICPLAYER_STATUS_TRACK) && gMPlayInfo_BGM.songHeader == gSongTable[song].header;
}

static void Tone(void)
{
    s16 second = DaemonsCreatorSecond();
    bool8 allowed = MapMusicPlaying();
    bool8 want = allowed && second >= TONE_FIRST && second < TONE_LAST - 30 && Hours_CatchUp() != 0;
    u16 volume;

    if (want)
    {
        if (sToneLevel < TONE_FADE)
            sToneLevel++;
    }
    else if (sToneLevel != 0)
    {
        u16 step = allowed ? 1 : TONE_FADE_FAST;

        sToneLevel = sToneLevel > step ? sToneLevel - step : 0;
    }

    if (ToneIsPlaying())
    {
        if (sToneLevel == 0)
        {
            m4aMPlayStop(&gMPlayInfo_SE3);
            m4aMPlayVolumeControl(&gMPlayInfo_SE3, TRACKS_ALL, 256);
            sSe3Dirty = FALSE;
            return;
        }
        if (gMPlayInfo_SE3.songHeader != gSongTable[ToneSong()].header)
        {
            m4aSongNumStart(ToneSong());        // the chorus joins the pad, or leaves it, at the level the tone is at
            sToneVolume = 0xFFFF;
        }
    }
    else
    {
        if (sSe3Dirty)
        {
            // the weather took the player: its sound at its own volume
            m4aMPlayVolumeControl(&gMPlayInfo_SE3, TRACKS_ALL, 256);
            sSe3Dirty = FALSE;
        }
        if (sToneLevel == 0 || ((gMPlayInfo_SE3.status & MUSICPLAYER_STATUS_TRACK)
                             && !(gMPlayInfo_SE3.status & MUSICPLAYER_STATUS_PAUSE)))
            return;                             // nothing wanted, or the weather is sounding: it waits
        m4aSongNumStart(ToneSong());
        sToneVolume = 0xFFFF;
    }
    volume = (u32)sToneLevel * 256 / TONE_FADE * Hours_CatchUp() / 256;
    if (volume != sToneVolume)
    {
        m4aMPlayVolumeControl(&gMPlayInfo_SE3, TRACKS_ALL, volume);
        sToneVolume = volume;
        sSe3Dirty = TRUE;
    }
}

//  ---------------------------------------------------------------- once a frame

void Hours_Tick(void)
{
    bool8 grey;
    u16 s = FieldStrength(&grey);

    Bloom(s);
    Motes(s, grey);
    Tone();
}
