#include "global.h"
#include "gflib.h"
#include "event_object_lock.h"
#include "event_data.h"
#include "event_object_movement.h"
#include "event_scripts.h"
#include "field_player_avatar.h"
#include "fieldmap.h"
#include "map_name_popup.h"
#include "menu.h"
#include "play.h"
#include "m4a.h"
#include "quest_log.h"
#include "random.h"
#include "script.h"
#include "sound.h"
#include "string_util.h"
#include "task.h"
#include "text_window.h"
#include "trig.h"
#include "constants/event_objects.h"
#include "constants/flags.h"
#include "constants/items.h"
#include "constants/maps.h"
#include "constants/songs.h"

// T-417: PLAY (the user, 2026-10-17; docs/private/draft-play-2026-10-17.md). The edition's instrument -- CONTENT's
// HARP, CONTEXT's FLUTE -- is taken out from the BAG (or SELECT), and a KEY WHEEL of twelve coloured dots rings the
// player: C at twelve o'clock, round clockwise, each key in the user's colour (A is INDIGO everywhere). LEFT and RIGHT
// turn it, A chooses, and the wheel folds up into the STAFF across the top of the screen. Then the buttons are the
// seven notes of the key, the D-pad clockwise from the bottom -- DOWN 1, LEFT 2, UP 3, RIGHT 4 -- then B 5, A 6,
// SELECT 7; L held is an octave down and R an octave up; START puts it away. Each note sounds, is written on the staff
// as a small head of its colour (the last twelve, sliding left), and a note of its colour drifts up from the
// instrument and flickers out. The HARP is plucked (a press, which rings and dies by itself, on SE2 and SE3 in turn so
// one note rings under the next); the FLUTE is breathed (it swells in, sounds while the button is held, and fades
// when it is let go).
//
// A FIELD TASK, not a screen: the point is to play in front of things. Field input is locked while it runs, so the
// START menu, SELECT's registered item, R = AGAIN, stepping and every script a step or a press could start are quiet;
// people keep walking. Everything it uses is on the heap or in VRAM and freed on put-away -- no EWRAM, no save (the
// draft's sec. 11). The notes ALWAYS SHINE (the user, 2026-10-17): their palette is put back untinted every frame, and
// HALFTONE's grey passes it by (fieldmap.c).
//
// P2 (the user, 2026-10-17): every note goes into a ring of the last sixteen, and after each one the songs PLAY knows
// are listened for in it -- ORDER ONLY: tempo and octave do not count, and THE KEY DOES. A song played in its own key
// is MATCHED: its notes rise and hold, a line of dots in their colours joins them, and the song comes back in ECHO --
// on SE2, and eight frames later on SE3 at two-fifths -- before what it opens happens. The right shape in another key
// is ALMOST: something answers that it sits wrong. O CHRISTMAS TREE in B-flat, played facing any singing fir, keys it
// and opens its door (no YES/NO -- it was played open); played to THE NEXUS's Christmas tree, the tree lights. It is
// listened for from the start, sheet or no sheet (the user: the band's sheet is the hint, not the lock). The other
// seven are listened for once learned, and their melodies are still to come (P4).
//
// Not yet (P3-P5): ghost notes, the giver and the palm, the last key used. Every word here is DRAFT.

#define PLAY_TAG_NOTES 0x1040   // 16x16 x 12, a note of each key's colour
#define PLAY_TAG_SMALL 0x1041   // 8x8 x 24: twelve wheel dots, then twelve staff heads

#define WHEEL_RADIUS   28
#define FOLD_FRAMES    16
#define STAFF_HEADS    12
#define DRIFT_MAX      8
#define STAFF_TOP_TILE 0x050    // past the map name popup's tiles, short of the YES/NO box's 0x125 and the dialog's 0x198
#define STAFF_PAL      13       // the popup's own: 13-15 are the UI's, never the map's
#define STAFF_W        30
#define STAFF_H        6        // 48px: three octaves of heads at a step and a half each
#define STAFF_Y0       30       // where step 0 (middle C) sits; a line every two steps, E4 to F5 (steps 2-10)
#define STAFF_X0       84       // the lines start here; the key's note and name sit to their left
#define HEAD_RIGHT     228      // the newest head
#define HEAD_GAP       12
#define KEY_MARK_X     12       // the key's note, at the staff's left
#define KEY_MARK_Y     22
#define KEY_NAME_X     26
#define RING_MAX       16       // the last sixteen notes, listened in
#define MOTIF_MAX      8
#define JOIN_GAP       18       // between the joined notes
#define JOIN_BELOW     36       // the joined line's lowest note, below the player (above is the staff's)
#define ECHO_DELAY     8        // SE3 answers SE2 this many frames late
#define ECHO_SOFT      102      // ...at two-fifths
#define ECHO_TAIL      24       // frames held after the last echo note

enum { STATE_WAIT, STATE_WHEEL, STATE_FOLD, STATE_PLAY, STATE_JOINED };
enum { TARGET_NONE, TARGET_FIR, TARGET_NEXUS_TREE };

struct Play
{
    u8 instrument;
    u8 state;
    u8 key;
    u8 palSlot;
    bool8 borrowed;          // no free palette slot: the player's reflection slot is lent, and given back
    u8 windowId;
    u8 timer;
    u8 headCount;
    u8 nextDrift;
    bool8 onSe3;             // the HARP's next note goes to SE3
    u8 breath;               // the FLUTE: frames into the note's swell, or of its fade
    bool8 fading;
    u16 heldButton;          // the FLUTE: the button whose note is sounding
    s16 cx, cy;              // the instrument, on screen
    u8 dots[12];
    u8 keyNote;
    u8 heads[STAFF_HEADS];
    u8 drift[DRIFT_MAX];
    u16 lentColours[16];
    s8 ring[RING_MAX];       // P2: the notes played, as semitones from middle C, oldest first
    u8 ringCount;
    u8 song;                 // the song matched
    u8 target;               // what it was played to (TARGET_*)
    u16 clock;               // frames into the echo
    s8 echo[MOTIF_MAX];      // the matched notes, as they were played
    u8 joined[MOTIF_MAX];
    u8 joinDots[2 * (MOTIF_MAX - 1)];
    u8 pulse[MOTIF_MAX];
};

