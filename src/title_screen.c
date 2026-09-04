#include "global.h"
#include "trig.h"
#include "gflib.h"
#include "task.h"
#include "new_menu_helpers.h"
#include "m4a.h"
#include "scanline_effect.h"
#include "graphics.h"
#include "help_system.h"
#include "intro.h"
#include "load_save.h"
#include "new_game.h"
#include "random.h"
#include "save.h"
#include "main_menu.h"
#include "clear_save_data_screen.h"
#include "berry_fix_program.h"
#include "decompress.h"
#include "constants/songs.h"

enum TitleScreenScene
{
    TITLESCREENSCENE_INIT = 0,
    TITLESCREENSCENE_FLASHSPRITE,
    TITLESCREENSCENE_FADEIN,
    TITLESCREENSCENE_RUN,
    TITLESCREENSCENE_RESTART,
    TITLESCREENSCENE_CRY
};

#if   defined(FIRERED)
#define TITLE_SPECIES SPECIES_CHARIZARD
#elif defined(LEAFGREEN)
#define TITLE_SPECIES SPECIES_VENUSAUR
#endif

static EWRAM_DATA u8 sTitleScreenTimerTaskId = 0;

static void ResetGpuRegs(void);
static void CB2_TitleScreenRun(void);
static void VBlankCB(void);
static void Task_TitleScreenTimer(u8 taskId);
static void Task_TitleScreenMain(u8 taskId);
static void CreateFaceOff(void);
static void SpriteCallback_FaceOffIdle(struct Sprite *sprite);
static bool32 CreateCrossingSprite(s32 x, s32 y, s32 xspeed, s32 yspeed, u8 anim);
static void SetTitleScreenScene(s16 *data, u8 sceneNum);
static void SetTitleScreenScene_Init(s16 *data);
static void SetTitleScreenScene_FlashSprite(s16 *data);
static void SetTitleScreenScene_FadeIn(s16 *data);
static void SetTitleScreenScene_Run(s16 *data);
static void SetGpuRegsForTitleScreenRun(void);
static void SetTitleScreenScene_Restart(s16 *data);
static void SetTitleScreenScene_Cry(s16 *data);
static void Task_TitleScreen_SlideWin0(u8 taskId);
static void Task_TitleScreen_BlinkPressStart(u8 taskId);
static void SignalEndTitleScreenPaletteSomethingTask(void);
static void UpdateScanlineEffectRegBuffer(s16 y);
static void ScheduleStopScanlineEffect(void);
static void LoadMainTitleScreenPalsAndResetBgs(void);
static void CB2_FadeOutTransitionToSaveClearScreen(void);
static void CB2_FadeOutTransitionToBerryFix(void);
static void LoadSpriteGfxAndPals(void);
#if defined(FIRERED)
static void SpriteCallback_TitleScreenFlame(struct Sprite *sprite);
static void Task_FlameSpawner(u8 taskId);
#elif defined(LEAFGREEN)
static void SpriteCallback_TitleScreenLeaf(struct Sprite *sprite);
static void Task_LeafSpawner(u8 taskId);
#endif
static void TitleScreen_srand(u8 taskId, u8 field, u16 seed);
static u16 TitleScreen_rand(u8 taskId, u8 field);
static u32 CreateBlankSprite(void);
static void SetPalOnOrCreateBlankSprite(bool32 hasCreatedBlankSprite);
static u8 CreateSlashSprite(void);
static void DeactivateSlashSprite(u8 spriteId);
static bool32 IsSlashSpriteDeactivated(u8 spriteId);
static void SpriteCallback_Slash(struct Sprite *sprite);

static const u8 sBorderBgTiles[] = INCBIN_U8("graphics/title_screen/border_bg.4bpp.lz");

#if defined(FIRERED)
static const u8 sBorderBgMap[] = INCBIN_U8("graphics/title_screen/firered/border_bg.bin.lz");
#elif defined(LEAFGREEN)
static const u8 sBorderBgMap[] = INCBIN_U8("graphics/title_screen/leafgreen/border_bg.bin.lz");
#endif

static const u32 sSlash_Gfx[] = INCBIN_U32("graphics/title_screen/slash.4bpp.lz");

#if defined(FIRERED)
static const u16 sFlames_Pal[] = INCBIN_U16("graphics/title_screen/firered/flames.gbapal");
static const u32 sFlames_Gfx[] = INCBIN_U32("graphics/title_screen/firered/flames.4bpp.lz");
static const u32 sBlankFlames_Gfx[] = INCBIN_U32("graphics/title_screen/firered/blank_flames.4bpp.lz");
#elif defined(LEAFGREEN)
static const u16 sLeaves_Pal[] = INCBIN_U16("graphics/title_screen/leafgreen/leaves.gbapal");
static const u32 sLeaves_Gfx[] = INCBIN_U32("graphics/title_screen/leafgreen/leaves.4bpp.lz");
static const u32 sStreak_Gfx[] = INCBIN_U32("graphics/title_screen/leafgreen/streak.4bpp.lz");
#endif

static const struct OamData sOamData_FlameOrLeaf = {
    .objMode = ST_OAM_OBJ_NORMAL,
    .shape = ST_OAM_SQUARE,
    .size = ST_OAM_SIZE_1,
    .tileNum = 0,
    .priority = 3,
    .paletteNum = 0
};

// 9.14, and the same sentence the presents scene tells. A particle leaves ONE
// of the two creatures and crosses toward the other; what it looks like on the
// way says which sent it, and what happens where they meet is the point.
//
//   ANIM_CODE   0 and 1, cold, from the one that lunges
//   ANIM_MUSIC  a note, warm, from the one that opens
//   ANIM_BLOOM  played on ARRIVAL, not on a timer -- and it is the only one
//               that ends, so the bloom is also the particle's death
//
// Both editions get all three: the argument does not change by cartridge.
enum { ANIM_CODE, ANIM_MUSIC, ANIM_BLOOM };

static const union AnimCmd sSpriteAnim_Code[] = {
    ANIMCMD_FRAME(0, 6), ANIMCMD_FRAME(4, 6), ANIMCMD_FRAME(8, 6), ANIMCMD_JUMP(0)
};
static const union AnimCmd sSpriteAnim_Music[] = {
    ANIMCMD_FRAME(12, 6), ANIMCMD_FRAME(16, 6), ANIMCMD_FRAME(20, 6), ANIMCMD_JUMP(0)
};
static const union AnimCmd sSpriteAnim_Bloom[] = {
    ANIMCMD_FRAME(24, 4), ANIMCMD_FRAME(28, 4),
    ANIMCMD_FRAME(32, 5), ANIMCMD_FRAME(36, 6), ANIMCMD_END
};
static const union AnimCmd *const sSpriteAnim_FlameOrLeaf[] = {
    sSpriteAnim_Code, sSpriteAnim_Music, sSpriteAnim_Bloom
};

