#include "global.h"
#include "gflib.h"
#include "decompress.h"
#include "task.h"
#include "coins.h"
#include "quest_log.h"
#include "overworld.h"
#include "menu.h"
#include "new_menu_helpers.h"
#include "text_window.h"
#include "random.h"
#include "trig.h"
#include "strings.h"
#include "constants/songs.h"
#include "daemons_casino.h"
#include "daemons_house.h"
#include "pokemon_icon.h"

// T-422 (the user, 2026-10-18, confirmed by phone): THE GAME CORNER PLAYS AS THE COMPANION'S CASINO. The reels carry
// daemons -- the lead's (three of it the jackpot), three from the INDEX seen, at home in the season or the hour first --
// a coin and the season's piece (or the holiday's, or the weather's), with the holiday's hats on the daemons and the
// season falling over the reels. The result is decided at the pull from the companion's own table (src/daemons_casino.c,
// its numbers carried in by DAEMONS tools/gencasino.py): between 90 and 100 back of every 100 bet as the GAME's world
// has it, the cat and the string shaping when wins come, and a loss often falling NEAR. Each reel then slips on to its
// piece as the button stops it -- stopping is the fun of it, not a skill. It pays on the middle line only, the bet
// times what slots.ts pays. Its sounds are the companion's TUNE_SLOTS_*, on the day's note. Every new word DRAFT.

#define NUM_REELS 3
#define REEL_LENGTH CASINO_STRIP_LENGTH // each piece twice round a reel (T-422: was 21)

// Only 4 icons are actually visible per reel at a single
// time, with 1 on deck. Only 3 visible when not spinning.
#define REEL_LOAD_LENGTH 5

// 3 horizontal, 2 diagonal
#define NUM_MATCH_LINES 5

#define NUM_BUTTON_TILES 4
#define NUM_DIGIT_SPRITES 4

enum {
    PALSLOT_LINE_NORMAL = 4, // Loaded as part of sBg_Pal
    PALSLOT_LINE_BET,
    PALSLOT_LINE_MATCH,
};

enum {
    GFXTAG_REEL_ICONS,
    GFXTAG_CLEFAIRY,
    GFXTAG_DIGITS,
    GFXTAG_BITS,        // T-422: the season falling over the reels
};

enum {
    PALTAG_REEL_ICONS_0,
    PALTAG_REEL_ICONS_1,
    PALTAG_REEL_ICONS_2,
    PALTAG_REEL_ICONS_3,
    PALTAG_REEL_ICONS_4,
    PALTAG_CLEFAIRY,
    PALTAG_DIGITS,
    PALTAG_BITS,
};

enum {
    ICON_7,
    ICON_ROCKET,
    ICON_PIKACHU,
    ICON_PSYDUCK,   // Psyduck in FR, Slowpoke in LG
    ICON_CHERRIES,
    ICON_MAGNEMITE, // Magnemite in FR, Voltorb in LG
    ICON_SHELLDER,  // Shellder in FR, Staryu in LG
};

#define MIDDLE_LINE 2   // sLineStateTileIdxs' middle row: the one line the CASINO pays on

enum {
    ROWATTR_COL1POS,
    ROWATTR_COL2POS,
    ROWATTR_COL3POS,
    ROWATTR_MINBET,
};

enum {
    SLOTTASK_GFX_INIT,
    SLOTTASK_FADEOUT_EXIT,
    SLOTTASK_UPDATE_LINE_LIGHTS,
    SLOTTASK_CLEFAIRY_BOUNCE,
    SLOTTASK_ANIM_WIN,
    SLOTTASK_END_ANIM_WIN,
    SLOTTASK_ANIM_LOSE,
    SLOTTASK_ANIM_BETTING,
    SLOTTASK_SHOW_AMOUNTS,
    SLOTTASK_MSG_NO_COINS,
    SLOTTASK_ASK_QUIT,
    SLOTTASK_DESTROY_YESNO,
    SLOTTASK_PRESS_BUTTON,
    SLOTTASK_RELEASE_BUTTONS,
    SLOTTASK_SHOWHELP,
    SLOTTASK_HIDEHELP,
};

struct SlotMachineState
{
    // T-422: the CASINO, first so a check can read it (sSlotMachineState): the day, the string, the pull
    struct DaemonsCasinoDay day;
    struct DaemonsCasinoCat cat;
    u16 pullReturn;                 // this pull's, ten-thousandths
    u8 outcome;                     // CASINO_*
    u8 nearWanted;                  // the piece shown one place off the line on the third reel, or 0xFF
    u8 want[NUM_REELS];             // the piece each reel stops on
    u8 note;                        // a word under the reels after the result: 0 none, 1 SO CLOSE, 2 ALMOST, 3 BET BACK
    u8 reels[NUM_REELS][REEL_LENGTH];
    u8 piecePal[CASINO_PIECE_COUNT];
    u16 species[4];
    u32 personality[4];
    u16 artPals[2][16];
    MainCallback savedCallback;
    u16 machineIdx;
    u16 currentReel;
    u16 slotRewardClass;
    u16 bet;
    u8 taskId;
    u8 spinReelsTaskId;
    bool32 reelIsSpinning[NUM_REELS];
    s16 reelPositions[NUM_REELS];
    s16 reelSubpixel[NUM_REELS];
    s16 destReelPos[NUM_REELS];
    s16 reelStopOrder[NUM_REELS];
    bool32 winFlags[NUM_MATCH_LINES];
    u16 payout;
};

struct SlotMachineGfxManager
{
    u32 field_00[NUM_REELS]; // Never read
    struct Sprite *reelIconSprites[NUM_REELS][REEL_LOAD_LENGTH];
    struct Sprite *creditDigitSprites[NUM_DIGIT_SPRITES];
    struct Sprite *payoutDigitSprites[NUM_DIGIT_SPRITES];
    struct Sprite *clefairySprites[2];
    vu16 * reelIconAffineParamPtr;
};

struct SlotMachineSetupTaskDataSub_0000
{
    u16 funcno;
    u8 state;
    bool8 active;
};

struct SlotMachineSetupTaskData
{
    struct SlotMachineSetupTaskDataSub_0000 tasks[8];
    u8 reelButtonToPress;
    // align 2
    s32 bg1X;
    bool32 yesNoMenuActive;
    u16 buttonPressedTiles[NUM_REELS][NUM_BUTTON_TILES];
    u16 buttonReleasedTiles[NUM_REELS][NUM_BUTTON_TILES];
    u8 unusedBuffer[BG_SCREEN_SIZE];
    u8 bg0TilemapBuffer[BG_SCREEN_SIZE];
    u8 bg1TilemapBuffer[BG_SCREEN_SIZE];
    u8 bg2TilemapBuffer[BG_SCREEN_SIZE];
    u8 bg3TilemapBuffer[BG_SCREEN_SIZE];
}; // size: 285C

struct LineStateTileIdxList
{
    const u16 * tiles;
    u32 count;
};

static EWRAM_DATA struct SlotMachineState * sSlotMachineState = NULL;
static EWRAM_DATA struct SlotMachineGfxManager * sSlotMachineGfxManager = NULL;

static void InitSlotMachineState(struct SlotMachineState * ptr);
static void InitCasino(struct SlotMachineState * ptr);
static s16 GetNextReelPosition(u16 whichReel);
static void CB2_InitSlotMachine(void);
static void CleanSupSlotMachineState(void);
static void CB2_RunSlotMachine(void);
static void MainTask_SlotsGameLoop(u8 taskId);
static void MainTask_NoCoinsGameOver(u8 taskId);
static void MainTask_ShowHelp(u8 taskId);
static void MainTask_ConfirmExitGame(u8 taskId);
static void MainTask_DarnNoPayout(u8 taskId);
static void MainTask_WinHandlePayout(u8 taskId);
static void MainTask_ExitSlots(u8 taskId);
static void SetMainTask(TaskFunc taskFunc);
static void Task_SpinReels(u8 taskId);
static void StartReels(void);
static void CasinoSay(const u8 *str);
static void CreateCasinoBits(void);
static bool32 IsReelSpinning(u16);
static u16 CalcPayout(void);
static void InitGfxManager(struct SlotMachineGfxManager * manager);
static void UpdateReelIconSprites(const s16 *, const s16 *);
static bool32 CreateSlotMachine(void);
static void DestroySlotMachine(void);
static struct SlotMachineSetupTaskData * GetSlotMachineSetupTaskDataPtr(void);
static void Task_SlotMachine(u8 taskId);
static void SetSlotMachineSetupTask(u16 funcno, u8 taskId);
static bool32 IsSlotMachineSetupTaskActive(u8 taskId);
static bool8 SlotsTask_GraphicsInit(u8 *state, struct SlotMachineSetupTaskData * ptr);
static bool8 SlotsTask_FadeOut(u8 *state, struct SlotMachineSetupTaskData * ptr);
static bool8 SlotsTask_UpdateLineStates(u8 *state, struct SlotMachineSetupTaskData * ptr);
static bool8 SlotsTask_ClefairyUpdateOnReelsStart(u8 *state, struct SlotMachineSetupTaskData * ptr);
static bool8 SlotsTask_StartClefairyDanceAndWinningLineFlash(u8 *state, struct SlotMachineSetupTaskData * ptr);
static bool8 SlotsTask_StopWinningLineFlashTask(u8 *state, struct SlotMachineSetupTaskData * ptr);
static bool8 SlotsTask_ClefairyFainted(u8 *state, struct SlotMachineSetupTaskData * ptr);
static bool8 SlotsTask_ClefairyNeutral(u8 *state, struct SlotMachineSetupTaskData * ptr);
static bool8 SlotsTask_UpdateCoinsDisplay(u8 *state, struct SlotMachineSetupTaskData * ptr);
static bool8 SlotsTask_MessageOutOfCoins(u8 *state, struct SlotMachineSetupTaskData * ptr);
static bool8 SlotsTask_AskQuitPlaying(u8 *state, struct SlotMachineSetupTaskData * ptr);
static bool8 SlotsTask_DestroyYesNoMenu(u8 *state, struct SlotMachineSetupTaskData * ptr);
static bool8 SlotsTask_PressReelButton(u8 *state, struct SlotMachineSetupTaskData * ptr);
static bool8 SlotsTask_ReleaseReelButtons(u8 *state, struct SlotMachineSetupTaskData * ptr);
static bool8 SlotsTask_ShowHelp(u8 *state, struct SlotMachineSetupTaskData * ptr);
static bool8 SlotsTask_HideHelp(u8 *state, struct SlotMachineSetupTaskData * ptr);
static void Slot_PrintOnWindow0(const u8 * str);
static void Slot_ClearWindow0(void);
static void SetLineStatesByBet(u16 * bgTilemapBuffer);
static void SetLineState(u16 * bgTilemapBuffer, u16 lineId, u16 paletteNo);
static void ClearUnpaidLines(u16 * bgTilemapBuffer);
static void Task_FlashWinningLine(u8 taskId);
static void SignalStopWinningLineFlashTask(void);
static void Slot_CreateYesNoMenu(u8 cursorPos);
static void Slot_DestroyYesNoMenu(void);
static void InitReelButtonTileMem(void);
static void SetReelButtonPressed(u8 reel);
static void ReleaseReelButtons(void);
static void PressReelButton(u8 reel, u8 taskId);

// T-422's surround (the user, 2026-10-18): vanilla's Clefairy dancers are gone -- the lead daemon dances in their
// places, drawn from the reels' own sheet (its JACK piece and its icon's second frame), in its icon palette: no tiles
// or palette of their own (vanilla's took 0xC00 bytes of OBJ tiles and an OBJ palette).
static const u16 sDigits_Pal[]        = INCBIN_U16("graphics/slot_machine/digits.gbapal");
static const u32 sDigits_Tiles[]      = INCBIN_U32("graphics/slot_machine/digits.4bpp.lz");