// ------------------------------------------------------------------------------------------------------------ songs

// A song: its key, its motif as pitch classes (C 0 .. B 11) and as staff steps for the SONGBOOK (letters from middle
// C, so it reads as pitch), and how long each note is held when it echoes. A motif of no notes is still to come.
struct PlaySong
{
    u16 flag;                // learned
    u8 key;
    u8 count;
    const u8 *pcs;
    const u8 *steps;
    const u8 *frames;
};

// O CHRISTMAS TREE's opening, in B-flat: F Bb Bb Bb C D D D -- on the buttons B, DOWN x3, LEFT, UP x3. The fir's own
// sFirTune (field_specials.c) is the reference; the echo keeps its first eight notes' lengths.
static const u8 sFir_Pcs[]    = {5, 10, 10, 10, 0, 2, 2, 2};
static const u8 sFir_Steps[]  = {3, 6, 6, 6, 7, 8, 8, 8};      // F4 B4 B4 B4 C5 D5 D5 D5, on the staff
static const u8 sFir_Frames[] = {30, 22, 8, 40, 18, 22, 8, 40};

// The seven (the user's starting pairings, TODO T-417): their motifs come from the user's albums (P4). Until then each
// is a row with no notes -- never listened for, and in the SONGBOOK only in the DEBUG build.
static const struct PlaySong sSongs[PLAY_SONG_COUNT] =
{
    [PLAY_SONG_FIR] = {FLAG_SONG_FIR, 10, ARRAY_COUNT(sFir_Pcs), sFir_Pcs, sFir_Steps, sFir_Frames},
    [PLAY_SONG_1 + 0] = {FLAG_SONG_1 + 0},   // TWELVE DAYS OF DECEMBER   SLATE / NAMING      TBD
    [PLAY_SONG_1 + 1] = {FLAG_SONG_1 + 1},   // THE REALIZATION           SLOPE / STILL       TBD
    [PLAY_SONG_1 + 2] = {FLAG_SONG_1 + 2},   // 1001 / FORGIVENESS        SENSE / MEND        TBD
    [PLAY_SONG_1 + 3] = {FLAG_SONG_1 + 3},   // SEQUEL THE GUARDIAN       FIT / PRACTICE      TBD
    [PLAY_SONG_1 + 4] = {FLAG_SONG_1 + 4},   // HARMONY IN BALANCE        SKEW / STANCE       TBD
    [PLAY_SONG_1 + 5] = {FLAG_SONG_1 + 5},   // THE SHIMMERING CANVAS     FRAME / NOTICE      TBD
    [PLAY_SONG_1 + 6] = {FLAG_SONG_1 + 6},   // REFLECTIONS WITHIN        HEAT / REFLECT      TBD
};

static const u16 sKeyColours_Pal[] = INCBIN_U16("graphics/play/key_colours.gbapal");
static const u32 sNotes_Gfx[]      = INCBIN_U32("graphics/play/notes.4bpp");
static const u32 sSmall_Gfx[]      = INCBIN_U32("graphics/play/small.4bpp");

static const struct SpriteSheet sPlaySheets[] = {
    {sNotes_Gfx, sizeof(sNotes_Gfx), PLAY_TAG_NOTES},
    {sSmall_Gfx, sizeof(sSmall_Gfx), PLAY_TAG_SMALL},
};
static const struct SpritePalette sKeyColoursPalette = {sKeyColours_Pal, PALTAG_KEY_COLOURS};

static const struct OamData sOam_Note = {
    .shape = SPRITE_SHAPE(16x16),
    .size = SPRITE_SIZE(16x16),
    .priority = 1,
};
static const struct OamData sOam_Small = {
    .shape = SPRITE_SHAPE(8x8),
    .size = SPRITE_SIZE(8x8),
    .priority = 0,           // over the staff's paper on BG0
};

#define NOTE_FRAME(k)  {ANIMCMD_FRAME((k) * 4, 1), ANIMCMD_END}
#define SMALL_FRAME(k) {ANIMCMD_FRAME((k), 1), ANIMCMD_END}
static const union AnimCmd sAnim_Note[12][2] = {
    NOTE_FRAME(0), NOTE_FRAME(1), NOTE_FRAME(2), NOTE_FRAME(3), NOTE_FRAME(4), NOTE_FRAME(5),
    NOTE_FRAME(6), NOTE_FRAME(7), NOTE_FRAME(8), NOTE_FRAME(9), NOTE_FRAME(10), NOTE_FRAME(11),
};
static const union AnimCmd sAnim_Small[24][2] = {
    SMALL_FRAME(0), SMALL_FRAME(1), SMALL_FRAME(2), SMALL_FRAME(3), SMALL_FRAME(4), SMALL_FRAME(5),
    SMALL_FRAME(6), SMALL_FRAME(7), SMALL_FRAME(8), SMALL_FRAME(9), SMALL_FRAME(10), SMALL_FRAME(11),
    SMALL_FRAME(12), SMALL_FRAME(13), SMALL_FRAME(14), SMALL_FRAME(15), SMALL_FRAME(16), SMALL_FRAME(17),
    SMALL_FRAME(18), SMALL_FRAME(19), SMALL_FRAME(20), SMALL_FRAME(21), SMALL_FRAME(22), SMALL_FRAME(23),
};
static const union AnimCmd *const sAnims_Note[12] = {
    sAnim_Note[0], sAnim_Note[1], sAnim_Note[2], sAnim_Note[3], sAnim_Note[4], sAnim_Note[5],
    sAnim_Note[6], sAnim_Note[7], sAnim_Note[8], sAnim_Note[9], sAnim_Note[10], sAnim_Note[11],
};
static const union AnimCmd *const sAnims_Small[24] = {
    sAnim_Small[0], sAnim_Small[1], sAnim_Small[2], sAnim_Small[3], sAnim_Small[4], sAnim_Small[5],
    sAnim_Small[6], sAnim_Small[7], sAnim_Small[8], sAnim_Small[9], sAnim_Small[10], sAnim_Small[11],
    sAnim_Small[12], sAnim_Small[13], sAnim_Small[14], sAnim_Small[15], sAnim_Small[16], sAnim_Small[17],
    sAnim_Small[18], sAnim_Small[19], sAnim_Small[20], sAnim_Small[21], sAnim_Small[22], sAnim_Small[23],
};

