#include "global.h"
#include "gflib.h"
#include "decompress.h"
#include "data.h"

struct PicData
{
    u8 *frames;
    struct SpriteFrameImage *images;
    u16 paletteTag;
    u8 spriteId;
    u8 active;
};

#define PICS_COUNT 8

static EWRAM_DATA struct SpriteTemplate sCreatingSpriteTemplate = {};
static EWRAM_DATA struct PicData sSpritePics[PICS_COUNT] = {};

static const struct PicData sDummyPicData = {};

static const struct OamData sOamData_Normal =
{
    .shape = SPRITE_SHAPE(64x64),
    .size = SPRITE_SIZE(64x64)
};

void DummyPicSpriteCallback(struct Sprite *sprite)
{

}

bool16 ResetAllPicSprites(void)
{
    int i;

    for (i = 0; i < PICS_COUNT; i ++)
        sSpritePics[i] = sDummyPicData;

    return FALSE;
}

static bool16 DecompressPic(u16 species, u32 personality, bool8 isFrontPic, u8 *dest, bool8 isTrainer, bool8 ignoreDeoxys)
{
    if (!isTrainer)
    {
        if (isFrontPic)
        {
            if (!ignoreDeoxys)
                LoadSpecialPokePic(&gMonFrontPicTable[species], dest, species, personality, isFrontPic);
            else
                LoadSpecialPokePic_DontHandleDeoxys(&gMonFrontPicTable[species], dest, species, personality, isFrontPic);
        }
        else
        {
            if (!ignoreDeoxys)
                LoadSpecialPokePic(&gMonBackPicTable[species], dest, species, personality, isFrontPic);
            else
                LoadSpecialPokePic_DontHandleDeoxys(&gMonBackPicTable[species], dest, species, personality, isFrontPic);
        }
    }
    else
    {
        if (isFrontPic)
            DecompressPicFromTable(&gTrainerFrontPicTable[species], dest, species);
        else
            DecompressPicFromTable(&gTrainerBackPicTable[species], dest, species);
    }
    return FALSE;
}

static bool16 DecompressPic_HandleDeoxys(u16 species, u32 personality, bool8 isFrontPic, u8 *dest, bool8 isTrainer)
{
    return DecompressPic(species, personality, isFrontPic, dest, isTrainer, FALSE);
}

void LoadPicPaletteByTagOrSlot(u16 species, u32 otId, u32 personality, u8 paletteSlot, u16 paletteTag, bool8 isTrainer)
{
    if (!isTrainer)
    {
        if (paletteTag == TAG_NONE)
        {
            sCreatingSpriteTemplate.paletteTag = TAG_NONE;
            LoadCompressedPalette(GetMonSpritePalFromSpeciesAndPersonality(species, otId, personality), OBJ_PLTT_ID(paletteSlot), PLTT_SIZE_4BPP);
        }
        else
        {
            sCreatingSpriteTemplate.paletteTag = paletteTag;
            LoadCompressedSpritePalette(GetMonSpritePalStructFromOtIdPersonality(species, otId, personality));
        }
    }
    else
    {
        if (paletteTag == TAG_NONE)
        {
            sCreatingSpriteTemplate.paletteTag = TAG_NONE;
            LoadCompressedPalette(gTrainerFrontPicPaletteTable[species].data, OBJ_PLTT_ID(paletteSlot), PLTT_SIZE_4BPP);
        }
        else
        {
            sCreatingSpriteTemplate.paletteTag = paletteTag;
            LoadCompressedSpritePalette(&gTrainerFrontPicPaletteTable[species]);
        }
    }
}

void LoadPicPaletteBySlot(u16 species, u32 otId, u32 personality, u8 paletteSlot, bool8 isTrainer)
{
    if (!isTrainer)
        LoadCompressedPalette(GetMonSpritePalFromSpeciesAndPersonality(species, otId, personality), BG_PLTT_ID(paletteSlot), PLTT_SIZE_4BPP);
    else
        LoadCompressedPalette(gTrainerFrontPicPaletteTable[species].data, BG_PLTT_ID(paletteSlot), PLTT_SIZE_4BPP);
}

