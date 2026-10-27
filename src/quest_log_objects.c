#include "global.h"
#include "quest_log.h"
#include "fieldmap.h"
#include "field_player_avatar.h"
#include "metatile_behavior.h"
#include "constants/event_objects.h"

//  T-461: the recap keeps an object's graphics id in a u8 (QuestLogObjectEvent has no spare bits; its three `unused`
//  bits hold data in real saves), so an id from OBJ_EVENT_GFX_WIDE_START (256) up is RECORDED as a stand-in: a
//  narrow id of the same shape and feel, which the recap replays in its place. One row per wide id, [id - 256];
//  a wide id with no row (or a row left 0) records OBJ_EVENT_GFX_QL_STAND_IN, a generic person. The real id is never
//  lost: the save's live objects and templates keep it, and the recap is only a memory.
#if NUM_OBJ_EVENT_GFX > OBJ_EVENT_GFX_WIDE_START
static const u8 sQuestLogGfxStandIns[NUM_OBJ_EVENT_GFX - OBJ_EVENT_GFX_WIDE_START] =
{
    // [OBJ_EVENT_GFX_<NEW> - OBJ_EVENT_GFX_WIDE_START] = OBJ_EVENT_GFX_<a like-shaped narrow id>,
};
#endif

static u8 QL_GfxStandIn(u16 graphicsId)
{
    if (graphicsId < OBJ_EVENT_GFX_WIDE_START)
        return graphicsId;
#if NUM_OBJ_EVENT_GFX > OBJ_EVENT_GFX_WIDE_START
    if (graphicsId < NUM_OBJ_EVENT_GFX && sQuestLogGfxStandIns[graphicsId - OBJ_EVENT_GFX_WIDE_START] != 0)
        return sQuestLogGfxStandIns[graphicsId - OBJ_EVENT_GFX_WIDE_START];
#endif
    return OBJ_EVENT_GFX_QL_STAND_IN;
}

void QL_RecordObjects(struct QuestLogScene * questLog)
{
    u32 i;

    for (i = 0; i < OBJECT_EVENTS_COUNT; i++)
    {
        questLog->objectEvents[i].active                         = gObjectEvents[i].active;
        questLog->objectEvents[i].triggerGroundEffectsOnStop     = gObjectEvents[i].triggerGroundEffectsOnStop;
        questLog->objectEvents[i].disableCoveringGroundEffects   = gObjectEvents[i].disableCoveringGroundEffects;
        questLog->objectEvents[i].landingJump                    = gObjectEvents[i].landingJump;
        questLog->objectEvents[i].frozen                         = gObjectEvents[i].frozen;
        questLog->objectEvents[i].facingDirectionLocked          = gObjectEvents[i].facingDirectionLocked;
        questLog->objectEvents[i].disableAnim                    = gObjectEvents[i].disableAnim;
        questLog->objectEvents[i].enableAnim                     = gObjectEvents[i].enableAnim;
        questLog->objectEvents[i].inanimate                      = gObjectEvents[i].inanimate;
        questLog->objectEvents[i].invisible                      = gObjectEvents[i].invisible;
        questLog->objectEvents[i].offScreen                      = gObjectEvents[i].offScreen;
        questLog->objectEvents[i].trackedByCamera                = gObjectEvents[i].trackedByCamera;
        questLog->objectEvents[i].isPlayer                       = gObjectEvents[i].isPlayer;
        questLog->objectEvents[i].spriteAnimPausedBackup         = gObjectEvents[i].spriteAnimPausedBackup;
        questLog->objectEvents[i].spriteAffineAnimPausedBackup   = gObjectEvents[i].spriteAffineAnimPausedBackup;
        questLog->objectEvents[i].disableJumpLandingGroundEffect = gObjectEvents[i].disableJumpLandingGroundEffect;
        questLog->objectEvents[i].fixedPriority                  = gObjectEvents[i].fixedPriority;
        questLog->objectEvents[i].facingDirection                = gObjectEvents[i].facingDirection;
        questLog->objectEvents[i].currentElevation               = gObjectEvents[i].currentElevation;
        questLog->objectEvents[i].previousElevation              = gObjectEvents[i].previousElevation;
        questLog->objectEvents[i].graphicsId                     = QL_GfxStandIn(OBJ_GFX_ID(&gObjectEvents[i]));   // T-461
        questLog->objectEvents[i].movementType                   = gObjectEvents[i].movementType;
        questLog->objectEvents[i].trainerType                    = gObjectEvents[i].trainerType;
        questLog->objectEvents[i].localId                        = gObjectEvents[i].localId;
        questLog->objectEvents[i].mapNum                         = gObjectEvents[i].mapNum;
        questLog->objectEvents[i].mapGroup                       = gObjectEvents[i].mapGroup;
        questLog->objectEvents[i].x                              = gObjectEvents[i].currentCoords.x;
        questLog->objectEvents[i].y                              = gObjectEvents[i].currentCoords.y;
        questLog->objectEvents[i].trainerRange_berryTreeId       = gObjectEvents[i].trainerRange_berryTreeId;
        questLog->objectEvents[i].previousMetatileBehavior       = gObjectEvents[i].previousMetatileBehavior;
        questLog->objectEvents[i].directionSequenceIndex         = gObjectEvents[i].directionSequenceIndex;
        questLog->objectEvents[i].animId                         = gObjectEvents[i].playerCopyableMovement;
    }
}

