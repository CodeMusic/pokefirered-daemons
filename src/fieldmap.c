#include "global.h"
#include "book_reader.h"   // T-317: DaemonsUnderstandingsMissing
#include "gflib.h"
#include "overworld.h"
#include "script.h"
#include "new_menu_helpers.h"
#include "quest_log.h"
#include "fieldmap.h"
#include "event_data.h"
#include "constants/region_map_sections.h"
#include "constants/maps.h"
#include "daemons_time.h"
#include "daemons_holidays.h"
#include "constants/metatile_behaviors.h"
#include "data/day_trims.h"
#include "school_lift.h"
#include "play.h"
#include "battle.h"            // T-430: the player side's battlers
#include "battle_main.h"
#include "battle_anim.h"
#include "daemons_follower.h"
#include "event_object_movement.h"
#include "constants/flags.h"
#include "field_message_box.h"   // T-430: the first sight waits for no box

extern const struct Tileset gTileset_General;   // T-275: the CHECKPOINT's tileset
extern const struct Tileset gTileset_Nexus;     // T-395: THE NEXUS, made of paint
#include "overworld.h"

struct ConnectionFlags
{
    u8 south:1;
    u8 north:1;
    u8 west:1;
    u8 east:1;
};

COMMON_DATA struct BackupMapLayout VMap = {0};
EWRAM_DATA u16 gBackupMapData[VIRTUAL_MAP_SIZE] = {};
EWRAM_DATA struct MapHeader gMapHeader = {};
EWRAM_DATA struct Camera gCamera = {};
static EWRAM_DATA struct ConnectionFlags gMapConnectionFlags = {};
EWRAM_DATA u8 gGlobalFieldTintMode = QL_TINT_NONE;

static const struct ConnectionFlags sDummyConnectionFlags = {};

static void InitMapLayoutData(struct MapHeader *);
static void DaemonsScatterSpringFlowers(struct MapHeader *);
static void InitBackupMapLayoutData(const u16 *, u16, u16);
static void InitBackupMapLayoutConnections(struct MapHeader *);
static void FillSouthConnection(struct MapHeader const *, struct MapHeader const *, s32);
static void FillNorthConnection(struct MapHeader const *, struct MapHeader const *, s32);
static void FillWestConnection(struct MapHeader const *, struct MapHeader const *, s32);
static void FillEastConnection(struct MapHeader const *, struct MapHeader const *, s32);
static void LoadSavedMapView(void);
static const struct MapConnection *GetIncomingConnection(u8, s32, s32);
static bool8 IsPosInIncomingConnectingMap(u8, s32, s32, const struct MapConnection *);
static bool8 IsCoordInIncomingConnectingMap(s32, s32, s32, s32);
static u32 GetAttributeByMetatileIdAndMapLayout(const struct MapLayout *, u16, u8);

#define GetBorderBlockAt(x, y) ({                                                                 \
    u16 block;                                                                                    \
    s32 xprime;                                                                                   \
    s32 yprime;                                                                                   \
                                                                                                  \
    const struct MapLayout *mapLayout = gMapHeader.mapLayout;                                     \
                                                                                                  \
    xprime = x - MAP_OFFSET;                                                                      \
    xprime += 8 * mapLayout->borderWidth;                                                         \
    xprime %= mapLayout->borderWidth;                                                             \
                                                                                                  \
    yprime = y - MAP_OFFSET;                                                                      \
    yprime += 8 * mapLayout->borderHeight;                                                        \
    yprime %= mapLayout->borderHeight;                                                            \
                                                                                                  \
    block = mapLayout->border[xprime + yprime * mapLayout->borderWidth] | MAPGRID_COLLISION_MASK; \
})

#define AreCoordsWithinMapGridBounds(x, y) (x >= 0 && x < VMap.Xsize && y >= 0 && y < VMap.Ysize)

#define GetMapGridBlockAt(x, y) (AreCoordsWithinMapGridBounds(x, y) ? VMap.map[x + VMap.Xsize * y] : GetBorderBlockAt(x, y))

// Masks/shifts for metatile attributes
// This is the format of the data stored in each data/tilesets/*/*/metatile_attributes.bin file
static const u32 sMetatileAttrMasks[METATILE_ATTRIBUTE_COUNT] = {
    [METATILE_ATTRIBUTE_BEHAVIOR]       = 0x000001ff, // Bits 0-8
    [METATILE_ATTRIBUTE_TERRAIN]        = 0x00003e00, // Bits 9-13
    [METATILE_ATTRIBUTE_2]              = 0x0003c000, // Bits 14-17
    [METATILE_ATTRIBUTE_3]              = 0x00fc0000, // Bits 18-23
    [METATILE_ATTRIBUTE_ENCOUNTER_TYPE] = 0x07000000, // Bits 24-26
    [METATILE_ATTRIBUTE_5]              = 0x18000000, // Bits 27-28
    [METATILE_ATTRIBUTE_LAYER_TYPE]     = 0x60000000, // Bits 29-30
    [METATILE_ATTRIBUTE_7]              = 0x80000000  // Bit  31
};

static const u8 sMetatileAttrShifts[METATILE_ATTRIBUTE_COUNT] = {
    [METATILE_ATTRIBUTE_BEHAVIOR]       = 0,
    [METATILE_ATTRIBUTE_TERRAIN]        = 9,
    [METATILE_ATTRIBUTE_2]              = 14,
    [METATILE_ATTRIBUTE_3]              = 18,
    [METATILE_ATTRIBUTE_ENCOUNTER_TYPE] = 24,
    [METATILE_ATTRIBUTE_5]              = 27,
    [METATILE_ATTRIBUTE_LAYER_TYPE]     = 29,
    [METATILE_ATTRIBUTE_7]              = 31
};

const struct MapHeader * GetMapHeaderFromConnection(const struct MapConnection * connection)
{
    return Overworld_GetMapHeaderByGroupAndId(connection->mapGroup, connection->mapNum);
}

void InitMap(void)
{
    InitMapLayoutData(&gMapHeader);
    RunOnLoadMapScript();
}

void InitMapFromSavedGame(void)
{
    InitMapLayoutData(&gMapHeader);
    LoadSavedMapView();
    RunOnLoadMapScript();
}

static void InitMapLayoutData(struct MapHeader * mapHeader)
{
    const struct MapLayout * mapLayout = mapHeader->mapLayout;
    CpuFastFill16(MAPGRID_UNDEFINED, gBackupMapData, sizeof(gBackupMapData));
    VMap.map = gBackupMapData;
    VMap.Xsize = mapLayout->width + MAP_OFFSET_W;
    VMap.Ysize = mapLayout->height + MAP_OFFSET_H;
    AGB_ASSERT_EX(VMap.Xsize * VMap.Ysize <= VIRTUAL_MAP_SIZE, ABSPATH("fieldmap.c"), 158);
    InitBackupMapLayoutData(mapLayout->map, mapLayout->width, mapLayout->height);
    DaemonsScatterSpringFlowers(mapHeader);
    InitBackupMapLayoutConnections(mapHeader);
}

// T-359: SPRING'S FLOWERS -- "more flowers where flowers already grow". As the map's own cells are laid into the grid,
// a fixed scatter of its plain grass (about one cell in eight, the same cells every spring) becomes the flower block
// that is already drawn on it, walking exactly as grass does (MB_NORMAL); never tall grass, and never a cell a person,
// a sign, a warp or a trigger stands on. Only the grid in memory changes; nothing is saved. Seasons as above.
#define FLOWER_BLOCK 0x004
static const u16 sPlainGrass[] = { 0x001, 0x008, 0x009, 0x010, 0x011 };

static bool8 DaemonsEventAt(const struct MapEvents *ev, s32 x, s32 y)
{
    s32 i;

    for (i = 0; i < ev->objectEventCount; i++)
        if (ev->objectEvents[i].x == x && ev->objectEvents[i].y == y) return TRUE;
    for (i = 0; i < ev->warpCount; i++)
        if (ev->warps[i].x == x && ev->warps[i].y == y) return TRUE;
    for (i = 0; i < ev->coordEventCount; i++)
        if (ev->coordEvents[i].x == x && ev->coordEvents[i].y == y) return TRUE;
    for (i = 0; i < ev->bgEventCount; i++)
        if (ev->bgEvents[i].x == x && ev->bgEvents[i].y == y) return TRUE;
    return FALSE;
}

static void DaemonsScatterSpringFlowers(struct MapHeader *mapHeader)
{
    const struct MapLayout *layout = mapHeader->mapLayout;
    s32 x, y;
    u32 i;

    if (layout->primaryTileset != &gTileset_General || !IsMapTypeOutdoors(mapHeader->mapType)
     || DaemonsIsBlancheOutdoors() || DaemonsSeason() != SEASON_SPRING || mapHeader->events == NULL)
        return;
    for (y = 0; y < layout->height; y++)
    {
        for (x = 0; x < layout->width; x++)
        {
            u16 *cell = &VMap.map[(y + MAP_OFFSET) * VMap.Xsize + x + MAP_OFFSET];
            u16 id = *cell & MAPGRID_METATILE_ID_MASK;
            bool8 plain = FALSE;

            if ((((u32)x * 73856093u) ^ ((u32)y * 19349663u)) % 1000 >= 120)
                continue;
            for (i = 0; i < ARRAY_COUNT(sPlainGrass); i++)
                plain |= (id == sPlainGrass[i]);
            if (!plain || MapGridGetMetatileBehaviorAt(x + MAP_OFFSET, y + MAP_OFFSET) != MB_NORMAL
             || DaemonsEventAt(mapHeader->events, x, y))
                continue;
            *cell = (*cell & ~MAPGRID_METATILE_ID_MASK) | FLOWER_BLOCK;
        }
    }
}

