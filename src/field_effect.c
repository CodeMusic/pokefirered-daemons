#include "global.h"
#include "gflib.h"
#include "decompress.h"
#include "event_data.h"
#include "event_object_movement.h"
#include "field_camera.h"
#include "field_control_avatar.h"
#include "field_effect.h"
#include "field_effect_helpers.h"
#include "field_effect_scripts.h"
#include "field_fadetransition.h"
#include "field_player_avatar.h"
#include "field_weather.h"
#include "fieldmap.h"
#include "help_system.h"
#include "metatile_behavior.h"
#include "new_menu_helpers.h"
#include "overworld.h"
#include "party_menu.h"
#include "quest_log.h"
#include "random.h"
#include "script.h"
#include "special_field_anim.h"
#include "task.h"
#include "trainer_pokemon_sprites.h"
#include "trig.h"
#include "util.h"
#include "constants/event_object_movement.h"
#include "constants/metatile_behaviors.h"
#include "constants/songs.h"
#include "constants/sound.h"
#include "daemon_streaks.h"
#include "daemons_time.h"

extern struct CompressedSpritePalette gMonPaletteTable[]; // Intentionally declared (incorrectly) without const in order to match
extern const struct CompressedSpritePalette gTrainerFrontPicPaletteTable[];
extern const struct CompressedSpriteSheet gTrainerFrontPicTable[];

#define subsprite_table(ptr) {.subsprites = ptr, .subspriteCount = (sizeof ptr) / (sizeof(struct Subsprite))}
#define FIELD_EFFECT_COUNT 32

EWRAM_DATA u32 gFieldEffectArguments[8] = {0};

static u8 sFieldEffectActiveList[FIELD_EFFECT_COUNT];

static void FieldEffectActiveListAdd(u8 fldeff);
static bool8 FieldEffectCmd_loadtiles(const u8 **script, u32 *result);
static bool8 FieldEffectCmd_loadfadedpal(const u8 **script, u32 *result);
static bool8 FieldEffectCmd_loadpal(const u8 **script, u32 *result);
static bool8 FieldEffectCmd_callnative(const u8 **script, u32 *result);
static bool8 FieldEffectCmd_end(const u8 **script, u32 *result);
static bool8 FieldEffectCmd_loadgfx_callnative(const u8 **script, u32 *result);
static bool8 FieldEffectCmd_loadtiles_callnative(const u8 **script, u32 *result);
static bool8 FieldEffectCmd_loadfadedpal_callnative(const u8 **script, u32 *result);
static void FieldEffectScript_LoadTiles(const u8 **script);
static void FieldEffectScript_LoadFadedPal(const u8 **script);
static void FieldEffectScript_LoadPal(const u8 **script);
static void FieldEffectScript_CallNative(const u8 **script, u32 *result);
static void FieldEffectFreeTilesIfUnused(u16 tilesTag);
static void FieldEffectFreePaletteIfUnused(u8 paletteNum);
static void Task_PokecenterHeal(u8 taskId);
static void SpriteCB_PokeballGlow(struct Sprite *sprite);
static void SpriteCB_PokecenterMonitor(struct Sprite *sprite);
static void SpriteCB_HallOfFameMonitor(struct Sprite *sprite);

// Unused
static const u16 sNewGameOakObject_Gfx[] = INCBIN_U16("graphics/field_effects/pics/new_game_oak.4bpp");
static const u16 sNewGameOakObject_Pal[] = INCBIN_U16("graphics/field_effects/pics/new_game_oak.gbapal");

static const u16 sPokeballGlow_Gfx[] = INCBIN_U16("graphics/field_effects/pics/pokeball_glow.4bpp");
static const u16 sPokeballGlow_Pal[] = INCBIN_U16("graphics/field_effects/pics/pokeball_glow.gbapal");
static const u16 sPokecenterMonitor_Gfx[] = INCBIN_U16("graphics/field_effects/pics/pokemoncenter_monitor.4bpp");
static const u16 sHofMonitor_Pal[] = INCBIN_U16("graphics/field_effects/pics/hof_monitor.gbapal");
static const u16 sHofMonitor_Gfx[] = INCBIN_U16("graphics/field_effects/pics/hof_monitor.4bpp");

static const u16 sFieldMoveStreaksOutdoors_Gfx[] = INCBIN_U16("graphics/field_effects/pics/field_move_streaks_outdoors.4bpp");
static const u16 sFieldMoveStreaksOutdoors_Pal[] = INCBIN_U16("graphics/field_effects/pics/field_move_streaks_outdoors.gbapal");
static const u16 sFieldMoveStreaksOutdoors_Tilemap[] = INCBIN_U16("graphics/field_effects/pics/field_move_streaks_outdoors.bin");

static const u16 sFieldMoveStreaksIndoors_Gfx[] = INCBIN_U16("graphics/field_effects/pics/field_move_streaks_indoors.4bpp");
static const u16 sFieldMoveStreaksIndoors_Pal[] = INCBIN_U16("graphics/field_effects/pics/field_move_streaks_indoors.gbapal");
static const u16 sFieldMoveStreaksIndoors_Tilemap[] = INCBIN_U16("graphics/field_effects/pics/field_move_streaks_indoors.bin");

static const u16 sRockFragment_TopLeft[] = INCBIN_U16("graphics/field_effects/pics/deoxys_rock_fragment_top_left.4bpp");
static const u16 sRockFragment_TopRight[] = INCBIN_U16("graphics/field_effects/pics/deoxys_rock_fragment_top_right.4bpp");
static const u16 sRockFragment_BottomLeft[] = INCBIN_U16("graphics/field_effects/pics/deoxys_rock_fragment_bottom_left.4bpp");
static const u16 sRockFragment_BottomRight[] = INCBIN_U16("graphics/field_effects/pics/deoxys_rock_fragment_bottom_right.4bpp");

static bool8 (*const sFldEffScrcmdTable[])(const u8 **script, u32 *result) = {
    FieldEffectCmd_loadtiles,
    FieldEffectCmd_loadfadedpal,
    FieldEffectCmd_loadpal,
    FieldEffectCmd_callnative,
    FieldEffectCmd_end,
    FieldEffectCmd_loadgfx_callnative,
    FieldEffectCmd_loadtiles_callnative,
    FieldEffectCmd_loadfadedpal_callnative
};

static const struct OamData sNewGameOakOamAttributes = {
    .y = 0,
    .affineMode = ST_OAM_AFFINE_OFF,
    .objMode = ST_OAM_OBJ_NORMAL,
    .mosaic = FALSE,
    .bpp = ST_OAM_4BPP,
    .shape = SPRITE_SHAPE(64x64),
    .x = 0,
    .matrixNum = 0,
    .size = SPRITE_SIZE(64x64),
    .tileNum = 0x000,
    .priority = 0,
    .paletteNum = 0,
    .affineParam = 0
};

static const struct OamData sOamData_8x8 = {
    .y = 0,
    .affineMode = ST_OAM_AFFINE_OFF,
    .objMode = ST_OAM_OBJ_NORMAL,
    .mosaic = FALSE,
    .bpp = ST_OAM_4BPP,
    .shape = SPRITE_SHAPE(8x8),
    .x = 0,
    .matrixNum = 0,
    .size = SPRITE_SIZE(8x8),
    .tileNum = 0x000,
    .priority = 0,
    .paletteNum = 0,
    .affineParam = 0
};

static const struct OamData sOamData_16x16 = {
    .y = 0,
    .affineMode = ST_OAM_AFFINE_OFF,
    .objMode = ST_OAM_OBJ_NORMAL,
    .mosaic = FALSE,
    .bpp = ST_OAM_4BPP,
    .shape = SPRITE_SHAPE(16x16),
    .x = 0,
    .matrixNum = 0,
    .size = SPRITE_SIZE(16x16),
    .tileNum = 0x000,
    .priority = 0,
    .paletteNum = 0,
    .affineParam = 0
};

static const struct SpriteFrameImage sNewGameOakObjectSpriteFrames[] = {
    {sNewGameOakObject_Gfx, 0x800}
};

static const struct SpritePalette sNewGameOakObjectPaletteInfo = {
    sNewGameOakObject_Pal, 4102
};

static const union AnimCmd sNewGameOakAnim[] = {
    ANIMCMD_FRAME(0, 1),
    ANIMCMD_END
};

static const union AnimCmd *const sNewGameOakAnimTable[] = {
    sNewGameOakAnim
};

static const struct SpriteTemplate sNewGameOakObjectTemplate = {
    .tileTag = TAG_NONE,
    .paletteTag = 0x1006,
    .oam = &sNewGameOakOamAttributes,
    .anims = sNewGameOakAnimTable,
    .images = sNewGameOakObjectSpriteFrames,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCallbackDummy
};

const struct SpritePalette gSpritePalette_PokeballGlow = {
    sPokeballGlow_Pal, 4103
};

const struct SpritePalette gSpritePalette_HofMonitor = {
    sHofMonitor_Pal, 4112
};

static const struct OamData sOamData_32x16 = {
    .y = 0,
    .affineMode = ST_OAM_AFFINE_OFF,
    .objMode = ST_OAM_OBJ_NORMAL,
    .mosaic = FALSE,
    .bpp = ST_OAM_4BPP,
    .shape = SPRITE_SHAPE(32x16),
    .x = 0,
    .matrixNum = 0,
    .size = SPRITE_SIZE(32x16),
    .tileNum = 0x000,
    .priority = 0,
    .paletteNum = 0,
    .affineParam = 0
};

static const struct SpriteFrameImage sPicTable_PokeballGlow[] = {
    {sPokeballGlow_Gfx, 0x20}
};

static const struct SpriteFrameImage sPicTable_PokecenterMonitor[] = {
    {sPokecenterMonitor_Gfx + 0x000, 0x100},
    {sPokecenterMonitor_Gfx + 0x080, 0x100},
    {sPokecenterMonitor_Gfx + 0x100, 0x100},
    {sPokecenterMonitor_Gfx + 0x180, 0x100}
};

static const struct SpriteFrameImage sPicTable_HofMonitor[] = {
    {sHofMonitor_Gfx + 0x00, 0x80},
    {sHofMonitor_Gfx + 0x40, 0x80},
    {sHofMonitor_Gfx + 0x80, 0x80},
    {sHofMonitor_Gfx + 0xC0, 0x80}
};

// Unused, leftover from RSE
static const struct Subsprite sSubsprites_PokecenterMonitor[] =
{
    {
        .x = -12,
        .y =  -8,
        .shape = SPRITE_SHAPE(16x8),
        .size = SPRITE_SIZE(16x8),
        .tileOffset = 0,
        .priority = 2
    }, {
        .x =  4,
        .y = -8,
        .shape = SPRITE_SHAPE(8x8),
        .size = SPRITE_SIZE(8x8),
        .tileOffset = 2,
        .priority = 2
    }, {
        .x = -12,
        .y =   0,
        .shape = SPRITE_SHAPE(16x8),
        .size = SPRITE_SIZE(16x8),
        .tileOffset = 3,
        .priority = 2
    }, {
        .x = 4,
        .y = 0,
        .shape = SPRITE_SHAPE(8x8),
        .size = SPRITE_SIZE(8x8),
        .tileOffset = 5,
        .priority = 2
    }
};

// Unused, leftover from RSE
static const struct SubspriteTable sSubspriteTable_PokecenterMonitor = subsprite_table(sSubsprites_PokecenterMonitor);

// Unused, leftover from RSE
static const struct Subsprite sSubsprites_HofMonitorBig[] =
{
    {
        .x = -32,
        .y = -8,
        .shape = SPRITE_SHAPE(32x8),
        .size = SPRITE_SIZE(32x8),
        .tileOffset = 0,
        .priority = 2
    }, {
        .x =  0,
        .y = -8,
        .shape = SPRITE_SHAPE(32x8),
        .size = SPRITE_SIZE(32x8),
        .tileOffset = 4,
        .priority = 2
    }, {
        .x = -32,
        .y =  0,
        .shape = SPRITE_SHAPE(32x8),
        .size = SPRITE_SIZE(32x8),
        .tileOffset = 8,
        .priority = 2
    }, {
        .x =   0,
        .y =  0,
        .shape = SPRITE_SHAPE(32x8),
        .size = SPRITE_SIZE(32x8),
        .tileOffset = 12,
        .priority = 2
    }
};

// Unused, leftover from RSE
static const struct SubspriteTable sSubspriteTable_HofMonitorBig = subsprite_table(sSubsprites_HofMonitorBig);

static const union AnimCmd sAnim_Static[] = {
    ANIMCMD_FRAME(0, 1),
    ANIMCMD_JUMP(0)
};

static const union AnimCmd sAnim_Flicker[] = {
    ANIMCMD_FRAME(1, 5),
    ANIMCMD_FRAME(2, 5),
    ANIMCMD_FRAME(3, 7),
    ANIMCMD_FRAME(2, 5),
    ANIMCMD_FRAME(1, 5),
    ANIMCMD_FRAME(0, 5),
    ANIMCMD_LOOP(3),
    ANIMCMD_END
};

static const union AnimCmd *const sAnims_Flicker[] = {
    sAnim_Static,
    sAnim_Flicker
};

static const union AnimCmd sAnim_HofMonitor[] = {
    ANIMCMD_FRAME(3, 8),
    ANIMCMD_FRAME(2, 8),
    ANIMCMD_FRAME(1, 8),
    ANIMCMD_FRAME(0, 8),
    ANIMCMD_FRAME(1, 8),
    ANIMCMD_FRAME(2, 8),
    ANIMCMD_LOOP(2),
    ANIMCMD_FRAME(1, 8),
    ANIMCMD_FRAME(0, 8),
    ANIMCMD_END
};

static const union AnimCmd *const sAnims_HofMonitor[] = {
    sAnim_HofMonitor
};

static const struct SpriteTemplate sSpriteTemplate_PokeballGlow = {
    .tileTag = TAG_NONE,
    .paletteTag = 4103,
    .oam = &sOamData_8x8,
    .anims = sAnims_Flicker,
    .images = sPicTable_PokeballGlow,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCB_PokeballGlow
};

static const struct SpriteTemplate sSpriteTemplate_PokecenterMonitor = {
    .tileTag = TAG_NONE,
    .paletteTag = 4103,
    .oam = &sOamData_32x16,
    .anims = sAnims_Flicker,
    .images = sPicTable_PokecenterMonitor,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCB_PokecenterMonitor
};

static const struct SpriteTemplate sSpriteTemplate_HofMonitor = {
    .tileTag = TAG_NONE,
    .paletteTag = 4112,
    .oam = &sOamData_16x16,
    .anims = sAnims_HofMonitor,
    .images = sPicTable_HofMonitor,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCB_HallOfFameMonitor
};


u32 FieldEffectStart(u8 fldeff)
{
    const u8 *script;
    u32 result;
    FieldEffectActiveListAdd(fldeff);
    script = gFieldEffectScriptPointers[fldeff];
    while (sFldEffScrcmdTable[*script](&script, &result))
        ;
    return result;
}

static bool8 FieldEffectCmd_loadtiles(const u8 **script, u32 *result)
{
    (*script)++;
    FieldEffectScript_LoadTiles(script);
    return TRUE;
}

static bool8 FieldEffectCmd_loadfadedpal(const u8 **script, u32 *result)
{
    (*script)++;
    FieldEffectScript_LoadFadedPal(script);
    return TRUE;
}

static bool8 FieldEffectCmd_loadpal(const u8 **script, u32 *result)
{
    (*script)++;
    FieldEffectScript_LoadPal(script);
    return TRUE;
}
static bool8 FieldEffectCmd_callnative(const u8 **script, u32 *result)
{
    (*script)++;
    FieldEffectScript_CallNative(script, result);
    return TRUE;
}

static bool8 FieldEffectCmd_end(const u8 **script, u32 *result)
{
    return FALSE;
}

static bool8 FieldEffectCmd_loadgfx_callnative(const u8 **script, u32 *result)
{
    (*script)++;
    FieldEffectScript_LoadTiles(script);
    FieldEffectScript_LoadFadedPal(script);
    FieldEffectScript_CallNative(script, result);
    return TRUE;
}

static bool8 FieldEffectCmd_loadtiles_callnative(const u8 **script, u32 *result)
{
    (*script)++;
    FieldEffectScript_LoadTiles(script);
    FieldEffectScript_CallNative(script, result);
    return TRUE;
}

static bool8 FieldEffectCmd_loadfadedpal_callnative(const u8 **script, u32 *result)
{
    (*script)++;
    FieldEffectScript_LoadFadedPal(script);
    FieldEffectScript_CallNative(script, result);
    return TRUE;
}

static u32 FieldEffectScript_ReadWord(const u8 **script)
{
    return T2_READ_32(*script);
}

static void FieldEffectScript_LoadTiles(const u8 **script)
{
    const struct SpriteSheet * spriteSheet = (const struct SpriteSheet * )FieldEffectScript_ReadWord(script);
    if (GetSpriteTileStartByTag(spriteSheet->tag) == 0xFFFF)
        LoadSpriteSheet(spriteSheet);
    *script += sizeof(u32);
}

void ApplyGlobalFieldPaletteTint(u8 paletteIdx)
{
    // Object palettes ask the same question the tilesets do, or the town greys
    // and every person standing in it does not.
    switch (DaemonsFieldTint())
    {
    case 0:
        return;
    case 1:
        TintPalette_GrayScale(&gPlttBufferUnfaded[OBJ_PLTT_ID2(paletteIdx)], 16);
        break;
    case 2:
        TintPalette_SepiaTone(&gPlttBufferUnfaded[OBJ_PLTT_ID2(paletteIdx)], 16);
        break;
    case 3:
        QuestLog_BackUpPalette(OBJ_PLTT_ID2(paletteIdx), 16);
        TintPalette_GrayScale(&gPlttBufferUnfaded[OBJ_PLTT_ID2(paletteIdx)], 16);
        break;
    case DAEMONS_TINT_WATCH:   // T-268: the people outdoors are in the same light as the ground
        DaemonsTintForWatch(&gPlttBufferUnfaded[OBJ_PLTT_ID2(paletteIdx)], 16);
        break;
    default:
        return;
    }
    CpuFastCopy(&gPlttBufferUnfaded[OBJ_PLTT_ID2(paletteIdx)], &gPlttBufferFaded[OBJ_PLTT_ID2(paletteIdx)], PLTT_SIZE_4BPP);
}

static void FieldEffectScript_LoadFadedPal(const u8 **script)
{
    const struct SpritePalette * spritePalette = (const struct SpritePalette * )FieldEffectScript_ReadWord(script);
    u8 idx = IndexOfSpritePaletteTag(spritePalette->tag);
    LoadSpritePalette(spritePalette);
    if (idx == 0xFF)
        ApplyGlobalFieldPaletteTint(IndexOfSpritePaletteTag(spritePalette->tag));
    UpdateSpritePaletteWithWeather(IndexOfSpritePaletteTag(spritePalette->tag));
    *script += sizeof(u32);
}

static void FieldEffectScript_LoadPal(const u8 **script)
{
    const struct SpritePalette * spritePalette = (const struct SpritePalette * )FieldEffectScript_ReadWord(script);
    u8 idx = IndexOfSpritePaletteTag(spritePalette->tag);
    LoadSpritePalette(spritePalette);
    //  T-279: vanilla tinted here only when the palette was ALREADY loaded -- the reverse of LoadFadedPal above -- so a
    //  first load went untinted and every reuse tinted again. The Quest Log's grey never showed it (grey of grey is
    //  grey); the watch's tint is not idempotent, so it would have darkened the ball's trail each time it was thrown.
    if (idx == 0xFF)
        ApplyGlobalFieldPaletteTint(IndexOfSpritePaletteTag(spritePalette->tag));
    *script += sizeof(u32);
}

//  T-279: the field effects' shared palettes -- grass, sand, ripples, and BLANCHE's pale grass -- loaded again under the
//  current light when the map is relit (T-276), so a rustle in the grass is not left in the light of an hour ago.
extern const struct SpritePalette gSpritePalette_GeneralFieldEffect0;
extern const struct SpritePalette gSpritePalette_GeneralFieldEffect1;
extern const struct SpritePalette gSpritePalette_BlancheGrass;
static const struct SpritePalette *const sRelitFieldEffectPalettes[] = {
    &gSpritePalette_GeneralFieldEffect0, &gSpritePalette_GeneralFieldEffect1, &gSpritePalette_BlancheGrass,
};

