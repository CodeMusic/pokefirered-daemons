#ifndef GUARD_REGION_MAP_H
#define GUARD_REGION_MAP_H

#include "global.h"
#include "bg.h"

enum {
    REGIONMAP_TYPE_NORMAL,
    REGIONMAP_TYPE_WALL,
    REGIONMAP_TYPE_FLY,
    REGIONMAP_TYPE_COUNT
};

u8 *GetMapName(u8 *dest, u16 mapsec, u16 fill);
u8 *GetMapNameGeneric(u8 *dest, u16 mapsec);
u8 *GetMapNameGeneric_(u8 *dest, u16 mapsec);
void InitRegionMapWithExitCB(u8 type, void (*callback)(void));
void InitRegionMapFocusedWithExitCB(u8 type, u8 mapsec, void (*callback)(void));   // T-403
void CB2_OpenFlyMap(void);
bool8 DaemonsMapsecPlace(u8 mapsec, u8 *sheet, u8 *x, u8 *y, u8 *w, u8 *h);   // T-415

#endif // GUARD_REGION_MAP_H
