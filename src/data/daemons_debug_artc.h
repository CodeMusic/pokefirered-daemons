#include "global.h"
#include "gflib.h"
#include "battle_bg.h"
#include "data.h"
#include "decompress.h"
#include "graphics.h"
#include "main.h"
#include "menu.h"
#include "overworld.h"
#include "pokemon_icon.h"
#include "quest_log.h"
#include "scanline_effect.h"
#include "script.h"
#include "sound.h"
#include "string_util.h"
#include "task.h"
#include "constants/battle.h"
#include "constants/songs.h"
#include "constants/species.h"
#include "constants/rgb.h"

//  T-452 arm C's trial (the user, 2026-11-20, by multiple choice): "I won't know until I see them in game and full."
//  DEBUG -> GAME EVENTS -> ART C: OLD / NEW shows each of the 28 daemons arm C redrew (engine a01ea9d67) on the battle
//  terrain -- its front where the wild one stands, its back where yours does, and its party icon old and new in the
//  corner -- first as it was before arm C (graphics/daemons_armc_old/, taken from a01ea9d67^), then on A as the ROM
//  has it now. A swaps back and forth, B is the next daemon, SELECT the one before, START leaves. The user says keep
//  or revert for each; DAEMONS gfx/drafts/t452/c/armc_verdict.json holds the answers for a script to apply.
//
//  Included by src/daemons_debug_events.c inside its DAEMONS_DEBUG block, not compiled on its own: a new object file,
//  even an empty one, reorders the linker's long-branch veneers and changes the release ROMs (found 2026-11-20).
//
//  A screen of its own, not a battle: the battle's own art (its terrain, its textbox, its coordinates and elevation)
//  without a battle's state, so nothing about the party, the save or the battle engine is touched. Every word on it
//  is DRAFT and DEBUG-only; the release ROMs carry none of this file (it is empty there, byte for byte).

#if DAEMONS_DEBUG