// 9.14: the face-off. CODEMUSAI lunges and CAREMUSAI opens, in BOTH editions;
// only the side changes, and the engine does that with a hardware flip. Both
// are 64x64, which is the frame 9.4 refused to upscale into -- CODEMUSAI is 48
// and CAREMUSAI is 56, and each sits inside it untouched.
static const u32 sFaceOffCode_Gfx[] = INCBIN_U32("graphics/title_screen/faceoff/codemusai.4bpp.lz");
static const u16 sFaceOffCode_Pal[] = INCBIN_U16("graphics/title_screen/faceoff/codemusai.gbapal");
static const u32 sFaceOffCare_Gfx[] = INCBIN_U32("graphics/title_screen/faceoff/caremusai.4bpp.lz");
static const u16 sFaceOffCare_Pal[] = INCBIN_U16("graphics/title_screen/faceoff/caremusai.gbapal");

enum {
    TILE_TAG_FLAME_OR_LEAF,
    TILE_TAG_BLANK_OR_STREAK,
    TILE_TAG_BLANK,
    TILE_TAG_SLASH,
    TILE_TAG_FACEOFF_CODE,
    TILE_TAG_FACEOFF_CARE,
};

enum {
    PAL_TAG_DEFAULT,
    PAL_TAG_UNUSED,
    PAL_TAG_SLASH,
    PAL_TAG_FACEOFF_CODE,
    PAL_TAG_FACEOFF_CARE,
};

static const struct OamData sOamData_FaceOff = {
    .objMode = ST_OAM_OBJ_NORMAL,
    .shape = ST_OAM_SQUARE,
    .size = ST_OAM_SIZE_3,          // 64x64
    .tileNum = 0,
    .priority = 2,                  // in front of the background, behind nothing
    .paletteNum = 0
};

static const struct SpriteTemplate sSpriteTemplate_FaceOffCode = {
    .tileTag = TILE_TAG_FACEOFF_CODE,
    .paletteTag = PAL_TAG_FACEOFF_CODE,
    .oam = &sOamData_FaceOff,
    .anims = gDummySpriteAnimTable,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCallbackDummy
};

static const struct SpriteTemplate sSpriteTemplate_FaceOffCare = {
    .tileTag = TILE_TAG_FACEOFF_CARE,
    .paletteTag = PAL_TAG_FACEOFF_CARE,
    .oam = &sOamData_FaceOff,
    .anims = gDummySpriteAnimTable,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCallbackDummy
};

static const struct SpriteTemplate sSpriteTemplate_FlameOrLeaf = {
    .tileTag = TILE_TAG_FLAME_OR_LEAF,
    .paletteTag = PAL_TAG_DEFAULT,
    .oam = &sOamData_FlameOrLeaf,
    .anims = sSpriteAnim_FlameOrLeaf,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCallbackDummy
};

#if defined(FIRERED)
static const struct SpriteTemplate sSpriteTemplate_BlankFlame = {
    .tileTag = TILE_TAG_BLANK_OR_STREAK,
    .paletteTag = PAL_TAG_DEFAULT,
    .oam = &sOamData_FlameOrLeaf,
    .anims = sSpriteAnim_FlameOrLeaf,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCallbackDummy
};

#elif defined(LEAFGREEN)
static const struct OamData sOamData_Streak = {
    .shape = SPRITE_SHAPE(32x16),
    .size = SPRITE_SIZE(32x16),
    .priority = 3
};

static const struct SpriteTemplate sSpriteTemplate_Streak = {
    .tileTag = TILE_TAG_BLANK_OR_STREAK,
    .paletteTag = PAL_TAG_DEFAULT,
    .oam = &sOamData_Streak,
    .anims = gDummySpriteAnimTable,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCallbackDummy
};
#endif

static const struct OamData sOamData_BlankSprite = {
    .objMode = ST_OAM_OBJ_NORMAL,
    .shape = ST_OAM_V_RECTANGLE,
    .size = ST_OAM_SIZE_3,
    .tileNum = 0,
    .priority = 0,
    .paletteNum = 0
};

static const struct SpriteTemplate sSpriteTemplate_BlankSprite = {
    .tileTag = TILE_TAG_BLANK,
    .paletteTag = PAL_TAG_SLASH,
    .oam = &sOamData_BlankSprite,
    .anims = gDummySpriteAnimTable,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCallbackDummy
};

static const struct OamData sOamData_SlashSprite = {
    .objMode = ST_OAM_OBJ_WINDOW,
    .shape = ST_OAM_SQUARE,
    .size = ST_OAM_SIZE_3,
    .tileNum = 0,
    .priority = 0,
    .paletteNum = 0
};

static const struct SpriteTemplate sSlashSpriteTemplate = {
    .tileTag = TILE_TAG_SLASH,
    .paletteTag = PAL_TAG_SLASH,
    .oam = &sOamData_SlashSprite,
    .anims = gDummySpriteAnimTable,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCallbackDummy
};

static const struct BgTemplate sBgTemplates[] = {
    {
        .bg = 0,
        .charBaseIndex = 0,
        .mapBaseIndex = 31,
        .screenSize = 0,
        .paletteMode = 1, // 8bpp
        .priority = 0,
        .baseTile = 0
    }, {
        .bg = 1,
        .charBaseIndex = 1,
        .mapBaseIndex = 30,
        .screenSize = 0,
        .paletteMode = 0, // 4bpp
        .priority = 1,
        .baseTile = 0
    }, {
        .bg = 2,
        .charBaseIndex = 2,
        .mapBaseIndex = 29,
        .screenSize = 0,
        .paletteMode = 0, // 4bpp
        .priority = 2,
        .baseTile = 0
    }, {
        .bg = 3,
        .charBaseIndex = 3,
        .mapBaseIndex = 28,
        .screenSize = 0,
        .paletteMode = 0, // 4bpp
        .priority = 3,
        .baseTile = 0
    }
};

static void (*const sSceneFuncs[])(s16 *data) = {
    [TITLESCREENSCENE_INIT]        = SetTitleScreenScene_Init,
    [TITLESCREENSCENE_FLASHSPRITE] = SetTitleScreenScene_FlashSprite,
    [TITLESCREENSCENE_FADEIN]      = SetTitleScreenScene_FadeIn,
    [TITLESCREENSCENE_RUN]         = SetTitleScreenScene_Run,
    [TITLESCREENSCENE_RESTART]     = SetTitleScreenScene_Restart,
    [TITLESCREENSCENE_CRY]         = SetTitleScreenScene_Cry
};

