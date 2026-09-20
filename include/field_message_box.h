#ifndef GUARD_FIELD_MESSAGE_BOX_H
#define GUARD_FIELD_MESSAGE_BOX_H

#include "global.h"

enum {
    FIELD_MESSAGE_BOX_HIDDEN,
    FIELD_MESSAGE_BOX_UNUSED,
    FIELD_MESSAGE_BOX_NORMAL,
    FIELD_MESSAGE_BOX_AUTO_SCROLL,
};

bool8 ShowFieldMessage(const u8 *message);
bool8 ShowFieldAutoScrollMessage(const u8 *message);
void HideFieldMessageBox(void);
bool8 IsFieldMessageBoxHidden(void);
u8 GetFieldMessageBoxType(void);
void InitFieldMessageBox(void);
const u8 *GetLastFieldMessage(void);      // T-179: the line R says again, or NULL before any

#endif // GUARD_FIELD_MESSAGE_BOX_H