//  Each from a01ea9d67^'s graphics/pokemon/<folder>/ (preproc's INCBIN takes one literal string, so one line each).
static const u32 sOldFront_Pidgeot[] = INCBIN_U32("graphics/daemons_armc_old/pidgeot/front.4bpp.lz");
static const u32 sOldBack_Pidgeot[]  = INCBIN_U32("graphics/daemons_armc_old/pidgeot/back.4bpp.lz");
static const u32 sOldPal_Pidgeot[]   = INCBIN_U32("graphics/daemons_armc_old/pidgeot/normal.gbapal.lz");
static const u8  sOldIcon_Pidgeot[]  = INCBIN_U8("graphics/daemons_armc_old/pidgeot/icon.4bpp");
static const u32 sOldFront_Golbat[] = INCBIN_U32("graphics/daemons_armc_old/golbat/front.4bpp.lz");
static const u32 sOldBack_Golbat[]  = INCBIN_U32("graphics/daemons_armc_old/golbat/back.4bpp.lz");
static const u32 sOldPal_Golbat[]   = INCBIN_U32("graphics/daemons_armc_old/golbat/normal.gbapal.lz");
static const u8  sOldIcon_Golbat[]  = INCBIN_U8("graphics/daemons_armc_old/golbat/icon.4bpp");
static const u32 sOldFront_Aerodactyl[] = INCBIN_U32("graphics/daemons_armc_old/aerodactyl/front.4bpp.lz");
static const u32 sOldBack_Aerodactyl[]  = INCBIN_U32("graphics/daemons_armc_old/aerodactyl/back.4bpp.lz");
static const u32 sOldPal_Aerodactyl[]   = INCBIN_U32("graphics/daemons_armc_old/aerodactyl/normal.gbapal.lz");
static const u8  sOldIcon_Aerodactyl[]  = INCBIN_U8("graphics/daemons_armc_old/aerodactyl/icon.4bpp");
static const u32 sOldFront_Bulbasaur[] = INCBIN_U32("graphics/daemons_armc_old/bulbasaur/front.4bpp.lz");
static const u32 sOldBack_Bulbasaur[]  = INCBIN_U32("graphics/daemons_armc_old/bulbasaur/back.4bpp.lz");
static const u32 sOldPal_Bulbasaur[]   = INCBIN_U32("graphics/daemons_armc_old/bulbasaur/normal.gbapal.lz");
static const u8  sOldIcon_Bulbasaur[]  = INCBIN_U8("graphics/daemons_armc_old/bulbasaur/icon.4bpp");
static const u32 sOldFront_Weezing[] = INCBIN_U32("graphics/daemons_armc_old/weezing/front.4bpp.lz");
static const u32 sOldBack_Weezing[]  = INCBIN_U32("graphics/daemons_armc_old/weezing/back.4bpp.lz");
static const u32 sOldPal_Weezing[]   = INCBIN_U32("graphics/daemons_armc_old/weezing/normal.gbapal.lz");
static const u8  sOldIcon_Weezing[]  = INCBIN_U8("graphics/daemons_armc_old/weezing/icon.4bpp");
static const u32 sOldFront_Houndoom[] = INCBIN_U32("graphics/daemons_armc_old/houndoom/front.4bpp.lz");
static const u32 sOldBack_Houndoom[]  = INCBIN_U32("graphics/daemons_armc_old/houndoom/back.4bpp.lz");
static const u32 sOldPal_Houndoom[]   = INCBIN_U32("graphics/daemons_armc_old/houndoom/normal.gbapal.lz");
static const u8  sOldIcon_Houndoom[]  = INCBIN_U8("graphics/daemons_armc_old/houndoom/icon.4bpp");
static const u32 sOldFront_Lodestar[] = INCBIN_U32("graphics/daemons_armc_old/nexus_lodestar/front.4bpp.lz");
static const u32 sOldBack_Lodestar[]  = INCBIN_U32("graphics/daemons_armc_old/nexus_lodestar/back.4bpp.lz");
static const u32 sOldPal_Lodestar[]   = INCBIN_U32("graphics/daemons_armc_old/nexus_lodestar/normal.gbapal.lz");
static const u8  sOldIcon_Lodestar[]  = INCBIN_U8("graphics/daemons_armc_old/nexus_lodestar/icon.4bpp");
static const u32 sOldFront_Reflection[] = INCBIN_U32("graphics/daemons_armc_old/nexus_reflection/front.4bpp.lz");
static const u32 sOldBack_Reflection[]  = INCBIN_U32("graphics/daemons_armc_old/nexus_reflection/back.4bpp.lz");
static const u32 sOldPal_Reflection[]   = INCBIN_U32("graphics/daemons_armc_old/nexus_reflection/normal.gbapal.lz");
static const u8  sOldIcon_Reflection[]  = INCBIN_U8("graphics/daemons_armc_old/nexus_reflection/icon.4bpp");
static const u32 sOldFront_Bastet[] = INCBIN_U32("graphics/daemons_armc_old/nexus_bastet/front.4bpp.lz");
static const u32 sOldBack_Bastet[]  = INCBIN_U32("graphics/daemons_armc_old/nexus_bastet/back.4bpp.lz");
static const u32 sOldPal_Bastet[]   = INCBIN_U32("graphics/daemons_armc_old/nexus_bastet/normal.gbapal.lz");
static const u8  sOldIcon_Bastet[]  = INCBIN_U8("graphics/daemons_armc_old/nexus_bastet/icon.4bpp");
static const u32 sOldFront_Treecko[] = INCBIN_U32("graphics/daemons_armc_old/treecko/front.4bpp.lz");
static const u32 sOldBack_Treecko[]  = INCBIN_U32("graphics/daemons_armc_old/treecko/back.4bpp.lz");
static const u32 sOldPal_Treecko[]   = INCBIN_U32("graphics/daemons_armc_old/treecko/normal.gbapal.lz");
static const u8  sOldIcon_Treecko[]  = INCBIN_U8("graphics/daemons_armc_old/treecko/icon.4bpp");
static const u32 sOldFront_Grovyle[] = INCBIN_U32("graphics/daemons_armc_old/grovyle/front.4bpp.lz");
static const u32 sOldBack_Grovyle[]  = INCBIN_U32("graphics/daemons_armc_old/grovyle/back.4bpp.lz");
static const u32 sOldPal_Grovyle[]   = INCBIN_U32("graphics/daemons_armc_old/grovyle/normal.gbapal.lz");
static const u8  sOldIcon_Grovyle[]  = INCBIN_U8("graphics/daemons_armc_old/grovyle/icon.4bpp");
static const u32 sOldFront_Sceptile[] = INCBIN_U32("graphics/daemons_armc_old/sceptile/front.4bpp.lz");
static const u32 sOldBack_Sceptile[]  = INCBIN_U32("graphics/daemons_armc_old/sceptile/back.4bpp.lz");
static const u32 sOldPal_Sceptile[]   = INCBIN_U32("graphics/daemons_armc_old/sceptile/normal.gbapal.lz");
static const u8  sOldIcon_Sceptile[]  = INCBIN_U8("graphics/daemons_armc_old/sceptile/icon.4bpp");
static const u32 sOldFront_Torchic[] = INCBIN_U32("graphics/daemons_armc_old/torchic/front.4bpp.lz");
static const u32 sOldBack_Torchic[]  = INCBIN_U32("graphics/daemons_armc_old/torchic/back.4bpp.lz");
static const u32 sOldPal_Torchic[]   = INCBIN_U32("graphics/daemons_armc_old/torchic/normal.gbapal.lz");
static const u8  sOldIcon_Torchic[]  = INCBIN_U8("graphics/daemons_armc_old/torchic/icon.4bpp");
static const u32 sOldFront_Combusken[] = INCBIN_U32("graphics/daemons_armc_old/combusken/front.4bpp.lz");
static const u32 sOldBack_Combusken[]  = INCBIN_U32("graphics/daemons_armc_old/combusken/back.4bpp.lz");
static const u32 sOldPal_Combusken[]   = INCBIN_U32("graphics/daemons_armc_old/combusken/normal.gbapal.lz");
static const u8  sOldIcon_Combusken[]  = INCBIN_U8("graphics/daemons_armc_old/combusken/icon.4bpp");
static const u32 sOldFront_Blaziken[] = INCBIN_U32("graphics/daemons_armc_old/blaziken/front.4bpp.lz");
static const u32 sOldBack_Blaziken[]  = INCBIN_U32("graphics/daemons_armc_old/blaziken/back.4bpp.lz");
static const u32 sOldPal_Blaziken[]   = INCBIN_U32("graphics/daemons_armc_old/blaziken/normal.gbapal.lz");
static const u8  sOldIcon_Blaziken[]  = INCBIN_U8("graphics/daemons_armc_old/blaziken/icon.4bpp");
static const u32 sOldFront_Mudkip[] = INCBIN_U32("graphics/daemons_armc_old/mudkip/front.4bpp.lz");
static const u32 sOldBack_Mudkip[]  = INCBIN_U32("graphics/daemons_armc_old/mudkip/back.4bpp.lz");
static const u32 sOldPal_Mudkip[]   = INCBIN_U32("graphics/daemons_armc_old/mudkip/normal.gbapal.lz");
static const u8  sOldIcon_Mudkip[]  = INCBIN_U8("graphics/daemons_armc_old/mudkip/icon.4bpp");
static const u32 sOldFront_Marshtomp[] = INCBIN_U32("graphics/daemons_armc_old/marshtomp/front.4bpp.lz");
static const u32 sOldBack_Marshtomp[]  = INCBIN_U32("graphics/daemons_armc_old/marshtomp/back.4bpp.lz");
static const u32 sOldPal_Marshtomp[]   = INCBIN_U32("graphics/daemons_armc_old/marshtomp/normal.gbapal.lz");
static const u8  sOldIcon_Marshtomp[]  = INCBIN_U8("graphics/daemons_armc_old/marshtomp/icon.4bpp");
static const u32 sOldFront_Swampert[] = INCBIN_U32("graphics/daemons_armc_old/swampert/front.4bpp.lz");
static const u32 sOldBack_Swampert[]  = INCBIN_U32("graphics/daemons_armc_old/swampert/back.4bpp.lz");
static const u32 sOldPal_Swampert[]   = INCBIN_U32("graphics/daemons_armc_old/swampert/normal.gbapal.lz");
static const u8  sOldIcon_Swampert[]  = INCBIN_U8("graphics/daemons_armc_old/swampert/icon.4bpp");
static const u32 sOldFront_Poochyena[] = INCBIN_U32("graphics/daemons_armc_old/poochyena/front.4bpp.lz");
static const u32 sOldBack_Poochyena[]  = INCBIN_U32("graphics/daemons_armc_old/poochyena/back.4bpp.lz");
static const u32 sOldPal_Poochyena[]   = INCBIN_U32("graphics/daemons_armc_old/poochyena/normal.gbapal.lz");
static const u8  sOldIcon_Poochyena[]  = INCBIN_U8("graphics/daemons_armc_old/poochyena/icon.4bpp");
static const u32 sOldFront_Zigzagoon[] = INCBIN_U32("graphics/daemons_armc_old/zigzagoon/front.4bpp.lz");
static const u32 sOldBack_Zigzagoon[]  = INCBIN_U32("graphics/daemons_armc_old/zigzagoon/back.4bpp.lz");
static const u32 sOldPal_Zigzagoon[]   = INCBIN_U32("graphics/daemons_armc_old/zigzagoon/normal.gbapal.lz");
static const u8  sOldIcon_Zigzagoon[]  = INCBIN_U8("graphics/daemons_armc_old/zigzagoon/icon.4bpp");
static const u32 sOldFront_Linoone[] = INCBIN_U32("graphics/daemons_armc_old/linoone/front.4bpp.lz");
static const u32 sOldBack_Linoone[]  = INCBIN_U32("graphics/daemons_armc_old/linoone/back.4bpp.lz");
static const u32 sOldPal_Linoone[]   = INCBIN_U32("graphics/daemons_armc_old/linoone/normal.gbapal.lz");
static const u8  sOldIcon_Linoone[]  = INCBIN_U8("graphics/daemons_armc_old/linoone/icon.4bpp");
static const u32 sOldFront_Exeggutor[] = INCBIN_U32("graphics/daemons_armc_old/exeggutor/front.4bpp.lz");
static const u32 sOldBack_Exeggutor[]  = INCBIN_U32("graphics/daemons_armc_old/exeggutor/back.4bpp.lz");
static const u32 sOldPal_Exeggutor[]   = INCBIN_U32("graphics/daemons_armc_old/exeggutor/normal.gbapal.lz");
static const u8  sOldIcon_Exeggutor[]  = INCBIN_U8("graphics/daemons_armc_old/exeggutor/icon.4bpp");
static const u32 sOldFront_Dugtrio[] = INCBIN_U32("graphics/daemons_armc_old/dugtrio/front.4bpp.lz");
static const u32 sOldBack_Dugtrio[]  = INCBIN_U32("graphics/daemons_armc_old/dugtrio/back.4bpp.lz");
static const u32 sOldPal_Dugtrio[]   = INCBIN_U32("graphics/daemons_armc_old/dugtrio/normal.gbapal.lz");
static const u8  sOldIcon_Dugtrio[]  = INCBIN_U8("graphics/daemons_armc_old/dugtrio/icon.4bpp");
static const u32 sOldFront_Blastoise[] = INCBIN_U32("graphics/daemons_armc_old/blastoise/front.4bpp.lz");
static const u32 sOldBack_Blastoise[]  = INCBIN_U32("graphics/daemons_armc_old/blastoise/back.4bpp.lz");
static const u32 sOldPal_Blastoise[]   = INCBIN_U32("graphics/daemons_armc_old/blastoise/normal.gbapal.lz");
static const u8  sOldIcon_Blastoise[]  = INCBIN_U8("graphics/daemons_armc_old/blastoise/icon.4bpp");
static const u32 sOldFront_Golem[] = INCBIN_U32("graphics/daemons_armc_old/golem/front.4bpp.lz");
static const u32 sOldBack_Golem[]  = INCBIN_U32("graphics/daemons_armc_old/golem/back.4bpp.lz");
static const u32 sOldPal_Golem[]   = INCBIN_U32("graphics/daemons_armc_old/golem/normal.gbapal.lz");
static const u8  sOldIcon_Golem[]  = INCBIN_U8("graphics/daemons_armc_old/golem/icon.4bpp");
static const u32 sOldFront_Clefable[] = INCBIN_U32("graphics/daemons_armc_old/clefable/front.4bpp.lz");
static const u32 sOldBack_Clefable[]  = INCBIN_U32("graphics/daemons_armc_old/clefable/back.4bpp.lz");
static const u32 sOldPal_Clefable[]   = INCBIN_U32("graphics/daemons_armc_old/clefable/normal.gbapal.lz");
static const u8  sOldIcon_Clefable[]  = INCBIN_U8("graphics/daemons_armc_old/clefable/icon.4bpp");
static const u32 sOldFront_Blissey[] = INCBIN_U32("graphics/daemons_armc_old/blissey/front.4bpp.lz");
static const u32 sOldBack_Blissey[]  = INCBIN_U32("graphics/daemons_armc_old/blissey/back.4bpp.lz");
static const u32 sOldPal_Blissey[]   = INCBIN_U32("graphics/daemons_armc_old/blissey/normal.gbapal.lz");
static const u8  sOldIcon_Blissey[]  = INCBIN_U8("graphics/daemons_armc_old/blissey/icon.4bpp");
static const u32 sOldFront_Marowak[] = INCBIN_U32("graphics/daemons_armc_old/marowak/front.4bpp.lz");
static const u32 sOldBack_Marowak[]  = INCBIN_U32("graphics/daemons_armc_old/marowak/back.4bpp.lz");
static const u32 sOldPal_Marowak[]   = INCBIN_U32("graphics/daemons_armc_old/marowak/normal.gbapal.lz");
static const u8  sOldIcon_Marowak[]  = INCBIN_U8("graphics/daemons_armc_old/marowak/icon.4bpp");