static const struct CompressedSpriteSheet sSpriteSheets[] = {
    {.data = sDigits_Tiles,    .size = 0x280, .tag = GFXTAG_DIGITS},
};

static const struct SpritePalette sSpritePalettes[] = {
    {.data = sDigits_Pal,       .tag = PALTAG_DIGITS},
    {}
};

static const u16 sReelIconAffineParams[] = {
    0x0120, 0x011f, 0x011e, 0x011d, 0x011c, 0x011b, 0x011a, 0x0119, 0x0118, 0x0117, 0x0116, 0x0115, 0x0114, 0x0113, 0x0112, 0x0111,
    0x0110, 0x010f, 0x010e, 0x010d, 0x010c, 0x010b, 0x010a, 0x0109, 0x0108, 0x0107, 0x0106, 0x0105, 0x0104, 0x0103, 0x0102, 0x0101,
    0x0100, 0x0100, 0x0100, 0x0100, 0x0100, 0x0100, 0x0100, 0x0100, 0x0100, 0x0100, 0x0100, 0x0100, 0x0100, 0x0100, 0x0100, 0x0100,
    0x0100, 0x0100, 0x0100, 0x0100, 0x0100, 0x0100, 0x0100, 0x0100, 0x0100, 0x0100, 0x0100, 0x0100, 0x0100, 0x0100, 0x0100, 0x0100,
    0x0101, 0x0102, 0x0103, 0x0104, 0x0105, 0x0106, 0x0107, 0x0108, 0x0109, 0x010a, 0x010b, 0x010c, 0x010d, 0x010e, 0x010f, 0x0110,
    0x0111, 0x0112, 0x0113, 0x0114, 0x0115, 0x0116, 0x0117, 0x0118, 0x0119, 0x011a, 0x011b, 0x011c, 0x011d, 0x011e, 0x011f, 0x0120
};

static const u8 sReelIconBldY[] = {
    0x10, 0x10, 0x10, 0x10, 0x0f, 0x0e, 0x0d, 0x0d, 0x0c, 0x0b, 0x0a, 0x0a, 0x09, 0x08, 0x07, 0x07, 0x06, 0x05, 0x04, 0x04, 0x03, 0x02, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x02, 0x03, 0x03, 0x04, 0x05, 0x06, 0x06, 0x07, 0x08, 0x09, 0x09, 0x0a, 0x0b, 0x0c, 0x0c, 0x0d, 0x0e, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f
};

static const struct OamData sOamData_ReelIcons = {
    .y = 0,
    .affineMode = ST_OAM_AFFINE_NORMAL,
    .objMode = ST_OAM_OBJ_NORMAL,
    .mosaic = FALSE,
    .bpp = ST_OAM_4BPP,
    .shape = ST_OAM_SQUARE,
    .x = 0,
    .matrixNum = 0,
    .size = ST_OAM_SIZE_2,
    .tileNum = 0,
    .priority = 3,
    .paletteNum = 0,
    .affineParam = 0
};

static const union AnimCmd sAnimCmd_ReelIcon_7[] = {
    ANIMCMD_FRAME(0x00, 4),
    ANIMCMD_END
};

static const union AnimCmd sAnimCmd_ReelIcon_Rocket[] = {
    ANIMCMD_FRAME(0x10, 4),
    ANIMCMD_END
};

static const union AnimCmd sAnimCmd_ReelIcon_Pikachu[] = {
    ANIMCMD_FRAME(0x20, 4),
    ANIMCMD_END
};

static const union AnimCmd sAnimCmd_ReelIcon_Psyduck[] = {
    ANIMCMD_FRAME(0x30, 4),
    ANIMCMD_END
};

static const union AnimCmd sAnimCmd_ReelIcon_Cherries[] = {
    ANIMCMD_FRAME(0x40, 4),
    ANIMCMD_END
};

static const union AnimCmd sAnimCmd_ReelIcon_Magnemite[] = {
    ANIMCMD_FRAME(0x50, 4),
    ANIMCMD_END
};

static const union AnimCmd sAnimCmd_ReelIcon_Shellder[] = {
    ANIMCMD_FRAME(0x60, 4),
    ANIMCMD_END
};

static const union AnimCmd sAnimCmd_ReelIcon_Pikachu_2[] = {
    ANIMCMD_FRAME(0x20, 4),
    ANIMCMD_END
};

static const union AnimCmd sAnimCmd_ReelIcon_Psyduck_2[] = {
    ANIMCMD_FRAME(0x30, 4),
    ANIMCMD_END
};

static const union AnimCmd sAnimCmd_ReelIcon_Cherries_2[] = {
    ANIMCMD_FRAME(0x40, 4),
    ANIMCMD_END
};

static const union AnimCmd sAnimCmd_ReelIcon_Magnemite_2[] = {
    ANIMCMD_FRAME(0x50, 4),
    ANIMCMD_END
};

static const union AnimCmd sAnimCmd_ReelIcon_Shellder_2[] = {
    ANIMCMD_FRAME(0x60, 4),
    ANIMCMD_END
};

static const union AnimCmd *const sAnimTable_ReelIcons[] = {
    [ICON_7]         = sAnimCmd_ReelIcon_7,
    [ICON_ROCKET]    = sAnimCmd_ReelIcon_Rocket,
    [ICON_PIKACHU]   = sAnimCmd_ReelIcon_Pikachu,
    [ICON_PSYDUCK]   = sAnimCmd_ReelIcon_Psyduck,
    [ICON_CHERRIES]  = sAnimCmd_ReelIcon_Cherries,
    [ICON_MAGNEMITE] = sAnimCmd_ReelIcon_Magnemite,
    [ICON_SHELLDER]  = sAnimCmd_ReelIcon_Shellder,
    sAnimCmd_ReelIcon_Pikachu_2,
    sAnimCmd_ReelIcon_Psyduck_2,
    sAnimCmd_ReelIcon_Cherries_2,
    sAnimCmd_ReelIcon_Magnemite_2,
    sAnimCmd_ReelIcon_Shellder_2
};

static const union AffineAnimCmd sAffineAnimCmd_ReelIcons_Unused[] = {
    AFFINEANIMCMD_FRAME(0x100, 0x100, 0, 1),
    AFFINEANIMCMD_END
};

static const union AffineAnimCmd *const sAffineAnimTable_ReelIcons_Unused[] = {
    sAffineAnimCmd_ReelIcons_Unused
};

static const struct SpriteTemplate sSpriteTemplate_ReelIcons = {
    .tileTag = GFXTAG_REEL_ICONS,
    .paletteTag = PALTAG_REEL_ICONS_0,
    .oam = &sOamData_ReelIcons,
    .anims = sAnimTable_ReelIcons,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCallbackDummy
};

static const struct OamData sOamData_Digits = {
    .y = 0,
    .affineMode = ST_OAM_AFFINE_OFF,
    .objMode = ST_OAM_OBJ_NORMAL,
    .mosaic = FALSE,
    .bpp = ST_OAM_4BPP,
    .shape = ST_OAM_V_RECTANGLE,
    .x = 0,
    .matrixNum = 0,
    .size = ST_OAM_SIZE_0,
    .tileNum = 0,
    .priority = 1,
    .paletteNum = 0,
    .affineParam = 0
};

static const union AnimCmd sSpriteAnim_Digit0[] = {
    ANIMCMD_FRAME(0x00, 4),
    ANIMCMD_END
};

static const union AnimCmd sSpriteAnim_Digit1[] = {
    ANIMCMD_FRAME(0x02, 4),
    ANIMCMD_END
};

static const union AnimCmd sSpriteAnim_Digit2[] = {
    ANIMCMD_FRAME(0x04, 4),
    ANIMCMD_END
};

static const union AnimCmd sSpriteAnim_Digit3[] = {
    ANIMCMD_FRAME(0x06, 4),
    ANIMCMD_END
};

static const union AnimCmd sSpriteAnim_Digit4[] = {
    ANIMCMD_FRAME(0x08, 4),
    ANIMCMD_END
};

static const union AnimCmd sSpriteAnim_Digit5[] = {
    ANIMCMD_FRAME(0x0a, 4),
    ANIMCMD_END
};

static const union AnimCmd sSpriteAnim_Digit6[] = {
    ANIMCMD_FRAME(0x0c, 4),
    ANIMCMD_END
};

static const union AnimCmd sSpriteAnim_Digit7[] = {
    ANIMCMD_FRAME(0x0e, 4),
    ANIMCMD_END
};

static const union AnimCmd sSpriteAnim_Digit8[] = {
    ANIMCMD_FRAME(0x10, 4),
    ANIMCMD_END
};

static const union AnimCmd sSpriteAnim_Digit9[] = {
    ANIMCMD_FRAME(0x12, 4),
    ANIMCMD_END
};

static const union AnimCmd *const sAnimTable_Digits[] = {
    sSpriteAnim_Digit0,
    sSpriteAnim_Digit1,
    sSpriteAnim_Digit2,
    sSpriteAnim_Digit3,
    sSpriteAnim_Digit4,
    sSpriteAnim_Digit5,
    sSpriteAnim_Digit6,
    sSpriteAnim_Digit7,
    sSpriteAnim_Digit8,
    sSpriteAnim_Digit9
};

static const struct SpriteTemplate sSpriteTemplate_Digits = {
    .tileTag = GFXTAG_DIGITS,
    .paletteTag = PALTAG_DIGITS,
    .oam = &sOamData_Digits,
    .anims = sAnimTable_Digits,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCallbackDummy
};

static const struct OamData sOamData_Clefairy = {
    .y = 0,
    .affineMode = ST_OAM_AFFINE_OFF,
    .objMode = ST_OAM_OBJ_NORMAL,
    .mosaic = FALSE,
    .bpp = ST_OAM_4BPP,
    .shape = ST_OAM_SQUARE,
    .x = 0,
    .matrixNum = 0,
    .size = ST_OAM_SIZE_2,
    .tileNum = 0,
    .priority = 1,
    .paletteNum = 0,
    .affineParam = 0
};

// the dancers' frames in the reels' sheet: the JACK piece (the lead's icon, its hat on) and the icon's second frame
#define DANCER_FRAME_A (CASINO_JACK * 16)
#define DANCER_FRAME_B (CASINO_PIECE_COUNT * 16)

static const union AnimCmd sAnimCmd_Clefairy_Neutral[] = {
    ANIMCMD_FRAME(DANCER_FRAME_A, 4),
    ANIMCMD_END
};

// the reels turning: the icon's own two steps, as in the party
static const union AnimCmd sAnimCmd_Clefairy_Spinning[] = {
    ANIMCMD_FRAME(DANCER_FRAME_A, 16),
    ANIMCMD_FRAME(DANCER_FRAME_B, 16),
    ANIMCMD_JUMP(0)
};

// a win: the steps quicker, turning about
static const union AnimCmd sAnimCmd_Clefairy_Payout[] = {
    ANIMCMD_FRAME(DANCER_FRAME_A, 8),
    ANIMCMD_FRAME(DANCER_FRAME_B, 8),
    ANIMCMD_FRAME(DANCER_FRAME_A, 8, .hFlip = TRUE),
    ANIMCMD_FRAME(DANCER_FRAME_B, 8, .hFlip = TRUE),
    ANIMCMD_JUMP(0)
};

