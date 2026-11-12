#include "global.h"
#include "main.h"
#include "palette.h"
#include "event_data.h"
#include "overworld.h"
#include "fieldmap.h"
#include "battle.h"
#include "battle_main.h"
#include "constants/maps.h"
#include "constants/map_types.h"
#include "constants/flags.h"
#include "pokemon.h"
#include "random.h"
#include "sprite.h"
#include "trig.h"
#include "event_object_movement.h"
#include "constants/event_objects.h"
#include "daemons_time.h"
#include "daemons_pulse.h"
#include "daemons_battle_bg.h"
#include "daemons_aurora.h"

// T-528 PART 1: THE AURORA, THE DAWN THAT COMES AT NIGHT (the user, 2026-11-07, by multiple choice; vision.md 4.40).
// "Once the unity book is restored it creates some effect surrounding it, and that illumination goes further."
//
// ONE ATOM, TWO COLOURS. Both are atomic oxygen: CONTENT's sky glows in the high red line (630.0 nm), CONTEXT's in the
// common green (557.7 nm). The colour says which cartridge is looking, and nothing else (invariant 5); north and south
// are the map's -- the mainland the north wind's side, the Sevii islands the south wind's -- in both editions.
//
// WHERE, in the order it was built:
//   (a) OVER DAEMON TOWER, every night once the book is mended: the tower's own pale stones (LAVENDER's palette row 7,
//       which in the town only the tower draws) breathe toward the edition's line.
//   (b) IN THE GROVES' BATTLE SKIES, on the light's nights, in a grove whose light is on: a band across T-527's sky
//       bands (T-481's, which the hour has already stepped toward navy) -- CONTENT's red high in the sky, CONTEXT's
//       green lower, as the two lines hang in a real aurora. A grove takes the light the first night it is entered
//       after the mending, and lights its pair across the sea (sGroves below: conjugate aurorae share a field line).
//   (c) OUTDOORS AT NIGHT, on the light's nights, once every grove found is lit: a faint breath over 9.21's night on the
//       map's own rows. Never in a building or a cave (map type), never in a timeless place.
//
// THE LIGHT'S NIGHTS: about one in three, from the date, so the game and the companion agree (Aurora_IsLightNight;
// DAEMONS docs/TODO.md T-528 states the rule as arithmetic). THE BREATH: the creator's hour's sixteen-second one
// (Pulse_GoldBreath, the pulse's clock), so a cartridge and the companion breathe together.
//
// HOW: laid on PLTT after the pulse, read back from what is there -- so the palette buffers are never touched, the
// watch's tint, the season, the weather's wash (T-415) and the pulse (T-482) stay underneath, and a fade takes the
// light with it. HALFTONE's grey outranks it (the mended town greys in the witching half-hour), as it outranks the
// pulse. Link battles keep their own light.
//
// COST: EWRAM 0 (1 in DEBUG), IWRAM 24, save 14 flags (FLAG_*_GROVE_LIT, the DAEMONS block 0x120-0x12D).
//
// PART 2 (T-528 (d), (e), (f)):
//   (d) THE DAWN CHORUS (Aurora_DawnChorus): the creator's hour's tone becomes SE_DAWN_CHORUS in a lit grove after a
//       night of light (daemons_hours.c); the LARK sits at the fir and sings along (daemons_eudaimon.c).
//   (e) TULIKKO, the arctic fox sweeping snow at STILLFALL CAVE (Aurora_FoxSays), and the two lines that name the
//       light: the mainland's at LAVENDER, the islands' at FOUR ISLAND.
//   (f) THE CONJUGATE LIGHT (Aurora_ConjugateLight): the book mended and a daemon from the other edition in the party.
//       (a), (b) and (c) then show both lines: green below, red crowning it. PLAY's duet waits on it (play.c).
// Part 2's cost: IWRAM 13 more (each entry's line, and the red over the rows).
//
// AND THE SPARKS (T-528 (e)'s last piece, 2026-11-11; Aurora_SparksTick): on a night of light, the book mended, the
// snow TULIKKO flicks at FOUR ISLAND keeps rising -- a few sparks at a time from its tail, fading as they rise, in the
// edition's line; under the conjugate light each one rises green and turns red as it climbs, red over green. A pool of
// four 8x8 OAM sprites at most, six tiles and one palette (taken only while two are free), all given back off the map.
// Cost: RAM 0 (the sprites' own data words), ~0.6 KB ROM.

