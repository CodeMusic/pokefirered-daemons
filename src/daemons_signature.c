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
// had been lost on its own pink. The union routines' own scripts are still debug-only; the release ROMs play their row.

#include "data/daemons_signatures.h"

#define SIG_BIT_STEP  2    // frames between the word's glyphs
#define SIG_LAG       12   // frames the second kind waits for the first
#define SIG_OVERRUN   12   // frames it may run past the picture (CLEAR)
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

// ---------------------------------------------------------------------------------------------------------- which

bool8 DaemonsSignature_Wants(u16 move)
{
    const struct DaemonsSignature *sig;

    if (move == MOVE_NONE || move >= MOVES_COUNT)
        return FALSE;
    sig = &sDaemonsSignatures[move];
    if (sig->word == 0 && sig->rhythm == 0)
        return FALSE;
#if DAEMONS_DEBUG
    if (sig->flags & SIG_OWN_SCRIPT)
        return FALSE;   // a union routine's own script carries its signature, and it plays in these ROMs
    return TRUE;
#else
    return (sig->flags & SIG_APPROVED) != 0;   // a union routine's own script is debug-only: its row plays here
#endif
}

bool8 DaemonsSignature_WantsSplash(u16 move)
{
    return (sDaemonsSignatures[move].flags & SIG_UNION) != 0;
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
#define sShape  data[7]   // path | mods << 4 | notes << 12 | the bit << 13

#define SHAPE_PATH(s)  ((s) & 0xF)
#define SHAPE_MODS(s)  (((s) >> 4) & 0xFF)
#define SHAPE_NOTES    (1 << 12)
#define SHAPE_BIT      (1 << 13)

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

static void LaunchGlyph(const struct DaemonsSignature *sig, bool8 splash, u8 lane, bool8 notes, u8 frame, u8 frames)
{
    const struct SpriteTemplate *template = splash ? &sSignatureSplashTemplate : &sSignatureInkTemplate;
    u8 self = (sig->mods & SIG_MOD_SELF) != 0;
    s16 ax = GetBattlerSpriteCoord(gBattleAnimAttacker, BATTLER_COORD_X_2);
    s16 ay = GetBattlerSpriteCoord(gBattleAnimAttacker, BATTLER_COORD_Y_PIC_OFFSET);
    s16 tx = GetBattlerSpriteCoord(gBattleAnimTarget, BATTLER_COORD_X_2);
    s16 ty = GetBattlerSpriteCoord(gBattleAnimTarget, BATTLER_COORD_Y_PIC_OFFSET);
    s16 cx = self ? ax : tx, cy = self ? ay : ty;
    s16 x0 = ax, y0 = ay, x1 = tx, y1 = ty;
    u8 phase = ((sig->word >> (lane % 9)) & 7) * 24 + lane * 24;   // the lane order, from the word
    s16 sub = GetBattlerSpriteSubpriority(gBattleAnimAttacker) - 2;
    u8 spriteId;
    struct Sprite *sprite;

    switch (sig->path)
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
        phase = lane * 28 + (sig->word & 0x1F);
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
    sprite->sShape = (sig->path & 0xF) | (sig->mods << 4) | (notes ? SHAPE_NOTES : 0) | (frame == 1 ? SHAPE_BIT : 0);
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
    if (tFlute)
        m4aMPlayVolumeControl(&gMPlayInfo_SE3, TRACKS_ALL, 256);   // at SE volume, not PLAY's breath
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
    u8 taskId = FindTaskIdByFunc(Task_DaemonsSignature);
    s16 *data;
    u8 holiday, last, k;
    s16 lastPluck, noteStart, bitStart, end;
    u16 effect = gBattleMoves[move].effect;

    if (taskId != TASK_NONE)
        Stop(taskId);

    // The ink: each grey of ANIM_TAG_DAEMONS_INK times the type's colour, as AnimTask_DaemonsInk shades it (clause 3).
    if (!(sig->flags & SIG_UNION))
    {
        u8 slot = IndexOfSpritePaletteTag(ANIM_TAG_DAEMONS_INK);
        u16 colour = sDaemonsSignatureInk[gBattleMoves[move].type];

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
    bool8 splash = (sig->flags & SIG_UNION) != 0;
    s16 t = tTimer, i;
    u8 step = NoteStep(sig);

    if (tMode == SIG_MODE_ONE_NOTE)
    {
        if (t == 0)
        {
            Pluck(data, sig, tHit);
            LaunchGlyph(sig, splash, SIG_NOTE_LANE + tHit % 3, TRUE, 2 + (tHit & 1), tFlight);
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
            LaunchGlyph(sig, splash, lane, FALSE, (sig->word >> (8 - lane)) & 1, tFlight);
        }
        i = t - noteStart;
        if (i >= 0 && i % step == 0 && i / step < 9 && ((sig->rhythm >> (8 - i / step)) & 1))
        {
            Pluck(data, sig, tPlucks);
            if (tNotes < 3)
            {
                LaunchGlyph(sig, splash, SIG_NOTE_LANE + tNotes, TRUE, 2 + (tNotes & 1), tFlight);
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
            m4aMPlayVolumeControl(&gMPlayInfo_SE3, TRACKS_ALL, 256 * (5 - f) / 5);
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
    if (++gTasks[taskId].tOver > SIG_OVERRUN)
    {
        Stop(taskId);   // CLEAR: up to 12 frames past the picture, and no further
        return FALSE;
    }
    return TRUE;
}