void DaemonsRetintFieldEffectPalettes(void)
{
    u8 i, idx;

    for (i = 0; i < ARRAY_COUNT(sRelitFieldEffectPalettes); i++)
    {
        idx = IndexOfSpritePaletteTag(sRelitFieldEffectPalettes[i]->tag);
        if (idx == 0xFF)
            continue;
        LoadPalette(sRelitFieldEffectPalettes[i]->data, OBJ_PLTT_ID(idx), PLTT_SIZE_4BPP);
        ApplyGlobalFieldPaletteTint(idx);
        UpdateSpritePaletteWithWeather(idx);
    }
}

static void FieldEffectScript_CallNative(const u8 **script, u32 *result)
{
    u32 (*func)(void) = (u32 (*)(void))FieldEffectScript_ReadWord(script);
    *result = func();
    *script += sizeof(u32);
}

static void FieldEffectFreeGraphicsResources(struct Sprite *sprite)
{
    u16 tileStart = sprite->sheetTileStart;
    u8 paletteNum = sprite->oam.paletteNum;
    DestroySprite(sprite);
    FieldEffectFreeTilesIfUnused(tileStart);
    FieldEffectFreePaletteIfUnused(paletteNum);
}

void FieldEffectStop(struct Sprite *sprite, u8 fldeff)
{
    FieldEffectFreeGraphicsResources(sprite);
    FieldEffectActiveListRemove(fldeff);
}

static void FieldEffectFreeTilesIfUnused(u16 tileStart)
{
    u8 i;
    u16 tileTag = GetSpriteTileTagByTileStart(tileStart);
    if (tileTag == TAG_NONE)
        return;
    for (i = 0; i < MAX_SPRITES; i++)
    {
        if (gSprites[i].inUse && gSprites[i].usingSheet && tileStart == gSprites[i].sheetTileStart)
            return;
    }
    FreeSpriteTilesByTag(tileTag);
}

static void FieldEffectFreePaletteIfUnused(u8 paletteNum)
{
    u8 i;
    u16 paletteTag = GetSpritePaletteTagByPaletteNum(paletteNum);
    if (paletteTag == TAG_NONE)
        return;
    for (i = 0; i < MAX_SPRITES; i++)
    {
        if (gSprites[i].inUse && gSprites[i].oam.paletteNum == paletteNum)
            return;
    }
    FreeSpritePaletteByTag(paletteTag);
}

void FieldEffectActiveListClear(void)
{
    u8 i;
    for (i = 0; i < FIELD_EFFECT_COUNT; i++)
    {
        sFieldEffectActiveList[i] = 0xFF;
    }
}

static void FieldEffectActiveListAdd(u8 fldeff)
{
    u8 i;
    for (i = 0; i < FIELD_EFFECT_COUNT; i++)
    {
        if (sFieldEffectActiveList[i] == 0xFF)
        {
            sFieldEffectActiveList[i] = fldeff;
            return;
        }
    }
}

void FieldEffectActiveListRemove(u8 fldeff)
{
    u8 i;
    for (i = 0; i < FIELD_EFFECT_COUNT; i++)
    {
        if (sFieldEffectActiveList[i] == fldeff)
        {
            sFieldEffectActiveList[i] = 0xFF;
            return;
        }
    }
}

bool8 FieldEffectActiveListContains(u8 fldeff)
{
    u8 i;
    for (i = 0; i < FIELD_EFFECT_COUNT; i++)
    {
        if (sFieldEffectActiveList[i] == fldeff)
        {
            return TRUE;
        }
    }
    return FALSE;
}

u8 CreateTrainerSprite(u8 trainerSpriteID, s16 x, s16 y, u8 subpriority, u8 *buffer)
{
    struct SpriteTemplate spriteTemplate;
    LoadCompressedSpritePaletteOverrideBuffer(&gTrainerFrontPicPaletteTable[trainerSpriteID], buffer);
    LoadCompressedSpriteSheetOverrideBuffer(&gTrainerFrontPicTable[trainerSpriteID], buffer);
    spriteTemplate.tileTag = gTrainerFrontPicTable[trainerSpriteID].tag;
    spriteTemplate.paletteTag = gTrainerFrontPicPaletteTable[trainerSpriteID].tag;
    spriteTemplate.oam = &sNewGameOakOamAttributes;
    spriteTemplate.anims = gDummySpriteAnimTable;
    spriteTemplate.images = NULL;
    spriteTemplate.affineAnims = gDummySpriteAffineAnimTable;
    spriteTemplate.callback = SpriteCallbackDummy;
    return CreateSprite(&spriteTemplate, x, y, subpriority);
}

// Unused
static void LoadTrainerGfx_TrainerCard(u8 gender, u16 palOffset, u8 *dest)
{
    LZDecompressVram(gTrainerFrontPicTable[gender].data, dest);
    LoadCompressedPalette(gTrainerFrontPicPaletteTable[gender].data, palOffset, PLTT_SIZE_4BPP);
}

// Unused
static u8 AddNewGameBirchObject(s16 x, s16 y, u8 subpriority)
{
    LoadSpritePalette(&sNewGameOakObjectPaletteInfo);
    return CreateSprite(&sNewGameOakObjectTemplate, x, y, subpriority);
}

u8 CreateMonSprite_PicBox(u16 species, s16 x, s16 y, u8 subpriority)
{
    u16 spriteId = CreateMonPicSprite_HandleDeoxys(species, 0, 0x8000, TRUE, x, y, 0, gMonPaletteTable[species].tag);
    PreservePaletteInWeather(IndexOfSpritePaletteTag(gMonPaletteTable[species].tag) + 0x10);
    if (spriteId == 0xFFFF)
        return MAX_SPRITES;
    else
        return spriteId;
}

static u8 CreateMonSprite_FieldMove(u16 species, u32 otId, u32 personality, s16 x, s16 y, u8 subpriority)
{
    const struct CompressedSpritePalette * spritePalette = GetMonSpritePalStructFromOtIdPersonality(species, otId, personality);
    u16 spriteId = CreateMonPicSprite_HandleDeoxys(species, otId, personality, 1, x, y, 0, spritePalette->tag);
    // T-132: the daemon using the move is in the party; find it by who it is, and show what it understands.
    {
        s32 i;

        for (i = 0; i < PARTY_SIZE; i++)
        {
            if (GetMonData(&gPlayerParty[i], MON_DATA_PERSONALITY, NULL) == personality
             && GetMonData(&gPlayerParty[i], MON_DATA_OT_ID, NULL) == otId)
            {
                u16 moves[MAX_MON_MOVES];

                Streaks_MovesOfMon(&gPlayerParty[i], moves);
                Streaks_ApplyToLoaded(OBJ_PLTT_ID(IndexOfSpritePaletteTag(spritePalette->tag)), species, moves);
                break;
            }
        }
    }
    PreservePaletteInWeather(IndexOfSpritePaletteTag(spritePalette->tag) + 0x10);
    if (spriteId == 0xFFFF)
        return MAX_SPRITES;
    else
        return spriteId;
}

void FreeResourcesAndDestroySprite(struct Sprite *sprite, u8 spriteId)
{
    ResetPreservedPalettesInWeather();
    if (sprite->oam.affineMode != ST_OAM_AFFINE_OFF)
    {
        FreeOamMatrix(sprite->oam.matrixNum);
    }
    FreeAndDestroyMonPicSprite(spriteId);
}

// r, g, b are between 0 and 16
void MultiplyInvertedPaletteRGBComponents(u16 i, u8 r, u8 g, u8 b)
{
    int curRed;
    int curGreen;
    int curBlue;
    u16 outPal;

    outPal = gPlttBufferUnfaded[i];
    curRed = outPal & 0x1f;
    curGreen = (outPal & (0x1f << 5)) >> 5;
    curBlue = (outPal & (0x1f << 10)) >> 10;
    curRed += (((0x1f - curRed) * r) >> 4);
    curGreen += (((0x1f - curGreen) * g) >> 4);
    curBlue += (((0x1f - curBlue) * b) >> 4);
    outPal = curRed;
    outPal |= curGreen << 5;
    outPal |= curBlue << 10;
    gPlttBufferFaded[i] = outPal;
}

// r, g, b are between 0 and 16
static void MultiplyPaletteRGBComponents(u16 i, u8 r, u8 g, u8 b)
{
    int curRed;
    int curGreen;
    int curBlue;
    u16 outPal;

    outPal = gPlttBufferUnfaded[i];
    curRed = outPal & 0x1f;
    curGreen = (outPal & (0x1f << 5)) >> 5;
    curBlue = (outPal & (0x1f << 10)) >> 10;
    curRed -= ((curRed * r) >> 4);
    curGreen -= ((curGreen * g) >> 4);
    curBlue -= ((curBlue * b) >> 4);
    outPal = curRed;
    outPal |= curGreen << 5;
    outPal |= curBlue << 10;
    gPlttBufferFaded[i] = outPal;
}

static void PokecenterHealEffect_Init(struct Task *task);
static void PokecenterHealEffect_WaitForBallPlacement(struct Task *task);
static void PokecenterHealEffect_WaitForBallFlashing(struct Task *task);
static void PokecenterHealEffect_WaitForSoundAndEnd(struct Task *task);
static void HallOfFameRecordEffect_Init(struct Task *task);
static void HallOfFameRecordEffect_WaitForBallPlacement(struct Task *task);
static void HallOfFameRecordEffect_WaitForBallFlashing(struct Task *task);
static void HallOfFameRecordEffect_WaitForSoundAndEnd(struct Task *task);
static void Task_HallOfFameRecord(u8 taskId);
static u8 CreateGlowingPokeballsEffect(s16 duration, s16 x, s16 y, bool16 fanfare);
static void SpriteCB_PokeballGlowEffect(struct Sprite *sprite);
static void PokeballGlowEffect_PlaceBalls(struct Sprite *sprite);
static void PokeballGlowEffect_TryPlaySe(struct Sprite *sprite);
static void PokeballGlowEffect_FlashFirstThree(struct Sprite *sprite);
static void PokeballGlowEffect_FlashLast(struct Sprite *sprite);
static void PokeballGlowEffect_WaitAfterFlash(struct Sprite *sprite);
static void PokeballGlowEffect_Dummy(struct Sprite *sprite);
static void PokeballGlowEffect_WaitForSound(struct Sprite *sprite);
static void PokeballGlowEffect_Idle(struct Sprite *sprite);
static u8 CreatePokecenterMonitorSprite(s32 x, s32 y);
static void CreateHofMonitorSprite(s32 x, s32 y);

static void (*const sPokecenterHealEffectFuncs[])(struct Task *) =
{
    PokecenterHealEffect_Init,
    PokecenterHealEffect_WaitForBallPlacement,
    PokecenterHealEffect_WaitForBallFlashing,
    PokecenterHealEffect_WaitForSoundAndEnd
};

static void (*const sHallOfFameRecordEffectFuncs[])(struct Task *) =
{
    HallOfFameRecordEffect_Init,
    HallOfFameRecordEffect_WaitForBallPlacement,
    HallOfFameRecordEffect_WaitForBallFlashing,
    HallOfFameRecordEffect_WaitForSoundAndEnd
};

static void (*const sPokeballGlowEffectFuncs[])(struct Sprite *) =
{
    PokeballGlowEffect_PlaceBalls,
    PokeballGlowEffect_TryPlaySe,
    PokeballGlowEffect_FlashFirstThree,
    PokeballGlowEffect_FlashLast,
    PokeballGlowEffect_WaitAfterFlash,
    PokeballGlowEffect_Dummy,
    PokeballGlowEffect_WaitForSound,
    PokeballGlowEffect_Idle
};

// Task data for Task_PokecenterHeal and Task_HallOfFameRecord
#define tState              data[0]
#define tNumMons            data[1]
#define tFirstBallX         data[2]
#define tFirstBallY         data[3]
#define tMonitorX           data[4]
#define tMonitorY           data[5]
#define tGlowEffectSpriteId data[6]
#define tMonitorSpriteId    data[7]

// Sprite data for SpriteCB_PokeballGlowEffect
#define sState      data[0]
#define sTimer      data[1]
#define sCounter    data[2]
#define sNumFlashed data[3]
#define sPlayHealSe data[5]
#define sNumMons    data[6]
#define sSpriteId   data[7]

// Sprite data for SpriteCB_PokeballGlow
#define sGlowEffectSpriteId data[0]

// Sprite data for SpriteCB_PokecenterMonitor
#define sStartFlash data[0]

bool8 FldEff_PokecenterHeal(void)
{
    u8 nPokemon;
    struct Task *task;

    nPokemon = CalculatePlayerPartyCount();
    task = &gTasks[CreateTask(Task_PokecenterHeal, 0xFF)];
    task->tNumMons = nPokemon;
    task->tFirstBallX = 93;
    task->tFirstBallY = 36;
    task->tMonitorX = 128;
    task->tMonitorY = 24;
    return FALSE;
}

static void Task_PokecenterHeal(u8 taskId)
{
    struct Task *task = &gTasks[taskId];
    sPokecenterHealEffectFuncs[task->tState](task);
}

static void PokecenterHealEffect_Init(struct Task *task)
{
    task->tState++;
    task->tGlowEffectSpriteId = CreateGlowingPokeballsEffect(task->tNumMons, task->tFirstBallX, task->tFirstBallY, TRUE);
    task->tMonitorSpriteId = CreatePokecenterMonitorSprite(task->tMonitorX, task->tMonitorY);
}

static void PokecenterHealEffect_WaitForBallPlacement(struct Task *task)
{
    if (gSprites[task->tGlowEffectSpriteId].sState >= 2)
    {
        gSprites[task->tMonitorSpriteId].sStartFlash++;
        task->tState++;
    }
}

static void PokecenterHealEffect_WaitForBallFlashing(struct Task *task)
{
    if (gSprites[task->tGlowEffectSpriteId].sState > 4)
        task->tState++;
}

static void PokecenterHealEffect_WaitForSoundAndEnd(struct Task *task)
{
    if (gSprites[task->tGlowEffectSpriteId].sState > 6)
    {
        DestroySprite(&gSprites[task->tGlowEffectSpriteId]);
        FieldEffectActiveListRemove(FLDEFF_POKECENTER_HEAL);
        DestroyTask(FindTaskIdByFunc(Task_PokecenterHeal));
    }
}

bool8 FldEff_HallOfFameRecord(void)
{
    u8 nPokemon;
    struct Task *task;

    nPokemon = CalculatePlayerPartyCount();
    task = &gTasks[CreateTask(Task_HallOfFameRecord, 0xFF)];
    task->tNumMons = nPokemon;
    task->tFirstBallX = 117;
    task->tFirstBallY = 60;
    return FALSE;
}

static void Task_HallOfFameRecord(u8 taskId)
{
    struct Task *task;
    task = &gTasks[taskId];
    sHallOfFameRecordEffectFuncs[task->tState](task);
}

static void HallOfFameRecordEffect_Init(struct Task *task)
{
    u8 taskId;
    task->tState++;
    task->tGlowEffectSpriteId = CreateGlowingPokeballsEffect(task->tNumMons, task->tFirstBallX, task->tFirstBallY, FALSE);
}

static void HallOfFameRecordEffect_WaitForBallPlacement(struct Task *task)
{
    if (gSprites[task->tGlowEffectSpriteId].sState > 1)
    {
        CreateHofMonitorSprite(120, 25);
        task->data[15]++; // unused, leftover from RSE
        task->tState++;
    }
}

static void HallOfFameRecordEffect_WaitForBallFlashing(struct Task *task)
{
    if (gSprites[task->tGlowEffectSpriteId].sState > 4)
        task->tState++;
}

static void HallOfFameRecordEffect_WaitForSoundAndEnd(struct Task *task)
{
    if (gSprites[task->tGlowEffectSpriteId].sState > 6)
    {
        DestroySprite(&gSprites[task->tGlowEffectSpriteId]);
        FieldEffectActiveListRemove(FLDEFF_HALL_OF_FAME_RECORD);
        DestroyTask(FindTaskIdByFunc(Task_HallOfFameRecord));
    }
}

#undef tState
#undef tNumMons
#undef tFirstBallX
#undef tFirstBallY
#undef tMonitorX
#undef tMonitorY
#undef tGlowEffectSpriteId
#undef tMonitorSpriteId

static u8 CreateGlowingPokeballsEffect(s16 numMons, s16 x, s16 y, bool16 playHealSe)
{
    u8 spriteId;
    struct Sprite *sprite;
    spriteId = CreateInvisibleSprite(SpriteCB_PokeballGlowEffect);
    sprite = &gSprites[spriteId];
    sprite->x2 = x;
    sprite->y2 = y;
    sprite->subpriority = 0xFF;
    sprite->sPlayHealSe = playHealSe;
    sprite->sNumMons = numMons;
    sprite->sSpriteId = spriteId;
    return spriteId;
}

static void SpriteCB_PokeballGlowEffect(struct Sprite *sprite)
{
    sPokeballGlowEffectFuncs[sprite->sState](sprite);
}

static const struct Coords16 sPokeballCoordOffsets[] = {
    {0, 0},
    {6, 0},
    {0, 4},
    {6, 4},
    {0, 8},
    {6, 8}
};

static const u8 sPokeballGlowReds[]   = {16, 12,  8,  0};
static const u8 sPokeballGlowGreens[] = {16, 12,  8,  0};
static const u8 sPokeballGlowBlues[]  = { 0,  0,  0,  0};

static void PokeballGlowEffect_PlaceBalls(struct Sprite *sprite)
{
    u8 spriteId;
    if (sprite->sTimer == 0 || (--sprite->sTimer) == 0)
    {
        sprite->sTimer = 25;
        spriteId = CreateSpriteAtEnd(&sSpriteTemplate_PokeballGlow, sPokeballCoordOffsets[sprite->sCounter].x + sprite->x2, sPokeballCoordOffsets[sprite->sCounter].y + sprite->y2, 0xFF);
        gSprites[spriteId].oam.priority = 2;
        gSprites[spriteId].sGlowEffectSpriteId = sprite->sSpriteId;
        sprite->sCounter++;
        sprite->sNumMons--;
        PlaySE(SE_BALL);
    }
    if (sprite->sNumMons == 0)
    {
        sprite->sTimer = 32;
        sprite->sState++;
    }
}

static void PokeballGlowEffect_TryPlaySe(struct Sprite *sprite)
{
    if ((--sprite->sTimer) == 0)
    {
        sprite->sState++;
        sprite->sTimer = 8;
        sprite->sCounter = 0;
        sprite->sNumFlashed = 0;
        if (sprite->sPlayHealSe)
            PlayFanfare(MUS_HEAL);
    }
}

static void PokeballGlowEffect_FlashFirstThree(struct Sprite *sprite)
{
    u8 phase;
    if ((--sprite->sTimer) == 0)
    {
        sprite->sTimer = 8;
        sprite->sCounter++;
        sprite->sCounter &= 3;
        if (sprite->sCounter == 0)
            sprite->sNumFlashed++;
    }
    phase = (sprite->sCounter + 3) & 3;
    MultiplyInvertedPaletteRGBComponents(OBJ_PLTT_ID(IndexOfSpritePaletteTag(0x1007)) + 8, sPokeballGlowReds[phase], sPokeballGlowGreens[phase], sPokeballGlowBlues[phase]);
    phase = (sprite->sCounter + 2) & 3;
    MultiplyInvertedPaletteRGBComponents(OBJ_PLTT_ID(IndexOfSpritePaletteTag(0x1007)) + 6, sPokeballGlowReds[phase], sPokeballGlowGreens[phase], sPokeballGlowBlues[phase]);
    phase = (sprite->sCounter + 1) & 3;
    MultiplyInvertedPaletteRGBComponents(OBJ_PLTT_ID(IndexOfSpritePaletteTag(0x1007)) + 2, sPokeballGlowReds[phase], sPokeballGlowGreens[phase], sPokeballGlowBlues[phase]);
    phase = sprite->sCounter;
    MultiplyInvertedPaletteRGBComponents(OBJ_PLTT_ID(IndexOfSpritePaletteTag(0x1007)) + 5, sPokeballGlowReds[phase], sPokeballGlowGreens[phase], sPokeballGlowBlues[phase]);
    MultiplyInvertedPaletteRGBComponents(OBJ_PLTT_ID(IndexOfSpritePaletteTag(0x1007)) + 3, sPokeballGlowReds[phase], sPokeballGlowGreens[phase], sPokeballGlowBlues[phase]);
    if (sprite->sNumFlashed >= 3)
    {
        sprite->sState++;
        sprite->sTimer = 8;
        sprite->sCounter = 0;
    }
}

