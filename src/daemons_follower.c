#include "global.h"
#include "daemons_hours.h"   // T-519
#include "main.h"
#include "daemons_spirits.h"   // T-519: its tone
#include "palette.h"
#include "daemons_witching.h"   // T-480
#include "daemons_hermetic.h"   // T-483: Halloween's costume
#include "data.h"               // T-483: gSpeciesNames
#include "gflib.h"
#include "event_data.h"
#include "event_object_movement.h"
#include "field_effect.h"
#include "field_effect_helpers.h"
#include "field_camera.h"
#include "metatile_behavior.h"
#include "constants/field_effects.h"
#include "constants/event_objects.h"
#include "field_weather.h"
#include "overworld.h"
#include "pokemon.h"
#include "pokemon_icon.h"
#include "quest_log.h"
#include "script.h"
#include "string_util.h"
#include "field_player_avatar.h"
#include "event_scripts.h"
#include "fieldmap.h"
#include "field_specials.h"
#include "trainer_see.h"
#include "daemons_time.h"
#include "daemons_weather.h"
#include "constants/event_bg.h"
#include "constants/region_map_sections.h"
#include "daemon_saturation.h"
#include "daemons_follower.h"
#include "constants/event_object_movement.h"
#include "constants/quest_log.h"
#include "data/follower_palettes.h"
#include "data/follower_gfx.h"
#include "daemons_feelings.h"
#include "reveal.h"
#include "daemons_reads.h"   // T-577: the pond's read
#include "constants/maps.h"
#include "constants/metatile_behaviors.h"
#include "daemon_song.h"        // T-596: the wish to sing
#include "play.h"               // T-596: PLAY's own note colours, and not over PLAY
#include "random.h"
#include "task.h"
#include "field_message_box.h"
#include "sound.h"

//  T-18 P1: THE FOLLOWER -- it walks and hides (the user decided the plan 2026-10-20; every rule below is the plan's).
//
//  WHAT IT IS. The first party daemon that is not an egg, not HALTED (0 HP) and not AWAY, drawn at 1x one tile behind
//  the player as its FOLLOWER PICTURE: one 32x32 frame, the creature at most 20 tall and 22 wide on row 28
//  (src/data/follower_gfx.h, DAEMONS tools/gbafollower.py -- T-464: the party icons became 28px busts, and the
//  follower walked larger than the player's 16x22). A species with no picture there has an icon that already fits,
//  and walks as its icon's first frame. Its colours are its
//  TYPE's (src/data/follower_palettes.h, DAEMONS tools/gbafollowerpal.py: the icon's shared palette remapped to the
//  nearest colours of its front palette, so invariant 5 holds), greyed until sung as its body is (T-597), lighter or
//  darker by its SPIRITS (T-519, as the summary draws it), then put
//  in the field's light and weather as every person is. Icons face left; facing right is the same frame flipped. A
//  1 px hop in the middle of each tile it walks is its step (387 of 396 icons have two identical frames, so the
//  picture is one).
//
//  HOW IT FOLLOWS. Not an object event: one SPRITE, which takes no object slot (so no map is too full for it), blocks
//  nothing and nobody (so the player, a trainer walking up or a scripted path never stops at it), is never saved and
//  is never recorded by the quest log. It replays the player's own path 16 pixels late: every pixel the player's
//  sprite moves is pushed into a ring of sixteen, and the follower stands where the player stood sixteen pixels ago.
//  So it steps onto the tile the player left at the player's own speed -- walking, running, on the bike, jumping a
//  ledge (the jump's height is recorded with each pixel), along the stairs -- and turning back into it swaps the two
//  over. The player's sprite moves in WORLD pixels, which a connection seam does not shift, so it keeps across seams
//  with nothing rewritten. A jump of more than two tiles in one frame (a warp, a script putting the player somewhere)
//  SNAPS it: back onto the player's tile, hidden until the player has walked a tile out of it. A ledge is the one
//  exception to "a tile behind": it waits at the top while the player lands, and jumps it on the player's next step.
//
//  WHEN IT HIDES (and snaps, so it walks out again after): surfing; forced movement (ice, spinners, currents); a cave
//  still dark for want of FLASH; a door or warp (the player's own sprite hidden); a script moving the player -- a cut
//  scene, a door's walk in -- until the script lets go. It is not there at all with the OPTIONS row off, with no
//  daemon to follow, in the link rooms (group 0: the colosseums, the trade centre, the record corner, the union room),
//  during the Continue recap, or when no OBJ palette slot (12-15) or sprite is free -- never painted into another's.
//
//  WHERE IT COMES BACK. Every field entry tears the sprites down; the next frame finds its sprite gone and makes it
//  again. If the player is on the same map and tile it left (a battle, a menu), it stands where it stood; otherwise
//  (a warp) it starts on the player's tile. State is in IWRAM and nothing of it is saved.

#define FOLLOWER_TAG     0x5418   // its tiles and its palette (T-18)
#define EMOJI_TAG        0x5419   // the daemon-emoji's palette, only while it shows (T-431 P3)
#define SENSE_TAG        0x541A   // the subtle layer's ripple and glint: their palette, only while they show (T-459)
#define HUM_TAG          0x541B   // T-596: the notes it hums -- PLAY's key colours, only while they show
#define HUM_FIRST        300      // T-596: after it is made (a map, a battle, a menu), five seconds before it may hum
#define TRAIL            16       // pixels behind the player: one tile
#define FOLLOWER_Y       2        // its picture's lowest row is row 28 (gbafollower.py); this puts it on the player's feet
#define MAX_JUMP         32       // a move bigger than this in one frame is a warp, not a step
#define RETRY_FRAMES     30       // after a failed spawn (no palette slot, no sprite), try again this often
#define CHECK_FRAMES     8        // the lead and its friendship are read this often
#define SENSE_FIRST      180      // T-459: after it is made (a map, a battle, a menu), three seconds before it may notice
#define SENSE_STILL      45       // ... and it notices only once it has stood still this long
#define SENSE_AGAIN      1200     // ... and then not again for twenty seconds: now and then, never a beacon
#define SENSE_LOOK       60       // with nothing unfound in sight, it looks again a second later
#define TURN_BEAT        50       // T-580: back from the LAST WORD, it looks away this long, then turns to the player

static struct
{
    u8 ringDir[TRAIL];     // each pixel the player moved: DIR_NONE (a snap's filler) or DIR_SOUTH..DIR_EAST
    s8 ringY2[TRAIL];      // ... and the player's jump height when it moved it (two arrays: agbcc pads a struct to 4)
    u8 head;               // the oldest step: the next one the follower takes
    u8 spriteId;
    u8 playerSpriteId;
    u8 retry;
    u8 check;
    u8 facing;
    u8 idle;               // frames since it last moved
    u8 friendship;
    u16 songKeep;          // T-597: its colour, by its song (Saturation_MonKeep): its body's, over the remapped palette
    bool8 scriptHidden;
    bool8 restValid;
    s8 y2;
    s8 restDX, restDY;     // where it stood, in tiles from the player, when it last stood still
    u8 restGroup, restNum;
    u16 species;           // the icon species shown
    u16 steps;             // pixels it has walked
    s16 sumX, sumY;        // the player minus the follower, in pixels: the ring's sum
    s16 lastX, lastY;      // the player's sprite, last frame
    s16 restX, restY;      // the player's tile when it was stored
    u32 personality;
    bool8 half;            // in the air, a pixel waits for its pair (see Step)
    u8 talks;              // T-18 P3: presses since it last moved -- even, its friendship; odd, a mood
    u8 moodTurn;           // T-18 P2: a mood's two lines take turns
    bool8 shiny;           // T-18 P3: the lead is shiny -- its shiny colours
    bool8 sceneHidden;     // T-18 P3: hidefollower, until showfollower or the script lets go
    bool8 continued;       // T-18 P3: the game was just continued -- stand behind the player, not inside them
    u8 reflSpriteId;       // T-18 P3: its reflection, while it stands above water or ice
    s16 tileX, tileY;      // T-18 P3: the map tile it stands in (nearest), for grass, water and the people
    s16 grassX, grassY;    // ... and the tile its last tall grass was raised on
    u16 senseWait;         // T-459: frames until it may notice something again
    u8 shimmer;            // T-480: the witching hour's shimmer is on its palette (put back when the hour ends)
    u16 costume;           // T-483: Halloween, with THE HERMETIC LAYER held -- the species it goes as, or SPECIES_NONE
    u16 shown;             // T-483: the icon species whose picture and colours it wears (its own, or the costume's)
    u8 tone;               // T-519: its SPIRITS' tone, lighter or darker over its colours (Spirits_ToneOf)
    bool8 turnPending;     // T-580: THE SUBTLE LAYER's LAST WORD was read -- it will turn to face the player
    u8 turnWait;           // ... frames still looking away (0: the turn has not begun)
    u16 pondStill;         // T-577: frames the two have stood still at a still pond's edge
    u16 pondMap;           // ... the map they are on, + 1, and whether its rings came this arrival
    bool8 pondDone;
    u8 pondRings;          // ... rings still spreading (2, 1, 0)
    u8 wish;               // T-596: how much the lead wishes to sing (DaemonSong_Wish), read with its friendship
    bool8 humTicked;       // ... a hum was allowed to go on last frame (if not, it is stopped)
    bool8 singing;         // ... it sings because it was asked (after a battle): its notes rise too
    u16 humWait;           // ... frames until it may hum again
    u16 askWait;           // ... after a battle it may ask, within this many frames
    u8 askBeat;            // ... frames the player has been free since, standing
    u16 heardSerial;       // ... the note last drawn
    s8 mouthX, mouthY;     // ... its mouth in its picture (facing left), from the picture's centre
} sFollower;

static void Shimmer(void);   // T-480
static void HumTidy(void);   // T-596
static void Hum(struct Sprite *sprite);
static void AskToSing(struct Sprite *sprite);

static void SpriteCB_Follower(struct Sprite *sprite)
{
}

static const struct OamData sOam_Follower = {
    .y = 0,
    .affineMode = ST_OAM_AFFINE_OFF,
    .objMode = ST_OAM_OBJ_NORMAL,
    .bpp = ST_OAM_4BPP,
    .shape = SPRITE_SHAPE(32x32),
    .size = SPRITE_SIZE(32x32),
    .priority = 2,
};

static const struct SpriteTemplate sSpriteTemplate_Follower = {
    .tileTag = FOLLOWER_TAG,
    .paletteTag = FOLLOWER_TAG,
    .oam = &sOam_Follower,
    .anims = gDummySpriteAnimTable,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCB_Follower,
};

#define STEP_DIR     7
#define STEP_DOUBLE  8          // a ring entry two pixels long: the player was in the air (a ledge)

static const s8 sDirX[] = { [DIR_NONE] = 0, [DIR_SOUTH] = 0, [DIR_NORTH] = 0, [DIR_WEST] = -1, [DIR_EAST] = 1 };
static const s8 sDirY[] = { [DIR_NONE] = 0, [DIR_SOUTH] = 1, [DIR_NORTH] = -1, [DIR_WEST] = 0, [DIR_EAST] = 0 };

//  ---------------------------------------------------------------- who follows

s8 Follower_LeadSlot(void)
{
    s32 i;

    for (i = 0; i < PARTY_SIZE; i++)
    {
        struct Pokemon *mon = &gPlayerParty[i];

        if (GetMonData(mon, MON_DATA_SPECIES) == SPECIES_NONE)
            break;
        if (GetMonData(mon, MON_DATA_IS_EGG) || GetMonData(mon, MON_DATA_HP) == 0 || DaemonIsAway(mon))
            continue;
        return i;
    }
    return -1;
}

static bool8 InLinkRoom(void)
{
    // MAP_BATTLE_COLOSSEUM_2P, MAP_TRADE_CENTER, MAP_RECORD_CORNER, MAP_BATTLE_COLOSSEUM_4P, MAP_UNION_ROOM
    return gSaveBlock1Ptr->location.mapGroup == 0 && gSaveBlock1Ptr->location.mapNum <= 4;
}

//  Not there at all: its sprite and palette slot given back. (Whether anyone can follow is asked only every
//  CHECK_FRAMES, below: reading the party decrypts it, which is not a thing to do sixty times a second.)
static bool8 FollowerIsAbsent(void)
{
    return FlagGet(FLAG_FOLLOWER_OFF) || QL_IS_PLAYBACK_STATE || InLinkRoom();
}

//  There, but hidden this frame (and snapped, so it walks out again after).
static bool8 FollowerIsHidden(struct Sprite *player)
{
    if (gPlayerAvatar.flags & (PLAYER_AVATAR_FLAG_SURFING | PLAYER_AVATAR_FLAG_UNDERWATER | PLAYER_AVATAR_FLAG_FORCED))
        return TRUE;
    if (gMapHeader.cave && gSaveBlock1Ptr->flashLevel != 0)
        return TRUE;
    if (player->invisible || sFollower.scriptHidden)
        return TRUE;
    return FALSE;
}

//  ---------------------------------------------------------------- the path

static void Snap(void)
{
    s32 i;

    for (i = 0; i < TRAIL; i++)
    {
        sFollower.ringDir[i] = DIR_NONE;
        sFollower.ringY2[i] = 0;
    }
    sFollower.head = 0;
    sFollower.sumX = sFollower.sumY = 0;
    sFollower.steps = 0;
    sFollower.y2 = 0;
    sFollower.half = FALSE;
    sFollower.restValid = FALSE;
}

