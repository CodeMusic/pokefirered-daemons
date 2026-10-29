#ifndef GUARD_DAEMONS_WITCHING_H
#define GUARD_DAEMONS_WITCHING_H

// T-480: the witching hour, the game's half (src/daemons_witching.c).

// The battle menu's hour page answers with this "slot": a fifth choice that is never a slot of the moveset.
#define WITCHING_MENU_SLOT  4
#define WITCHING_PP         5      // uses a battle, per party slot

bool8 Witching_IsHour(void);
bool8 Witching_ShimmerNow(void);   // T-483: the hour, or Halloween with THE HERMETIC LAYER held
u16 Witching_MoveForSpecies(u16 species);

// The shimmer.
u8 Witching_ShimmerWeight(void);
void Witching_ShimmerFadedInPlace(u16 palOffset, u8 weight);
void Witching_ShimmerFromUnfaded(u16 palOffset);
void Witching_BattleShimmer(void);

// The routine, in battle.
void Witching_BattleStart(void);
void Witching_TurnStart(void);
u16 Witching_MoveFor(u8 battler);           // MOVE_NONE when the hour's routine is not offered to it now
u8 Witching_PpLeft(u8 battler);
bool8 Witching_CanChoose(u8 battler);       // offered, with a use left, and not choice-locked
void Witching_SetChosen(u8 battler, bool8 chosen);
bool8 Witching_IsChosen(u8 battler);
bool8 Witching_PpReduce(s32 ppToDeduct);    // Cmd_ppreduce's first question: TRUE when it took the hour's use itself
bool8 Witching_MenuPage(void);
void Witching_SetMenuPage(bool8 on);

#endif // GUARD_DAEMONS_WITCHING_H