enum { ARTC_WHAT_ALL, ARTC_WHAT_BACK, ARTC_WHAT_BACK_ICON };   // what arm C redrew: front, back and icon; the back; the back and icon

struct ArtCOld
{
    u16 species;
    const u32 *front, *back, *pal;
    const u8 *icon;
    u8 frontY, backY;   // gMonFrontPicCoords' and gMonBackPicCoords' y_offset before a01ea9d67
    u8 iconPal;         // gMonIconPaletteIndices before a01ea9d67
    u8 what;
};

#define ARTC_ROW(S, Name, fy, by, ip, w) { SPECIES_##S, sOldFront_##Name, sOldBack_##Name, sOldPal_##Name, sOldIcon_##Name, fy, by, ip, w }

//  In T-452's order: the 21 redrawn whole, then the seven backs.
static const struct ArtCOld sArtC[] = {
    ARTC_ROW(PIDGEOT,     Pidgeot,     8, 18, 2, ARTC_WHAT_ALL),    // BROADCAST
    ARTC_ROW(GOLBAT,      Golbat,      3, 16, 1, ARTC_WHAT_ALL),    // TRACER
    ARTC_ROW(AERODACTYL,  Aerodactyl,  4, 18, 0, ARTC_WHAT_ALL),    // FOSSILNET
    ARTC_ROW(BULBASAUR,   Bulbasaur,   5, 15, 1, ARTC_WHAT_ALL),    // ROVERCUB (redrawn again since, T-552: NEW is today's)
    ARTC_ROW(WEEZING,     Weezing,     8, 13, 1, ARTC_WHAT_ALL),    // SMOGSTACK
    ARTC_ROW(HOUNDOOM,    Houndoom,    1,  1, 2, ARTC_WHAT_ALL),    // OBSESSION
    ARTC_ROW(OLD_UNOWN_D, Lodestar,    2,  0, 2, ARTC_WHAT_ALL),    // LODESTAR
    ARTC_ROW(OLD_UNOWN_C, Reflection,  3,  1, 2, ARTC_WHAT_ALL),    // REFLECTION
    ARTC_ROW(OLD_UNOWN_J, Bastet,      2,  0, 2, ARTC_WHAT_ALL),    // BASTET
    ARTC_ROW(TREECKO,     Treecko,    12, 12, 1, ARTC_WHAT_ALL),    // STUMP
    ARTC_ROW(GROVYLE,     Grovyle,     6,  6, 1, ARTC_WHAT_ALL),    // BOOSTING
    ARTC_ROW(SCEPTILE,    Sceptile,    2,  2, 1, ARTC_WHAT_ALL),    // CANOPY
    ARTC_ROW(TORCHIC,     Torchic,     7,  7, 1, ARTC_WHAT_ALL),    // COINFLIP
    ARTC_ROW(COMBUSKEN,   Combusken,   7,  7, 1, ARTC_WHAT_ALL),    // MONTECARLO
    ARTC_ROW(BLAZIKEN,    Blaziken,    2,  2, 1, ARTC_WHAT_ALL),    // LASVEGAS
    ARTC_ROW(MUDKIP,      Mudkip,      9,  9, 0, ARTC_WHAT_ALL),    // RUNOFF
    ARTC_ROW(MARSHTOMP,   Marshtomp,   9,  9, 0, ARTC_WHAT_ALL),    // ALLUVIUM
    ARTC_ROW(SWAMPERT,    Swampert,    5,  5, 0, ARTC_WHAT_ALL),    // DELTA
    ARTC_ROW(POOCHYENA,   Poochyena,   7,  7, 2, ARTC_WHAT_ALL),    // SMUDGE (its ROM icon kept)
    ARTC_ROW(ZIGZAGOON,   Zigzagoon,  10, 10, 2, ARTC_WHAT_ALL),    // DETOUR
    ARTC_ROW(LINOONE,     Linoone,    13, 13, 2, ARTC_WHAT_ALL),    // UNBRANCHED
    ARTC_ROW(EXEGGUTOR,   Exeggutor,   4,  5, 1, ARTC_WHAT_BACK_ICON),  // ENSEMBLE
    ARTC_ROW(DUGTRIO,     Dugtrio,     9, 19, 2, ARTC_WHAT_BACK),   // WIRETAP
    ARTC_ROW(BLASTOISE,   Blastoise,  19, 19, 0, ARTC_WHAT_BACK),   // MANIFOLD
    ARTC_ROW(GOLEM,       Golem,       8, 11, 0, ARTC_WHAT_BACK),   // MONOLITH
    ARTC_ROW(CLEFABLE,    Clefable,    7, 10, 2, ARTC_WHAT_BACK),   // OUTLIER
    ARTC_ROW(BLISSEY,     Blissey,     8,  7, 2, ARTC_WHAT_BACK),   // HEARTBEAT
    ARTC_ROW(MAROWAK,     Marowak,    10,  9, 2, ARTC_WHAT_BACK),   // COREFILE
};