// a loss: it stands and looks on
static const union AnimCmd sAnimCmd_Clefairy_Lose[] = {
    ANIMCMD_FRAME(DANCER_FRAME_A, 12),
    ANIMCMD_END
};

static const union AnimCmd *const sAnimTable_Clefairy[] = {
    sAnimCmd_Clefairy_Neutral,
    sAnimCmd_Clefairy_Spinning,
    sAnimCmd_Clefairy_Payout,
    sAnimCmd_Clefairy_Lose
};

static const struct SpriteTemplate sSpriteTemplate_Clefairy = {
    .tileTag = GFXTAG_REEL_ICONS,
    .paletteTag = PALTAG_REEL_ICONS_0,   // set to the lead's own as it is made (CreateClefairySprites)
    .oam = &sOamData_Clefairy,
    .anims = sAnimTable_Clefairy,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCallbackDummy
};

bool8 (*const sSlotMachineSetupTasks[])(u8 *, struct SlotMachineSetupTaskData *) = {
    [SLOTTASK_GFX_INIT] = SlotsTask_GraphicsInit,
    [SLOTTASK_FADEOUT_EXIT] = SlotsTask_FadeOut,
    [SLOTTASK_UPDATE_LINE_LIGHTS] = SlotsTask_UpdateLineStates,
    [SLOTTASK_CLEFAIRY_BOUNCE] = SlotsTask_ClefairyUpdateOnReelsStart,
    [SLOTTASK_ANIM_WIN] = SlotsTask_StartClefairyDanceAndWinningLineFlash,
    [SLOTTASK_END_ANIM_WIN] = SlotsTask_StopWinningLineFlashTask,
    [SLOTTASK_ANIM_LOSE] = SlotsTask_ClefairyFainted,
    [SLOTTASK_ANIM_BETTING] = SlotsTask_ClefairyNeutral,
    [SLOTTASK_SHOW_AMOUNTS] = SlotsTask_UpdateCoinsDisplay,
    [SLOTTASK_MSG_NO_COINS] = SlotsTask_MessageOutOfCoins,
    [SLOTTASK_ASK_QUIT] = SlotsTask_AskQuitPlaying,
    [SLOTTASK_DESTROY_YESNO] = SlotsTask_DestroyYesNoMenu,
    [SLOTTASK_PRESS_BUTTON] = SlotsTask_PressReelButton,
    [SLOTTASK_RELEASE_BUTTONS] = SlotsTask_ReleaseReelButtons,
    [SLOTTASK_SHOWHELP] = SlotsTask_ShowHelp,
    [SLOTTASK_HIDEHELP] = SlotsTask_HideHelp
};

#if defined(FIRERED)
static const u16 sBg_Pal[][16]             = INCBIN_U16("graphics/slot_machine/firered/bg.gbapal");
static const u32 sBg_Tiles[]               = INCBIN_U32("graphics/slot_machine/firered/bg.4bpp.lz");
static const u32 sBg_Tilemap[]             = INCBIN_U32("graphics/slot_machine/firered/bg.bin.lz");
static const u16 sBgPal_MatchLines[]       = INCBIN_U16("graphics/slot_machine/firered/match_lines.gbapal");
static const u16 sBgPal_PayoutLight[][16]  = INCBIN_U16("graphics/slot_machine/firered/payout_lights.gbapal");
static const u32 sButtonPressed_Tiles[]    = INCBIN_U32("graphics/slot_machine/firered/button_pressed.4bpp.lz");
static const u16 sCombosWindow_Pal[]       = INCBIN_U16("graphics/slot_machine/firered/combos_window.gbapal");
static const u32 sCombosWindow_Tiles[]     = INCBIN_U32("graphics/slot_machine/firered/combos_window.4bpp.lz");
static const u32 sCombosWindow_Tilemap[]   = INCBIN_U32("graphics/slot_machine/firered/combos_window.bin.lz");
#elif defined(LEAFGREEN)
static const u16 sBg_Pal[][16]             = INCBIN_U16("graphics/slot_machine/leafgreen/bg.gbapal");
static const u32 sBg_Tiles[]               = INCBIN_U32("graphics/slot_machine/leafgreen/bg.4bpp.lz");
static const u32 sBg_Tilemap[]             = INCBIN_U32("graphics/slot_machine/leafgreen/bg.bin.lz");
static const u16 sBgPal_MatchLines[]       = INCBIN_U16("graphics/slot_machine/leafgreen/match_lines.gbapal");
static const u16 sBgPal_PayoutLight[][16]  = INCBIN_U16("graphics/slot_machine/leafgreen/payout_lights.gbapal");
static const u32 sButtonPressed_Tiles[]    = INCBIN_U32("graphics/slot_machine/leafgreen/button_pressed.4bpp.lz");
static const u16 sCombosWindow_Pal[]       = INCBIN_U16("graphics/slot_machine/leafgreen/combos_window.gbapal");
static const u32 sCombosWindow_Tiles[]     = INCBIN_U32("graphics/slot_machine/leafgreen/combos_window.4bpp.lz");
static const u32 sCombosWindow_Tilemap[]   = INCBIN_U32("graphics/slot_machine/leafgreen/combos_window.bin.lz");
#endif

static const struct BgTemplate sBgTemplates[] = {
    {
        .bg = 0,
        .charBaseIndex = 0,
        .mapBaseIndex = 29,
        .screenSize = 0,
        .paletteMode = 0,
        .priority = 0,
        .baseTile = 0x000
    }, {
        .bg = 3,
        .charBaseIndex = 3,
        .mapBaseIndex = 31,
        .screenSize = 0,
        .paletteMode = 0,
        .priority = 3,
        .baseTile = 0x000
    }, {
        .bg = 2,
        .charBaseIndex = 2,
        .mapBaseIndex = 30,
        .screenSize = 0,
        .paletteMode = 0,
        .priority = 2,
        .baseTile = 0x000
    }, {
        .bg = 1,
        .charBaseIndex = 1,
        .mapBaseIndex = 28,
        .screenSize = 0,
        .paletteMode = 0,
        .priority = 0,
        .baseTile = 0x000
    }
};

static const struct WindowTemplate sWindowTemplates[] = {
    {
        .bg = 0,
        .tilemapLeft = 5,
        .tilemapTop = 15,
        .width = 20,
        .height = 4,
        .paletteNum = 15,
        .baseBlock = 0x04f
    }, {
        .bg = 0,
        .tilemapLeft = 0,
        .tilemapTop = 0,
        .width = 30,
        .height = 2,
        .paletteNum = 14,
        .baseBlock = 0x013
    }, {
        // T-422: what the reels pay, and why today is as it is (where vanilla slid in its COMBOS card)
        .bg = 0,
        .tilemapLeft = 2,
        .tilemapTop = 3,     // its frame clear of the bar over the machine (window 1, rows 0-1)
        .width = 26,
        .height = 15,
        .paletteNum = 15,
        .baseBlock = 0x04F   // window 0's and the YES/NO's tiles: neither shows while this does
    },
    DUMMY_WIN_TEMPLATE
};

static const u16 sLineTiles_TLBR[] = {
    0x00a4, 0x00a5, 0x00a6, 0x00c4, 0x00c5, 0x00c6, 0x00c7, 0x00e7, 0x012c, 0x014c, 0x0191, 0x01b1, 0x01f6, 0x0216, 0x0217, 0x0218, 0x0219, 0x0237, 0x0238, 0x0239
};

static const u16 sLineTiles_TopRow[] = {
    0x00e4, 0x00e5, 0x00e6, 0x00f7, 0x00f8, 0x00f9, 0x0104, 0x0105, 0x0106, 0x0107, 0x010c, 0x0111, 0x0116, 0x0117, 0x0118, 0x0119, 0x0124, 0x0125, 0x0126, 0x0137, 0x0138, 0x0139
};

static const u16 sLineTiles_MiddleRow[] = {
    0x0144, 0x0145, 0x0146, 0x0157, 0x0158, 0x0159, 0x0164, 0x0165, 0x0166, 0x0167, 0x016c, 0x0171, 0x0176, 0x0177, 0x0178, 0x0179, 0x0184, 0x0185, 0x0186, 0x0197, 0x0198, 0x0199
};

static const u16 sLineTiles_BottomRow[] = {
    0x01a4, 0x01a5, 0x01a6, 0x01b7, 0x01b8, 0x01b9, 0x01c4, 0x01c5, 0x01c6, 0x01c7, 0x01cc, 0x01d1, 0x01d6, 0x01d7, 0x01d8, 0x01d9, 0x01e4, 0x01e5, 0x01e6, 0x01f7, 0x01f8, 0x01f9
};

static const u16 sLineTiles_BLTR[] = {
    0x0204, 0x0205, 0x0206, 0x0224, 0x0225, 0x0226, 0x01e7, 0x0207, 0x018c, 0x01ac, 0x0131, 0x0151, 0x00d6, 0x00f6, 0x00b7, 0x00b8, 0x00b9, 0x00d7, 0x00d8, 0x00d9
};

static const struct LineStateTileIdxList sLineStateTileIdxs[NUM_MATCH_LINES] = {
    { sLineTiles_TLBR, ARRAY_COUNT(sLineTiles_TLBR) },
    { sLineTiles_TopRow, ARRAY_COUNT(sLineTiles_TopRow) },
    { sLineTiles_MiddleRow, ARRAY_COUNT(sLineTiles_MiddleRow) },
    { sLineTiles_BottomRow, ARRAY_COUNT(sLineTiles_BottomRow) },
    { sLineTiles_BLTR, ARRAY_COUNT(sLineTiles_BLTR) }
};

static const u8 sWinningLineFlashPalIdxs[2] = {2, 4};

static const struct WindowTemplate sYesNoWindowTemplate = {
    .bg = 0,
    .tilemapLeft = 19,
    .tilemapTop = 9,
    .width = 6,
    .height = 4,
    .paletteNum = 15,
    .baseBlock = 0x9F
};

static const u16 sReelButtonMapTileIdxs[NUM_REELS][NUM_BUTTON_TILES] = {
    {0x0229, 0x022a, 0x0249, 0x024a},
    {0x022e, 0x022f, 0x024e, 0x024f},
    {0x0233, 0x0234, 0x0253, 0x0254}
};

void PlaySlotMachine(u16 machineIdx, MainCallback savedCallback)
{
    ResetTasks();
    sSlotMachineState = Alloc(sizeof(*sSlotMachineState));
    if (sSlotMachineState == NULL)
        SetMainCallback2(savedCallback);
    else
    {
        sSlotMachineState->machineIdx = machineIdx;
        sSlotMachineState->savedCallback = savedCallback;
        InitSlotMachineState(sSlotMachineState);
        InitCasino(sSlotMachineState);
        SetMainCallback2(CB2_InitSlotMachine);
    }
}

// T-422: the day, the daemons, the reels -- each piece twice round each reel, shuffled -- and the string at rest
static void InitCasino(struct SlotMachineState * ptr)
{
    s32 i, j, k;
    u8 t;

    DaemonsCasino_ReadDay(&ptr->day);
    DaemonsCasino_ReelSpecies(&ptr->day, ptr->species, ptr->personality);
    ptr->cat.reach = 0;
    ptr->cat.dry = ptr->cat.streak = ptr->cat.pulls = 0;
    ptr->pullReturn = DaemonsCasino_PullReturn(&ptr->day, &ptr->cat);
    ptr->outcome = CASINO_LOSE;
    ptr->nearWanted = 0xFF;
    ptr->note = 0;
    for (i = 0; i < NUM_REELS; i++)
    {
        for (j = 0; j < REEL_LENGTH; j++)
            ptr->reels[i][j] = j % CASINO_PIECE_COUNT;
        for (j = REEL_LENGTH - 1; j > 0; j--)
        {
            k = Random() % (j + 1);
            t = ptr->reels[i][j], ptr->reels[i][j] = ptr->reels[i][k], ptr->reels[i][k] = t;
        }
        ptr->reelPositions[i] = Random() % REEL_LENGTH;
    }
}