static void PokeballGlowEffect_FlashLast(struct Sprite *sprite)
{
    u8 phase;
    if ((--sprite->sTimer) == 0)
    {
        sprite->sTimer = 8;
        sprite->sCounter++;
        sprite->sCounter &= 3;
        if (sprite->sCounter == 3)
        {
            sprite->sState++;
            sprite->sTimer = 30;
        }
    }
    phase = sprite->sCounter;
    MultiplyInvertedPaletteRGBComponents(OBJ_PLTT_ID(IndexOfSpritePaletteTag(0x1007)) + 8, sPokeballGlowReds[phase], sPokeballGlowGreens[phase], sPokeballGlowBlues[phase]);
    MultiplyInvertedPaletteRGBComponents(OBJ_PLTT_ID(IndexOfSpritePaletteTag(0x1007)) + 6, sPokeballGlowReds[phase], sPokeballGlowGreens[phase], sPokeballGlowBlues[phase]);
    MultiplyInvertedPaletteRGBComponents(OBJ_PLTT_ID(IndexOfSpritePaletteTag(0x1007)) + 2, sPokeballGlowReds[phase], sPokeballGlowGreens[phase], sPokeballGlowBlues[phase]);
    MultiplyInvertedPaletteRGBComponents(OBJ_PLTT_ID(IndexOfSpritePaletteTag(0x1007)) + 5, sPokeballGlowReds[phase], sPokeballGlowGreens[phase], sPokeballGlowBlues[phase]);
    MultiplyInvertedPaletteRGBComponents(OBJ_PLTT_ID(IndexOfSpritePaletteTag(0x1007)) + 3, sPokeballGlowReds[phase], sPokeballGlowGreens[phase], sPokeballGlowBlues[phase]);
}

static void PokeballGlowEffect_WaitAfterFlash(struct Sprite *sprite)
{
    if ((--sprite->sTimer) == 0)
        sprite->sState++;
}

static void PokeballGlowEffect_Dummy(struct Sprite *sprite)
{
    sprite->sState++;
}

static void PokeballGlowEffect_WaitForSound(struct Sprite *sprite)
{
    if (sprite->sPlayHealSe == FALSE || IsFanfareTaskInactive())
        sprite->sState++;
}

static void PokeballGlowEffect_Idle(struct Sprite *sprite)
{
}

static void SpriteCB_PokeballGlow(struct Sprite *sprite)
{
    if (gSprites[sprite->sGlowEffectSpriteId].sState > 4)
        FieldEffectFreeGraphicsResources(sprite);
}

#undef sState
#undef sTimer
#undef sCounter
#undef sNumFlashed
#undef sPlayHealSe
#undef sNumMons
#undef sSpriteId

#undef sGlowEffectSpriteId

static u8 CreatePokecenterMonitorSprite(s32 x, s32 y)
{
    u8 spriteId;
    struct Sprite *sprite;
    spriteId = CreateSpriteAtEnd(&sSpriteTemplate_PokecenterMonitor, x, y, 0);
    sprite = &gSprites[spriteId];
    sprite->oam.priority = 2;
    sprite->invisible = TRUE;
    return spriteId;
}

static void SpriteCB_PokecenterMonitor(struct Sprite *sprite)
{
    if (sprite->sStartFlash != FALSE)
    {
        sprite->sStartFlash = FALSE;
        sprite->invisible = FALSE;
        StartSpriteAnim(sprite, 1);
    }
    if (sprite->animEnded)
        FieldEffectFreeGraphicsResources(sprite);
}

#undef sStartFlash

static void CreateHofMonitorSprite(s32 x, s32 y)
{
    CreateSpriteAtEnd(&sSpriteTemplate_HofMonitor, x, y, 0);
}

static void SpriteCB_HallOfFameMonitor(struct Sprite *sprite)
{
    if (sprite->animEnded)
        FieldEffectFreeGraphicsResources(sprite);
}

static void FieldCallback_UseFly(void);
static void Task_UseFly(u8 taskId);
static void FieldCallback_FlyIntoMap(void);
static void Task_FlyIntoMap(u8 taskId);

void ReturnToFieldFromFlyMapSelect(void)
{
    SetMainCallback2(CB2_ReturnToField);
    gFieldCallback = FieldCallback_UseFly;
}

static void FieldCallback_UseFly(void)
{
    FadeInFromBlack();
    CreateTask(Task_UseFly, 0);
    LockPlayerFieldControls();
    FreezeObjectEvents();
    gFieldCallback = NULL;
}

static void Task_UseFly(u8 taskId)
{
    struct Task *task;
    task = &gTasks[taskId];
    if (task->data[0] == 0)
    {
        if (!IsWeatherNotFadingIn())
            return;
        gFieldEffectArguments[0] = GetCursorSelectionMonId();
        if ((int)gFieldEffectArguments[0] >= PARTY_SIZE)
            gFieldEffectArguments[0] = 0;
        FieldEffectStart(FLDEFF_FLY_OUT);
        task->data[0]++;
    }
    if (!FieldEffectActiveListContains(FLDEFF_FLY_OUT))
    {
        Overworld_ResetStateAfterFly();
        WarpIntoMap();
        SetMainCallback2(CB2_LoadMap);
        gFieldCallback = FieldCallback_FlyIntoMap;
        DestroyTask(taskId);
    }
}

static void FieldCallback_FlyIntoMap(void)
{
    Overworld_PlaySpecialMapMusic();
    FadeInFromBlack();
    CreateTask(Task_FlyIntoMap, 0);
    gObjectEvents[gPlayerAvatar.objectEventId].invisible = TRUE;
    if (gPlayerAvatar.flags & PLAYER_AVATAR_FLAG_SURFING)
        ObjectEventTurn(&gObjectEvents[gPlayerAvatar.objectEventId], DIR_WEST);
    LockPlayerFieldControls();
    FreezeObjectEvents();
    gFieldCallback = NULL;
}

static void Task_FlyIntoMap(u8 taskId)
{
    struct Task *task;
    task = &gTasks[taskId];
    if (task->data[0] == 0)
    {
        if (gPaletteFade.active)
            return;
        FieldEffectStart(FLDEFF_FLY_IN);
        task->data[0]++;
    }
    if (!FieldEffectActiveListContains(FLDEFF_FLY_IN))
    {
        UnlockPlayerFieldControls();
        UnfreezeObjectEvents();
        DestroyTask(taskId);
    }
}

static void Task_FallWarpFieldEffect(u8 taskId);
static bool8 FallWarpEffect_1(struct Task *task);
static bool8 FallWarpEffect_2(struct Task *task);
static bool8 FallWarpEffect_3(struct Task *task);
static bool8 FallWarpEffect_4(struct Task *task);
static bool8 FallWarpEffect_5(struct Task *task);
static bool8 FallWarpEffect_6(struct Task *task);
static bool8 FallWarpEffect_7(struct Task *task);

static bool8 (*const sFallWarpEffectCBPtrs[])(struct Task *task) = {
    FallWarpEffect_1,
    FallWarpEffect_2,
    FallWarpEffect_3,
    FallWarpEffect_4,
    FallWarpEffect_5,
    FallWarpEffect_6,
    FallWarpEffect_7
};

void FieldCB_FallWarpExit(void)
{
    Overworld_PlaySpecialMapMusic();
    WarpFadeInScreen();
    QuestLog_DrawPreviouslyOnQuestHeaderIfInPlaybackMode();
    LockPlayerFieldControls();
    FreezeObjectEvents();
    CreateTask(Task_FallWarpFieldEffect, 0);
    gFieldCallback = NULL;
}

static void Task_FallWarpFieldEffect(u8 taskId)
{
    struct Task *task = &gTasks[taskId];
    while (sFallWarpEffectCBPtrs[task->data[0]](task))
        ;
}

static bool8 FallWarpEffect_1(struct Task *task)
{
    struct ObjectEvent * playerObject;
    struct Sprite *playerSprite;
    playerObject = &gObjectEvents[gPlayerAvatar.objectEventId];
    playerSprite = &gSprites[gPlayerAvatar.spriteId];
    CameraObjectReset2();
    gObjectEvents[gPlayerAvatar.objectEventId].invisible = TRUE;
    gPlayerAvatar.preventStep = TRUE;
    ObjectEventSetHeldMovement(playerObject, GetFaceDirectionMovementAction(GetPlayerFacingDirection()));
    task->data[4] = playerSprite->subspriteMode;
    playerObject->fixedPriority = TRUE;
    playerSprite->oam.priority = 1;
    playerSprite->subspriteMode = SUBSPRITES_IGNORE_PRIORITY;
    task->data[0]++;
    return TRUE;
}

static bool8 FallWarpEffect_2(struct Task *task)
{
    if (IsWeatherNotFadingIn())
    {
        task->data[0]++;
    }
    return FALSE;
}

static bool8 FallWarpEffect_3(struct Task *task)
{
    struct Sprite *sprite;
    s16 centerToCornerVecY;
    sprite = &gSprites[gPlayerAvatar.spriteId];
    centerToCornerVecY = -(sprite->centerToCornerVecY << 1);
    sprite->y2 = -(sprite->y + sprite->centerToCornerVecY + gSpriteCoordOffsetY + centerToCornerVecY);
    task->data[1] = 1;
    task->data[2] = 0;
    gObjectEvents[gPlayerAvatar.objectEventId].invisible = FALSE;
    PlaySE(SE_FALL);
    task->data[0]++;
    return FALSE;
}

static bool8 FallWarpEffect_4(struct Task *task)
{
    struct ObjectEvent * objectEvent;
    struct Sprite *sprite;

    objectEvent = &gObjectEvents[gPlayerAvatar.objectEventId];
    sprite = &gSprites[gPlayerAvatar.spriteId];
    sprite->y2 += task->data[1];
    if (task->data[1] < 8)
    {
        task->data[2] += task->data[1];
        if (task->data[2] & 0xf)
        {
            task->data[1] <<= 1;
        }
    }
    if (task->data[3] == 0 && sprite->y2 >= -16)
    {
        task->data[3]++;
        objectEvent->fixedPriority = FALSE;
        sprite->subspriteMode = task->data[4];
        objectEvent->triggerGroundEffectsOnMove = TRUE;
    }
    if (sprite->y2 >= 0)
    {
        PlaySE(SE_M_STRENGTH);
        objectEvent->triggerGroundEffectsOnStop = TRUE;
        objectEvent->landingJump = TRUE;
        sprite->y2 = 0;
        task->data[0]++;
    }
    return FALSE;
}

static bool8 FallWarpEffect_5(struct Task *task)
{
    task->data[0]++;
    task->data[1] = 4;
    task->data[2] = 0;
    SetCameraPanningCallback(NULL);
    return TRUE;
}

static bool8 FallWarpEffect_6(struct Task *task)
{
    SetCameraPanning(0, task->data[1]);
    task->data[1] = -task->data[1];
    task->data[2]++;
    if ((task->data[2] & 3) == 0)
    {
        task->data[1] >>= 1;
    }
    if (task->data[1] == 0)
    {
        task->data[0]++;
    }
    return FALSE;
}

static bool8 FallWarpEffect_7(struct Task *task)
{
    s16 x, y;
    gPlayerAvatar.preventStep = FALSE;
    UnlockPlayerFieldControls();
    CameraObjectReset1();
    UnfreezeObjectEvents();
    InstallCameraPanAheadCallback();
    PlayerGetDestCoords(&x, &y);
    if (MetatileBehavior_IsSurfableInSeafoamIslands(MapGridGetMetatileBehaviorAt(x, y)) == TRUE)
    {
        VarSet(VAR_TEMP_1, 1);
        SetPlayerAvatarTransitionFlags(PLAYER_AVATAR_FLAG_SURFING);
        SetHelpContext(HELPCONTEXT_SURFING);
    }
    DestroyTask(FindTaskIdByFunc(Task_FallWarpFieldEffect));
    return FALSE;
}

static void Task_EscalatorWarpFieldEffect(u8 taskId);
static bool8 EscalatorWarpEffect_1(struct Task *task);
static bool8 EscalatorWarpEffect_2(struct Task *task);
static bool8 EscalatorWarpEffect_3(struct Task *task);
static bool8 EscalatorWarpEffect_4(struct Task *task);
static bool8 EscalatorWarpEffect_5(struct Task *task);
static bool8 EscalatorWarpEffect_6(struct Task *task);
static void Escalator_AnimatePlayerGoingDown(struct Task *task);
static void Escalator_AnimatePlayerGoingUp(struct Task *task);
static void Escalator_BeginFadeOutToNewMap(void);
static void Escalator_TransitionToWarpInEffect(void);
static void FieldCB_EscalatorWarpIn(void);
static void Task_EscalatorWarpInFieldEffect(u8 taskId);
static bool8 EscalatorWarpInEffect_1(struct Task *task);
static bool8 EscalatorWarpInEffect_2(struct Task *task);
static bool8 EscalatorWarpInEffect_3(struct Task *task);
static bool8 EscalatorWarpInEffect_4(struct Task *task);
static bool8 EscalatorWarpInEffect_5(struct Task *task);
static bool8 EscalatorWarpInEffect_6(struct Task *task);
static bool8 EscalatorWarpInEffect_7(struct Task *task);

static bool8 (*const sEscalatorWarpFieldEffectFuncs[])(struct Task *task) = {
    EscalatorWarpEffect_1,
    EscalatorWarpEffect_2,
    EscalatorWarpEffect_3,
    EscalatorWarpEffect_4,
    EscalatorWarpEffect_5,
    EscalatorWarpEffect_6
};

void StartEscalatorWarp(u8 metatileBehavior, u8 priority)
{
    u8 taskId = CreateTask(Task_EscalatorWarpFieldEffect, priority);
    gTasks[taskId].data[1] = 0;
    if (metatileBehavior == MB_UP_ESCALATOR)
        gTasks[taskId].data[1] = 1;
}

static void Task_EscalatorWarpFieldEffect(u8 taskId)
{
    struct Task *task = &gTasks[taskId];
    while (sEscalatorWarpFieldEffectFuncs[task->data[0]](task))
        ;
}

static bool8 EscalatorWarpEffect_1(struct Task *task)
{
    FreezeObjectEvents();
    CameraObjectReset2();
    StartEscalator(task->data[1]);
    QuestLog_OnEscalatorWarp(QL_ESCALATOR_OUT);
    task->data[0]++;
    return FALSE;
}

static bool8 EscalatorWarpEffect_2(struct Task *task)
{
    struct ObjectEvent * objectEvent;
    objectEvent = &gObjectEvents[gPlayerAvatar.objectEventId];
    if (!ObjectEventIsMovementOverridden(objectEvent) || ObjectEventClearHeldMovementIfFinished(objectEvent))
    {
        ObjectEventSetHeldMovement(objectEvent, GetFaceDirectionMovementAction(GetPlayerFacingDirection()));
        task->data[0]++;
        task->data[2] = 0;
        task->data[3] = 0;
        if ((u8)task->data[1] == 0)
        {
            task->data[0] = 4;
        }
        PlaySE(SE_ESCALATOR);
    }
    return FALSE;
}

static bool8 EscalatorWarpEffect_3(struct Task *task)
{
    Escalator_AnimatePlayerGoingDown(task);
    if (task->data[2] > 3)
    {
        Escalator_BeginFadeOutToNewMap();
        task->data[0]++;
    }
    return FALSE;
}

static bool8 EscalatorWarpEffect_4(struct Task *task)
{
    Escalator_AnimatePlayerGoingDown(task);
    Escalator_TransitionToWarpInEffect();
    return FALSE;
}

static bool8 EscalatorWarpEffect_5(struct Task *task)
{
    Escalator_AnimatePlayerGoingUp(task);
    if (task->data[2] > 3)
    {
        Escalator_BeginFadeOutToNewMap();
        task->data[0]++;
    }
    return FALSE;
}

static bool8 EscalatorWarpEffect_6(struct Task *task)
{
    Escalator_AnimatePlayerGoingUp(task);
    Escalator_TransitionToWarpInEffect();
    return FALSE;
}


static void Escalator_AnimatePlayerGoingDown(struct Task *task)
{
    struct Sprite *sprite;
    sprite = &gSprites[gPlayerAvatar.spriteId];
    sprite->x2 = Cos(0x84, task->data[2]);
    sprite->y2 = Sin(0x94, task->data[2]);
    task->data[3]++;
    if (task->data[3] & 1)
    {
        task->data[2]++;
    }
}

static void Escalator_AnimatePlayerGoingUp(struct Task *task)
{
    struct Sprite *sprite;
    sprite = &gSprites[gPlayerAvatar.spriteId];
    sprite->x2 = Cos(0x7c, task->data[2]);
    sprite->y2 = Sin(0x76, task->data[2]);
    task->data[3]++;
    if (task->data[3] & 1)
    {
        task->data[2]++;
    }
}

static void Escalator_BeginFadeOutToNewMap(void)
{
    TryFadeOutOldMapMusic();
    WarpFadeOutScreen();
}

static void Escalator_TransitionToWarpInEffect(void)
{
    if (!gPaletteFade.active && BGMusicStopped() == TRUE)
    {
        StopEscalator();
        WarpIntoMap();
        gFieldCallback = FieldCB_EscalatorWarpIn;
        SetMainCallback2(CB2_LoadMap);
        DestroyTask(FindTaskIdByFunc(Task_EscalatorWarpFieldEffect));
    }
}

static bool8 (*const sEscalatorWarpInFieldEffectFuncs[])(struct Task *task) = {
    EscalatorWarpInEffect_1,
    EscalatorWarpInEffect_2,
    EscalatorWarpInEffect_3,
    EscalatorWarpInEffect_4,
    EscalatorWarpInEffect_5,
    EscalatorWarpInEffect_6,
    EscalatorWarpInEffect_7
};

static void FieldCB_EscalatorWarpIn(void)
{
    Overworld_PlaySpecialMapMusic();
    WarpFadeInScreen();
    QuestLog_DrawPreviouslyOnQuestHeaderIfInPlaybackMode();
    LockPlayerFieldControls();
    FreezeObjectEvents();
    CreateTask(Task_EscalatorWarpInFieldEffect, 0);
    gFieldCallback = NULL;
}

static void Task_EscalatorWarpInFieldEffect(u8 taskId)
{
    struct Task *task = &gTasks[taskId];
    while (sEscalatorWarpInFieldEffectFuncs[task->data[0]](task))
        ;
}

static bool8 EscalatorWarpInEffect_1(struct Task *task)
{
    struct ObjectEvent * objectEvent;
    s16 x;
    s16 y;
    u8 behavior;
    CameraObjectReset2();
    objectEvent = &gObjectEvents[gPlayerAvatar.objectEventId];
    ObjectEventSetHeldMovement(objectEvent, GetFaceDirectionMovementAction(DIR_EAST));
    PlayerGetDestCoords(&x, &y);
    behavior = MapGridGetMetatileBehaviorAt(x, y);
    task->data[0]++;
    task->data[1] = 16;
    if (behavior == MB_DOWN_ESCALATOR)
    {
        behavior = 1;
        task->data[0] = 3;
    } else
    {
        behavior = 0;
    }
    StartEscalator(behavior);
    return TRUE;
}

static bool8 EscalatorWarpInEffect_2(struct Task *task)
{
    struct Sprite *sprite;
    sprite = &gSprites[gPlayerAvatar.spriteId];
    sprite->x2 = Cos(0x84, task->data[1]);
    sprite->y2 = Sin(0x94, task->data[1]);
    task->data[0]++;
    return FALSE;
}

static bool8 EscalatorWarpInEffect_3(struct Task *task)
{
    struct Sprite *sprite;
    sprite = &gSprites[gPlayerAvatar.spriteId];
    sprite->x2 = Cos(0x84, task->data[1]);
    sprite->y2 = Sin(0x94, task->data[1]);
    task->data[2]++;
    if (task->data[2] & 1)
    {
        task->data[1]--;
    }
    if (task->data[1] == 0)
    {
        sprite->x2 = 0;
        sprite->y2 = 0;
        task->data[0] = 5;
    }
    return FALSE;
}


