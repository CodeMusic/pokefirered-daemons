#ifndef GUARD_TRAINER_SEE_H
#define GUARD_TRAINER_SEE_H

#include "global.h"

bool8 CheckForTrainersWantingBattle(void);
void MovementAction_RevealTrainer_RunTrainerSeeFuncList(struct ObjectEvent *var);
u8 FldEff_ExclamationMarkIcon1(void);
u8 FldEff_DoubleExclMarkIcon(void);
u8 FldEff_XIcon(void);
u8 FldEff_SmileyFaceIcon(void);
u8 FldEff_QuestionMarkIcon(void);
// T-18 P2: the same bubbles over a sprite that is not an object event (the follower); the caller's callback moves it.
#define EMOTICON_EXCLAMATION  0
#define EMOTICON_DOUBLE_EXCL  1
#define EMOTICON_X            2
#define EMOTICON_SMILEY       3
#define EMOTICON_QUESTION     4
u8 CreateEmoticonSprite(u8 animNum, void (*callback)(struct Sprite *));

#endif //GUARD_TRAINER_SEE_H