#if defined(FIRERED)
static const struct CompressedSpriteSheet sSpriteSheets[] = {
    {sFlames_Gfx,                    0x500, TILE_TAG_FLAME_OR_LEAF},
    {sBlankFlames_Gfx,               0x500, TILE_TAG_BLANK_OR_STREAK},
    {gTitleScreen_BlankSprite_Tiles, 0x400, TILE_TAG_BLANK},
    {sSlash_Gfx,                     0x800, TILE_TAG_SLASH},
    {sFaceOffCode_Gfx,               0x800, TILE_TAG_FACEOFF_CODE},
    {sFaceOffCare_Gfx,               0x800, TILE_TAG_FACEOFF_CARE}
};

static const struct SpritePalette sSpritePals[] = {
    {sFlames_Pal,            PAL_TAG_DEFAULT},
    {gTitleScreen_Slash_Pal, PAL_TAG_SLASH},
    {sFaceOffCode_Pal,       PAL_TAG_FACEOFF_CODE},
    {sFaceOffCare_Pal,       PAL_TAG_FACEOFF_CARE},
    {}
};

static const u8 sFlameXPositions[] = {
    4, 16, 26, 32, 48, 200, 216, 224, 232, 60, 76, 92, 108, 128, 144, 0
};

#elif defined(LEAFGREEN)
static const struct CompressedSpriteSheet sSpriteSheets[] = {
    {sLeaves_Gfx,                    0x580, TILE_TAG_FLAME_OR_LEAF},
    {sStreak_Gfx,                    0x100, TILE_TAG_BLANK_OR_STREAK},
    {gTitleScreen_BlankSprite_Tiles, 0x400, TILE_TAG_BLANK},
    {sSlash_Gfx,                     0x800, TILE_TAG_SLASH},
    {sFaceOffCode_Gfx,               0x800, TILE_TAG_FACEOFF_CODE},
    {sFaceOffCare_Gfx,               0x800, TILE_TAG_FACEOFF_CARE}
};

static const struct SpritePalette sSpritePals[] = {
    {sLeaves_Pal,            PAL_TAG_DEFAULT},
    {gTitleScreen_Slash_Pal, PAL_TAG_SLASH},
    {sFaceOffCode_Pal,       PAL_TAG_FACEOFF_CODE},
    {sFaceOffCare_Pal,       PAL_TAG_FACEOFF_CARE},
    {}
};

static const u16 sStreakYPositions[] = {
    40, 80, 110, 60, 90, 70, 100, 50
};
#endif

static const u32 sUnused_Tilemap1[] = INCBIN_U32("graphics/title_screen/unused1.bin.lz");
static const u32 sUnused_Tilemap2[] = INCBIN_U32("graphics/title_screen/unused2.bin.lz");
static const u32 sUnused_Tilemap3[] = INCBIN_U32("graphics/title_screen/unused3.bin.lz");
static const u32 sUnused_Tilemap4[] = INCBIN_U32("graphics/title_screen/unused4.bin.lz");
static const u32 sUnused_Tilemap5[] = INCBIN_U32("graphics/title_screen/unused5.bin.lz");
static const u32 sUnused_Tilemap6[] = INCBIN_U32("graphics/title_screen/unused6.bin.lz");

static const u32 *const sUnused_Tilemaps[] = {
    sUnused_Tilemap1,
    sUnused_Tilemap2,
    sUnused_Tilemap3,
    sUnused_Tilemap4,
    sUnused_Tilemap5,
    sUnused_Tilemap6,
};

void CB2_InitTitleScreen(void)
{
    switch (gMain.state)
    {
    default:
        gMain.state = 0;
        // fallthrough
    case 0:
        SetVBlankCallback(NULL);
        StartTimer1();
        InitHeap(gHeap, HEAP_SIZE);
        ResetTasks();
        ResetSpriteData();
        FreeAllSpritePalettes();
        ResetPaletteFade();
        ResetGpuRegs();
        DmaFill16(3, 0, (void *)VRAM, VRAM_SIZE);
        DmaFill32(3, 0, (void *)OAM, OAM_SIZE);
        DmaFill16(3, 0, (void *)PLTT, PLTT_SIZE);
        ResetBgsAndClearDma3BusyFlags(FALSE);
        InitBgsFromTemplates(0, sBgTemplates, NELEMS(sBgTemplates));
        SetGpuRegBits(REG_OFFSET_DISPCNT, DISPCNT_OBJ_1D_MAP | DISPCNT_OBJ_ON);
        sTitleScreenTimerTaskId = TASK_NONE;
        break;
    case 1:
        LoadPalette(gGraphics_TitleScreen_GameTitleLogoPals, BG_PLTT_ID(0), 13 * PLTT_SIZE_4BPP);
        DecompressAndCopyTileDataToVram(0, gGraphics_TitleScreen_GameTitleLogoTiles, 0, 0, 0);
        DecompressAndCopyTileDataToVram(0, gGraphics_TitleScreen_GameTitleLogoMap, 0, 0, 1);
        LoadPalette(gGraphics_TitleScreen_BoxArtMonPals, BG_PLTT_ID(13), PLTT_SIZE_4BPP);
        DecompressAndCopyTileDataToVram(1, gGraphics_TitleScreen_BoxArtMonTiles, 0, 0, 0);
        DecompressAndCopyTileDataToVram(1, gGraphics_TitleScreen_BoxArtMonMap, 0, 0, 1);
        LoadPalette(gGraphics_TitleScreen_BackgroundPals, BG_PLTT_ID(15), PLTT_SIZE_4BPP);
        DecompressAndCopyTileDataToVram(2, gGraphics_TitleScreen_CopyrightPressStartTiles, 0, 0, 0);
        DecompressAndCopyTileDataToVram(2, gGraphics_TitleScreen_CopyrightPressStartMap, 0, 0, 1);
        LoadPalette(gGraphics_TitleScreen_BackgroundPals, BG_PLTT_ID(14), PLTT_SIZE_4BPP);
        DecompressAndCopyTileDataToVram(3, sBorderBgTiles, 0, 0, 0);
        DecompressAndCopyTileDataToVram(3, sBorderBgMap, 0, 0, 1);
        LoadSpriteGfxAndPals();
        break;
    case 2:
        if (!FreeTempTileDataBuffersIfPossible())
        {
            BlendPalettes(PALETTES_BG, 16, RGB_BLACK);
            CreateTask(Task_TitleScreenMain, 4);
            sTitleScreenTimerTaskId = CreateTask(Task_TitleScreenTimer, 2);
            SetVBlankCallback(VBlankCB);
            SetMainCallback2(CB2_TitleScreenRun);
            m4aSongNumStart(MUS_TITLE);
        }
        return;
    }
    gMain.state++;
}