static void SpriteCB_Drift(struct Sprite *sprite);

// The palette is not named in the templates: each sprite is pointed at the slot PLAY holds, its own or a lent one.
static const struct SpriteTemplate sTemplate_Note = {
    .tileTag = PLAY_TAG_NOTES, .paletteTag = TAG_NONE, .oam = &sOam_Note, .anims = sAnims_Note,
    .images = NULL, .affineAnims = gDummySpriteAffineAnimTable, .callback = SpriteCallbackDummy,
};
static const struct SpriteTemplate sTemplate_Drift = {
    .tileTag = PLAY_TAG_NOTES, .paletteTag = TAG_NONE, .oam = &sOam_Note, .anims = sAnims_Note,
    .images = NULL, .affineAnims = gDummySpriteAffineAnimTable, .callback = SpriteCB_Drift,
};
static const struct SpriteTemplate sTemplate_Small = {
    .tileTag = PLAY_TAG_SMALL, .paletteTag = TAG_NONE, .oam = &sOam_Small, .anims = sAnims_Small,
    .images = NULL, .affineAnims = gDummySpriteAffineAnimTable, .callback = SpriteCallbackDummy,
};

// The ring, C at twelve o'clock and round clockwise, radius 28.
static const s8 sWheel[12][2] = {
    {  0, -28}, { 14, -24}, { 24, -14}, { 28,   0}, { 24,  14}, { 14,  24},
    {  0,  28}, {-14,  24}, {-24,  14}, {-28,   0}, {-24, -14}, {-14, -24},
};

static const u8 sMajor[7] = {0, 2, 4, 5, 7, 9, 11};
// Each key's tonic as a letter (C 0 .. B 6), spelled as its signature would; G and up sit an octave lower, so every
// key's middle octave starts within a fifth of middle C and three octaves fit the staff.
static const s8 sKeyLetter[12] = {0, 1, 1, 2, 2, 3, 3, 4 - 7, 5 - 7, 5 - 7, 6 - 7, 6 - 7};
static const s8 sKeyTonic[12]  = {0, 1, 2, 3, 4, 5, 6, 7 - 12, 8 - 12, 9 - 12, 10 - 12, 11 - 12};

// The natural each semitone is played from, and the semitones of those naturals.
static const u8 sNaturalOf[12] = {0, 0, 1, 1, 2, 3, 3, 4, 4, 5, 5, 6};
static const u8 sNaturalSemi[7] = {0, 2, 4, 5, 7, 9, 11};

// DRAFT: the keys' names, spelled as their signatures would be (the charmap has no flat sign).
static const u8 sKeyName_C[]  = _("C");
static const u8 sKeyName_Db[] = _("D FLAT");
static const u8 sKeyName_D[]  = _("D");
static const u8 sKeyName_Eb[] = _("E FLAT");
static const u8 sKeyName_E[]  = _("E");
static const u8 sKeyName_F[]  = _("F");
static const u8 sKeyName_Fs[] = _("F SHARP");
static const u8 sKeyName_G[]  = _("G");
static const u8 sKeyName_Ab[] = _("A FLAT");
static const u8 sKeyName_A[]  = _("A");
static const u8 sKeyName_Bb[] = _("B FLAT");
static const u8 sKeyName_B[]  = _("B");
static const u8 *const sKeyNames[12] = {
    sKeyName_C, sKeyName_Db, sKeyName_D, sKeyName_Eb, sKeyName_E, sKeyName_F,
    sKeyName_Fs, sKeyName_G, sKeyName_Ab, sKeyName_A, sKeyName_Bb, sKeyName_B,
};

static const u8 sStaffColours[3] = {TEXT_COLOR_WHITE, TEXT_COLOR_DARK_GRAY, TEXT_COLOR_LIGHT_GRAY};

// The seven notes, in the order the hand climbs them.
static const u16 sNoteButtons[7] = {DPAD_DOWN, DPAD_LEFT, DPAD_UP, DPAD_RIGHT, B_BUTTON, A_BUTTON, SELECT_BUTTON};

#define tPlay  0   // the state's pointer, in data[0] and data[1]

static struct Play *GetPlay(u8 taskId)
{
    return (struct Play *)GetWordTaskArg(taskId, tPlay);
}

// ------------------------------------------------------------------------------------------------------------ sound

static struct MusicPlayerInfo *NotePlayer(bool8 onSe3)
{
    return onSe3 ? &gMPlayInfo_SE3 : &gMPlayInfo_SE2;
}

void Play_SoundNote(u8 instrument, s16 semitone, bool8 onSe3)
{
    struct MusicPlayerInfo *mp = NotePlayer(onSe3);
    s16 octave = (semitone >= 0) ? semitone / 12 : -((11 - semitone) / 12);
    s16 pc = semitone - octave * 12;
    u8 natural = sNaturalOf[pc];
    u16 song = (instrument == PLAY_FLUTE ? SE_FLUTE_C : SE_HARP_C) + natural;

    m4aMPlayStop(mp);
    MPlayStart(mp, gSongTable[song].header);
    m4aMPlayImmInit(mp);
    m4aMPlayPitchControl(mp, TRACKS_ALL, (semitone - sNaturalSemi[natural]) * 256);
    if (instrument == PLAY_FLUTE)
        m4aMPlayVolumeControl(mp, TRACKS_ALL, 64);   // it swells in (PlayBreath)
}