#undef ARTC_ROW

#define TAG_FRONT   0x7A40
#define TAG_BACK    0x7A41
#define TAG_ICON_OLD 0x7A42
#define TAG_ICON_NEW 0x7A43
#define TAG_MON_PAL 0x7A40
#define TAG_ICON_PAL 0x7A48   // + 0..2, the three shared icon palettes

enum { ARTC_WIN_MSG, ARTC_WIN_LABEL };

struct ArtCView
{
    u8 tilemap[0x1000];   // BG0: the battle's textbox
    u8 index;
    bool8 showNew;
    bool8 leaving;
    u8 initState;
    u8 spriteIds[4];
    u8 right[32];         // line 1's right-hand side
};

static const struct BgTemplate sArtCBgTemplates[] = {
    { .bg = 0, .charBaseIndex = 0, .mapBaseIndex = 24, .screenSize = 2, .paletteMode = 0, .priority = 0, .baseTile = 0 },
    { .bg = 3, .charBaseIndex = 2, .mapBaseIndex = 26, .screenSize = 1, .paletteMode = 0, .priority = 3, .baseTile = 0 },
};

static const struct WindowTemplate sArtCWindows[] = {
    [ARTC_WIN_MSG]   = { .bg = 0, .tilemapLeft = 1, .tilemapTop = 15, .width = 28, .height = 4, .paletteNum = 0, .baseBlock = 0x090 },
    [ARTC_WIN_LABEL] = { .bg = 0, .tilemapLeft = 1, .tilemapTop = 0,  .width = 9,  .height = 2, .paletteNum = 0, .baseBlock = 0x100 },
    DUMMY_WIN_TEMPLATE
};