static void ResetGpuRegs(void)
{
    SetGpuReg(REG_OFFSET_DISPCNT,  0);
    SetGpuReg(REG_OFFSET_BLDCNT,   0);
    SetGpuReg(REG_OFFSET_BLDALPHA, 0);
    SetGpuReg(REG_OFFSET_BLDY,     0);
    SetGpuReg(REG_OFFSET_BG0HOFS,  0);
    SetGpuReg(REG_OFFSET_BG0VOFS,  0);
    SetGpuReg(REG_OFFSET_BG1HOFS,  0);
    SetGpuReg(REG_OFFSET_BG1VOFS,  0);
    SetGpuReg(REG_OFFSET_BG2HOFS,  0);
    SetGpuReg(REG_OFFSET_BG2VOFS,  0);
    SetGpuReg(REG_OFFSET_BG3HOFS,  0);
    SetGpuReg(REG_OFFSET_BG3VOFS,  0);
}

static void CB2_TitleScreenRun(void)
{
    RunTasks();
    AnimateSprites();
    BuildOamBuffer();
    UpdatePaletteFade();
}

static void VBlankCB(void)
{
    LoadOam();
    ProcessSpriteCopyRequests();
    TransferPlttBuffer();
    ScanlineEffect_InitHBlankDmaTransfer();

    if (sTitleScreenTimerTaskId != TASK_NONE)
        gTasks[sTitleScreenTimerTaskId].data[0]++;
}

static void Task_TitleScreenTimer(u8 taskId)
{
    s16 *data = gTasks[taskId].data;

    if (data[0] >= 2700)
    {
        sTitleScreenTimerTaskId = TASK_NONE;
        DestroyTask(taskId);
    }
}

// task data for Task_TitleScreenMain and the scenes
#define tSceneNum              data[0]
#define tState                 data[1]
#define tHasCreatedBlankSprite data[5]
#define tSlashSpriteId         data[6]

static void Task_TitleScreenMain(u8 taskId)
{
    s16 *data = gTasks[taskId].data;

    if (JOY_NEW(A_BUTTON | B_BUTTON | START_BUTTON)
        && tSceneNum != TITLESCREENSCENE_RUN
        && tSceneNum != TITLESCREENSCENE_RESTART
        && tSceneNum != TITLESCREENSCENE_CRY)
    {
        ScheduleStopScanlineEffect();
        LoadMainTitleScreenPalsAndResetBgs();
        SetPalOnOrCreateBlankSprite(tHasCreatedBlankSprite);
        SetTitleScreenScene(data, TITLESCREENSCENE_RUN);
    }
    else
        sSceneFuncs[tSceneNum](data);
}

static void SetTitleScreenScene(s16 *data, u8 sceneNum)
{
    tState = 0;
    tSceneNum = sceneNum;
}

static void SetTitleScreenScene_Init(s16 *data)
{
    struct ScanlineEffectParams params;

    HideBg(0);
    ShowBg(1);
    ShowBg(2);
    ShowBg(3);

    params.dmaDest = (volatile void *)REG_ADDR_BLDY;
    params.dmaControl = SCANLINE_EFFECT_DMACNT_16BIT;
    params.initState = 1;
    params.unused9 = 0;

    CpuFill16(0, gScanlineEffectRegBuffers[0], 0x140);
    CpuFill16(0, gScanlineEffectRegBuffers[1], 0x140);

    ScanlineEffect_SetParams(params);

    SetTitleScreenScene(data, TITLESCREENSCENE_FLASHSPRITE);
}

static void SetTitleScreenScene_FlashSprite(s16 *data)
{
    switch (tState)
    {
    case 0:
        SetGpuReg(REG_OFFSET_BLDCNT, BLDCNT_TGT1_BG1 | BLDCNT_EFFECT_LIGHTEN);
        SetGpuReg(REG_OFFSET_BLDY, 0);
        data[2] = 128;
        UpdateScanlineEffectRegBuffer(data[2]);
        tState++;
        break;
    case 1:
        data[2] -= 4;
        UpdateScanlineEffectRegBuffer(data[2]);
        if (data[2] < 0)
        {
            gScanlineEffect.state = 3;
            tState++;
        }
        break;
    case 2:
        SetGpuReg(REG_OFFSET_BLDCNT, 0);
        SetGpuReg(REG_OFFSET_BLDY, 0);
        SetTitleScreenScene(data, TITLESCREENSCENE_FADEIN);
    }
}

static void SetTitleScreenScene_FadeIn(s16 *data)
{
    switch (tState)
    {
    case 0:
        data[2] = 0;
        tState++;
        break;
    case 1:
        data[2]++;
        if (data[2] > 10)
        {
            TintPalette_GrayScale2(&gPlttBufferUnfaded[BG_PLTT_ID(13)], 16);
            BeginNormalPaletteFade(1 << 13, 9, 16, 0, RGB_BLACK);
            tState++;
        }
        break;
    case 2:
        if (!gPaletteFade.active)
        {
            data[2] = 0;
            tState++;
        }
        break;
    case 3:
        data[2]++;
        if (data[2] > 36)
        {
            CreateTask(Task_TitleScreen_SlideWin0, 3);
            BlendPalettesGradually(1 << 13, -4, 1, 16, RGB(30, 30, 31), 0, 0);
            data[2] = 0;
            tState++;
        }
        break;
    case 4:
        if (!IsBlendPalettesGraduallyTaskActive(0))
        {
            BlendPalettesGradually(1 << 13, -4, 15, 0, RGB(30, 30, 31), 0, 0);
            tState++;
        }
        break;
    case 5:
        data[2]++;
        if (data[2] > 20)
        {
            data[2] = 0;
            BlendPalettesGradually(1 << 13, -4, 1, 16, RGB(30, 30, 31), 0, 0);
            tState++;
        }
        break;
    case 6:
        if (!IsBlendPalettesGraduallyTaskActive(0))
        {
            BlendPalettesGradually(1 << 13, -4, 15, 0, RGB(30, 30, 31), 0, 0);
            tState++;
        }
        break;
    case 7:
        data[2]++;
        if (data[2] > 20)
        {
            data[2] = 0;
            BlendPalettesGradually(1 << 13, -3, 0, 16, RGB(30, 30, 31), 0, 0);
            tState++;
        }
        break;
    case 8:
        if (!IsBlendPalettesGraduallyTaskActive(0))
        {
            u32 palettes;
            tHasCreatedBlankSprite = TRUE;
            palettes = (PALETTES_BG & ~(1 << 13) & ~(1 << 14) & ~(1 << 15)) | (0x10000 << CreateBlankSprite());
            BlendPalettes(palettes, 16, RGB(30, 30, 31));
            BeginNormalPaletteFade(palettes, 1, 16, 0, RGB(30, 30, 31));
            ShowBg(0);
            CpuCopy16(gGraphics_TitleScreen_BoxArtMonPals, &gPlttBufferUnfaded[BG_PLTT_ID(13)], PLTT_SIZE_4BPP);
            BlendPalettesGradually(1 << 13, 1, 15, 0, RGB(30, 30, 31), 0, 0);
            tState++;
        }
        break;
    case 9:
        if (!IsBlendPalettesGraduallyTaskActive(0) && !gPaletteFade.active)
        {
            SetTitleScreenScene(data, TITLESCREENSCENE_RUN);
        }
        break;
    }
}