//  A full tile behind, as if the player had just walked out of it (after a battle or a menu).
static void StandAt(s8 dx, s8 dy)
{
    s32 i;
    u8 dir = dx < 0 ? DIR_EAST : dx > 0 ? DIR_WEST : dy < 0 ? DIR_SOUTH : DIR_NORTH;

    Snap();
    for (i = 0; i < TRAIL; i++)
        sFollower.ringDir[i] = dir;
    sFollower.sumX = -dx * TRAIL;
    sFollower.sumY = -dy * TRAIL;
    sFollower.facing = dir;
}

static void Push(u8 dir, s8 y2)
{
    u8 old = sFollower.ringDir[sFollower.head];
    s32 now = dir & STEP_DOUBLE ? 2 : 1, then = old & STEP_DOUBLE ? 2 : 1;

    if (old != DIR_NONE)
    {
        sFollower.facing = old & STEP_DIR;
        sFollower.steps += then;
        sFollower.idle = 0;
        sFollower.talks = 0;
    }
    sFollower.y2 = sFollower.ringY2[sFollower.head];
    sFollower.sumX += sDirX[dir & STEP_DIR] * now - sDirX[old & STEP_DIR] * then;
    sFollower.sumY += sDirY[dir & STEP_DIR] * now - sDirY[old & STEP_DIR] * then;
    sFollower.ringDir[sFollower.head] = dir;
    sFollower.ringY2[sFollower.head] = y2;
    sFollower.head = (sFollower.head + 1) % TRAIL;
}

//  One pixel of the player's path. In the air (a ledge: 32 pixels, its height in y2) two pixels make one entry, so
//  the jump fills half the ring: when the player lands, the follower has just reached the tile before the ledge (two
//  tiles behind, on the ground), and the player's next step carries it over the ledge in one jump of its own --
//  rather than leaving it hanging at the top of the player's arc, over the ledge tile.
static void Step(u8 dir, s8 y2, bool8 jumping)
{
    if (jumping)
    {
        if (sFollower.half)
            Push(dir | STEP_DOUBLE, y2);
        sFollower.half = !sFollower.half;
        return;
    }
    if (sFollower.half)
        Push(dir, 0);   // a jump's odd last pixel
    sFollower.half = FALSE;
    Push(dir, 0);
}

static void Track(struct Sprite *player, struct ObjectEvent *playerObj)
{
    s32 dx = player->x - sFollower.lastX, dy = player->y - sFollower.lastY;
    // a ledge's whole jump, its first and last pixels too (they are drawn at height 0)
    bool8 jumping = playerObj->movementActionId >= MOVEMENT_ACTION_JUMP_2_DOWN
                 && playerObj->movementActionId <= MOVEMENT_ACTION_JUMP_2_RIGHT;

    if (sFollower.idle < 255)
        sFollower.idle++;
    sFollower.lastX = player->x;
    sFollower.lastY = player->y;
    if (dx > MAX_JUMP || dx < -MAX_JUMP || dy > MAX_JUMP || dy < -MAX_JUMP)
    {
        Snap();
        return;
    }
    if ((dx || dy) && ArePlayerFieldControlsLocked())
        sFollower.scriptHidden = TRUE;   // a script moved the player: a cut scene, a door
    for (; dx > 0; dx--) Step(DIR_EAST, player->y2, jumping);
    for (; dx < 0; dx++) Step(DIR_WEST, player->y2, jumping);
    for (; dy > 0; dy--) Step(DIR_SOUTH, player->y2, jumping);
    for (; dy < 0; dy++) Step(DIR_NORTH, player->y2, jumping);
}

//  ---------------------------------------------------------------- its colours and its picture

static void BuildPalette(u16 *buf)
{
    u16 species = sFollower.shown < ARRAY_COUNT(sFollowerPalettes) ? sFollower.shown : SPECIES_NONE;   // T-483
    const u16 *pal = sFollowerPalettes[species];
    u32 i;

    // T-18 P3: a shiny lead follows in its shiny colours (the user, 2026-10-20) -- the same remap, from shiny.pal.
    // The shiny set holds only the species whose shiny colours differ; every other shiny looks as its normal self.
    if (sFollower.shiny)
        for (i = 0; i < ARRAY_COUNT(sFollowerShinySpecies); i++)
            if (sFollowerShinySpecies[i] == species)
                pal = sFollowerShinyPalettes[i];
    CpuCopy16(pal, buf, PLTT_SIZE_4BPP);
    Saturation_KeepToBuffer(buf, sFollower.songKeep);      // T-424, T-597: greyed until sung, as its body is
    Spirits_ToneBuffer(buf, sFollower.tone);               // T-519: then lighter or darker by its spirits, as the
                                                           // summary draws it; the hours' gold and shimmer go over this
}

static u8 LoadFollowerPalette(void)
{
    u16 buf[16];
    u8 idx = IndexOfSpritePaletteTag(FOLLOWER_TAG);

    if (idx == 0xFF)
        idx = AllocSpritePalette(FOLLOWER_TAG);   // 12-15 only: the twelve below are the objects'
    if (idx == 0xFF)
        return 0xFF;
    BuildPalette(buf);
    LoadPalette(buf, OBJ_PLTT_ID(idx), PLTT_SIZE_4BPP);
    ApplyGlobalFieldPaletteTint(idx);         // the watch's light, HALFTONE's grey
    UpdateSpritePaletteWithWeather(idx);      // and the weather, as PatchObjectPalette's callers do
    return idx;
}

static bool8 IsOurs(void)
{
    struct Sprite *sprite = &gSprites[sFollower.spriteId];

    return sFollower.spriteId < MAX_SPRITES && sprite->inUse && sprite->callback == SpriteCB_Follower;
}

static void SpriteCB_FollowerReflection(struct Sprite *sprite)
{
}

static bool8 ReflectionIsOurs(void)
{
    struct Sprite *sprite = &gSprites[sFollower.reflSpriteId];

    return sFollower.reflSpriteId < MAX_SPRITES && sprite->inUse && sprite->callback == SpriteCB_FollowerReflection;
}

static void DropReflection(void)
{
    if (ReflectionIsOurs())
        DestroySprite(&gSprites[sFollower.reflSpriteId]);   // it shares the follower's tiles and palette: nothing to free
    sFollower.reflSpriteId = MAX_SPRITES;
}

static void Despawn(void)
{
    DropReflection();
    if (IsOurs())
        DestroySprite(&gSprites[sFollower.spriteId]);
    sFollower.spriteId = MAX_SPRITES;
    FreeSpriteTilesByTag(FOLLOWER_TAG);
    FreeSpritePaletteByTag(FOLLOWER_TAG);
}

static void ReadLead(s8 slot, u16 *species, u32 *personality, u8 *friendship, bool8 *shiny)
{
    struct Pokemon *mon = &gPlayerParty[slot];

    *personality = GetMonData(mon, MON_DATA_PERSONALITY);
    *species = GetIconSpecies(GetMonData(mon, MON_DATA_SPECIES), *personality);
    *friendship = GetMonData(mon, MON_DATA_FRIENDSHIP);
    *shiny = IsMonShiny(mon);
}

//  T-18 P3: after Continue it stands a tile behind the player, as if they had just walked out of it -- when that tile
//  is one the player could walk onto (no wall, no water, no ledge, nobody standing there); else it walks out of them.
static bool8 StandBehind(struct ObjectEvent *playerObj)
{
    u8 facing = playerObj->facingDirection;
    u8 behavior;
    s16 x, y;

    if (facing < DIR_SOUTH || facing > DIR_EAST)
        return FALSE;
    x = playerObj->currentCoords.x - sDirX[facing];
    y = playerObj->currentCoords.y - sDirY[facing];
    if (GetCollisionAtCoords(playerObj, x, y, GetOppositeDirection(facing)) != COLLISION_NONE)
        return FALSE;
    behavior = MapGridGetMetatileBehaviorAt(x, y);
    if (MetatileBehavior_IsSurfable(behavior) || MetatileBehavior_IsForcedMovementTile(behavior)
     || MetatileBehavior_IsJumpEast(behavior) || MetatileBehavior_IsJumpWest(behavior)
     || MetatileBehavior_IsJumpNorth(behavior) || MetatileBehavior_IsJumpSouth(behavior))
        return FALSE;
    StandAt(-sDirX[facing], -sDirY[facing]);
    return TRUE;
}

// T-483: Halloween's costume for the daemon in SLOT (src/daemons_hermetic.c), or SPECIES_NONE.
static u16 CostumeFor(s8 slot)
{
    return Hermetic_Costume(GetMonData(&gPlayerParty[slot], MON_DATA_SPECIES));
}

// T-464: its own picture when its icon is too big to walk beside the player, else its icon's first frame.
static const u8 *FollowerPic(u16 species)
{
    if (species < ARRAY_COUNT(sFollowerPics) && sFollowerPics[species] != NULL)
        return sFollowerPics[species];
    return GetMonIconTiles(species, FALSE);
}

static void FindMouth(const u8 *pic, s8 *mouthX, s8 *mouthY);   // T-596 (T-598: and the summoned)

static void Spawn(s8 slot, struct Sprite *player, struct ObjectEvent *playerObj)
{
    struct SpriteSheet sheet;
    u8 idx;

    ReadLead(slot, &sFollower.species, &sFollower.personality, &sFollower.friendship, &sFollower.shiny);
    sFollower.tone = Spirits_ToneOf(&gPlayerParty[slot].box);   // T-519
    sFollower.songKeep = Saturation_MonKeep(&gPlayerParty[slot]);   // T-597
    sFollower.costume = CostumeFor(slot);   // T-483: in a costume it wears the other daemon's picture and colours
    sFollower.shown = sFollower.costume != SPECIES_NONE ? GetIconSpecies(sFollower.costume, 0) : sFollower.species;
    FreeSpriteTilesByTag(FOLLOWER_TAG);
    sheet.data = FollowerPic(sFollower.shown);
    sheet.size = 0x200;   // one 32x32 frame
    sheet.tag = FOLLOWER_TAG;
    LoadSpriteSheet(&sheet);
    idx = LoadFollowerPalette();
    if (GetSpriteTileStartByTag(FOLLOWER_TAG) == 0xFFFF || idx == 0xFF)
    {
        Despawn();
        sFollower.retry = RETRY_FRAMES;
        return;
    }
    sFollower.spriteId = CreateSprite(&sSpriteTemplate_Follower, player->x, player->y, 0);
    if (sFollower.spriteId == MAX_SPRITES)
    {
        Despawn();
        sFollower.retry = RETRY_FRAMES;
        return;
    }
    gSprites[sFollower.spriteId].coordOffsetEnabled = TRUE;
    gSprites[sFollower.spriteId].animPaused = TRUE;
    gSprites[sFollower.spriteId].invisible = TRUE;
    gSprites[sFollower.spriteId].oam.paletteNum = idx;

    // Back where it stood after a battle or a menu; on the player's tile after a warp.
    sFollower.playerSpriteId = gPlayerAvatar.spriteId;
    sFollower.lastX = player->x;
    sFollower.lastY = player->y;
    sFollower.scriptHidden = FALSE;
    sFollower.check = CHECK_FRAMES;
    sFollower.reflSpriteId = MAX_SPRITES;
    sFollower.grassX = sFollower.grassY = -1;
    sFollower.pondRings = 0;             // T-577: a menu or a battle took its rings down
    sFollower.senseWait = SENSE_FIRST;   // T-459: a moment in a new place (or back from a battle) before it notices
    FindMouth(sheet.data, &sFollower.mouthX, &sFollower.mouthY);   // T-596
    sFollower.wish = DaemonSong_Wish(&gPlayerParty[slot]);
    if (sFollower.humWait < HUM_FIRST)
        sFollower.humWait = HUM_FIRST;   // and before it hums
    if (sFollower.restValid
     && sFollower.restGroup == gSaveBlock1Ptr->location.mapGroup && sFollower.restNum == gSaveBlock1Ptr->location.mapNum
     && sFollower.restX == playerObj->currentCoords.x && sFollower.restY == playerObj->currentCoords.y)
        StandAt(sFollower.restDX, sFollower.restDY);
    else
        Snap();   // after Continue, Follower_Update stands it behind the player once it may be drawn
}

//  The lead may have changed (a swap in the party, a HALT in battle) or its friendship moved.
static void CheckLead(s8 slot)
{
    u16 species;
    u32 personality;
    u8 friendship;
    u8 idx;
    bool8 shiny;

    // A swap in the party, a trade, an evolution, a HALT in battle, a daemon gone AWAY: another picture
    ReadLead(slot, &species, &personality, &friendship, &shiny);
    if (species != sFollower.species || personality != sFollower.personality || shiny != sFollower.shiny
     || CostumeFor(slot) != sFollower.costume)   // T-483: the mask put on (Halloween came) or taken off
    {
        Despawn();   // made again next frame, from the same path
        return;
    }
    sFollower.wish = DaemonSong_Wish(&gPlayerParty[slot]);   // T-596: it learned in a battle, or was given experience
    if (friendship != sFollower.friendship || Spirits_ToneOf(&gPlayerParty[slot].box) != sFollower.tone   // T-519
     || Saturation_MonKeep(&gPlayerParty[slot]) != sFollower.songKeep)                                 // T-597
    {
        sFollower.friendship = friendship;
        sFollower.songKeep = Saturation_MonKeep(&gPlayerParty[slot]);
        sFollower.tone = Spirits_ToneOf(&gPlayerParty[slot].box);
        idx = LoadFollowerPalette();
        if (idx != 0xFF)
            gSprites[sFollower.spriteId].oam.paletteNum = idx;
    }
}

//  T-430: its palette slot, or 0xFF -- HALFTONE's grey leaves the follower in colour (DaemonsGreyHalftoneFrame).
u8 Follower_PaletteSlot(void)
{
    return IndexOfSpritePaletteTag(FOLLOWER_TAG);
}