static void InitBackupMapLayoutData(const u16 *map, u16 width, u16 height)
{
    s32 y;
    u16 *dest = VMap.map;
    dest += VMap.Xsize * 7 + MAP_OFFSET;

    for (y = 0; y < height; y++)
    {
        CpuCopy16(map, dest, width * sizeof(u16));
        dest += width + MAP_OFFSET_W;
        map += width;
    }
}

static void InitBackupMapLayoutConnections(struct MapHeader *mapHeader)
{
    s32 count;
    const struct MapConnection *connection;
    s32 i;

    gMapConnectionFlags = sDummyConnectionFlags;

    /*
     * This null pointer check is new to FireRed.  It was kept in
     * Emerald, with the above struct assignment moved to after
     * this check.
     */
    if (mapHeader->connections)
    {
        count = mapHeader->connections->count;
        connection = mapHeader->connections->connections;
        for (i = 0; i < count; i++, connection++)
        {
            struct MapHeader const *cMap = GetMapHeaderFromConnection(connection);
            u32 offset = connection->offset;
            switch (connection->direction)
            {
            case CONNECTION_SOUTH:
                FillSouthConnection(mapHeader, cMap, offset);
                gMapConnectionFlags.south = TRUE;
                break;
            case CONNECTION_NORTH:
                FillNorthConnection(mapHeader, cMap, offset);
                gMapConnectionFlags.north = TRUE;
                break;
            case CONNECTION_WEST:
                FillWestConnection(mapHeader, cMap, offset);
                gMapConnectionFlags.west = TRUE;
                break;
            case CONNECTION_EAST:
                FillEastConnection(mapHeader, cMap, offset);
                gMapConnectionFlags.east = TRUE;
                break;
            }
        }
    }
}

static void FillConnection(s32 x, s32 y, const struct MapHeader *connectedMapHeader, s32 x2, s32 y2, s32 width, s32 height)
{
    s32 i;
    const u16 *src;
    u16 *dest;
    s32 mapWidth;

    mapWidth = connectedMapHeader->mapLayout->width;
    src = &connectedMapHeader->mapLayout->map[mapWidth * y2 + x2];
    dest = &VMap.map[VMap.Xsize * y + x];

    for (i = 0; i < height; i++)
    {
        CpuCopy16(src, dest, width * 2);
        dest += VMap.Xsize;
        src += mapWidth;
    }
}

static void FillSouthConnection(struct MapHeader const *mapHeader, struct MapHeader const *connectedMapHeader, s32 offset)
{
    s32 x, y;
    s32 x2;
    s32 width;
    s32 cWidth;

    if (connectedMapHeader)
    {
        cWidth = connectedMapHeader->mapLayout->width;
        x = offset + MAP_OFFSET;
        y = mapHeader->mapLayout->height + MAP_OFFSET;
        if (x < 0)
        {
            x2 = -x;
            x += cWidth;
            if (x < VMap.Xsize)
                width = x;
            else
                width = VMap.Xsize;
            x = 0;
        }
        else
        {
            x2 = 0;
            if (x + cWidth < VMap.Xsize)
                width = cWidth;
            else
                width = VMap.Xsize - x;
        }

        FillConnection(
            x, y,
            connectedMapHeader,
            x2, /*y2*/ 0,
            width, /*height*/ MAP_OFFSET);
    }
}

static void FillNorthConnection(struct MapHeader const *mapHeader, struct MapHeader const *connectedMapHeader, s32 offset)
{
    s32 x;
    s32 x2, y2;
    s32 width;
    s32 cWidth, cHeight;

    if (connectedMapHeader)
    {
        cWidth = connectedMapHeader->mapLayout->width;
        cHeight = connectedMapHeader->mapLayout->height;
        x = offset + MAP_OFFSET;
        y2 = cHeight - MAP_OFFSET;
        if (x < 0)
        {
            x2 = -x;
            x += cWidth;
            if (x < VMap.Xsize)
                width = x;
            else
                width = VMap.Xsize;
            x = 0;
        }
        else
        {
            x2 = 0;
            if (x + cWidth < VMap.Xsize)
                width = cWidth;
            else
                width = VMap.Xsize - x;
        }

        FillConnection(
            x, /*y*/ 0,
            connectedMapHeader,
            x2, y2,
            width, /*height*/ MAP_OFFSET);

    }
}

static void FillWestConnection(struct MapHeader const *mapHeader, struct MapHeader const *connectedMapHeader, s32 offset)
{
    s32 y;
    s32 x2, y2;
    s32 height;
    s32 cWidth, cHeight;
    if (connectedMapHeader)
    {
        cWidth = connectedMapHeader->mapLayout->width;
        cHeight = connectedMapHeader->mapLayout->height;
        y = offset + MAP_OFFSET;
        x2 = cWidth - MAP_OFFSET;
        if (y < 0)
        {
            y2 = -y;
            if (y + cHeight < VMap.Ysize)
                height = y + cHeight;
            else
                height = VMap.Ysize;
            y = 0;
        }
        else
        {
            y2 = 0;
            if (y + cHeight < VMap.Ysize)
                height = cHeight;
            else
                height = VMap.Ysize - y;
        }

        FillConnection(
            /*x*/ 0, y,
            connectedMapHeader,
            x2, y2,
            /*width*/ MAP_OFFSET, height);
    }
}

static void FillEastConnection(struct MapHeader const *mapHeader, struct MapHeader const *connectedMapHeader, s32 offset)
{
    s32 x, y;
    s32 y2;
    s32 height;
    s32 cHeight;
    if (connectedMapHeader)
    {
        cHeight = connectedMapHeader->mapLayout->height;
        x = mapHeader->mapLayout->width + MAP_OFFSET;
        y = offset + MAP_OFFSET;
        if (y < 0)
        {
            y2 = -y;
            if (y + cHeight < VMap.Ysize)
                height = y + cHeight;
            else
                height = VMap.Ysize;
            y = 0;
        }
        else
        {
            y2 = 0;
            if (y + cHeight < VMap.Ysize)
                height = cHeight;
            else
                height = VMap.Ysize - y;
        }

        FillConnection(
            x, y,
            connectedMapHeader,
            /*x2*/ 0, y2,
            /*width*/ MAP_OFFSET + 1, height);
    }
}

u8 MapGridGetElevationAt(s32 x, s32 y)
{
    u16 block = GetMapGridBlockAt(x, y);

    if (block == MAPGRID_UNDEFINED)
        return 0;

    return block >> MAPGRID_ELEVATION_SHIFT;
}

u8 MapGridGetCollisionAt(s32 x, s32 y)
{
    u16 block = GetMapGridBlockAt(x, y);

    if (block == MAPGRID_UNDEFINED)
        return TRUE;

    return (block & MAPGRID_COLLISION_MASK) >> MAPGRID_COLLISION_SHIFT;
}

u32 MapGridGetMetatileIdAt(s32 x, s32 y)
{
    u16 block = GetMapGridBlockAt(x, y);

    if (block == MAPGRID_UNDEFINED)
        return GetBorderBlockAt(x, y) & MAPGRID_METATILE_ID_MASK;

    return block & MAPGRID_METATILE_ID_MASK;
}

u32 ExtractMetatileAttribute(u32 attributes, u8 attributeType)
{
    if (attributeType >= METATILE_ATTRIBUTE_COUNT) // Check for METATILE_ATTRIBUTES_ALL
        return attributes;

    return (attributes & sMetatileAttrMasks[attributeType]) >> sMetatileAttrShifts[attributeType];
}

u32 MapGridGetMetatileAttributeAt(s16 x, s16 y, u8 attributeType)
{
    u16 metatileId = MapGridGetMetatileIdAt(x, y);
    return GetAttributeByMetatileIdAndMapLayout(gMapHeader.mapLayout, metatileId, attributeType);
}

u32 MapGridGetMetatileBehaviorAt(s16 x, s16 y)
{
    return MapGridGetMetatileAttributeAt(x, y, METATILE_ATTRIBUTE_BEHAVIOR);
}

u8 MapGridGetMetatileLayerTypeAt(s16 x, s16 y)
{
    return MapGridGetMetatileAttributeAt(x, y, METATILE_ATTRIBUTE_LAYER_TYPE);
}

void MapGridSetMetatileIdAt(s32 x, s32 y, u16 metatile)
{
    s32 i;
    if (AreCoordsWithinMapGridBounds(x, y))
    {
        i = x + y * VMap.Xsize;
        VMap.map[i] = (VMap.map[i] & MAPGRID_ELEVATION_MASK) | (metatile & ~MAPGRID_ELEVATION_MASK);
    }
}

