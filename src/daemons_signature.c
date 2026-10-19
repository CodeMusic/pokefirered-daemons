#include "global.h"
#include "battle.h"
#include "battle_anim.h"
#include "daemons_holidays.h"
#include "daemons_signature.h"
#include "m4a.h"
#include "palette.h"
#include "play.h"
#include "pokemon.h"
#include "sound.h"
#include "sprite.h"
#include "task.h"
#include "trig.h"
#include "constants/battle_anim.h"
#include "constants/battle_move_effects.h"
#include "constants/moves.h"
#include "constants/songs.h"

// T-392 P1 (the user, 2026-10-18): EVERY ROUTINE SIGNED. The splash's 0s and 1s and its notes -- the presents scene's
// code becoming music -- as each routine's own codeword (tools/gensignature.py: a bijection of the id, so no two are
// the same). Started by battle_anim.c's LaunchBattleAnimation for a move, on top of the routine's own picture, which
// plays underneath unchanged. Decided by phone: CLEAR (the whole word and its figure at SE volume, up to 12 frames
// past the picture), the type's key and the month's note on a holiday, BOTH bits and notes, the side's kind first.
//
// THE BOOKKEEPING (the plan's risk 1). Nothing here is counted in gAnimVisualTaskCount, so the script's own
// waitforvisualfinish never waits on the signature and the picture keeps its timing; `end` asks
// DaemonsSignature_Holds() instead, before it frees the sprite tags. The tags (the glyph sheet, the ink, the splash)
// are loaded and registered by battle_anim.c exactly as loadspritegfx does, so `end` frees them; these sprites are
// destroyed with DestroySprite, never DestroyAnimSprite, and Holds() destroys any still flying before it says done.
// The notes go on SE3 (PLAY's own path, Play_SoundNote), which `end` neither waits for nor stops: the harp rings out.
// SE3 is also the low-HP alarm's player, so a signature never sounds over a song of anyone else's (risk 2).
//
// T-392 P2 (the user, 2026-10-18): ALL 363 routines play their row in the release ROMs too (every family APPROVED in
// the generated table), and every glyph wears a true-black outline (graphics/battle_anims/sprites/daemons_glyphs.png,
// index 1 black in both the ink and the splash palette), so its type's colour reads on any picture -- MISREAD's magenta
// had been lost on its own pink.
//
// The user, 2026-10-18 (P2's release): NO CAP. A signature runs in full -- `end` waits for it, however far past its
// picture (the longest, SHIFT's, is 98 frames from the launch; gensignature.py reports it) -- and the seven union
// routines' own scripts are released into every ROM, so the hook stands aside for them everywhere (SIG_OWN_SCRIPT).
// Nothing else in battle moves while it plays: the HP bar, the next line and the next hit all wait on the animation,
// and the notes keep off SE3 while the low-HP alarm holds it.
//
// T-392 P3 (the user, 2026-10-18): THE HOLIDAY LOOKS. On a holiday a routine its row names wears the day's look
// (sHolidayRoutineLooks, tools/genholidays.py from docs/holidays.json): colours ONLY from the twelve month colours
// (ANIM_TAG_DAEMONS_LANES: the same 0, 1 and note in thirteen lanes -- each month's bright colour, and black), a shape
// (a burst, a heart of bits -- never vanilla's heart sprite -- held stars, confetti, streamers, falling petals, an
// orbit), every glyph a note, or the type's own ink paled (white, silver, frost: no month has them). The figure is in
// the month's note, as on every holiday. Never the daemon: only the signature's own palette is touched. A QUIET day
// dresses nothing: the ink as ever, the notes soft.

#include "data/daemons_signatures.h"

#define SIG_BIT_STEP  2    // frames between the word's glyphs
#define SIG_LAG       12   // frames the second kind waits for the first
#define SIG_FUSE      255  // frames past the picture after which `end` stops waiting: a guard against a hang, never
                           // reached (a whole signature is at most ~100 frames from its launch) -- not a cap
#define SIG_VOL_SOFT  96   // a QUIET holiday's notes (the day banner's soft note)
#define SIG_FLUTE_HOLD 10  // frames the last FLUTE note is held before it breathes out over five
#define SIG_NOTE_LANE 9    // lanes 0-8 are the word's glyphs, 9-11 the figure's notes

enum { SIG_MODE_FULL, SIG_MODE_ONE_NOTE, SIG_MODE_FIGURE };

static const u8 sMajor8[8] = {0, 2, 4, 5, 7, 9, 11, 12};

static void SpriteCB_SignatureGlyph(struct Sprite *sprite);
static void Task_DaemonsSignature(u8 taskId);