#define RED_LINE   RGB(30, 7, 9)           // oxygen's red line, 630.0 nm: CONTENT's
#define GREEN_LINE RGB(10, 30, 15)          // oxygen's green line, 557.7 nm: CONTEXT's
#ifdef FIRERED
#define OWN_LINE RED_LINE
#else
#define OWN_LINE GREEN_LINE
#endif

// THE GROVES AND THEIR PAIRS: each mainland grove (the north wind's side, psychology's residents) lit with an island
// grove (the south wind's, machine learning's), by what their residents are -- DAEMONS docs/TODO.md T-528 gives each
// pair's reason. In FOUND-flag order; LIT is FOUND + 0x10.
struct AuroraGrove
{
    u16 map;
    u16 found;
    u16 lit;
    u8 pair;
};

enum
{
    G_VIRIDIAN, G_ROUTE25, G_ROUTE1, G_ROUTE24, G_ROUTE8, G_ROUTE11, G_ROUTE13,
    G_FIVE, G_SIX, G_THREE, G_ONE, G_TWO, G_FOUR, G_SEVEN,
};

static const struct AuroraGrove sGroves[AURORA_GROVE_COUNT] = {
    [G_VIRIDIAN] = { MAP_VIRIDIAN_FOREST_GROVE,           FLAG_VIRIDIAN_FOREST_GROVE_FOUND,           FLAG_VIRIDIAN_FOREST_GROVE_LIT,           G_THREE },
    [G_ROUTE25]  = { MAP_ROUTE25_GROVE,                   FLAG_ROUTE25_GROVE_FOUND,                   FLAG_ROUTE25_GROVE_LIT,                   G_SEVEN },
    [G_ROUTE1]   = { MAP_ROUTE1_GROVE,                    FLAG_ROUTE1_GROVE_FOUND,                    FLAG_ROUTE1_GROVE_LIT,                    G_ONE },
    [G_ROUTE24]  = { MAP_ROUTE24_GROVE,                   FLAG_ROUTE24_GROVE_FOUND,                   FLAG_ROUTE24_GROVE_LIT,                   G_FOUR },
    [G_ROUTE8]   = { MAP_ROUTE8_GROVE,                    FLAG_ROUTE8_GROVE_FOUND,                    FLAG_ROUTE8_GROVE_LIT,                    G_SIX },
    [G_ROUTE11]  = { MAP_ROUTE11_GROVE,                   FLAG_ROUTE11_GROVE_FOUND,                   FLAG_ROUTE11_GROVE_LIT,                   G_TWO },
    [G_ROUTE13]  = { MAP_ROUTE13_GROVE,                   FLAG_ROUTE13_GROVE_FOUND,                   FLAG_ROUTE13_GROVE_LIT,                   G_FIVE },
    [G_FIVE]     = { MAP_FIVE_ISLAND_MEADOW_GROVE,        FLAG_FIVE_ISLAND_MEADOW_GROVE_FOUND,        FLAG_FIVE_ISLAND_MEADOW_GROVE_LIT,        G_ROUTE13 },
    [G_SIX]      = { MAP_SIX_ISLAND_WATER_PATH_GROVE,     FLAG_SIX_ISLAND_WATER_PATH_GROVE_FOUND,     FLAG_SIX_ISLAND_WATER_PATH_GROVE_LIT,     G_ROUTE8 },
    [G_THREE]    = { MAP_THREE_ISLAND_BERRY_FOREST_GROVE, FLAG_THREE_ISLAND_BERRY_FOREST_GROVE_FOUND, FLAG_THREE_ISLAND_BERRY_FOREST_GROVE_LIT, G_VIRIDIAN },
    [G_ONE]      = { MAP_ONE_ISLAND_GROVE,                FLAG_ONE_ISLAND_GROVE_FOUND,                FLAG_ONE_ISLAND_GROVE_LIT,                G_ROUTE1 },
    [G_TWO]      = { MAP_TWO_ISLAND_GROVE,                FLAG_TWO_ISLAND_GROVE_FOUND,                FLAG_TWO_ISLAND_GROVE_LIT,                G_ROUTE11 },
    [G_FOUR]     = { MAP_FOUR_ISLAND_GROVE,               FLAG_FOUR_ISLAND_GROVE_FOUND,               FLAG_FOUR_ISLAND_GROVE_LIT,               G_ROUTE24 },
    [G_SEVEN]    = { MAP_SEVEN_ISLAND_GROVE,              FLAG_SEVEN_ISLAND_GROVE_FOUND,              FLAG_SEVEN_ISLAND_GROVE_LIT,              G_ROUTE25 },
};