void MapGridSetMetatileEntryAt(s32 x, s32 y, u16 metatile)
{
    s32 i;
    if (AreCoordsWithinMapGridBounds(x, y))
    {
        i = x + VMap.Xsize * y;
        VMap.map[i] = metatile;
    }
}

void MapGridSetMetatileImpassabilityAt(s32 x, s32 y, bool32 impassable)
{
    if (AreCoordsWithinMapGridBounds(x, y))
    {
        if (impassable)
            VMap.map[x + VMap.Xsize * y] |= MAPGRID_COLLISION_MASK;
        else
            VMap.map[x + VMap.Xsize * y] &= ~MAPGRID_COLLISION_MASK;
    }
}

static u32 GetAttributeByMetatileIdAndMapLayout(const struct MapLayout *mapLayout, u16 metatile, u8 attributeType)
{
    const u32 * attributes;

    if (metatile < NUM_METATILES_IN_PRIMARY)
    {
        attributes = mapLayout->primaryTileset->metatileAttributes;
        return ExtractMetatileAttribute(attributes[metatile], attributeType);
    }
    else if (metatile < NUM_METATILES_TOTAL)
    {
        attributes = mapLayout->secondaryTileset->metatileAttributes;
        return ExtractMetatileAttribute(attributes[metatile - NUM_METATILES_IN_PRIMARY], attributeType);
    }
    else
    {
        return 0xFF;
    }
}

void SaveMapView(void)
{
    s32 i, j;
    s32 x, y;
    u16 *mapView;
    s32 width;
    mapView = gSaveBlock2Ptr->mapView;
    width = VMap.Xsize;
    x = gSaveBlock1Ptr->pos.x;
    y = gSaveBlock1Ptr->pos.y;
    for (i = y; i < y + MAP_OFFSET_H; i++)
    {
        for (j = x; j < x + MAP_OFFSET_W; j++)
            *mapView++ = gBackupMapData[width * i + j];
    }
}

static bool32 SavedMapViewIsEmpty(void)
{
    u16 i;
    u32 marker = 0;

#ifndef UBFIX
    // BUG: This loop extends past the bounds of the mapView array. Its size is only 0x100.
    for (i = 0; i < 0x200; i++)
        marker |= gSaveBlock2Ptr->mapView[i];
#else
    for (i = 0; i < NELEMS(gSaveBlock2Ptr->mapView); i++)
        marker |= gSaveBlock2Ptr->mapView[i];
#endif

    if (marker == 0)
        return TRUE;
    else
        return FALSE;
}

static void ClearSavedMapView(void)
{
    CpuFill16(0, gSaveBlock2Ptr->mapView, sizeof(gSaveBlock2Ptr->mapView));
}

static void LoadSavedMapView(void)
{
    s32 i, j;
    s32 x, y;
    u16 *mapView;
    s32 width;
    mapView = gSaveBlock2Ptr->mapView;
    if (!SavedMapViewIsEmpty())
    {
        width = VMap.Xsize;
        x = gSaveBlock1Ptr->pos.x;
        y = gSaveBlock1Ptr->pos.y;
        for (i = y; i < y + MAP_OFFSET_H; i++)
        {
            for (j = x; j < x + MAP_OFFSET_W; j++)
            {
                gBackupMapData[j + width * i] = *mapView;
                mapView++;
            }
        }
        ClearSavedMapView();
    }
}

static void MoveMapViewToBackup(u8 direction)
{
    s32 width;
    u16 *mapView;
    s32 x0, y0;
    s32 x2, y2;
    u16 *src, *dest;
    s32 srci, desti;
    s32 r9, r8;
    s32 x, y;
    s32 i, j;
    mapView = gSaveBlock2Ptr->mapView;
    width = VMap.Xsize;
    r9 = 0;
    r8 = 0;
    x0 = gSaveBlock1Ptr->pos.x;
    y0 = gSaveBlock1Ptr->pos.y;
    x2 = 15;
    y2 = 14;
    switch (direction)
    {
    case CONNECTION_NORTH:
        y0 += 1;
        y2 = MAP_OFFSET_H - 1;
        break;
    case CONNECTION_SOUTH:
        r8 = 1;
        y2 = MAP_OFFSET_H - 1;
        break;
    case CONNECTION_WEST:
        x0 += 1;
        x2 = MAP_OFFSET_W - 1;
        break;
    case CONNECTION_EAST:
        r9 = 1;
        x2 = MAP_OFFSET_W - 1;
        break;
    }
    for (y = 0; y < y2; y++)
    {
        i = 0;
        j = 0;
        for (x = 0; x < x2; x++)
        {
            desti = width * (y + y0);
            srci = (y + r8) * MAP_OFFSET_W + r9;
            src = &mapView[srci + i];
            dest = &gBackupMapData[x0 + desti + j];
            *dest = *src;
            i++;
            j++;
        }
    }
    ClearSavedMapView();
}

s32 GetMapBorderIdAt(s32 x, s32 y)
{
    if (GetMapGridBlockAt(x, y) == MAPGRID_UNDEFINED)
        return CONNECTION_INVALID;

    if (x >= VMap.Xsize - (MAP_OFFSET + 1))
    {
        if (!gMapConnectionFlags.east)
            return CONNECTION_INVALID;

        return CONNECTION_EAST;
    }

    if (x < MAP_OFFSET)
    {
        if (!gMapConnectionFlags.west)
            return CONNECTION_INVALID;

        return CONNECTION_WEST;
    }

    if (y >= VMap.Ysize - MAP_OFFSET)
    {
        if (!gMapConnectionFlags.south)
            return CONNECTION_INVALID;

        return CONNECTION_SOUTH;
    }

    if (y < MAP_OFFSET)
    {
        if (!gMapConnectionFlags.north)
            return CONNECTION_INVALID;

        return CONNECTION_NORTH;
    }

    return CONNECTION_NONE;
}

static s32 GetPostCameraMoveMapBorderId(s32 x, s32 y)
{
    return GetMapBorderIdAt(gSaveBlock1Ptr->pos.x + MAP_OFFSET + x, gSaveBlock1Ptr->pos.y + MAP_OFFSET + y);
}

bool32 CanCameraMoveInDirection(s32 direction)
{
    s32 x, y;
    x = gSaveBlock1Ptr->pos.x + MAP_OFFSET + gDirectionToVectors[direction].x;
    y = gSaveBlock1Ptr->pos.y + MAP_OFFSET + gDirectionToVectors[direction].y;

    if (GetMapBorderIdAt(x, y) == CONNECTION_INVALID)
        return FALSE;

    return TRUE;
}

static void SetPositionFromConnection(const struct MapConnection *connection, int direction, s32 x, s32 y)
{
    struct MapHeader const *mapHeader;
    mapHeader = GetMapHeaderFromConnection(connection);
    switch (direction)
    {
    case CONNECTION_EAST:
        gSaveBlock1Ptr->pos.x = -x;
        gSaveBlock1Ptr->pos.y -= connection->offset;
        break;
    case CONNECTION_WEST:
        gSaveBlock1Ptr->pos.x = mapHeader->mapLayout->width;
        gSaveBlock1Ptr->pos.y -= connection->offset;
        break;
    case CONNECTION_SOUTH:
        gSaveBlock1Ptr->pos.x -= connection->offset;
        gSaveBlock1Ptr->pos.y = -y;
        break;
    case CONNECTION_NORTH:
        gSaveBlock1Ptr->pos.x -= connection->offset;
        gSaveBlock1Ptr->pos.y = mapHeader->mapLayout->height;
        break;
    }
}

bool8 CameraMove(s32 x, s32 y)
{
    s32 direction;
    const struct MapConnection *connection;
    s32 old_x, old_y;
    gCamera.active = FALSE;
    direction = GetPostCameraMoveMapBorderId(x, y);
    if (direction == CONNECTION_NONE || direction == CONNECTION_INVALID)
    {
        gSaveBlock1Ptr->pos.x += x;
        gSaveBlock1Ptr->pos.y += y;
    }
    else
    {
        SaveMapView();
        old_x = gSaveBlock1Ptr->pos.x;
        old_y = gSaveBlock1Ptr->pos.y;
        connection = GetIncomingConnection(direction, gSaveBlock1Ptr->pos.x, gSaveBlock1Ptr->pos.y);
        SetPositionFromConnection(connection, direction, x, y);
        LoadMapFromCameraTransition(connection->mapGroup, connection->mapNum);
        gCamera.active = TRUE;
        gCamera.x = old_x - gSaveBlock1Ptr->pos.x;
        gCamera.y = old_y - gSaveBlock1Ptr->pos.y;
        gSaveBlock1Ptr->pos.x += x;
        gSaveBlock1Ptr->pos.y += y;
        MoveMapViewToBackup(direction);
    }
    return gCamera.active;
}