//  T-431 P3: the daemon-emoji's slot while it shows, or 0xFF -- it keeps its colour in HALFTONE as the follower does.
u8 Follower_EmojiPaletteSlot(void)
{
    return IndexOfSpritePaletteTag(EMOJI_TAG);
}

//  T-459: the ripple's and the glint's slot while they show, or 0xFF -- the follower's noticing keeps its colour too.
u8 Follower_SensePaletteSlot(void)
{
    return IndexOfSpritePaletteTag(SENSE_TAG);
}

static u8 LoadEmojiPalette(void);
static u8 LoadSensePalette(void);

void Follower_RetintPalette(void)
{
    if (IsOurs())
        LoadFollowerPalette();
    if (Follower_EmojiPaletteSlot() != 0xFF)
        LoadEmojiPalette();
    if (Follower_SensePaletteSlot() != 0xFF)
        LoadSensePalette();
}

//  ---------------------------------------------------------------- what it stands on (T-18 P3)

//  Only to mark its tall grass apart from the people's (it is not an object event). Under 128: the grass keeps
//  localId << 8 in an s16 and reads it back with >> 8, so 254 would come back as -2 and never be found.
#define LOCALID_FOLLOWER 126

//  The map tile it stands in, from where its sprite is: the inverse of SetSpritePosToMapCoords (an object standing on
//  (x, y) is drawn at that position + 8 across, + 0 down for a 32-high sprite). Rounded to the NEAREST tile, so a pixel
//  of camera lag never moves it, and so it is in a tile once it is halfway into it.
static void FindTile(s16 baseX, s16 baseY)
{
    s16 ox, oy, rx, ry;

    SetSpritePosToMapCoords(gSaveBlock1Ptr->pos.x, gSaveBlock1Ptr->pos.y, &ox, &oy);
    rx = baseX - 8 - ox + 8;
    ry = baseY - oy + 8;
    sFollower.tileX = gSaveBlock1Ptr->pos.x + (rx >= 0 ? rx / 16 : -((15 - rx) / 16));
    sFollower.tileY = gSaveBlock1Ptr->pos.y + (ry >= 0 ? ry / 16 : -((15 - ry) / 16));
}

//  Somebody else stands in (or is walking through) its tile: a person a script walks past it, a trainer walking up to
//  the player, someone who wandered into the tile behind the player. It is not an object, so nothing stopped them --
//  it gives way by not being drawn until they have gone (the plan's "hide", not "step aside").
static bool8 SomeoneInItsTile(void)
{
    s32 i;

    for (i = 0; i < OBJECT_EVENTS_COUNT; i++)
    {
        struct ObjectEvent *obj = &gObjectEvents[i];

        if (!obj->active || obj->isPlayer || obj->invisible || obj->localId == LOCALID_CAMERA)
            continue;
        if ((obj->currentCoords.x == sFollower.tileX && obj->currentCoords.y == sFollower.tileY)
         || (obj->previousCoords.x == sFollower.tileX && obj->previousCoords.y == sFollower.tileY))
            return TRUE;
    }
    return FALSE;
}

//  Tall grass over its feet: the people's own field effect (the rustle as it steps in, then the blades in front of
//  it), its sprite kept for as long as the follower stands in the tile. The grass is 16 wide and the icon's body
//  about 20, so a pixel or two of it shows either side -- as it does for a 16-wide person, near enough.
static void SpriteCB_FollowerGrass(struct Sprite *sprite)
{
    u8 mapNum = sprite->data[5] >> 8, mapGroup = sprite->data[5];
    struct Sprite *follower;

    // across a seam the map's coordinates shift under it, as UpdateTallGrassFieldEffect allows for
    if (gCamera.active && (gSaveBlock1Ptr->location.mapNum != mapNum || gSaveBlock1Ptr->location.mapGroup != mapGroup))
    {
        sprite->data[1] -= gCamera.x;
        sprite->data[2] -= gCamera.y;
        sprite->data[5] = ((u8)gSaveBlock1Ptr->location.mapNum << 8) | (u8)gSaveBlock1Ptr->location.mapGroup;
    }
    if (!IsOurs() || gSprites[sFollower.spriteId].invisible
     || sFollower.tileX != sprite->data[1] || sFollower.tileY != sprite->data[2])
        sprite->data[7] = TRUE;   // it has gone: finish the rustle, then go
    if (!MetatileBehavior_IsTallGrass(MapGridGetMetatileBehaviorAt(sprite->data[1], sprite->data[2]))
     || (sprite->data[7] && sprite->animEnded))
    {
        FieldEffectStop(sprite, FLDEFF_TALL_GRASS);
        return;
    }
    UpdateObjectEventSpriteInvisibility(sprite, FALSE);
    if (IsOurs())
    {
        follower = &gSprites[sFollower.spriteId];
        sprite->oam.priority = follower->oam.priority;
        sprite->subpriority = follower->subpriority ? follower->subpriority - 1 : 0;   // in front of it
    }
}

static void RaiseGrass(struct ObjectEvent *playerObj, struct Sprite *follower, bool8 moving)
{
    u8 spriteId;

    if (sFollower.tileX == sFollower.grassX && sFollower.tileY == sFollower.grassY)
        return;
    sFollower.grassX = sFollower.tileX;
    sFollower.grassY = sFollower.tileY;
    if (!MetatileBehavior_IsTallGrass(MapGridGetMetatileBehaviorAt(sFollower.tileX, sFollower.tileY)))
        return;
    gFieldEffectArguments[0] = sFollower.tileX;
    gFieldEffectArguments[1] = sFollower.tileY;
    gFieldEffectArguments[2] = playerObj->previousElevation;
    gFieldEffectArguments[3] = follower->oam.priority;
    gFieldEffectArguments[4] = LOCALID_FOLLOWER << 8 | gSaveBlock1Ptr->location.mapNum;
    gFieldEffectArguments[5] = gSaveBlock1Ptr->location.mapGroup;
    gFieldEffectArguments[6] = (u8)gSaveBlock1Ptr->location.mapNum << 8 | (u8)gSaveBlock1Ptr->location.mapGroup;
    gFieldEffectArguments[7] = !moving;   // standing there already (back from a battle): no rustle
    FieldEffectStart(FLDEFF_TALL_GRASS);
    spriteId = FindTallGrassFieldEffectSpriteId(LOCALID_FOLLOWER, gSaveBlock1Ptr->location.mapNum,
                                                gSaveBlock1Ptr->location.mapGroup, sFollower.tileX, sFollower.tileY);
    if (spriteId != MAX_SPRITES)
        gSprites[spriteId].callback = SpriteCB_FollowerGrass;
    else
        FieldEffectActiveListRemove(FLDEFF_TALL_GRASS);   // no sprite was free: the grass is simply not drawn
}

//  Above still water or ice it is reflected, as the people are: the same tiles and the same palette (a person's
//  reflection in FRLG is drawn in their own palette too), upside down and behind the ground. Its own sprite, made
//  only while it is needed; if none is free there is no reflection, and nothing else changes.
static void Reflect(struct Sprite *follower)
{
    struct Sprite *refl;

    if (follower->invisible
     || Witching_ShimmerNow()   // T-480: in the witching hour it casts no reflection (T-483: all Halloween, with the book)
     || !MetatileBehavior_IsReflective(MapGridGetMetatileBehaviorAt(sFollower.tileX, sFollower.tileY + 1)))
    {
        DropReflection();
        return;
    }
    if (!ReflectionIsOurs())
    {
        sFollower.reflSpriteId = CreateSprite(&sSpriteTemplate_Follower, follower->x, follower->y, 0xFF);
        if (sFollower.reflSpriteId == MAX_SPRITES)
            return;
        refl = &gSprites[sFollower.reflSpriteId];
        refl->callback = SpriteCB_FollowerReflection;
        refl->coordOffsetEnabled = TRUE;
        refl->animPaused = TRUE;
        refl->oam.priority = 3;
    }
    refl = &gSprites[sFollower.reflSpriteId];
    refl->oam.paletteNum = follower->oam.paletteNum;
    refl->oam.tileNum = follower->oam.tileNum;
    refl->oam.matrixNum = follower->oam.matrixNum | ST_OAM_VFLIP;
    refl->x = follower->x;
    refl->y = follower->y + 26;   // mirrored about its feet: the body ends three rows above the cell's foot
    refl->y2 = -follower->y2;
    refl->invisible = FALSE;
}

//  ---------------------------------------------------------------- it notices first (T-459 A)
//
//  The user, 2026-10-27: once THE SUBTLE LAYER is held, the player starts to notice -- and first through the daemon
//  beside them, the book's practice of reading your daemon. Now and then, once it has stood still a moment, the follower
//  turns toward the nearest thing still unfound within a screen (src/reveal.c Reveal_NearestUnfound: a hidden item, a
//  find, the way into a grove not yet found), a small ripple rises from it, and a faint glint shows where it looks. In
//  the book's own hue, CONTEXT's purple (the user's choice over a violet of its own): DAEMONS tools/gensense.py draws
//  both from gensubtle.py's colours, graphics/misc/subtle_sense.png, frames 0-3 the ripple and 4-6 the glint.
//
//  WHAT IT IS NOT. A colour beside a feeling (craft rule 1): the purple marks the noticing, and the daemon-emoji keeps
//  its own colours. A lens: it points at one thing at a time, every twenty seconds at most, and only at what is really
//  there and findable blind -- REVEAL still shows them all. Nothing is saved: the wait is IWRAM, and a new place (or a
//  battle) starts it again.
//
//  ITS COLOURS, as the face's: its own palette, loaded while either shows and freed when both are gone, in the field's
//  light and weather, and left in colour in HALFTONE as the follower is (Follower_SensePaletteSlot). Not greyed by
//  friendship -- the hue is the book's, not the daemon's. With no slot or sprite free it simply does not show.

static const u16 sSenseGfx[] = INCBIN_U16("graphics/misc/subtle_sense.4bpp");
static const u16 sSensePal[] = INCBIN_U16("graphics/misc/subtle_sense.gbapal");

static const struct SpriteFrameImage sSenseImages[] = {
    overworld_frame(sSenseGfx, 2, 2, 0), overworld_frame(sSenseGfx, 2, 2, 1), overworld_frame(sSenseGfx, 2, 2, 2),
    overworld_frame(sSenseGfx, 2, 2, 3), overworld_frame(sSenseGfx, 2, 2, 4), overworld_frame(sSenseGfx, 2, 2, 5),
    overworld_frame(sSenseGfx, 2, 2, 6),
};

static const struct OamData sSenseOam = {
    .affineMode = ST_OAM_AFFINE_OFF,
    .objMode = ST_OAM_OBJ_NORMAL,
    .bpp = ST_OAM_4BPP,
    .shape = SPRITE_SHAPE(16x16),
    .size = SPRITE_SIZE(16x16),
    .priority = 1,
};

//  The ripple widens twice; the glint catches the light twice, as a sparkle does. About a second each.
static const union AnimCmd sSenseAnim_Ripple[] = {
    ANIMCMD_FRAME(0, 8), ANIMCMD_FRAME(1, 8), ANIMCMD_FRAME(2, 8), ANIMCMD_FRAME(3, 8),
    ANIMCMD_FRAME(0, 8), ANIMCMD_FRAME(1, 8), ANIMCMD_FRAME(2, 8), ANIMCMD_FRAME(3, 10),
    ANIMCMD_END
};
static const union AnimCmd sSenseAnim_Glint[] = {
    ANIMCMD_FRAME(4, 6), ANIMCMD_FRAME(5, 6), ANIMCMD_FRAME(6, 8), ANIMCMD_FRAME(5, 6), ANIMCMD_FRAME(4, 6),
    ANIMCMD_FRAME(4, 6), ANIMCMD_FRAME(5, 6), ANIMCMD_FRAME(6, 8), ANIMCMD_FRAME(5, 6), ANIMCMD_FRAME(4, 6),
    ANIMCMD_END
};
static const union AnimCmd *const sSenseAnims_Ripple[] = { sSenseAnim_Ripple };
static const union AnimCmd *const sSenseAnims_Glint[] = { sSenseAnim_Glint };

static void SpriteCB_SenseRipple(struct Sprite *sprite);
static void SpriteCB_SenseGlint(struct Sprite *sprite);
static void SpriteCB_PondRing(struct Sprite *sprite);   // T-577

static const struct SpriteTemplate sSenseTemplate_Ripple = {
    .tileTag = TAG_NONE,
    .paletteTag = SENSE_TAG,
    .oam = &sSenseOam,
    .anims = sSenseAnims_Ripple,
    .images = sSenseImages,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCB_SenseRipple,
};

static const struct SpriteTemplate sSenseTemplate_Glint = {
    .tileTag = TAG_NONE,
    .paletteTag = SENSE_TAG,
    .oam = &sSenseOam,
    .anims = sSenseAnims_Glint,
    .images = sSenseImages,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCB_SenseGlint,
};

#if DAEMONS_DEBUG
EWRAM_DATA u8 gFollowerDebugSense = 0;   // noticings since the game started (wraps), for the theatre
#endif

static u8 LoadSensePalette(void)
{
    u8 idx = IndexOfSpritePaletteTag(SENSE_TAG);

    if (idx == 0xFF)
        idx = AllocSpritePalette(SENSE_TAG);
    if (idx == 0xFF)
        return 0xFF;
    LoadPalette(sSensePal, OBJ_PLTT_ID(idx), PLTT_SIZE_4BPP);
    ApplyGlobalFieldPaletteTint(idx);
    UpdateSpritePaletteWithWeather(idx);
    return idx;
}