#define KEYSTROKE_DELSAVE (B_BUTTON | SELECT_BUTTON | DPAD_UP)
#define KEYSTROKE_BERRY_FIX (B_BUTTON | SELECT_BUTTON)

static void SetTitleScreenScene_Run(s16 *data)
{
    switch (tState)
    {
    case 0:
        SetHelpContext(HELPCONTEXT_TITLE_SCREEN);
        CreateTask(Task_TitleScreen_BlinkPressStart, 0);
#if defined(FIRERED)
        CreateTask(Task_FlameSpawner, 5);
#elif defined(LEAFGREEN)
        CreateTask(Task_LeafSpawner, 5);
#endif
        SetGpuRegsForTitleScreenRun();
        tSlashSpriteId = CreateSlashSprite();
        HelpSystem_Enable();
        tState++;
        // fallthrough
    case 1:
        if (JOY_HELD(KEYSTROKE_DELSAVE) == KEYSTROKE_DELSAVE)
        {
            DeactivateSlashSprite(tSlashSpriteId);
            DestroyTask(FindTaskIdByFunc(Task_TitleScreenMain));
            SetMainCallback2(CB2_FadeOutTransitionToSaveClearScreen);
        }
#if REVISION >= 0xA
        // Berry fix trigger has been removed.
#else
        else if (JOY_HELD(KEYSTROKE_BERRY_FIX) == KEYSTROKE_BERRY_FIX)
        {
            DeactivateSlashSprite(tSlashSpriteId);
            DestroyTask(FindTaskIdByFunc(Task_TitleScreenMain));
            SetMainCallback2(CB2_FadeOutTransitionToBerryFix);
        }
#endif
        else if (JOY_NEW(A_BUTTON | START_BUTTON))
        {
            SetTitleScreenScene(data, TITLESCREENSCENE_CRY);
        }
        else if (!FuncIsActiveTask(Task_TitleScreenTimer))
        {
            SetTitleScreenScene(data, TITLESCREENSCENE_RESTART);
        }
        break;
    }
}

static void SetGpuRegsForTitleScreenRun(void)
{
    SetGpuRegBits(REG_OFFSET_DISPCNT, DISPCNT_OBJWIN_ON);
    SetGpuReg(REG_OFFSET_WINOUT, WINOUT_WIN01_BG_ALL | WINOUT_WIN01_OBJ | WINOUT_WINOBJ_ALL);
    SetGpuReg(REG_OFFSET_BLDCNT, BLDCNT_TGT1_BG0 | BLDCNT_EFFECT_LIGHTEN);
    SetGpuReg(REG_OFFSET_BLDY, 13);
}

static void SetTitleScreenScene_Restart(s16 *data)
{
    switch (tState)
    {
    case 0:
        DeactivateSlashSprite(tSlashSpriteId);
        tState++;
        break;
    case 1:
        if (!gPaletteFade.active && !IsSlashSpriteDeactivated(tSlashSpriteId))
        {
            FadeOutMapMusic(10);
            BeginNormalPaletteFade(PALETTES_ALL, 3, 0, 0x10, RGB_BLACK);
            SignalEndTitleScreenPaletteSomethingTask();
            data[1]++;
        }
        break;
    case 2:
        if (IsNotWaitingForBGMStop() && !gPaletteFade.active)
        {
            DestroyTask(FindTaskIdByFunc(Task_TitleScreen_BlinkPressStart));
            data[2] = 0;
            tState++;
        }
        break;
    case 3:
        data[2]++;
        if (data[2] >= 20)
        {
            DestroyTask(FindTaskIdByFunc(Task_TitleScreen_BlinkPressStart));
            tState++;
        }
        break;
    case 4:
        HelpSystem_Disable();
        DestroyTask(FindTaskIdByFunc(Task_TitleScreenMain));
        SetMainCallback2(CB2_InitCopyrightScreenAfterTitleScreen);
        break;
    }
}

static void SetTitleScreenScene_Cry(s16 *data)
{
    switch (tState)
    {
    case 0:
        if (!gPaletteFade.active)
        {
            PlayCry_Normal(TITLE_SPECIES, 0);
            DeactivateSlashSprite(tSlashSpriteId);
            data[2] = 0;
            tState++;
        }
        break;
    case 1:
        if (data[2] < 90)
            data[2]++;
        else if (!IsSlashSpriteDeactivated(tSlashSpriteId))
        {
            BeginNormalPaletteFade((PALETTES_ALL & ~(1 << 0x1C) & ~(1 << 0x1D) & ~(1 << 0x1E) & ~(1 << 0x1F)), 0, 0, 16, RGB_WHITE);
            SignalEndTitleScreenPaletteSomethingTask();
            FadeOutBGM(4);
            tState++;
        }
        break;
    case 2:
        if (!gPaletteFade.active)
        {
            SeedRngAndSetTrainerId();
            SetSaveBlocksPointers();
            ResetMenuAndMonGlobals();
            Save_ResetSaveCounters();
            LoadGameSave(SAVE_NORMAL);
            if (gSaveFileStatus == SAVE_STATUS_EMPTY || gSaveFileStatus == SAVE_STATUS_INVALID)
                Sav2_ClearSetDefault();
            SetPokemonCryStereo(gSaveBlock2Ptr->optionsSound);
            InitHeap(gHeap, HEAP_SIZE);
            SetMainCallback2(CB2_InitMainMenu);
            DestroyTask(FindTaskIdByFunc(Task_TitleScreenMain));
        }
        break;
    }
}

#undef tSceneNum
#undef tState
#undef tHasCreatedBlankSprite
#undef tSlashSpriteId

