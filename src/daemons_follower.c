#include "global.h"
#include "gflib.h"
#include "event_data.h"
#include "event_object_movement.h"
#include "field_effect.h"
#include "field_weather.h"
#include "overworld.h"
#include "pokemon.h"
#include "pokemon_icon.h"
#include "quest_log.h"
#include "script.h"
#include "daemon_saturation.h"
#include "daemons_follower.h"
#include "constants/event_object_movement.h"
#include "constants/quest_log.h"
#include "data/follower_palettes.h"

//  T-18 P1: THE FOLLOWER -- it walks and hides (the user decided the plan 2026-10-20; every rule below is the plan's).
//
//  WHAT IT IS. The first party daemon that is not an egg, not HALTED (0 HP) and not AWAY, drawn as its own party icon
//  (no new art: graphics/pokemon/*/icon.png, two 32x32 frames) at 1x, one tile behind the player. Its colours are its
//  TYPE's (src/data/follower_palettes.h, DAEMONS tools/gbafollowerpal.py: the icon's shared palette remapped to the
//  nearest colours of its front palette, so invariant 5 holds), greyed by friendship with T-424's own curve, then put
//  in the field's light and weather as every person is. Icons face left; facing right is the same frame flipped. A
//  1 px hop in the middle of each tile it walks is its step (387 of 396 icons have two identical frames).
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
#define TRAIL            16       // pixels behind the player: one tile
#define FOLLOWER_Y       2        // the icon's body ends two rows above its cell; this puts it on the ground
#define MAX_JUMP         32       // a move bigger than this in one frame is a warp, not a step
#define RETRY_FRAMES     30       // after a failed spawn (no palette slot, no sprite), try again this often
#define CHECK_FRAMES     8        // the lead and its friendship are read this often

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

    CpuCopy16(sFollowerPalettes[species], buf, PLTT_SIZE_4BPP);
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

static void Despawn(void)
{
    if (IsOurs())
        DestroySprite(&gSprites[sFollower.spriteId]);
    sFollower.spriteId = MAX_SPRITES;
    FreeSpriteTilesByTag(FOLLOWER_TAG);
    FreeSpritePaletteByTag(FOLLOWER_TAG);
}

static void ReadLead(s8 slot, u16 *species, u32 *personality, u8 *friendship)
{
    struct Pokemon *mon = &gPlayerParty[slot];

    *personality = GetMonData(mon, MON_DATA_PERSONALITY);
    *species = GetIconSpecies(GetMonData(mon, MON_DATA_SPECIES), *personality);
    *friendship = GetMonData(mon, MON_DATA_FRIENDSHIP);
}

static void Spawn(s8 slot, struct Sprite *player, struct ObjectEvent *playerObj)
{
    struct SpriteSheet sheet;
    u8 idx;

    ReadLead(slot, &sFollower.species, &sFollower.personality, &sFollower.friendship);
    FreeSpriteTilesByTag(FOLLOWER_TAG);
    sheet.data = GetMonIconTiles(sFollower.species, FALSE);
    sheet.size = 0x400;   // both frames
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
    if (sFollower.restValid
     && sFollower.restGroup == gSaveBlock1Ptr->location.mapGroup && sFollower.restNum == gSaveBlock1Ptr->location.mapNum
     && sFollower.restX == playerObj->currentCoords.x && sFollower.restY == playerObj->currentCoords.y)
        StandAt(sFollower.restDX, sFollower.restDY);
    else
        Snap();
}

//  The lead may have changed (a swap in the party, a HALT in battle) or its friendship moved.
static void CheckLead(s8 slot)
{
    u16 species;
    u32 personality;
    u8 friendship;
    u8 idx;

    ReadLead(slot, &species, &personality, &friendship);
    if (species != sFollower.species || personality != sFollower.personality)
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

void Follower_RetintPalette(void)
{
    if (IsOurs())
        LoadFollowerPalette();
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
    sprite->oam.tileNum = sprite->sheetTileStart + ((sFollower.steps / TRAIL) & 1) * 16;   // its two frames
    sprite->invisible = dist < TRAIL / 2;   // still inside the player: walking out of them

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
    if (FollowerIsHidden(player))
    {
        Snap();
        sprite->invisible = TRUE;
        return;
    }
    Place(sprite, player, playerObj);
}