static bool8 SenseShowing(void)
{
    u8 i;

    for (i = 0; i < MAX_SPRITES; i++)
        if (gSprites[i].inUse && (gSprites[i].callback == SpriteCB_SenseRipple || gSprites[i].callback == SpriteCB_SenseGlint
                               || gSprites[i].callback == SpriteCB_PondRing))   // T-577: the pond's rings share it
            return TRUE;
    return FALSE;
}

static void SenseEnd(struct Sprite *sprite)
{
    DestroySprite(sprite);
    if (!SenseShowing())
        FreeSpritePaletteByTag(SENSE_TAG);   // the last of the two gives the palette back
}

//  It rides the follower as its face does, rising a pixel every ten frames as it widens.
static void SpriteCB_SenseRipple(struct Sprite *sprite)
{
    struct Sprite *follower;

    if (sprite->animEnded || !IsOurs())
    {
        SenseEnd(sprite);
        return;
    }
    follower = &gSprites[sFollower.spriteId];
    sprite->x = follower->x;
    sprite->y = follower->y - 10;   // just over its head: the icon's body sits low in its 32x32 cell
    sprite->x2 = 0;
    sprite->y2 = follower->y2 - (sprite->data[0]++ / 10);
    sprite->invisible = follower->invisible;
}

//  It stays where it was seen: the map scrolls under it (coordOffsetEnabled), as a sparkle does.
static void SpriteCB_SenseGlint(struct Sprite *sprite)
{
    if (sprite->animEnded)
        SenseEnd(sprite);
}

//  The ripple over it and the glint at (x, y), in map coordinates. FALSE when there is no palette slot or sprite for
//  them, and then nothing shows at all -- never one without the other, and never in another's colours.
static bool8 ShowSense(s16 x, s16 y)
{
    u8 ripple, glint;

    if (SenseShowing() || LoadSensePalette() == 0xFF)
        return FALSE;
    ripple = CreateSpriteAtEnd(&sSenseTemplate_Ripple, 0, 0, 0x52);
    if (ripple == MAX_SPRITES)
    {
        FreeSpritePaletteByTag(SENSE_TAG);
        return FALSE;
    }
    SetSpritePosToOffsetMapCoords(&x, &y, 8, 8);
    glint = CreateSpriteAtEnd(&sSenseTemplate_Glint, x, y, 0x52);
    if (glint == MAX_SPRITES)
    {
        DestroySprite(&gSprites[ripple]);
        FreeSpritePaletteByTag(SENSE_TAG);
        return FALSE;
    }
    gSprites[ripple].coordOffsetEnabled = TRUE;
    gSprites[ripple].invisible = TRUE;                 // until the callback has placed it
    gSprites[glint].coordOffsetEnabled = TRUE;
    return TRUE;
}

static void Sense(struct Sprite *sprite, struct ObjectEvent *playerObj)
{
    s16 x, y, at;

    if (!FlagGet(FLAG_GOT_SUBTLE_LAYER) || sprite->invisible || ArePlayerFieldControlsLocked())
        return;
    if (sFollower.senseWait)
    {
        sFollower.senseWait--;
        return;
    }
    if (sFollower.idle < SENSE_STILL)
        return;
    if (!Reveal_NearestUnfound(&x, &y) || !ShowSense(x, y))
    {
        sFollower.senseWait = SENSE_LOOK;
        return;
    }
    // It turns toward it (icons face left; east is the same picture flipped). Straight above or below, it keeps its side.
    at = playerObj->currentCoords.x - sFollower.sumX / TRAIL;
    if (x < at)
        sFollower.facing = DIR_WEST;
    else if (x > at)
        sFollower.facing = DIR_EAST;
    sFollower.senseWait = SENSE_AGAIN;
#if DAEMONS_DEBUG
    gFollowerDebugSense++;
#endif
}

//  ---------------------------------------------------------------- the last word (T-580)
//
//  The user, 2026-11-17 (C-357's LAST WORD page, A): after THE SUBTLE LAYER's last page in the game's reader, the
//  follower turns to face the player. Nothing is said. Back in the field, once the fade is over and it is drawn, it
//  stands a beat looking away from the player, then turns to them. Icons face left or right only, so with the player
//  straight above or below it, the turn is the flip itself (DRAFT, a choice for the user). Walking on before the turn
//  lets it go. Nothing is saved.

#if DAEMONS_DEBUG
EWRAM_DATA u8 gFollowerDebugTurn = 0;   // turns made since the game started (wraps), for the theatre
#endif

void Follower_TurnAfterBook(void)
{
    if (!FlagGet(FLAG_GOT_SUBTLE_LAYER))
        return;
    sFollower.turnPending = TRUE;
    sFollower.turnWait = 0;
}

static void TurnToPlayer(struct Sprite *sprite)
{
    u8 away;

    if (!sFollower.turnPending)
        return;
    if (sFollower.turnWait != 0 && sFollower.idle < 2)
    {
        sFollower.turnPending = FALSE;          // the player walked on: it follows instead
        return;
    }
    if (sprite->invisible || gPaletteFade.active)
        return;
    if (sFollower.turnWait == 0)
    {
        // away from the player: east of it, it looks west; straight above or below, it keeps the side it has
        if (sFollower.sumX > 0)
            away = DIR_WEST;
        else if (sFollower.sumX < 0)
            away = DIR_EAST;
        else
            away = sFollower.facing == DIR_EAST ? DIR_EAST : DIR_WEST;
        sFollower.facing = away;
        sFollower.turnWait = TURN_BEAT;
        return;
    }
    if (--sFollower.turnWait != 0)
        return;
    sFollower.facing = sFollower.facing == DIR_EAST ? DIR_WEST : DIR_EAST;   // to the player
    sFollower.turnPending = FALSE;
#if DAEMONS_DEBUG
    gFollowerDebugTurn++;
#endif
}

//  ---------------------------------------------------------------- two ripples, one pond (T-577)
//
//  The user, 2026-11-17 (C-357's RIPPLES page, A): at a still pond, stand ten seconds with the follower, and two rings
//  spread from the two reflections and meet; then one purple line, a read, kept in THE LOG like every read. The still
//  ponds are the three whose edge two can stand on side by side with POND water under both (MB_POND_WATER, still --
//  not a river's or the sea's): VIRIDIAN CITY's, CELADON CITY's and ROUTE 22's (DRAFT, the game lane's pick). The two
//  stand a tile apart along the edge, the water just south of each, where their reflections are; neither moves for
//  RIPPLES_STILL frames. The rings are the noticing's own ripple (graphics/misc/subtle_sense.png frames 0-3, in the
//  book's purple) spreading wider as it plays, one on each reflection, until the two touch between them. Once an
//  arrival; walking away and back is another. Wakes only with THE SUBTLE LAYER held.

#define RIPPLES_STILL    600      // ten seconds

static const u16 sStillPonds[] = { MAP_VIRIDIAN_CITY, MAP_CELADON_CITY, MAP_ROUTE22 };

static const struct OamData sPondRingOam = {
    .affineMode = ST_OAM_AFFINE_DOUBLE,
    .objMode = ST_OAM_OBJ_NORMAL,
    .bpp = ST_OAM_4BPP,
    .shape = SPRITE_SHAPE(16x16),
    .size = SPRITE_SIZE(16x16),
    .priority = 2,
};

//  It widens (frames 0-2), holds its widest while it spreads, and breaks where it has met the other (frame 3).
static const union AnimCmd sPondRingAnim[] = {
    ANIMCMD_FRAME(0, 12), ANIMCMD_FRAME(1, 12), ANIMCMD_FRAME(2, 60), ANIMCMD_FRAME(3, 24),
    ANIMCMD_END
};
static const union AnimCmd *const sPondRingAnims[] = { sPondRingAnim };

//  1x while it widens, then out to 1.5x over a second: frame 2 is 13 pixels across, so two a tile apart touch.
static const union AffineAnimCmd sPondRingAffine[] = {
    AFFINEANIMCMD_FRAME(0x100, 0x100, 0, 0),
    AFFINEANIMCMD_FRAME(0, 0, 0, 24),
    AFFINEANIMCMD_FRAME(2, 2, 0, 64),
    AFFINEANIMCMD_FRAME(0, 0, 0, 40),
    AFFINEANIMCMD_END,
};
static const union AffineAnimCmd *const sPondRingAffines[] = { sPondRingAffine };

static const struct SpriteTemplate sPondRingTemplate = {
    .tileTag = TAG_NONE,
    .paletteTag = SENSE_TAG,
    .oam = &sPondRingOam,
    .anims = sPondRingAnims,
    .images = sSenseImages,
    .affineAnims = sPondRingAffines,
    .callback = SpriteCB_PondRing,
};

#if DAEMONS_DEBUG
EWRAM_DATA u8 gFollowerDebugPond = 0;   // ponds that rippled since the game started (wraps), for the theatre
#endif

static void SpriteCB_PondRing(struct Sprite *sprite)
{
    if (!sprite->animEnded)
        return;
    FreeOamMatrix(sprite->oam.matrixNum);
    SenseEnd(sprite);
    if (sFollower.pondRings != 0)
        sFollower.pondRings--;
}

static bool8 PondUnder(s16 x, s16 y)
{
    return MapGridGetMetatileBehaviorAt(x, y + 1) == MB_POND_WATER;
}

static bool8 AtAStillPond(void)
{
    u16 map = (gSaveBlock1Ptr->location.mapGroup << 8) | gSaveBlock1Ptr->location.mapNum;
    u8 i;

    for (i = 0; i < ARRAY_COUNT(sStillPonds); i++)
        if (sStillPonds[i] == map)
            return TRUE;
    return FALSE;
}

static bool8 PondRing(s16 x, s16 y)
{
    u8 id;

    SetSpritePosToOffsetMapCoords(&x, &y, 8, 8);
    id = CreateSprite(&sPondRingTemplate, x, y + 16, 0x52);   // on the reflection: the water a tile south
    if (id == MAX_SPRITES)
        return FALSE;
    gSprites[id].coordOffsetEnabled = TRUE;
    return TRUE;
}

static void Ripples(struct Sprite *sprite, struct ObjectEvent *playerObj)
{
    u16 here = ((gSaveBlock1Ptr->location.mapGroup << 8) | gSaveBlock1Ptr->location.mapNum) + 1;
    s16 px = playerObj->currentCoords.x, py = playerObj->currentCoords.y;

    if (here != sFollower.pondMap)
    {
        sFollower.pondMap = here;
        sFollower.pondDone = FALSE;
        sFollower.pondStill = 0;
    }
    if (sFollower.pondDone)
        return;
    if (sFollower.pondStill >= RIPPLES_STILL)
    {
        if (sFollower.pondRings != 0 || ArePlayerFieldControlsLocked())
            return;
        // both rings have met and gone: the read
        sFollower.pondDone = TRUE;
        Daemons_GiveRead(READ_RIPPLES);
        LockPlayerFieldControls();
        ScriptContext_SetupScript(Daemons_EventScript_Read);
#if DAEMONS_DEBUG
        gFollowerDebugPond++;
#endif
        return;
    }
    if (!FlagGet(FLAG_GOT_SUBTLE_LAYER) || !AtAStillPond() || sprite->invisible || ArePlayerFieldControlsLocked()
     || sFollower.idle < 2 || sFollower.sumY != 0 || (sFollower.sumX != TRAIL && sFollower.sumX != -TRAIL)
     || !PondUnder(px, py) || !PondUnder(px - sFollower.sumX / TRAIL, py))
    {
        sFollower.pondStill = 0;
        return;
    }
    if (++sFollower.pondStill < RIPPLES_STILL)
        return;
    sFollower.pondRings = 0;
    if (LoadSensePalette() != 0xFF)
    {
        sFollower.pondRings += PondRing(px, py);
        sFollower.pondRings += PondRing(px - sFollower.sumX / TRAIL, py);
        if (sFollower.pondRings == 0)
            FreeSpritePaletteByTag(SENSE_TAG);
    }
}

//  ---------------------------------------------------------------- once a frame

static void Place(struct Sprite *sprite, struct Sprite *player, struct ObjectEvent *playerObj)
{
    s32 dist = (sFollower.sumX < 0 ? -sFollower.sumX : sFollower.sumX) + (sFollower.sumY < 0 ? -sFollower.sumY : sFollower.sumY);
    bool8 moving = sFollower.idle < 2;
    u8 inTile = sFollower.steps % TRAIL;

    sprite->x = player->x - sFollower.sumX;
    sprite->y = player->y - sFollower.sumY;   // as the player stands, for the order it is drawn in
    SetObjectSubpriorityByElevation(playerObj->previousElevation, sprite, 2);   // behind the player on a tie
    sprite->y += FOLLOWER_Y;
    sprite->y2 = sFollower.y2 - (moving && inTile >= 4 && inTile < 12 ? 1 : 0);   // its step: a 1 px hop
    sprite->oam.priority = player->oam.priority;
    sprite->oam.matrixNum = sFollower.facing == DIR_EAST ? ST_OAM_HFLIP : 0;   // icons face left
    sprite->oam.tileNum = sprite->sheetTileStart;   // one frame; the hop above is its step
    sprite->invisible = dist < TRAIL / 2;   // still inside the player: walking out of them
    FindTile(sprite->x, player->y - sFollower.sumY);
    if (sFollower.sceneHidden || SomeoneInItsTile())
        sprite->invisible = TRUE;            // not snapped: it is there again the moment they have gone
    if (!sprite->invisible)
        RaiseGrass(playerObj, sprite, moving);
    else
        sFollower.grassX = sFollower.grassY = -1;
    Reflect(sprite);

    // Where it stands, kept for the next field entry (a battle, a menu) -- only when it stands a whole tile off.
    if (!moving && dist == TRAIL && !sprite->invisible)
    {
        sFollower.restValid = TRUE;
        sFollower.restDX = -sFollower.sumX / TRAIL;
        sFollower.restDY = -sFollower.sumY / TRAIL;
        sFollower.restGroup = gSaveBlock1Ptr->location.mapGroup;
        sFollower.restNum = gSaveBlock1Ptr->location.mapNum;
        sFollower.restX = playerObj->currentCoords.x;
        sFollower.restY = playerObj->currentCoords.y;
    }
}