// The FLUTE's breath, once a frame: up from a quarter over six frames, or down to nothing over five and stopped.
static bool8 PlayBreath(struct MusicPlayerInfo *mp, u8 *breath, bool8 fading)
{
    if (!fading)
    {
        if (*breath < 6)
        {
            (*breath)++;
            m4aMPlayVolumeControl(mp, TRACKS_ALL, 64 + 192 * *breath / 6);
        }
        return TRUE;
    }
    if (*breath == 0)
        return FALSE;
    (*breath)--;
    if (*breath == 0)
        m4aMPlayStop(mp);
    else
        m4aMPlayVolumeControl(mp, TRACKS_ALL, 256 * *breath / 6);
    return *breath != 0;
}

// ---------------------------------------------------------------------------------------------------------- sprites

static u8 MakeSprite(struct Play *play, const struct SpriteTemplate *template, s16 x, s16 y, u8 anim)
{
    u8 spriteId = CreateSprite(template, x, y, 0);

    if (spriteId == MAX_SPRITES)
        return MAX_SPRITES;
    gSprites[spriteId].oam.paletteNum = play->palSlot;
    StartSpriteAnim(&gSprites[spriteId], anim);
    return spriteId;
}

static void DestroyIf(u8 *spriteId)
{
    if (*spriteId != MAX_SPRITES)
    {
        DestroySprite(&gSprites[*spriteId]);
        *spriteId = MAX_SPRITES;
    }
}

#define sAge   data[0]
#define sLife  data[1]
#define sRise  data[2]
#define sX0    data[3]
#define sY0    data[4]
#define sSway  data[5]

// HARP: up 24px in 32 frames; FLUTE: slower and further, 48px in 48, swaying like breath. The last eight frames flicker.
static void SpriteCB_Drift(struct Sprite *sprite)
{
    if (sprite->sAge >= sprite->sLife)
    {
        sprite->invisible = TRUE;
        return;
    }
    sprite->sAge++;
    sprite->y = sprite->sY0 - sprite->sRise * sprite->sAge / sprite->sLife;
    if (sprite->sSway)
        sprite->x = sprite->sX0 + Sin((sprite->sAge * 8) & 0xFF, 4);
    if (sprite->sLife - sprite->sAge < 8)
        sprite->invisible = sprite->sAge & 1;
}

static void SpawnDrift(struct Play *play, u8 key)
{
    u8 *slot = &play->drift[play->nextDrift];
    s16 x = play->cx + (Random() % 9) - 4;
    s16 y = play->cy - 8;

    DestroyIf(slot);   // eight in the air at most: the oldest goes
    *slot = MakeSprite(play, &sTemplate_Drift, x, y, key);
    if (*slot != MAX_SPRITES)
    {
        struct Sprite *s = &gSprites[*slot];
        s->sX0 = x;
        s->sY0 = y;
        s->sLife = play->instrument == PLAY_FLUTE ? 48 : 32;
        s->sRise = play->instrument == PLAY_FLUTE ? 48 : 24;
        s->sSway = play->instrument == PLAY_FLUTE;
    }
    play->nextDrift = (play->nextDrift + 1) % DRIFT_MAX;
}

#undef sAge
#undef sLife
#undef sRise
#undef sX0
#undef sY0
#undef sSway

// ------------------------------------------------------------------------------------------------------------ staff

static void PrintKeyName(struct Play *play)
{
    if (play->windowId == WINDOW_NONE)
        return;
    FillWindowPixelRect(play->windowId, PIXEL_FILL(TEXT_COLOR_WHITE), KEY_NAME_X, 14, STAFF_X0 - 4 - KEY_NAME_X, 18);
    AddTextPrinterParameterized4(play->windowId, FONT_NORMAL, KEY_NAME_X, 15, 0, 0, sStaffColours, TEXT_SKIP_DRAW,
                                 sKeyNames[play->key]);
    CopyWindowToVram(play->windowId, COPYWIN_GFX);
}

// The paper, and five lines drawn once (engine.md trap 22).
static void OpenStaff(struct Play *play)
{
    struct WindowTemplate template = {
        .bg = 0, .tilemapLeft = 0, .tilemapTop = 0, .width = STAFF_W, .height = STAFF_H,
        .paletteNum = STAFF_PAL, .baseBlock = STAFF_TOP_TILE,
    };
    u8 i;

    play->windowId = AddWindow(&template);
    if (play->windowId == WINDOW_NONE)
        return;
    LoadPalette(GetTextWindowPalette(3), BG_PLTT_ID(STAFF_PAL), PLTT_SIZE_4BPP);
    FillWindowPixelBuffer(play->windowId, PIXEL_FILL(TEXT_COLOR_WHITE));
    for (i = 0; i < 5; i++)   // steps 2, 4, 6, 8, 10
        FillWindowPixelRect(play->windowId, PIXEL_FILL(TEXT_COLOR_DARK_GRAY), STAFF_X0, STAFF_Y0 - 3 * (i + 1), 240 - 4 - STAFF_X0, 1);
    FillWindowPixelRect(play->windowId, PIXEL_FILL(TEXT_COLOR_LIGHT_GRAY), 0, STAFF_H * 8 - 1, 240, 1);
    PutWindowTilemap(play->windowId);
    PrintKeyName(play);
    CopyWindowToVram(play->windowId, COPYWIN_FULL);
}

static void CloseStaff(struct Play *play)
{
    if (play->windowId == WINDOW_NONE)
        return;
    ClearWindowTilemap(play->windowId);
    CopyWindowToVram(play->windowId, COPYWIN_MAP);
    RemoveWindow(play->windowId);
    play->windowId = WINDOW_NONE;
}

// A head for degree d (0-6) in the key, `octave` -1/0/+1: placed by its letter, so the staff reads as pitch.
static void WriteHead(struct Play *play, u8 degree, s8 octave, u8 pc)
{
    s16 step = sKeyLetter[play->key] + degree + 7 * octave;
    u8 i;

    if (play->headCount == STAFF_HEADS)
    {
        DestroyIf(&play->heads[0]);
        for (i = 1; i < STAFF_HEADS; i++)
            play->heads[i - 1] = play->heads[i];
        play->headCount--;
    }
    for (i = 0; i < play->headCount; i++)
        if (play->heads[i] != MAX_SPRITES)
            gSprites[play->heads[i]].x -= HEAD_GAP;
    play->heads[play->headCount++] = MakeSprite(play, &sTemplate_Small, HEAD_RIGHT + 1, STAFF_Y0 - (step * 3) / 2, 12 + pc);
}