static const struct OamData sArtCOam64 = { .affineMode = ST_OAM_AFFINE_OFF, .objMode = ST_OAM_OBJ_NORMAL, .bpp = ST_OAM_4BPP,
                                       .shape = SPRITE_SHAPE(64x64), .size = SPRITE_SIZE(64x64), .priority = 1 };
static const struct OamData sArtCOam32 = { .affineMode = ST_OAM_AFFINE_OFF, .objMode = ST_OAM_OBJ_NORMAL, .bpp = ST_OAM_4BPP,
                                       .shape = SPRITE_SHAPE(32x32), .size = SPRITE_SIZE(32x32), .priority = 1 };

static const u8 sArtCTextColour[] = { 15, 1, 6 };     // the battle's B_WIN_MSG: bg 15, fg 1, shadow 6
static const u8 sArtCLabelColour[] = { 0, 1, 6 };

// DRAFT (DEBUG only)
static const u8 sArtCText_Old[]      = _("OLD");
static const u8 sArtCText_New[]      = _("NEW");
static const u8 sArtCText_All[]      = _("front back icon");
static const u8 sArtCText_Back[]     = _("back");
static const u8 sArtCText_BackIcon[] = _("back icon");
static const u8 sArtCText_Keys[]     = _("A SWAP   B NEXT   SELECT BACK   START OUT");
static const u8 sArtCText_Label[]    = _("OLD   NEW");
static const u8 sArtCText_Slash[]    = _("/");
static const u8 sArtCText_Gap[]      = _("  ");