const struct MapConnection *GetIncomingConnection(u8 direction, s32 x, s32 y)
{
    s32 count;
    const struct MapConnection *connection;
    const struct MapConnections *connections = gMapHeader.connections;
    s32 i;

#ifdef UBFIX // UB: Multiple possible null dereferences
    if (connections == NULL || connections->connections == NULL)
        return NULL;
#endif
    count = connections->count;
    connection = connections->connections;
    for (i = 0; i < count; i++, connection++)
    {
        if (connection->direction == direction && IsPosInIncomingConnectingMap(direction, x, y, connection) == TRUE)
            return connection;
    }
    return NULL;

}

static bool8 IsPosInIncomingConnectingMap(u8 direction, s32 x, s32 y, const struct MapConnection *connection)
{
    struct MapHeader const *mapHeader;
    mapHeader = GetMapHeaderFromConnection(connection);
    switch (direction)
    {
    case CONNECTION_SOUTH:
    case CONNECTION_NORTH:
        return IsCoordInIncomingConnectingMap(x, gMapHeader.mapLayout->width, mapHeader->mapLayout->width, connection->offset);
    case CONNECTION_WEST:
    case CONNECTION_EAST:
        return IsCoordInIncomingConnectingMap(y, gMapHeader.mapLayout->height, mapHeader->mapLayout->height, connection->offset);
    }
    return FALSE;
}

static bool8 IsCoordInIncomingConnectingMap(s32 coord, s32 srcMax, s32 destMax, s32 offset)
{
    s32 offset2 = max(offset, 0);

    if (destMax + offset < srcMax)
        srcMax = destMax + offset;

    if (offset2 <= coord && coord <= srcMax)
        return TRUE;

    return FALSE;
}

static bool32 IsCoordInConnectingMap(s32 coord, s32 max)
{
    if (coord >= 0 && coord < max)
        return TRUE;

    return FALSE;
}

static s32 IsPosInConnectingMap(const struct MapConnection *connection, s32 x, s32 y)
{
    struct MapHeader const *mapHeader;
    mapHeader = GetMapHeaderFromConnection(connection);
    switch (connection->direction)
    {
    case CONNECTION_SOUTH:
    case CONNECTION_NORTH:
        return IsCoordInConnectingMap(x - connection->offset, mapHeader->mapLayout->width);
    case CONNECTION_WEST:
    case CONNECTION_EAST:
        return IsCoordInConnectingMap(y - connection->offset, mapHeader->mapLayout->height);
    }
    return FALSE;
}

const struct MapConnection *GetMapConnectionAtPos(s16 x, s16 y)
{
    s32 count;
    const struct MapConnection *connection;
    s32 i;
    u8 direction;
    if (!gMapHeader.connections)
    {
        return NULL;
    }
    else
    {
        count = gMapHeader.connections->count;
        connection = gMapHeader.connections->connections;
        for (i = 0; i < count; i++, connection++)
        {
            direction = connection->direction;
            if ((direction == CONNECTION_DIVE || direction == CONNECTION_EMERGE)
                || (direction == CONNECTION_NORTH && y > MAP_OFFSET - 1)
                || (direction == CONNECTION_SOUTH && y < gMapHeader.mapLayout->height + MAP_OFFSET)
                || (direction == CONNECTION_WEST && x > MAP_OFFSET - 1)
                || (direction == CONNECTION_EAST && x < gMapHeader.mapLayout->width + MAP_OFFSET))
            {
                continue;
            }

            if (IsPosInConnectingMap(connection, x - MAP_OFFSET, y - MAP_OFFSET) == TRUE)
                return connection;
        }
    }
    return NULL;
}

void SetCameraFocusCoords(u16 x, u16 y)
{
    gSaveBlock1Ptr->pos.x = x - MAP_OFFSET;
    gSaveBlock1Ptr->pos.y = y - MAP_OFFSET;
}

void GetCameraFocusCoords(u16 *x, u16 *y)
{
    *x = gSaveBlock1Ptr->pos.x + MAP_OFFSET;
    *y = gSaveBlock1Ptr->pos.y + MAP_OFFSET;
}

// Unused
static void SetCameraCoords(u16 x, u16 y)
{
    gSaveBlock1Ptr->pos.x = x;
    gSaveBlock1Ptr->pos.y = y;
}

void GetCameraCoords(u16 *x, u16 *y)
{
    *x = gSaveBlock1Ptr->pos.x;
    *y = gSaveBlock1Ptr->pos.y;
}

static void CopyTilesetToVram(struct Tileset const *tileset, u16 numTiles, u16 offset)
{
    if (tileset)
    {
        if (!tileset->isCompressed)
            LoadBgTiles(2, tileset->tiles, numTiles * 32, offset);
        else
            DecompressAndCopyTileDataToVram2(2, tileset->tiles, numTiles * 32, offset, 0);
    }
}

static void CopyTilesetToVramUsingHeap(struct Tileset const *tileset, u16 numTiles, u16 offset)
{
    if (tileset)
    {
        if (!tileset->isCompressed)
            LoadBgTiles(2, tileset->tiles, numTiles * 32, offset);
        else
            DecompressAndLoadBgGfxUsingHeap2(2, tileset->tiles, numTiles * 32, offset, 0);
    }
}

// 8.6 as settled: COLOUR IS CONTEXT. Halftone is the one town that is
// genuinely drained -- 3.1's premise is dots that only LOOK like grey, and a
// second grey town halves it, so every other achromatic town on the ladder is
// graded rather than desaturated and that is authored in its tileset palettes,
// not here.
//
// The Quest Log owns gGlobalFieldTintMode while it is playing back, so this
// only ever speaks when that global is silent. Never override the record.
// HALFTONE IS THE TOWN AND ITS TOWER. The tower has its own map section, so a
// check on the town's section alone left all seven floors in colour -- and the
// tower is where Halftone's lesson lives: something the Index cannot name, and
// an instrument needed to see it. Battles ask this too, so the scene stays
// grey when a fight starts.
// T-430: THE TOWER IS THE EXCEPTION THAT ENDS. Once INIT is rescued each floor comes back into colour the first time
// you stand on it after (DaemonsHalftoneTick fades it in and sets its flag); the town stays grey.
static u16 DaemonsTowerFloorFlag(void)
{
    if (gSaveBlock1Ptr->location.mapGroup != MAP_GROUP(MAP_POKEMON_TOWER_1F)
     || gSaveBlock1Ptr->location.mapNum < MAP_NUM(MAP_POKEMON_TOWER_1F)
     || gSaveBlock1Ptr->location.mapNum > MAP_NUM(MAP_POKEMON_TOWER_7F))
        return 0;
    return FLAG_TOWER_COLOUR_1F + gSaveBlock1Ptr->location.mapNum - MAP_NUM(MAP_POKEMON_TOWER_1F);
}

bool8 DaemonsIsHalftone(void)
{
    u16 floor;

    if (gMapHeader.regionMapSectionId == MAPSEC_POKEMON_TOWER)
    {
        floor = DaemonsTowerFloorFlag();
        return floor == 0 || !FlagGet(floor);
    }
    return gMapHeader.regionMapSectionId == MAPSEC_LAVENDER_TOWN
        || DaemonsCaveUnperceived();
}

// T-251: AND DOLDRUM CAVE, UNTIL IT IS UNDERSTOOD. The second grey place, and the first whose grey can be lifted:
// the REVIEW BOARD lets you in, and without the islands' understanding you cannot read what you walked into.
// Everything that asks "is this Halftone?" -- the grey frame, the tint, the PERSPECTIVE flash -- asks this too.
bool8 DaemonsCaveUnperceived(void)
{
    return gMapHeader.regionMapSectionId == MAPSEC_CERULEAN_CAVE && !FlagGet(FLAG_UNDERSTANDING_FIRST);
}

// IN HALFTONE EVERYTHING IS GREY (vision.md 8.6a, revised 2026-09-13): the
// town, the tower, the battle, your own daemons, you, and every window of text.
// Greying each loader missed whatever it did not name -- the healthboxes, the
// menus, the message box -- so this greys the one buffer every palette reaches
// the screen through, once a frame, just before the vblank copies it out.
// gPlttBufferUnfaded is left in colour, which is what lets PERSPECTIVE's flash
// put the colour back in one copy. A colour that is already grey is one
// multiply and a compare, so a settled frame costs almost nothing.
EWRAM_DATA static bool8 sDaemonsHalftoneInColour = FALSE;

void DaemonsSetHalftoneColour(bool8 inColour)
{
    sDaemonsHalftoneInColour = inColour;
    if (inColour && !gPaletteFade.active)
        CpuCopy16(gPlttBufferUnfaded, gPlttBufferFaded, PLTT_SIZE);
}

// T-255: THE UNREAD CAVE IS A DRAWING, NOT A HALFTONE. The Painted Mirror's door, "the harder I tried to open it,
// the less real it became ... mere dots and lines, just a simple drawing" (lineage.md 3b): every colour falls to one
// of four flat tones of ink on warm paper, so the cave reads as drawn, where HALFTONE reads as a grey photograph.
// A colour that is already one of the four is left alone, so a settled frame costs four compares an entry.
static const u16 sDrawingTones[4] = { RGB(3, 3, 5), RGB(11, 10, 11), RGB(22, 21, 18), RGB(30, 29, 25) };