// -------------------------------------------------------------------------------------------------------- the wheel

static void PlaceWheel(struct Play *play)
{
    u8 k;

    for (k = 0; k < 12; k++)
        if (play->dots[k] != MAX_SPRITES)
            gSprites[play->dots[k]].invisible = (k == play->key);
    if (play->keyNote != MAX_SPRITES)
    {
        gSprites[play->keyNote].x = play->cx + sWheel[play->key][0];
        gSprites[play->keyNote].y = play->cy + sWheel[play->key][1];
        StartSpriteAnim(&gSprites[play->keyNote], play->key);
    }
}

static void OpenWheel(struct Play *play)
{
    u8 k;

    for (k = 0; k < 12; k++)
        play->dots[k] = MakeSprite(play, &sTemplate_Small, play->cx + sWheel[k][0], play->cy + sWheel[k][1], k);
    play->keyNote = MakeSprite(play, &sTemplate_Note, play->cx, play->cy, play->key);
    if (play->keyNote != MAX_SPRITES)
        gSprites[play->keyNote].oam.priority = 0;
    PlaceWheel(play);
}

// The ring folds up into the staff: every dot flies to the key's mark at its left, and the chosen note with them.
static bool8 FoldWheel(struct Play *play)
{
    u8 k, t = ++play->timer;

    for (k = 0; k < 12; k++)
    {
        if (play->dots[k] == MAX_SPRITES)
            continue;
        gSprites[play->dots[k]].x = play->cx + sWheel[k][0] + (KEY_MARK_X - play->cx - sWheel[k][0]) * t / FOLD_FRAMES;
        gSprites[play->dots[k]].y = play->cy + sWheel[k][1] + (KEY_MARK_Y - play->cy - sWheel[k][1]) * t / FOLD_FRAMES;
    }
    if (play->keyNote != MAX_SPRITES)
    {
        s16 x0 = play->cx + sWheel[play->key][0], y0 = play->cy + sWheel[play->key][1];
        gSprites[play->keyNote].x = x0 + (KEY_MARK_X - x0) * t / FOLD_FRAMES;
        gSprites[play->keyNote].y = y0 + (KEY_MARK_Y - y0) * t / FOLD_FRAMES;
    }
    if (t < FOLD_FRAMES)
        return FALSE;
    for (k = 0; k < 12; k++)
        DestroyIf(&play->dots[k]);
    return TRUE;
}

// ---------------------------------------------------------------------------------------------------- take out / away

// The notes always shine: their colours go back into the frame's palette untinted, every frame, unless the screen is
// fading (night, rain's gamma and fog's haze are all written there; HALFTONE's grey passes the slot by, fieldmap.c).
static void Shine(struct Play *play)
{
    if (!gPaletteFade.active)
        CpuCopy16(sKeyColours_Pal, &gPlttBufferFaded[OBJ_PLTT_ID(play->palSlot)], PLTT_SIZE_4BPP);
}

static bool8 TakeOut(struct Play *play)
{
    struct Sprite *avatar = &gSprites[gPlayerAvatar.spriteId];
    u8 i;

    play->cx = avatar->x + avatar->x2 + gSpriteCoordOffsetX;
    play->cy = avatar->y + avatar->y2 + gSpriteCoordOffsetY;
    play->windowId = WINDOW_NONE;
    play->keyNote = MAX_SPRITES;
    for (i = 0; i < 12; i++)
        play->dots[i] = MAX_SPRITES;
    for (i = 0; i < STAFF_HEADS; i++)
        play->heads[i] = MAX_SPRITES;
    for (i = 0; i < DRIFT_MAX; i++)
        play->drift[i] = MAX_SPRITES;
    for (i = 0; i < MOTIF_MAX; i++)
        play->joined[i] = MAX_SPRITES;
    for (i = 0; i < ARRAY_COUNT(play->joinDots); i++)
        play->joinDots[i] = MAX_SPRITES;

    LoadSpriteSheet(&sPlaySheets[0]);
    LoadSpriteSheet(&sPlaySheets[1]);
    play->palSlot = LoadSpritePalette(&sKeyColoursPalette);
    if (play->palSlot == 0xFF)
    {
        //  Four dynamic slots taken (weather, two field effects, a fourth): borrow the player's reflection slot --
        //  idle unless the player stands on reflective water, and the player stands still while playing.
        play->borrowed = TRUE;
        play->palSlot = PALSLOT_PLAYER_REFLECTION;
        CpuCopy16(&gPlttBufferUnfaded[OBJ_PLTT_ID(play->palSlot)], play->lentColours, PLTT_SIZE_4BPP);
        LoadPalette(sKeyColours_Pal, OBJ_PLTT_ID(play->palSlot), PLTT_SIZE_4BPP);
    }
    OpenStaff(play);
    OpenWheel(play);
    return TRUE;
}

static void ClearJoined(struct Play *play)
{
    u8 i;

    for (i = 0; i < MOTIF_MAX; i++)
        DestroyIf(&play->joined[i]);
    for (i = 0; i < ARRAY_COUNT(play->joinDots); i++)
        DestroyIf(&play->joinDots[i]);
}