void QL_LoadObjects(const struct QuestLogScene * questLog, const struct ObjectEventTemplate * templates)
{
    u32 i, j;
    const struct QuestLogObjectEvent * questLogObjectEvents = questLog->objectEvents;

    CpuFill16(0, gObjectEvents, sizeof(gObjectEvents));

    for (i = 0; i < OBJECT_EVENTS_COUNT; i++)
    {
        gObjectEvents[i].active                         = questLogObjectEvents[i].active;
        gObjectEvents[i].triggerGroundEffectsOnStop     = questLogObjectEvents[i].triggerGroundEffectsOnStop;
        gObjectEvents[i].disableCoveringGroundEffects   = questLogObjectEvents[i].disableCoveringGroundEffects;
        gObjectEvents[i].landingJump                    = questLogObjectEvents[i].landingJump;
        gObjectEvents[i].frozen                         = questLogObjectEvents[i].frozen;
        gObjectEvents[i].facingDirectionLocked          = questLogObjectEvents[i].facingDirectionLocked;
        gObjectEvents[i].disableAnim                    = questLogObjectEvents[i].disableAnim;
        gObjectEvents[i].enableAnim                     = questLogObjectEvents[i].enableAnim;
        gObjectEvents[i].inanimate                      = questLogObjectEvents[i].inanimate;
        gObjectEvents[i].invisible                      = questLogObjectEvents[i].invisible;
        gObjectEvents[i].offScreen                      = questLogObjectEvents[i].offScreen;
        gObjectEvents[i].trackedByCamera                = questLogObjectEvents[i].trackedByCamera;
        gObjectEvents[i].isPlayer                       = questLogObjectEvents[i].isPlayer;
        gObjectEvents[i].spriteAnimPausedBackup         = questLogObjectEvents[i].spriteAnimPausedBackup;
        gObjectEvents[i].spriteAffineAnimPausedBackup   = questLogObjectEvents[i].spriteAffineAnimPausedBackup;
        gObjectEvents[i].disableJumpLandingGroundEffect = questLogObjectEvents[i].disableJumpLandingGroundEffect;
        gObjectEvents[i].fixedPriority                  = questLogObjectEvents[i].fixedPriority;
        gObjectEvents[i].facingDirection                = questLogObjectEvents[i].facingDirection;
        gObjectEvents[i].currentElevation               = questLogObjectEvents[i].currentElevation;
        gObjectEvents[i].previousElevation              = questLogObjectEvents[i].previousElevation;
        SET_OBJ_GFX_ID(&gObjectEvents[i], questLogObjectEvents[i].graphicsId);   // T-461: always narrow here
        gObjectEvents[i].movementType                   = questLogObjectEvents[i].movementType;
        gObjectEvents[i].trainerType                    = questLogObjectEvents[i].trainerType;
        gObjectEvents[i].localId                        = questLogObjectEvents[i].localId;
        gObjectEvents[i].mapNum                         = questLogObjectEvents[i].mapNum;
        gObjectEvents[i].mapGroup                       = questLogObjectEvents[i].mapGroup;
        gObjectEvents[i].currentCoords.x                = questLogObjectEvents[i].x;
        gObjectEvents[i].currentCoords.y                = questLogObjectEvents[i].y;
        gObjectEvents[i].trainerRange_berryTreeId       = questLogObjectEvents[i].trainerRange_berryTreeId;
        gObjectEvents[i].previousMetatileBehavior       = questLogObjectEvents[i].previousMetatileBehavior;
        gObjectEvents[i].directionSequenceIndex         = questLogObjectEvents[i].directionSequenceIndex;
        gObjectEvents[i].playerCopyableMovement         = questLogObjectEvents[i].animId;

        for (j = 0; j < OBJECT_EVENT_TEMPLATES_COUNT; j++)
        {
            if (gObjectEvents[i].localId == templates[j].localId)
            {
                gObjectEvents[i].initialCoords.x = templates[j].x + MAP_OFFSET;
                gObjectEvents[i].initialCoords.y = templates[j].y + MAP_OFFSET;
                gObjectEvents[i].rangeX = templates[j].objUnion.normal.movementRangeX;
                gObjectEvents[i].rangeY = templates[j].objUnion.normal.movementRangeY;
            }
        }

        gObjectEvents[i].currentMetatileBehavior = MapGridGetMetatileBehaviorAt(gObjectEvents[i].currentCoords.x, gObjectEvents[i].currentCoords.y);
        if (gObjectEvents[i].previousMetatileBehavior == MapGridGetMetatileBehaviorAt((s16)(gObjectEvents[i].currentCoords.x), (s16)(gObjectEvents[i].currentCoords.y)))
        {
            gObjectEvents[i].previousCoords.x = gObjectEvents[i].currentCoords.x;
            gObjectEvents[i].previousCoords.y = gObjectEvents[i].currentCoords.y;
        }
        else if (gObjectEvents[i].previousMetatileBehavior == MapGridGetMetatileBehaviorAt((s16)(gObjectEvents[i].currentCoords.x - 1), (s16)(gObjectEvents[i].currentCoords.y)))
        {
            gObjectEvents[i].previousCoords.x = gObjectEvents[i].currentCoords.x - 1;
            gObjectEvents[i].previousCoords.y = gObjectEvents[i].currentCoords.y;
        }
        else if (gObjectEvents[i].previousMetatileBehavior == MapGridGetMetatileBehaviorAt((s16)(gObjectEvents[i].currentCoords.x + 1), (s16)(gObjectEvents[i].currentCoords.y)))
        {
            gObjectEvents[i].previousCoords.x = gObjectEvents[i].currentCoords.x + 1;
            gObjectEvents[i].previousCoords.y = gObjectEvents[i].currentCoords.y;
        }
        else if (gObjectEvents[i].previousMetatileBehavior == MapGridGetMetatileBehaviorAt((s16)(gObjectEvents[i].currentCoords.x), (s16)(gObjectEvents[i].currentCoords.y - 1)))
        {
            gObjectEvents[i].previousCoords.x = gObjectEvents[i].currentCoords.x;
            gObjectEvents[i].previousCoords.y = gObjectEvents[i].currentCoords.y - 1;
        }
        else if (gObjectEvents[i].previousMetatileBehavior == MapGridGetMetatileBehaviorAt((s16)(gObjectEvents[i].currentCoords.x), (s16)(gObjectEvents[i].currentCoords.y + 1)))
        {
            gObjectEvents[i].previousCoords.x = gObjectEvents[i].currentCoords.x;
            gObjectEvents[i].previousCoords.y = gObjectEvents[i].currentCoords.y + 1;
        }
    }

    CpuCopy16(gObjectEvents, gSaveBlock1Ptr->objectEvents, sizeof(gObjectEvents));
}

void QL_TryStopSurfing(void)
{
    if (gQuestLogState == QL_STATE_PLAYBACK)
    {
        s16 x, y;

        PlayerGetDestCoords(&x, &y);
        if (!MetatileBehavior_IsSurfable(MapGridGetMetatileBehaviorAt(x, y)) && TestPlayerAvatarFlags(PLAYER_AVATAR_FLAG_SURFING))
        {
            struct ObjectEvent * objectEvent = &gObjectEvents[gPlayerAvatar.objectEventId];
            SetPlayerAvatarTransitionFlags(PLAYER_AVATAR_FLAG_ON_FOOT);
            DestroySprite(&gSprites[objectEvent->fieldEffectSpriteId]);
        }
    }
}