static struct ArtCView *ArtC_View(u8 taskId)
{
    return (struct ArtCView *)GetWordTaskArg(taskId, 0);
}

static void ArtC_FreeShown(struct ArtCView *v)
{
    u8 i;

    for (i = 0; i < ARRAY_COUNT(v->spriteIds); i++)
    {
        if (v->spriteIds[i] != MAX_SPRITES)
            DestroySprite(&gSprites[v->spriteIds[i]]);
        v->spriteIds[i] = MAX_SPRITES;
    }
    FreeSpriteTilesByTag(TAG_FRONT);
    FreeSpriteTilesByTag(TAG_BACK);
    FreeSpriteTilesByTag(TAG_ICON_OLD);
    FreeSpriteTilesByTag(TAG_ICON_NEW);
    FreeSpritePaletteByTag(TAG_MON_PAL);
}

static u8 ArtC_MakeSprite(const struct OamData *oam, u16 tileTag, u16 palTag, s16 x, s16 y)
{
    struct SpriteTemplate t = {
        .tileTag = tileTag, .paletteTag = palTag, .oam = oam, .anims = gDummySpriteAnimTable,
        .images = NULL, .affineAnims = gDummySpriteAffineAnimTable, .callback = SpriteCallbackDummy,
    };

    return CreateSprite(&t, x, y, 0);
}