// Everything freed, and the field given back -- or, when a song has opened something, handed to that script instead
// (it locks what it needs, and the field comes back when it ends: script.c).
static void Teardown(u8 taskId, const u8 *script)
{
    struct Play *play = GetPlay(taskId);
    u8 i;

    if (play->instrument == PLAY_FLUTE)
        m4aMPlayStop(&gMPlayInfo_SE2);   // the HARP's last note rings on; a breath stops with the player
    for (i = 0; i < 12; i++)
        DestroyIf(&play->dots[i]);
    DestroyIf(&play->keyNote);
    for (i = 0; i < STAFF_HEADS; i++)
        DestroyIf(&play->heads[i]);
    for (i = 0; i < DRIFT_MAX; i++)
        DestroyIf(&play->drift[i]);
    ClearJoined(play);
    FreeSpriteTilesByTag(PLAY_TAG_NOTES);
    FreeSpriteTilesByTag(PLAY_TAG_SMALL);
    if (play->borrowed)
    {
        LoadPalette(play->lentColours, OBJ_PLTT_ID(play->palSlot), PLTT_SIZE_4BPP);
    }
    else
    {
        FreeSpritePaletteByTag(PALTAG_KEY_COLOURS);
    }
    CloseStaff(play);
    Free(play);
    DestroyTask(taskId);
    ClearPlayerHeldMovementAndUnfreezeObjectEvents();
    if (script != NULL)
        ScriptContext_SetupScript(script);
    else
        UnlockPlayerFieldControls();
}

static void PutAway(u8 taskId)
{
    Teardown(taskId, NULL);
}

// ---------------------------------------------------------------------------------------------- listening (P2)

static u8 PitchClass(s8 semitone)
{
    return (semitone + 36) % 12;
}

// What the player faces: a singing fir (any grove's, or the pier's -- the object in front runs EventScript_SingingFir,
// and it becomes the one talked to, as A would make it), or THE NEXUS's Christmas tree (map tiles, so its two tiles).
static u8 FacingTarget(void)
{
    s16 x, y;
    u8 objectEventId;

    GetXYCoordsOneStepInFrontOfPlayer(&x, &y);
    if (gSaveBlock1Ptr->location.mapGroup == MAP_GROUP(MAP_NEXUS) && gSaveBlock1Ptr->location.mapNum == MAP_NUM(MAP_NEXUS)
     && GetPlayerFacingDirection() == DIR_NORTH && y - MAP_OFFSET == 4 && (x - MAP_OFFSET == 12 || x - MAP_OFFSET == 13))
        return TARGET_NEXUS_TREE;
    objectEventId = GetObjectEventIdByPosition(x, y, PlayerGetElevation());
    if (objectEventId == OBJECT_EVENTS_COUNT || gObjectEvents[objectEventId].localId == LOCALID_PLAYER)
        return TARGET_NONE;
    if (GetObjectEventScriptPointerByObjectEventId(objectEventId) != EventScript_SingingFir)
        return TARGET_NONE;
    gSelectedObjectEvent = objectEventId;
    gSpecialVar_LastTalked = gObjectEvents[objectEventId].localId;
    gSpecialVar_Facing = GetPlayerFacingDirection();
    return TARGET_FIR;
}

// The matched notes come back and hold under the player (above is the staff), in the order played, higher for higher; between each two, two dots
// -- the first in the colour of the note before, the second in the note after's -- so the line blends as it joins.
static void Join(struct Play *play, u8 song)
{
    const struct PlaySong *tune = &sSongs[song];
    u8 n = tune->count, i, d = 0;
    s16 x0, low = 127, xs[MOTIF_MAX], ys[MOTIF_MAX];

    for (i = 0; i < DRIFT_MAX; i++)
        DestroyIf(&play->drift[i]);
    for (i = 0; i < n; i++)
    {
        play->echo[i] = play->ring[play->ringCount - n + i];
        if (play->echo[i] < low)
            low = play->echo[i];
    }
    x0 = play->cx - (JOIN_GAP * (n - 1)) / 2;
    if (x0 < 12)
        x0 = 12;
    if (x0 + JOIN_GAP * (n - 1) > 228)
        x0 = 228 - JOIN_GAP * (n - 1);
    for (i = 0; i < n; i++)
    {
        xs[i] = x0 + JOIN_GAP * i;
        ys[i] = play->cy + JOIN_BELOW - ((play->echo[i] - low) * 3) / 2;
        play->joined[i] = MakeSprite(play, &sTemplate_Note, xs[i], ys[i], PitchClass(play->echo[i]));
        play->pulse[i] = 0;
    }
    for (i = 0; i + 1 < n; i++)
    {
        play->joinDots[d++] = MakeSprite(play, &sTemplate_Small, xs[i] + JOIN_GAP / 3, ys[i] + (ys[i + 1] - ys[i]) / 3,
                                         PitchClass(play->echo[i]));
        play->joinDots[d++] = MakeSprite(play, &sTemplate_Small, xs[i] + 2 * JOIN_GAP / 3, ys[i] + 2 * (ys[i + 1] - ys[i]) / 3,
                                         PitchClass(play->echo[i + 1]));
    }
    play->heldButton = 0;    // a breath held into the last note lets go: the echo is the song's now
    play->fading = TRUE;
    play->breath = 0;
    play->song = song;
    play->clock = 0;
    play->ringCount = 0;
    play->state = STATE_JOINED;
}

// The echo, a frame at a time: each note on SE2 when its time comes (and its joined note leaps), and again on SE3,
// softer, ECHO_DELAY frames on. TRUE when it has died away.
static bool8 Echo(struct Play *play)
{
    const struct PlaySong *tune = &sSongs[play->song];
    u16 t = 0;
    u8 i;

    for (i = 0; i < tune->count; i++)
    {
        if (play->clock == t)
        {
            Play_SoundNote(play->instrument, play->echo[i], FALSE);
            m4aMPlayVolumeControl(&gMPlayInfo_SE2, TRACKS_ALL, 256);
            play->pulse[i] = 6;
        }
        if (play->clock == t + ECHO_DELAY)
        {
            Play_SoundNote(play->instrument, play->echo[i], TRUE);
            m4aMPlayVolumeControl(&gMPlayInfo_SE3, TRACKS_ALL, ECHO_SOFT);
        }
        t += tune->frames[i];
    }
    for (i = 0; i < tune->count; i++)
    {
        if (play->joined[i] == MAX_SPRITES)
            continue;
        gSprites[play->joined[i]].y2 = play->pulse[i] ? -3 : 0;
        if (play->pulse[i])
            play->pulse[i]--;
    }
    if (play->clock == t + ECHO_DELAY && play->instrument == PLAY_FLUTE)
    {
        m4aMPlayStop(&gMPlayInfo_SE2);   // a breath does not ring on by itself
        m4aMPlayStop(&gMPlayInfo_SE3);
    }
    return ++play->clock > t + ECHO_DELAY + ECHO_TAIL;
}