static bool8 EscalatorWarpInEffect_4(struct Task *task)
{
    struct Sprite *sprite;
    sprite = &gSprites[gPlayerAvatar.spriteId];
    sprite->x2 = Cos(0x7c, task->data[1]);
    sprite->y2 = Sin(0x76, task->data[1]);
    task->data[0]++;
    return FALSE;
}

static bool8 EscalatorWarpInEffect_5(struct Task *task)
{
    struct Sprite *sprite;
    sprite = &gSprites[gPlayerAvatar.spriteId];
    sprite->x2 = Cos(0x7c, task->data[1]);
    sprite->y2 = Sin(0x76, task->data[1]);
    task->data[2]++;
    if (task->data[2] & 1)
    {
        task->data[1]--;
    }
    if (task->data[1] == 0)
    {
        sprite->x2 = 0;
        sprite->y2 = 0;
        task->data[0]++;
    }
    return FALSE;
}

static bool8 EscalatorWarpInEffect_6(struct Task *task)
{
    if (IsEscalatorMoving())
    {
        return FALSE;
    }
    StopEscalator();
    task->data[0]++;
    return TRUE;
}

static bool8 EscalatorWarpInEffect_7(struct Task *task)
{
    struct ObjectEvent * objectEvent;
    objectEvent = &gObjectEvents[gPlayerAvatar.objectEventId];
    if (ObjectEventClearHeldMovementIfFinished(objectEvent))
    {
        CameraObjectReset1();
        UnlockPlayerFieldControls();
        UnfreezeObjectEvents();
        ObjectEventSetHeldMovement(objectEvent, GetWalkNormalMovementAction(DIR_EAST));
        DestroyTask(FindTaskIdByFunc(Task_EscalatorWarpInFieldEffect));
        QuestLog_OnEscalatorWarp(QL_ESCALATOR_IN);
    }
    return FALSE;
}

static void Task_UseWaterfall(u8 taskId);

static bool8 waterfall_0_setup(struct Task *task, struct ObjectEvent * playerObj);
static bool8 waterfall_1_do_anim_probably(struct Task *task, struct ObjectEvent * playerObj);
static bool8 waterfall_2_wait_anim_finish_probably(struct Task *task, struct ObjectEvent * playerObj);
static bool8 waterfall_3_move_player_probably(struct Task *task, struct ObjectEvent * playerObj);
static bool8 waterfall_4_wait_player_move_probably(struct Task *task, struct ObjectEvent * playerObj);

static bool8 (*const sUseWaterfallFieldEffectFuncs[])(struct Task *task, struct ObjectEvent * playerObj) = {
    waterfall_0_setup,
    waterfall_1_do_anim_probably,
    waterfall_2_wait_anim_finish_probably,
    waterfall_3_move_player_probably,
    waterfall_4_wait_player_move_probably
};

u32 FldEff_UseWaterfall(void)
{
    u8 taskId = CreateTask(Task_UseWaterfall, 0xFF);
    gTasks[taskId].data[1] = gFieldEffectArguments[0];
    Task_UseWaterfall(taskId);
    return 0;
}

static void Task_UseWaterfall(u8 taskId)
{
    while (sUseWaterfallFieldEffectFuncs[gTasks[taskId].data[0]](&gTasks[taskId], &gObjectEvents[gPlayerAvatar.objectEventId]))
        ;
}

static bool8 waterfall_0_setup(struct Task *task, struct ObjectEvent * playerObj)
{
    LockPlayerFieldControls();
    gPlayerAvatar.preventStep = TRUE;
    task->data[0]++;
    return FALSE;
}

static bool8 waterfall_1_do_anim_probably(struct Task *task, struct ObjectEvent * playerObj)
{
    LockPlayerFieldControls();
    if (!ObjectEventIsMovementOverridden(playerObj))
    {
        ObjectEventClearHeldMovementIfFinished(playerObj);
        gFieldEffectArguments[0] = task->data[1];
        FieldEffectStart(FLDEFF_FIELD_MOVE_SHOW_MON_INIT);
        task->data[0]++;
    }
    return FALSE;
}

static bool8 waterfall_2_wait_anim_finish_probably(struct Task *task, struct ObjectEvent * playerObj)
{
    if (FieldEffectActiveListContains(FLDEFF_FIELD_MOVE_SHOW_MON))
        return FALSE;
    task->data[0]++;
    return TRUE;
}

static bool8 waterfall_3_move_player_probably(struct Task *task, struct ObjectEvent * playerObj)
{
    ObjectEventSetHeldMovement(playerObj, GetWalkSlowerMovementAction(DIR_NORTH));
    task->data[0]++;
    return FALSE;
}

static bool8 waterfall_4_wait_player_move_probably(struct Task *task, struct ObjectEvent * playerObj)
{
    if (!ObjectEventClearHeldMovementIfFinished(playerObj))
        return FALSE;
    if (MetatileBehavior_IsWaterfall(playerObj->currentMetatileBehavior))
    {
        task->data[0] = 3;
        return TRUE;
    }
    UnlockPlayerFieldControls();
    gPlayerAvatar.preventStep = FALSE;
    DestroyTask(FindTaskIdByFunc(Task_UseWaterfall));
    FieldEffectActiveListRemove(FLDEFF_USE_WATERFALL);
    return FALSE;
}

static void Task_UseDive(u8 taskId);
static bool8 DiveFieldEffect_Init(struct Task *task);
static bool8 DiveFieldEffect_ShowMon(struct Task *task);
static bool8 DiveFieldEffect_TryWarp(struct Task *task);

static bool8 (*const sDiveFieldEffectFuncs[])(struct Task *task) = 
{
    DiveFieldEffect_Init,
    DiveFieldEffect_ShowMon,
    DiveFieldEffect_TryWarp
};

u32 FldEff_UseDive(void)
{
    u8 taskId = CreateTask(Task_UseDive, 0xFF);
    gTasks[taskId].data[15] = gFieldEffectArguments[0]; // party index of pokemon with dive
    gTasks[taskId].data[14] = gFieldEffectArguments[1]; // unused
    Task_UseDive(taskId);
    return 0;
}

static void Task_UseDive(u8 taskId)
{
    while (sDiveFieldEffectFuncs[gTasks[taskId].data[0]](&gTasks[taskId]));
}

static bool8 DiveFieldEffect_Init(struct Task *task)
{
    gPlayerAvatar.preventStep = TRUE;
    task->data[0]++;
    return FALSE;
}

static bool8 DiveFieldEffect_ShowMon(struct Task *task)
{
    LockPlayerFieldControls();
    gFieldEffectArguments[0] = task->data[15];
    FieldEffectStart(FLDEFF_FIELD_MOVE_SHOW_MON_INIT);
    task->data[0]++;
    return FALSE;
}

static bool8 DiveFieldEffect_TryWarp(struct Task *task)
{
    struct MapPosition pos;
    PlayerGetDestCoords(&pos.x, &pos.y);
    if (!FieldEffectActiveListContains(FLDEFF_FIELD_MOVE_SHOW_MON))
    {
        dive_warp(&pos, gObjectEvents[gPlayerAvatar.objectEventId].currentMetatileBehavior);
        DestroyTask(FindTaskIdByFunc(Task_UseDive));
        FieldEffectActiveListRemove(FLDEFF_USE_DIVE);
    }
    return FALSE;
}

static void Task_LavaridgeGymB1FWarp(u8 taskId);
static bool8 LavaridgeGymB1FWarpEffect_1(struct Task *task, struct ObjectEvent * objectEvent, struct Sprite *sprite);
static bool8 LavaridgeGymB1FWarpEffect_2(struct Task *task, struct ObjectEvent * objectEvent, struct Sprite *sprite);
static bool8 LavaridgeGymB1FWarpEffect_3(struct Task *task, struct ObjectEvent * objectEvent, struct Sprite *sprite);
static bool8 LavaridgeGymB1FWarpEffect_4(struct Task *task, struct ObjectEvent * objectEvent, struct Sprite *sprite);
static bool8 LavaridgeGymB1FWarpEffect_5(struct Task *task, struct ObjectEvent * objectEvent, struct Sprite *sprite);
static bool8 LavaridgeGymB1FWarpEffect_6(struct Task *task, struct ObjectEvent * objectEvent, struct Sprite *sprite);
static void FieldCB_LavaridgeGymB1FWarpExit(void);
static void Task_LavaridgeGymB1FWarpExit(u8 taskId);
static bool8 LavaridgeGymB1FWarpExitEffect_1(struct Task *task, struct ObjectEvent * objectEvent, struct Sprite *sprite);
static bool8 LavaridgeGymB1FWarpExitEffect_2(struct Task *task, struct ObjectEvent * objectEvent, struct Sprite *sprite);
static bool8 LavaridgeGymB1FWarpExitEffect_3(struct Task *task, struct ObjectEvent * objectEvent, struct Sprite *sprite);
static bool8 LavaridgeGymB1FWarpExitEffect_4(struct Task *task, struct ObjectEvent * objectEvent, struct Sprite *sprite);

static bool8 (*const sLavaridgeGymB1FWarpEffectFuncs[])(struct Task *task, struct ObjectEvent * objectEvent, struct Sprite *sprite) = {
    LavaridgeGymB1FWarpEffect_1,
    LavaridgeGymB1FWarpEffect_2,
    LavaridgeGymB1FWarpEffect_3,
    LavaridgeGymB1FWarpEffect_4,
    LavaridgeGymB1FWarpEffect_5,
    LavaridgeGymB1FWarpEffect_6
};

void StartLavaridgeGymB1FWarp(u8 priority)
{
    CreateTask(Task_LavaridgeGymB1FWarp, priority);
}

static void Task_LavaridgeGymB1FWarp(u8 taskId)
{
    while (sLavaridgeGymB1FWarpEffectFuncs[gTasks[taskId].data[0]](&gTasks[taskId], &gObjectEvents[gPlayerAvatar.objectEventId], &gSprites[gPlayerAvatar.spriteId]));
}

static bool8 LavaridgeGymB1FWarpEffect_1(struct Task *task, struct ObjectEvent * objectEvent, struct Sprite *sprite)
{
    FreezeObjectEvents();
    CameraObjectReset2();
    SetCameraPanningCallback(NULL);
    gPlayerAvatar.preventStep = TRUE;
    objectEvent->fixedPriority = TRUE;
    task->data[1] = 1;
    task->data[0]++;
    return TRUE;
}

static bool8 LavaridgeGymB1FWarpEffect_2(struct Task *task, struct ObjectEvent * objectEvent, struct Sprite *sprite)
{
    SetCameraPanning(0, task->data[1]);
    task->data[1] = -task->data[1];
    task->data[2]++;
    if (task->data[2] > 7)
    {
        task->data[2] = 0;
        task->data[0]++;
    }
    return FALSE;
}

static bool8 LavaridgeGymB1FWarpEffect_3(struct Task *task, struct ObjectEvent * objectEvent, struct Sprite *sprite)
{
    sprite->y2 = 0;
    task->data[3] = 1;
    gFieldEffectArguments[0] = objectEvent->currentCoords.x;
    gFieldEffectArguments[1] = objectEvent->currentCoords.y;
    gFieldEffectArguments[2] = sprite->subpriority - 1;
    gFieldEffectArguments[3] = sprite->oam.priority;
    FieldEffectStart(FLDEFF_LAVARIDGE_GYM_WARP);
    PlaySE(SE_M_EXPLOSION);
    task->data[0]++;
    return TRUE;
}

static bool8 LavaridgeGymB1FWarpEffect_4(struct Task *task, struct ObjectEvent * objectEvent, struct Sprite *sprite)
{
    s16 centerToCornerVecY;
    SetCameraPanning(0, task->data[1]);
    if (task->data[1] = -task->data[1], ++task->data[2] <= 17)
    {
        if (!(task->data[2] & 1) && (task->data[1] <= 3))
        {
            task->data[1] <<= 1;
        }
    } else if (!(task->data[2] & 4) && (task->data[1] > 0))
    {
        task->data[1] >>= 1;
    }
    if (task->data[2] > 6)
    {
        centerToCornerVecY = -(sprite->centerToCornerVecY << 1);
        if (sprite->y2 > -(sprite->y + sprite->centerToCornerVecY + gSpriteCoordOffsetY + centerToCornerVecY))
        {
            sprite->y2 -= task->data[3];
            if (task->data[3] <= 7)
            {
                task->data[3]++;
            }
        } else
        {
            task->data[4] = 1;
        }
    }
    if (task->data[5] == 0 && sprite->y2 < -0x10)
    {
        task->data[5]++;
        objectEvent->fixedPriority = TRUE;
        sprite->oam.priority = 1;
        sprite->subspriteMode = SUBSPRITES_IGNORE_PRIORITY;
    }
    if (task->data[1] == 0 && task->data[4] != 0)
    {
        task->data[0]++;
    }
    return FALSE;
}

static bool8 LavaridgeGymB1FWarpEffect_5(struct Task *task, struct ObjectEvent * objectEvent, struct Sprite *sprite)
{
    TryFadeOutOldMapMusic();
    WarpFadeOutScreen();
    task->data[0]++;
    return FALSE;
}

static bool8 LavaridgeGymB1FWarpEffect_6(struct Task *task, struct ObjectEvent * objectEvent, struct Sprite *sprite)
{
    if (!gPaletteFade.active && BGMusicStopped() == TRUE)
    {
        WarpIntoMap();
        gFieldCallback = FieldCB_LavaridgeGymB1FWarpExit;
        SetMainCallback2(CB2_LoadMap);
        DestroyTask(FindTaskIdByFunc(Task_LavaridgeGymB1FWarp));
    }
    return FALSE;
}

static bool8 (*const sLavaridgeGymB1FWarpExitEffectFuncs[])(struct Task *task, struct ObjectEvent * objectEvent, struct Sprite *sprite) = {
    LavaridgeGymB1FWarpExitEffect_1,
    LavaridgeGymB1FWarpExitEffect_2,
    LavaridgeGymB1FWarpExitEffect_3,
    LavaridgeGymB1FWarpExitEffect_4
};

static void FieldCB_LavaridgeGymB1FWarpExit(void)
{
    Overworld_PlaySpecialMapMusic();
    WarpFadeInScreen();
    QuestLog_DrawPreviouslyOnQuestHeaderIfInPlaybackMode();
    LockPlayerFieldControls();
    gFieldCallback = NULL;
    CreateTask(Task_LavaridgeGymB1FWarpExit, 0);
}

static void Task_LavaridgeGymB1FWarpExit(u8 taskId)
{
    while (sLavaridgeGymB1FWarpExitEffectFuncs[gTasks[taskId].data[0]](&gTasks[taskId], &gObjectEvents[gPlayerAvatar.objectEventId], &gSprites[gPlayerAvatar.spriteId]));
}

static bool8 LavaridgeGymB1FWarpExitEffect_1(struct Task *task, struct ObjectEvent * objectEvent, struct Sprite *sprite)
{
    CameraObjectReset2();
    FreezeObjectEvents();
    gPlayerAvatar.preventStep = TRUE;
    objectEvent->invisible = TRUE;
    task->data[0]++;
    return FALSE;
}

static bool8 LavaridgeGymB1FWarpExitEffect_2(struct Task *task, struct ObjectEvent * objectEvent, struct Sprite *sprite)
{
    if (IsWeatherNotFadingIn())
    {
        gFieldEffectArguments[0] = objectEvent->currentCoords.x;
        gFieldEffectArguments[1] = objectEvent->currentCoords.y;
        gFieldEffectArguments[2] = sprite->subpriority - 1;
        gFieldEffectArguments[3] = sprite->oam.priority;
        task->data[1] = FieldEffectStart(FLDEFF_POP_OUT_OF_ASH);
        task->data[0]++;
    }
    return FALSE;
}

static bool8 LavaridgeGymB1FWarpExitEffect_3(struct Task *task, struct ObjectEvent * objectEvent, struct Sprite *sprite)
{
    sprite = &gSprites[task->data[1]];
    if (sprite->animCmdIndex > 1)
    {
        task->data[0]++;
        objectEvent->invisible = FALSE;
        CameraObjectReset1();
        PlaySE(SE_M_DIG);
        ObjectEventSetHeldMovement(objectEvent, GetJumpMovementAction(DIR_EAST));
    }
    return FALSE;
}

static bool8 LavaridgeGymB1FWarpExitEffect_4(struct Task *task, struct ObjectEvent * objectEvent, struct Sprite *sprite)
{
    if (ObjectEventClearHeldMovementIfFinished(objectEvent))
    {
        gPlayerAvatar.preventStep = FALSE;
        UnlockPlayerFieldControls();
        UnfreezeObjectEvents();
        DestroyTask(FindTaskIdByFunc(Task_LavaridgeGymB1FWarpExit));
    }
    return FALSE;
}

static void Task_LavaridgeGym1FWarp(u8 taskId);
static bool8 LavaridgeGym1FWarpEffect_1(struct Task *task, struct ObjectEvent * objectEvent, struct Sprite *sprite);
static bool8 LavaridgeGym1FWarpEffect_2(struct Task *task, struct ObjectEvent * objectEvent, struct Sprite *sprite);
static bool8 LavaridgeGym1FWarpEffect_3(struct Task *task, struct ObjectEvent * objectEvent, struct Sprite *sprite);
static bool8 LavaridgeGym1FWarpEffect_4(struct Task *task, struct ObjectEvent * objectEvent, struct Sprite *sprite);
static bool8 LavaridgeGym1FWarpEffect_5(struct Task *task, struct ObjectEvent * objectEvent, struct Sprite *sprite);

static bool8 (*const sLavaridgeGym1FWarpEffectFuncs[])(struct Task *task, struct ObjectEvent * objectEvent, struct Sprite *sprite) = {
    LavaridgeGym1FWarpEffect_1,
    LavaridgeGym1FWarpEffect_2,
    LavaridgeGym1FWarpEffect_3,
    LavaridgeGym1FWarpEffect_4,
    LavaridgeGym1FWarpEffect_5
};

// For the ash puff effect when warping off the B1F ash tiles
u8 FldEff_LavaridgeGymWarp(void)
{
    u8 spriteId;
    SetSpritePosToOffsetMapCoords((s16 *)&gFieldEffectArguments[0], (s16 *)&gFieldEffectArguments[1], 8, 8);
    spriteId = CreateSpriteAtEnd(gFieldEffectObjectTemplatePointers[FLDEFFOBJ_ASH_LAUNCH], gFieldEffectArguments[0], gFieldEffectArguments[1], gFieldEffectArguments[2]);
    gSprites[spriteId].oam.priority = gFieldEffectArguments[3];
    gSprites[spriteId].coordOffsetEnabled = TRUE;
    return spriteId;
}

void SpriteCB_AshLaunch(struct Sprite *sprite)
{
    if (sprite->animEnded)
    {
        FieldEffectStop(sprite, FLDEFF_LAVARIDGE_GYM_WARP);
    }
}

void StartLavaridgeGym1FWarp(u8 priority)
{
    CreateTask(Task_LavaridgeGym1FWarp, priority);
}

static void Task_LavaridgeGym1FWarp(u8 taskId)
{
    while(sLavaridgeGym1FWarpEffectFuncs[gTasks[taskId].data[0]](&gTasks[taskId], &gObjectEvents[gPlayerAvatar.objectEventId], &gSprites[gPlayerAvatar.spriteId]));
}

static bool8 LavaridgeGym1FWarpEffect_1(struct Task *task, struct ObjectEvent * objectEvent, struct Sprite *sprite)
{
    FreezeObjectEvents();
    CameraObjectReset2();
    gPlayerAvatar.preventStep = TRUE;
    objectEvent->fixedPriority = TRUE;
    task->data[0]++;
    return FALSE;
}

static bool8 LavaridgeGym1FWarpEffect_2(struct Task *task, struct ObjectEvent * objectEvent, struct Sprite *sprite)
{
    if (ObjectEventClearHeldMovementIfFinished(objectEvent))
    {
        if (task->data[1] > 3)
        {
            gFieldEffectArguments[0] = objectEvent->currentCoords.x;
            gFieldEffectArguments[1] = objectEvent->currentCoords.y;
            gFieldEffectArguments[2] = sprite->subpriority - 1;
            gFieldEffectArguments[3] = sprite->oam.priority;
            task->data[1] = FieldEffectStart(FLDEFF_POP_OUT_OF_ASH);
            task->data[0]++;
        } else
        {
            task->data[1]++;
            ObjectEventSetHeldMovement(objectEvent, GetWalkInPlaceFasterMovementAction(objectEvent->facingDirection));
            PlaySE(SE_LAVARIDGE_FALL_WARP);
        }
    }
    return FALSE;
}