//  The daemon at v->index, old or new: front where a wild one stands (176, 40), back where yours does (72, 80), each at
//  its own y_offset and the front lifted by its elevation, as GetBattlerSpriteFinal_Y puts them. Both icons, always.
static void ArtC_Show(struct ArtCView *v)
{
    const struct ArtCOld *a = &sArtC[v->index];
    u16 species = a->species;
    struct CompressedSpriteSheet front, back;
    struct CompressedSpritePalette pal;
    struct SpriteSheet iconOld = { a->icon, 0x200, TAG_ICON_OLD };
    struct SpriteSheet iconNew = { gMonIconTable[species], 0x200, TAG_ICON_NEW };
    u8 frontY, backY, elevation = gEnemyMonElevation[species];
    const u8 *what;
    u8 *p;

    ArtC_FreeShown(v);
    front.size = back.size = MON_PIC_WIDTH * MON_PIC_HEIGHT / 2;
    front.tag = TAG_FRONT;
    back.tag = TAG_BACK;
    pal.tag = TAG_MON_PAL;
    if (v->showNew)
    {
        front.data = gMonFrontPicTable[species].data;
        back.data = gMonBackPicTable[species].data;
        pal.data = gMonPaletteTable[species].data;
        frontY = gMonFrontPicCoords[species].y_offset;
        backY = gMonBackPicCoords[species].y_offset;
    }
    else
    {
        front.data = a->front;
        back.data = a->back;
        pal.data = a->pal;
        frontY = a->frontY;
        backY = a->backY;
    }
    LoadCompressedSpriteSheet(&front);
    LoadCompressedSpriteSheet(&back);
    LoadCompressedSpritePalette(&pal);
    LoadSpriteSheet(&iconOld);
    LoadSpriteSheet(&iconNew);

    v->spriteIds[0] = ArtC_MakeSprite(&sArtCOam64, TAG_FRONT, TAG_MON_PAL, 176, 40 + frontY - elevation);
    v->spriteIds[1] = ArtC_MakeSprite(&sArtCOam64, TAG_BACK, TAG_MON_PAL, 72, 80 + backY);
    v->spriteIds[2] = ArtC_MakeSprite(&sArtCOam32, TAG_ICON_OLD, TAG_ICON_PAL + a->iconPal, 24, 30);
    v->spriteIds[3] = ArtC_MakeSprite(&sArtCOam32, TAG_ICON_NEW, TAG_ICON_PAL + gMonIconPaletteIndices[species], 60, 30);

    //  Line 1: "LODESTAR  OLD", and at its right "7/28  front back icon" (what arm C redrew); line 2: the keys.
    //  gStringVar4 holds line 1 while the screen is up (the theatre captions a capture with it).
    p = StringCopy(gStringVar4, gSpeciesNames[species]);
    p = StringCopy(p, sArtCText_Gap);
    StringCopy(p, v->showNew ? sArtCText_New : sArtCText_Old);
    what = a->what == ARTC_WHAT_ALL ? sArtCText_All : a->what == ARTC_WHAT_BACK ? sArtCText_Back : sArtCText_BackIcon;
    p = ConvertIntToDecimalStringN(v->right, v->index + 1, STR_CONV_MODE_LEFT_ALIGN, 2);
    p = StringCopy(p, sArtCText_Slash);
    p = ConvertIntToDecimalStringN(p, ARRAY_COUNT(sArtC), STR_CONV_MODE_LEFT_ALIGN, 2);
    p = StringCopy(p, sArtCText_Gap);
    StringCopy(p, what);
    FillWindowPixelBuffer(ARTC_WIN_MSG, PIXEL_FILL(15));
    AddTextPrinterParameterized3(ARTC_WIN_MSG, FONT_NORMAL, 2, 1, sArtCTextColour, TEXT_SKIP_DRAW, gStringVar4);
    AddTextPrinterParameterized3(ARTC_WIN_MSG, FONT_SMALL, 28 * 8 - 4 - GetStringWidth(FONT_SMALL, v->right, 0), 3,
                                 sArtCTextColour, TEXT_SKIP_DRAW, v->right);
    AddTextPrinterParameterized3(ARTC_WIN_MSG, FONT_SMALL, 2, 17, sArtCTextColour, TEXT_SKIP_DRAW, sArtCText_Keys);
    CopyWindowToVram(ARTC_WIN_MSG, COPYWIN_GFX);
}

static void VBlankCB_ArtC(void)
{
    LoadOam();
    ProcessSpriteCopyRequests();
    TransferPlttBuffer();
}

static void CB2_ArtC(void)
{
    RunTasks();
    AnimateSprites();
    BuildOamBuffer();
    UpdatePaletteFade();
}

static void Task_ArtCInput(u8 taskId)
{
    struct ArtCView *v = ArtC_View(taskId);

    if (v->leaving)
    {
        if (gPaletteFade.active)
            return;
        ArtC_FreeShown(v);
        FreeAllWindowBuffers();
        ResetSpriteData();
        FreeAllSpritePalettes();
        Free(v);
        DestroyTask(taskId);
        StringCopy(gStringVar4, sTitles[sPage]);   // the GAME EVENTS box says its own title again, not the last daemon
        SetMainCallback2(CB2_ReturnToFieldContinueScriptPlayMapMusic);
        return;
    }
    if (JOY_NEW(A_BUTTON))
    {
        v->showNew ^= 1;
        PlaySE(SE_SELECT);
        ArtC_Show(v);
    }
    else if (JOY_NEW(B_BUTTON))
    {
        v->index = (v->index + 1) % ARRAY_COUNT(sArtC);
        v->showNew = FALSE;
        PlaySE(SE_SELECT);
        ArtC_Show(v);
    }
    else if (JOY_NEW(SELECT_BUTTON))
    {
        v->index = (v->index + ARRAY_COUNT(sArtC) - 1) % ARRAY_COUNT(sArtC);
        v->showNew = FALSE;
        PlaySE(SE_SELECT);
        ArtC_Show(v);
    }
    else if (JOY_NEW(START_BUTTON))
    {
        PlaySE(SE_SELECT);
        BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 16, RGB_BLACK);
        v->leaving = TRUE;
    }
}