void AssignSpriteAnimsTable(bool8 isTrainer)
{
    if (!isTrainer)
        sCreatingSpriteTemplate.anims = gAnims_MonPic;
    else
        sCreatingSpriteTemplate.anims = gTrainerFrontAnimsPtrTable[0];
}

u16 CreatePicSprite(u16 species, u32 otId, u32 personality, bool8 isFrontPic, s16 x, s16 y, u8 paletteSlot, u16 paletteTag, bool8 isTrainer, bool8 ignoreDeoxys)
{
    u8 i;
    u8 *framePics;
    struct SpriteFrameImage *images;
    int j;
    u8 spriteId;

    for (i = 0; i < PICS_COUNT; i ++)
    {
        if (!sSpritePics[i].active)
            break;
    }
    if (i == PICS_COUNT)
        return 0xFFFF;

    framePics = Alloc(4 * 0x800);
    if (!framePics)
        return 0xFFFF;

    images = Alloc(4 * sizeof(struct SpriteFrameImage));
    if (!images)
    {
        Free(framePics);
        return 0xFFFF;
    }
    if (DecompressPic(species, personality, isFrontPic, framePics, isTrainer, ignoreDeoxys))
    {
        // debug trap?
        return 0xFFFF;
    }
    for (j = 0; j < 4; j ++)
    {
        images[j].data = framePics + 0x800 * j;
        images[j].size = 0x800;
    }
    sCreatingSpriteTemplate.tileTag = TAG_NONE;
    sCreatingSpriteTemplate.oam = &sOamData_Normal;
    AssignSpriteAnimsTable(isTrainer);
    sCreatingSpriteTemplate.images = images;
    sCreatingSpriteTemplate.affineAnims = gDummySpriteAffineAnimTable;
    sCreatingSpriteTemplate.callback = DummyPicSpriteCallback;
    LoadPicPaletteByTagOrSlot(species, otId, personality, paletteSlot, paletteTag, isTrainer);
    spriteId = CreateSprite(&sCreatingSpriteTemplate, x, y, 0);
    if (paletteTag == TAG_NONE)
        gSprites[spriteId].oam.paletteNum = paletteSlot;
    sSpritePics[i].frames = framePics;
    sSpritePics[i].images = images;
    sSpritePics[i].paletteTag = paletteTag;
    sSpritePics[i].spriteId = spriteId;
    sSpritePics[i].active = TRUE;
    return spriteId;
}

u16 CreatePicSprite_HandleDeoxys(u16 species, u32 otId, u32 personality, bool8 isFrontPic, s16 x, s16 y, u8 paletteSlot, u16 paletteTag, bool8 isTrainer)
{
    return CreatePicSprite(species, otId, personality, isFrontPic, x, y, paletteSlot, paletteTag, isTrainer, FALSE);
}

u16 FreeAndDestroyPicSpriteInternal(u16 spriteId)
{
    u8 i;
    u8 *framePics;
    struct SpriteFrameImage *images;

    for (i = 0; i < PICS_COUNT; i ++)
    {
        if (sSpritePics[i].spriteId == spriteId)
            break;
    }
    if (i == PICS_COUNT)
        return 0xFFFF;

    framePics = sSpritePics[i].frames;
    images = sSpritePics[i].images;
    if (sSpritePics[i].paletteTag != TAG_NONE)
        FreeSpritePaletteByTag(GetSpritePaletteTagByPaletteNum(gSprites[spriteId].oam.paletteNum));
    DestroySprite(&gSprites[spriteId]);
    Free(framePics);
    Free(images);
    sSpritePics[i] = sDummyPicData;
    return 0;
}

static u16 LoadPicSpriteInWindow(u16 species, u32 otId, u32 personality, bool8 isFrontPic, u8 paletteSlot, u8 windowId, bool8 isTrainer)
{
    if (DecompressPic_HandleDeoxys(species, personality, isFrontPic, (u8 *)GetWindowAttribute(windowId, WINDOW_TILE_DATA), FALSE))
        return 0xFFFF;

    LoadPicPaletteBySlot(species, otId, personality, paletteSlot, isTrainer);
    return 0;
}