// The pull (slots.ts, casino.cpp pull()): the day's luck and the string's reach decide this pull's table; a loss may
// fall near, and two JACKs show a third one place off the line -- almost the jackpot
static void CasinoPull(void)
{
    struct SlotMachineState *st = sSlotMachineState;
    u8 p[3], wanted, third;

    st->pullReturn = DaemonsCasino_PullReturn(&st->day, &st->cat);
    st->outcome = DaemonsCasino_Draw(st->pullReturn);
    DaemonsCasino_PiecesFor(st->outcome, st->want);
    st->nearWanted = 0xFF;
    st->note = 0;
    if (st->outcome == CASINO_LOSE && Random() % 10000 < DaemonsCasino_NearChance(&st->cat))
    {
        if (DaemonsCasino_NearPieces(st->reels[2], p, &wanted))
        {
            st->want[0] = p[0], st->want[1] = p[1], st->want[2] = p[2];
            st->nearWanted = wanted;
        }
    }
    else if (st->outcome == CASINO_JACK2 && DaemonsCasino_AlmostThird(st->reels[2], &third))
    {
        st->want[2] = third;
        st->nearWanted = CASINO_JACK;
    }
    DaemonsCasino_PlayTune(CASINO_TUNE_PULL, st->day.dayNote);
}

static u8 PieceAt(u16 reel, s32 pos)
{
    return sSlotMachineState->reels[reel][((pos % REEL_LENGTH) + REEL_LENGTH) % REEL_LENGTH];
}

// A reel stops on its piece: on (never back) to the first place its piece shows on the middle line -- and the third
// reel of a near miss where the piece it wanted sits beside it, one place off the line, if such a place comes round
static void StopCasinoReel(u16 reel)
{
    struct SlotMachineState *st = sSlotMachineState;
    s16 next = GetNextReelPosition(reel);
    s32 i, pos;

    st->reelStopOrder[0] = reel;
    if (reel == 2 && st->nearWanted != 0xFF)
    {
        for (i = 0; i < REEL_LENGTH; i++)
        {
            pos = next - i;
            if (PieceAt(reel, pos + 2) == st->want[reel]
             && (PieceAt(reel, pos + 1) == st->nearWanted || PieceAt(reel, pos + 3) == st->nearWanted))
            {
                st->destReelPos[reel] = ((pos % REEL_LENGTH) + REEL_LENGTH) % REEL_LENGTH;
                return;
            }
        }
    }
    for (i = 0; i < REEL_LENGTH; i++)
    {
        pos = next - i;
        if (PieceAt(reel, pos + 2) == st->want[reel])
            break;
    }
    st->destReelPos[reel] = (((next - i) % REEL_LENGTH) + REEL_LENGTH) % REEL_LENGTH;
}

static void InitSlotMachineState(struct SlotMachineState * ptr)
{
    s32 i;

    ptr->currentReel = 0;
    ptr->bet = 0;
    ptr->payout = 0;
    // for whatever reason, the loop does not use the ptr param
    for (i = 0; i < NUM_REELS; i++)
    {
        sSlotMachineState->reelIsSpinning[i] = FALSE;
        sSlotMachineState->reelPositions[i] = 0;
        sSlotMachineState->reelSubpixel[i] = 0;
        sSlotMachineState->destReelPos[i] = REEL_LENGTH;
    }
}

static void CB2_InitSlotMachine(void)
{
    RunTasks();
    AnimateSprites();
    BuildOamBuffer();

    switch (gMain.state)
    {
    case 0:
        if (CreateSlotMachine())
        {
            SetMainCallback2(sSlotMachineState->savedCallback);
            CleanSupSlotMachineState();
        }
        else
        {
            SetSlotMachineSetupTask(SLOTTASK_GFX_INIT, 0);
            gMain.state++;
        }
        break;
    case 1:
        if (!IsSlotMachineSetupTaskActive(0))
        {
            sSlotMachineState->taskId = CreateTask(MainTask_SlotsGameLoop, 0);
            sSlotMachineState->spinReelsTaskId = CreateTask(Task_SpinReels, 1);
            SetMainCallback2(CB2_RunSlotMachine);
        }
        break;
    }
}

static void CleanSupSlotMachineState(void)
{
    DestroySlotMachine();
    if (sSlotMachineState != NULL)
    {
        Free(sSlotMachineState);
        sSlotMachineState = NULL;
    }
}

static void CB2_RunSlotMachine(void)
{
    RunTasks();
    AnimateSprites();
    BuildOamBuffer();
    UpdatePaletteFade();
}

static void MainTask_SlotsGameLoop(u8 taskId)
{
    s16 * data = gTasks[taskId].data;

    switch (data[0])
    {
    case 0:
        // Betting Phase
        if (GetCoins() == 0)
        {
            SetMainTask(MainTask_NoCoinsGameOver);
        }
        else if (JOY_NEW(DPAD_DOWN))
        {
            sSlotMachineState->bet++;
            RemoveCoins(1);
            PlaySE(SE_RS_SHOP);
            SetSlotMachineSetupTask(SLOTTASK_SHOW_AMOUNTS, 0);
            SetSlotMachineSetupTask(SLOTTASK_UPDATE_LINE_LIGHTS, 1);
            data[0] = 1;
        }
        else if (JOY_NEW(R_BUTTON))
        {
            s32 toAdd = 3 - sSlotMachineState->bet;
            if (GetCoins() >= toAdd)
            {
                sSlotMachineState->bet = 3;
                RemoveCoins(toAdd);
            }
            else
            {
                sSlotMachineState->bet += GetCoins();
                SetCoins(0);
            }
            PlaySE(SE_RS_SHOP);
            SetSlotMachineSetupTask(SLOTTASK_SHOW_AMOUNTS, 0);
            SetSlotMachineSetupTask(SLOTTASK_UPDATE_LINE_LIGHTS, 1);
            data[0] = 1;
        }
        else if (JOY_NEW(A_BUTTON) && sSlotMachineState->bet != 0)
        {
            data[0] = 2;
        }
        else if (JOY_NEW(B_BUTTON))
        {
            SetMainTask(MainTask_ConfirmExitGame);
        }
        else if (JOY_NEW(DPAD_RIGHT))
        {
            SetMainTask(MainTask_ShowHelp);
        }
        break;
    case 1:
        if (!IsSlotMachineSetupTaskActive(0) && !IsSlotMachineSetupTaskActive(1))
        {
            if (sSlotMachineState->bet == 3 || GetCoins() == 0)
                data[0] = 2;
            else
                data[0] = 0;
        }
        break;
    case 2:
        SetQLPlayedTheSlots();
        CasinoPull();
        StartReels();
        sSlotMachineState->currentReel = 0;
        SetSlotMachineSetupTask(SLOTTASK_CLEFAIRY_BOUNCE, 0);
        data[0] = 3;
        break;
    case 3:
        if (!IsSlotMachineSetupTaskActive(0))
        {
            if (JOY_NEW(A_BUTTON))
            {
                PlaySE(SE_CONTEST_PLACE);
                StopCasinoReel(sSlotMachineState->currentReel);
                PressReelButton(sSlotMachineState->currentReel, 0);
                data[0] = 4;
            }
        }
        break;
    case 4:
        if (IsReelSpinning(sSlotMachineState->currentReel) == 0 && !IsSlotMachineSetupTaskActive(0))
        {
            sSlotMachineState->currentReel++;
            if (sSlotMachineState->currentReel >= NUM_REELS)
            {
                sSlotMachineState->slotRewardClass = CalcPayout();
                sSlotMachineState->bet = 0;
                sSlotMachineState->currentReel = 0;
                DaemonsCasino_Learn(&sSlotMachineState->cat, sSlotMachineState->outcome);   // the string learns
                if (sSlotMachineState->slotRewardClass == CASINO_LOSE)
                    SetMainTask(MainTask_DarnNoPayout);
                else
                {
                    if (sSlotMachineState->slotRewardClass == CASINO_JACK3)
                        IncrementGameStat(GAME_STAT_SLOT_JACKPOTS);
                    SetMainTask(MainTask_WinHandlePayout);
                }
            }
            else
                data[0] = 3;
        }
        break;
    }
}

static void MainTask_NoCoinsGameOver(u8 taskId)
{
    s16 * data = gTasks[taskId].data;

    switch (data[0])
    {
    case 0:
        SetSlotMachineSetupTask(SLOTTASK_MSG_NO_COINS, 0);
        data[0]++;
        break;
    case 1:
        if (!IsSlotMachineSetupTaskActive(0))
            data[0]++;
        break;
    case 2:
        if (JOY_NEW(A_BUTTON | B_BUTTON | DPAD_ANY))
            SetMainTask(MainTask_ExitSlots);
        break;
    }
}

static void MainTask_ShowHelp(u8 taskId)
{
    s16 * data = gTasks[taskId].data;

    switch (data[0])
    {
    case 0:
        SetSlotMachineSetupTask(SLOTTASK_SHOWHELP, 0);
        data[0]++;
        break;
    case 1:
        if (!IsSlotMachineSetupTaskActive(0))
            data[0]++;
        break;
    case 2:
        if (JOY_NEW(DPAD_LEFT))
        {
            SetSlotMachineSetupTask(SLOTTASK_HIDEHELP, 0);
            data[0]++;
        }
        break;
    case 3:
        if (!IsSlotMachineSetupTaskActive(0))
            SetMainTask(MainTask_SlotsGameLoop);
        break;
    }
}

static void MainTask_ConfirmExitGame(u8 taskId)
{
    s16 * data = gTasks[taskId].data;

    switch (data[0])
    {
    case 0:
        SetSlotMachineSetupTask(SLOTTASK_ASK_QUIT, 0);
        data[0]++;
        break;
    case 1:
        if (!IsSlotMachineSetupTaskActive(0))
            data[0]++;
        break;
    case 2:
        switch (Menu_ProcessInputNoWrapClearOnChoose())
        {
        case 0:
            AddCoins(sSlotMachineState->bet);
            SetSlotMachineSetupTask(SLOTTASK_SHOW_AMOUNTS, 0);
            data[0] = 3;
            break;
        case 1:
        case -1:
            SetSlotMachineSetupTask(SLOTTASK_DESTROY_YESNO, 0);
            data[0] = 4;
            break;
        }
        break;
    case 3:
        if (!IsSlotMachineSetupTaskActive(0))
            SetMainTask(MainTask_ExitSlots);
        break;
    case 4:
        if (!IsSlotMachineSetupTaskActive(0))
            SetMainTask(MainTask_SlotsGameLoop);
        break;
    }
}

static void MainTask_DarnNoPayout(u8 taskId)
{
    s16 * data = gTasks[taskId].data;

    switch (data[0])
    {
    case 0:
        SetSlotMachineSetupTask(SLOTTASK_ANIM_LOSE, 0);
        DaemonsCasino_PlayTune(CASINO_TUNE_LOSE, sSlotMachineState->day.dayNote);
        if (sSlotMachineState->nearWanted != 0xFF)
            CasinoSay(gText_CasinoSoClose);
        data[1] = 0;
        data[0]++;
        break;
    case 1:
        data[1]++;
        if (data[1] > 60)
        {
            CasinoSay(NULL);
            SetSlotMachineSetupTask(SLOTTASK_ANIM_BETTING, 0);
            SetSlotMachineSetupTask(SLOTTASK_UPDATE_LINE_LIGHTS, 1);
            SetSlotMachineSetupTask(SLOTTASK_RELEASE_BUTTONS, 2);
            data[0]++;
        }
        break;
    case 2:
        if (!IsSlotMachineSetupTaskActive(0) && !IsSlotMachineSetupTaskActive(1) && !IsSlotMachineSetupTaskActive(2))
            SetMainTask(MainTask_SlotsGameLoop);
        break;
    }
}