void Follower_Update(void)
{
    struct ObjectEvent *playerObj = &gObjectEvents[gPlayerAvatar.objectEventId];
    struct Sprite *player = &gSprites[gPlayerAvatar.spriteId];
    struct Sprite *sprite;
    s8 slot;

    if (!playerObj->active || !player->inUse)
        return;
    HumTidy();   // T-596
    if (FollowerIsAbsent())
    {
        if (IsOurs())
            Despawn();
        sFollower.spriteId = MAX_SPRITES;
        sFollower.restValid = FALSE;
        sFollower.askWait = 0;   // T-596: no follower, no ask -- not one saved for later
        return;
    }
    if (!IsOurs())
    {
        sFollower.spriteId = MAX_SPRITES;
        if (sFollower.retry)
        {
            sFollower.retry--;
            return;
        }
        slot = Follower_LeadSlot();
        if (slot < 0)
        {
            sFollower.retry = CHECK_FRAMES;   // nobody to follow: ask again in a moment
            sFollower.restValid = FALSE;
            return;
        }
        Spawn(slot, player, playerObj);
        if (!IsOurs())
            return;
    }
    else if (--sFollower.check == 0)
    {
        sFollower.check = CHECK_FRAMES;
        slot = Follower_LeadSlot();
        if (slot < 0)
        {
            Despawn();
            sFollower.restValid = FALSE;
            return;
        }
        CheckLead(slot);
        if (!IsOurs())
            return;
    }
    sprite = &gSprites[sFollower.spriteId];

    if (gPlayerAvatar.spriteId != sFollower.playerSpriteId)
    {
        sFollower.playerSpriteId = gPlayerAvatar.spriteId;
        sFollower.lastX = player->x;
        sFollower.lastY = player->y;
        Snap();
    }
    Track(player, playerObj);
    if (!ArePlayerFieldControlsLocked())
        sFollower.scriptHidden = FALSE;
    if (!ArePlayerFieldControlsLocked())
        sFollower.sceneHidden = FALSE;      // hidefollower lasts the scene, at most
    if (FollowerIsHidden(player))
    {
        Snap();
        sprite->invisible = TRUE;
        sFollower.grassX = sFollower.grassY = -1;
        DropReflection();
        return;
    }
    // After Continue: the first frame it may be drawn (the field has faded in, the recap is over), and while it is
    // still on the player's own tile, it stands a tile behind them instead of walking out of them.
    if (sFollower.continued)
    {
        if (sFollower.sumX == 0 && sFollower.sumY == 0)
            StandBehind(playerObj);
        sFollower.continued = FALSE;
    }
    Place(sprite, player, playerObj);
    TurnToPlayer(sprite);       // T-580
    Ripples(sprite, playerObj); // T-577
    Sense(sprite, playerObj);   // T-459
    Shimmer();                  // T-480
    Hum(sprite);                // T-596
    AskToSing(sprite);          // T-596
}

//  T-480: THE WITCHING HOUR's shimmer (src/daemons_witching.c). Every fourth frame of the hour its palette is loaded
//  again -- friendship's saturation, the field's light, the weather -- and a few sixteenths of the book's purple laid
//  over it where it stands, so nothing compounds and a flicker is only the weight moving. Never during a fade (a load
//  writes the faded colours too, and would pull it out of the fade). When the hour ends it is loaded clean once.
static void Shimmer(void)
{
    u8 idx;

    if (gPaletteFade.active)
        return;
    if (!Witching_ShimmerNow())
    {
        u8 glow = Hours_FollowerGlow();   // T-519: THE CREATOR'S HOUR's faint, steady gold instead

        if (glow != 0)
        {
            if ((gMain.vblankCounter2 & 3) != 0 && sFollower.shimmer)
                return;
            idx = LoadFollowerPalette();
            if (idx == 0xFF)
                return;
            Hours_GlowFadedInPlace(OBJ_PLTT_ID(idx), glow);
            sFollower.shimmer = TRUE;
            return;
        }
        if (sFollower.shimmer)
        {
            sFollower.shimmer = FALSE;
            LoadFollowerPalette();
        }
        return;
    }
    if ((gMain.vblankCounter2 & 3) != 0 && sFollower.shimmer)
        return;
    idx = LoadFollowerPalette();
    if (idx == 0xFF)
        return;
    Witching_ShimmerFadedInPlace(OBJ_PLTT_ID(idx), Witching_ShimmerWeight());
    sFollower.shimmer = TRUE;
}

//  ---------------------------------------------------------------- A on it (T-18 P2)
//
//  The plan's P2: A on the follower gives the line for its friendship (the five readings of the ladder -- observation,
//  never a feeling named), then one mood when one holds, in this order: it noticed something (an unfound hidden item
//  within two tiles), the place (THE NEXUS; HALFTONE, which DOLDRUM CAVE unread is too), and, outdoors and not in a
//  timeless place, the sky, the watch (the day has no line) and last the season. A mood's two lines take turns. A
//  bubble rises over it -- the trainers' own -- and it turns to face the player. Every line is DRAFT
//  (data/scripts/follower.inc). Friendship's readings: 0-49, 50-99, 100-149, 150-219, 220-255.

enum
{
    MOOD_NONE, MOOD_NOTICE, MOOD_NEXUS, MOOD_HALFTONE,
    MOOD_RAIN, MOOD_STORM, MOOD_SNOW, MOOD_FOG, MOOD_WIND,
    MOOD_NIGHT, MOOD_DUSK, MOOD_DAWN,
    MOOD_WINTER, MOOD_SPRING, MOOD_SUMMER, MOOD_AUTUMN,   // SEASON_* order
    MOOD_COUNT
};

static const u8 sTierFloor[] = { 0, 50, 100, 150, 220 };
static const u8 *const sTierLines[] = {
    Follower_Text_Tier0, Follower_Text_Tier1, Follower_Text_Tier2, Follower_Text_Tier3, Follower_Text_Tier4,
};
//  The bubble by reading: a question while it keeps its distance, a mark once it stays, the smile at the top.
static const u8 sTierEmote[] = {
    EMOTICON_QUESTION, EMOTICON_QUESTION, EMOTICON_EXCLAMATION, EMOTICON_EXCLAMATION, EMOTICON_SMILEY,
};

static const u8 *const sMoodLines[MOOD_COUNT][2] = {
    [MOOD_NOTICE]   = { Follower_Text_Notice,    Follower_Text_Notice },
    [MOOD_NEXUS]    = { Follower_Text_Nexus0,    Follower_Text_Nexus1 },
    [MOOD_HALFTONE] = { Follower_Text_Halftone0, Follower_Text_Halftone1 },
    [MOOD_RAIN]     = { Follower_Text_Rain0,     Follower_Text_Rain1 },
    [MOOD_STORM]    = { Follower_Text_Storm0,    Follower_Text_Storm1 },
    [MOOD_SNOW]     = { Follower_Text_Snow0,     Follower_Text_Snow1 },
    [MOOD_FOG]      = { Follower_Text_Fog0,      Follower_Text_Fog1 },
    [MOOD_WIND]     = { Follower_Text_Wind0,     Follower_Text_Wind1 },
    [MOOD_NIGHT]    = { Follower_Text_Night0,    Follower_Text_Night1 },
    [MOOD_DUSK]     = { Follower_Text_Dusk0,     Follower_Text_Dusk1 },
    [MOOD_DAWN]     = { Follower_Text_Dawn0,     Follower_Text_Dawn1 },
    [MOOD_WINTER]   = { Follower_Text_Winter0,   Follower_Text_Winter1 },
    [MOOD_SPRING]   = { Follower_Text_Spring0,   Follower_Text_Spring1 },
    [MOOD_SUMMER]   = { Follower_Text_Summer0,   Follower_Text_Summer1 },
    [MOOD_AUTUMN]   = { Follower_Text_Autumn0,   Follower_Text_Autumn1 },
};

#define NOTICE_TILES 2

//  Where it stands, in map coordinates (MAP_OFFSET included, as the player's): only when drawn and a whole tile off.
static bool8 StandsAt(s16 x, s16 y)
{
    struct ObjectEvent *playerObj = &gObjectEvents[gPlayerAvatar.objectEventId];

    if (!IsOurs() || gSprites[sFollower.spriteId].invisible)
        return FALSE;
    if (sFollower.sumX % TRAIL != 0 || sFollower.sumY % TRAIL != 0)
        return FALSE;
    return playerObj->currentCoords.x - sFollower.sumX / TRAIL == x
        && playerObj->currentCoords.y - sFollower.sumY / TRAIL == y;
}

const u8 *Follower_GetInteractionScript(struct MapPosition *position)
{
    if (!StandsAt(position->x, position->y))
        return NULL;
    return EventScript_Follower;
}

//  An unfound hidden item (not one underfoot: those are the ITEMFINDER's alone) within NOTICE_TILES of the player.
static bool8 HiddenItemNear(void)
{
    const struct MapEvents *events = gMapHeader.events;
    s16 x, y, dx, dy;
    s32 i;

    PlayerGetDestCoords(&x, &y);
    for (i = 0; i < events->bgEventCount; i++)
    {
        const struct BgEvent *bg = &events->bgEvents[i];

        if (bg->kind != BG_EVENT_HIDDEN_ITEM
         || FlagGet(GetHiddenItemAttr(bg->bgUnion.hiddenItem, HIDDEN_ITEM_FLAG))
         || GetHiddenItemAttr(bg->bgUnion.hiddenItem, HIDDEN_ITEM_UNDERFOOT))
            continue;
        dx = bg->x + MAP_OFFSET - x;
        dy = bg->y + MAP_OFFSET - y;
        if (dx >= -NOTICE_TILES && dx <= NOTICE_TILES && dy >= -NOTICE_TILES && dy <= NOTICE_TILES)
            return TRUE;
    }
    return FALSE;
}

static u8 PickMood(void)
{
    if (HiddenItemNear())
        return MOOD_NOTICE;
    if (gMapHeader.regionMapSectionId == MAPSEC_NEXUS)
        return MOOD_NEXUS;
    if (DaemonsIsHalftone())
        return MOOD_HALFTONE;
    if (!IsMapTypeOutdoors(gMapHeader.mapType) || DaemonsIsTimeless())
        return MOOD_NONE;
    switch (DaemonsSkyNow())
    {
    case DAEMONS_SKY_RAIN:  return MOOD_RAIN;
    case DAEMONS_SKY_STORM: return MOOD_STORM;
    case DAEMONS_SKY_SNOW:  return MOOD_SNOW;
    case DAEMONS_SKY_FOG:   return MOOD_FOG;
    case DAEMONS_SKY_WIND:  return MOOD_WIND;
    }
    switch (DaemonsWatch())
    {
    case WATCH_NIGHT: return MOOD_NIGHT;
    case WATCH_DUSK:  return MOOD_DUSK;
    case WATCH_DAWN:  return MOOD_DAWN;
    }
    return MOOD_WINTER + DaemonsSeason() % SEASON_COUNT;
}

//  The bubble rides it as the trainers' rides an object event: a small hop up, then held until its animation ends.
static void SpriteCB_FollowerEmote(struct Sprite *sprite)
{
    struct Sprite *follower;

    if (sprite->animEnded || !IsOurs())
    {
        DestroySprite(sprite);
        return;
    }
    follower = &gSprites[sFollower.spriteId];
    sprite->data[4] += sprite->data[3];
    if (sprite->data[4])
        sprite->data[3]++;
    else
        sprite->data[3] = 0;
    sprite->x = follower->x;
    sprite->y = follower->y - 16;   // over the icon's body, which sits low in its 32x32 cell
    sprite->x2 = 0;
    sprite->y2 = follower->y2 + sprite->data[4];
    sprite->invisible = follower->invisible;
}

//  ---------------------------------------------------------------- the daemon-emoji (T-431 P3)
//
//  The bubble over it is a small screen with a tail, and on the glass a face built from three of its feelings, none of
//  them named (DAEMONS tools/genemoji.py, graphics/misc/daemon_emoji.png: 27 frames of 16x16, three a row, frame
//  mad * 9 + eyes * 3 + mouth). EYES from CALM: half-lidded at +20 or more, wide at -20 or less, open between. MOUTH
//  from GLAD - SAD: up at +20, down at -20, flat between. TINT from MAD: none under 20, warm from 20, hot from 40 --
//  the SUMMARY page's own lines (FEELINGS_LEAN, FEELINGS_QUIET, FEELINGS_MAD_LINE), so a face and its dot agree. A
//  daemon never stamped feels nothing yet: open, flat, cool. It has its OWN palette (the trainers' bubbles share OBJ
//  palette 0), loaded while it shows and freed after, greyed by friendship exactly as the follower is, and in HALFTONE
//  left in colour with the follower (DaemonsGreyHalftoneFrame, ApplyGlobalFieldPaletteTint). With no slot free it
//  is not drawn in another's colours: the caller falls back to the friendship bubble. One frame for the trainers'
//  whole 60 (their bubble's 4 + 4 + 52), with the same hop.

static const u16 sEmojiGfx[] = INCBIN_U16("graphics/misc/daemon_emoji.4bpp");
static const u16 sEmojiPal[] = INCBIN_U16("graphics/misc/daemon_emoji.gbapal");

