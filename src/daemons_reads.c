#include "global.h"
#include "daemons_reads.h"
#include "event_data.h"
#include "fieldmap.h"
#include "field_player_avatar.h"
#include "metatile_behavior.h"
#include "overworld.h"
#include "random.h"
#include "region_map.h"
#include "reveal.h"
#include "string_util.h"
#include "constants/flags.h"
#include "constants/vars.h"

// T-459 B: see include/daemons_reads.h -- what a read is, and the table.
//
// WHAT A READ IS ABOUT. With READ_TRUE percent, when the map has anything still unfound (REVEAL's set less what lies
// underfoot: Reveal_UnfoundNth), one of those, at random; otherwise a tile anywhere on the map, at random -- a read
// about nothing, worded the same way. WHAT IT SAYS is where the thing is, as a sense rather than a place: water, grass
// or sand near it; "in here" indoors; else close by, or the way it mostly lies from the step the read came on.
//
// THE LOG lives in vars, no new save bytes: two counts and READ_LOG reads, newest first. A read's WHERE var holds the
// map's section (low byte; Kanto's start at 0x58, so 0 is an empty row), its phrase (bits 8-11) and LANDED (bit 15);
// its THING var the flag that says the thing has been found, 0 for a read about nothing. A read LANDS when that flag is
// set -- checked on each arrival and when the page is opened -- and a read that leaves the log unlanded never does.

#define READ_LANDED     (1 << 15)
#define READ_PHRASE(w)  (((w) >> 8) & 0xF)
#define READ_MAPSEC(w)  ((w) & 0xFF)

enum
{
    PHRASE_WATER,
    PHRASE_GRASS,
    PHRASE_SAND,
    PHRASE_IN_HERE,
    PHRASE_CLOSE,
    PHRASE_NORTH,
    PHRASE_SOUTH,
    PHRASE_EAST,
    PHRASE_WEST,
    PHRASE_COUNT
};

// Every word DRAFT. The read is the field's purple (charmap PURPLE, new_menu_helpers.c), the shadow the box's own.
static const u8 sText_ReadHead[] = _("{COLOR PURPLE}{SHADOW LIGHT_GRAY}A read:\n");
static const u8 sPhrase_Water[]  = _("something by the water.");
static const u8 sPhrase_Grass[]  = _("something in the grass.");
static const u8 sPhrase_Sand[]   = _("something in the sand.");
static const u8 sPhrase_InHere[] = _("something in here.");
static const u8 sPhrase_Close[]  = _("something close by.");
static const u8 sPhrase_North[]  = _("something to the north.");
static const u8 sPhrase_South[]  = _("something to the south.");
static const u8 sPhrase_East[]   = _("something to the east.");
static const u8 sPhrase_West[]   = _("something to the west.");

static const u8 *const sPhrases[PHRASE_COUNT] =
{
    [PHRASE_WATER]   = sPhrase_Water,
    [PHRASE_GRASS]   = sPhrase_Grass,
    [PHRASE_SAND]    = sPhrase_Sand,
    [PHRASE_IN_HERE] = sPhrase_InHere,
    [PHRASE_CLOSE]   = sPhrase_Close,
    [PHRASE_NORTH]   = sPhrase_North,
    [PHRASE_SOUTH]   = sPhrase_South,
    [PHRASE_EAST]    = sPhrase_East,
    [PHRASE_WEST]    = sPhrase_West,
};

// THE LOG's page (DRAFT). Reflowed by the book reader: \p starts a line.
static const u8 sLog_Reads[]  = _("Reads: {STR_VAR_1} given, {STR_VAR_2} landed.\p");
static const u8 sLog_Leans[]  = _("Leans: {STR_VAR_1} told, {STR_VAR_2} landed.\p");
static const u8 sLog_Colon[]  = _(": ");
static const u8 sLog_Landed[] = _(" Landed.");
static const u8 sLog_Line[]   = _("\p");

static EWRAM_DATA u16 sReadLastMap = 0;     // the map the last step was on, + 1 (0: none yet since power on)
static EWRAM_DATA u8 sReadRest = 0;         // arrivals still to pass before another read can come
#if DAEMONS_DEBUG
EWRAM_DATA u8 gReadsDebugAlways = 0;
#define DEBUG_ALWAYS        (gReadsDebugAlways != 0)
#define DEBUG_ALWAYS_TRUE   (gReadsDebugAlways == 2)
#else
#define DEBUG_ALWAYS        FALSE
#define DEBUG_ALWAYS_TRUE   FALSE
#endif

static void Count(u16 var)
{
    u16 n = VarGet(var);

    if (n < 9999)
        VarSet(var, n + 1);
}

static s16 Abs(s16 v)
{
    return v < 0 ? -v : v;
}

