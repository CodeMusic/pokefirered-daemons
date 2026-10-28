#include "global.h"
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

//  T-18 P1: THE FOLLOWER -- it walks and hides (the user decided the plan 2026-10-20; every rule below is the plan's).
//
//  WHAT IT IS. The first party daemon that is not an egg, not HALTED (0 HP) and not AWAY, drawn at 1x one tile behind
//  the player as its FOLLOWER PICTURE: one 32x32 frame, the creature at most 20 tall and 22 wide on row 28
//  (src/data/follower_gfx.h, DAEMONS tools/gbafollower.py -- T-464: the party icons became 28px busts, and the
//  follower walked larger than the player's 16x22). A species with no picture there has an icon that already fits,
//  and walks as its icon's first frame. Its colours are its
//  TYPE's (src/data/follower_palettes.h, DAEMONS tools/gbafollowerpal.py: the icon's shared palette remapped to the
//  nearest colours of its front palette, so invariant 5 holds), greyed by friendship with T-424's own curve, then put
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
#define TRAIL            16       // pixels behind the player: one tile
#define FOLLOWER_Y       2        // its picture's lowest row is row 28 (gbafollower.py); this puts it on the player's feet
#define MAX_JUMP         32       // a move bigger than this in one frame is a warp, not a step
#define RETRY_FRAMES     30       // after a failed spawn (no palette slot, no sprite), try again this often
#define CHECK_FRAMES     8        // the lead and its friendship are read this often
#define SENSE_FIRST      180      // T-459: after it is made (a map, a battle, a menu), three seconds before it may notice
#define SENSE_STILL      45       // ... and it notices only once it has stood still this long
#define SENSE_AGAIN      1200     // ... and then not again for twenty seconds: now and then, never a beacon
#define SENSE_LOOK       60       // with nothing unfound in sight, it looks again a second later

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
} sFollower;

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
    u16 species = sFollower.species < ARRAY_COUNT(sFollowerPalettes) ? sFollower.species : SPECIES_NONE;
    const u16 *pal = sFollowerPalettes[species];
    u32 i;

    // T-18 P3: a shiny lead follows in its shiny colours (the user, 2026-10-20) -- the same remap, from shiny.pal.
    // The shiny set holds only the species whose shiny colours differ; every other shiny looks as its normal self.
    if (sFollower.shiny)
        for (i = 0; i < ARRAY_COUNT(sFollowerShinySpecies); i++)
            if (sFollowerShinySpecies[i] == species)
                pal = sFollowerShinyPalettes[i];
    CpuCopy16(pal, buf, PLTT_SIZE_4BPP);
    Saturation_ApplyToBuffer(buf, sFollower.friendship);   // T-424: greyed by friendship
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

// T-464: its own picture when its icon is too big to walk beside the player, else its icon's first frame.
static const u8 *FollowerPic(u16 species)
{
    if (species < ARRAY_COUNT(sFollowerPics) && sFollowerPics[species] != NULL)
        return sFollowerPics[species];
    return GetMonIconTiles(species, FALSE);
}

static void Spawn(s8 slot, struct Sprite *player, struct ObjectEvent *playerObj)
{
    struct SpriteSheet sheet;
    u8 idx;

    ReadLead(slot, &sFollower.species, &sFollower.personality, &sFollower.friendship, &sFollower.shiny);
    FreeSpriteTilesByTag(FOLLOWER_TAG);
    sheet.data = FollowerPic(sFollower.species);
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
    sFollower.senseWait = SENSE_FIRST;   // T-459: a moment in a new place (or back from a battle) before it notices
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
    if (species != sFollower.species || personality != sFollower.personality || shiny != sFollower.shiny)
    {
        Despawn();   // made again next frame, from the same path
        return;
    }
    if (friendship != sFollower.friendship)
    {
        sFollower.friendship = friendship;
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
        if (gSprites[i].inUse && (gSprites[i].callback == SpriteCB_SenseRipple || gSprites[i].callback == SpriteCB_SenseGlint))
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
    if (FollowerIsAbsent())
    {
        if (IsOurs())
            Despawn();
        sFollower.spriteId = MAX_SPRITES;
        sFollower.restValid = FALSE;
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
    Sense(sprite, playerObj);   // T-459
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
    Saturation_ApplyToBuffer(buf, sFollower.friendship);   // T-424: greyed by friendship, as the follower is
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