static bool8 LavaridgeGym1FWarpEffect_3(struct Task *task, struct ObjectEvent * objectEvent, struct Sprite *sprite)
{
    if (gSprites[task->data[1]].animCmdIndex == 2)
    {
        objectEvent->invisible = TRUE;
        task->data[0]++;
    }
    return FALSE;
}

static bool8 LavaridgeGym1FWarpEffect_4(struct Task *task, struct ObjectEvent * objectEvent, struct Sprite *sprite)
{
    if (!FieldEffectActiveListContains(FLDEFF_POP_OUT_OF_ASH))
    {
        TryFadeOutOldMapMusic();
        WarpFadeOutScreen();
        task->data[0]++;
    }
    return FALSE;
}

static bool8 LavaridgeGym1FWarpEffect_5(struct Task *task, struct ObjectEvent * objectEvent, struct Sprite *sprite)
{
    if (!gPaletteFade.active && BGMusicStopped() == TRUE)
    {
        WarpIntoMap();
        gFieldCallback = FieldCB_FallWarpExit;
        SetMainCallback2(CB2_LoadMap);
        DestroyTask(FindTaskIdByFunc(Task_LavaridgeGym1FWarp));
    }
    return FALSE;
}

u8 FldEff_PopOutOfAsh(void)
{
    u8 spriteId;
    SetSpritePosToOffsetMapCoords((s16 *)&gFieldEffectArguments[0], (s16 *)&gFieldEffectArguments[1], 8, 8);
    spriteId = CreateSpriteAtEnd(gFieldEffectObjectTemplatePointers[FLDEFFOBJ_ASH_PUFF], gFieldEffectArguments[0], gFieldEffectArguments[1], gFieldEffectArguments[2]);
    gSprites[spriteId].oam.priority = gFieldEffectArguments[3];
    gSprites[spriteId].coordOffsetEnabled = TRUE;
    return spriteId;
}

void SpriteCB_PopOutOfAsh(struct Sprite *sprite)
{
    if (sprite->animEnded)
    {
        FieldEffectStop(sprite, FLDEFF_POP_OUT_OF_ASH);
    }
}

// Task data for Task_EscapeRopeWarpOut
#define tState       data[0]
#define tSpinDelay   data[1]
#define tNumTurns    data[2]
#define tTimer       data[3]
#define tOffscreen   data[4]
#define tMovingState data[5]
#define tOffsetY     data[6]
#define tDirection   data[15]

static void Task_EscapeRopeWarpOut(u8 taskId);
static void EscapeRopeWarpOutEffect_Init(struct Task *task);
static void EscapeRopeWarpOutEffect_Spin(struct Task *task);
static u8 SpinObjectEvent(struct ObjectEvent *playerObj, s16 *timer, s16 *numTurns);
static bool32 WarpOutObjectEventUpwards(struct ObjectEvent *playerObj, s16 *movingState, s16 *offsetY);
static void FieldCallback_EscapeRopeExit(void);
static void Task_EscapeRopeWarpIn(u8 taskId);
static void EscapeRopeWarpInEffect_Init(struct Task *task);
static void EscapeRopeWarpInEffect_Spin(struct Task *task);

static void (*const sEscapeRopeWarpOutEffectFuncs[])(struct Task *task) =
{
    EscapeRopeWarpOutEffect_Init,
    EscapeRopeWarpOutEffect_Spin
};

void StartEscapeRopeFieldEffect(void)
{
    LockPlayerFieldControls();
    FreezeObjectEvents();
    CreateTask(Task_EscapeRopeWarpOut, 80);
}

static void Task_EscapeRopeWarpOut(u8 taskId)
{
    sEscapeRopeWarpOutEffectFuncs[gTasks[taskId].tState](&gTasks[taskId]);
}

static void EscapeRopeWarpOutEffect_Init(struct Task *task)
{
    task->tState++;
    task->data[13] = 64; // unused
    task->data[14] = GetPlayerFacingDirection(); // unused
    task->tDirection = DIR_NONE;
}

static void EscapeRopeWarpOutEffect_Spin(struct Task *task)
{
    struct ObjectEvent *playerObj = &gObjectEvents[gPlayerAvatar.objectEventId];
    s16 *data = task->data;
    SpinObjectEvent(playerObj, &task->tSpinDelay, &task->tNumTurns);
    if (tTimer < 60)
    {
        tTimer++;
        if (tTimer == 20)
            PlaySE(SE_WARP_IN);
    }
    else if (tOffscreen == FALSE && !WarpOutObjectEventUpwards(playerObj, &task->tMovingState, &task->tOffsetY))
    {
        TryFadeOutOldMapMusic();
        WarpFadeOutScreen();
        tOffscreen = TRUE;
    }
    if (tOffscreen == TRUE && !gPaletteFade.active && BGMusicStopped() == TRUE)
    {
        SetObjectEventDirection(playerObj, task->tDirection); // always DIR_NONE
        SetWarpDestinationToEscapeWarp();
        WarpIntoMap();
        gFieldCallback = FieldCallback_EscapeRopeExit;
        SetMainCallback2(CB2_LoadMap);
        DestroyTask(FindTaskIdByFunc(Task_EscapeRopeWarpOut));
    }
}

static const u8 sSpinDirections[] = 
{
    [DIR_NONE]  = DIR_SOUTH,
    [DIR_SOUTH] = DIR_WEST,
    [DIR_NORTH] = DIR_EAST,
    [DIR_WEST]  = DIR_NORTH,
    [DIR_EAST]  = DIR_SOUTH,
};

static u8 SpinObjectEvent(struct ObjectEvent *playerObj, s16 *spinDelay, s16 *numTurns)
{
    if (!ObjectEventIsMovementOverridden(playerObj) || ObjectEventClearHeldMovementIfFinished(playerObj))
    {
        if (*spinDelay != 0 && --(*spinDelay) != 0)
            return playerObj->facingDirection;
        ObjectEventSetHeldMovement(playerObj, GetFaceDirectionMovementAction(sSpinDirections[playerObj->facingDirection]));
        if (*numTurns < 12)
            (*numTurns)++;
        *spinDelay = 12 >> (*numTurns);
        return sSpinDirections[playerObj->facingDirection];
    }
    return playerObj->facingDirection;
}

static bool32 WarpOutObjectEventUpwards(struct ObjectEvent *playerObj, s16 *movingState, s16 *offsetY)
{
    struct Sprite *sprite = &gSprites[playerObj->spriteId];
    switch (*movingState)
    {
    case 0:
        CameraObjectReset2();
        (*movingState)++;
        // fallthrough
    case 1:
        sprite->y2 -= 8;
        (*offsetY) -= 8;
        if (*offsetY <= -16)
        {
            playerObj->fixedPriority = TRUE;
            sprite->oam.priority = 1;
            sprite->subpriority = 0;
            sprite->subspriteMode = SUBSPRITES_OFF;
            (*movingState)++;
        }
        break;
    case 2:
        sprite->y2 -= 8;
        (*offsetY) -= 8;
        if (*offsetY <= -88)
        {
            (*movingState)++;
            return FALSE;
        }
        break;
    case 3:
        return FALSE;
    }
    return TRUE;
}

#undef tState
#undef tSpinDelay
#undef tNumTurns
#undef tTimer
#undef tOffscreen
#undef tMovingState
#undef tOffsetY
#undef tDirection

// Task data for Task_EscapeRopeWarpIn
#define tState         data[0]
#define tMovingState   data[1]
#define tOffsetY       data[2]
#define tPriority      data[3]
#define tSubpriority   data[4]
#define tSubspriteMode data[5]
#define tTimer         data[6]
#define tSpinEnded     data[7]
#define tCurrentDir    data[8]
#define tSpinDelay     data[9]
#define tNumTurns      data[10]
#define tOriginalDir   data[15]

static void (*const sEscapeRopeWarpInEffectFuncs[])(struct Task *task) =
{
    EscapeRopeWarpInEffect_Init,
    EscapeRopeWarpInEffect_Spin
};

static bool32 WarpInObjectEventDownwards(struct ObjectEvent *playerObj, s16 *movingState, s16 *offsetY, s16 *priority, s16 *subpriority, s16 *subspriteMode)
{
    struct Sprite *sprite = &gSprites[playerObj->spriteId];
    switch (*movingState)
    {
    case 0:
        CameraObjectReset2();
        *offsetY = -88;
        sprite->y2 -= 88;
        *priority = sprite->oam.priority;
        *subpriority = sprite->subpriority;
        *subspriteMode = sprite->subspriteMode;
        playerObj->fixedPriority = TRUE;
        sprite->oam.priority = 1;
        sprite->subpriority = 0;
        sprite->subspriteMode = SUBSPRITES_OFF;
        (*movingState)++;
        // fallthrough
    case 1:
        sprite->y2 += 4;
        (*offsetY) += 4;
        if (*offsetY >= -16)
        {
            sprite->oam.priority = *priority;
            sprite->subpriority = *subpriority;
            sprite->subspriteMode = *subspriteMode;
            (*movingState)++;
        }
        break;
    case 2:
        sprite->y2 += 4;
        (*offsetY) += 4;
        if (*offsetY >= 0)
        {
            PlaySE(SE_CLICK);
            CameraObjectReset1();
            (*movingState)++;
            return FALSE;
        }
        break;
    case 3:
        return FALSE;
    }
    return TRUE;
}

static void FieldCallback_EscapeRopeExit(void)
{
    Overworld_PlaySpecialMapMusic();
    WarpFadeInScreen();
    QuestLog_DrawPreviouslyOnQuestHeaderIfInPlaybackMode();
    LockPlayerFieldControls();
    FreezeObjectEvents();
    gFieldCallback = NULL;
    gObjectEvents[gPlayerAvatar.objectEventId].invisible = TRUE;
    CreateTask(Task_EscapeRopeWarpIn, 0);
}

static void Task_EscapeRopeWarpIn(u8 taskId)
{
    sEscapeRopeWarpInEffectFuncs[gTasks[taskId].tState](&gTasks[taskId]);
}

static void EscapeRopeWarpInEffect_Init(struct Task *task)
{
    if (IsWeatherNotFadingIn())
    {
        PlaySE(SE_WARP_OUT);
        task->tOriginalDir = GetPlayerFacingDirection();
        task->tState++;
    }
}

static void EscapeRopeWarpInEffect_Spin(struct Task *task)
{
    s16 *data = task->data;
    struct ObjectEvent *playerObj = &gObjectEvents[gPlayerAvatar.objectEventId];
    bool32 moving = WarpInObjectEventDownwards(playerObj, &tMovingState, &tOffsetY, &tPriority, &tSubpriority, &tSubspriteMode);
    playerObj->invisible = FALSE;
    if (tTimer < 8)
        tTimer++;
    else if (tSpinEnded == FALSE)
    {
        tTimer++;
        tCurrentDir = SpinObjectEvent(playerObj, &tSpinDelay, &tNumTurns);
        if (tTimer >= 50 && tCurrentDir == tOriginalDir)
            tSpinEnded = TRUE;
    }
    if (!moving && tCurrentDir == tOriginalDir && ObjectEventCheckHeldMovementStatus(playerObj) == TRUE)
    {
        playerObj->invisible = FALSE;
        playerObj->fixedPriority = FALSE;
        UnlockPlayerFieldControls();
        UnfreezeObjectEvents();
        DestroyTask(FindTaskIdByFunc(Task_EscapeRopeWarpIn));
    }
}

#undef tState
#undef tMovingState
#undef tOffsetY
#undef tPriority
#undef tSubpriority
#undef tSubspriteMode
#undef tTimer
#undef tSpinEnded
#undef tCurrentDir
#undef tSpinDelay
#undef tNumTurns
#undef tOriginalDir

static void Task_DoTeleportFieldEffect(u8 taskId);
static void TeleportFieldEffectTask1(struct Task *task);
static void TeleportFieldEffectTask2(struct Task *task);
static void TeleportFieldEffectTask3(struct Task *task);
static void TeleportFieldEffectTask4(struct Task *task);
static void FieldCallback_TeleportIn(void);
static void Task_DoTeleportInFieldEffect(u8 taskId);
static void TeleportInFieldEffectTask1(struct Task *task);
static void TeleportInFieldEffectTask2(struct Task *task);
static void TeleportInFieldEffectTask3(struct Task *task);

static void (*const sTeleportEffectFuncs[])(struct Task *) = {
    TeleportFieldEffectTask1,
    TeleportFieldEffectTask2,
    TeleportFieldEffectTask3,
    TeleportFieldEffectTask4
};

void CreateTeleportFieldEffectTask(void)
{
    CreateTask(Task_DoTeleportFieldEffect, 0);
}

static void Task_DoTeleportFieldEffect(u8 taskId)
{
    sTeleportEffectFuncs[gTasks[taskId].data[0]](&gTasks[taskId]);
}

static void TeleportFieldEffectTask1(struct Task *task)
{
    LockPlayerFieldControls();
    FreezeObjectEvents();
    CameraObjectReset2();
    task->data[15] = GetPlayerFacingDirection();
    task->data[0]++;
}

static void TeleportFieldEffectTask2(struct Task *task)
{
    u8 spinDirections[5] = {
        [DIR_NONE]  = DIR_SOUTH,
        [DIR_SOUTH] = DIR_WEST,
        [DIR_WEST]  = DIR_NORTH,
        [DIR_NORTH] = DIR_EAST,
        [DIR_EAST]  = DIR_SOUTH
    };
    struct ObjectEvent * objectEvent = &gObjectEvents[gPlayerAvatar.objectEventId];
    if (task->data[1] == 0 || (--task->data[1]) == 0)
    {
        ObjectEventTurn(objectEvent, spinDirections[objectEvent->facingDirection]);
        task->data[1] = 8;
        task->data[2]++;
    }
    if (task->data[2] > 7 && task->data[15] == objectEvent->facingDirection)
    {
        task->data[0]++;
        task->data[1] = 4;
        task->data[2] = 8;
        task->data[3] = 1;
        PlaySE(SE_WARP_IN);
    }
}

static void TeleportFieldEffectTask3(struct Task *task)
{
    u8 spinDirections[5] = {DIR_SOUTH, DIR_WEST, DIR_EAST, DIR_NORTH, DIR_SOUTH};
    struct ObjectEvent * objectEvent = &gObjectEvents[gPlayerAvatar.objectEventId];
    struct Sprite *sprite = &gSprites[gPlayerAvatar.spriteId];
    if ((--task->data[1]) <= 0)
    {
        task->data[1] = 4;
        ObjectEventTurn(objectEvent, spinDirections[objectEvent->facingDirection]);
    }
    sprite->y -= task->data[3];
    task->data[4] += task->data[3];
    if ((--task->data[2]) <= 0 && (task->data[2] = 4, task->data[3] < 8))
    {
        task->data[3] <<= 1;
    }
    if (task->data[4] > 8 && (sprite->oam.priority = 1, sprite->subspriteMode != SUBSPRITES_OFF))
    {
        sprite->subspriteMode = SUBSPRITES_IGNORE_PRIORITY;
    }
    if (task->data[4] >= 0xa8)
    {
        task->data[0]++;
        TryFadeOutOldMapMusic();
        WarpFadeOutScreen();
    }
}

static void TeleportFieldEffectTask4(struct Task *task)
{
    if (!gPaletteFade.active)
    {
        if (BGMusicStopped() == TRUE)
        {
            SetWarpDestinationToLastHealLocation();
            WarpIntoMap();
            SetMainCallback2(CB2_LoadMap);
            gFieldCallback = FieldCallback_TeleportIn;
            DestroyTask(FindTaskIdByFunc(Task_DoTeleportFieldEffect));
        }
    }
}

static void (*const sTeleportInEffectFuncs[])(struct Task *) = {
    TeleportInFieldEffectTask1,
    TeleportInFieldEffectTask2,
    TeleportInFieldEffectTask3
};

static void FieldCallback_TeleportIn(void)
{
    Overworld_PlaySpecialMapMusic();
    WarpFadeInScreen();
    QuestLog_DrawPreviouslyOnQuestHeaderIfInPlaybackMode();
    LockPlayerFieldControls();
    FreezeObjectEvents();
    gFieldCallback = NULL;
    gObjectEvents[gPlayerAvatar.objectEventId].invisible = TRUE;
    CameraObjectReset2();
    CreateTask(Task_DoTeleportInFieldEffect, 0);
}

static void Task_DoTeleportInFieldEffect(u8 taskId)
{
    sTeleportInEffectFuncs[gTasks[taskId].data[0]](&gTasks[taskId]);
}

static void TeleportInFieldEffectTask1(struct Task *task)
{
    struct Sprite *sprite;
    s16 centerToCornerVecY;
    if (IsWeatherNotFadingIn())
    {
        sprite = &gSprites[gPlayerAvatar.spriteId];
        centerToCornerVecY = -(sprite->centerToCornerVecY << 1);
        sprite->y2 = -(sprite->y + sprite->centerToCornerVecY + gSpriteCoordOffsetY + centerToCornerVecY);
        gObjectEvents[gPlayerAvatar.objectEventId].invisible = FALSE;
        task->data[0]++;
        task->data[1] = 8;
        task->data[2] = 1;
        task->data[14] = sprite->subspriteMode;
        task->data[15] = GetPlayerFacingDirection();
        PlaySE(SE_WARP_IN);
    }
}

static void TeleportInFieldEffectTask2(struct Task *task)
{
    u8 spinDirections[5] = {1, 3, 4, 2, 1};
    struct ObjectEvent * objectEvent = &gObjectEvents[gPlayerAvatar.objectEventId];
    struct Sprite *sprite = &gSprites[gPlayerAvatar.spriteId];
    if ((sprite->y2 += task->data[1]) >= -8)
    {
        if (task->data[13] == 0)
        {
            task->data[13]++;
            objectEvent->triggerGroundEffectsOnMove = TRUE;
            sprite->subspriteMode = task->data[14];
        }
    } else
    {
        sprite->oam.priority = 1;
        if (sprite->subspriteMode != SUBSPRITES_OFF)
        {
            sprite->subspriteMode = SUBSPRITES_IGNORE_PRIORITY;
        }
    }
    if (sprite->y2 >= -0x30 && task->data[1] > 1 && !(sprite->y2 & 1))
    {
        task->data[1]--;
    }
    if ((--task->data[2]) == 0)
    {
        task->data[2] = 4;
        ObjectEventTurn(objectEvent, spinDirections[objectEvent->facingDirection]);
    }
    if (sprite->y2 >= 0)
    {
        sprite->y2 = 0;
        task->data[0]++;
        task->data[1] = 1;
        task->data[2] = 0;
    }
}

static void TeleportInFieldEffectTask3(struct Task *task)
{
    u8 spinDirections[5] = {1, 3, 4, 2, 1};
    struct ObjectEvent * objectEvent = &gObjectEvents[gPlayerAvatar.objectEventId];
    if ((--task->data[1]) == 0)
    {
        ObjectEventTurn(objectEvent, spinDirections[objectEvent->facingDirection]);
        task->data[1] = 8;
        if ((++task->data[2]) > 4 && task->data[14] == objectEvent->facingDirection)
        {
            UnlockPlayerFieldControls();
            CameraObjectReset1();
            UnfreezeObjectEvents();
            DestroyTask(FindTaskIdByFunc(Task_DoTeleportInFieldEffect));
        }
    }
}

static void Task_ShowMon_Outdoors(u8 taskId);
static void ShowMonEffect_Outdoors_1(struct Task *task);
static void ShowMonEffect_Outdoors_2(struct Task *task);
static void ShowMonEffect_Outdoors_3(struct Task *task);
static void ShowMonEffect_Outdoors_4(struct Task *task);
static void ShowMonEffect_Outdoors_5(struct Task *task);
static void ShowMonEffect_Outdoors_6(struct Task *task);
static void ShowMonEffect_Outdoors_7(struct Task *task);
static void VBlankCB_ShowMonEffect_Outdoors(void);
static void LoadFieldMoveStreaksTilemapToVram(u16 screenbase);
static void Task_ShowMon_Indoors(u8 taskId);
static void ShowMonEffect_Indoors_1(struct Task *task);
static void ShowMonEffect_Indoors_2(struct Task *task);
static void ShowMonEffect_Indoors_3(struct Task *task);
static void ShowMonEffect_Indoors_4(struct Task *task);
static void ShowMonEffect_Indoors_5(struct Task *task);
static void ShowMonEffect_Indoors_6(struct Task *task);
static void ShowMonEffect_Indoors_7(struct Task *task);
static void VBlankCB_ShowMonEffect_Indoors(void);
static void AnimateIndoorShowMonBg(struct Task *task);
static bool8 SlideIndoorBannerOnscreen(struct Task *task);
static bool8 SlideIndoorBannerOffscreen(struct Task *task);
static u8 InitFieldMoveMonSprite(u32 species, u32 otId, u32 personality);
static void SpriteCB_FieldMoveMonSlideOnscreen(struct Sprite *sprite);
static void SpriteCB_FieldMoveMonWaitAfterCry(struct Sprite *sprite);
static void SpriteCB_FieldMoveMonSlideOffscreen(struct Sprite *sprite);