static void Task_TitleScreen_SlideWin0(u8 taskId)
{
    s16 *data = gTasks[taskId].data;

    switch (data[0])
    {
    case 0:
        SetGpuRegBits(REG_OFFSET_DISPCNT, DISPCNT_WIN0_ON);
        SetGpuReg(REG_OFFSET_WININ, WININ_WIN0_ALL);
        SetGpuReg(REG_OFFSET_WINOUT, WINOUT_WIN01_BG0 | WINOUT_WIN01_BG1 | WINOUT_WIN01_BG2 | WINOUT_WIN01_OBJ | WINOUT_WIN01_CLR);
        SetGpuReg(REG_OFFSET_WIN0V, WIN_RANGE(0, DISPLAY_HEIGHT));
        SetGpuReg(REG_OFFSET_WIN0H, WIN_RANGE(0, 0));
        BlendPalettes(1 << 0xE, 0, RGB_BLACK);
        data[0]++;
        break;
    case 1:
        data[1] += 24 << 4;
        data[2] = data[1] >> 4;
        if (data[2] >= DISPLAY_WIDTH)
        {
            data[2] = DISPLAY_WIDTH;
            data[0]++;
        }
        SetGpuReg(REG_OFFSET_WIN0H, WIN_RANGE(0, data[2]));
        break;
    case 2:
        data[3]++;
        if (data[3] >= 10)
        {
            data[3] = 0;
            data[0]++;
        }
        break;
    case 3:
        SetGpuReg(REG_OFFSET_WINOUT, WINOUT_WIN01_BG0 | WINOUT_WIN01_BG1 | WINOUT_WIN01_BG3 | WINOUT_WIN01_OBJ | WINOUT_WIN01_CLR);
        SetGpuReg(REG_OFFSET_WIN0H, WIN_RANGE(DISPLAY_WIDTH, DISPLAY_WIDTH));
        ChangeBgX(2, -0xF000, 0);
        BlendPalettes(1 << 0xF, 0, RGB_BLACK);
        data[1] = 10 * 24 << 4;
        data[0]++;
        break;
    case 4:
        data[1] -= 24 << 4;
        data[2] = data[1] >> 4;
        if (data[2] <= 0)
        {
            data[2] = 0;
            data[0]++;
        }
        ChangeBgX(2, -data[2] << 8, 0);
        SetGpuReg(REG_OFFSET_WIN0H, WIN_RANGE(data[2], DISPLAY_WIDTH));
        break;
    case 5:
        ClearGpuRegBits(REG_OFFSET_DISPCNT, DISPCNT_WIN0_ON);
        DestroyTask(taskId);
        break;
    }
}

static void Task_TitleScreen_BlinkPressStart(u8 taskId)
{
    s16 *data = gTasks[taskId].data;
    s32 i;

    if (data[15] && gPaletteFade.active)
        data[14] = 1;
    if (data[14] && !gPaletteFade.active)
        DestroyTask(taskId);
    else
    {
        if (!data[1])
            data[2] = 60;
        else
            data[2] = 30;
        data[0]++;
        if (data[0] >= data[2])
        {
            data[0] = 0;
            data[1] ^= 1;
            if (data[1])
            {
                for (i = 0; i < 5; i++)
                {
                    gPlttBufferUnfaded[BG_PLTT_ID(15) + 1 + i] = gGraphics_TitleScreen_BackgroundPals[6];
                    gPlttBufferFaded[BG_PLTT_ID(15) + 1 + i] = gGraphics_TitleScreen_BackgroundPals[6];
                }
            }
            else
            {
                for (i = 0; i < 5; i++)
                {
                    gPlttBufferUnfaded[BG_PLTT_ID(15) + 1 + i] = gGraphics_TitleScreen_BackgroundPals[1 + i];
                    gPlttBufferFaded[BG_PLTT_ID(15) + 1 + i] = gGraphics_TitleScreen_BackgroundPals[1 + i];
                }
            }
            if (data[14])
            {
                BlendPalettes(0x00008000, gPaletteFade.y, gPaletteFade.blendColor);
            }
        }
    }
}

static void SignalEndTitleScreenPaletteSomethingTask(void)
{
    u8 taskId = FindTaskIdByFunc(Task_TitleScreen_BlinkPressStart);
    gTasks[taskId].data[15] = TRUE;
}

static void UpdateScanlineEffectRegBuffer(s16 y)
{
    s32 i;

    if (y >= 0)
        gScanlineEffectRegBuffers[gScanlineEffect.srcBuffer][y] = 16;

    for (i = 0; i < 16; i++)
    {
        if (y + i >= 0)
            gScanlineEffectRegBuffers[gScanlineEffect.srcBuffer][y + i] = 15 - i;

        if (y - i >= 0)
            gScanlineEffectRegBuffers[gScanlineEffect.srcBuffer][y - i] = 15 - i;

    }

    for (i = y + 16; i < 160; i++)
    {
        if (i >= 0)
            gScanlineEffectRegBuffers[gScanlineEffect.srcBuffer][i] = 0;
    }

    for (i = y - 16; i >= 0; i--)
    {
        if (i >= 0)
            gScanlineEffectRegBuffers[gScanlineEffect.srcBuffer][i] = 0;
    }
}

static void ScheduleStopScanlineEffect(void)
{
    if (gScanlineEffect.state)
        gScanlineEffect.state = 3;
    SetGpuReg(REG_OFFSET_BLDCNT, 0);
    SetGpuReg(REG_OFFSET_BLDY, 0);
}

static void LoadMainTitleScreenPalsAndResetBgs(void)
{
    u8 taskId;

    taskId = FindTaskIdByFunc(Task_TitleScreen_SlideWin0);
    if (taskId != TASK_NONE)
        DestroyTask(taskId);

    DestroyBlendPalettesGraduallyTask();
    ResetPaletteFadeControl();
    LoadPalette(gGraphics_TitleScreen_GameTitleLogoPals, BG_PLTT_ID(0), 13 * PLTT_SIZE_4BPP);
    LoadPalette(gGraphics_TitleScreen_BoxArtMonPals, BG_PLTT_ID(13), PLTT_SIZE_4BPP);
    LoadPalette(gGraphics_TitleScreen_BackgroundPals, BG_PLTT_ID(15), PLTT_SIZE_4BPP);
    LoadPalette(gGraphics_TitleScreen_BackgroundPals, BG_PLTT_ID(14), PLTT_SIZE_4BPP);
    ResetBgPositions();
    ClearGpuRegBits(REG_OFFSET_DISPCNT, DISPCNT_WIN0_ON | DISPCNT_WIN1_ON | DISPCNT_OBJWIN_ON);
    ShowBg(1);
    ShowBg(2);
    ShowBg(0);
    ShowBg(3);
}

static void CB2_FadeOutTransitionToSaveClearScreen(void)
{
    if (!UpdatePaletteFade())
        SetMainCallback2(CB2_SaveClearScreen_Init);
}

static void CB2_FadeOutTransitionToBerryFix(void)
{
    if (!UpdatePaletteFade())
    {
        m4aMPlayAllStop();
        SetMainCallback2(CB2_InitBerryFixProgram);
    }
}

static bool32 CreateCrossingSprite(s32 x, s32 y, s32 xspeed, s32 yspeed, u8 anim)
{
    u8 spriteId = CreateSprite(&sSpriteTemplate_FlameOrLeaf, x, y, 0);

    if (spriteId == MAX_SPRITES)
        return FALSE;
    gSprites[spriteId].data[0] = x * 16;   // sPosX
    gSprites[spriteId].data[1] = xspeed;   // sSpeedX
    gSprites[spriteId].data[2] = y * 16;   // sPosY
    gSprites[spriteId].data[3] = yspeed;   // sSpeedY
    gSprites[spriteId].data[4] = 0;
    gSprites[spriteId].data[6] = 0;
#if defined(FIRERED)
    gSprites[spriteId].callback = SpriteCallback_TitleScreenFlame;
#else
    gSprites[spriteId].callback = SpriteCallback_TitleScreenLeaf;
#endif
    StartSpriteAnim(&gSprites[spriteId], anim);
    return TRUE;
}