#define EMOJI_FRAME(n) overworld_frame(sEmojiGfx, 2, 2, n)
static const struct SpriteFrameImage sEmojiImages[27] = {
    EMOJI_FRAME(0),  EMOJI_FRAME(1),  EMOJI_FRAME(2),  EMOJI_FRAME(3),  EMOJI_FRAME(4),  EMOJI_FRAME(5),
    EMOJI_FRAME(6),  EMOJI_FRAME(7),  EMOJI_FRAME(8),  EMOJI_FRAME(9),  EMOJI_FRAME(10), EMOJI_FRAME(11),
    EMOJI_FRAME(12), EMOJI_FRAME(13), EMOJI_FRAME(14), EMOJI_FRAME(15), EMOJI_FRAME(16), EMOJI_FRAME(17),
    EMOJI_FRAME(18), EMOJI_FRAME(19), EMOJI_FRAME(20), EMOJI_FRAME(21), EMOJI_FRAME(22), EMOJI_FRAME(23),
    EMOJI_FRAME(24), EMOJI_FRAME(25), EMOJI_FRAME(26),
};

static const struct OamData sEmojiOam = {
    .affineMode = ST_OAM_AFFINE_OFF,
    .objMode = ST_OAM_OBJ_NORMAL,
    .bpp = ST_OAM_4BPP,
    .shape = SPRITE_SHAPE(16x16),
    .size = SPRITE_SIZE(16x16),
    .priority = 1,
};

static const union AnimCmd sEmojiAnim[] = {
    ANIMCMD_FRAME(0, 60),
    ANIMCMD_END
};
static const union AnimCmd *const sEmojiAnims[] = { sEmojiAnim };

static void SpriteCB_FollowerEmoji(struct Sprite *sprite);

static const struct SpriteTemplate sEmojiTemplate = {
    .tileTag = TAG_NONE,
    .paletteTag = EMOJI_TAG,
    .oam = &sEmojiOam,
    .anims = sEmojiAnims,
    .images = sEmojiImages,   // replaced by the one face before its first frame (CreateEmojiSprite)
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCB_FollowerEmoji,
};

static u8 LoadEmojiPalette(void)
{
    u16 buf[16];
    u8 idx = IndexOfSpritePaletteTag(EMOJI_TAG);

    if (idx == 0xFF)
        idx = AllocSpritePalette(EMOJI_TAG);
    if (idx == 0xFF)
        return 0xFF;
    CpuCopy16(sEmojiPal, buf, PLTT_SIZE_4BPP);
    Saturation_KeepToBuffer(buf, sFollower.songKeep);      // T-424, T-597: greyed until sung, as the follower is
    LoadPalette(buf, OBJ_PLTT_ID(idx), PLTT_SIZE_4BPP);
    ApplyGlobalFieldPaletteTint(idx);
    UpdateSpritePaletteWithWeather(idx);
    return idx;
}

static void SpriteCB_FollowerEmoji(struct Sprite *sprite)
{
    if (sprite->animEnded || !IsOurs())
    {
        DestroySprite(sprite);
        FreeSpritePaletteByTag(EMOJI_TAG);
        return;
    }
    SpriteCB_FollowerEmote(sprite);
}

//  The face for these feelings: frame mad * 9 + eyes * 3 + mouth.
static u8 EmojiFrame(struct Pokemon *mon)
{
    struct DaemonFeelings f;
    s16 lean;
    u8 eyes, mouth, tint;

    if (!Feelings_IsSet(&mon->box))
        return 0 * 9 + 1 * 3 + 1;   // never stamped: open, flat, cool
    Feelings_Get(&mon->box, &f);
    eyes = f.calm >= FEELINGS_LEAN ? 0 : f.calm <= -FEELINGS_LEAN ? 2 : 1;
    lean = (s16)f.glad - f.sad;
    mouth = lean >= FEELINGS_LEAN ? 2 : lean <= -FEELINGS_LEAN ? 0 : 1;
    tint = f.mad >= FEELINGS_MAD_LINE ? 2 : f.mad >= FEELINGS_QUIET ? 1 : 0;
    return tint * 9 + eyes * 3 + mouth;
}

#if DAEMONS_DEBUG
EWRAM_DATA u8 gFollowerDebugEmoji = 0;   // the last face drawn + 1 (0: none yet, 0xFF: the fallback bubble), for the theatre
#endif

static u8 CreateEmojiSprite(struct Pokemon *mon)
{
    u8 i, spriteId, frame;

    // A second press before the first face has gone: the old one goes now, so the palette is never freed under a
    // face still showing.
    for (i = 0; i < MAX_SPRITES; i++)
        if (gSprites[i].inUse && gSprites[i].callback == SpriteCB_FollowerEmoji)
            DestroySprite(&gSprites[i]);
    if (LoadEmojiPalette() == 0xFF)
    {
#if DAEMONS_DEBUG
        gFollowerDebugEmoji = 0xFF;
#endif
        return MAX_SPRITES;
    }
    frame = EmojiFrame(mon);
    spriteId = CreateSpriteAtEnd(&sEmojiTemplate, 0, 0, 0x52);
    if (spriteId == MAX_SPRITES)
    {
        FreeSpritePaletteByTag(EMOJI_TAG);
        return MAX_SPRITES;
    }
    gSprites[spriteId].images = &sEmojiImages[frame];   // the anim's frame 0 is this face
    gSprites[spriteId].coordOffsetEnabled = TRUE;
    gSprites[spriteId].invisible = TRUE;                 // until the callback has placed it
#if DAEMONS_DEBUG
    gFollowerDebugEmoji = frame + 1;
#endif
    return spriteId;
}

void Follower_Talk(void)
{
    s8 slot = Follower_LeadSlot();
    struct Pokemon *mon;
    u8 friendship, band, mood, spriteId;

    gStringVar4[0] = EOS;
    if (slot < 0)
        return;
    mon = &gPlayerParty[slot];
    GetMonData(mon, MON_DATA_NICKNAME, gStringVar1);
    StringGet_Nickname(gStringVar1);
    // T-483: Halloween -- the first A takes the costume's mask off, for the rest of the day.
    if (sFollower.costume != SPECIES_NONE && IsOurs())
    {
        StringCopy(gStringVar2, gSpeciesNames[sFollower.costume]);
        Hermetic_Unmask();
        StringExpandPlaceholders(gStringVar4, Follower_Text_Unmask);
        Despawn();   // made again next frame, as itself
        return;
    }
    friendship = GetMonData(mon, MON_DATA_FRIENDSHIP);
    for (band = ARRAY_COUNT(sTierFloor) - 1; band > 0 && friendship < sTierFloor[band]; band--)
        ;
    mood = PickMood();

    // T-18 P3, the user 2026-10-21: ONE box a press, friendship and mood in turn -- the friendship line first, the
    // mood on the next press (when none holds, the friendship line again). Walking on starts it over.
    if (!(sFollower.talks & 1))
        mood = MOOD_NONE;
    if (mood == MOOD_NOTICE && FlagGet(FLAG_GOT_SUBTLE_LAYER))
    {
        // T-459: once THE SUBTLE LAYER is held, its noticing is said in the book's purple (the words are T-18's own),
        // on the dialogue box's grey shadow: {COLOR PURPLE}{SHADOW LIGHT_GRAY}, then the line.
        static const u8 sPurple[] = {
            EXT_CTRL_CODE_BEGIN, EXT_CTRL_CODE_COLOR, TEXT_COLOR_PURPLE,
            EXT_CTRL_CODE_BEGIN, EXT_CTRL_CODE_SHADOW, TEXT_COLOR_LIGHT_GRAY, EOS,
        };
        StringExpandPlaceholders(StringCopy(gStringVar4, sPurple), sMoodLines[mood][sFollower.moodTurn++ & 1]);
    }
    else if (mood != MOOD_NONE)
        StringExpandPlaceholders(gStringVar4, sMoodLines[mood][sFollower.moodTurn++ & 1]);
    else
        StringExpandPlaceholders(gStringVar4, sTierLines[band]);
    sFollower.talks++;

    // It turns to the player (icons face left; east is the same picture flipped). Above or below, it keeps its side.
    if (sFollower.sumX > 0)
        sFollower.facing = DIR_EAST;
    else if (sFollower.sumX < 0)
        sFollower.facing = DIR_WEST;

    // T-431 P3: its own face, from what it feels -- unless it is noticing something (the hidden item's !! stays), or
    // there is no palette slot for the face, when the friendship bubble of T-18 P2 shows in the trainers' colours.
    spriteId = MAX_SPRITES;
    if (mood != MOOD_NOTICE)
        spriteId = CreateEmojiSprite(mon);
    if (spriteId == MAX_SPRITES)
        spriteId = CreateEmoticonSprite(mood == MOOD_NOTICE ? EMOTICON_DOUBLE_EXCL : sTierEmote[band], SpriteCB_FollowerEmote);
    if (spriteId != MAX_SPRITES)
        gSprites[spriteId].data[3] = -5;
}

//  ---------------------------------------------------------------- scenes and Continue (T-18 P3)

//  hidefollower / showfollower (asm/macros/event.inc): a scene that would read wrong with the daemon standing in it.
//  It is still there and still follows the player's path; it is only not drawn, and the hide lasts until showfollower
//  or the end of the script, whichever comes first -- a scene cannot leave it hidden by forgetting to show it.
void Follower_SceneHide(void)
{
    sFollower.sceneHidden = TRUE;
}

void Follower_SceneShow(void)
{
    sFollower.sceneHidden = FALSE;
}

//  ContinueLoadedSave (both of Continue's doors, the recap's too): nothing of it was saved, so it comes back a tile behind the player (if that tile can be
//  stood on), not out of the player's own tile as it does through a door.
void Follower_OnContinue(void)
{
    sFollower.continued = TRUE;
    sFollower.restValid = FALSE;
}

//  ---------------------------------------------------------------- the wish to sing (T-596)
//
//  The user, 2026-11-18 (by multiple choice: hums as you walk and asks after a battle): "visual correctly colour coded
//  notes emerge from the daemons mouth ... the player associates the fact the daemon said it wants to sing along with
//  the visual emergence of the notes creating an intuitive memory pointer". As a daemon nears its evolution it wishes
//  to sing (DaemonSong_Wish: a form that grows on, five-eighths of its own part learned or more).
//
//  THE HUM. Now and then the follower hums one bar of its own part as it knows it (DaemonSong_Hum: the bar it is
//  learning, now and then one it has), quietly under the map's music (HUM_VOLUME, src/daemon_song.c) -- the nearer its
//  evolution, the more often: about every forty-five seconds at the wish's start, about every ten when perfect. Never
//  while a script, a message box or a menu runs (ArePlayerFieldControlsLocked covers the START menu and every script),
//  nor in a fade, under PLAY, in the quest log's replay, or while it is not drawn; a hum caught by one of them stops.
//
//  THE NOTES. Each note it sings is a small note rising from its MOUTH -- found in its own picture as it is made: the
//  leftmost pixel of the top of it (its snout, facing left), mirrored when it faces right -- drifting the way it faces,
//  in its pitch's colour: PLAY's own colours (graphics/play/key_colours.gbapal, by reference), a black key's note the
//  two white keys either side of it (DAEMONS tools/genhumnotes.py, graphics/daemon_song/hum_notes.png). A note not yet
//  learned is STATIC: the note's shape in snow, in its true colour only on the frames its true pitch is heard -- the
//  audio's own flicker, read as it sounds (DaemonSong_Heard) -- and, once its sound is over, at the same odds. Its own
//  OBJ palette, loaded while a note shows or it hums and freed after, never the last slot free (it does not hum
//  then), and untinted: they shine as PLAY's notes do, in HALFTONE too (fieldmap.c, field_effect.c).
//
//  THE ASK. After a battle won (battle_setup.c), the nearer its evolution the likelier (a quarter at the wish's start,
//  near certain when perfect), once back in the field and standing a moment: "NAME wants to sing." -- yes and it sings
//  its song as it knows it, the notes rising from it, as SING does in the party menu (B stops it). DRAFT, every word.

#define HUM_SLOWEST      2700    // at the wish's start: about every forty-five seconds
#define HUM_FASTEST      600     // perfect: about every ten
#define NOTE_LIFE        56      // frames a note rises before it is gone
#define NOTE_FLICKER     4       // a static note changes this often (daemon_song.c's FLICKER)
#define NOTE_STATIC      12      // hum_notes.png: frames 12-14 are static
#define ASK_STILL        60      // after a battle, a second free and standing in the field before it asks
#define ASK_WINDOW       900     // ... and if that does not come within fifteen seconds, it lets the wish go

static const u16 sHumNotesGfx[] = INCBIN_U16("graphics/daemon_song/hum_notes.4bpp");
static const u16 sKeyColoursPal[] = INCBIN_U16("graphics/play/key_colours.gbapal");   // PLAY's, by reference

static const struct SpriteFrameImage sHumNoteImages[] = {
    overworld_frame(sHumNotesGfx, 1, 1, 0), overworld_frame(sHumNotesGfx, 1, 1, 1), overworld_frame(sHumNotesGfx, 1, 1, 2),
    overworld_frame(sHumNotesGfx, 1, 1, 3), overworld_frame(sHumNotesGfx, 1, 1, 4), overworld_frame(sHumNotesGfx, 1, 1, 5),
    overworld_frame(sHumNotesGfx, 1, 1, 6), overworld_frame(sHumNotesGfx, 1, 1, 7), overworld_frame(sHumNotesGfx, 1, 1, 8),
    overworld_frame(sHumNotesGfx, 1, 1, 9), overworld_frame(sHumNotesGfx, 1, 1, 10), overworld_frame(sHumNotesGfx, 1, 1, 11),
    overworld_frame(sHumNotesGfx, 1, 1, 12), overworld_frame(sHumNotesGfx, 1, 1, 13), overworld_frame(sHumNotesGfx, 1, 1, 14),
};