static void (*const sShowMonOutdoorsEffectFuncs[])(struct Task *task) = {
    ShowMonEffect_Outdoors_1,
    ShowMonEffect_Outdoors_2,
    ShowMonEffect_Outdoors_3,
    ShowMonEffect_Outdoors_4,
    ShowMonEffect_Outdoors_5,
    ShowMonEffect_Outdoors_6,
    ShowMonEffect_Outdoors_7
};

u32 FldEff_FieldMoveShowMon(void)
{
    u8 taskId;
    if (IsMapTypeOutdoors(GetCurrentMapType()) == TRUE)
        taskId = CreateTask(Task_ShowMon_Outdoors, 0xFF);
    else
        taskId = CreateTask(Task_ShowMon_Indoors, 0xFF);
    gTasks[taskId].data[15] = InitFieldMoveMonSprite(gFieldEffectArguments[0], gFieldEffectArguments[1], gFieldEffectArguments[2]);
    return 0;
}

u32 FldEff_FieldMoveShowMonInit(void)
{
    u32 r6 = gFieldEffectArguments[0] & 0x80000000;
    u8 partyIdx = gFieldEffectArguments[0];
    gFieldEffectArguments[0] = GetMonData(&gPlayerParty[partyIdx], MON_DATA_SPECIES);
    gFieldEffectArguments[1] = GetMonData(&gPlayerParty[partyIdx], MON_DATA_OT_ID);
    gFieldEffectArguments[2] = GetMonData(&gPlayerParty[partyIdx], MON_DATA_PERSONALITY);
    gFieldEffectArguments[0] |= r6;
    FieldEffectStart(FLDEFF_FIELD_MOVE_SHOW_MON);
    FieldEffectActiveListRemove(FLDEFF_FIELD_MOVE_SHOW_MON_INIT);
    return 0;
}

static void Task_ShowMon_Outdoors(u8 taskId)
{
    sShowMonOutdoorsEffectFuncs[gTasks[taskId].data[0]](&gTasks[taskId]);
}

static void ShowMonEffect_Outdoors_1(struct Task *task)
{
    task->data[11] = GetGpuReg(REG_OFFSET_WININ);
    task->data[12] = GetGpuReg(REG_OFFSET_WINOUT);
    StoreWordInTwoHalfwords((u16 *)&task->data[13], (u32)gMain.vblankCallback);
    task->data[1] = WIN_RANGE(0xF0, 0xF1);
    task->data[2] = WIN_RANGE(0x50, 0x51);
    task->data[3] = WININ_WIN0_BG_ALL | WININ_WIN0_OBJ | WININ_WIN0_CLR;
    task->data[4] = WINOUT_WIN01_BG1 | WINOUT_WIN01_BG2 | WINOUT_WIN01_BG3 | WINOUT_WIN01_OBJ | WINOUT_WIN01_CLR;
    SetGpuReg(REG_OFFSET_WIN0H, task->data[1]);
    SetGpuReg(REG_OFFSET_WIN0V, task->data[2]);
    SetGpuReg(REG_OFFSET_WININ, task->data[3]);
    SetGpuReg(REG_OFFSET_WINOUT, task->data[4]);
    SetVBlankCallback(VBlankCB_ShowMonEffect_Outdoors);
    task->data[0]++;
}

static void ShowMonEffect_Outdoors_2(struct Task *task)
{
    u16 charbase = ((GetGpuReg(REG_OFFSET_BG0CNT) >> 2) << 14);
    u16 screenbase = ((GetGpuReg(REG_OFFSET_BG0CNT) >> 8) << 11);
    CpuCopy16(sFieldMoveStreaksOutdoors_Gfx, (void *)(VRAM + charbase), 0x200);
    CpuFill32(0, (void *)(VRAM + screenbase), 0x800);
    LoadPalette(sFieldMoveStreaksOutdoors_Pal, BG_PLTT_ID(15), sizeof(sFieldMoveStreaksOutdoors_Pal));
    LoadFieldMoveStreaksTilemapToVram(screenbase);
    task->data[0]++;
}

static void ShowMonEffect_Outdoors_3(struct Task *task)
{
    s16 win0h_lo;
    s16 win0v_lo;
    s16 win0v_hi;
    task->data[5] -= 16;
    win0h_lo = ((u16)task->data[1] >> 8);
    win0v_lo = ((u16)task->data[2] >> 8);
    win0v_hi = ((u16)task->data[2] & 0xff);
    win0h_lo -= 16;
    win0v_lo -= 2;
    win0v_hi += 2;
    if (win0h_lo < 0)
    {
        win0h_lo = 0;
    }
    if (win0v_lo < 0x28)
    {
        win0v_lo = 0x28;
    }
    if (win0v_hi > 0x78)
    {
        win0v_hi = 0x78;
    }
    task->data[1] = WIN_RANGE(win0h_lo, task->data[1] & 0xff);
    task->data[2] = WIN_RANGE(win0v_lo, win0v_hi);
    if (win0h_lo == 0 && win0v_lo == 0x28 && win0v_hi == 0x78)
    {
        gSprites[task->data[15]].callback = SpriteCB_FieldMoveMonSlideOnscreen;
        task->data[0]++;
    }
}

static void ShowMonEffect_Outdoors_4(struct Task *task)
{
    task->data[5] -= 16;
    if (gSprites[task->data[15]].data[7])
    {
        task->data[0]++;
    }
}

static void ShowMonEffect_Outdoors_5(struct Task *task)
{
    s16 win0v_lo;
    s16 win0v_hi;
    task->data[5] -= 16;
    win0v_lo = (task->data[2] >> 8);
    win0v_hi = (task->data[2] & 0xff);
    win0v_lo += 6;
    win0v_hi -= 6;
    if (win0v_lo > 0x50)
    {
        win0v_lo = 0x50;
    }
    if (win0v_hi < 0x51)
    {
        win0v_hi = 0x51;
    }
    task->data[2] = WIN_RANGE(win0v_lo, win0v_hi);
    if (win0v_lo == 0x50 && win0v_hi == 0x51)
    {
        task->data[0]++;
    }
}

static void ShowMonEffect_Outdoors_6(struct Task *task)
{
    u16 bg0cnt = (GetGpuReg(REG_OFFSET_BG0CNT) >> 8) << 11;
    CpuFill32(0, (void *)VRAM + bg0cnt, 0x800);
    task->data[1] = WIN_RANGE(0x00, 0xf1);
    task->data[2] = WIN_RANGE(0x00, 0xa1);
    task->data[3] = task->data[11];
    task->data[4] = task->data[12];
    task->data[0]++;
}

static void ShowMonEffect_Outdoors_7(struct Task *task)
{
    IntrCallback callback;
    LoadWordFromTwoHalfwords((u16 *)&task->data[13], (u32 *)&callback);
    SetVBlankCallback(callback);
    ChangeBgX(0, 0, 0);
    ChangeBgY(0, 0, 0);
    Menu_LoadStdPal();
    FreeResourcesAndDestroySprite(&gSprites[task->data[15]], task->data[15]);
    FieldEffectActiveListRemove(FLDEFF_FIELD_MOVE_SHOW_MON);
    DestroyTask(FindTaskIdByFunc(Task_ShowMon_Outdoors));
}

static void VBlankCB_ShowMonEffect_Outdoors(void)
{
    IntrCallback callback;
    struct Task *task = &gTasks[FindTaskIdByFunc(Task_ShowMon_Outdoors)];
    LoadWordFromTwoHalfwords((u16 *)&task->data[13], (u32 *)&callback);
    callback();
    SetGpuReg(REG_OFFSET_WIN0H, task->data[1]);
    SetGpuReg(REG_OFFSET_WIN0V, task->data[2]);
    SetGpuReg(REG_OFFSET_WININ, task->data[3]);
    SetGpuReg(REG_OFFSET_WINOUT, task->data[4]);
    SetGpuReg(REG_OFFSET_BG0HOFS, task->data[5]);
    SetGpuReg(REG_OFFSET_BG0VOFS, task->data[6]);
}

static void LoadFieldMoveStreaksTilemapToVram(u16 screenbase)
{
    u16 i;
    u16 *dest;
    dest = (u16 *)(VRAM + (10 * 32) + screenbase);
    for (i = 0; i < (10 * 32); i++, dest++)
        *dest = sFieldMoveStreaksOutdoors_Tilemap[i] | 0xF000;
}

static void (*const sShowMonIndoorsEffectFuncs[])(struct Task *) = {
    ShowMonEffect_Indoors_1,
    ShowMonEffect_Indoors_2,
    ShowMonEffect_Indoors_3,
    ShowMonEffect_Indoors_4,
    ShowMonEffect_Indoors_5,
    ShowMonEffect_Indoors_6,
    ShowMonEffect_Indoors_7
};

static void Task_ShowMon_Indoors(u8 taskId)
{
    sShowMonIndoorsEffectFuncs[gTasks[taskId].data[0]](&gTasks[taskId]);
}

static void ShowMonEffect_Indoors_1(struct Task *task)
{
    SetGpuReg(REG_OFFSET_BG0HOFS, task->data[1]);
    SetGpuReg(REG_OFFSET_BG0VOFS, task->data[2]);
    StoreWordInTwoHalfwords((u16 *)&task->data[13], (u32)gMain.vblankCallback);
    SetVBlankCallback(VBlankCB_ShowMonEffect_Indoors);
    task->data[0]++;
}

static void ShowMonEffect_Indoors_2(struct Task *task)
{
    u16 charbase;
    u16 screenbase;
    charbase = ((GetGpuReg(REG_OFFSET_BG0CNT) >> 2) << 14);
    screenbase = ((GetGpuReg(REG_OFFSET_BG0CNT) >> 8) << 11);
    task->data[12] = screenbase;
    CpuCopy16(sFieldMoveStreaksIndoors_Gfx, (void *)(VRAM + charbase), 0x80);
    CpuFill32(0, (void *)(VRAM + screenbase), 0x800);
    LoadPalette(sFieldMoveStreaksIndoors_Pal, BG_PLTT_ID(15), sizeof(sFieldMoveStreaksIndoors_Pal));
    task->data[0]++;
}

static void ShowMonEffect_Indoors_3(struct Task *task)
{
    if (SlideIndoorBannerOnscreen(task))
    {
        task->data[5] = GetGpuReg(REG_OFFSET_WININ);
        SetGpuReg(REG_OFFSET_WININ, (task->data[5] & 0xFF) | WININ_WIN1_BG0 | WININ_WIN1_OBJ);
        SetGpuReg(REG_OFFSET_WIN1H, WIN_RANGE(0x00, 0xf0));
        SetGpuReg(REG_OFFSET_WIN1V, WIN_RANGE(0x28, 0x78));
        gSprites[task->data[15]].callback = SpriteCB_FieldMoveMonSlideOnscreen;
        task->data[0]++;
    }
    AnimateIndoorShowMonBg(task);
}

static void ShowMonEffect_Indoors_4(struct Task *task)
{
    AnimateIndoorShowMonBg(task);
    if (gSprites[task->data[15]].data[7])
    {
        task->data[0]++;
    }
}

static void ShowMonEffect_Indoors_5(struct Task *task)
{
    AnimateIndoorShowMonBg(task);
    task->data[3] = task->data[1] & 7;
    task->data[4] = 0;
    SetGpuReg(REG_OFFSET_WIN1H, WIN_RANGE(0xff, 0xff));
    SetGpuReg(REG_OFFSET_WIN1V, WIN_RANGE(0xff, 0xff));
    SetGpuReg(REG_OFFSET_WININ, task->data[5]);
    task->data[0]++;
}

static void ShowMonEffect_Indoors_6(struct Task *task)
{
    AnimateIndoorShowMonBg(task);
    if (SlideIndoorBannerOffscreen(task))
    {
        task->data[0]++;
    }
}

static void ShowMonEffect_Indoors_7(struct Task *task)
{
    IntrCallback intrCallback;
    u16 charbase;
    charbase = (GetGpuReg(REG_OFFSET_BG0CNT) >> 8) << 11;
    CpuFill32(0, (void *)VRAM + charbase, 0x800);
    LoadWordFromTwoHalfwords((u16 *)&task->data[13], (u32 *)&intrCallback);
    SetVBlankCallback(intrCallback);
    ChangeBgX(0, 0, 0);
    ChangeBgY(0, 0, 0);
    Menu_LoadStdPal();
    FreeResourcesAndDestroySprite(&gSprites[task->data[15]], task->data[15]);
    FieldEffectActiveListRemove(FLDEFF_FIELD_MOVE_SHOW_MON);
    DestroyTask(FindTaskIdByFunc(Task_ShowMon_Indoors));
}

static void VBlankCB_ShowMonEffect_Indoors(void)
{
    IntrCallback intrCallback;
    struct Task *task;
    task = &gTasks[FindTaskIdByFunc(Task_ShowMon_Indoors)];
    LoadWordFromTwoHalfwords((u16 *)&task->data[13], (u32 *)&intrCallback);
    intrCallback();
    SetGpuReg(REG_OFFSET_BG0HOFS, task->data[1]);
    SetGpuReg(REG_OFFSET_BG0VOFS, task->data[2]);
}

static void AnimateIndoorShowMonBg(struct Task *task)
{
    task->data[1] -= 16;
    task->data[3] += 16;
}

static bool8 SlideIndoorBannerOnscreen(struct Task *task)
{
    u16 i;
    u16 srcOffs;
    u16 dstOffs;
    u16 *dest;
    if (task->data[4] >= 32)
    {
        return TRUE;
    }
    dstOffs = (task->data[3] >> 3) & 0x1f;
    if (dstOffs >= task->data[4])
    {
        dstOffs = (32 - dstOffs) & 0x1f;
        srcOffs = (32 - task->data[4]) & 0x1f;
        dest = (u16 *)(VRAM + 0x140 + (u16)task->data[12]);
        for (i = 0; i < 10; i++)
        {
            dest[dstOffs + i * 32] = sFieldMoveStreaksIndoors_Tilemap[srcOffs + i * 32];
            dest[dstOffs + i * 32] |= 0xf000;

            dest[((dstOffs + 1) & 0x1f) + i * 32] = sFieldMoveStreaksIndoors_Tilemap[((srcOffs + 1) & 0x1f) + i * 32] | 0xf000;
            dest[((dstOffs + 1) & 0x1f) + i * 32] |= 0xf000;
        }
        task->data[4] += 2;
    }
    return FALSE;
}

static bool8 SlideIndoorBannerOffscreen(struct Task *task)
{
    u16 i;
    u16 dstOffs;
    u16 *dest;
    if (task->data[4] >= 32)
    {
        return TRUE;
    }
    dstOffs = task->data[3] >> 3;
    if (dstOffs >= task->data[4])
    {
        dstOffs = (task->data[1] >> 3) & 0x1f;
        dest = (u16 *)(VRAM + 0x140 + (u16)task->data[12]);
        for (i = 0; i < 10; i++)
        {
            dest[dstOffs + i * 32] = 0xf000;
            dest[((dstOffs + 1) & 0x1f) + i * 32] = 0xf000;
        }
        task->data[4] += 2;
    }
    return FALSE;
}

static u8 InitFieldMoveMonSprite(u32 species, u32 otId, u32 personality)
{
    bool16 playCry;
    u8 monSprite;
    struct Sprite *sprite;
    playCry = (species & 0x80000000) >> 16;
    species &= 0x7fffffff;
    monSprite = CreateMonSprite_FieldMove(species, otId, personality, 0x140, 0x50, 0);
    sprite = &gSprites[monSprite];
    sprite->callback = SpriteCallbackDummy;
    sprite->oam.priority = 0;
    sprite->data[0] = species;
    sprite->data[6] = playCry;
    return monSprite;
}

static void SpriteCB_FieldMoveMonSlideOnscreen(struct Sprite *sprite)
{
    if ((sprite->x -= 20) <= 0x78)
    {
        sprite->x = 0x78;
        sprite->data[1] = 30;
        sprite->callback = SpriteCB_FieldMoveMonWaitAfterCry;
        if (sprite->data[6])
        {
            PlayCry_NormalNoDucking(sprite->data[0], 0, CRY_VOLUME_RS, CRY_PRIORITY_NORMAL);
        }
        else
        {
            PlayCry_Normal(sprite->data[0], 0);
        }
    }
}

static void SpriteCB_FieldMoveMonWaitAfterCry(struct Sprite *sprite)
{
    if ((--sprite->data[1]) == 0)
    {
        sprite->callback = SpriteCB_FieldMoveMonSlideOffscreen;
    }
}

static void SpriteCB_FieldMoveMonSlideOffscreen(struct Sprite *sprite)
{
    if (sprite->x < -0x40)
    {
        sprite->data[7] = 1;
    }
    else
    {
        sprite->x -= 20;
    }
}

static void Task_FldEffUseSurf(u8 taskId);
static void UseSurfEffect_1(struct Task *task);
static void UseSurfEffect_2(struct Task *task);
static void UseSurfEffect_3(struct Task *task);
static void UseSurfEffect_4(struct Task *task);
static void UseSurfEffect_5(struct Task *task);

static void (*const sUseSurfEffectFuncs[])(struct Task *) = {
    UseSurfEffect_1,
    UseSurfEffect_2,
    UseSurfEffect_3,
    UseSurfEffect_4,
    UseSurfEffect_5,
};

u8 FldEff_UseSurf(void)
{
    u8 taskId = CreateTask(Task_FldEffUseSurf, 0xff);
    gTasks[taskId].data[15] = gFieldEffectArguments[0];
    Overworld_ClearSavedMusic();
    if (Overworld_MusicCanOverrideMapMusic(MUS_SURF))
        Overworld_ChangeMusicTo(MUS_SURF);
    return FALSE;
}

static void Task_FldEffUseSurf(u8 taskId)
{
    sUseSurfEffectFuncs[gTasks[taskId].data[0]](&gTasks[taskId]);
}

static void UseSurfEffect_1(struct Task *task)
{
    LockPlayerFieldControls();
    FreezeObjectEvents();
    gPlayerAvatar.preventStep = TRUE;
    SetPlayerAvatarStateMask(PLAYER_AVATAR_FLAG_SURFING);
    PlayerGetDestCoords(&task->data[1], &task->data[2]);
    MoveCoords(gObjectEvents[gPlayerAvatar.objectEventId].movementDirection, &task->data[1], &task->data[2]);
    task->data[0]++;
}

static void UseSurfEffect_2(struct Task *task)
{
    struct ObjectEvent * objectEvent;
    objectEvent = &gObjectEvents[gPlayerAvatar.objectEventId];
    if (!ObjectEventIsMovementOverridden(objectEvent) || ObjectEventClearHeldMovementIfFinished(objectEvent))
    {
        StartPlayerAvatarSummonMonForFieldMoveAnim();
        ObjectEventSetHeldMovement(objectEvent, MOVEMENT_ACTION_START_ANIM_IN_DIRECTION);
        task->data[0]++;
    }
}

static void UseSurfEffect_3(struct Task *task)
{
    struct ObjectEvent * objectEvent;
    objectEvent = &gObjectEvents[gPlayerAvatar.objectEventId];
    if (ObjectEventCheckHeldMovementStatus(objectEvent))
    {
        gFieldEffectArguments[0] = task->data[15] | 0x80000000;
        FieldEffectStart(FLDEFF_FIELD_MOVE_SHOW_MON_INIT);
        task->data[0]++;
    }
}