static void MainTask_WinHandlePayout(u8 taskId)
{
    s16 * data = gTasks[taskId].data;

    switch (data[0])
    {
    case 0:
        // casino.cpp: the bet back is counted out in coins; a win plays its tune, a big one a holiday's own
        if (DaemonsCasino_Pays(sSlotMachineState->outcome) > 1)
            DaemonsCasino_PlayTune(DaemonsCasino_WinTune(sSlotMachineState->outcome, sSlotMachineState->day.look),
                                   sSlotMachineState->day.dayNote);
        if (sSlotMachineState->outcome == CASINO_JACK2 && sSlotMachineState->nearWanted == CASINO_JACK)
            CasinoSay(gText_CasinoAlmost);
        else if (DaemonsCasino_Pays(sSlotMachineState->outcome) == 1)
            CasinoSay(gText_CasinoBetBack);
        SetSlotMachineSetupTask(SLOTTASK_SHOW_AMOUNTS, 0);
        SetSlotMachineSetupTask(SLOTTASK_ANIM_WIN, 1);
        data[1] = 8;
        data[0]++;
        break;
    case 1:
        data[1]++;
        if (data[1] > 120)
        {
            data[1] = 8;
            if (JOY_HELD(A_BUTTON))
                data[1] = 2;
            data[0]++;
        }
        break;
    case 2:
        if (!IsSlotMachineSetupTaskActive(0))
        {
            if (!DaemonsCasino_TunePlaying() && JOY_NEW(START_BUTTON))
            {
                AddCoins(sSlotMachineState->payout);
                sSlotMachineState->payout = 0;
            }
            else
            {
                data[1]--;
                if (data[1] == 0)
                {
                    if (!DaemonsCasino_TunePlaying())
                        PlaySE(SE_PIN);
                    if (sSlotMachineState->payout != 0)
                    {
                        AddCoins(1);
                        sSlotMachineState->payout--;
                    }
                    data[1] = 8;
                    if (JOY_HELD(A_BUTTON))
                        data[1] = 2;
                }
            }
            SetSlotMachineSetupTask(SLOTTASK_SHOW_AMOUNTS, 0);
            if (sSlotMachineState->payout == 0)
                data[0]++;
        }
        break;
    case 3:
        if (!DaemonsCasino_TunePlaying() && !IsSlotMachineSetupTaskActive(0))
        {
            SetSlotMachineSetupTask(SLOTTASK_END_ANIM_WIN, 0);
            data[0]++;
        }
        break;
    case 4:
        if (!IsSlotMachineSetupTaskActive(0))
        {
            CasinoSay(NULL);
            SetSlotMachineSetupTask(SLOTTASK_UPDATE_LINE_LIGHTS, 0);
            SetSlotMachineSetupTask(SLOTTASK_RELEASE_BUTTONS, 1);
            data[0]++;
        }
        break;
    case 5:
        if (!IsSlotMachineSetupTaskActive(0) && !IsSlotMachineSetupTaskActive(1))
            SetMainTask(MainTask_SlotsGameLoop);
        break;
    }
}

static void MainTask_ExitSlots(u8 taskId)
{
    s16 * data = gTasks[taskId].data;

    switch (data[0])
    {
    case 0:
        DaemonsCasino_StopTune();
        SetSlotMachineSetupTask(SLOTTASK_FADEOUT_EXIT, 0);
        data[0]++;
        break;
    case 1:
        if (!IsSlotMachineSetupTaskActive(0))
        {
            SetMainCallback2(sSlotMachineState->savedCallback);
            CleanSupSlotMachineState();
        }
        break;
    }
}

static void SetMainTask(TaskFunc taskFunc)
{
    gTasks[sSlotMachineState->taskId].func = taskFunc;
    gTasks[sSlotMachineState->taskId].data[0] = 0;
}

static void Task_SpinReels(u8 taskId)
{
    // taskId is never used

    s32 i;

    for (i = 0; i < NUM_REELS; i++)
    {
        if (sSlotMachineState->reelIsSpinning[i] || sSlotMachineState->reelSubpixel[i] != 0)
        {
            if (sSlotMachineState->reelSubpixel[i] != 0 || sSlotMachineState->reelPositions[i] != sSlotMachineState->destReelPos[i])
            {
                sSlotMachineState->reelSubpixel[i]++;
                if (sSlotMachineState->reelSubpixel[i] > 2)
                {
                    sSlotMachineState->reelSubpixel[i] = 0;
                    sSlotMachineState->reelPositions[i]--;
                    if (sSlotMachineState->reelPositions[i] < 0)
                        sSlotMachineState->reelPositions[i] = REEL_LENGTH - 1;
                }
                if (sSlotMachineState->reelPositions[i] != sSlotMachineState->destReelPos[i])
                    continue;
            }
            sSlotMachineState->destReelPos[i] = REEL_LENGTH;
            sSlotMachineState->reelIsSpinning[i] = FALSE;
        }
    }
    UpdateReelIconSprites(sSlotMachineState->reelPositions, sSlotMachineState->reelSubpixel);
}

static void StartReels(void)
{
    s32 i;

    for (i = 0; i < NUM_REELS; i++)
        sSlotMachineState->reelIsSpinning[i] = TRUE;
}

static bool32 IsReelSpinning(u16 whichReel)
{
    return sSlotMachineState->reelIsSpinning[whichReel];
}

static s16 GetNextReelPosition(u16 whichReel)
{
    s16 position = sSlotMachineState->reelPositions[whichReel];
    if (sSlotMachineState->reelSubpixel[whichReel] != 0)
    {
        position--;
        if (position < 0)
            position = REEL_LENGTH - 1;
    }
    return position;
}

// What the pull came to: the bet times what slots.ts pays, on the middle line
static u16 CalcPayout(void)
{
    s32 i;

    for (i = 0; i < NUM_MATCH_LINES; i++)
        sSlotMachineState->winFlags[i] = FALSE;
    sSlotMachineState->payout = sSlotMachineState->bet * DaemonsCasino_Pays(sSlotMachineState->outcome);
    DaemonsHouse_AddPull(sSlotMachineState->bet, sSlotMachineState->payout);   // T-427: what the house keeps
    if (sSlotMachineState->outcome != CASINO_LOSE)
        sSlotMachineState->winFlags[MIDDLE_LINE] = TRUE;
    return sSlotMachineState->outcome;
}

static u16 GetPayout(void)
{
    return sSlotMachineState->payout;
}

static u8 GetPlayerBet(void)
{
    return sSlotMachineState->bet;
}

static bool32 GetWinFlagByLine(int lineId)
{
    return sSlotMachineState->winFlags[lineId];
}

// T-422: the six pieces, built for this sitting -- the daemons' icons (a holiday's hat on each), the coin, the season's
// piece -- and the falling season's tiles and palettes
static void LoadCasinoGraphics(void)
{
    struct SlotMachineState *st = sSlotMachineState;
    struct SpriteSheet sheet;
    struct SpritePalette pal;
    u16 bitsPal[16];
    u32 size, i;
    u8 *tiles = Alloc((CASINO_PIECE_COUNT + 1) * 0x200);

    if (tiles != NULL)
    {
        DaemonsCasino_BuildPieces(&st->day, st->species, st->personality, tiles, st->piecePal, st->artPals);
        DaemonsCasino_BuildDancer(&st->day, st->species[0], st->personality[0], tiles + CASINO_PIECE_COUNT * 0x200);
        sheet.data = tiles;
        sheet.size = (CASINO_PIECE_COUNT + 1) * 0x200;   // the six pieces and the dancers' second frame
        sheet.tag = GFXTAG_REEL_ICONS;
        LoadSpriteSheet(&sheet);
        Free(tiles);
    }
    for (i = 0; i < 3; i++)
    {
        pal.data = gMonIconPalettes[i];
        pal.tag = PALTAG_REEL_ICONS_0 + i;
        LoadSpritePalette(&pal);
    }
    pal.data = st->artPals[0], pal.tag = PALTAG_REEL_ICONS_3;
    LoadSpritePalette(&pal);
    pal.data = st->artPals[1], pal.tag = PALTAG_REEL_ICONS_4;
    LoadSpritePalette(&pal);
    sheet.data = DaemonsCasino_BitTiles(&size);
    sheet.size = size;
    sheet.tag = GFXTAG_BITS;
    LoadSpriteSheet(&sheet);
    DaemonsCasino_BitsPalette(&st->day, bitsPal);
    pal.data = bitsPal, pal.tag = PALTAG_BITS;
    LoadSpritePalette(&pal);
}

static bool32 LoadSpriteGraphicsAndAllocateManager(void)
{
    s32 i;

    for (i = 0; i < ARRAY_COUNT(sSpriteSheets); i++)
        LoadCompressedSpriteSheet(&sSpriteSheets[i]);
    LoadCasinoGraphics();
    LoadSpritePalettes(sSpritePalettes);
    sSlotMachineGfxManager = Alloc(sizeof(*sSlotMachineGfxManager));
    if (sSlotMachineGfxManager == NULL)
        return FALSE;
    InitGfxManager(sSlotMachineGfxManager);
    return TRUE;
}

static void DestroyGfxManager(void)
{
    if (sSlotMachineGfxManager != NULL)
    {
        Free(sSlotMachineGfxManager);
        sSlotMachineGfxManager = NULL;
    }
}

static void InitGfxManager(struct SlotMachineGfxManager * manager)
{
    s32 i, j;

    for (i = 0; i < NUM_REELS; i++)
    {
        manager->field_00[i] = 0;
        for (j = 0; j < REEL_LOAD_LENGTH; j++)
            manager->reelIconSprites[i][j] = NULL;
    }
}

static void CreateReelIconSprites(void)
{
    struct Sprite *sprite;
    s32 i, j;
    s32 spriteId;
    s32 animId;
    for (i = 0; i < NUM_REELS; i++)
    {
        for (j = 0; j < REEL_LOAD_LENGTH; j++)
        {
            spriteId = CreateSprite(&sSpriteTemplate_ReelIcons, 80 + 40 * i, 44 + 24 * j, 2);
            animId = PieceAt(i, sSlotMachineState->reelPositions[i] + j);
            sprite = &gSprites[spriteId];
            StartSpriteAnim(sprite, animId);
            sprite->oam.paletteNum = IndexOfSpritePaletteTag(PALTAG_REEL_ICONS_0 + sSlotMachineState->piecePal[animId]);
            sprite->data[0] = i;
            sprite->data[1] = j;
            sprite->data[2] = j;
            sprite->data[3] = 0;
            sprite->oam.matrixNum = 0;
            sSlotMachineGfxManager->reelIconSprites[i][j] = sprite;
            sSlotMachineGfxManager->reelIconAffineParamPtr = (vu16 *)(OAM + 0 * sizeof(struct OamData) + offsetof(struct OamData, affineParam));
        }
    }
}

