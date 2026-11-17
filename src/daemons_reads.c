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
#include "strings.h"
#include "daemons_time.h"
#include "event_object_movement.h"
#include "constants/event_objects.h"
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
// map's section (low byte; Kanto's start at 0x58, so 0 is an empty row), its phrase (bits 8-11), its weekday + 1 (bits
// 12-14, T-511: 0 for a read logged before the day was kept) and LANDED (bit 15);
// its THING var the flag that says the thing has been found, 0 for a read about nothing. A read LANDS when that flag is
// set -- checked on each arrival and when the page is opened -- and a read that leaves the log unlanded never does.

#define READ_LANDED     (1 << 15)
#define READ_PHRASE(w)  (((w) >> 8) & 0xF)
#define READ_MAPSEC(w)  ((w) & 0xFF)
#define READ_DAY(w)     (((w) >> 12) & 0x7)   // T-511: the weekday + 1, 0 none

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
    PHRASE_CHORUS,      // T-550: a crowd's read -- about no thing, so it is given and never lands
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
static const u8 sPhrase_Chorus[] = _("the room is thinking one thought.");   // T-550 (the user, 2026-11-12: A), 181px

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
    [PHRASE_CHORUS]  = sPhrase_Chorus,
};

// THE LOG's page (DRAFT). Reflowed by the book reader: \p starts a line.
static const u8 sLog_Reads[]  = _("Reads: {STR_VAR_1} given, {STR_VAR_2} landed.\p");
static const u8 sLog_Leans[]  = _("Leans: {STR_VAR_1} told, {STR_VAR_2} landed.\p");
static const u8 sLog_Colon[]  = _(": ");
static const u8 sLog_DayComma[] = _(", ");   // T-511, DRAFT: "FRIDAY, SAFFRON CITY: ..."
static const u8 sLog_Landed[] = _(" Landed.");
static const u8 sLog_Line[]   = _("\p");
// T-516 S1, DRAFT (the user, 2026-11-02, by multiple choice): SATURDAY -- "never what did I see, but who was looking".
// The NOTEBOOK's sheet has no field purple (charmap PURPLE is the dialogue box's slot), so its own: book_reader.c's
// C_COVER, CONTEXT's hue on the notebook's palette, over the sheet's shadow (C_SHADOW, 3).
static const u8 sLog_Saturday[] = _("{COLOR 15}{SHADOW 3}SATURDAY. Who was looking?\p");

// T-511: the day's names, as the day banner writes them (strings.c).
static const u8 *const sDayNames[WEEKDAY_COUNT] =
{
    gText_DaySunday, gText_DayMonday, gText_DayTuesday, gText_DayWednesday, gText_DayThursday, gText_DayFriday,
    gText_DaySaturday,
};

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

// T-579, THE SUBTLE LAYER's HACKED (the user, 2026-11-17, A: a read is fixed by the day and the place): every draw a
// read makes -- whether one comes, what it is about, where it points -- comes from a generator of its own, seeded from
// the day (DaemonsDayAndMinute's day number: the date with its year, or play time's hour without a clock) and the map
// it comes on. So reloading a save and walking in again gives the same read, true or not, in the same words, and
// nothing says so. The game's own Random() is not drawn on, so nothing else moves with it. (DRAFT, a choice for the
// user: the same read comes back on each arrival that day, once the rest between reads allows one; THE LOG keeps it
// once.)
static EWRAM_DATA u32 sFixed = 0;

static void FixedSeed(u16 salt)
{
    u16 day, minute;
    u8 i;

    DaemonsDayAndMinute(&day, &minute);
    sFixed = ((u32)day << 16) ^ ((u32)gSaveBlock1Ptr->location.mapGroup << 8) ^ gSaveBlock1Ptr->location.mapNum
           ^ ((u32)salt * 0x9E3779B9);
    for (i = 0; i < 4; i++)
        sFixed = ISO_RANDOMIZE1(sFixed);
}

static u16 FixedRandom(void)
{
    sFixed = ISO_RANDOMIZE1(sFixed);
    return sFixed >> 16;
}

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