static void UseSurfEffect_4(struct Task *task)
{
    struct ObjectEvent * objectEvent;
    if (!FieldEffectActiveListContains(FLDEFF_FIELD_MOVE_SHOW_MON))
    {
        objectEvent = &gObjectEvents[gPlayerAvatar.objectEventId];
        ObjectEventSetGraphicsId(objectEvent, GetPlayerAvatarGraphicsIdByStateId(PLAYER_AVATAR_GFX_RIDE));
        ObjectEventClearHeldMovementIfFinished(objectEvent);
        ObjectEventSetHeldMovement(objectEvent, GetJumpSpecialMovementAction(objectEvent->movementDirection));
        gFieldEffectArguments[0] = task->data[1];
        gFieldEffectArguments[1] = task->data[2];
        gFieldEffectArguments[2] = gPlayerAvatar.objectEventId;
        objectEvent->fieldEffectSpriteId = FieldEffectStart(FLDEFF_SURF_BLOB);
        task->data[0]++;
    }
}

static void UseSurfEffect_5(struct Task *task)
{
    struct ObjectEvent * objectEvent;
    objectEvent = &gObjectEvents[gPlayerAvatar.objectEventId];
    if (ObjectEventClearHeldMovementIfFinished(objectEvent))
    {
        gPlayerAvatar.preventStep = FALSE;
        gPlayerAvatar.flags &= ~PLAYER_AVATAR_FLAG_CONTROLLABLE;
        ObjectEventSetHeldMovement(objectEvent, GetFaceDirectionMovementAction(objectEvent->movementDirection));
        SetSurfBlob_BobState(objectEvent->fieldEffectSpriteId, BOB_PLAYER_AND_MON);
        UnfreezeObjectEvents();
        UnlockPlayerFieldControls();
        FieldEffectActiveListRemove(FLDEFF_USE_SURF);
        DestroyTask(FindTaskIdByFunc(Task_FldEffUseSurf));
        SetHelpContext(HELPCONTEXT_SURFING);
    }
}

static void Task_FldEffUseVsSeeker(u8 taskId);
static void UseVsSeekerEffect_1(struct Task *task);
static void UseVsSeekerEffect_2(struct Task *task);
static void UseVsSeekerEffect_3(struct Task *task);
static void UseVsSeekerEffect_4(struct Task *task);

static void (*const sUseVsSeekerEffectFuncs[])(struct Task *task) = {
    UseVsSeekerEffect_1,
    UseVsSeekerEffect_2,
    UseVsSeekerEffect_3,
    UseVsSeekerEffect_4
};

u32 FldEff_UseVsSeeker(void)
{
    if (gQuestLogState == QL_STATE_RECORDING)
        QuestLogRecordPlayerAvatarGfxTransitionWithDuration(8, 89);
    CreateTask(Task_FldEffUseVsSeeker, 0xFF);
    return 0;
}

static void Task_FldEffUseVsSeeker(u8 taskId)
{
    sUseVsSeekerEffectFuncs[gTasks[taskId].data[0]](&gTasks[taskId]);
}

static void UseVsSeekerEffect_1(struct Task *task)
{
    LockPlayerFieldControls();
    FreezeObjectEvents();
    gPlayerAvatar.preventStep = TRUE;
    task->data[0]++;
}

static void UseVsSeekerEffect_2(struct Task *task)
{
    struct ObjectEvent * playerObj = &gObjectEvents[gPlayerAvatar.objectEventId];
    if (!ObjectEventIsMovementOverridden(playerObj) || ObjectEventClearHeldMovementIfFinished(playerObj))
    {
        StartPlayerAvatarVsSeekerAnim();
        ObjectEventSetHeldMovement(playerObj, MOVEMENT_ACTION_START_ANIM_IN_DIRECTION);
        task->data[0]++;
    }
}

static void UseVsSeekerEffect_3(struct Task *task)
{
    struct ObjectEvent * playerObj = &gObjectEvents[gPlayerAvatar.objectEventId];
    if (ObjectEventClearHeldMovementIfFinished(playerObj))
    {
        if (gPlayerAvatar.flags & (PLAYER_AVATAR_FLAG_ACRO_BIKE | PLAYER_AVATAR_FLAG_MACH_BIKE))
            ObjectEventSetGraphicsId(playerObj, GetPlayerAvatarGraphicsIdByStateId(PLAYER_AVATAR_GFX_BIKE));
        else if (gPlayerAvatar.flags & PLAYER_AVATAR_FLAG_SURFING)
            ObjectEventSetGraphicsId(playerObj, GetPlayerAvatarGraphicsIdByStateId(PLAYER_AVATAR_GFX_RIDE));
        else
            ObjectEventSetGraphicsId(playerObj, GetPlayerAvatarGraphicsIdByStateId(PLAYER_AVATAR_GFX_NORMAL));
        ObjectEventForceSetHeldMovement(playerObj, GetFaceDirectionMovementAction(playerObj->facingDirection));
        task->data[0]++;
    }
}

static void UseVsSeekerEffect_4(struct Task *task)
{
    struct ObjectEvent * playerObj = &gObjectEvents[gPlayerAvatar.objectEventId];
    if (ObjectEventClearHeldMovementIfFinished(playerObj))
    {
        gPlayerAvatar.preventStep = FALSE;
        FieldEffectActiveListRemove(FLDEFF_USE_VS_SEEKER);
        DestroyTask(FindTaskIdByFunc(Task_FldEffUseVsSeeker));
    }
}

static void SpriteCB_NPCFlyOut(struct Sprite *sprite);

u8 FldEff_NpcFlyOut(void)
{
    u8 spriteId = CreateSprite(gFieldEffectObjectTemplatePointers[FLDEFFOBJ_BIRD], 0x78, 0, 1);
    struct Sprite *sprite = &gSprites[spriteId];

    sprite->oam.paletteNum = 0;
    sprite->oam.priority = 1;
    sprite->callback = SpriteCB_NPCFlyOut;
    sprite->data[1] = gFieldEffectArguments[0];
    PlaySE(SE_M_FLY);
    return spriteId;
}

static void SpriteCB_NPCFlyOut(struct Sprite *sprite)
{
    struct Sprite *npcSprite;

    sprite->x2 = Cos(sprite->data[2], 0x8c);
    sprite->y2 = Sin(sprite->data[2], 0x48);
    sprite->data[2] = (sprite->data[2] + 4) & 0xff;
    if (sprite->data[0])
    {
        npcSprite = &gSprites[sprite->data[1]];
        npcSprite->coordOffsetEnabled = FALSE;
        npcSprite->x = sprite->x + sprite->x2;
        npcSprite->y = sprite->y + sprite->y2 - 8;
        npcSprite->x2 = 0;
        npcSprite->y2 = 0;
    }
    if (sprite->data[2] >= 0x80)
    {
        FieldEffectStop(sprite, FLDEFF_NPCFLY_OUT);
    }
}

// T-387: GOTO is not a bird (the user, 2026-10-08). Going, the player comes apart into the splash's own 0s and 1s
// (graphics/intro/game_freak: the presents scene's code becoming music), a ribbon of its notes comes down the swoop the
// bird flew, and the code goes off with the music along it. The ribbon rides a travelling wave -- the ripple of a
// sound, not the glide of a wing -- so the path is the bird's ellipse but the flight is not. Arriving is the same run
// backwards: the ribbon brings the code down, it settles where the player stands and flickers back into them, and the
// notes go on.
#define tState       data[0]
#define tMonPartyId  data[1]
#define tTimer       data[2]
#define tFrame       data[3] // frames since the ribbon's head set out (negative while the player comes apart)
#define tCenterX     data[4] // the player, on screen
#define tCenterY     data[5]
#define tLanding     data[6]
#define tReform      data[7] // arriving: frames since the code began turning back into the player
#define tAvatarFlags data[15]

#define GOTO_TAG_GLYPHS 0x1030 // the splash's 8x8 sheet: 0, 1, a note, a note lit -- and the palette both share
#define GOTO_TAG_NOTES  0x1031 // its 32x32 sheet: four notes in four colours
#define GOTO_NUM_BITS   12
#define GOTO_NUM_NOTES  12     // every third one big
#define GOTO_DISSOLVE   40     // frames the player takes to come apart, or back
#define GOTO_OUT_MEET   16     // the ribbon's frame at the player, going ...
#define GOTO_IN_MEET    32     // ... and arriving
#define GOTO_WAVE       6      // the ripple's depth, in pixels

enum {
    GLYPH_ON_PLAYER,
    GLYPH_RIBBON,
    GLYPH_GONE,
};

static const u16 sGoto_Pal[]        = INCBIN_U16("graphics/intro/game_freak/sparkles.gbapal");
static const u32 sGoto_Glyphs_Gfx[] = INCBIN_U32("graphics/intro/game_freak/sparkles_small.4bpp.lz");
static const u32 sGoto_Notes_Gfx[]  = INCBIN_U32("graphics/intro/game_freak/sparkles_big.4bpp.lz");

static const struct CompressedSpriteSheet sGoto_SpriteSheets[] =
{
    {sGoto_Glyphs_Gfx, 0x80,  GOTO_TAG_GLYPHS},
    {sGoto_Notes_Gfx,  0x800, GOTO_TAG_NOTES},
};

static const struct SpritePalette sGoto_SpritePalette = {sGoto_Pal, GOTO_TAG_GLYPHS};

static const struct OamData sGoto_Oam_Glyph =
{
    .affineMode = ST_OAM_AFFINE_OFF,
    .objMode = ST_OAM_OBJ_NORMAL,
    .shape = SPRITE_SHAPE(8x8),
    .size = SPRITE_SIZE(8x8),
    .priority = 1,
};

static const struct OamData sGoto_Oam_Note =
{
    .affineMode = ST_OAM_AFFINE_OFF,
    .objMode = ST_OAM_OBJ_NORMAL,
    .shape = SPRITE_SHAPE(32x32),
    .size = SPRITE_SIZE(32x32),
    .priority = 1,
};

// One still frame each; the callback chooses between them
static const union AnimCmd sGoto_Anim_Frame0[] = {ANIMCMD_FRAME(0, 1), ANIMCMD_END};
static const union AnimCmd sGoto_Anim_Frame1[] = {ANIMCMD_FRAME(1, 1), ANIMCMD_END};
static const union AnimCmd sGoto_Anim_Frame2[] = {ANIMCMD_FRAME(2, 1), ANIMCMD_END};
static const union AnimCmd sGoto_Anim_Frame3[] = {ANIMCMD_FRAME(3, 1), ANIMCMD_END};
static const union AnimCmd sGoto_Anim_Note0[]  = {ANIMCMD_FRAME(0, 1), ANIMCMD_END};
static const union AnimCmd sGoto_Anim_Note1[]  = {ANIMCMD_FRAME(16, 1), ANIMCMD_END};
static const union AnimCmd sGoto_Anim_Note2[]  = {ANIMCMD_FRAME(32, 1), ANIMCMD_END};
static const union AnimCmd sGoto_Anim_Note3[]  = {ANIMCMD_FRAME(48, 1), ANIMCMD_END};

static const union AnimCmd *const sGoto_Anims_Glyph[] =
{
    sGoto_Anim_Frame0, // 0
    sGoto_Anim_Frame1, // 1
    sGoto_Anim_Frame2, // a note
    sGoto_Anim_Frame3, // a note lit
};

static const union AnimCmd *const sGoto_Anims_Note[] =
{
    sGoto_Anim_Note0,
    sGoto_Anim_Note1,
    sGoto_Anim_Note2,
    sGoto_Anim_Note3,
};

static void SpriteCB_GotoGlyph(struct Sprite *sprite);

static const struct SpriteTemplate sGoto_SpriteTemplate_Glyph =
{
    .tileTag = GOTO_TAG_GLYPHS,
    .paletteTag = GOTO_TAG_GLYPHS,
    .oam = &sGoto_Oam_Glyph,
    .anims = sGoto_Anims_Glyph,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCB_GotoGlyph,
};

static const struct SpriteTemplate sGoto_SpriteTemplate_Note =
{
    .tileTag = GOTO_TAG_NOTES,
    .paletteTag = GOTO_TAG_GLYPHS,
    .oam = &sGoto_Oam_Note,
    .anims = sGoto_Anims_Note,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCB_GotoGlyph,
};

// Sprite data for a glyph
#define sTaskId data[0]
#define sDelay  data[1] // frames behind the ribbon's head
#define sOffX   data[2] // where on the player a bit sits, or how far off the ribbon a note rides
#define sOffY   data[3]
#define sIsBit  data[4]
#define sMode   data[5]
#define sAppear data[6] // going: the frame a bit shows on the player; arriving: the reform frame it goes out
#define sAge    data[7]

// Where the ribbon is f frames after its head set out. Not the bird's ellipse: a parabola, steep where it falls from
// above the screen's corner and flat where it meets the player -- going, it drops from the top right in GOTO_OUT_MEET
// frames and climbs away to the top left in twice that, carrying; arriving, the other way round -- with a wave along it
// `amp` pixels deep. Each glyph's phase is its place in the ribbon, so the ripple runs down it.
static bool8 GotoRibbonAt(struct Task *task, s16 f, s16 phase, s16 amp, s16 *x, s16 *y)
{
    s32 n, k, w, h, meet = task->tLanding ? GOTO_IN_MEET : GOTO_OUT_MEET;
    if (f < 0)
        return FALSE;
    if (f < meet) // coming down to the player, from the right
    {
        n = meet;
        k = meet - f;
        w = task->tLanding ? 180 : 140;
        h = 120;
    }
    else          // and away from them, to the left
    {
        n = task->tLanding ? GOTO_OUT_MEET : GOTO_IN_MEET;
        k = f - meet;
        if (k > n)
            return FALSE;
        w = task->tLanding ? -140 : -180;
        h = 120;
    }
    *x = task->tCenterX + w * k / n;
    *y = task->tCenterY - h * k * k / (n * n) + Sin((u8)phase, amp);
    return TRUE;
}

static void SpriteCB_GotoGlyph(struct Sprite *sprite)
{
    struct Task *task = &gTasks[sprite->sTaskId];
    s16 f = task->tFrame - sprite->sDelay;
    s16 x, y, amp, offX, offY;

    sprite->sAge++;
    if (sprite->sIsBit)
    {
        if ((Random() & 3) == 0)
            StartSpriteAnim(sprite, Random() & 1); // the 0s and 1s flicker
    }
    else if (sprite->template == &sGoto_SpriteTemplate_Glyph && (sprite->sAge & 7) == 0)
    {
        StartSpriteAnim(sprite, sprite->animNum ^ 1); // the small notes twinkle, note to note lit
    }

    switch (sprite->sMode)
    {
    case GLYPH_ON_PLAYER:
        sprite->x = task->tCenterX + sprite->sOffX;
        sprite->y = task->tCenterY + sprite->sOffY;
        if (!task->tLanding)
        {
            sprite->invisible = task->tFrame < sprite->sAppear || (Random() & 3) == 0;
            if (f >= GOTO_OUT_MEET) // the ribbon has reached it: it goes with the music
            {
                sprite->sMode = GLYPH_RIBBON;
                sprite->sAge = 0;
            }
        }
        else
        {
            sprite->invisible = (Random() & 3) == 0;
            if (task->tReform > sprite->sAppear) // it is the player again
            {
                sprite->sMode = GLYPH_GONE;
                sprite->invisible = TRUE;
            }
        }
        break;
    case GLYPH_RIBBON:
        offX = sprite->sOffX;
        offY = sprite->sOffY;
        amp = GOTO_WAVE;
        if (sprite->sIsBit && !task->tLanding)
        {
            // leaving the player it is still where it was on them, and closes on the ribbon as the wave takes it
            s16 closing = 16 + max(0, 16 - sprite->sAge);
            offX = offX * closing / 32;
            offY = offY * closing / 32;
            amp = GOTO_WAVE * min(f - GOTO_OUT_MEET, 16) / 16;
        }
        else if (sprite->sIsBit && task->tLanding)
        {
            if (f >= GOTO_IN_MEET) // down: it settles where it will be part of the player
            {
                sprite->sMode = GLYPH_ON_PLAYER;
                sprite->x = task->tCenterX + offX;
                sprite->y = task->tCenterY + offY;
                break;
            }
            offX = offX * max(0, f) / GOTO_IN_MEET;
            offY = offY * max(0, f) / GOTO_IN_MEET;
            amp = GOTO_WAVE * (GOTO_IN_MEET - max(0, f)) / GOTO_IN_MEET;
        }
        if (!GotoRibbonAt(task, f, task->tFrame * 8 + sprite->sDelay * 24, amp, &x, &y))
        {
            sprite->invisible = TRUE;
            if (f > 0)
                sprite->sMode = GLYPH_GONE;
            break;
        }
        sprite->x = x + offX;
        sprite->y = y + offY;
        sprite->invisible = x < -32 || x > DISPLAY_WIDTH + 32 || y < -32 || y > DISPLAY_HEIGHT + 32
                         || (sprite->sIsBit && (Random() & 7) == 0);
        break;
    case GLYPH_GONE:
        sprite->invisible = TRUE;
        break;
    }
}

// The splash's sheets and palette, faded to the weather like any field effect's; FALSE when there was no room for them
static bool8 GotoLoadGlyphs(void)
{
    u8 paletteNum;
    LoadCompressedSpriteSheet(&sGoto_SpriteSheets[0]);
    LoadCompressedSpriteSheet(&sGoto_SpriteSheets[1]);
    paletteNum = LoadSpritePalette(&sGoto_SpritePalette);
    if (paletteNum == 0xFF)
        return FALSE;
    UpdateSpritePaletteWithWeather(paletteNum);
    return TRUE;
}

static void GotoSpawnGlyphs(u8 taskId)
{
    bool8 landing = gTasks[taskId].tLanding;
    u8 i, spriteId;
    struct Sprite *sprite;

    for (i = 0; i < GOTO_NUM_NOTES; i++)
    {
        bool8 big = (i % 3) == 0;
        spriteId = CreateSprite(big ? &sGoto_SpriteTemplate_Note : &sGoto_SpriteTemplate_Glyph, 0, -64, 0);
        if (spriteId == MAX_SPRITES)
            return;
        sprite = &gSprites[spriteId];
        sprite->sTaskId = taskId;
        sprite->sDelay = i * 2;
        sprite->sOffX = (Random() % 13) - 6;
        sprite->sOffY = (Random() % 13) - 6;
        sprite->sIsBit = FALSE;
        sprite->sMode = GLYPH_RIBBON;
        sprite->invisible = TRUE;
        StartSpriteAnim(sprite, big ? (i / 3) % 4 : 2 + (i & 1));
    }
    for (i = 0; i < GOTO_NUM_BITS; i++)
    {
        spriteId = CreateSprite(&sGoto_SpriteTemplate_Glyph, 0, -64, 0);
        if (spriteId == MAX_SPRITES)
            return;
        sprite = &gSprites[spriteId];
        sprite->sTaskId = taskId;
        sprite->sOffX = (Random() % 13) - 6;
        sprite->sOffY = (Random() % 25) - 10;
        sprite->sIsBit = TRUE;
        sprite->invisible = TRUE;
        StartSpriteAnim(sprite, Random() & 1);
        if (!landing)
        {
            sprite->sDelay = Random() % 11;
            sprite->sAppear = GOTO_OUT_MEET - GOTO_DISSOLVE + i * 2 + Random() % 3;
            sprite->sMode = GLYPH_ON_PLAYER;
        }
        else
        {
            sprite->sDelay = 4 + Random() % 13;
            sprite->sAppear = 1 + Random() % (GOTO_DISSOLVE - 1);
            sprite->sMode = GLYPH_RIBBON;
        }
    }
}

// Whether any glyph is still to be seen
static bool8 GotoGlyphsLeft(void)
{
    u8 i;
    for (i = 0; i < MAX_SPRITES; i++)
        if (gSprites[i].inUse && gSprites[i].callback == SpriteCB_GotoGlyph && gSprites[i].sMode != GLYPH_GONE)
            return TRUE;
    return FALSE;
}

static void GotoClearGlyphs(void)
{
    u8 i;
    for (i = 0; i < MAX_SPRITES; i++)
        if (gSprites[i].inUse && gSprites[i].callback == SpriteCB_GotoGlyph)
            DestroySprite(&gSprites[i]);
    FreeSpriteTilesByTag(GOTO_TAG_GLYPHS);
    FreeSpriteTilesByTag(GOTO_TAG_NOTES);
    FreeSpritePaletteByTag(GOTO_TAG_GLYPHS);
}