static void UpdateReelIconSprites(const s16 * reelPosPtr, const s16 * yposPtr)
{
    s32 i, j;
    s32 reelPos, ypos;

    for (i = 0; i < NUM_REELS; i++)
    {
        reelPos = *reelPosPtr;
        ypos = *yposPtr * 8;
        for (j = 0; j < REEL_LOAD_LENGTH; j++)
        {
            sSlotMachineGfxManager->reelIconSprites[i][j]->y2 = ypos;
            {
                s32 animId = PieceAt(i, reelPos);
                struct Sprite *sprite = sSlotMachineGfxManager->reelIconSprites[i][j];
                if (sprite->animNum != animId)
                    StartSpriteAnim(sprite, animId);
                sprite->oam.paletteNum = IndexOfSpritePaletteTag(PALTAG_REEL_ICONS_0 + sSlotMachineState->piecePal[animId]);
            }
            reelPos++;
            if (reelPos >= REEL_LENGTH)
                reelPos = 0;
        }
        reelPosPtr++;
        yposPtr++;
    }
}

static void HBlankCB_SlotMachine(void)
{
    s32 vcount = REG_VCOUNT - 0x2B;
    if (vcount < 0x54u)
    {
        *sSlotMachineGfxManager->reelIconAffineParamPtr = sReelIconAffineParams[vcount];
        REG_BLDY = sReelIconBldY[vcount];
    }
    else
    {
        *sSlotMachineGfxManager->reelIconAffineParamPtr = 0x100;
        REG_BLDY = 0;
    }
}

static void CreateScoreDigitSprites(void)
{
    s32 i;
    s32 spriteId;

    for (i = 0; i < NUM_DIGIT_SPRITES; i++)
    {
        spriteId = CreateSprite(&sSpriteTemplate_Digits, 85 + 7 * i, 30, 0);
        sSlotMachineGfxManager->creditDigitSprites[i] = &gSprites[spriteId];
        spriteId = CreateSprite(&sSpriteTemplate_Digits, 133 + 7 * i, 30, 0);
        sSlotMachineGfxManager->payoutDigitSprites[i] = &gSprites[spriteId];
    }
}

static void UpdateCoinsDisplay(void)
{
    s32 coins = GetCoins();
    s32 payout = GetPayout();
    s32 i;
    s32 divisor = 1000;
    s32 quotient;

    for (i = 0; i < NUM_DIGIT_SPRITES; i++)
    {
        quotient = coins / divisor;
        StartSpriteAnim(sSlotMachineGfxManager->creditDigitSprites[i], quotient);
        coins -= quotient * divisor;
        quotient = payout / divisor;
        StartSpriteAnim(sSlotMachineGfxManager->payoutDigitSprites[i], quotient);
        payout -= quotient * divisor;
        divisor /= 10;
    }
}

// T-422's surround: the lead daemon in both places, each turned to face the reels (an icon faces left)
static void CreateClefairySprites(void)
{
    u8 palNum = IndexOfSpritePaletteTag(PALTAG_REEL_ICONS_0 + sSlotMachineState->piecePal[CASINO_JACK]);
    s32 spriteId = CreateSprite(&sSpriteTemplate_Clefairy, 16, 136, 1);
    sSlotMachineGfxManager->clefairySprites[0] = &gSprites[spriteId];
    sSlotMachineGfxManager->clefairySprites[0]->hFlip = TRUE;
    sSlotMachineGfxManager->clefairySprites[0]->oam.paletteNum = palNum;
    spriteId = CreateSprite(&sSpriteTemplate_Clefairy, DISPLAY_WIDTH - 16, 136, 1);
    sSlotMachineGfxManager->clefairySprites[1] = &gSprites[spriteId];
    sSlotMachineGfxManager->clefairySprites[1]->oam.paletteNum = palNum;
}

static void SetClefairySpriteAnim(u8 animId)
{
    s32 i;
    for (i = 0; i < (int)ARRAY_COUNT(sSlotMachineGfxManager->clefairySprites); i++)
        StartSpriteAnim(sSlotMachineGfxManager->clefairySprites[i], animId);
}

static bool32 CreateSlotMachine(void)
{
    s32 i;

    struct SlotMachineSetupTaskData * ptr = Alloc(sizeof(struct SlotMachineSetupTaskData));
    if (ptr == NULL)
        return FALSE;
    for (i = 0; i < (int)ARRAY_COUNT(ptr->tasks); i++)
        ptr->tasks[i].active = 0;
    ptr->yesNoMenuActive = FALSE;
    SetWordTaskArg(CreateTask(Task_SlotMachine, 2), 0, (uintptr_t)ptr);
    return FALSE;
}

static void DestroySlotMachine(void)
{
    if (FuncIsActiveTask(Task_SlotMachine))
    {
        Free(GetSlotMachineSetupTaskDataPtr());
        DestroyTask(FindTaskIdByFunc(Task_SlotMachine));
    }
    DestroyGfxManager();
    FreeAllWindowBuffers();
}

static void Task_SlotMachine(u8 taskId)
{
    struct SlotMachineSetupTaskData * ptr = (void *)GetWordTaskArg(taskId, 0);
    s32 i;

    for (i = 0; i < (int)ARRAY_COUNT(ptr->tasks); i++)
    {
        if (ptr->tasks[i].active)
            ptr->tasks[i].active = sSlotMachineSetupTasks[ptr->tasks[i].funcno](&ptr->tasks[i].state, ptr);
    }
}

static void VBlankCB_SlotMachine(void)
{
    TransferPlttBuffer();
    LoadOam();
    ProcessSpriteCopyRequests();
}

static struct SlotMachineSetupTaskData * GetSlotMachineSetupTaskDataPtr(void)
{
    return (void *)GetWordTaskArg(FindTaskIdByFunc(Task_SlotMachine), 0);
}

static void SetSlotMachineSetupTask(u16 funcno, u8 taskId)
{
    struct SlotMachineSetupTaskData * ptr = GetSlotMachineSetupTaskDataPtr();
    ptr->tasks[taskId].funcno = funcno;
    ptr->tasks[taskId].state = 0;
    ptr->tasks[taskId].active = sSlotMachineSetupTasks[funcno](&ptr->tasks[taskId].state, ptr);
}

static bool32 IsSlotMachineSetupTaskActive(u8 taskId)
{
    return GetSlotMachineSetupTaskDataPtr()->tasks[taskId].active;
}

static inline void SetBackdropColor(u16 color, u16 *pal)
{
    *pal = color;
    SetBackdropFromPalette(pal);
}

static bool8 SlotsTask_GraphicsInit(u8 * state, struct SlotMachineSetupTaskData * ptr)
{
    u16 pal[2];
    u8 textColor[3];
    u32 x;

    switch (*state)
    {
    case 0:
        BlendPalettes(PALETTES_ALL, 16, RGB_BLACK);
        (*state)++;
        break;
    case 1:
        SetVBlankCallback(NULL);
        ResetSpriteData();
        FreeAllSpritePalettes();
        RequestDma3Fill(0, (void *)OAM, OAM_SIZE, DMA3_32BIT);
        RequestDma3Fill(0, (void *)VRAM, 0x20, DMA3_32BIT);
        RequestDma3Fill(0, (void *)(VRAM + 0xC000), 0x20, DMA3_32BIT);
        SetGpuReg(REG_OFFSET_DISPCNT, 0);
        ResetBgPositions();
        ResetBgsAndClearDma3BusyFlags(FALSE);
        InitBgsFromTemplates(0, sBgTemplates, ARRAY_COUNT(sBgTemplates));
        InitWindows(sWindowTemplates);

        SetBgTilemapBuffer(3, ptr->bg3TilemapBuffer);
        FillBgTilemapBufferRect_Palette0(3, 0, 0, 0, 32, 32);
        CopyBgTilemapBufferToVram(3);

        ResetTempTileDataBuffers();
        DecompressAndCopyTileDataToVram(2, sBg_Tiles, 0, 0x00, 0);
        DecompressAndCopyTileDataToVram(2, sButtonPressed_Tiles, 0, 0xC0, 0);
        SetBgTilemapBuffer(2, ptr->bg2TilemapBuffer);
        CopyToBgTilemapBuffer(2, sBg_Tilemap, 0, 0x00);
        ClearUnpaidLines(GetBgTilemapBuffer(2));
        CopyBgTilemapBufferToVram(2);
        LoadPalette(sBg_Pal, BG_PLTT_ID(0), sizeof(sBg_Pal));
        LoadPalette(sBgPal_MatchLines, BG_PLTT_ID(PALSLOT_LINE_BET), sizeof(sBgPal_MatchLines));
        LoadPalette(sCombosWindow_Pal, BG_PLTT_ID(7), sizeof(sCombosWindow_Pal));
        SetBackdropColor(RGB(30, 30, 31), pal);
        LoadUserWindowGfx2(0, 0x00A, BG_PLTT_ID(13));
        LoadStdWindowGfxOnBg(0, 0x001, BG_PLTT_ID(15));

        SetBgTilemapBuffer(0, ptr->bg0TilemapBuffer);
        FillBgTilemapBufferRect_Palette0(0, 0, 0, 2, 32, 30);
        DecompressAndCopyTileDataToVram(1, sCombosWindow_Tiles, 0, 0, 0);
        DecompressAndCopyTileDataToVram(1, sCombosWindow_Tilemap, 0, 0, 1);
        CopyBgTilemapBufferToVram(1);

        LoadPalette(GetTextWindowPalette(2), BG_PLTT_ID(14), PLTT_SIZE_4BPP);
        FillWindowPixelBuffer(1, 0xFF);
        PutWindowTilemap(1);

        x = DISPLAY_WIDTH - 4 - GetStringWidth(FONT_SMALL, gString_SlotMachineControls, 0);
        textColor[0] = TEXT_DYNAMIC_COLOR_6;
        textColor[1] = TEXT_COLOR_WHITE;
        textColor[2] = TEXT_COLOR_DARK_GRAY;
        AddTextPrinterParameterized3(1, FONT_SMALL, x, 0, textColor, 0, gString_SlotMachineControls);
        CopyBgTilemapBufferToVram(0);

        SetGpuRegBits(REG_OFFSET_DISPCNT, DISPCNT_MODE_0 | 0x20 | DISPCNT_OBJ_1D_MAP | DISPCNT_OBJ_ON);
        SetGpuReg(REG_OFFSET_BLDCNT, BLDCNT_TGT1_BG3 | BLDCNT_TGT1_OBJ | BLDCNT_TGT1_BD | BLDCNT_EFFECT_DARKEN);
        LoadSpriteGraphicsAndAllocateManager();
        CreateReelIconSprites();
        CreateCasinoBits();
        CreateScoreDigitSprites();
        CreateClefairySprites();
        UpdateCoinsDisplay();
        BlendPalettes(PALETTES_ALL, 0x10, RGB_BLACK);
        SetVBlankCallback(VBlankCB_SlotMachine);
        SetHBlankCallback(HBlankCB_SlotMachine);
        (*state)++;
        break;
    case 2:
        if (!FreeTempTileDataBuffersIfPossible())
        {
            ShowBg(0);
            ShowBg(3);
            ShowBg(2);
            HideBg(1);
            InitReelButtonTileMem();
            BlendPalettes(PALETTES_ALL, 0x10, RGB_BLACK);
            BeginNormalPaletteFade(PALETTES_ALL, -1, 16, 0, RGB_BLACK);
            EnableInterrupts(INTR_FLAG_VBLANK | INTR_FLAG_HBLANK);
            (*state)++;
        }
        break;
    case 3:
        UpdatePaletteFade();
        if (!gPaletteFade.active)
            return FALSE;
        break;
    }
    return TRUE;
}