static void DaemonsDrawingTone(u16 *colour)
{
    s32 r, g, b, lum;

    if (*colour == sDrawingTones[0] || *colour == sDrawingTones[1] || *colour == sDrawingTones[2] || *colour == sDrawingTones[3])
        return;
    r = *colour & 0x1F;
    g = (*colour >> 5) & 0x1F;
    b = (*colour >> 10) & 0x1F;
    lum = (r * 77 + g * 150 + b * 29) >> 8;
    *colour = sDrawingTones[lum < 7 ? 0 : lum < 14 ? 1 : lum < 23 ? 2 : 3];
}

// T-430 (vision.md 8.6a, revised by the user 2026-10-21): WHAT YOU BRING KEEPS ITS COLOUR. The grey is still one pass
// over the one buffer, and the exemptions are still PALETTE SLOTS, never loaders: in the field the player's own slot
// and its reflection's and the follower's; in a battle each of the player side's battlers (its OBJ slot and its BG
// copy for the animations, so your daemons and your back sprite), and the slots the player side's routine loaded,
// which keep less colour the further their sprites have gone toward the target. Everything else -- menus, windows,
// banners, healthboxes, the town, its people and the other side -- is grey as before.
// The colour a slot shows is taken from gPlttBufferUnfaded, so a partial share never compounds frame on frame; the
// field's tint therefore no longer greys a palette as it loads in HALFTONE (the unread cave's drawing still does).
#define HT_OBJ(slot) (1u << (16 + (slot)))
#define HT_BG(slot)  (1u << (slot))
#define HT_FLICKER_WAIT   24     // settled field frames before the first sight tries
#define HT_FLOOR_WAIT     150    // a tower floor stays grey this long as you arrive, then comes back over HT_FLOOR_FADE
#define HT_FLOOR_FADE     120
#define HT_SPLASH_FRAMES  40

// The first sight of HALFTONE: how much colour (of 256) the screen reaches, a frame at a time, before it fails.
static const u8 sHalftoneFlicker[] = {
    40, 96, 150, 120, 60, 0, 0, 0, 30, 80, 104, 64, 0, 0, 0, 0, 0, 24, 48, 20, 0,
};

static struct {
    u8 wait;          // settled field frames (the first sight, a tower floor)
    u8 flicker;       // frames into the first sight, 0 when not running
    u8 floorFade;     // frames into a tower floor's return
    u8 splash;        // frames left of a splash
    u8 splashSlot;    // the OBJ slot it lands on, or 0xFF
    u8 share;         // the whole field's share of colour this frame (the flicker, a floor's return)
    u16 splashColour;
    u16 routineMask;  // OBJ slots the player side's routine is drawn in (battle_anim.c), this frame
    u16 routineKeep[16];
    u8 washTarget;    // the foe your routine is playing on, or 0xFF
    u8 washDirty;     // the slot the wash was last drawn on, greyed afresh from the colour copy once it ends
    bool8 washSeen;   // the routine has washed it (AnimTask_BlendBattleAnimPal), so wash is that colour
    u16 wash;         // the colour a share of which the foe shows while your routine plays
    u16 washTrue[16]; // the foe's palette as the animation last wrote it ...
    u16 washOut[16];  // ... and as the grey last wrote it over that, so a frame the animation skips is told apart
} sHalftone;

#define HT_WASH_SHARE 80   // of 256, at the wash's full strength: faint

void DaemonsHalftoneWash(u16 colour)
{
    s32 e;

    sHalftone.washSeen = colour != 0;
    sHalftone.wash = colour;
    for (e = 0; e < 16; e++)
        sHalftone.washOut[e] = 0x8000;   // no colour is this: the first frame always reads the animation's
}

void DaemonsHalftoneWashDefault(u16 colour)
{
    if (!sHalftone.washSeen)
        sHalftone.wash = colour;
}

static u16 DaemonsHalftoneMix(u16 c, s32 share)
{
    s32 r = GET_R(c), g = GET_G(c), b = GET_B(c);
    s32 grey = (r * Q_8_8(0.3) + g * Q_8_8(0.59) + b * Q_8_8(0.1133)) >> 8;

    if (share <= 0)
        return RGB2(grey, grey, grey);
    r = grey + (((r - grey) * share) >> 8);
    g = grey + (((g - grey) * share) >> 8);
    b = grey + (((b - grey) * share) >> 8);
    return RGB2(r, g, b);
}

static s32 DaemonsHalftoneLum(u16 c)
{
    return (GET_R(c) * Q_8_8(0.3) + GET_G(c) * Q_8_8(0.59) + GET_B(c) * Q_8_8(0.1133)) >> 8;
}

static u16 DaemonsHalftoneLightOnly(u16 own, u16 now)
{
    s32 shift = DaemonsHalftoneLum(now) - DaemonsHalftoneLum(own);
    s32 r = GET_R(own) + shift, g = GET_G(own) + shift, b = GET_B(own) + shift;

    if (shift == 0)
        return own;
    r = r < 0 ? 0 : r > 31 ? 31 : r;
    g = g < 0 ? 0 : g > 31 ? 31 : g;
    b = b < 0 ? 0 : b > 31 ? 31 : b;
    return RGB2(r, g, b);
}

static s32 DaemonsSplashChannel(s32 grey, s32 channel, s32 amount)
{
    s32 tint = (grey * channel) / 31 + grey / 4;

    if (tint > 31)
        tint = 31;
    return grey + (((tint - grey) * amount) >> 8);
}

// A grey, multiplied by the splash's colour (and lifted a quarter, so the dark of it still shows), by amount of 256.
static u16 DaemonsHalftoneSplashTint(u16 grey, s32 amount)
{
    s32 g = GET_R(grey);
    u16 c = sHalftone.splashColour;

    return RGB2(DaemonsSplashChannel(g, GET_R(c), amount), DaemonsSplashChannel(g, GET_G(c), amount),
                DaemonsSplashChannel(g, GET_B(c), amount));
}

static s32 DaemonsColourDistance(u16 a, u16 b)
{
    s32 r = (s32)GET_R(a) - GET_R(b), g = (s32)GET_G(a) - GET_G(b), bl = (s32)GET_B(a) - GET_B(b);

    return (r < 0 ? -r : r) + (g < 0 ? -g : g) + (bl < 0 ? -bl : bl);
}

// The foe your routine is playing on: its grey, and over it a faint share of the wash -- as much as the animation has
// moved the foe toward that wash (HT_WASH_SHARE at all of it). The palette the animation wrote is kept beside what we
// wrote, so a frame the animation leaves alone is read from the animation's, never from our own (no compounding).
static void DaemonsHalftoneWashFoe(u16 *colour, const u16 *source)
{
    s32 e, moved = 0, room = 0, share;
    u16 saved = sHalftone.splashColour;

    for (e = 0; e < 16; e++)
    {
        if (colour[e] != sHalftone.washOut[e])
            sHalftone.washTrue[e] = colour[e];
        moved += DaemonsColourDistance(sHalftone.washTrue[e], source[e]);
        room += DaemonsColourDistance(sHalftone.wash, source[e]);
    }
    share = room != 0 ? (moved * HT_WASH_SHARE) / room : 0;
    if (share > HT_WASH_SHARE)
        share = HT_WASH_SHARE;
    sHalftone.splashColour = sHalftone.wash;
    for (e = 0; e < 16; e++)
    {
        colour[e] = share != 0 ? DaemonsHalftoneSplashTint(DaemonsHalftoneMix(sHalftone.washTrue[e], 0), share)
                               : DaemonsHalftoneMix(sHalftone.washTrue[e], 0);
        sHalftone.washOut[e] = colour[e];
    }
    sHalftone.splashColour = saved;
}

// Called by battle_anim.c when the player side's routine reaches a foe: its grey takes the attacker's type colour,
// shaded by its own light, and lets it go over HT_SPLASH_FRAMES.
void DaemonsHalftoneSplash(u8 battler, u16 colour)
{
    if (!DaemonsIsHalftone() || sDaemonsHalftoneInColour || DaemonsLatentAbstractionBattler() == battler)
        return;
    sHalftone.splash = HT_SPLASH_FRAMES;
    sHalftone.splashSlot = battler;
    sHalftone.splashColour = colour;
}

static bool8 DaemonsHalftoneFieldSettled(void)
{
    return DaemonsFieldOnScreen() && !gMain.inBattle && !gPaletteFade.active;
}