static void LoadSpriteGfxAndPals(void)
{
    s32 i;

    for (i = 0; i < NELEMS(sSpriteSheets); i++)
        LoadCompressedSpriteSheet(&sSpriteSheets[i]);
    LoadSpritePalettes(sSpritePals);
    CreateFaceOff();
}

// The sides swap between editions and THE POSES DO NOT (9.14, and 2.4 before
// it). Each creature is drawn once facing right; whichever stands on the right
// is flipped so both face the gap between them, and the one that lunges is
// still the one that lunges.
// Measured, not guessed -- tools/gbatitleview.py composites this screen offline
// and reports every layer's ink box. The subtitle occupies x 100-153 and a
// 64x64 sprite's art reaches about 31px either side of its centre, so anything
// closer in than 50 and 190 puts a creature on top of the edition's own name.
#define FACEOFF_LEFT_X   50
#define FACEOFF_RIGHT_X  190
#define FACEOFF_Y        104
#define FACEOFF_BOB      3       // pixels, peak to centre
#define FACEOFF_PHASE    64      // a quarter cycle between the two
#define FACEOFF_MEET_X   120     // halfway, the only place worth meeting

static void CreateFaceOff(void)
{
    u8 left, right;

#if defined(FIRERED)
    left  = CreateSprite(&sSpriteTemplate_FaceOffCode,  FACEOFF_LEFT_X,  FACEOFF_Y, 1);
    right = CreateSprite(&sSpriteTemplate_FaceOffCare,  FACEOFF_RIGHT_X, FACEOFF_Y, 1);
#else
    left  = CreateSprite(&sSpriteTemplate_FaceOffCare,  FACEOFF_LEFT_X,  FACEOFF_Y, 1);
    right = CreateSprite(&sSpriteTemplate_FaceOffCode,  FACEOFF_RIGHT_X, FACEOFF_Y, 1);
#endif
    // Sprite.hFlip is copied into the OAM by the ANIMATION system, and these
    // two have no animation -- gDummySpriteAnimTable never runs, so the field
    // was set and never read. For a non-affine sprite the flip lives in
    // oam.matrixNum, so put it there.
    if (right != MAX_SPRITES)
        gSprites[right].oam.matrixNum = ST_OAM_HFLIP;

    // They stood perfectly still through a reveal that takes several seconds,
    // which reads as artwork rather than as two creatures about to do
    // something. A slow breath fixes that, and the two are a quarter cycle
    // apart -- bobbing in lockstep would read as one animation played twice.
    if (left != MAX_SPRITES)
    {
        gSprites[left].callback = SpriteCallback_FaceOffIdle;
        gSprites[left].data[1] = 0;
    }
    if (right != MAX_SPRITES)
    {
        gSprites[right].callback = SpriteCallback_FaceOffIdle;
        gSprites[right].data[1] = FACEOFF_PHASE;
    }
}

static void SpriteCallback_FaceOffIdle(struct Sprite *sprite)
{
    sprite->data[0] += 2;
    sprite->y2 = Sin((sprite->data[0] + sprite->data[1]) & 0xFF, FACEOFF_BOB);
}

#if defined(FIRERED)

#define sPosX      data[0]
#define sSpeedX    data[1]
#define sPosY      data[2]
#define sSpeedY    data[3]

static void SpriteCallback_TitleScreenFlame(struct Sprite *sprite)
{
    s16 *data = sprite->data;
    s32 dx;

    // sSpeedX is SIGNED here rather than subtracted: the two sides travel in
    // opposite directions and the old code only ever went one way.
    sPosX += sSpeedX;
    sPosY += sSpeedY;
    sprite->x = sPosX >> 4;
    sprite->y = sPosY >> 4;

    if (data[6] == 0)
    {
        dx = sprite->x - FACEOFF_MEET_X;
        if (dx < 0)
            dx = -dx;
        if (dx <= 3)
        {
            data[6] = 1;
            sSpeedX = 0;
            sSpeedY = 0;
            StartSpriteAnim(sprite, ANIM_BLOOM);
        }
        else if (++data[4] > 240)
        {
            DestroySprite(sprite);      // never arrived; do not leak it
            return;
        }
    }
    if (sprite->animEnded)
        DestroySprite(sprite);
}

static bool32 CreateFlameSprite(s32 x, s32 y, s32 xspeed, s32 yspeed, bool32 createFlame)
{
    u8 spriteId;
    if (createFlame)
        spriteId = CreateSprite(&sSpriteTemplate_FlameOrLeaf, x, y, 0);
    else
        spriteId = CreateSprite(&sSpriteTemplate_BlankFlame, x, y, 0);

    if (spriteId != MAX_SPRITES)
    {
        gSprites[spriteId].sPosX = x * 16;
        gSprites[spriteId].sSpeedX = xspeed;
        gSprites[spriteId].sPosY = y * 16;
        gSprites[spriteId].sSpeedY = yspeed;
        gSprites[spriteId].data[4] = 0;
        gSprites[spriteId].data[5] = (xspeed * yspeed) % 16;
        gSprites[spriteId].data[6] = createFlame;
        gSprites[spriteId].callback = SpriteCallback_TitleScreenFlame;
        return TRUE;
    }
    return FALSE;
}

#undef sPosX
#undef sSpeedX
#undef sPosY
#undef sSpeedY

#define tState       data[0]
#define tTimer       data[1]
#define tDelay       data[2]
#define tOff_Seed       3   // data[3] and data[4]
#define tOffsetX     data[5]

static void Task_FlameSpawner(u8 taskId)
{
    s16 *data = gTasks[taskId].data;
    s32 y, drift;

    switch (tState)
    {
    case 0:
        TitleScreen_srand(taskId, 3, 30840);
        tState++;
        break;
    case 1:
        tTimer++;
        if (tTimer >= tDelay)
        {
            tTimer = 0;
            tDelay = 10;
            // One from each side, always. They are a pair or they are nothing:
            // a bloom needs two arrivals.
            y = FACEOFF_Y - 14 + (TitleScreen_rand(taskId, 3) % 28);
            drift = (TitleScreen_rand(taskId, 3) % 5) - 2;
            CreateCrossingSprite(FACEOFF_LEFT_X + 30, y, 10, drift, ANIM_CODE);
            drift = (TitleScreen_rand(taskId, 3) % 5) - 2;
            CreateCrossingSprite(FACEOFF_RIGHT_X - 30, y, -10, drift, ANIM_MUSIC);
        }
    }
}

#undef tState
#undef tTimer
#undef tDelay
#undef tOff_Seed
#undef tOffsetX

#elif defined(LEAFGREEN)

#define sPosX        data[0]
#define sSpeedX      data[1]
#define sPosY        data[2]
#define sSpeedY      data[3]