static const struct OamData sHumNoteOam = {
    .affineMode = ST_OAM_AFFINE_OFF,
    .objMode = ST_OAM_OBJ_NORMAL,
    .bpp = ST_OAM_4BPP,
    .shape = SPRITE_SHAPE(8x8),
    .size = SPRITE_SIZE(8x8),
    .priority = 1,
};

#define HUM_FRAME(n) {ANIMCMD_FRAME(n, 1), ANIMCMD_END}
static const union AnimCmd sHumNoteAnim[15][2] = {
    HUM_FRAME(0), HUM_FRAME(1), HUM_FRAME(2), HUM_FRAME(3), HUM_FRAME(4), HUM_FRAME(5), HUM_FRAME(6), HUM_FRAME(7),
    HUM_FRAME(8), HUM_FRAME(9), HUM_FRAME(10), HUM_FRAME(11), HUM_FRAME(12), HUM_FRAME(13), HUM_FRAME(14),
};
static const union AnimCmd *const sHumNoteAnims[15] = {
    sHumNoteAnim[0], sHumNoteAnim[1], sHumNoteAnim[2], sHumNoteAnim[3], sHumNoteAnim[4], sHumNoteAnim[5],
    sHumNoteAnim[6], sHumNoteAnim[7], sHumNoteAnim[8], sHumNoteAnim[9], sHumNoteAnim[10], sHumNoteAnim[11],
    sHumNoteAnim[12], sHumNoteAnim[13], sHumNoteAnim[14],
};

static void SpriteCB_HumNote(struct Sprite *sprite);

static const struct SpriteTemplate sHumNoteTemplate = {
    .tileTag = TAG_NONE,
    .paletteTag = HUM_TAG,
    .oam = &sHumNoteOam,
    .anims = sHumNoteAnims,
    .images = sHumNoteImages,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCB_HumNote,
};

#if DAEMONS_DEBUG
EWRAM_DATA u8 gFollowerDebugHum = 0;    // hums since the game started (wraps), for the theatre
EWRAM_DATA u8 gFollowerDebugNotes = 0;  // notes drawn (wraps)
EWRAM_DATA u8 gFollowerDebugAsk = 0;    // asks (wraps)
static EWRAM_DATA u8 sDebugAskDelay = 0; // DEBUG's ask waits this many free frames more: the menu's own B presses
                                         // would otherwise answer it
#endif

//  Its mouth: the top of its picture is its head; the leftmost pixel of the head's band (the lowest such row on a
//  tie, so a beak's tip and not the brow above it) is where its mouth is, facing left. 32x32 at 4bpp, tiles row-major.
static void FindMouth(const u8 *pic, s8 *mouthX, s8 *mouthY)
{
    s16 x, y, top = -1, bestX = 32, bestY = 12;

    for (y = 0; y < 32; y++)
    {
        for (x = 0; x < 32; x++)
        {
            u8 byte = pic[((y / 8) * 4 + x / 8) * 32 + (y % 8) * 4 + (x % 8) / 2];

            if ((x & 1) ? (byte >> 4) : (byte & 0xF))
            {
                if (top < 0)
                    top = y;
                if (y <= top + 7 && x <= bestX)
                {
                    bestX = x;
                    bestY = y;
                }
                break;   // the row's leftmost pixel is all this needs
            }
        }
    }
    if (top < 0)
        bestX = 8;
    *mouthX = bestX - 16;
    *mouthY = bestY + 1 - 16;   // just under the snout's edge
}

static u8 FreeSpritePaletteSlots(void)
{
    u8 i, n = 0;

    for (i = gReservedSpritePaletteCount; i < 16; i++)
        if (GetSpritePaletteTagByPaletteNum(i) == TAG_NONE)
            n++;
    return n;
}

//  Its palette, untinted (they shine, as PLAY's notes do) -- and never the last slot free: that is left to whoever
//  comes next (a field effect, a person), and without it the follower does not hum.
static u8 LoadHumPalette(void)
{
    u8 idx = IndexOfSpritePaletteTag(HUM_TAG);

    if (idx != 0xFF)
        return idx;
    if (FreeSpritePaletteSlots() < 2)
        return 0xFF;
    idx = AllocSpritePalette(HUM_TAG);
    if (idx == 0xFF)
        return 0xFF;
    LoadPalette(sKeyColoursPal, OBJ_PLTT_ID(idx), PLTT_SIZE_4BPP);
    return idx;
}

u8 Follower_HumPaletteSlot(void)
{
    return IndexOfSpritePaletteTag(HUM_TAG);
}

static bool8 HumNotesShowing(void)
{
    u8 i;

    for (i = 0; i < MAX_SPRITES; i++)
        if (gSprites[i].inUse && gSprites[i].callback == SpriteCB_HumNote)
            return TRUE;
    return FALSE;
}

static bool8 FollowerSings(void)
{
    if (sFollower.singing && !DaemonSong_IsPlaying())
        sFollower.singing = FALSE;
    return sFollower.singing || DaemonSong_IsHumming();
}

//  Once a frame, before anything else: a hum not let go on last frame stops; the palette goes back with its last note;
//  and while it shows, its colours are put back over whatever tinted every palette this frame (the weather's fade).
static void HumTidy(void)
{
    u8 idx;

    if (DaemonSong_IsHumming() && !sFollower.humTicked)
        DaemonSong_Stop();
    sFollower.humTicked = FALSE;
    idx = IndexOfSpritePaletteTag(HUM_TAG);
    if (idx == 0xFF)
        return;
    if (!FollowerSings() && !HumNotesShowing())
    {
        FreeSpritePaletteByTag(HUM_TAG);
        return;
    }
    if (!gPaletteFade.active)
        CpuCopy16(sKeyColoursPal, &gPlttBufferFaded[OBJ_PLTT_ID(idx)], PLTT_SIZE_4BPP);
}

#define tAge     data[0]
#define tPitch   data[1]   // its true pitch class, 0 C .. 11 B
#define tNear    data[2]   // how near it is to learned, 0..255, or -1 learned
#define tSerial  data[3]   // the note it is (DaemonSong_Heard), while it sounds
#define tDrift   data[4]   // -1 left, 1 right
#define tRng     data[5]
#define tSnow    data[6]   // the static frame shown last

static u8 PitchClass(s16 semitone)
{
    return ((semitone % 12) + 12) % 12;
}

static void SpriteCB_HumNote(struct Sprite *sprite)
{
    u16 serial;
    s16 heard, truePitch, near;
    bool8 trueNow;

    if (++sprite->tAge >= NOTE_LIFE)
    {
        DestroySprite(sprite);   // the palette goes back in HumTidy, when it was the last
        return;
    }
    // It rises a pixel every three frames, and drifts the way the daemon faced, a pixel every eight, with a sway.
    sprite->y2 = -(sprite->tAge / 3);
    sprite->x2 = sprite->tDrift * (sprite->tAge / 8) + ((sprite->tAge / 12) & 1);
    sprite->invisible = sprite->tAge > NOTE_LIFE - 12 && (sprite->tAge & 2);   // it blinks out

    if (sprite->tNear < 0)
        return;   // learned: its colour, as it was made
    if (DaemonSong_Heard(&serial, &heard, &truePitch, &near) && serial == sprite->tSerial)
    {
        trueNow = (heard == truePitch);   // the very flicker the audio has, this frame
    }
    else
    {
        if (sprite->tAge % NOTE_FLICKER != 0)
            return;
        sprite->tRng = (u16)sprite->tRng * 25173 + 13849;
        trueNow = ((u16)sprite->tRng >> 8) < 64 + 191 * sprite->tNear / 255;   // daemon_song.c's odds
    }
    if (trueNow)
    {
        StartSpriteAnimIfDifferent(sprite, sprite->tPitch);
    }
    else
    {
        if (sprite->tAge % NOTE_FLICKER == 0)
            sprite->tSnow = NOTE_STATIC + (sprite->tSnow + 1 - NOTE_STATIC) % 3;
        StartSpriteAnimIfDifferent(sprite, sprite->tSnow);
    }
}

//  A note from its mouth, for the note just begun (the follower's, or T-598's summoned daemon's).
static void HumNoteFrom(struct Sprite *follower, u8 facing, s8 mouthX, s8 mouthY, s16 truePitch, s16 near, u16 serial)
{
    u8 spriteId;
    s16 dir = facing == DIR_EAST ? 1 : -1;
    s16 x, y;

    if (LoadHumPalette() == 0xFF)
        return;
    x = follower->x + (dir > 0 ? -mouthX - 1 : mouthX) + dir * 3;
    y = follower->y + follower->y2 + mouthY - 3;
    spriteId = CreateSprite(&sHumNoteTemplate, x, y, 1);
    if (spriteId == MAX_SPRITES)
        return;
    gSprites[spriteId].coordOffsetEnabled = TRUE;   // it stays where it was sung: the map scrolls under it
    gSprites[spriteId].oam.priority = follower->oam.priority;
    gSprites[spriteId].tPitch = PitchClass(truePitch);
    gSprites[spriteId].tNear = near;
    gSprites[spriteId].tSerial = serial;
    gSprites[spriteId].tDrift = dir;
    gSprites[spriteId].tRng = serial * 31 + truePitch;
    gSprites[spriteId].tSnow = NOTE_STATIC;
    StartSpriteAnim(&gSprites[spriteId], near < 0 ? gSprites[spriteId].tPitch : NOTE_STATIC);
#if DAEMONS_DEBUG
    gFollowerDebugNotes++;
#endif
}

#undef tAge
#undef tPitch
#undef tNear
#undef tSerial
#undef tDrift
#undef tRng
#undef tSnow

//  A note begun since last frame: draw it.
static void DrawHeard(struct Sprite *follower)
{
    u16 serial;
    s16 heard, truePitch, near;

    if (!DaemonSong_Heard(&serial, &heard, &truePitch, &near) || serial == sFollower.heardSerial)
        return;
    sFollower.heardSerial = serial;
    if (!follower->invisible)
        HumNoteFrom(follower, sFollower.facing, sFollower.mouthX, sFollower.mouthY, truePitch, near, serial);
}

static bool8 MayHum(struct Sprite *sprite)
{
    return !sprite->invisible && !sFollower.sceneHidden && !ArePlayerFieldControlsLocked() && !ScriptContext_IsEnabled()
        && IsFieldMessageBoxHidden() && !gPaletteFade.active && !Play_IsActive() && gQuestLogState != QL_STATE_PLAYBACK;
}

//  How long until the next: from HUM_SLOWEST at the wish's start to HUM_FASTEST when perfect, give or take a quarter.
static u16 HumInterval(u8 wish)
{
    u16 base = HUM_SLOWEST - (HUM_SLOWEST - HUM_FASTEST) * wish / 255;

    return base - base / 4 + Random() % (base / 2 + 1);
}

static void Hum(struct Sprite *sprite)
{
    s8 slot;
    struct Pokemon *mon;

    if (sFollower.singing && DaemonSong_IsPlaying())
    {
        DrawHeard(sprite);   // asked: it sings through the script that asked it
        return;
    }
    sFollower.singing = FALSE;
    if (DaemonSong_IsHumming())
    {
        if (!MayHum(sprite))
            return;              // not ticked: HumTidy stops it next frame
        sFollower.humTicked = TRUE;
        DrawHeard(sprite);
        return;
    }
    if (sFollower.wish == 0 || !MayHum(sprite) || DaemonSong_IsPlaying())
        return;                  // the wait runs only while it could hum: never straight out of a menu
    if (sFollower.humWait != 0)
    {
        sFollower.humWait--;
        return;
    }
    slot = Follower_LeadSlot();
    sFollower.humWait = HumInterval(sFollower.wish);
    if (slot < 0 || LoadHumPalette() == 0xFF)
        return;
    mon = &gPlayerParty[slot];
    sFollower.heardSerial = 0xFFFF;
    if (DaemonSong_Hum(GetMonData(mon, MON_DATA_SPECIES), DaemonSong_MonShare(mon), Random(), DaemonSong_MonMode(mon) != 0))
    {
        sFollower.humTicked = TRUE;
#if DAEMONS_DEBUG
        gFollowerDebugHum++;
#endif
    }
}

//  After a battle won (battle_setup.c): it may ask, the nearer its evolution the likelier.
void Follower_AfterBattle(void)
{
    s8 slot = Follower_LeadSlot();
    u8 wish;

    sFollower.askWait = 0;
    if (slot < 0)
        return;
    wish = DaemonSong_Wish(&gPlayerParty[slot]);
    if (wish != 0 && (Random() & 0xFF) < 64 + 176 * wish / 255)
        sFollower.askWait = ASK_WINDOW;
}

static void AskToSing(struct Sprite *sprite)
{
    if (sFollower.askWait == 0)
        return;
    sFollower.askWait--;
    if (sFollower.wish == 0)
        sFollower.askWait = 0;   // it evolved, or the lead changed: the wish was another's
    if (sFollower.askWait == 0 || sFollower.idle < 2 || !MayHum(sprite))
    {
        sFollower.askBeat = 0;   // a beat free and still, counted again from the start
        return;
    }
#if DAEMONS_DEBUG
    if (sDebugAskDelay != 0)
    {
        sDebugAskDelay--;
        return;
    }
#endif
    if (++sFollower.askBeat < ASK_STILL)
        return;
    sFollower.askWait = 0;
    sFollower.askBeat = 0;
    DaemonSong_Stop();
    LockPlayerFieldControls();
    ScriptContext_SetupScript(EventScript_FollowerWantsToSing);
#if DAEMONS_DEBUG
    gFollowerDebugAsk++;
#endif
}