// Once a frame, from main.c only (the grey itself also runs at the vblank's copy and a fade's, so it must not count).
void DaemonsHalftoneTick(void)
{
    u16 floor;

    sHalftone.share = 0;
    sHalftone.routineMask = 0;
    if (sHalftone.splash != 0)
        sHalftone.splash--;
    if (!DaemonsIsHalftone() || sDaemonsHalftoneInColour)
    {
        sHalftone.wait = sHalftone.flicker = sHalftone.floorFade = sHalftone.splash = 0;
        sHalftone.splashSlot = 0xFF;
        return;
    }
    if (gMain.inBattle)
    {
        sHalftone.wait = sHalftone.flicker = sHalftone.floorFade = 0;
        sHalftone.washTarget = 0xFF;
        if (gMain.callback2 == BattleMainCB2)
            sHalftone.routineMask = DaemonsHalftoneRoutineSlots(sHalftone.routineKeep, &sHalftone.washTarget);
        return;
    }
    sHalftone.splashSlot = 0xFF;
    if (!DaemonsHalftoneFieldSettled())
    {
        sHalftone.wait = sHalftone.flicker = sHalftone.floorFade = 0;
        return;
    }
    if (sHalftone.wait < 0xFF)
        sHalftone.wait++;
    // The first time you see it, the screen tries to come into colour and fails. Once.
    if (!FlagGet(FLAG_HALFTONE_TRIED_COLOUR) && !DaemonsCaveUnperceived())
    {
        // Only while you are walking in it freely: never over a box or a scene (the user, 2026-10-21).
        if (sHalftone.flicker == 0 && (ArePlayerFieldControlsLocked() || ScriptContext_IsEnabled() || !IsFieldMessageBoxHidden()))
        {
            sHalftone.wait = 0;
            return;
        }
        if (sHalftone.wait < HT_FLICKER_WAIT)
            return;
        if (sHalftone.flicker >= ARRAY_COUNT(sHalftoneFlicker))
        {
            FlagSet(FLAG_HALFTONE_TRIED_COLOUR);
            sHalftone.flicker = 0;
            sHalftone.wait = 0;
            return;
        }
        sHalftone.share = sHalftoneFlicker[sHalftone.flicker++];
        return;
    }
    // A tower floor after INIT is rescued: grey as you arrive, then it comes back, and stays.
    floor = DaemonsTowerFloorFlag();
    if (floor != 0 && FlagGet(FLAG_RESCUED_MR_FUJI) && sHalftone.wait >= HT_FLOOR_WAIT)
    {
        if (++sHalftone.floorFade >= HT_FLOOR_FADE)
        {
            FlagSet(floor);
            sHalftone.floorFade = 0;
            CpuCopy16(gPlttBufferUnfaded, gPlttBufferFaded, PLTT_SIZE);
            return;
        }
        sHalftone.share = (sHalftone.floorFade * 256) / HT_FLOOR_FADE;
    }
}

void DaemonsGreyHalftoneFrame(void)
{
    u16 *colour, *source;
    s32 p, e, keep, shine, splash, washed;
    u32 skip, routine, lightOnly;
    bool8 drawing;

    if (sDaemonsHalftoneInColour || !DaemonsIsHalftone())
        return;
    skip = 0;
    routine = 0;
    lightOnly = 0;
    keep = DaemonsLatentAbstractionBattler();
    // T-251: the one thing in the unread cave that keeps its colour is the thing you cannot read -- the LATENT
    // abstraction, whose palette is different every time (battle_gfx_sfx_util.c). Its sprite palette is the opponent
    // battler's, and its copy for the battle animations is BG palette 8 + that battler.
    if (keep >= 0)
        skip |= HT_OBJ(keep) | HT_BG(8 + keep);
    shine = IndexOfSpritePaletteTag(PALTAG_KEY_COLOURS);   // T-417: PLAY's notes always shine (the user, 2026-10-17)
    if (shine != 0xFF)
        skip |= HT_OBJ(shine);
    if (gMain.inBattle)
    {
        if (gMain.callback2 == BattleMainCB2)
        {
            // BG 8 and 9 are also the animations' own backdrops (battle_anim_mons.c's BG_ANIM_PAL_1/2), so while the
            // other side's routine plays, they are left grey: its SURF must not come in colour.
            bool8 foeAnim = gAnimScriptActive && GetBattlerSide(gBattleAnimAttacker) != B_SIDE_PLAYER;

            for (p = 0; p < gBattlersCount; p++)
            {
                if (GetBattlerSide(p) != B_SIDE_PLAYER)
                    continue;
                if (!foeAnim)
                    skip |= HT_OBJ(p) | HT_BG(8 + p);
                else if (!gPaletteFade.active)
                    lightOnly |= HT_OBJ(p);    // their routine on you: its light, not its colour
                else
                    skip |= HT_OBJ(p);
            }
            if (!gPaletteFade.active)
                routine = (u32)sHalftone.routineMask << 16;
        }
    }
    else if (DaemonsFieldOnScreen())
    {
        u8 follower = Follower_PaletteSlot();

        skip |= HT_OBJ(PALSLOT_PLAYER) | HT_OBJ(PALSLOT_PLAYER_REFLECTION);
        if (follower < 16)
            skip |= HT_OBJ(follower);
    }
    splash = -1;
    washed = -1;
    if (gMain.inBattle && sHalftone.splashSlot < 16 && !gPaletteFade.active)
        splash = 16 + sHalftone.splashSlot;
    else if (gMain.inBattle && gMain.callback2 == BattleMainCB2 && sHalftone.washTarget < 16 && !gPaletteFade.active)
        washed = 16 + sHalftone.washTarget;
    drawing = DaemonsCaveUnperceived();
    colour = gPlttBufferFaded;
    source = gPlttBufferUnfaded;
    for (p = 0; p < 32; p++, colour += 16, source += 16)
    {
        if (skip & (1u << p))
            continue;
        if (lightOnly & (1u << p))
        {
            // The other side's routine blending your daemon: your colour, moved only as far as its grey moved.
            for (e = 0; e < 16; e++)
                colour[e] = DaemonsHalftoneLightOnly(source[e], colour[e]);
            continue;
        }
        if (routine & (1u << p))
        {
            keep = sHalftone.routineKeep[p - 16];
            for (e = 0; e < 16; e++)
                colour[e] = DaemonsHalftoneMix(source[e], keep);
            continue;
        }
        if (p == splash)
        {
            // The foe's grey, multiplied by the colour that reached it, fading back to its grey; on the frame after
            // it ends, its grey from the colour copy again, so the tint's light is not kept.
            keep = (sHalftone.splash * 120) / HT_SPLASH_FRAMES;   // a little colour: under half, at its height
            for (e = 0; e < 16; e++)
                colour[e] = DaemonsHalftoneSplashTint(DaemonsHalftoneMix(source[e], 0), keep);
            if (sHalftone.splash == 0)
                sHalftone.splashSlot = 0xFF;
            continue;
        }
        if (p == washed)
        {
            DaemonsHalftoneWashFoe(colour, source);
            sHalftone.washDirty = p;
            continue;
        }
        if (sHalftone.washDirty >= 16 && p == sHalftone.washDirty && p != splash && !gPaletteFade.active)
        {
            for (e = 0; e < 16; e++)
                colour[e] = DaemonsHalftoneMix(source[e], 0);
            sHalftone.washDirty = 0;
            continue;
        }
        if (sHalftone.share != 0 && !drawing)
        {
            for (e = 0; e < 16; e++)
                colour[e] = DaemonsHalftoneMix(source[e], sHalftone.share);
            continue;
        }
        for (e = 0; e < 16; e++)
        {
            if (drawing)
                DaemonsDrawingTone(&colour[e]);
            else if (colour[e] != (colour[e] & 0x1F) * 0x421)
                TintPalette_GrayScale(&colour[e], 1);
        }
    }
}

// T-56 (vision.md 9.22): BLANCHE'S GROUND. The houses went pale in their own
// rows (T-55) and the lab keeps row 9; what is left -- grass, trees, flowers,
// the pond -- is drawn in the COMMON rows 0..6, which every outdoor map shares,
// and in Blanche's own rows 11 and 12. So it is washed as the palettes LOAD:
// only on the Blanche map itself, not its houses or its lab; only tiles, never
// people, which is why this is not DaemonsFieldTint; and never rows 8..10,
// which are already pale, and the lab.
bool8 DaemonsIsBlancheOutdoors(void)
{
    return gSaveBlock1Ptr->location.mapGroup == MAP_GROUP(MAP_PALLET_TOWN)
        && gSaveBlock1Ptr->location.mapNum == MAP_NUM(MAP_PALLET_TOWN);
}

// tools/gbablanche.py's pale, in five-bit colour: toward the colour's own grey
// by 0.55, then toward white by 0.42, 0.26 or 0.12 as it is light, mid or dark,
// so trees fade and their outlines stay outlines. Since T-58 Blanche's own ground
// is drawn pale in its rows 7 and 12, so only the primary rows are washed: the
// town sign, and the edges of Route 1 and Route 21 seen across the connections.
static void DaemonsPaleEntries(u16 offset, u16 count)
{
    u16 i;

    for (i = 0; i < count; i++)
    {
        u16 c = gPlttBufferUnfaded[offset + i];
        s32 r = c & 0x1F, g = (c >> 5) & 0x1F, b = (c >> 10) & 0x1F;
        s32 grey = (r * 77 + g * 150 + b * 29) >> 8;
        s32 lift = grey > 18 ? 108 : grey > 12 ? 67 : 31;

        r += ((grey - r) * 141) / 256;
        g += ((grey - g) * 141) / 256;
        b += ((grey - b) * 141) / 256;
        r += ((31 - r) * lift + 128) >> 8;
        g += ((31 - g) * lift + 128) >> 8;
        b += ((31 - b) * lift + 128) >> 8;
        gPlttBufferUnfaded[offset + i] = RGB2(r, g, b);
    }
    CpuCopy16(&gPlttBufferUnfaded[offset], &gPlttBufferFaded[offset], PLTT_SIZEOF(count));
}