//  T-318 (the user, 2026-09-27): EACH VIRTUE RESTORES ITS PART OF THE AVATAR. The USER card's portrait is cut into
//  seven bands, the feet to the crown -- the week's order, root to crown -- and a band whose virtue has not arrived
//  is drawn one shade down: every colour moved to the nearest darker colour of the portrait's own sixteen. Nothing
//  new is drawn and no colour is added; the light comes up from the feet as the MARKS are earned. Bit k of the mask
//  is band k counted from the feet; trainer_card.c sets it for the player's own card and clears it after.
u8 gTrainerCardPicShade;

#define PIC_PIXEL(buf, x, y) (&(buf)[(((y) / 8) * 8 + (x) / 8) * 32 + ((y) % 8) * 4 + ((x) % 8) / 2])

static u16 ColourLuma(u16 c)
{
    return (c & 0x1F) * 3 + ((c >> 5) & 0x1F) * 6 + ((c >> 10) & 0x1F);
}

static void ShadePicBands(u8 *pic, const u16 *pal)
{
    u8 down[16];
    s32 i, j, x, y, top = -1, bottom = -1, height;

    //  One shade down: the colour nearest to this one at seven-tenths its brightness, and darker than it.
    for (i = 0; i < 16; i++)
    {
        s32 r = (pal[i] & 0x1F) * 7 / 10, g = ((pal[i] >> 5) & 0x1F) * 7 / 10, b = ((pal[i] >> 10) & 0x1F) * 7 / 10;
        s32 best = 0x7FFFFFFF;
        down[i] = i;
        if (i == 0)
            continue;
        for (j = 1; j < 16; j++)
        {
            s32 dr = (pal[j] & 0x1F) - r, dg = ((pal[j] >> 5) & 0x1F) - g, db = ((pal[j] >> 10) & 0x1F) - b;
            s32 d = dr * dr + dg * dg + db * db;
            if (ColourLuma(pal[j]) < ColourLuma(pal[i]) && d < best)
                best = d, down[i] = j;
        }
    }
    for (y = 0; y < 64; y++)
        for (x = 0; x < 64; x++)
            if (*PIC_PIXEL(pic, x, y) & (x & 1 ? 0xF0 : 0x0F))
            {
                if (top < 0)
                    top = y;
                bottom = y;
            }
    if (top < 0)
        return;
    height = bottom - top + 1;
    for (y = top; y <= bottom; y++)
    {
        if (!(gTrainerCardPicShade & (1 << ((bottom - y) * 7 / height))))
            continue;
        for (x = 0; x < 64; x++)
        {
            u8 *p = PIC_PIXEL(pic, x, y);
            if (x & 1)
                *p = (*p & 0x0F) | (down[*p >> 4] << 4);
            else
                *p = (*p & 0xF0) | down[*p & 0x0F];
        }
    }
}

//  T-317 (the user, 2026-09-27: "yes" to the daemons sharpening too): THE INDEX'S PICTURES COME CLEAR WITH THE MAP.
//  While understandings are still to come a daemon's picture in the INDEX is drawn a little blocky -- every block of
//  gDexPicMosaicW by gDexPicMosaicH pixels takes its top-left pixel, the GBA's mosaic done in software so the page's
//  text on the same layer stays sharp. region_map.c's rule sets the size; pokedex_screen.c sets it and clears it.
u8 gDexPicMosaicW, gDexPicMosaicH;

static void MosaicPic(u8 *pic)
{
    s32 x, y;

    for (y = 0; y < 64; y++)
        for (x = 0; x < 64; x++)
        {
            s32 sx = x - x % gDexPicMosaicW, sy = y - y % gDexPicMosaicH;
            u8 v = *PIC_PIXEL(pic, sx, sy);
            u8 *p = PIC_PIXEL(pic, x, y);
            v = (sx & 1) ? v >> 4 : v & 0x0F;
            *p = (x & 1) ? (*p & 0x0F) | (v << 4) : (*p & 0xF0) | v;
        }
}