static void SpriteCallback_TitleScreenLeaf(struct Sprite *sprite)
{
    s16 *data = sprite->data;
    s32 dx;

    // sSpeedX is SIGNED here rather than subtracted: the two sides travel in
    // opposite directions and the old code only ever went one way.
    sPosX += sSpeedX;
    sPosY += sSpeedY;
    sprite->x = sPosX >> 4;
    sprite->y = sPosY >> 4;

    if (data[6] == 0)
    {
        dx = sprite->x - FACEOFF_MEET_X;
        if (dx < 0)
            dx = -dx;
        if (dx <= 3)
        {
            data[6] = 1;
            sSpeedX = 0;
            sSpeedY = 0;
            StartSpriteAnim(sprite, ANIM_BLOOM);
        }
        else if (++data[4] > 240)
        {
            DestroySprite(sprite);      // never arrived; do not leak it
            return;
        }
    }
    if (sprite->animEnded)
        DestroySprite(sprite);
}

static void CreateLeafSprite(s32 y, s32 xspeed, s32 yspeed)
{
    u8 spriteId = CreateSprite(&sSpriteTemplate_FlameOrLeaf, DISPLAY_WIDTH, y, 0);
    if (spriteId != MAX_SPRITES)
    {
        gSprites[spriteId].sPosX = DISPLAY_WIDTH * 16;
        gSprites[spriteId].sSpeedX = xspeed;
        gSprites[spriteId].sPosY = y * 16;
        gSprites[spriteId].sSpeedY = yspeed;
        gSprites[spriteId].callback = SpriteCallback_TitleScreenLeaf;
    }
}

#undef sPosX
#undef sSpeedX
#undef sPosY
#undef sSpeedY

static void SpriteCallback_Streak(struct Sprite *sprite)
{
    sprite->x -= 7;
    if (sprite->x < -16)
    {
        sprite->x = DISPLAY_WIDTH + 16;
        sprite->data[7]++;
        if (sprite->data[7] >= ARRAY_COUNT(sStreakYPositions))
            sprite->data[7] = 0;
        sprite->y = sStreakYPositions[sprite->data[7]];
    }
}

static void CreateStreakSprites(void)
{
    int i;
    u8 spriteId;
    for (i = 0; i < 4; i++)
    {
        spriteId = CreateSprite(&sSpriteTemplate_Streak, DISPLAY_WIDTH + 16 + 40 * i, sStreakYPositions[i], 0xFF);
        if (spriteId != MAX_SPRITES)
        {
            gSprites[spriteId].data[7] = i;
            gSprites[spriteId].callback = SpriteCallback_Streak;
        }
    }
}

#define tState       data[0]
#define tTimer       data[1]
#define tDelay       data[2]
#define tOff_Seed       3   // data[3] and data[4]

static void Task_LeafSpawner(u8 taskId)
{
    s16 *data = gTasks[taskId].data;
    s32 y, drift;

    switch (tState)
    {
    case 0:
        TitleScreen_srand(taskId, 3, 30840);
        tState++;
        break;
    case 1:
        tTimer++;
        if (tTimer >= tDelay)
        {
            tTimer = 0;
            tDelay = 10;
            // One from each side, always. They are a pair or they are nothing:
            // a bloom needs two arrivals.
            y = FACEOFF_Y - 14 + (TitleScreen_rand(taskId, 3) % 28);
            drift = (TitleScreen_rand(taskId, 3) % 5) - 2;
            CreateCrossingSprite(FACEOFF_LEFT_X + 30, y, 10, drift, ANIM_CODE);
            drift = (TitleScreen_rand(taskId, 3) % 5) - 2;
            CreateCrossingSprite(FACEOFF_RIGHT_X - 30, y, -10, drift, ANIM_MUSIC);
        }
    }
}

#undef tState
#undef tData1
#undef tData2
#undef tData3And4

#endif //FRLG

static void TitleScreen_srand(u8 taskId, u8 field, u16 seed)
{
    SetWordTaskArg(taskId, field, seed);
}

static u16 TitleScreen_rand(u8 taskId, u8 field)
{
    u32 rngval;

    rngval = GetWordTaskArg(taskId, field);
    rngval = ISO_RANDOMIZE1(rngval);
    SetWordTaskArg(taskId, field, rngval);
    return rngval >> 16;
}

static u32 CreateBlankSprite(void)
{
    CreateSprite(&sSpriteTemplate_BlankSprite, 24, 144, 0);
    return IndexOfSpritePaletteTag(PAL_TAG_SLASH);
}

static void SetPalOnOrCreateBlankSprite(bool32 hasCreatedBlankSprite)
{
    u32 palIdx;

    if (hasCreatedBlankSprite)
    {
        palIdx = IndexOfSpritePaletteTag(PAL_TAG_SLASH);
        LoadPalette(gTitleScreen_Slash_Pal, OBJ_PLTT_ID(palIdx), PLTT_SIZE_4BPP);
    }
    else
        CreateBlankSprite();
}

#define sState       data[0]
#define sTimer       data[1]
#define sDeactivate  data[2]

static u8 CreateSlashSprite(void)
{
    u8 spriteId = CreateSprite(&sSlashSpriteTemplate, -32, 27, 1);
    if (spriteId != MAX_SPRITES)
    {
        gSprites[spriteId].callback = SpriteCallback_Slash;
        gSprites[spriteId].sTimer = 540;
    }
    return spriteId;
}

static void DeactivateSlashSprite(u8 spriteId)
{
    if (spriteId != MAX_SPRITES)
        gSprites[spriteId].sDeactivate = TRUE;
}

static bool32 IsSlashSpriteDeactivated(u8 spriteId)
{
    if (spriteId != MAX_SPRITES)
        return gSprites[spriteId].sState ^ 2 ? TRUE : FALSE;
    else
        return FALSE;
}

static void SpriteCallback_Slash(struct Sprite *sprite)
{
    switch (sprite->sState)
    {
    case 0:
        if (sprite->sDeactivate)
        {
            sprite->invisible = TRUE;
            sprite->sState = 2;
        }
        sprite->sTimer--;
        if (sprite->sTimer == 0)
        {
            sprite->invisible = FALSE;
            sprite->sState = 1;
        }
        break;
    case 1:
        sprite->x += 9;
        if (sprite->x == 67)
            sprite->y -= 7;

        if (sprite->x == 148)
            sprite->y += 7;

        if (sprite->x > DISPLAY_WIDTH + 32)
        {
            sprite->invisible = TRUE;
            if (sprite->sDeactivate)
                sprite->sState = 2;
            else
            {
                sprite->x = -32;
                sprite->sTimer = 540;
                sprite->sState = 0;
            }
        }
        break;
    case 2:
        break;
    }
}

#undef sState
#undef sTimer
#undef sDeactivate