// (b) the band across the sky bands, in 256ths of its strength, top first: the red line high, the green one lower.
#define SKY_MAX 6
#ifdef FIRERED
static const u8 sBand[SKY_MAX] = { 255, 160, 64, 16, 0, 0 };
#else
static const u8 sBand[SKY_MAX] = { 96, 255, 160, 48, 16, 0 };
#endif
// (f) under the conjugate light, both lines in the same sky as a real aurora hangs them: the red crowning it in the top
// band, the green below in the next three (a grove's trees stand in front of the lower bands, so the green begins
// high enough to be seen). Each band takes one line, never a mix (the two mixed are olive).
static const u8 sBandConjugate[SKY_MAX] = { 255, 255, 176, 64, 16, 0 };
#define CONJUGATE_RED_BANDS 1

// (a) DAEMON TOWER's pale stones in LAVENDER's row 7 (the tower is the only thing in the town that draws the row),
// and each one's share of the light. Under the conjugate light the palest, the lit faces, takes the red that crowns
// it and the other three the green below.
#define TOWER_ROW 7
static const u8 sTowerStones[][2] = { { 1, 255 }, { 9, 255 }, { 2, 192 }, { 6, 128 } };
#define CONJUGATE_RED_STONES 1

// IWRAM, uninitialised: what this frame lays on PLTT.
static u32 sRowMask;            // (c): BG rows washed whole, colours 1-15
static u8 sRowWeight;
static u8 sRowRedWeight;        // (f): and the red over the green, under the conjugate light
static u8 sCount;               // (a) and (b): single colours, each with its own weight and line
static u16 sEntry[SKY_MAX];
static u8 sWeight[SKY_MAX];
static u16 sColour[SKY_MAX];

#if DAEMONS_DEBUG
EWRAM_DATA u8 gAuroraDebugNights = 0;
#endif

// THE LIGHT'S NIGHTS. N is the night's date as a day number: days since 2000-01-01 on the cartridge's clock (DEBUG's
// DATE moves it), or the hours played without one (an hour of play is a day, T-273) -- and a night belongs to the
// evening it began on, so before noon it is the day before's. Then two rounds of a 16-bit multiplicative hash, and
// one night in three: every product fits in 32 bits, so the companion's TypeScript gets the same answer.
u16 Aurora_NightNumber(void)
{
    u16 day, minute;

    DaemonsDayAndMinute(&day, &minute);
    if (minute < 12 * 60)
        day--;
    return day;
}

bool8 Aurora_IsLightNight(void)
{
    u32 x;

#if DAEMONS_DEBUG
    if (gAuroraDebugNights != 0)
        return gAuroraDebugNights == 1;
#endif
    x = ((u32)Aurora_NightNumber() * 40503) & 0xFFFF;
    x ^= x >> 7;
    x = (x * 40503) & 0xFFFF;
    return ((x >> 8) % 3) == 0;
}

// (f) THE CONJUGATE LIGHT. The other edition is the one this cartridge is not: CONTENT (FireRed's version byte) sees
// CONTEXT's (LeafGreen's) and the other way round; a daemon from Ruby, Sapphire, Emerald or Colosseum is from neither.
#if GAME_VERSION == VERSION_FIRE_RED
#define OTHER_EDITION VERSION_LEAF_GREEN
#else
#define OTHER_EDITION VERSION_FIRE_RED
#endif