// After every note: is the end of what has been played a song? In its own key, it is; the right shape in any other
// key (or the right notes, in a key that is not the song's) is ALMOST. A known song: O CHRISTMAS TREE always, the
// seven once learned. TRUE if the task has been handed on.
static bool8 Listen(u8 taskId)
{
    struct Play *play = GetPlay(taskId);
    const struct PlaySong *tune;
    const s8 *tail;
    u8 song, i, shift;
    bool8 same;

    for (song = 0; song < PLAY_SONG_COUNT; song++)
    {
        tune = &sSongs[song];
        if (tune->count == 0 || play->ringCount < tune->count)
            continue;
        if (song != PLAY_SONG_FIR && !FlagGet(tune->flag))
            continue;
        tail = &play->ring[play->ringCount - tune->count];
        shift = (PitchClass(tail[0]) + 12 - tune->pcs[0]) % 12;
        same = TRUE;
        for (i = 1; i < tune->count && same; i++)
            same = ((PitchClass(tail[i]) + 12 - tune->pcs[i]) % 12 == shift);
        if (!same)
            continue;
        play->target = FacingTarget();
        if (shift == 0 && play->key == tune->key)
        {
            Join(play, song);
            return FALSE;
        }
        //  ALMOST: the fir hums its flat version back; anywhere else, a line says so. The instrument goes away for it.
        Teardown(taskId, play->target == TARGET_FIR ? EventScript_Play_FirAlmost : EventScript_Play_Almost);
        return TRUE;
    }
    return FALSE;
}

// The echo has died away: what the song opens, if it was played to something -- or the line lets go, and play goes on.
static bool8 AfterEcho(u8 taskId)
{
    struct Play *play = GetPlay(taskId);

    if (play->song == PLAY_SONG_FIR && play->target == TARGET_FIR)
    {
        Teardown(taskId, EventScript_Play_FirOpened);
        return TRUE;
    }
    if (play->song == PLAY_SONG_FIR && play->target == TARGET_NEXUS_TREE)
    {
        Teardown(taskId, EventScript_Play_NexusLit);
        return TRUE;
    }
    ClearJoined(play);
    play->state = STATE_PLAY;
    return FALSE;
}

// ------------------------------------------------------------------------------------------------------- the task

static void PlayNote(struct Play *play, u8 degree)
{
    u8 i;
    s8 octave = 0;
    s16 semitone;
    u8 pc;

    if (JOY_HELD(L_BUTTON) && !JOY_HELD(R_BUTTON))
        octave = -1;
    else if (JOY_HELD(R_BUTTON) && !JOY_HELD(L_BUTTON))
        octave = 1;
    semitone = sKeyTonic[play->key] + sMajor[degree] + 12 * octave;
    pc = (semitone + 24) % 12;
    if (play->instrument == PLAY_FLUTE)
    {
        Play_SoundNote(PLAY_FLUTE, semitone, FALSE);
        play->heldButton = sNoteButtons[degree];
        play->breath = 0;
        play->fading = FALSE;
    }
    else
    {
        Play_SoundNote(PLAY_HARP, semitone, play->onSe3);
        play->onSe3 ^= 1;
    }
    WriteHead(play, degree, octave, pc);
    SpawnDrift(play, pc);
    if (play->ringCount == RING_MAX)
    {
        for (i = 1; i < RING_MAX; i++)
            play->ring[i - 1] = play->ring[i];
        play->ringCount--;
    }
    play->ring[play->ringCount++] = semitone;
}

static void Task_Play(u8 taskId)
{
    struct Play *play = GetPlay(taskId);
    u8 d;

    switch (play->state)
    {
    case STATE_WAIT:   // the map name popup slides away first: it scrolls BG0, and the staff is on BG0
        if (IsMapNamePopupTaskActive())
            return;
        TakeOut(play);
        play->state = STATE_WHEEL;
        break;
    case STATE_WHEEL:
        if (JOY_NEW(B_BUTTON | START_BUTTON))
        {
            PlaySE(SE_SELECT);
            PutAway(taskId);
            return;
        }
        if (JOY_NEW(A_BUTTON))
        {
            PlaySE(SE_SELECT);
            play->timer = 0;
            play->state = STATE_FOLD;
            break;
        }
        if (JOY_REPT(DPAD_RIGHT) || JOY_REPT(DPAD_LEFT))
        {
            play->key = (play->key + (JOY_REPT(DPAD_RIGHT) ? 1 : 11)) % 12;
            PlaceWheel(play);
            PrintKeyName(play);
            Play_SoundNote(play->instrument, sKeyTonic[play->key], FALSE);   // the key's own note, as it turns
            play->breath = 0;
            play->fading = FALSE;
            play->heldButton = JOY_HELD(DPAD_RIGHT) ? DPAD_RIGHT : DPAD_LEFT;
        }
        break;
    case STATE_FOLD:
        if (FoldWheel(play))
            play->state = STATE_PLAY;
        break;
    case STATE_PLAY:
        if (JOY_NEW(START_BUTTON))
        {
            PutAway(taskId);
            return;
        }
        for (d = 0; d < 7; d++)
        {
            if (JOY_NEW(sNoteButtons[d]))
            {
                PlayNote(play, d);
                if (Listen(taskId))
                    return;
                break;
            }
        }
        break;
    case STATE_JOINED:   // the notes hold, joined, while the song comes back in echo; the buttons wait
        if (Echo(play) && AfterEcho(taskId))
            return;
        break;
    }
    //  A breath ends when its button is let go.
    if (play->instrument == PLAY_FLUTE && play->heldButton && !play->fading && !JOY_HELD(play->heldButton))
    {
        play->fading = TRUE;
        play->breath = play->breath < 5 ? play->breath : 5;
        play->heldButton = 0;
    }
    if (play->instrument == PLAY_FLUTE && play->state != STATE_WAIT && play->state != STATE_JOINED)
        PlayBreath(&gMPlayInfo_SE2, &play->breath, play->fading);
    if (play->state != STATE_WAIT)
        Shine(play);
}

