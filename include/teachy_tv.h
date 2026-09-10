#ifndef GUARD_TEACHY_TV_H
#define GUARD_TEACHY_TV_H

enum TeachyTvScript
{
    // The six vanilla shows. The first four hand off to a scripted battle and
    // the last two to the bag, which is the only thing that differs between
    // them -- all six cutscenes are otherwise the same nineteen commands.
    TTVSCR_BATTLE,
    TTVSCR_STATUS,
    TTVSCR_MATCHUPS,
    TTVSCR_CATCHING,
    TTVSCR_TMS,
    TTVSCR_REGISTER,

    // Ours: talk-only shows. sTalkScript is the vanilla cutscene with the
    // hand-off removed, so a lesson is two strings and a table row. The eight
    // MARK shows are gated on the mark being held.
    TTVSCR_HOSTING,
    TTVSCR_MARK1,
    TTVSCR_MARK2,
    TTVSCR_MARK3,
    TTVSCR_MARK4,
    TTVSCR_MARK5,
    TTVSCR_MARK6,
    TTVSCR_MARK7,
    TTVSCR_MARK8,
    TTVSCR_COUNT
};

#define TTVSCR_FIRST_TALK TTVSCR_HOSTING
#define TTV_MENU_MAX      (TTVSCR_COUNT + 1)   // every show, plus CANCEL

void InitTeachyTvController(u8 mode, void (*cb)());
void CB2_ReturnToTeachyTV(void);
void SetTeachyTvControllerModeToResume(void);

#endif //GUARD_TEACHY_TV_H

// T-12: the overworld notification. See src/field_control_avatar.c.
s8 TeachyTvFirstUntoldShow(void);
void TeachyTvMarkShowAsTold(u8 script);
void TeachyTvMarkEveryUnlockedShowAsTold(void);