bool8 Aurora_PartyFromOtherEdition(void)
{
    u32 i;

    for (i = 0; i < gPlayerPartyCount && i < PARTY_SIZE; i++)
    {
        if (GetMonData(&gPlayerParty[i], MON_DATA_SPECIES) != SPECIES_NONE
         && !GetMonData(&gPlayerParty[i], MON_DATA_IS_EGG)
         && GetMonData(&gPlayerParty[i], MON_DATA_MET_GAME) == OTHER_EDITION)
            return TRUE;
    }
    return FALSE;
}

bool8 Aurora_ConjugateLight(void)
{
    return FlagGet(FLAG_UNITY_MENDED) && Aurora_PartyFromOtherEdition();
}

static u16 MapHere(void)
{
    return gSaveBlock1Ptr->location.mapNum | (gSaveBlock1Ptr->location.mapGroup << 8);
}

static s8 GroveHere(void)
{
    u16 here = MapHere();
    u32 i;

    for (i = 0; i < AURORA_GROVE_COUNT; i++)
    {
        if (sGroves[i].map == here)
            return i;
    }
    return -1;
}

void Aurora_OnArrive(void)
{
    s8 grove;

    if (!FlagGet(FLAG_UNITY_MENDED) || DaemonsWatch() != WATCH_NIGHT || (grove = GroveHere()) < 0)
        return;
    FlagSet(sGroves[grove].lit);
    FlagSet(sGroves[sGroves[grove].pair].lit);
}

// (d) THE DAWN CHORUS: in a lit grove, in THE CREATOR'S HOUR, on the morning after a night of light (before noon the
// night asked about is the evening before's, so the hour asks about the night just gone). daemons_hours.c plays
// SE_DAWN_CHORUS for its tone then, the pad with the rising whistles under it, and the LARK sits at the fir and sings.
bool8 Aurora_DawnChorus(void)
{
    s8 grove;

    return FlagGet(FLAG_UNITY_MENDED) && DaemonsCreatorSecond() >= 0 && (grove = GroveHere()) >= 0
        && FlagGet(sGroves[grove].lit) && Aurora_IsLightNight();
}

// (e) TULIKKO at STILLFALL CAVE (specialvar): 0 before the mending, 1 after, 2 a night of light (the night watch, so
// the sparks are up), 3 the conjugate light at night. The islands' line (FOUR ISLAND) and the mainland's (LAVENDER)
// ask the mending alone.
u16 Aurora_FoxSays(void)
{
    if (!FlagGet(FLAG_UNITY_MENDED))
        return 0;
    if (DaemonsWatch() == WATCH_NIGHT && Aurora_ConjugateLight())
        return 3;
    if (DaemonsWatch() == WATCH_NIGHT && Aurora_IsLightNight())
        return 2;
    return 1;
}

// (c)'s gate: every grove found is lit, and at least one is.
static bool8 EveryFoundGroveLit(void)
{
    u32 i;
    bool8 any = FALSE;

    for (i = 0; i < AURORA_GROVE_COUNT; i++)
    {
        if (FlagGet(sGroves[i].lit))
            any = TRUE;
        else if (FlagGet(sGroves[i].found))
            return FALSE;
    }
    return any;
}

static void AddEntry(u16 entry, u32 weight, u16 colour)
{
    if (sCount < SKY_MAX && weight != 0)
    {
        sEntry[sCount] = entry;
        sWeight[sCount] = min(weight, 255);
        sColour[sCount] = colour;
        sCount++;
    }
}