bool8 Play_IsActive(void)
{
    return FuncIsActiveTask(Task_Play);
}

// From the BAG or SELECT (item_use.c): the field is back, so lock it and take the instrument out. Priority 90, after
// the weather's task, so the shine is the last word on the palette each frame.
void Play_Start(void)
{
    struct Play *play;
    u8 taskId;

    ClearPlayerHeldMovementAndUnfreezeObjectEvents();   // people keep walking
    LockPlayerFieldControls();
    QuestLog_CutRecording();   // the Quest Log neither records nor replays a performance
    DismissMapNamePopup();
    play = AllocZeroed(sizeof(*play));
    if (play == NULL)
    {
        UnlockPlayerFieldControls();
        return;
    }
    play->instrument = PLAY_INSTRUMENT;
    play->state = STATE_WAIT;
    play->fading = TRUE;
    taskId = CreateTask(Task_Play, 90);
    SetWordTaskArg(taskId, tPlay, (u32)play);
}

// ------------------------------------------------------------------------------------------------- the SONGBOOK

// DRAFT, every word. Until the key is known (the band's sheet held, or a fir keyed) the page says only what the fir
// hums, and its notes are the plain naturals -- the yellow E and violet B; once it is, they are written with their flats.
static const u8 sSongPage_FirFlat[] = _("Hummed by the singing firs.\pClose to right, all the way through. Something is missing.");
static const u8 sSongPage_FirKeyed[] = _("Hummed by the singing firs.\pIn the key of {STR_VAR_1}.");
static const u8 sSongPage_ToCome[] = _("The melody is still to come.");

static bool8 SongKeyKnown(u8 song)
{
    if (song == PLAY_SONG_FIR)
        return FlagGet(FLAG_NOTEBOOK_LOOSE_KEY) || FlagGet(FLAG_FIR_IN_KEY);
    return TRUE;
}

void Play_WriteSongPage(u8 song, u8 *dest)
{
    if (song >= PLAY_SONG_COUNT || sSongs[song].count == 0)
    {
        StringCopy(dest, sSongPage_ToCome);
        return;
    }
    if (!SongKeyKnown(song))
    {
        StringCopy(dest, sSongPage_FirFlat);
        return;
    }
    StringCopy(gStringVar1, sKeyNames[sSongs[song].key]);
    StringExpandPlaceholders(dest, sSongPage_FirKeyed);
}

// The NOTEBOOK's page has the screen to itself (book_reader.c resets the sprites when it opens and the field when it
// closes), so the heads are drawn as the staff's are: the same 8x8 heads, the same three pixels a line. The key's own
// note stands at the staff's left once the key is known. `middleC` is the y of step 0; book_reader draws the lines.
void Play_BookSong(u8 song, s16 left, s16 middleC)
{
    const struct PlaySong *tune;
    struct Play paint = {0};   // only its palette slot is read
    bool8 known;
    u8 i, pc, spriteId;

    if (song >= PLAY_SONG_COUNT || sSongs[song].count == 0)
        return;
    tune = &sSongs[song];
    known = SongKeyKnown(song);
    LoadSpriteSheet(&sPlaySheets[0]);
    LoadSpriteSheet(&sPlaySheets[1]);
    paint.palSlot = LoadSpritePalette(&sKeyColoursPalette);
    if (paint.palSlot == 0xFF)
        return;
    if (known && (spriteId = MakeSprite(&paint, &sTemplate_Note, left + 8, middleC - 9, tune->key)) != MAX_SPRITES)
        gSprites[spriteId].oam.priority = 0;   // over the page
    for (i = 0; i < tune->count; i++)
    {
        pc = known ? tune->pcs[i] : sNaturalSemi[tune->steps[i] % 7];
        MakeSprite(&paint, &sTemplate_Small, left + 32 + 20 * i, middleC - (tune->steps[i] * 3) / 2, 12 + pc);
    }
}

void Play_BookSongFree(void)
{
    FreeSpriteTilesByTag(PLAY_TAG_NOTES);
    FreeSpriteTilesByTag(PLAY_TAG_SMALL);
    FreeSpritePaletteByTag(PALTAG_KEY_COLOURS);
}

// -------------------------------------------------------------------------------------------------------- DEBUG

#if DAEMONS_DEBUG
// DEBUG -> SOUND -> HARP / FLUTE: C major up through three octaves of bend, an octave down to an octave up, so the
// bend can be heard. The FLUTE holds each note for sixteen frames and lets it go, as a player would.
#define tInstrument data[2]
#define tNote       data[3]
#define tTimer      data[4]
#define tBreath     data[5]

static void Task_PlayScale(u8 taskId)
{
    s16 *data = gTasks[taskId].data;
    u8 breath = tBreath;

    if (tInstrument == PLAY_FLUTE)
    {
        PlayBreath(&gMPlayInfo_SE2, &breath, tTimer < 6);
        tBreath = breath;
    }
    if (tTimer != 0)
    {
        tTimer--;
        return;
    }
    if (tNote >= 22)
    {
        if (tInstrument == PLAY_FLUTE)
            m4aMPlayStop(&gMPlayInfo_SE2);
        DestroyTask(taskId);
        return;
    }
    Play_SoundNote(tInstrument, sMajor[tNote % 7] + 12 * (tNote / 7 - 1), tInstrument == PLAY_HARP && (tNote & 1));
    tBreath = 0;
    tNote++;
    tTimer = 22;
}

void Play_DebugScale(u8 instrument)
{
    u8 taskId;

    if (FuncIsActiveTask(Task_PlayScale))
        return;
    taskId = CreateTask(Task_PlayScale, 80);
    gTasks[taskId].tInstrument = instrument;
}

#undef tInstrument
#undef tNote
#undef tTimer
#undef tBreath
#endif
