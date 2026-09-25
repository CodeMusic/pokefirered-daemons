#ifndef GUARD_MAP_NAME_POPUP_H
#define GUARD_MAP_NAME_POPUP_H

#include "global.h"

void ShowMapNamePopup(bool32 palIntoFadedBuffer);
void ShowFieldLabelPopup(const u8 *text);   // T-196
void ShowMapLabelPopup(const u8 *text, bool32 palIntoFadedBuffer);
const u8 *GetMapFloorLabel(void);            // T-215: a school floor's subject, or NULL
void DismissMapNamePopup(void);
void MapNamePopup_QueueDayBanner(bool8 showNameFirst, bool32 palIntoFadedBuffer);

#endif //GUARD_MAP_NAME_POPUP_H