void Aurora_Tick(void)
{
    const u8 *sky;
    u32 breath, fade, i, n;
    s8 grove;
    bool8 both;

    sRowMask = 0;
    sRowRedWeight = 0;
    sCount = 0;
    if (!FlagGet(FLAG_UNITY_MENDED) || DaemonsIsHalftone() || DaemonsWatch() != WATCH_NIGHT)
        return;
    breath = Pulse_GoldBreath();
    both = Aurora_ConjugateLight();
    if (DaemonsFieldOnScreen())
    {
        // (a) over the tower, every night
        if (MapHere() == MAP_LAVENDER_TOWN)
        {
            for (i = 0; i < ARRAY_COUNT(sTowerStones); i++)
                AddEntry(BG_PLTT_ID(TOWER_ROW) + sTowerStones[i][0], (64 + breath * 112 / 255) * sTowerStones[i][1] / 255,
                         !both ? OWN_LINE : i < CONJUGATE_RED_STONES ? RED_LINE : GREEN_LINE);
        }
        // (c) outdoors, on the light's nights, once every grove found is lit
        if (IsMapTypeOutdoors(gMapHeader.mapType) && !DaemonsIsTimeless() && EveryFoundGroveLit() && Aurora_IsLightNight())
        {
            sRowMask = (1 << 13) - 1;       // the map's own rows, 0-12; 13-15 are the windows and the text
            sRowWeight = 10 + breath * 22 / 255;
            // (f) a palette row has no height, so the two lines cannot hang one over the other here: the green holds
            // the night and the red rises over it at the top of each breath
            if (both)
                sRowRedWeight = breath * 14 / 255;
        }
    }
    else if (gMain.inBattle && gMain.callback2 == BattleMainCB2 && !(gBattleTypeFlags & BATTLE_TYPE_LINK))
    {
        // (b) a lit grove's battle sky, on the light's nights
        grove = GroveHere();
        if (grove >= 0 && FlagGet(sGroves[grove].lit) && Aurora_IsLightNight()
         && (n = DaemonsBattleBgSkySlots(&sky)) != 0)
        {
            for (i = 0; i < n; i++)
            {
                if (both)
                    AddEntry(BG_PLTT_ID(2) + sky[i], (72 + breath * 96 / 255) * sBandConjugate[i] / 255,
                             i < CONJUGATE_RED_BANDS ? RED_LINE : GREEN_LINE);
                else
                    AddEntry(BG_PLTT_ID(2) + sky[i], (72 + breath * 96 / 255) * sBand[i] / 255, OWN_LINE);
            }
        }
    }
    // A fade takes the light with it, as it takes the pulse.
    if ((sRowMask != 0 || sCount != 0) && (gPaletteFade.active || gPaletteFade.y != 0))
    {
        fade = 16 - min(gPaletteFade.y, 16);
        sRowWeight = sRowWeight * fade / 16;
        sRowRedWeight = sRowRedWeight * fade / 16;
        for (i = 0; i < sCount; i++)
            sWeight[i] = sWeight[i] * fade / 16;
    }
}

//  ---------------------------------------------------------------- (e) THE SPARKS FROM THE FOX'S SNOW

// Six tiles (DAEMONS tools/gentulikko.py): a spark young, older and nearly gone, in the red line and then the green.
// Their colours are the sky's lines exactly, each with its core (halfway to white) and its embers (five eighths).
#define SPARK_CORE(c) RGB((GET_R(c) + 31) / 2, (GET_G(c) + 31) / 2, (GET_B(c) + 31) / 2)
#define SPARK_DIM(c)  RGB(GET_R(c) * 5 / 8, GET_G(c) * 5 / 8, GET_B(c) * 5 / 8)
#define SPARK_TAG          0x5291
#define SPARKS_MAX         4               // only a few at a time
#define SPARK_EVERY        18              // a new spark at most every 18 frames
#define SPARK_PALETTES_FREE 2              // take a palette only when this many are free (the motes' rule)
#define SPARK_FRAMES       3
#define SPARK_RED          0               // the tile of each line's first frame
#define SPARK_GREEN        SPARK_FRAMES
#ifdef FIRERED
#define SPARK_OWN SPARK_RED
#else
#define SPARK_OWN SPARK_GREEN
#endif