static bool8 SlotsTask_FadeOut(u8 * state, struct SlotMachineSetupTaskData * ptr)
{
    switch (*state)
    {
    case 0:
        BeginNormalPaletteFade(PALETTES_ALL, -1, 0, 16, 0);
        (*state)++;
        break;
    case 1:
        if (!gPaletteFade.active)
            return FALSE;
        break;
    }
    return TRUE;
}

static bool8 SlotsTask_UpdateLineStates(u8 * state, struct SlotMachineSetupTaskData * ptr)
{
    switch (*state)
    {
    case 0:
        SetLineStatesByBet(GetBgTilemapBuffer(2));
        CopyBgTilemapBufferToVram(2);
        (*state)++;
        break;
    case 1:
        if (!IsDma3ManagerBusyWithBgCopy())
            return FALSE;
        break;
    }
    return TRUE;
}

static bool8 SlotsTask_ClefairyUpdateOnReelsStart(u8 * state, struct SlotMachineSetupTaskData * ptr)
{
    SetClefairySpriteAnim(1);
    return FALSE;
}

static bool8 SlotsTask_StartClefairyDanceAndWinningLineFlash(u8 * state, struct SlotMachineSetupTaskData * ptr)
{
    SetClefairySpriteAnim(2);
    CreateTask(Task_FlashWinningLine, 3);
    return FALSE;
}

static bool8 SlotsTask_StopWinningLineFlashTask(u8 * state, struct SlotMachineSetupTaskData * ptr)
{
    switch (*state)
    {
    case 0:
        SignalStopWinningLineFlashTask();
        (*state)++;
        break;
    case 1:
        if (!FuncIsActiveTask(Task_FlashWinningLine))
        {
            SetClefairySpriteAnim(0);
            return FALSE;
        }
        break;
    }
    return TRUE;
}

static bool8 SlotsTask_ClefairyFainted(u8 * state, struct SlotMachineSetupTaskData * ptr)
{
    SetClefairySpriteAnim(3);
    return FALSE;
}

static bool8 SlotsTask_ClefairyNeutral(u8 * state, struct SlotMachineSetupTaskData * ptr)
{
    SetClefairySpriteAnim(0);
    return FALSE;
}

static bool8 SlotsTask_UpdateCoinsDisplay(u8 * state, struct SlotMachineSetupTaskData * ptr)
{
    UpdateCoinsDisplay();
    return FALSE;
}

static bool8 SlotsTask_MessageOutOfCoins(u8 * state, struct SlotMachineSetupTaskData * ptr)
{
    switch (*state)
    {
    case 0:
        Slot_PrintOnWindow0(gString_OutOfCoins);
        CopyWindowToVram(0, COPYWIN_FULL);
        (*state)++;
        break;
    case 1:
        if (!IsDma3ManagerBusyWithBgCopy())
            return FALSE;
        break;
    }
    return TRUE;
}

static bool8 SlotsTask_AskQuitPlaying(u8 * state, struct SlotMachineSetupTaskData * ptr)
{
    switch (*state)
    {
    case 0:
        Slot_PrintOnWindow0(gString_QuitPlaying);
        Slot_CreateYesNoMenu(0);
        CopyWindowToVram(0, COPYWIN_FULL);
        (*state)++;
        break;
    case 1:
        if (!IsDma3ManagerBusyWithBgCopy())
            return FALSE;
        break;
    }
    return TRUE;
}

static bool8 SlotsTask_DestroyYesNoMenu(u8 * state, struct SlotMachineSetupTaskData * ptr)
{
    switch (*state)
    {
    case 0:
        Slot_ClearWindow0();
        Slot_DestroyYesNoMenu();
        CopyWindowToVram(0, COPYWIN_FULL);
        (*state)++;
        break;
    case 1:
        if (!IsDma3ManagerBusyWithBgCopy())
            return FALSE;
        break;
    }
    return TRUE;
}

static bool8 SlotsTask_PressReelButton(u8 * state, struct SlotMachineSetupTaskData * ptr)
{
    switch (*state)
    {
    case 0:
        SetReelButtonPressed(ptr->reelButtonToPress);
        CopyBgTilemapBufferToVram(2);
        (*state)++;
        break;
    case 1:
        if (!IsDma3ManagerBusyWithBgCopy())
            return FALSE;
        break;
    }
    return TRUE;
}

static bool8 SlotsTask_ReleaseReelButtons(u8 * state, struct SlotMachineSetupTaskData * ptr)
{
    switch (*state)
    {
    case 0:
        ReleaseReelButtons();
        CopyBgTilemapBufferToVram(2);
        (*state)++;
        break;
    case 1:
        if (!IsDma3ManagerBusyWithBgCopy())
            return FALSE;
        break;
    }
    return TRUE;
}

// T-422: the pays, in words (vanilla's COMBOS card showed its own seven icons), and the day's biggest reason. One line
// each at a fixed 13px -- FONT_SMALL's own newline was wider and pushed the fifth pay line under "The season first"
// (the fix, 2026-10-18) -- nine rows in the card's 120px.
static const u8 sText_CasinoPaysHead[] = _("On the middle line, for each coin bet:");   // DRAFT
static const u8 sText_CasinoPayLead3[] = _("Three of your lead{CLEAR_TO 160}50");      // DRAFT
static const u8 sText_CasinoPayFriend3[] = _("Three of a friend{CLEAR_TO 160}12");    // DRAFT
static const u8 sText_CasinoPayCoin3[] = _("Three coins{CLEAR_TO 160}6");             // DRAFT
static const u8 sText_CasinoPaySeason3[] = _("Three of the season{CLEAR_TO 160}4");   // DRAFT
static const u8 sText_CasinoPayLead2[] = _("Two of your lead{CLEAR_TO 160}2");        // DRAFT
static const u8 sText_CasinoSeasonFirst[] = _("The season first: your bet back.");   // DRAFT
static const u8 sText_CasinoToday[] = _("Today: ");   // DRAFT

static const u8 *const sCasinoHelpLines[] = {
    sText_CasinoPaysHead, sText_CasinoPayLead3, sText_CasinoPayFriend3, sText_CasinoPayCoin3, sText_CasinoPaySeason3,
    sText_CasinoPayLead2, sText_CasinoSeasonFirst,
};

#define WIN_HELP 2
#define HELP_ROW(n) (1 + 13 * (n))
#define HELP_TEXT_WIDTH (26 * 8 - 4)

// "Today: " and the reason on row 7 (context.ts's words in full), what does not fit carried to row 8 at a word
static void PrintCasinoToday(void)
{
    u8 line[96], rest[96];
    u16 i, cut = 0;

    StringCopy(line, sText_CasinoToday);
    DaemonsCasino_WhyText(&sSlotMachineState->day, line + StringLength(line));
    rest[0] = EOS;
    if (GetStringWidth(FONT_SMALL, line, 0) > HELP_TEXT_WIDTH)
    {
        for (i = 0; line[i] != EOS; i++)
        {
            if (line[i] != CHAR_SPACE)
                continue;
            line[i] = EOS;
            if (GetStringWidth(FONT_SMALL, line, 0) <= HELP_TEXT_WIDTH)
                cut = i;
            line[i] = CHAR_SPACE;
        }
        if (cut)
        {
            StringCopy(rest, line + cut + 1);
            line[cut] = EOS;
        }
    }
    AddTextPrinterParameterized5(WIN_HELP, FONT_SMALL, line, 2, HELP_ROW(7), TEXT_SKIP_DRAW, NULL, 0, 0);
    if (rest[0] != EOS)
        AddTextPrinterParameterized5(WIN_HELP, FONT_SMALL, rest, 10, HELP_ROW(8), TEXT_SKIP_DRAW, NULL, 0, 0);
}

static bool8 SlotsTask_ShowHelp(u8 * state, struct SlotMachineSetupTaskData * ptr)
{
    u32 i;

    switch (*state)
    {
    case 0:
        FillWindowPixelBuffer(WIN_HELP, PIXEL_FILL(1));
        PutWindowTilemap(WIN_HELP);
        DrawTextBorderOuter(WIN_HELP, 0x001, 15);
        for (i = 0; i < ARRAY_COUNT(sCasinoHelpLines); i++)
            AddTextPrinterParameterized5(WIN_HELP, FONT_SMALL, sCasinoHelpLines[i], 2, HELP_ROW(i), TEXT_SKIP_DRAW, NULL, 0, 0);
        PrintCasinoToday();
        CopyWindowToVram(WIN_HELP, COPYWIN_FULL);
        PlaySE(SE_WIN_OPEN);
        (*state)++;
        break;
    case 1:
        if (!IsDma3ManagerBusyWithBgCopy())
            return FALSE;
        break;
    }
    return TRUE;
}

static bool8 SlotsTask_HideHelp(u8 * state, struct SlotMachineSetupTaskData * ptr)
{
    switch (*state)
    {
    case 0:
        PlaySE(SE_WIN_OPEN);
        rbox_fill_rectangle(WIN_HELP);
        CopyWindowToVram(WIN_HELP, COPYWIN_MAP);
        (*state)++;
        break;
    case 1:
        if (!IsDma3ManagerBusyWithBgCopy())
            return FALSE;
        break;
    }
    return TRUE;
}

// T-422: a word under the reels after the result -- SO CLOSE, ALMOST THE JACKPOT, BET BACK (the phone's and the
// boards' own, C-158); NULL takes it away
static void PrintTopBar(const u8 *str)
{
    u8 textColor[3] = {TEXT_DYNAMIC_COLOR_6, TEXT_COLOR_WHITE, TEXT_COLOR_DARK_GRAY};

    FillWindowPixelBuffer(1, 0xFF);
    AddTextPrinterParameterized3(1, FONT_SMALL, DISPLAY_WIDTH - 4 - GetStringWidth(FONT_SMALL, str, 0), 0, textColor, 0, str);
    CopyWindowToVram(1, COPYWIN_GFX);
}

static void CasinoSay(const u8 *str)
{
    if (str == NULL)
    {
        if (sSlotMachineState->note)
            PrintTopBar(gString_SlotMachineControls);
        sSlotMachineState->note = 0;
        return;
    }
    PrintTopBar(str);   // in the bar over the machine, so the reels it is about stay in sight
    sSlotMachineState->note = 1;
}

// T-422: THE SEASON FALLING OVER THE REELS (casino.cpp's bits): a dozen small pieces drifting down -- or rising, hearts
// and sparks -- or twinkling where they are, behind the machine's face so only the reels' windows show them
#define NUM_BITS 12
#define BITS_LEFT   60
#define BITS_RIGHT  180
#define BITS_TOP    40
#define BITS_BOTTOM 132

#define sTile   data[0]
#define sMotion data[1]
#define sVy     data[2]
#define sY16    data[3]
#define sPhase  data[4]
#define sX      data[5]
#define sTime   data[6]

static const struct OamData sOamData_CasinoBit = {
    .affineMode = ST_OAM_AFFINE_OFF,
    .objMode = ST_OAM_OBJ_NORMAL,
    .bpp = ST_OAM_4BPP,
    .shape = ST_OAM_SQUARE,
    .size = ST_OAM_SIZE_0,
    .priority = 3,
};

static void SpriteCB_CasinoBit(struct Sprite *sprite);

static const struct SpriteTemplate sSpriteTemplate_CasinoBit = {
    .tileTag = GFXTAG_BITS,
    .paletteTag = PALTAG_BITS,
    .oam = &sOamData_CasinoBit,
    .anims = gDummySpriteAnimTable,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCB_CasinoBit
};