// What a read says about (X, Y), in map coordinates with MAP_OFFSET.
static u8 Describe(s16 x, s16 y)
{
    s16 px, py, dx, dy, i, j;
    bool8 water = FALSE, grass = FALSE, sand = FALSE;
    u32 mb;

    if (IsMapTypeIndoors(gMapHeader.mapType))
        return PHRASE_IN_HERE;
    for (j = -READ_LOOK; j <= READ_LOOK; j++)
    {
        for (i = -READ_LOOK; i <= READ_LOOK; i++)
        {
            mb = MapGridGetMetatileBehaviorAt(x + i, y + j);
            if (MetatileBehavior_IsSurfable(mb))
                water = TRUE;
            else if (MetatileBehavior_IsTallGrass(mb) || MetatileBehavior_IsLongGrass(mb))
                grass = TRUE;
            else if (MetatileBehavior_IsSand(mb) || MetatileBehavior_IsDeepSand(mb))
                sand = TRUE;
        }
    }
    if (water)
        return PHRASE_WATER;
    if (grass)
        return PHRASE_GRASS;
    if (sand)
        return PHRASE_SAND;
    PlayerGetDestCoords(&px, &py);
    dx = x - px;
    dy = y - py;
    if (Abs(dx) + Abs(dy) <= READ_NEAR)
        return PHRASE_CLOSE;
    if (Abs(dy) >= Abs(dx))
        return dy < 0 ? PHRASE_NORTH : PHRASE_SOUTH;
    return dx < 0 ? PHRASE_WEST : PHRASE_EAST;
}

// Marks each read in the log whose thing has since been found, and counts it once.
static void CheckLanded(void)
{
    u8 i;
    u16 where, thing;

    for (i = 0; i < READ_LOG; i++)
    {
        where = VarGet(VAR_READ_LOG_WHERE + i);
        thing = VarGet(VAR_READ_LOG_THING + i);
        if (where != 0 && !(where & READ_LANDED) && thing != 0 && FlagGet(thing))
        {
            VarSet(VAR_READ_LOG_WHERE + i, where | READ_LANDED);
            Count(VAR_READS_LANDED);
        }
    }
}

static void Log(u16 where, u16 thing)
{
    u8 i;

    for (i = READ_LOG - 1; i > 0; i--)
    {
        VarSet(VAR_READ_LOG_WHERE + i, VarGet(VAR_READ_LOG_WHERE + i - 1));
        VarSet(VAR_READ_LOG_THING + i, VarGet(VAR_READ_LOG_THING + i - 1));
    }
    VarSet(VAR_READ_LOG_WHERE, where);
    VarSet(VAR_READ_LOG_THING, thing);
    Count(VAR_READS_GIVEN);
}

bool8 Daemons_TryReadArrives(void)
{
    u16 here = ((gSaveBlock1Ptr->location.mapGroup << 8) | gSaveBlock1Ptr->location.mapNum) + 1;
    u16 thing = 0;
    u8 n, phrase;
    s16 x, y;

    if (here == sReadLastMap)
        return FALSE;                   // not the first step here
    sReadLastMap = here;
    CheckLanded();
    if (!FlagGet(FLAG_GOT_SUBTLE_LAYER))
        return FALSE;
    if (!DEBUG_ALWAYS)
    {
        if (sReadRest != 0)
        {
            sReadRest--;
            return FALSE;
        }
        if (Random() % 100 >= READ_CHANCE)
            return FALSE;
    }
    sReadRest = READ_REST;
    n = Reveal_UnfoundCount();
    if (n != 0 && (DEBUG_ALWAYS_TRUE || Random() % 100 < READ_TRUE))
    {
        thing = Reveal_UnfoundNth(Random() % n, &x, &y);
    }
    else
    {
        x = Random() % gMapHeader.mapLayout->width;
        y = Random() % gMapHeader.mapLayout->height;
    }
    phrase = Describe(x + MAP_OFFSET, y + MAP_OFFSET);
    Log(gMapHeader.regionMapSectionId | (phrase << 8), thing);
    StringCopy(StringCopy(gStringVar4, sText_ReadHead), sPhrases[phrase]);
    return TRUE;
}

void Daemons_WriteReadsLog(u8 *dest)
{
    u8 i;
    u16 where;

    CheckLanded();
    ConvertIntToDecimalStringN(gStringVar1, VarGet(VAR_READS_GIVEN), STR_CONV_MODE_LEFT_ALIGN, 4);
    ConvertIntToDecimalStringN(gStringVar2, VarGet(VAR_READS_LANDED), STR_CONV_MODE_LEFT_ALIGN, 4);
    dest = StringExpandPlaceholders(dest, sLog_Reads);
    ConvertIntToDecimalStringN(gStringVar1, VarGet(VAR_LEANS_TOLD), STR_CONV_MODE_LEFT_ALIGN, 4);
    ConvertIntToDecimalStringN(gStringVar2, VarGet(VAR_LEANS_LANDED), STR_CONV_MODE_LEFT_ALIGN, 4);
    dest = StringExpandPlaceholders(dest, sLog_Leans);
    for (i = 0; i < READ_LOG; i++)
    {
        where = VarGet(VAR_READ_LOG_WHERE + i);
        if (where == 0)
            break;
        dest = GetMapName(dest, READ_MAPSEC(where), 0);
        dest = StringCopy(dest, sLog_Colon);
        dest = StringCopy(dest, sPhrases[READ_PHRASE(where)]);
        if (where & READ_LANDED)
            dest = StringCopy(dest, sLog_Landed);
        dest = StringCopy(dest, sLog_Line);
    }
}