u16 CreateTrainerCardSprite(u16 species, u32 otId, u32 personality, bool8 isFrontPic, u16 destX, u16 destY, u8 paletteSlot, u8 windowId, bool8 isTrainer)
{
    u8 *framePics;

    framePics = Alloc(4 * 0x800);
    if (framePics && !DecompressPic_HandleDeoxys(species, personality, isFrontPic, framePics, isTrainer))
    {
        if (isTrainer && gTrainerCardPicShade)
        {
            LoadPicPaletteBySlot(species, otId, personality, paletteSlot, isTrainer);
            ShadePicBands(framePics, &gPlttBufferUnfaded[paletteSlot * 16]);
        }
        if (!isTrainer && gDexPicMosaicW && gDexPicMosaicH && gDexPicMosaicW * gDexPicMosaicH > 1)
            MosaicPic(framePics);
        BlitBitmapRectToWindow(windowId, framePics, 0, 0, 0x40, 0x40, destX, destY, 0x40, 0x40);
        LoadPicPaletteBySlot(species, otId, personality, paletteSlot, isTrainer);
        Free(framePics);
        return 0;
    }
    return 0xFFFF;
}

u16 CreateMonPicSprite(u16 species, u32 otId, u32 personality, bool8 isFrontPic, s16 x, s16 y, u8 paletteSlot, u16 paletteTag, bool8 ignoreDeoxys)
{
    return CreatePicSprite(species, otId, personality, isFrontPic, x, y, paletteSlot, paletteTag, FALSE, ignoreDeoxys);
}

u16 CreateMonPicSprite_HandleDeoxys(u16 species, u32 otId, u32 personality, bool8 isFrontPic, s16 x, s16 y, u8 paletteSlot, u16 paletteTag)
{
    return CreateMonPicSprite(species, otId, personality, isFrontPic, x, y, paletteSlot, paletteTag, FALSE);
}

u16 FreeAndDestroyMonPicSprite(u16 spriteId)
{
    return FreeAndDestroyPicSpriteInternal(spriteId);
}

u16 LoadMonPicInWindow(u16 species, u32 otId, u32 personality, bool8 isFrontPic, u8 paletteSlot, u8 windowId)
{
    return CreateTrainerCardSprite(species, otId, personality, isFrontPic, 0, 0, paletteSlot, windowId, FALSE);
}

u16 CreateTrainerCardMonIconSprite(u16 species, u32 otId, u32 personality, bool8 isFrontPic, u16 destX, u16 destY, u8 paletteSlot, u8 windowId)
{
    return CreateTrainerCardSprite(species, otId, personality, isFrontPic, destX, destY, paletteSlot, windowId, FALSE);
}

u16 CreateTrainerPicSprite(u16 species, bool8 isFrontPic, s16 x, s16 y, u8 paletteSlot, u16 paletteTag)
{
    return CreatePicSprite_HandleDeoxys(species, 0, 0, isFrontPic, x, y, paletteSlot, paletteTag, TRUE);
}

u16 FreeAndDestroyTrainerPicSprite(u16 spriteId)
{
    return FreeAndDestroyPicSpriteInternal(spriteId);
}

u16 LoadTrainerPicInWindow(u16 species, bool8 isFrontPic, u8 paletteSlot, u8 windowId)
{
    return LoadPicSpriteInWindow(species, 0, 0, isFrontPic, paletteSlot, windowId, TRUE);
}

u16 CreateTrainerCardTrainerPicSprite(u16 species, bool8 isFrontPic, u16 destX, u16 destY, u8 paletteSlot, u8 windowId)
{
    return CreateTrainerCardSprite(species, 0, 0, isFrontPic, destX, destY, paletteSlot, windowId, TRUE);
}

u16 PlayerGenderToFrontTrainerPicId(u8 gender, bool8 getClass)
{
    if (getClass == TRUE)
    {
        if (gender != MALE)
            return gFacilityClassToPicIndex[FACILITY_CLASS_LEAF];
        else
            return gFacilityClassToPicIndex[FACILITY_CLASS_RED];
    }
    return gender;
}