static const struct OamData sOam_SignatureGlyph =
{
    .affineMode = ST_OAM_AFFINE_OFF,
    .objMode = ST_OAM_OBJ_NORMAL,
    .shape = SPRITE_SHAPE(8x8),
    .size = SPRITE_SIZE(8x8),
    .priority = 1,
};

static const union AnimCmd sAnim_SignatureGlyph0[] = {ANIMCMD_FRAME(0, 1), ANIMCMD_END};
static const union AnimCmd sAnim_SignatureGlyph1[] = {ANIMCMD_FRAME(1, 1), ANIMCMD_END};
static const union AnimCmd sAnim_SignatureGlyph2[] = {ANIMCMD_FRAME(2, 1), ANIMCMD_END};
static const union AnimCmd sAnim_SignatureGlyph3[] = {ANIMCMD_FRAME(3, 1), ANIMCMD_END};
static const union AnimCmd *const sAnims_SignatureGlyph[] =
{
    sAnim_SignatureGlyph0, sAnim_SignatureGlyph1, sAnim_SignatureGlyph2, sAnim_SignatureGlyph3,
};

static const struct SpriteTemplate sSignatureInkTemplate =
{
    .tileTag = ANIM_TAG_DAEMONS_GLYPHS,
    .paletteTag = ANIM_TAG_DAEMONS_INK,
    .oam = &sOam_SignatureGlyph,
    .anims = sAnims_SignatureGlyph,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCB_SignatureGlyph,
};

static const struct SpriteTemplate sSignatureSplashTemplate =
{
    .tileTag = ANIM_TAG_DAEMONS_GLYPHS,
    .paletteTag = ANIM_TAG_DAEMONS_GLYPHS,
    .oam = &sOam_SignatureGlyph,
    .anims = sAnims_SignatureGlyph,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCB_SignatureGlyph,
};