// T-317 (the user, 2026-10-02: "go with A"): UNDERSTANDING SHOWN IN THE WORLD, AS A FADED PRINT. The map and the
// INDEX already sharpen as understandings arrive (DaemonsClarity); the world's own colours do the same, never as a
// count: while four or more are still to come, every tileset colour is drawn 40% of the way to its own grey; while one
// to three are, 18%; once all are held, as painted. Only the TILES -- people and daemons are sprites, so they keep
// their full colour and stand a little clearer than the world until it catches up. Done as the palettes LOAD, as
// Blanche's wash is, so a fade or a reload can never compound it; HALFTONE's grey outranks it, and the watch's light
// falls on top of it. T-395: and never in THE NEXUS, which is made entirely of paint -- the one place that is not
// waiting to be understood before its colours show.
static u8 DaemonsClarityFade(void)
{
    u8 missing;

    if (DaemonsIsHalftone())
        return 0;
    if (gMapHeader.mapLayout != NULL && gMapHeader.mapLayout->secondaryTileset == &gTileset_Nexus)
        return 0;
    missing = DaemonsUnderstandingsMissing();
    return missing >= 4 ? 102 : missing != 0 ? 46 : 0;     // of 256
}

// T-359 (vision.md 9.21; the user approved the mock-ups 2026-10-03): THE SEASONS, BY PALETTE. On an outdoor map drawn
// from gTileset_General -- but never BLANCHE, which stays a town before colour -- palette 0's tree, grass and petal
// slots take the season's colour, and the entries of General's rows 1..6 that copy its greens follow (the edges of
// paths and ponds). The season goes in first, in the same pass as the faded print; the watch falls on both after.
// Summer is the palette as drawn. The tables are tools/gbaseasons.py's, from the approved mock-ups.
#include "data/season_palettes.h"

// T-417 (the user, 2026-10-18): BREATHVERSE stays timeless -- no season and no watch's light on the islet. Its weather
// is left as the day draws it (only the tints were ruled out).
bool8 DaemonsIsTimeless(void)
{
    return gSaveBlock1Ptr->location.mapGroup == MAP_GROUP(MAP_BREATHVERSE)
        && gSaveBlock1Ptr->location.mapNum == MAP_NUM(MAP_BREATHVERSE);
}

static bool8 DaemonsSeasonShows(void)
{
    return gMapHeader.mapLayout != NULL && gMapHeader.mapLayout->primaryTileset == &gTileset_General
        && IsMapTypeOutdoors(gMapHeader.mapType) && !DaemonsIsBlancheOutdoors() && !DaemonsIsTimeless();
}

static u8 DaemonsSeasonHere(void)
{
    return DaemonsSeasonShows() ? DaemonsSeason() : SEASON_SUMMER;
}

// T-416 P4: VIOLET BLOOM -- for the day, the petal slots (palette 0's 9-11) wear the month's purple, light to dark,
// whatever the season has made of them. Where the season shows, and only there.
static bool8 DaemonsBloomHere(u16 offset)
{
    return offset < BG_PLTT_ID(NUM_PALS_IN_PRIMARY) && DaemonsSeasonShows()
        && (DaemonsHolidayFlags(DaemonsHoliday()) & HF_VIOLET_BLOOM);
}

static u16 DaemonsSeasonColour(u16 index, u16 colour, u8 season, bool8 bloom)
{
    u32 i;

    if (bloom && index >= 9 && index <= 11)
        return DaemonsHolidayBloomColour(index - 9);
    if (season == SEASON_SUMMER)
        return colour;
    if (index < 16)
        return (SEASON_SLOT_MASK >> index) & 1 ? sSeasonRow0[season][index] : colour;
    for (i = 0; i < ARRAY_COUNT(sSeasonFollow); i++)
    {
        if (sSeasonFollow[i][0] == index)
            return sSeasonRow0[season][sSeasonFollow[i][1]];
    }
    return colour;
}

// T-408 (the user, 2026-10-19): EACH GROVE'S UNIVERSE IN ITS OWN LIGHT -- the same trees and ground recoloured to the
// universe's mood (DRAFT). Only the grove's own map, only its BG rows as they load (the dialogue rows are never loaded
// here), on top of the season and under the faded print and the watch's light, so dusk still falls in every universe.
// People, daemons and the fir are sprites and keep their colours. HALFTONE's grey outranks it; BREATHVERSE is no grove.
// The table and the shift are DAEMONS tools/gbagrovemood.py's, which reports how near each mood sits to a type.
#include "data/grove_moods.h"

static const struct DaemonsGroveMood *DaemonsGroveMoodHere(void)
{
    u16 here = gSaveBlock1Ptr->location.mapNum | (gSaveBlock1Ptr->location.mapGroup << 8);
    u32 i;

    if (DaemonsIsHalftone() || DaemonsIsTimeless())
        return NULL;
    for (i = 0; i < ARRAY_COUNT(sGroveMoods); i++)
    {
        if (sGroveMoods[i].map == here)
            return &sGroveMoods[i];
    }
    return NULL;
}

static u16 DaemonsGroveColour(u16 colour, const struct DaemonsGroveMood *mood)
{
    s32 ch[3], grey, v, t;
    u32 i;

    ch[0] = colour & 0x1F;
    ch[1] = (colour >> 5) & 0x1F;
    ch[2] = (colour >> 10) & 0x1F;
    grey = (ch[0] * 77 + ch[1] * 150 + ch[2] * 29) >> 8;
    for (i = 0; i < 3; i++)
    {
        v = grey + ((ch[i] - grey) * mood->sat) / 256;
        t = (mood->target[i] * grey) / mood->targetGrey;     // the mood's colour at this colour's own lightness
        if (t > 31)
            t = 31;
        v += ((t - v) * mood->amount) / 256;
        v = 16 + ((v - 16) * mood->contrast) / 256 + mood->lift;
        ch[i] = v < 0 ? 0 : v > 31 ? 31 : v;
    }
    return RGB2(ch[0], ch[1], ch[2]);
}

static void DaemonsClarityEntries(u16 offset, u16 count)
{
    s32 amount = DaemonsClarityFade(), r, g, b, grey;
    u16 i, c;
    u8 season = offset < BG_PLTT_ID(NUM_PALS_IN_PRIMARY) ? DaemonsSeasonHere() : SEASON_SUMMER;
    bool8 bloom = DaemonsBloomHere(offset);
    const struct DaemonsGroveMood *mood = DaemonsGroveMoodHere();

    if (amount == 0 && season == SEASON_SUMMER && !bloom && mood == NULL)
        return;
    for (i = 0; i < count; i++)
    {
        c = DaemonsSeasonColour(offset + i, gPlttBufferUnfaded[offset + i], season, bloom);
        if (mood != NULL)
            c = DaemonsGroveColour(c, mood);
        if (amount == 0)
        {
            gPlttBufferUnfaded[offset + i] = c;
            continue;
        }
        r = c & 0x1F;
        g = (c >> 5) & 0x1F;
        b = (c >> 10) & 0x1F;
        grey = (r * 77 + g * 150 + b * 29) >> 8;
        r += ((grey - r) * amount + 128) / 256;
        g += ((grey - g) * amount + 128) / 256;
        b += ((grey - b) * amount + 128) / 256;
        gPlttBufferUnfaded[offset + i] = RGB2(r, g, b);
    }
    CpuCopy16(&gPlttBufferUnfaded[offset], &gPlttBufferFaded[offset], PLTT_SIZEOF(count));
}

u8 DaemonsFieldTint(void)
{
    if (gGlobalFieldTintMode != QL_TINT_NONE)
        return gGlobalFieldTintMode;
    if (DaemonsIsHalftone())
        return QL_TINT_GRAYSCALE;
    // T-268: the watch's light, outdoors only -- and after HALFTONE, because grey outranks time. T-416 P4: and by
    // day too while a holiday lifts the light (BEACH DAY's bright day on the islands).
    if (IsMapTypeOutdoors(gMapHeader.mapType) && !DaemonsIsTimeless()
     && (DaemonsWatch() != WATCH_DAY || DaemonsHolidayLift() != HOLIDAY_LIFT_NONE))
        return DAEMONS_TINT_WATCH;
    return QL_TINT_NONE;
}

// T-268: what the primary rows were loaded under -- the tint, the watch, and Blanche's wash. A connection reloads only
// the secondary rows, so crossing one compares this with where the player now is and reloads the primary rows if they
// differ (overworld.c). Blanche's wash was the first thing to need it (T-56); HALFTONE's grey and the watch are the rest.
static EWRAM_DATA u16 sPrimarySignature = 0;