static void Task_ArtCInit(u8 taskId)
{
    struct ArtCView *v = ArtC_View(taskId);
    u8 i;

    switch (v->initState)
    {
    case 0:
        SetVBlankCallback(NULL);
        break;
    case 1:
        DmaClearLarge16(3, (void *)VRAM, VRAM_SIZE, 0x1000);
        DmaClear32(3, (void *)OAM, OAM_SIZE);
        DmaClear16(3, (void *)PLTT, PLTT_SIZE);
        SetGpuReg(REG_OFFSET_DISPCNT, 0);
        ResetBgsAndClearDma3BusyFlags(0);
        InitBgsFromTemplates(0, sArtCBgTemplates, ARRAY_COUNT(sArtCBgTemplates));
        for (i = 0; i < 4; i++)
        {
            ChangeBgX(i, 0, BG_COORD_SET);
            ChangeBgY(i, 0, BG_COORD_SET);
        }
        SetBgTilemapBuffer(0, v->tilemap);
        InitWindows(sArtCWindows);
        DeactivateAllTextPrinters();
        SetGpuReg(REG_OFFSET_BLDCNT, 0);
        break;
    case 2:
        //  The battle's textbox, and its terrain (grass, as a wild battle in the long grass has it -- the day's
        //  season, holiday and hour on it too, as LoadBattleTerrainGfx always paints them).
        LZDecompressVram(gBattleInterface_Textbox_Gfx, (void *)BG_CHAR_ADDR(0));
        CopyToBgTilemapBuffer(0, gBattleInterface_Textbox_Tilemap, 0, 0);
        CopyBgTilemapBufferToVram(0);
        LoadCompressedPalette(gBattleInterface_Textbox_Pal, BG_PLTT_ID(0), 2 * PLTT_SIZE_4BPP);
        DaemonsDebug_LoadBattleTerrain(BATTLE_TERRAIN_GRASS);
        for (i = 0; i < 3; i++)
        {
            struct SpritePalette iconPal = { gMonIconPalettes[i], TAG_ICON_PAL + i };
            LoadSpritePalette(&iconPal);
        }
        break;
    case 3:
        FillWindowPixelBuffer(ARTC_WIN_LABEL, PIXEL_FILL(0));
        AddTextPrinterParameterized3(ARTC_WIN_LABEL, FONT_SMALL, 2, 2, sArtCLabelColour, TEXT_SKIP_DRAW, sArtCText_Label);
        PutWindowTilemap(ARTC_WIN_LABEL);
        CopyWindowToVram(ARTC_WIN_LABEL, COPYWIN_FULL);
        PutWindowTilemap(ARTC_WIN_MSG);
        CopyBgTilemapBufferToVram(0);
        ArtC_Show(v);
        break;
    case 4:
        SetGpuReg(REG_OFFSET_DISPCNT, DISPCNT_MODE_0 | DISPCNT_OBJ_1D_MAP | DISPCNT_OBJ_ON);
        ShowBg(0);
        ShowBg(3);
        BeginNormalPaletteFade(PALETTES_ALL, 0, 16, 0, RGB_BLACK);
        break;
    case 5:
        SetVBlankCallback(VBlankCB_ArtC);
        break;
    default:
        if (gPaletteFade.active)
            return;
        gTasks[taskId].func = Task_ArtCInput;
        return;
    }
    v->initState++;
}

static void CB2_OpenArtC(void)
{
    struct ArtCView *v;
    u8 taskId, i;

    ResetSpriteData();
    ResetPaletteFade();
    FreeAllSpritePalettes();
    ResetTasks();
    ScanlineEffect_Stop();
    v = AllocZeroed(sizeof(*v));
    if (v == NULL)
    {
        SetMainCallback2(CB2_ReturnToFieldContinueScriptPlayMapMusic);
        return;
    }
    for (i = 0; i < ARRAY_COUNT(v->spriteIds); i++)
        v->spriteIds[i] = MAX_SPRITES;
    taskId = CreateTask(Task_ArtCInit, 0);
    SetWordTaskArg(taskId, 0, (u32)v);
    SetMainCallback2(CB2_ArtC);
}

//  callnative from DaemonsDebug_EventScript_GameEventsArt, after it has faded to black; the script waits (waitstate)
//  and the field's return resumes it. No special: the specials table is in the release ROMs too.
void DaemonsDebug_ArtCOpen(void)
{
    QuestLog_CutRecording();
    SetMainCallback2(CB2_OpenArtC);
    LockPlayerFieldControls();
}

#endif // DAEMONS_DEBUG