// P3: thirteen lanes of the four glyphs (frame = lane * 4 + glyph), for a holiday look.
static const union AnimCmd sAnim_SignatureLanes[52][2] =
{
    {ANIMCMD_FRAME(0, 1), ANIMCMD_END}, {ANIMCMD_FRAME(1, 1), ANIMCMD_END}, {ANIMCMD_FRAME(2, 1), ANIMCMD_END}, {ANIMCMD_FRAME(3, 1), ANIMCMD_END},
    {ANIMCMD_FRAME(4, 1), ANIMCMD_END}, {ANIMCMD_FRAME(5, 1), ANIMCMD_END}, {ANIMCMD_FRAME(6, 1), ANIMCMD_END}, {ANIMCMD_FRAME(7, 1), ANIMCMD_END},
    {ANIMCMD_FRAME(8, 1), ANIMCMD_END}, {ANIMCMD_FRAME(9, 1), ANIMCMD_END}, {ANIMCMD_FRAME(10, 1), ANIMCMD_END}, {ANIMCMD_FRAME(11, 1), ANIMCMD_END},
    {ANIMCMD_FRAME(12, 1), ANIMCMD_END}, {ANIMCMD_FRAME(13, 1), ANIMCMD_END}, {ANIMCMD_FRAME(14, 1), ANIMCMD_END}, {ANIMCMD_FRAME(15, 1), ANIMCMD_END},
    {ANIMCMD_FRAME(16, 1), ANIMCMD_END}, {ANIMCMD_FRAME(17, 1), ANIMCMD_END}, {ANIMCMD_FRAME(18, 1), ANIMCMD_END}, {ANIMCMD_FRAME(19, 1), ANIMCMD_END},
    {ANIMCMD_FRAME(20, 1), ANIMCMD_END}, {ANIMCMD_FRAME(21, 1), ANIMCMD_END}, {ANIMCMD_FRAME(22, 1), ANIMCMD_END}, {ANIMCMD_FRAME(23, 1), ANIMCMD_END},
    {ANIMCMD_FRAME(24, 1), ANIMCMD_END}, {ANIMCMD_FRAME(25, 1), ANIMCMD_END}, {ANIMCMD_FRAME(26, 1), ANIMCMD_END}, {ANIMCMD_FRAME(27, 1), ANIMCMD_END},
    {ANIMCMD_FRAME(28, 1), ANIMCMD_END}, {ANIMCMD_FRAME(29, 1), ANIMCMD_END}, {ANIMCMD_FRAME(30, 1), ANIMCMD_END}, {ANIMCMD_FRAME(31, 1), ANIMCMD_END},
    {ANIMCMD_FRAME(32, 1), ANIMCMD_END}, {ANIMCMD_FRAME(33, 1), ANIMCMD_END}, {ANIMCMD_FRAME(34, 1), ANIMCMD_END}, {ANIMCMD_FRAME(35, 1), ANIMCMD_END},
    {ANIMCMD_FRAME(36, 1), ANIMCMD_END}, {ANIMCMD_FRAME(37, 1), ANIMCMD_END}, {ANIMCMD_FRAME(38, 1), ANIMCMD_END}, {ANIMCMD_FRAME(39, 1), ANIMCMD_END},
    {ANIMCMD_FRAME(40, 1), ANIMCMD_END}, {ANIMCMD_FRAME(41, 1), ANIMCMD_END}, {ANIMCMD_FRAME(42, 1), ANIMCMD_END}, {ANIMCMD_FRAME(43, 1), ANIMCMD_END},
    {ANIMCMD_FRAME(44, 1), ANIMCMD_END}, {ANIMCMD_FRAME(45, 1), ANIMCMD_END}, {ANIMCMD_FRAME(46, 1), ANIMCMD_END}, {ANIMCMD_FRAME(47, 1), ANIMCMD_END},
    {ANIMCMD_FRAME(48, 1), ANIMCMD_END}, {ANIMCMD_FRAME(49, 1), ANIMCMD_END}, {ANIMCMD_FRAME(50, 1), ANIMCMD_END}, {ANIMCMD_FRAME(51, 1), ANIMCMD_END},
};
static const union AnimCmd *const sAnims_SignatureLanes[52] =
{
    sAnim_SignatureLanes[0], sAnim_SignatureLanes[1], sAnim_SignatureLanes[2], sAnim_SignatureLanes[3], sAnim_SignatureLanes[4], sAnim_SignatureLanes[5], sAnim_SignatureLanes[6], sAnim_SignatureLanes[7],
    sAnim_SignatureLanes[8], sAnim_SignatureLanes[9], sAnim_SignatureLanes[10], sAnim_SignatureLanes[11], sAnim_SignatureLanes[12], sAnim_SignatureLanes[13], sAnim_SignatureLanes[14], sAnim_SignatureLanes[15],
    sAnim_SignatureLanes[16], sAnim_SignatureLanes[17], sAnim_SignatureLanes[18], sAnim_SignatureLanes[19], sAnim_SignatureLanes[20], sAnim_SignatureLanes[21], sAnim_SignatureLanes[22], sAnim_SignatureLanes[23],
    sAnim_SignatureLanes[24], sAnim_SignatureLanes[25], sAnim_SignatureLanes[26], sAnim_SignatureLanes[27], sAnim_SignatureLanes[28], sAnim_SignatureLanes[29], sAnim_SignatureLanes[30], sAnim_SignatureLanes[31],
    sAnim_SignatureLanes[32], sAnim_SignatureLanes[33], sAnim_SignatureLanes[34], sAnim_SignatureLanes[35], sAnim_SignatureLanes[36], sAnim_SignatureLanes[37], sAnim_SignatureLanes[38], sAnim_SignatureLanes[39],
    sAnim_SignatureLanes[40], sAnim_SignatureLanes[41], sAnim_SignatureLanes[42], sAnim_SignatureLanes[43], sAnim_SignatureLanes[44], sAnim_SignatureLanes[45], sAnim_SignatureLanes[46], sAnim_SignatureLanes[47],
    sAnim_SignatureLanes[48], sAnim_SignatureLanes[49], sAnim_SignatureLanes[50], sAnim_SignatureLanes[51],
};

static const struct SpriteTemplate sSignatureLanesTemplate =
{
    .tileTag = ANIM_TAG_DAEMONS_LANES,
    .paletteTag = ANIM_TAG_DAEMONS_LANES,
    .oam = &sOam_SignatureGlyph,
    .anims = sAnims_SignatureLanes,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCB_SignatureGlyph,
};

#define LANE_RING_INDEX 14   // the BLACK lane's ring (daemons_lanes.pal): the look's first month, set at the start

// A heart, of the word's bits and its notes (PAIRING DAY): twelve points round it, from the cleft, clockwise.
static const s8 sHeart[12][2] =
{
    {0, -12}, {3, -20}, {18, -24}, {27, -11}, {18, 5}, {3, 19}, {0, 25}, {-3, 19}, {-18, 5}, {-27, -11}, {-18, -24}, {-3, -20},
};

// ---------------------------------------------------------------------------------------------------------- which

bool8 DaemonsSignature_Wants(u16 move)
{
    const struct DaemonsSignature *sig;

    if (move == MOVE_NONE || move >= MOVES_COUNT)
        return FALSE;
    sig = &sDaemonsSignatures[move];
    if (sig->word == 0 && sig->rhythm == 0)
        return FALSE;
    if (sig->flags & SIG_OWN_SCRIPT)
        return FALSE;   // a union routine's own script carries its signature, in every ROM (the user, 2026-10-18)
#if DAEMONS_DEBUG
    return TRUE;
#else
    return (sig->flags & SIG_APPROVED) != 0;
#endif
}