static const u32 sSparkTiles[] = INCBIN_U32("graphics/field_effects/pics/tulikko_sparks.4bpp");
static const u16 sSparkPalette[16] = {
    RGB(0, 0, 0),
    SPARK_CORE(RED_LINE), RED_LINE, SPARK_DIM(RED_LINE),
    SPARK_CORE(GREEN_LINE), GREEN_LINE, SPARK_DIM(GREEN_LINE),
};
static const struct SpriteSheet sSparkSheet = { sSparkTiles, sizeof(sSparkTiles), SPARK_TAG };
static const struct SpritePalette sSparkSpritePalette = { sSparkPalette, SPARK_TAG };

static const struct OamData sSparkOam = {
    .affineMode = ST_OAM_AFFINE_OFF,
    .objMode = ST_OAM_OBJ_NORMAL,
    .shape = SPRITE_SHAPE(8x8),
    .size = SPRITE_SIZE(8x8),
    .priority = 1,                          // under BG0: a text box or a menu covers them
};

static void SpriteCB_Spark(struct Sprite *sprite);

static const struct SpriteTemplate sSparkTemplate = {
    .tileTag = SPARK_TAG,
    .paletteTag = SPARK_TAG,
    .oam = &sSparkOam,
    .anims = gDummySpriteAnimTable,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCB_Spark,
};

#define sAge   data[0]
#define sLife  data[1]
#define sDX16  data[2]     // from the fox's centre, in sixteenths of a pixel
#define sDY16  data[3]
#define sPhase data[4]
#define sDrift data[5]     // sixteenths a frame, away from the fox
#define sFox   data[6]     // its object event, and SPARK_BOTH under the conjugate light: green rising, red above
#define sTile  data[7]
#define SPARK_BOTH 0x100

// The fox on screen: its object event, or -1.
static s8 FoxHere(void)
{
    u32 i;

    for (i = 0; i < OBJECT_EVENTS_COUNT; i++)
    {
        if (gObjectEvents[i].active && OBJ_GFX_ID(&gObjectEvents[i]) == OBJ_EVENT_GFX_TULIKKO
         && gObjectEvents[i].spriteId < MAX_SPRITES)
            return i;
    }
    return -1;
}

static void SpriteCB_Spark(struct Sprite *sprite)
{
    struct ObjectEvent *obj = &gObjectEvents[sprite->sFox & 0xFF];
    struct Sprite *fox;
    s16 age = ++sprite->sAge, life = sprite->sLife;
    u32 frame, line;

    if (age >= life || !obj->active || OBJ_GFX_ID(obj) != OBJ_EVENT_GFX_TULIKKO || obj->spriteId >= MAX_SPRITES)
    {
        DestroySprite(sprite);
        return;
    }
    fox = &gSprites[obj->spriteId];
    sprite->sDY16 -= 7;                     // under half a pixel a frame, slowing nothing: it goes on rising
    sprite->sDX16 += sprite->sDrift;
    sprite->sPhase = (sprite->sPhase + 5) & 0xFF;
    // Laid where the fox is drawn, so the camera carries them as it carries the fox.
    sprite->coordOffsetEnabled = fox->coordOffsetEnabled;
    sprite->x = fox->x + (sprite->sDX16 >> 4) + Sin(sprite->sPhase, 3);
    sprite->y = fox->y + (sprite->sDY16 >> 4);
    // Fading as it rises: the cross, the point, the speck, and the speck's last frames flickering out.
    frame = age * 20 < life * 9 ? 0 : age * 20 < life * 15 ? 1 : 2;
    line = !(sprite->sFox & SPARK_BOTH) ? SPARK_OWN : age * 20 < life * 8 ? SPARK_GREEN : SPARK_RED;
    sprite->oam.tileNum = sprite->sTile + line + frame;
    sprite->invisible = fox->invisible || (life - age < 16 && (age & 2));
}

static u8 CountSparks(bool8 destroy)
{
    u8 i, n = 0;

    for (i = 0; i < MAX_SPRITES; i++)
    {
        if (gSprites[i].inUse && gSprites[i].callback == SpriteCB_Spark)
        {
            if (destroy)
                DestroySprite(&gSprites[i]);
            else
                n++;
        }
    }
    return n;
}