// T-550, THE SUBTLE LAYER's CHORUS: "It feels like a room thinking one thought." A crowd is READ_CROWD or more people
// on the screen the first step shows -- counted, not listed by map, so it holds on every map there is and only while
// the people are really there: the busy interiors (a GAME CORNER, a DEPT. STORE floor, a CENTER's upper floor) and a
// crowded street alike, and an emptied room is not one. A person is any object drawn as somebody -- every actor is an
// animal here, so it is easier to say what is NOT one: a thing (an item, a sign, a prop, a tree, a stone) or a daemon.
static bool8 IsPerson(u16 gfx)
{
    if (gfx < OBJ_EVENT_GFX_GREEN_NORMAL)                                       // the player's own sprites
        return FALSE;
    if (gfx >= OBJ_EVENT_GFX_ITEM_BALL && gfx <= OBJ_EVENT_GFX_SS_ANNE)        // things, and the daemons drawn as they
        return gfx == OBJ_EVENT_GFX_LIONESS;                                    // are -- all but T-461's LIONESS
    if (gfx >= OBJ_EVENT_GFX_DAEMON_CHANSEY_S2 && gfx <= OBJ_EVENT_GFX_PLUGIN_DISC)   // T-438's daemons, the disc
        return FALSE;
    switch (gfx)
    {
    case OBJ_EVENT_GFX_SINGING_FIR:
    case OBJ_EVENT_GFX_SINGING_FIR_DIM:
    case OBJ_EVENT_GFX_BEACH_PALM:
    case OBJ_EVENT_GFX_BREATH_FLUTE:
    case OBJ_EVENT_GFX_CRYSTAL_RECEIVER:
    case OBJ_EVENT_GFX_DESK_FLOWERS:
    case OBJ_EVENT_GFX_DESK_CARD:
    case OBJ_EVENT_GFX_LIONESS_BALL:
    case OBJ_EVENT_GFX_BLANK_TABLET:
        return FALSE;
    }
    return TRUE;
}

static bool8 InACrowd(void)
{
    s16 px, py;
    u8 i, n = 0;
    struct ObjectEvent *obj;

    PlayerGetDestCoords(&px, &py);
    for (i = 0; i < OBJECT_EVENTS_COUNT; i++)
    {
        obj = &gObjectEvents[i];
        if (!obj->active || obj->isPlayer || obj->invisible || !IsPerson(OBJ_GFX_ID(obj)))
            continue;
        if (Abs(obj->currentCoords.x - px) <= READ_SCREEN_X && Abs(obj->currentCoords.y - py) <= READ_SCREEN_Y
         && ++n >= READ_CROWD)
            return TRUE;
    }
    return FALSE;
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

    // T-579: the same read again (the same day, place and words) is the one already kept, not another
    if ((VarGet(VAR_READ_LOG_WHERE) & ~READ_LANDED) == where && VarGet(VAR_READ_LOG_THING) == thing)
        return;

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
    u8 n, phrase, chance;
    bool8 crowd;
    s16 x, y;

    if (here == sReadLastMap)
        return FALSE;                   // not the first step here
    sReadLastMap = here;
    CheckLanded();
    if (!FlagGet(FLAG_GOT_SUBTLE_LAYER))
        return FALSE;
    FixedSeed(0);                       // T-579
    crowd = InACrowd();
    if (!DEBUG_ALWAYS)
    {
        if (sReadRest != 0)
        {
            sReadRest--;
            return FALSE;
        }
        chance = DaemonsWeekday() == 5 ? READ_CHANCE_FRIDAY : READ_CHANCE;   // T-511: FRIDAY
        if (crowd)
            chance /= 2;                                                    // T-550: half as often in a crowd
        if (FixedRandom() % 100 >= chance)
            return FALSE;
    }
    sReadRest = READ_REST;
    if (crowd)
    {
        // T-550: the crowd's read, about nothing found or findable -- THE LOG counts it given, and it never lands
        Log(gMapHeader.regionMapSectionId | (PHRASE_CHORUS << 8) | ((DaemonsWeekday() + 1) << 12), 0);
        StringCopy(StringCopy(gStringVar4, sText_ReadHead), sPhrases[PHRASE_CHORUS]);
        return TRUE;
    }
    n = Reveal_UnfoundCount();
    if (n != 0 && (DEBUG_ALWAYS_TRUE || FixedRandom() % 100 < READ_TRUE))
    {
        thing = Reveal_UnfoundNth(FixedRandom() % n, &x, &y);
    }
    else
    {
        x = FixedRandom() % gMapHeader.mapLayout->width;
        y = FixedRandom() % gMapHeader.mapLayout->height;
    }
    phrase = Describe(x + MAP_OFFSET, y + MAP_OFFSET);
    Log(gMapHeader.regionMapSectionId | (phrase << 8) | ((DaemonsWeekday() + 1) << 12), thing);   // T-511: the day
    StringCopy(StringCopy(gStringVar4, sText_ReadHead), sPhrases[phrase]);
    return TRUE;
}

void Daemons_WriteReadsLog(u8 *dest)
{
    u8 i;
    u16 where;

    CheckLanded();
    if (DaemonsWeekday() == 6)   // T-516 S1: a Saturday opens with one line above the counts
        dest = StringCopy(dest, sLog_Saturday);
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
        if (READ_DAY(where) != 0)   // T-511: TRUE OR NOT -- "write down each read with the day"
        {
            dest = StringCopy(dest, sDayNames[READ_DAY(where) - 1]);
            dest = StringCopy(dest, sLog_DayComma);
        }
        dest = GetMapName(dest, READ_MAPSEC(where), 0);
        dest = StringCopy(dest, sLog_Colon);
        dest = StringCopy(dest, sPhrases[READ_PHRASE(where)]);
        if (where & READ_LANDED)
            dest = StringCopy(dest, sLog_Landed);
        dest = StringCopy(dest, sLog_Line);
    }
}