// The look the day gives a routine (P3): none for a union routine, which wears the splash's colours on any day.
static const struct HolidayRoutineLook *LookFor(u16 move)
{
    if (sDaemonsSignatures[move].flags & SIG_UNION)
        return NULL;
    return DaemonsHolidayRoutineLook(move);
}

static bool8 LookHasLanes(const struct HolidayRoutineLook *look)
{
    return look != NULL && (look->lanes & ((2 << HRL_LANE_BLACK) - 1)) != 0;   // a month, or black
}

void DaemonsSignature_Tags(u16 move, u16 *tiles, u16 *palette)
{
    if (LookHasLanes(LookFor(move)))
    {
        *tiles = *palette = ANIM_TAG_DAEMONS_LANES;
        return;
    }
    *tiles = ANIM_TAG_DAEMONS_GLYPHS;
    *palette = (sDaemonsSignatures[move].flags & SIG_UNION) ? ANIM_TAG_DAEMONS_GLYPHS : ANIM_TAG_DAEMONS_INK;
}

// ---------------------------------------------------------------------------------------------------------- glyphs

// Sprite data. Placed once, then flown along its path for sFrames frames, then destroyed (not counted anywhere).
#define sTimer  data[0]
#define sFrames data[1]
#define sX0     data[2]
#define sY0     data[3]
#define sX1     data[4]
#define sY1     data[5]
#define sPhase  data[6]   // phase | wave << 8
#define sShape  data[7]   // path | mods << 4 | notes << 12 | the bit << 13 | twinkle << 14

#define SHAPE_PATH(s)  ((s) & 0xF)
#define SHAPE_MODS(s)  (((s) >> 4) & 0xFF)
#define SHAPE_NOTES    (1 << 12)
#define SHAPE_BIT      (1 << 13)
#define SHAPE_TWINKLE  (1 << 14)

static void SpriteCB_SignatureGlyph(struct Sprite *sprite)
{
    u16 shape = sprite->sShape;
    u8 path = SHAPE_PATH(shape), mods = SHAPE_MODS(shape);
    u8 phase = sprite->sPhase & 0xFF, wave = (sprite->sPhase >> 8) & 0xFF;
    s32 t = sprite->sTimer, n = sprite->sFrames;
    s32 x, y;

    if (t > n)
    {
        DestroySprite(sprite);
        return;
    }
    if (!(shape & SHAPE_NOTES) && (mods & SIG_MOD_FLIP) && (t & 3) == 3)
    {
        sprite->sShape ^= SHAPE_BIT;            // CORRUPT: each bit flips as it flies
        StartSpriteAnim(sprite, (sprite->sShape & SHAPE_BIT) ? 1 : 0);
    }
    if (shape & SHAPE_TWINKLE)
        sprite->invisible = ((t + phase) & 7) < 2;   // P3: a holiday's sparkle and its stars
    if ((mods & SIG_MOD_FADE) && t > n / 2)
        sprite->invisible = TRUE;                 // OPAQUE: gone halfway
    if ((mods & SIG_MOD_BACK))
    {
        t = (t < n / 2) ? t * 2 : (n - t) * 2;    // COPY: there, and the same way back
        if (t > n)
            t = n;
    }

    switch (path)
    {
    case DAEMONS_GLYPH_RING:
        if (mods & SIG_MOD_ORBIT)
        {
            u8 a = phase + t * 256 / n;           // CONTEXT: round the target, at one radius
            x = sprite->sX0 + Cos(a, 28);
            y = sprite->sY0 + Sin(a, 28) / 2;
        }
        else
        {
            s16 r = 4 + 36 * t / n;
            x = sprite->sX0 + Cos(phase, r);
            y = sprite->sY0 + Sin(phase, r) / 2;
        }
        break;
    case DAEMONS_GLYPH_HOLD:
        x = sprite->sX0;                          // FROZEN: it appears and does not move
        y = sprite->sY0;
        break;
    case DAEMONS_GLYPH_SCATTER:
    {
        s32 u = n - t;
        s16 r = 52 - 52 * u * u / (n * n);        // ENTROPY: thrown apart, slowing
        x = sprite->sX0 + Cos(phase, r);
        y = sprite->sY0 + Sin(phase, r) / 2;
        break;
    }
    case DAEMONS_GLYPH_STEP:
        x = sprite->sX0 + (sprite->sX1 - sprite->sX0) * t / n;
        y = sprite->sY0 + (sprite->sY1 - sprite->sY0) * (t * 4 / n) / 4;   // LOGIC: a stair, a step per derivation
        break;
    default:
        x = sprite->sX0 + (sprite->sX1 - sprite->sX0) * t / n;
        y = sprite->sY0 + (sprite->sY1 - sprite->sY0) * t / n;
        if (path == DAEMONS_GLYPH_SWEEP || path == DAEMONS_GLYPH_CONVERGE)
            x += Sin((u8)(phase + t * 12), wave);
        else
            y += Sin((u8)(phase + t * 12), wave);
        if (mods & SIG_MOD_ARC)
            y -= Sin((u8)(t * 128 / n), 24);      // VECTOR: through the air
        break;
    }
    sprite->x = x;
    sprite->y = y;
    sprite->sTimer++;
}