static void FreeSparks(void)
{
    if (GetSpriteTileStartByTag(SPARK_TAG) != 0xFFFF)
        FreeSpriteTilesByTag(SPARK_TAG);
    if (IndexOfSpritePaletteTag(SPARK_TAG) != 0xFF)
        FreeSpritePaletteByTag(SPARK_TAG);
}

static u8 FreeSpritePaletteCount(void)
{
    u8 i, n = 0;

    for (i = gReservedSpritePaletteCount; i < 16; i++)
        if (GetSpritePaletteTagByPaletteNum(i) == TAG_NONE)
            n++;
    return n;
}

// Once a frame from main.c. On FOUR ISLAND only; anywhere else the pool is emptied and its tiles and palette given
// back (a warp's reset takes them anyway; a walk off the map's edge does not).
void Aurora_SparksTick(void)
{
    struct ObjectEvent *obj;
    struct Sprite *sprite;
    u8 have, slot, spriteId;
    s8 fox;
    s16 side;

    if (!DaemonsFieldOnScreen())
        return;
    if (MapHere() != MAP_FOUR_ISLAND)
    {
        if (CountSparks(TRUE) == 0)
            FreeSparks();
        return;
    }
    have = CountSparks(FALSE);
    // The night's relight and the weather retint the sprites' slots: the sparks keep the sky's colours.
    if (have != 0 && !gPaletteFade.active && (slot = IndexOfSpritePaletteTag(SPARK_TAG)) != 0xFF)
    {
        CpuCopy16(sSparkPalette, &gPlttBufferUnfaded[OBJ_PLTT_ID(slot)], 7 * sizeof(u16));
        CpuCopy16(sSparkPalette, &gPlttBufferFaded[OBJ_PLTT_ID(slot)], 7 * sizeof(u16));
    }
    if (!FlagGet(FLAG_UNITY_MENDED) || DaemonsIsHalftone() || DaemonsWatch() != WATCH_NIGHT
     || !(Aurora_IsLightNight() || Aurora_ConjugateLight()) || (fox = FoxHere()) < 0)
    {
        if (have == 0)
            FreeSparks();               // the last one gone: its tiles and palette given back
        return;
    }
    obj = &gObjectEvents[fox];
    if (have >= SPARKS_MAX || gPaletteFade.active || (gMain.vblankCounter2 % SPARK_EVERY) != 0
     || gSprites[obj->spriteId].invisible)
        return;
    if (IndexOfSpritePaletteTag(SPARK_TAG) == 0xFF)
    {
        if (FreeSpritePaletteCount() < SPARK_PALETTES_FREE || LoadSpritePalette(&sSparkSpritePalette) == 0xFF)
            return;
    }
    if (GetSpriteTileStartByTag(SPARK_TAG) == 0xFFFF)
    {
        LoadSpriteSheet(&sSparkSheet);
        if (GetSpriteTileStartByTag(SPARK_TAG) == 0xFFFF)
            return;
    }
    spriteId = CreateSprite(&sSparkTemplate, 0, 0, gSprites[obj->spriteId].subpriority);
    if (spriteId == MAX_SPRITES)
        return;
    sprite = &gSprites[spriteId];
    // From the tail: it sweeps behind the fox (to the east when it faces west, its work), or to one side or the other
    // when it has turned to talk.
    if (obj->facingDirection == DIR_WEST)
        side = 1;
    else if (obj->facingDirection == DIR_EAST)
        side = -1;
    else
        side = (Random() & 1) ? 1 : -1;
    sprite->sAge = 0;
    sprite->sLife = 64 + Random() % 40;                         // a second to nearly two
    sprite->sDX16 = (side * (11 + Random() % 5)) << 4;
    sprite->sDY16 = (5 + Random() % 4) << 4;
    sprite->sPhase = Random() & 0xFF;
    sprite->sDrift = side * (Random() % 3);
    sprite->sFox = fox | (Aurora_ConjugateLight() ? SPARK_BOTH : 0);
    sprite->sTile = GetSpriteTileStartByTag(SPARK_TAG);
    SpriteCB_Spark(sprite);
    sprite->sAge = 0;
}