//  The ask's specials (data/scripts/follower.inc). Its name for the question, and it turns to the player.
void Follower_BufferSinger(void)
{
    s8 slot = Follower_LeadSlot();

    gStringVar1[0] = EOS;
    if (slot < 0)
        return;
    GetMonData(&gPlayerParty[slot], MON_DATA_NICKNAME, gStringVar1);
    StringGet_Nickname(gStringVar1);
    if (sFollower.sumX > 0)
        sFollower.facing = DIR_EAST;
    else if (sFollower.sumX < 0)
        sFollower.facing = DIR_WEST;
}

static void Task_FollowerSinging(u8 taskId)
{
    if (JOY_NEW(B_BUTTON))
        DaemonSong_Stop();
    if (!DaemonSong_IsPlaying())
    {
        sFollower.singing = FALSE;
        DestroyTask(taskId);
        ScriptContext_Enable();
    }
}

//  Yes: its song as it knows it, as SING plays it (party_menu.c), its notes rising from it. waitstate; B stops it.
void Follower_SingAsked(void)
{
    s8 slot = Follower_LeadSlot();
    struct Pokemon *mon;

    if (slot >= 0)
    {
        mon = &gPlayerParty[slot];
        LoadHumPalette();   // without a slot it still sings; only the notes are not drawn
        sFollower.heardSerial = 0xFFFF;
        if (DaemonSong_Start(GetMonData(mon, MON_DATA_SPECIES), DaemonSong_MonShare(mon), SONG_LINE | DaemonSong_MonMode(mon)))
        {
            sFollower.singing = TRUE;
            CreateTask(Task_FollowerSinging, 80);
            return;
        }
    }
    ScriptContext_Enable();
}

#if DAEMONS_DEBUG
//  DEBUG -> FOLLOWER: the lead a few notes short of its next form (its own part 240/255 learned), and a hum soon.
void Follower_DebugNearSong(void)
{
    s8 slot = Follower_LeadSlot();
    struct Pokemon *mon;
    u32 exp;

    if (slot < 0)
        return;
    mon = &gPlayerParty[slot];
    exp = DaemonSong_ExpAtShare(GetMonData(mon, MON_DATA_SPECIES), 240);
    if (exp == 0)
        return;
    SetMonData(mon, MON_DATA_EXP, &exp);
    CalculateMonStats(mon);
    sFollower.wish = DaemonSong_Wish(mon);
    sFollower.humWait = 60;
}

//  DEBUG -> FOLLOWER: as if a battle had just been won and the wish come up -- it asks once back in the field.
void Follower_DebugAskToSing(void)
{
    sFollower.askWait = ASK_WINDOW;
    sDebugAskDelay = 180;
}
#endif

//  ------------------------------------------------------------------- T-598: a daemon that comes to its song
//  PLAY's freestyle heard a daemon's opening (DaemonSong_Listen) and EventScript_DaemonSummoned runs. It is drawn as the
//  follower is -- its own picture, its own colours, whole (it knows its own part), a sprite and not an object -- and is
//  never anything the player can catch or fight. It appears two tiles off (ahead first, then either side, then behind:
//  ground the player could stand on, nobody there, not the follower's tile; one tile off if two will not do), flickering
//  in, turns to the player and sings its own part whole, its notes rising from its mouth as the follower's do. Then the
//  script says its QUOTATION, if it has one, and it walks on away from the player, flickering out. DRAFT.
#define SUMMON_TAG    0x541C   // its picture and its palette, only while it is here
#define SUMMON_FADE   48       // frames it flickers in, and out
#define SUMMON_LOOK   20       // frames it looks at the player before it sings, and after
#define SUMMON_HOP    6        // frames it lifts a pixel at each note

static EWRAM_DATA struct
{
    u16 species;
    u16 heardSerial;
    u8 spriteId;
    u8 facing;
    s8 dx, dy;                 // where it stands, in tiles from the player (and the way it leaves)
    s8 mouthX, mouthY;
} sSummon = {0};

static const struct SpriteTemplate sSpriteTemplate_Summon = {
    .tileTag = SUMMON_TAG,
    .paletteTag = SUMMON_TAG,
    .oam = &sOam_Follower,
    .anims = gDummySpriteAnimTable,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCB_Follower,
};

static bool8 SummonMayStand(s16 x, s16 y)
{
    s32 i;

    if (MapGridGetCollisionAt(x, y) || MetatileBehavior_IsSurfable(MapGridGetMetatileBehaviorAt(x, y)))
        return FALSE;
    if (sFollower.spriteId != MAX_SPRITES && IsOurs() && !gSprites[sFollower.spriteId].invisible
     && x == sFollower.tileX && y == sFollower.tileY)
        return FALSE;
    for (i = 0; i < OBJECT_EVENTS_COUNT; i++)
        if (gObjectEvents[i].active && !gObjectEvents[i].invisible
         && gObjectEvents[i].currentCoords.x == x && gObjectEvents[i].currentCoords.y == y)
            return FALSE;
    return TRUE;
}

//  Where it appears: TRUE with sSummon.dx, dy set
static bool8 SummonFindPlace(struct ObjectEvent *playerObj)
{
    static const s8 sDirX[] = {0, 0, 0, -1, 1}, sDirY[] = {0, 1, -1, 0, 0};   // DIR_SOUTH, NORTH, WEST, EAST
    u8 order[4], dist, i, d;
    s16 px = playerObj->currentCoords.x, py = playerObj->currentCoords.y;

    d = playerObj->facingDirection;
    if (d < DIR_SOUTH || d > DIR_EAST)
        d = DIR_SOUTH;
    order[0] = d;                                                       // ahead
    order[1] = (d == DIR_SOUTH || d == DIR_NORTH) ? DIR_WEST : DIR_NORTH;   // either side
    order[2] = (d == DIR_SOUTH || d == DIR_NORTH) ? DIR_EAST : DIR_SOUTH;
    order[3] = GetOppositeDirection(d);                                 // behind
    for (dist = 2; dist >= 1; dist--)
    {
        for (i = 0; i < 4; i++)
        {
            s8 ux = sDirX[order[i]], uy = sDirY[order[i]];

            if (!SummonMayStand(px + ux * dist, py + uy * dist) || (dist == 2 && !SummonMayStand(px + ux, py + uy)))
                continue;
            sSummon.dx = ux * dist;
            sSummon.dy = uy * dist;
            return TRUE;
        }
    }
    return FALSE;
}

static void SummonFree(void)
{
    if (sSummon.spriteId != MAX_SPRITES)
        DestroySprite(&gSprites[sSummon.spriteId]);
    sSummon.spriteId = MAX_SPRITES;
    FreeSpriteTilesByTag(SUMMON_TAG);
    FreeSpritePaletteByTag(SUMMON_TAG);
}

static void SummonSpawn(void)
{
    struct ObjectEvent *playerObj = &gObjectEvents[gPlayerAvatar.objectEventId];
    struct Sprite *player = &gSprites[gPlayerAvatar.spriteId];
    struct SpriteSheet sheet;
    struct Sprite *sprite;
    u16 shown = GetIconSpecies(sSummon.species, 0);
    u8 idx;

    sSummon.spriteId = MAX_SPRITES;
    if (!SummonFindPlace(playerObj))
        return;   // nowhere to stand: it is heard and not seen
    FreeSpriteTilesByTag(SUMMON_TAG);
    sheet.data = FollowerPic(shown);
    sheet.size = 0x200;
    sheet.tag = SUMMON_TAG;
    LoadSpriteSheet(&sheet);
    idx = IndexOfSpritePaletteTag(SUMMON_TAG);
    if (idx == 0xFF)
        idx = AllocSpritePalette(SUMMON_TAG);
    if (GetSpriteTileStartByTag(SUMMON_TAG) == 0xFFFF || idx == 0xFF)
    {
        SummonFree();
        return;
    }
    LoadPalette(sFollowerPalettes[shown < ARRAY_COUNT(sFollowerPalettes) ? shown : SPECIES_NONE], OBJ_PLTT_ID(idx),
                PLTT_SIZE_4BPP);   // whole: a daemon that knows its own song is in all its colours (T-597)
    ApplyGlobalFieldPaletteTint(idx);
    UpdateSpritePaletteWithWeather(idx);
    sSummon.spriteId = CreateSprite(&sSpriteTemplate_Summon, player->x + sSummon.dx * 16, player->y + sSummon.dy * 16, 0);
    if (sSummon.spriteId == MAX_SPRITES)
    {
        SummonFree();
        return;
    }
    sprite = &gSprites[sSummon.spriteId];
    sprite->coordOffsetEnabled = TRUE;
    sprite->animPaused = TRUE;
    sprite->invisible = TRUE;
    sprite->oam.paletteNum = idx;
    SetObjectSubpriorityByElevation(playerObj->previousElevation, sprite, 1);
    sprite->y += FOLLOWER_Y;
    sprite->oam.priority = player->oam.priority;
    sSummon.facing = sSummon.dx < 0 ? DIR_EAST : DIR_WEST;   // to the player: icons face left
    sprite->oam.matrixNum = sSummon.facing == DIR_EAST ? ST_OAM_HFLIP : 0;
    FindMouth(sheet.data, &sSummon.mouthX, &sSummon.mouthY);
}

#define tPhase  data[0]
#define tTimer  data[1]
#define tHop    data[2]
#define tCried  data[3]

static void Task_SummonArrive(u8 taskId)
{
    s16 *data = gTasks[taskId].data;
    struct Sprite *sprite = sSummon.spriteId != MAX_SPRITES ? &gSprites[sSummon.spriteId] : NULL;
    const u8 *quotation;
    u16 serial;
    s16 heard, truePitch, near;

    tTimer++;
    switch (tPhase)
    {
    case 0:   // it flickers in, settling a few pixels
        if (sprite != NULL)
        {
            sprite->invisible = (tTimer % 8) >= tTimer * 8 / SUMMON_FADE;
            sprite->y2 = -(SUMMON_FADE - tTimer) / 8;
        }
        if (tTimer >= SUMMON_FADE)
        {
            if (sprite != NULL)
            {
                sprite->invisible = FALSE;
                sprite->y2 = 0;
            }
            tPhase++;
            tTimer = 0;
        }
        break;
    case 1:   // it looks at the player, and sings
        if (tTimer < SUMMON_LOOK)
            break;
        sSummon.heardSerial = 0xFFFF;
        if (!DaemonSong_StartSummoned(sSummon.species))   // T-613: its whole song, as it knows it
        {
            PlayCry_Normal(sSummon.species, 0);
            tCried = TRUE;
        }
        tPhase++;
        tTimer = 0;
        break;
    case 2:   // its song, its notes rising from it; B cuts it short
        if (JOY_NEW(B_BUTTON))
            DaemonSong_Stop();
        if (DaemonSong_Heard(&serial, &heard, &truePitch, &near) && serial != sSummon.heardSerial)
        {
            sSummon.heardSerial = serial;
            tHop = SUMMON_HOP;
            if (sprite != NULL)
                HumNoteFrom(sprite, sSummon.facing, sSummon.mouthX, sSummon.mouthY, truePitch, near, serial);
        }
        if (tHop != 0)
            tHop--;
        if (sprite != NULL)
            sprite->y2 = tHop != 0 ? -1 : 0;
        if (DaemonSong_IsPlaying() || (tCried && IsCryPlaying()))
            break;
        tPhase++;
        tTimer = 0;
        break;
    default:   // a breath, and the script says its QUOTATION, if it has one
        if (tTimer < SUMMON_LOOK)
            break;
        quotation = DaemonSong_Quotation(sSummon.species);
        gSpecialVar_Result = quotation != NULL;
        if (quotation != NULL)
        {
            StringCopy(gStringVar1, gSpeciesNames[sSummon.species]);
            StringExpandPlaceholders(gStringVar4, quotation);
        }
        DestroyTask(taskId);
        ScriptContext_Enable();
        break;
    }
}

void Follower_SummonArrive(void)
{
    u8 taskId;

    sSummon.species = gSpecialVar_0x8004;
    SummonSpawn();
    taskId = CreateTask(Task_SummonArrive, 80);
    gTasks[taskId].tPhase = 0;
    gTasks[taskId].tTimer = 0;
    gTasks[taskId].tHop = 0;
    gTasks[taskId].tCried = FALSE;
}

//  It goes on its way: on away from the player, a pixel a frame with a step's hop, flickering out as it goes.
static void Task_SummonLeave(u8 taskId)
{
    s16 *data = gTasks[taskId].data;
    struct Sprite *sprite;

    if (sSummon.spriteId == MAX_SPRITES || ++tTimer > SUMMON_FADE)
    {
        SummonFree();
        DestroyTask(taskId);
        ScriptContext_Enable();
        return;
    }
    sprite = &gSprites[sSummon.spriteId];
    sprite->x += (sSummon.dx > 0) - (sSummon.dx < 0);
    sprite->y += (sSummon.dy > 0) - (sSummon.dy < 0);
    sprite->y2 = (tTimer % 16) >= 4 && (tTimer % 16) < 12 ? -1 : 0;
    sprite->invisible = (tTimer % 8) < tTimer * 8 / SUMMON_FADE;
}

void Follower_SummonLeave(void)
{
    u8 taskId = CreateTask(Task_SummonLeave, 80);

    gTasks[taskId].tTimer = 0;
}

#undef tPhase
#undef tTimer
#undef tHop
#undef tCried