// P3: the month (or black) the k-th glyph of a look wears -- the (k mod n)-th lane of the look's set.
static u8 LaneOf(const struct HolidayRoutineLook *look, u8 k)
{
    u8 bit, n = 0, i;

    for (bit = 0; bit <= HRL_LANE_BLACK; bit++)
        if (look->lanes & (1 << bit))
            n++;
    k %= n;
    for (bit = 0, i = 0; bit <= HRL_LANE_BLACK; bit++)
    {
        if (look->lanes & (1 << bit))
        {
            if (i == k)
                return bit;
            i++;
        }
    }
    return 0;
}

static void LaunchGlyph(const struct DaemonsSignature *sig, const struct HolidayRoutineLook *look, bool8 splash, u8 lane,
                        bool8 notes, u8 frame, u8 frames)
{
    const struct SpriteTemplate *template = splash ? &sSignatureSplashTemplate : &sSignatureInkTemplate;
    u8 path = sig->path, mods = sig->mods, shape = HRL_SHAPE_KEEP;
    u16 extra = 0;
    u8 self;
    s16 ax = GetBattlerSpriteCoord(gBattleAnimAttacker, BATTLER_COORD_X_2);
    s16 ay = GetBattlerSpriteCoord(gBattleAnimAttacker, BATTLER_COORD_Y_PIC_OFFSET);
    s16 tx = GetBattlerSpriteCoord(gBattleAnimTarget, BATTLER_COORD_X_2);
    s16 ty = GetBattlerSpriteCoord(gBattleAnimTarget, BATTLER_COORD_Y_PIC_OFFSET);
    s16 cx, cy, x0, y0, x1, y1;
    u8 phase = ((sig->word >> (lane % 9)) & 7) * 24 + lane * 24;   // the lane order, from the word
    s16 sub = GetBattlerSpriteSubpriority(gBattleAnimAttacker) - 2;
    u8 spriteId;
    struct Sprite *sprite;

    // P3: the day's look moves the glyphs onto its shape and into its lanes; the word, the figure and its timing stay.
    if (look != NULL)
    {
        shape = look->shape;
        if (shape != HRL_SHAPE_KEEP)
            mods &= SIG_MOD_SLOW | SIG_MOD_FAST | SIG_MOD_SELF;
        switch (shape)
        {
        case HRL_SHAPE_RING:    path = DAEMONS_GLYPH_RING; break;
        case HRL_SHAPE_HEART:
        case HRL_SHAPE_STARS:   path = DAEMONS_GLYPH_HOLD; break;
        case HRL_SHAPE_SCATTER: path = DAEMONS_GLYPH_SCATTER; break;
        case HRL_SHAPE_SWEEP:   path = DAEMONS_GLYPH_SWEEP; break;
        case HRL_SHAPE_FALL:    path = DAEMONS_GLYPH_FALL; break;
        case HRL_SHAPE_ORBIT:   path = DAEMONS_GLYPH_RING; mods |= SIG_MOD_ORBIT; break;
        }
        if ((look->flags & HRL_NOTES) && !notes)
            frame = 2 + (frame & 1);              // NEST DAY's lullaby: every glyph a note
        if (look->flags & HRL_TWINKLE)
            extra |= SHAPE_TWINKLE;
        if (LookHasLanes(look))
        {
            template = &sSignatureLanesTemplate;
            mods &= ~SIG_MOD_FLIP;                // a flip would leave the lane
            frame += LaneOf(look, lane) * 4;
        }
    }
    self = (mods & SIG_MOD_SELF) != 0;
    cx = self ? ax : tx;
    cy = self ? ay : ty;
    x0 = ax; y0 = ay; x1 = tx; y1 = ty;

    switch (path)
    {
    case DAEMONS_GLYPH_RETURN:
        x0 = tx; y0 = ty; x1 = ax; y1 = ay;
        break;
    case DAEMONS_GLYPH_RISE:
        x0 = cx + (lane % 5) * 6 - 12; y0 = cy + 12;
        x1 = x0 + ((lane & 1) ? 8 : -8); y1 = cy - 36;
        break;
    case DAEMONS_GLYPH_FALL:
        x0 = cx + (lane % 5) * 8 - 16; y0 = cy - 56;
        x1 = x0; y1 = cy + 8;
        break;
    case DAEMONS_GLYPH_RING:
    case DAEMONS_GLYPH_SCATTER:
        x0 = cx; y0 = cy;
        phase = lane * (look != NULL ? 21 : 28) + (sig->word & 0x1F);   // a look's twelve go all the way round
        break;
    case DAEMONS_GLYPH_CONVERGE:
        x0 = cx + (lane % 4) * 8 - 12; y0 = (lane & 1) ? cy - 40 : cy + 32;
        x1 = x0; y1 = cy;
        break;
    case DAEMONS_GLYPH_SWEEP:
        x0 = cx - 56; y0 = cy + (lane % 5) * 6 - 12;
        x1 = cx + 56; y1 = y0;
        break;
    case DAEMONS_GLYPH_HOLD:
        if (lane < SIG_NOTE_LANE)
        {
            x0 = cx + (lane % 3) * 10 - 10;       // the word as a block of three rows of three
            y0 = cy - 14 + (lane / 3) * 10;
        }
        else
        {
            x0 = cx + (lane - SIG_NOTE_LANE) * 12 - 12;
            y0 = cy - 26;
        }
        break;
    }
    if (shape == HRL_SHAPE_HEART)
    {
        x0 = cx + sHeart[lane % 12][0];           // PAIRING DAY: a heart of bits -- never vanilla's heart sprite
        y0 = cy - 10 + sHeart[lane % 12][1];
    }
    else if (shape == HRL_SHAPE_STARS)
    {
        u8 a = lane * 21 + (sig->word & 0x1F);    // SCIENCE FAIR: a ring frozen into points, a star chart
        s16 r = 14 + ((sig->word >> (lane % 7)) & 3) * 7;
        x0 = cx + Cos(a, r);
        y0 = cy + Sin(a, r) / 2 - 6;
    }
    if (sub < 3)
        sub = 3;
    spriteId = CreateSprite(template, x0, y0, sub);
    if (spriteId == MAX_SPRITES)
        return;
    sprite = &gSprites[spriteId];
    sprite->sTimer = 0;
    sprite->sFrames = frames;
    sprite->sX0 = x0;
    sprite->sY0 = y0;
    sprite->sX1 = x1;
    sprite->sY1 = y1;
    sprite->sPhase = phase | ((sig->wave & 0xFF) << 8);
    sprite->sShape = (path & 0xF) | (mods << 4) | (notes ? SHAPE_NOTES : 0) | (frame == 1 ? SHAPE_BIT : 0) | extra;
    StartSpriteAnim(sprite, frame);
}