static void Lay(u16 *pltt, u32 e, s32 w, u16 line)
{
    u16 c = pltt[e];
    s32 r = GET_R(c), g = GET_G(c), b = GET_B(c);

    r += ((GET_R(line) - r) * w) >> 8;
    g += ((GET_G(line) - g) * w) >> 8;
    b += ((GET_B(line) - b) * w) >> 8;
    pltt[e] = RGB(r, g, b);
}

//  T-546: into any palette (PLTT, or THE FIELD STAGE daemons_blackout.c builds outside the vblank), over entries lo..hi
//  -- so the map's rows can be laid before the vblank and DMA'd, never written by the CPU while the screen draws.
void Aurora_LayTo(u16 *pltt, u32 lo, u32 hi)
{
    u32 mask = sRowMask, p, e, i;

    if (mask != 0 && sRowWeight != 0)
    {
        for (p = 0; mask != 0; p++, mask >>= 1)
        {
            if (mask & 1)
            {
                for (e = p * 16 + 1; e < p * 16 + 16; e++)
                {
                    if (e < lo || e >= hi)
                        continue;
                    if (sRowRedWeight == 0)
                        Lay(pltt, e, sRowWeight, OWN_LINE);
                    else
                    {
                        Lay(pltt, e, sRowWeight, GREEN_LINE);
                        Lay(pltt, e, sRowRedWeight, RED_LINE);
                    }
                }
            }
        }
    }
    for (i = 0; i < sCount; i++)
        if (sEntry[i] >= lo && sEntry[i] < hi)
            Lay(pltt, sEntry[i], sWeight[i], sColour[i]);
}

void Aurora_ApplyToPltt(void)
{
    Aurora_LayTo((u16 *)PLTT, 0, PLTT_SIZE / 2);
}

//  T-546: whether the aurora lays anything on entries below `hi` this frame
bool8 Aurora_Touches(u32 hi)
{
    u32 i;

    if ((sRowMask & ((1u << ((hi + 15) / 16)) - 1)) != 0 && sRowWeight != 0)
        return TRUE;
    for (i = 0; i < sCount; i++)
        if (sEntry[i] < hi && sWeight[i] != 0)
            return TRUE;
    return FALSE;
}

#if DAEMONS_DEBUG
void Aurora_DebugLightAll(bool8 on)    // DEBUG -> GAME EVENTS -> EVERY GROVE LIT
{
    u32 i;

    for (i = 0; i < AURORA_GROVE_COUNT; i++)
    {
        if (on)
            FlagSet(sGroves[i].lit);
        else
            FlagClear(sGroves[i].lit);
    }
}

void Aurora_DebugOtherEdition(bool8 on)    // DEBUG -> GAME EVENTS -> THE OTHER SIDE'S DAEMON
{
    // on: the lead becomes the other edition's (its met game, the very byte the rule reads); off: every daemon the
    // party holds from the other edition is this one's again
    u8 version;
    u32 i;

    for (i = 0; i < gPlayerPartyCount && i < PARTY_SIZE; i++)
    {
        if (GetMonData(&gPlayerParty[i], MON_DATA_SPECIES) == SPECIES_NONE || GetMonData(&gPlayerParty[i], MON_DATA_IS_EGG))
            continue;
        if (on)
        {
            version = OTHER_EDITION;
            SetMonData(&gPlayerParty[i], MON_DATA_MET_GAME, &version);
            return;
        }
        if (GetMonData(&gPlayerParty[i], MON_DATA_MET_GAME) == OTHER_EDITION)
        {
            version = GAME_VERSION;
            SetMonData(&gPlayerParty[i], MON_DATA_MET_GAME, &version);
        }
    }
}

bool8 Aurora_DebugAllLit(void)
{
    u32 i;

    for (i = 0; i < AURORA_GROVE_COUNT; i++)
    {
        if (!FlagGet(sGroves[i].lit))
            return FALSE;
    }
    return TRUE;
}
#endif