#undef sTaskId
#undef sDelay
#undef sOffX
#undef sOffY
#undef sIsBit
#undef sMode
#undef sAppear
#undef sAge

// The player, on screen: where the code comes apart from and settles back into
static void GotoFindPlayer(struct Task *task)
{
    struct Sprite *sprite = &gSprites[gPlayerAvatar.spriteId];
    task->tCenterX = sprite->x + sprite->x2 + gSpriteCoordOffsetX;
    task->tCenterY = sprite->y + sprite->y2 + gSpriteCoordOffsetY;
}

// Some of the player's frames shown, more of them as `shown` runs from 0 to GOTO_DISSOLVE
static void GotoFlickerPlayer(s16 shown)
{
    gObjectEvents[gPlayerAvatar.objectEventId].invisible = (s16)(Random() % GOTO_DISSOLVE) >= shown;
}

static void Task_FlyOut(u8 taskId);
static void FlyOutFieldEffect_FieldMovePose(struct Task *task);
static void FlyOutFieldEffect_ShowMon(struct Task *task);
static void FlyOutFieldEffect_ComeApart(struct Task *task);
static void FlyOutFieldEffect_Ribbon(struct Task *task);
static void FlyOutFieldEffect_End(struct Task *task);

static void (*const sFlyOutFieldEffectFuncs[])(struct Task *) =
{
    FlyOutFieldEffect_FieldMovePose,
    FlyOutFieldEffect_ShowMon,
    FlyOutFieldEffect_ComeApart,
    FlyOutFieldEffect_Ribbon,
    FlyOutFieldEffect_End
};

u8 FldEff_FlyOut(void)
{
    u8 taskId = CreateTask(Task_FlyOut, 0xFE);
    gTasks[taskId].tMonPartyId = gFieldEffectArguments[0];
    return 0;
}

static void Task_FlyOut(u8 taskId)
{
    sFlyOutFieldEffectFuncs[gTasks[taskId].tState](&gTasks[taskId]);
}

static void FlyOutFieldEffect_FieldMovePose(struct Task *task)
{
    struct ObjectEvent *objectEvent = &gObjectEvents[gPlayerAvatar.objectEventId];
    if (!ObjectEventIsMovementOverridden(objectEvent) || ObjectEventClearHeldMovementIfFinished(objectEvent))
    {
        task->tAvatarFlags = gPlayerAvatar.flags;
        gPlayerAvatar.preventStep = TRUE;
        SetPlayerAvatarStateMask(PLAYER_AVATAR_FLAG_ON_FOOT);
        StartPlayerAvatarSummonMonForFieldMoveAnim();
        ObjectEventSetHeldMovement(objectEvent, MOVEMENT_ACTION_START_ANIM_IN_DIRECTION);
        task->tState++;
    }
}

static void FlyOutFieldEffect_ShowMon(struct Task *task)
{
    struct ObjectEvent *objectEvent = &gObjectEvents[gPlayerAvatar.objectEventId];
    if (ObjectEventClearHeldMovementIfFinished(objectEvent))
    {
        task->tState++;
        gFieldEffectArguments[0] = task->tMonPartyId;
        FieldEffectStart(FLDEFF_FIELD_MOVE_SHOW_MON_INIT);
    }
}

// The daemon has been shown: the player begins to come apart into code
static void FlyOutFieldEffect_ComeApart(struct Task *task)
{
    if (!FieldEffectActiveListContains(FLDEFF_FIELD_MOVE_SHOW_MON))
    {
        struct ObjectEvent *objectEvent = &gObjectEvents[gPlayerAvatar.objectEventId];
        if (task->tAvatarFlags & PLAYER_AVATAR_FLAG_SURFING)
        {
            SetSurfBlob_BobState(objectEvent->fieldEffectSpriteId, BOB_MON_ONLY);
            SetSurfBlob_DontSyncAnim(objectEvent->fieldEffectSpriteId, FALSE);
        }
        SetPlayerAvatarTransitionFlags(PLAYER_AVATAR_FLAG_ON_FOOT);
        GotoFindPlayer(task);
        task->tLanding = FALSE;
        task->tFrame = GOTO_OUT_MEET - GOTO_DISSOLVE;
        if (GotoLoadGlyphs())
            GotoSpawnGlyphs(FindTaskIdByFunc(Task_FlyOut));
        PlaySE(SE_M_TELEPORT);
        task->tState++;
    }
}

// The notes come down the swoop; where they meet the player there is only code left, and it goes with them
static void FlyOutFieldEffect_Ribbon(struct Task *task)
{
    struct ObjectEvent *objectEvent = &gObjectEvents[gPlayerAvatar.objectEventId];
    task->tFrame++;
    if (task->tFrame == 0)
        PlaySE(SE_M_HEAL_BELL);
    if (task->tFrame < GOTO_OUT_MEET)
    {
        GotoFlickerPlayer(GOTO_OUT_MEET - task->tFrame);
    }
    else if (task->tFrame == GOTO_OUT_MEET)
    {
        objectEvent->invisible = TRUE;
        objectEvent->hasShadow = FALSE;
    }
    else if (task->tFrame >= GOTO_OUT_MEET + 40 || !GotoGlyphsLeft())
    {
        WarpFadeOutScreen();
        task->tState++;
    }
}

static void FlyOutFieldEffect_End(struct Task *task)
{
    task->tFrame++;
    if (!gPaletteFade.active)
    {
        GotoClearGlyphs();
        FieldEffectActiveListRemove(FLDEFF_FLY_OUT);
        DestroyTask(FindTaskIdByFunc(Task_FlyOut));
    }
}

static void Task_FlyIn(u8 taskId);
static void FlyInFieldEffect_Begin(struct Task *task);
static void FlyInFieldEffect_Ribbon(struct Task *task);
static void FlyInFieldEffect_Reform(struct Task *task);
static void FlyInFieldEffect_End(struct Task *task);

static void (*const sFlyInFieldEffectFuncs[])(struct Task *task) =
{
    FlyInFieldEffect_Begin,
    FlyInFieldEffect_Ribbon,
    FlyInFieldEffect_Reform,
    FlyInFieldEffect_End
};

u32 FldEff_FlyIn(void)
{
    CreateTask(Task_FlyIn, 0xFE);
    return 0;
}

static void Task_FlyIn(u8 taskId)
{
    sFlyInFieldEffectFuncs[gTasks[taskId].tState](&gTasks[taskId]);
}

// The ribbon sets out for where the player will stand; the player is not there yet
static void FlyInFieldEffect_Begin(struct Task *task)
{
    struct ObjectEvent *playerObj = &gObjectEvents[gPlayerAvatar.objectEventId];
    if (!ObjectEventIsMovementOverridden(playerObj) || ObjectEventClearHeldMovementIfFinished(playerObj))
    {
        task->tAvatarFlags = gPlayerAvatar.flags;
        gPlayerAvatar.preventStep = TRUE;
        SetPlayerAvatarStateMask(PLAYER_AVATAR_FLAG_ON_FOOT);
        if (task->tAvatarFlags & PLAYER_AVATAR_FLAG_SURFING)
            SetSurfBlob_BobState(playerObj->fieldEffectSpriteId, BOB_NONE);
        ObjectEventTurn(playerObj, DIR_SOUTH);
        playerObj->invisible = TRUE;
        GotoFindPlayer(task);
        task->tLanding = TRUE;
        task->tFrame = 0;
        task->tReform = 0;
        if (GotoLoadGlyphs())
            GotoSpawnGlyphs(FindTaskIdByFunc(Task_FlyIn));
        PlaySE(SE_M_HEAL_BELL);
        task->tState++;
    }
}

// The code comes down with the music and settles where the player stands
static void FlyInFieldEffect_Ribbon(struct Task *task)
{
    task->tFrame++;
    if (task->tFrame >= GOTO_IN_MEET + 16) // every bit is down (the last set out 16 frames behind the head)
    {
        PlaySE(SE_M_TELEPORT);
        task->tState++;
    }
}

// ... and flickers back into them, while the notes go on
static void FlyInFieldEffect_Reform(struct Task *task)
{
    task->tFrame++;
    task->tReform++;
    GotoFlickerPlayer(task->tReform);
    if (task->tReform >= GOTO_DISSOLVE)
    {
        gObjectEvents[gPlayerAvatar.objectEventId].invisible = FALSE;
        task->tState++;
    }
}

static void FlyInFieldEffect_End(struct Task *task)
{
    struct ObjectEvent *playerObj;
    u8 state;
    task->tFrame++;
    if (!GotoGlyphsLeft())
    {
        GotoClearGlyphs();
        playerObj = &gObjectEvents[gPlayerAvatar.objectEventId];
        state = PLAYER_AVATAR_GFX_NORMAL;
        if (task->tAvatarFlags & PLAYER_AVATAR_FLAG_SURFING)
        {
            state = PLAYER_AVATAR_GFX_RIDE;
            SetSurfBlob_BobState(playerObj->fieldEffectSpriteId, BOB_PLAYER_AND_MON);
        }
        ObjectEventSetGraphicsId(playerObj, GetPlayerAvatarGraphicsIdByStateId(state));
        ObjectEventTurn(playerObj, DIR_SOUTH);
        gPlayerAvatar.flags = task->tAvatarFlags;
        gPlayerAvatar.preventStep = FALSE;
        FieldEffectActiveListRemove(FLDEFF_FLY_IN);
        DestroyTask(FindTaskIdByFunc(Task_FlyIn));
    }
}

#undef tState
#undef tMonPartyId
#undef tTimer
#undef tFrame
#undef tCenterX
#undef tCenterY
#undef tLanding
#undef tReform
#undef tAvatarFlags

static void Task_MoveDeoxysRock_Step(u8 taskId);

u32 FldEff_MoveDeoxysRock(void)
{
    u8 taskId;
    u8 objectEventIdBuffer;
    s32 x;
    s32 y;
    struct ObjectEvent * objectEvent;
    if (!TryGetObjectEventIdByLocalIdAndMap(gFieldEffectArguments[0], gFieldEffectArguments[1], gFieldEffectArguments[2], &objectEventIdBuffer))
    {
        objectEvent = &gObjectEvents[objectEventIdBuffer];
        x = objectEvent->currentCoords.x - 7;
        y = objectEvent->currentCoords.y - 7;
        x = (gFieldEffectArguments[3] - x) * 16;
        y = (gFieldEffectArguments[4] - y) * 16;
        ShiftObjectEventCoords(objectEvent, gFieldEffectArguments[3] + 7, gFieldEffectArguments[4] + 7);
        taskId = CreateTask(Task_MoveDeoxysRock_Step, 0x50);
        gTasks[taskId].data[1] = objectEvent->spriteId;
        gTasks[taskId].data[2] = gSprites[objectEvent->spriteId].x + x;
        gTasks[taskId].data[3] = gSprites[objectEvent->spriteId].y + y;
        gTasks[taskId].data[8] = gFieldEffectArguments[5];
        gTasks[taskId].data[9] = objectEventIdBuffer;
    }
    return FALSE;
}

static void Task_MoveDeoxysRock_Step(u8 taskId)
{
    s16 *data = gTasks[taskId].data;
    struct Sprite *sprite = &gSprites[data[1]];
    struct ObjectEvent * objectEvent;
    switch (data[0])
    {
    case 0:
        data[4] = sprite->x << 4;
        data[5] = sprite->y << 4;

        // UB: Possible divide by zero
        data[6] = SAFE_DIV(((data[2] << 4) - data[4]), data[8]);
        data[7] = SAFE_DIV(((data[3] << 4) - data[5]), data[8]);
        data[0]++;
        // fallthrough
    case 1:
        if (data[8] != 0)
        {
            data[8]--;
            data[4] += data[6];
            data[5] += data[7];
            sprite->x = data[4] >> 4;
            sprite->y = data[5] >> 4;
        }
        else
        {
            objectEvent = &gObjectEvents[data[9]];
            sprite->x = data[2];
            sprite->y = data[3];
            ShiftStillObjectEventCoords(objectEvent);
            objectEvent->triggerGroundEffectsOnStop = TRUE;
            FieldEffectActiveListRemove(FLDEFF_MOVE_DEOXYS_ROCK);
            DestroyTask(taskId);
        }
        break;
    }
}

static void Task_DestroyDeoxysRock(u8 taskId);
static void DestroyDeoxysRockEffect_CameraShake(s16 *data, u8 taskId);
static void DestroyDeoxysRockEffect_RockFragments(s16 *data, u8 taskId);
static void DestroyDeoxysRockEffect_WaitAndEnd(s16 *data, u8 taskId);
static void CreateDeoxysRockFragments(struct Sprite *sprite);
static void SpriteCB_DeoxysRockFragment(struct Sprite *sprite);

static void (*const sDestroyDeoxysRockEffectFuncs[])(s16 *data, u8 taskId) =
{
    DestroyDeoxysRockEffect_CameraShake,
    DestroyDeoxysRockEffect_RockFragments,
    DestroyDeoxysRockEffect_WaitAndEnd
};

static const struct SpriteFrameImage sImages_DeoxysRockFragment[] =
{
    {sRockFragment_TopLeft, 0x20},
    {sRockFragment_TopRight, 0x20},
    {sRockFragment_BottomLeft, 0x20},
    {sRockFragment_BottomRight, 0x20}
};

static const union AnimCmd sAnim_RockFragment_TopLeft[] =
{
    ANIMCMD_FRAME(0, 0),
    ANIMCMD_END
};

static const union AnimCmd sAnim_RockFragment_TopRight[] =
{
    ANIMCMD_FRAME(1, 0),
    ANIMCMD_END
};

static const union AnimCmd sAnim_RockFragment_BottomLeft[] =
{
    ANIMCMD_FRAME(2, 0),
    ANIMCMD_END
};

static const union AnimCmd sAnim_RockFragment_BottomRight[] =
{
    ANIMCMD_FRAME(3, 0),
    ANIMCMD_END
};

static const union AnimCmd *const sAnims_DeoxysRockFragment[] =
{
    sAnim_RockFragment_TopLeft,
    sAnim_RockFragment_TopRight,
    sAnim_RockFragment_BottomLeft,
    sAnim_RockFragment_BottomRight
};

static const struct SpriteTemplate sSpriteTemplate_DeoxysRockFragment =
{
    .tileTag = TAG_NONE,
    .paletteTag = 4371,
    .oam = &sOamData_8x8,
    .anims = sAnims_DeoxysRockFragment,
    .images = sImages_DeoxysRockFragment,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCB_DeoxysRockFragment
};

// Task data for Task_DestroyDeoxysRock
#define tState         data[1]
#define tObjectEventId data[2]
#define tTimer         data[3]
#define tCameraTaskId  data[5]
#define tLocalId       data[6]
#define tMapNum        data[7]
#define tMapGroup      data[8]

u32 FldEff_DestroyDeoxysRock(void)
{
    u8 taskId;
    u8 objectEventId;
    if (!TryGetObjectEventIdByLocalIdAndMap(gFieldEffectArguments[0], gFieldEffectArguments[1], gFieldEffectArguments[2], &objectEventId))
    {
        taskId = CreateTask(Task_DestroyDeoxysRock, 80);
        gTasks[taskId].data[2] = objectEventId;
        gTasks[taskId].tLocalId = gFieldEffectArguments[0];
        gTasks[taskId].tMapNum = gFieldEffectArguments[1];
        gTasks[taskId].tMapGroup = gFieldEffectArguments[2];
    }
    else
        FieldEffectActiveListRemove(FLDEFF_DESTROY_DEOXYS_ROCK);

    return FALSE;
}

// Task data for Task_DeoxysRockCameraShake
#define tShakeDelay data[0]
#define tShakeUp    data[1]
#define tShake      data[5]
#define tEndDelay   data[6]
#define tEnding     data[7]

static void Task_DeoxysRockCameraShake(u8 taskId)
{
    s16 *data = gTasks[taskId].data;
    if (data[7] != 0)
    {
        if (++data[6] > 20)
        {
            data[6] = 0;
            if (data[5] != 0)
                data[5]--;
        }
    }
    else
        data[5] = 4;

    if (++data[0] > 1)
    {
        data[0] = 0;
        if (++data[1] & 1)
            SetCameraPanning(0, -data[5]);
        else
            SetCameraPanning(0, data[5]);
    }
    UpdateCameraPanning();
    if (data[5] == 0)
        DestroyTask(taskId);
}

static void StartEndingDeoxysRockCameraShake(u8 taskId)
{
    gTasks[taskId].data[7] = 1;
}

#undef tShakeDelay
#undef tShakeUp
#undef tShake
#undef tEndDelay
#undef tEnding

static void Task_DestroyDeoxysRock(u8 taskId)
{
    s16 *data = gTasks[taskId].data;
    InstallCameraPanAheadCallback();
    SetCameraPanningCallback(NULL);
    sDestroyDeoxysRockEffectFuncs[tState](data, taskId);
}

static void DestroyDeoxysRockEffect_CameraShake(s16 *data, u8 taskId)
{
    u8 newTaskId = CreateTask(Task_DeoxysRockCameraShake, 90);
    PlaySE(SE_THUNDER2);
    tCameraTaskId = newTaskId;
    tState++;
}

static void DestroyDeoxysRockEffect_RockFragments(s16 *data, u8 taskId)
{
    if (++tTimer > 120)
    {
        struct Sprite *sprite = &gSprites[gObjectEvents[tObjectEventId].spriteId];
        gObjectEvents[tObjectEventId].invisible = TRUE;
        BlendPalettes(PALETTES_BG, 0x10, RGB_WHITE);
        BeginNormalPaletteFade(PALETTES_BG, 0, 0x10, 0, RGB_WHITE);
        CreateDeoxysRockFragments(sprite);
        PlaySE(SE_THUNDER);
        StartEndingDeoxysRockCameraShake(tCameraTaskId);
        tTimer = 0;
        tState++;
    }
}

static void DestroyDeoxysRockEffect_WaitAndEnd(s16 *data, u8 taskId)
{
    if (!gPaletteFade.active && !FuncIsActiveTask(Task_DeoxysRockCameraShake))
    {
        InstallCameraPanAheadCallback();
        RemoveObjectEventByLocalIdAndMap(tLocalId, tMapNum, tMapGroup);
        FieldEffectActiveListRemove(FLDEFF_DESTROY_DEOXYS_ROCK);
        DestroyTask(taskId);
    }
}

#undef tState
#undef tObjectEventId
#undef tTimer
#undef tCameraTaskId
#undef tLocalId
#undef tMapNum
#undef tMapGroup

static void CreateDeoxysRockFragments(struct Sprite *sprite)
{
    int i;
    int xPos = (s16)gTotalCameraPixelOffsetX + sprite->x + sprite->x2;
    int yPos = (s16)gTotalCameraPixelOffsetY + sprite->y + sprite->y2 - 4;

    for (i = 0; i < 4; i++)
    {
        u8 spriteId = CreateSprite(&sSpriteTemplate_DeoxysRockFragment, xPos, yPos, 0);
        if (spriteId != MAX_SPRITES)
        {
            StartSpriteAnim(&gSprites[spriteId], i);
            gSprites[spriteId].data[0] = i;
            gSprites[spriteId].oam.paletteNum = sprite->oam.paletteNum;
        }
    }
}

static void SpriteCB_DeoxysRockFragment(struct Sprite *sprite)
{
    switch (sprite->data[0])
    {
    case 0:
        sprite->x -= 16;
        sprite->y -= 12;
        break;
    case 1:
        sprite->x += 16;
        sprite->y -= 12;
        break;
    case 2:
        sprite->x -= 16;
        sprite->y += 12;
        break;
    case 3:
        sprite->x += 16;
        sprite->y += 12;
        break;
    }
    if (sprite->x < -4 || sprite->x > DISPLAY_WIDTH + 4 || sprite->y < -4 || sprite->y > DISPLAY_HEIGHT + 4)
        DestroySprite(sprite);
}

static void Task_PhotoFlash(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        FieldEffectActiveListRemove(FLDEFF_PHOTO_FLASH);
        DestroyTask(taskId);
    }
}

// Bug: Return value should be u32, not void
void FldEff_PhotoFlash(void)
{
    BlendPalettes(PALETTES_ALL, 0x10, RGB_WHITE);
    BeginNormalPaletteFade(PALETTES_ALL, -1, 0x0F, 0x00, RGB_WHITE);
    CreateTask(Task_PhotoFlash, 90);
}