#undef sTimer
#undef sFrames
#undef sX0
#undef sY0
#undef sX1
#undef sY1
#undef sPhase
#undef sShape

// ---------------------------------------------------------------------------------------------------------- the task

#define tMove    data[0]
#define tTimer   data[1]
#define tPlucks  data[2]
#define tNotes   data[3]   // note glyphs launched (three at most)
#define tOver    data[4]   // frames past the picture
#define tMode    data[5]
#define tTonic   data[6]
#define tEnd     data[7]
#define tFlute   data[8]   // the last note was the FLUTE's, and is still held
#define tSound   data[9]
#define tStopAt  data[10]
#define tHit     data[11]
#define tFlight  data[12]
#define tLook    13        // data[13] and [14]: the holiday look (a word: SetWordTaskArg), or NULL
#define tVolume  data[15]  // the notes' volume: 256, or a QUIET day's soft

static u8 NoteStep(const struct DaemonsSignature *sig)
{
    return sig->tempo / 4 < 3 ? 3 : sig->tempo / 4;
}

static u8 Flight(const struct DaemonsSignature *sig)
{
    if (sig->mods & SIG_MOD_SLOW)
        return sig->tempo * 3 / 2;
    if (sig->mods & SIG_MOD_FAST)
        return sig->tempo * 2 / 3;
    return sig->tempo;
}

static bool8 IsOurNote(struct SongHeader *header)
{
    u16 song;

    for (song = SE_HARP_C; song <= SE_FLUTE_C_HIGH; song++)
        if (gSongTable[song].header == header)
            return TRUE;
    return FALSE;
}

// SE3 carries the low-HP alarm (and the field's rain): never sound over a song that is not one of PLAY's notes.
static bool8 MaySound(void)
{
    if (!(gMPlayInfo_SE3.status & MUSICPLAYER_STATUS_TRACK) || (gMPlayInfo_SE3.status & MUSICPLAYER_STATUS_PAUSE))
        return TRUE;
    return IsOurNote(gMPlayInfo_SE3.songHeader);
}

static void Pluck(s16 *data, const struct DaemonsSignature *sig, u8 k)
{
    u8 degree = (sig->figure >> (6 - 3 * (k % 3))) & 7;
    u8 instrument = (sig->flags & SIG_UNION) ? ((k & 1) ? PLAY_FLUTE : PLAY_HARP) : PLAY_INSTRUMENT;

    if (!tSound)
        return;
    Play_SoundNote(instrument, tTonic + sMajor8[degree], TRUE);
    tFlute = (instrument == PLAY_FLUTE);
    if (tFlute || tVolume != 256)
        m4aMPlayVolumeControl(&gMPlayInfo_SE3, TRACKS_ALL, tVolume);   // at SE volume, not PLAY's breath (soft when QUIET)
}