u16 DaemonsPaletteSignature(void)
{
    // T-275: and the weekday, whose colour is on the CHECKPOINT's trim in the primary rows
    // T-317: and the clarity, so a connection crossed after an understanding arrives reloads the faded rows
    // T-359: and the season, so a connection crossed as the season turns reloads the rows it repaints
    // T-416 P4: and the holiday's light (bits 6, 14, 15: a lift, or VIOLET BLOOM's petals), or a map crossed as a
    // holiday begins keeps the old palette
    u8 look = DaemonsFieldTint() == DAEMONS_TINT_WATCH ? DaemonsHolidayLift() : HOLIDAY_LIFT_NONE;

    if (look == HOLIDAY_LIFT_NONE && DaemonsBloomHere(0))
        look = HOLIDAY_LIFT_COUNT;
    return ((look & 1) << 6) | ((look >> 1) << 14)
         | (DaemonsClarityFade() != 0 ? (DaemonsClarityFade() > 46 ? 0x2000 : 0x1000) : 0)
         | (DaemonsSeasonHere() << 10) | (DaemonsFieldTint() << 7) | (DaemonsWeekday() << 3) | (DaemonsWatch() << 1) | DaemonsIsBlancheOutdoors();
}

u16 DaemonsPrimarySignature(void)
{
    return sPrimarySignature;
}

// T-430: in HALFTONE (not the unread cave, whose drawing still starts from grey) the grey is DaemonsGreyHalftoneFrame's
// alone, so the colour copy keeps the colour it shows the player and the follower, and the first sight flickers into.
bool8 DaemonsHalftoneGreysByFrame(void)
{
    return gGlobalFieldTintMode == QL_TINT_NONE && DaemonsIsHalftone() && !DaemonsCaveUnperceived();
}

static void ApplyGlobalTintToPaletteEntries(u16 offset, u16 size)
{
    if (DaemonsHalftoneGreysByFrame())
        return;
    switch (DaemonsFieldTint())
    {
    case QL_TINT_NONE:
        return;
    case QL_TINT_GRAYSCALE:
        TintPalette_GrayScale(&gPlttBufferUnfaded[offset], size);
        break;
    case QL_TINT_SEPIA:
        TintPalette_SepiaTone(&gPlttBufferUnfaded[offset], size);
        break;
    case QL_TINT_BACKUP_GRAYSCALE:
        QuestLog_BackUpPalette(offset, size);
        TintPalette_GrayScale(&gPlttBufferUnfaded[offset], size);
        break;
    case DAEMONS_TINT_WATCH:
        DaemonsTintForWatch(&gPlttBufferUnfaded[offset], size);
        break;
    default:
        return;
    }
    CpuCopy16(&gPlttBufferUnfaded[offset], &gPlttBufferFaded[offset], PLTT_SIZEOF(size));
}

//  T-355: one tile colour through everything a tileset colour gets as it loads -- the faded print, then the field's
//  tint -- for the school lift's blue lamp, written after its floor loaded. Without it the blue stood at full colour in
//  a building drawn 40% grey around it, while the green it replaces had faded with the rest (the theatre, T-357).
void DaemonsWashTileColour(u16 offset)
{
    DaemonsClarityEntries(offset, 1);
    ApplyGlobalTintToPaletteEntries(offset, 1);
}

void ApplyGlobalTintToPaletteSlot(u8 slot, u8 count)
{
    if (DaemonsHalftoneGreysByFrame())
        return;
    switch (DaemonsFieldTint())
    {
    case QL_TINT_NONE:
        return;
    case QL_TINT_GRAYSCALE:
        TintPalette_GrayScale(&gPlttBufferUnfaded[BG_PLTT_ID(slot)], count * 16);
        break;
    case QL_TINT_SEPIA:
        TintPalette_SepiaTone(&gPlttBufferUnfaded[BG_PLTT_ID(slot)], count * 16);
        break;
    case QL_TINT_BACKUP_GRAYSCALE:
        QuestLog_BackUpPalette(BG_PLTT_ID(slot), count * 16);
        TintPalette_GrayScale(&gPlttBufferUnfaded[BG_PLTT_ID(slot)], count * 16);
        break;
    case DAEMONS_TINT_WATCH:
        DaemonsTintForWatch(&gPlttBufferUnfaded[BG_PLTT_ID(slot)], count * 16);
        break;
    default:
        return;
    }
    CpuFastCopy(&gPlttBufferUnfaded[BG_PLTT_ID(slot)], &gPlttBufferFaded[BG_PLTT_ID(slot)], count * PLTT_SIZE_4BPP);
}

static void LoadTilesetPalette(struct Tileset const *tileset, u16 destOffset, u16 size)
{
    u16 black = RGB_BLACK;

    if (tileset)
    {
        if (tileset->isSecondary == FALSE)
        {
            LoadPalette(&black, destOffset, PLTT_SIZEOF(1));
            LoadPalette(tileset->palettes[0] + 1, destOffset + 1, size - PLTT_SIZEOF(1));
            // T-275: THE DAY'S COLOUR, ON THE CHECKPOINT'S TRIM. The rotunda draws its trim -- the band round the
            // dome, the courses, the canopy -- in the General tileset's row 2, index 8, which nothing else draws
            // (DAEMONS tools/gbacivic.py). The day's colour goes there before the watch's tint, so dusk and night
            // fall on it too. Sunday first; every colour 22+ from each type's hue (tools/gbadaytrim.py). Unnamed.
            if (tileset == &gTileset_General)
            {
                gPlttBufferUnfaded[destOffset + BG_PLTT_ID(2) + 8] = sDayTrims[DaemonsWeekday()];
                gPlttBufferFaded[destOffset + BG_PLTT_ID(2) + 8] = sDayTrims[DaemonsWeekday()];
            }
            // T-268: Blanche's wash first and the tint over it, or night would be washed back toward white.
            if (DaemonsIsBlancheOutdoors())
                DaemonsPaleEntries(destOffset + 1, (size - 2) >> 1);
            DaemonsClarityEntries(destOffset + 1, (size - 2) >> 1);
            ApplyGlobalTintToPaletteEntries(destOffset + 1, (size - 2) >> 1);
            sPrimarySignature = DaemonsPaletteSignature();
        }
        else if (tileset->isSecondary == TRUE)
        {
            LoadPalette(tileset->palettes[NUM_PALS_IN_PRIMARY], destOffset, size);
            Play_OnTilesetPalettesLoaded();   // T-417 P5: THE NEXUS's tree, lit -- before the tint, so the watch and
                                              // the recap's grey (and the copy it puts back after) fall on the lit tree
            DaemonsClarityEntries(destOffset, size >> 1);
            ApplyGlobalTintToPaletteEntries(destOffset, size >> 1);
        }
        else
        {
            LoadCompressedPalette((const u32 *)tileset->palettes, destOffset, size);
            DaemonsClarityEntries(destOffset, size >> 1);
            ApplyGlobalTintToPaletteEntries(destOffset, size >> 1);
        }
    }
}

void CopyPrimaryTilesetToVram(const struct MapLayout *mapLayout)
{
    CopyTilesetToVram(mapLayout->primaryTileset, NUM_TILES_IN_PRIMARY, 0);
}

void CopySecondaryTilesetToVram(const struct MapLayout *mapLayout)
{
    CopyTilesetToVram(mapLayout->secondaryTileset, NUM_TILES_TOTAL - NUM_TILES_IN_PRIMARY, NUM_TILES_IN_PRIMARY);
}

void CopySecondaryTilesetToVramUsingHeap(const struct MapLayout *mapLayout)
{
    CopyTilesetToVramUsingHeap(mapLayout->secondaryTileset, NUM_TILES_TOTAL - NUM_TILES_IN_PRIMARY, NUM_TILES_IN_PRIMARY);
}

static void LoadPrimaryTilesetPalette(const struct MapLayout *mapLayout)
{
    LoadTilesetPalette(mapLayout->primaryTileset, BG_PLTT_ID(0), NUM_PALS_IN_PRIMARY * PLTT_SIZE_4BPP);
}

// Crossing a connection reloads only the secondary rows. Blanche's wash lives
// in the primary ones too, so walking into or out of Blanche reloads those.
void DaemonsReloadPrimaryTilesetPalette(const struct MapLayout *mapLayout)
{
    LoadPrimaryTilesetPalette(mapLayout);
}

void LoadSecondaryTilesetPalette(const struct MapLayout *mapLayout)
{
    LoadTilesetPalette(mapLayout->secondaryTileset, BG_PLTT_ID(NUM_PALS_IN_PRIMARY), (NUM_PALS_TOTAL - NUM_PALS_IN_PRIMARY) * PLTT_SIZE_4BPP);
}

void CopyMapTilesetsToVram(struct MapLayout const *mapLayout)
{
    if (mapLayout)
    {
        CopyTilesetToVramUsingHeap(mapLayout->primaryTileset, NUM_TILES_IN_PRIMARY, 0);
        CopyTilesetToVramUsingHeap(mapLayout->secondaryTileset, NUM_TILES_TOTAL - NUM_TILES_IN_PRIMARY, NUM_TILES_IN_PRIMARY);
    }
}

void LoadMapTilesetPalettes(struct MapLayout const *mapLayout)
{
    if (mapLayout)
    {
        LoadPrimaryTilesetPalette(mapLayout);
        LoadSecondaryTilesetPalette(mapLayout);
        SchoolLift_OnTilesetPalettesLoaded();     // the school lift's lamp, blue while a ride is arriving
    }
}