static void ResetCasinoBit(struct Sprite *sprite, bool8 anywhere)
{
    u8 motion;

    sprite->sTile = DaemonsCasino_BitTile(sSlotMachineState->day.look, &motion);
    sprite->sMotion = motion;
    sprite->sX = BITS_LEFT + Random() % (BITS_RIGHT - BITS_LEFT);
    sprite->sVy = motion == 2 ? 0 : 4 + Random() % 6;        // 14 to 35 a second, in sixteenths a frame
    if (anywhere || motion == 2)
        sprite->sY16 = (BITS_TOP + Random() % (BITS_BOTTOM - BITS_TOP)) * 16;
    else
        sprite->sY16 = (motion == 1 ? BITS_BOTTOM : BITS_TOP - 6) * 16;
    sprite->sPhase = Random() & 0xFF;
    sprite->oam.tileNum = GetSpriteTileStartByTag(GFXTAG_BITS) + sprite->sTile;
}

static void SpriteCB_CasinoBit(struct Sprite *sprite)
{
    sprite->sTime++;
    if (sprite->sMotion == 1)
        sprite->sY16 -= sprite->sVy;
    else
        sprite->sY16 += sprite->sVy;
    if (sprite->sY16 > BITS_BOTTOM * 16 || sprite->sY16 < (BITS_TOP - 8) * 16)
        ResetCasinoBit(sprite, FALSE);
    sprite->x = sprite->sX + Sin((sprite->sPhase + sprite->sTime) & 0xFF, 4);
    sprite->y = sprite->sY16 / 16;
    if (sprite->sMotion == 2)   // a twinkle: its four sizes, as a sine
        sprite->oam.tileNum = GetSpriteTileStartByTag(GFXTAG_BITS) + sprite->sTile
                            + ((Sin((sprite->sPhase + sprite->sTime * 3) & 0xFF, 256) + 256) * 4 / 513);
}

static void CreateCasinoBits(void)
{
    u32 i;
    u8 spriteId;

    if (sSlotMachineState->day.look == 0xFF)
        return;
    for (i = 0; i < NUM_BITS; i++)
    {
        spriteId = CreateSprite(&sSpriteTemplate_CasinoBit, BITS_LEFT, BITS_TOP, 1);
        if (spriteId != MAX_SPRITES)
        {
            ResetCasinoBit(&gSprites[spriteId], TRUE);
            SpriteCB_CasinoBit(&gSprites[spriteId]);
        }
    }
}

#undef sTile
#undef sMotion
#undef sVy
#undef sY16
#undef sPhase
#undef sX
#undef sTime

static void Slot_PrintOnWindow0(const u8 * str)
{
    FillWindowPixelBuffer(0, PIXEL_FILL(1));
    PutWindowTilemap(0);
    DrawTextBorderOuter(0, 0x001, 15);
    AddTextPrinterParameterized5(0, FONT_NORMAL, str, 1, 2, TEXT_SKIP_DRAW, NULL, 1, 2);
}

static void Slot_ClearWindow0(void)
{
    rbox_fill_rectangle(0);
}

static void SetLineStatesByBet(u16 * bgTilemapBuffer)
{
    switch (GetPlayerBet())
    {
    case 0:
        SetLineState(bgTilemapBuffer, 0, PALSLOT_LINE_NORMAL);
        SetLineState(bgTilemapBuffer, 1, PALSLOT_LINE_NORMAL);
        SetLineState(bgTilemapBuffer, 2, PALSLOT_LINE_NORMAL);
        SetLineState(bgTilemapBuffer, 3, PALSLOT_LINE_NORMAL);
        SetLineState(bgTilemapBuffer, 4, PALSLOT_LINE_NORMAL);
        break;
    default:   // T-422: every bet is on the middle line -- the coins only raise what it pays
        SetLineState(bgTilemapBuffer, MIDDLE_LINE, PALSLOT_LINE_BET);
        break;
    }
}

static void SetLineState(u16 * bgTilemapBuffer, u16 whichLine, u16 paletteNum)
{
    s32 i;
    const u16 * tileIdxs = sLineStateTileIdxs[whichLine].tiles;
    u16 palMask = (paletteNum & 0xF) << 12;

    if (whichLine != MIDDLE_LINE)
        return;   // T-422's surround: the other four lines are gone from the machine (ClearUnpaidLines)
    for (i = 0; i < sLineStateTileIdxs[whichLine].count; i++)
    {
        bgTilemapBuffer[*tileIdxs] &= 0x0FFF;
        bgTilemapBuffer[*tileIdxs] |= palMask;
        tileIdxs++;
    }
}

// T-422's surround (the user, 2026-10-18): only the middle line pays, so the 2 and 3 lamps and the four other lines'
// marks between the reels go. Every tile the four unpaid lines own is painted over with what lies around it: the plain
// orange beside the lamp rail, the rail itself (one tile of its own colour, loaded after the machine's 138 tiles), the
// reel window's plain edge (as on row 9) and the plain gap between the reels (as on row 7). Both editions' machines
// share these places (graphics/slot_machine/{firered,leafgreen}/bg.bin).
#define RAIL_TILE 0x8A
#define RAIL_PAL  PALSLOT_LINE_NORMAL

static void ClearUnpaidLines(u16 * bgTilemapBuffer)
{
    static const u32 sRail[8] = {0x22222222, 0x22222222, 0x22222222, 0x22222222, 0x22222222, 0x22222222, 0x22222222, 0x22222222};
    u32 line, i;
    u16 pos, col, hflip;

    LoadBgTiles(2, sRail, sizeof(sRail), RAIL_TILE);
    for (line = 0; line < NUM_MATCH_LINES; line++)
    {
        if (line == MIDDLE_LINE)
            continue;
        for (i = 0; i < sLineStateTileIdxs[line].count; i++)
        {
            pos = sLineStateTileIdxs[line].tiles[i];
            col = pos % 32;
            hflip = bgTilemapBuffer[pos] & 0x0400;
            if (col == 4 || col == 6 || col == 23 || col == 25)
                bgTilemapBuffer[pos] = 0x0001;                              // the plain orange (green), palette 0
            else if (col == 5 || col == 24)
                bgTilemapBuffer[pos] = (RAIL_PAL << 12) | hflip | RAIL_TILE;   // the lamp rail, unbroken
            else if (col == 7 || col == 22)
                bgTilemapBuffer[pos] = bgTilemapBuffer[9 * 32 + col];       // the reel window's plain edge
            else if (col == 12 || col == 17)
                bgTilemapBuffer[pos] = bgTilemapBuffer[7 * 32 + col];       // the plain gap between the reels
        }
    }
}

static void Task_FlashWinningLine(u8 taskId)
{
    s16 * data = gTasks[taskId].data;
    s32 i;

    switch (data[0])
    {
    case 0:
        LoadPalette(sBgPal_MatchLines, BG_PLTT_ID(PALSLOT_LINE_MATCH), sizeof(sBgPal_MatchLines));
        for (i = 0; i < NUM_MATCH_LINES; i++)
        {
            if (GetWinFlagByLine(i))
                SetLineState(GetBgTilemapBuffer(2), i, PALSLOT_LINE_MATCH);
        }
        CopyBgTilemapBufferToVram(2);
        data[0]++;
        break;
    case 1:
        if (data[1] == 0)
        {
            u16 y = gSineTable[data[2]] >> 7;
            LoadPalette(&sBgPal_PayoutLight[y], BG_PLTT_ID(1), sizeof(sBgPal_PayoutLight[0]));
            data[2] += 32;
            data[2] &= 0x7F;
            data[1] = 8;
        }
        else
            data[1]--;

        if (data[3] == 0)
        {
            data[4] += 8;
            data[4] &= 0x7F;
            data[5] = gSineTable[data[4]] >> 5;
            BlendPalettes(1 << PALSLOT_LINE_MATCH, data[5], RGB_BLACK);
        }
        else
        {
            data[4]++;
            if (data[4] > 1)
            {
                data[4] = 0;
                data[5]++;
                data[5] &= 1;
                BlendPalettes(1 << PALSLOT_LINE_MATCH, data[5] * 8, RGB_BLACK);
            }
        }

        for (i = 0; i < ARRAY_COUNT(sWinningLineFlashPalIdxs); i++)
            gPlttBufferFaded[sWinningLineFlashPalIdxs[i] + BG_PLTT_ID(PALSLOT_LINE_MATCH)] = gPlttBufferUnfaded[sWinningLineFlashPalIdxs[i] + BG_PLTT_ID(PALSLOT_LINE_MATCH)];
        break;
    case 2:
        // Restore match lines to normal color 
        for (i = 0; i < NUM_MATCH_LINES; i++)
        {
            if (GetWinFlagByLine(i))
                SetLineState(GetBgTilemapBuffer(2), i, PALSLOT_LINE_NORMAL);
        }
        
        // Restore payout lights to normal color
        LoadPalette(&sBg_Pal[1], BG_PLTT_ID(1), sizeof(sBg_Pal[1]));

        CopyBgTilemapBufferToVram(2);
        data[0]++;
        break;
    case 3:
        if (!IsDma3ManagerBusyWithBgCopy())
            DestroyTask(taskId);
        break;
    }
}

static void SignalStopWinningLineFlashTask(void)
{
    gTasks[FindTaskIdByFunc(Task_FlashWinningLine)].data[0] = 2;
}

static void Slot_CreateYesNoMenu(u8 cursorPos)
{
    CreateYesNoMenu(&sYesNoWindowTemplate, FONT_NORMAL, 0, 2, 10, 13, cursorPos);
    Menu_MoveCursorNoWrapAround(cursorPos);
    GetSlotMachineSetupTaskDataPtr()->yesNoMenuActive = TRUE;
}

static void Slot_DestroyYesNoMenu(void)
{
    struct SlotMachineSetupTaskData * data = GetSlotMachineSetupTaskDataPtr();
    if (data->yesNoMenuActive)
    {
        DestroyYesNoMenu();
        data->yesNoMenuActive = FALSE;
    }
}

static void InitReelButtonTileMem(void)
{
    s32 i, j;
    struct SlotMachineSetupTaskData * data = GetSlotMachineSetupTaskDataPtr();
    u16 * buffer = GetBgTilemapBuffer(2);

    for (i = 0; i < NUM_REELS; i++)
    {
        for (j = 0; j < NUM_BUTTON_TILES; j++)
        {
            u16 idx = sReelButtonMapTileIdxs[i][j];
            data->buttonReleasedTiles[i][j] = buffer[idx];
            data->buttonPressedTiles[i][j] = j + 0xC0;
        }
    }
}

static void SetReelButtonPressed(u8 reel)
{
    if (reel < NUM_REELS)
    {
        s32 i;
        struct SlotMachineSetupTaskData * data = GetSlotMachineSetupTaskDataPtr();
        u16 * buffer = GetBgTilemapBuffer(2);
        for (i = 0; i < NUM_BUTTON_TILES; i++)
        {
            u16 idx = sReelButtonMapTileIdxs[reel][i];
            buffer[idx] = data->buttonPressedTiles[reel][i];
        }
    }
}

static void ReleaseReelButtons(void)
{
    s32 i, j;
    struct SlotMachineSetupTaskData * data = GetSlotMachineSetupTaskDataPtr();
    u16 * buffer = GetBgTilemapBuffer(2);

    for (i = 0; i < NUM_REELS; i++)
    {
        for (j = 0; j < NUM_BUTTON_TILES; j++)
        {
            u16 idx = sReelButtonMapTileIdxs[i][j];
            buffer[idx] = data->buttonReleasedTiles[i][j];
        }
    }
}

static void PressReelButton(u8 reel, u8 taskId)
{
    GetSlotMachineSetupTaskDataPtr()->reelButtonToPress = reel;
    SetSlotMachineSetupTask(SLOTTASK_PRESS_BUTTON, taskId);
}