static void KillGlyphs(void)
{
    u8 i;

    for (i = 0; i < MAX_SPRITES; i++)
        if (gSprites[i].inUse && gSprites[i].callback == SpriteCB_SignatureGlyph)
            DestroySprite(&gSprites[i]);
}

static void Stop(u8 taskId)
{
    s16 *data = gTasks[taskId].data;

    if (tSound && tFlute && IsOurNote(gMPlayInfo_SE3.songHeader))
        m4aMPlayStop(&gMPlayInfo_SE3);            // the FLUTE is held until stopped; the HARP rings out
    KillGlyphs();
    DestroyTask(taskId);
}

void DaemonsSignature_Start(u16 move)
{
    const struct DaemonsSignature *sig = &sDaemonsSignatures[move];
    const struct HolidayRoutineLook *look = LookFor(move);
    u8 taskId = FindTaskIdByFunc(Task_DaemonsSignature);
    s16 *data;
    u8 holiday, last, k;
    s16 lastPluck, noteStart, bitStart, end;
    u16 effect = gBattleMoves[move].effect;

    if (taskId != TASK_NONE)
        Stop(taskId);

    // The ink: each grey of ANIM_TAG_DAEMONS_INK times the type's colour, as AnimTask_DaemonsInk shades it (clause 3).
    // P3: a look in white, silver or frost wears that colour paled, as LEGACY's is -- no month has them, so no colour
    // is added. A look in lanes needs no ink: its BLACK lane is ringed in the look's first month.
    if (LookHasLanes(look))
    {
        u8 slot = IndexOfSpritePaletteTag(ANIM_TAG_DAEMONS_LANES);

        if (slot != 0xFF && (look->lanes & (1 << HRL_LANE_BLACK)))
        {
            u16 base = OBJ_PLTT_ID(slot);
            u16 ring = gPlttBufferUnfaded[base + 2 + LaneOf(look, 0)];
            gPlttBufferUnfaded[base + LANE_RING_INDEX] = gPlttBufferFaded[base + LANE_RING_INDEX] = ring;
        }
    }
    else if (!(sig->flags & SIG_UNION))
    {
        u8 slot = IndexOfSpritePaletteTag(ANIM_TAG_DAEMONS_INK);
        u16 colour = sDaemonsSignatureInk[gBattleMoves[move].type];

        if (look != NULL && (look->flags & HRL_BLEACH))
        {
            u16 r = colour & 0x1F, g = (colour >> 5) & 0x1F, b = (colour >> 10) & 0x1F;
            colour = RGB(r + (31 - r) * 7 / 10, g + (31 - g) * 7 / 10, b + (31 - b) * 7 / 10);
        }
        if (slot != 0xFF)
        {
            u16 i, base = OBJ_PLTT_ID(slot);
            for (i = 1; i < 16; i++)
            {
                u16 grey = gPlttBufferUnfaded[base + i] & 0x1F;
                u16 r = (colour & 0x1F) * grey / 31, g = ((colour >> 5) & 0x1F) * grey / 31, b = ((colour >> 10) & 0x1F) * grey / 31;
                gPlttBufferUnfaded[base + i] = gPlttBufferFaded[base + i] = RGB(r, g, b);
            }
        }
    }

    taskId = CreateTask(Task_DaemonsSignature, 3);
    data = gTasks[taskId].data;
    tMove = move;
    tTimer = 0;
    tPlucks = 0;
    tNotes = 0;
    tOver = 0;
    tFlute = FALSE;
    tSound = MaySound();
    tFlight = Flight(sig);
    SetWordTaskArg(taskId, tLook, (u32)look);
    tVolume = DaemonsHolidayIsQuiet() ? SIG_VOL_SOFT : 256;

    // The key: the type's, or on a holiday the month's note, in the type's register (an achromatic type's octave).
    tTonic = sig->tonic;
    holiday = DaemonsHoliday();
    if (holiday != HOLIDAY_NONE)
        tTonic = (sig->tonic < 0 ? sig->tonic : 0) + DaemonsHolidayMonth(holiday);   // an achromatic tonic IS its register

    // A routine that hits again plays its whole word on the first hit and one note on each hit after it; a
    // two-turn routine (or one whose later turns animate) plays its word on the first turn and its figure after.
    tMode = SIG_MODE_FULL;
    tHit = 0;
    if ((effect == EFFECT_MULTI_HIT || effect == EFFECT_DOUBLE_HIT || effect == EFFECT_TWINEEDLE
      || effect == EFFECT_TRIPLE_KICK) && gBattleScripting.multihitString[4] != 0)
    {
        tMode = SIG_MODE_ONE_NOTE;
        tHit = gBattleScripting.multihitString[4];
    }
    else if (gAnimMoveTurn != 0)
    {
        tMode = SIG_MODE_FIGURE;
    }

    // When every glyph has landed and the last note is done.
    if (tMode == SIG_MODE_ONE_NOTE)
    {
        lastPluck = 0;
        end = tFlight;
    }
    else
    {
        noteStart = (tMode == SIG_MODE_FIGURE || (sig->flags & SIG_NOTES_LEAD)) ? 0 : SIG_LAG;
        bitStart = (sig->flags & SIG_NOTES_LEAD) ? SIG_LAG : 0;
        last = 0;
        for (k = 0; k < 9; k++)
            if ((sig->rhythm >> (8 - k)) & 1)
                last = k;
        lastPluck = noteStart + last * NoteStep(sig);
        end = lastPluck + tFlight;
        if (tMode == SIG_MODE_FULL && bitStart + 8 * SIG_BIT_STEP + tFlight > end)
            end = bitStart + 8 * SIG_BIT_STEP + tFlight;
    }
    tStopAt = lastPluck + SIG_FLUTE_HOLD;
    if (end < tStopAt + 5)
        end = tStopAt + 5;
    tEnd = end + 1;
    Task_DaemonsSignature(taskId);
}

static void Task_DaemonsSignature(u8 taskId)
{
    s16 *data = gTasks[taskId].data;
    const struct DaemonsSignature *sig = &sDaemonsSignatures[tMove];
    const struct HolidayRoutineLook *look = (const struct HolidayRoutineLook *)GetWordTaskArg(taskId, tLook);
    bool8 splash = (sig->flags & SIG_UNION) != 0;
    s16 t = tTimer, i;
    u8 step = NoteStep(sig);
    // P3: a heart or a star chart is built glyph by glyph and held whole to the end; anything else flies its flight.
    u8 flight = (look != NULL && (look->shape == HRL_SHAPE_HEART || look->shape == HRL_SHAPE_STARS) && tEnd > t)
              ? tEnd - t : tFlight;

    if (tMode == SIG_MODE_ONE_NOTE)
    {
        if (t == 0)
        {
            Pluck(data, sig, tHit);
            LaunchGlyph(sig, look, splash, SIG_NOTE_LANE + tHit % 3, TRUE, 2 + (tHit & 1), flight);
        }
    }
    else
    {
        s16 bitStart = (sig->flags & SIG_NOTES_LEAD) ? SIG_LAG : 0;
        s16 noteStart = (tMode == SIG_MODE_FIGURE || (sig->flags & SIG_NOTES_LEAD)) ? 0 : SIG_LAG;

        i = t - bitStart;
        if (tMode == SIG_MODE_FULL && i >= 0 && i % SIG_BIT_STEP == 0 && i / SIG_BIT_STEP < 9)
        {
            u8 lane = i / SIG_BIT_STEP;
            LaunchGlyph(sig, look, splash, lane, FALSE, (sig->word >> (8 - lane)) & 1, flight);
        }
        i = t - noteStart;
        if (i >= 0 && i % step == 0 && i / step < 9 && ((sig->rhythm >> (8 - i / step)) & 1))
        {
            Pluck(data, sig, tPlucks);
            if (tNotes < 3)
            {
                LaunchGlyph(sig, look, splash, SIG_NOTE_LANE + tNotes, TRUE, 2 + (tNotes & 1), flight);
                tNotes++;
            }
            tPlucks++;
        }
    }

    // The FLUTE breathes out after its last note; the HARP is left to ring.
    if (tSound && tFlute && t >= tStopAt && IsOurNote(gMPlayInfo_SE3.songHeader))
    {
        s16 f = t - tStopAt;
        if (f >= 5)
        {
            m4aMPlayStop(&gMPlayInfo_SE3);
            tFlute = FALSE;
        }
        else
        {
            m4aMPlayVolumeControl(&gMPlayInfo_SE3, TRACKS_ALL, tVolume * (5 - f) / 5);
        }
    }

    if (++tTimer > tEnd)
        Stop(taskId);
}

bool8 DaemonsSignature_Holds(void)
{
    u8 taskId = FindTaskIdByFunc(Task_DaemonsSignature);

    if (taskId == TASK_NONE)
        return FALSE;
    if (++gTasks[taskId].tOver > SIG_FUSE)
    {
        Stop(taskId);   // never in play: the task ends itself (tEnd) long before this
        return FALSE;
    }
    return TRUE;   // NO CAP (the user, 2026-10-18): the signature runs in full, and `end` waits for it
}
