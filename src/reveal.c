#include "global.h"
#include "event_scripts.h"
#include "event_data.h"
#include "field_effect.h"
#include "field_player_avatar.h"
#include "field_specials.h"
#include "fieldmap.h"
#include "reveal.h"
#include "constants/field_effects.h"

//  T-220: REVEAL, THE NINTH DRIVER (docs/school.md 8a; vision.md 4.35). 1001 is nine in binary.
//
//  THE FIRST DRIVER THAT OPENS NOTHING. Every other one is a key; this is a lens. It shows what is already there
//  and not shown -- the hidden items on the map you are standing in today, and the groves' trees when they
//  exist (T-221) -- and nothing it shows was unreachable without it. The ITEMFINDER beeps for one thing; this
//  marks every one in sight.
//
//  INSTALLED, NOT TAUGHT, and so not a disc. The TOOLKIT pocket is BAG_TMHM_COUNT, 58 slots, and those 58 are
//  laid out inside the save: a fifty-ninth disc would move every byte after them and break every existing
//  save. So the Reading Room's keeper installs REVEAL into the TOOLKIT itself, and a flag is all it is --
//  which is what "installed" was supposed to mean in the first place.
//
//  THE BEHAVIOUR. Once installed it simply runs: on arrival, then every second and a half, each unclaimed
//  hidden item within a screen of the player shimmers where it lies -- and so does every tree into a grove.

#define REVEAL_PERIOD        90
#define REVEAL_FIRST         60       // frames after arrival before the first shimmer
#define BG_KIND_HIDDEN_ITEM  7        // the same literal itemfinder.c reads
#define BG_KIND_SIGN_LAST    4        // 0..4: a script, read facing any way or one of the four

static EWRAM_DATA u8 sRevealTimer = 0;

//  T-221: the trees that lead into a GROVE, and out of one. A bg_event whose script is one of these twinkles like a
//  hidden item -- the driver shows WHICH tree, and pressing A on the tree that stands alone finds it without. The
//  table is written by tools/gbagrove.py from the same rows that build the groves, so a new grove needs no edit here.
#include "data/grove_trees.h"

//  T-297: THE FINDS. A thing the game hides in plain sight -- a book on a shelf, a page on a wall -- shimmers under
//  REVEAL until it has been found, and then never again: the lens shows where you have not been yet, not a list of
//  what you have. Each row is the find's script and the flag that says it was found. The user's rule (2026-09-26):
//  every find is findable blind first; REVEAL only makes legible what a sharp eye could already see.
static const struct { const u8 *script; u16 foundFlag; } sFinds[] = {
    { OneIsland_House2_EventScript_Guide,             FLAG_GOT_GUIDE },
    { CeladonCity_Condominiums_3F_EventScript_Folds,  FLAG_NOTEBOOK_LOOSE_FOLDS },
    { ViridianCity_Mart_EventScript_ShelfEnd,          FLAG_GOT_OPUS },
};

static bool8 IsShown(const struct BgEvent *e)
{
    u32 i;

    if (e->kind <= BG_KIND_SIGN_LAST)
    {
        for (i = 0; i < ARRAY_COUNT(sFinds); i++)
        {
            if (e->bgUnion.script == sFinds[i].script)
                return !FlagGet(sFinds[i].foundFlag);
        }
    }

    if (e->kind == BG_KIND_HIDDEN_ITEM)
        return !FlagGet(GetHiddenItemAttr(e->bgUnion.hiddenItem, HIDDEN_ITEM_FLAG));
    //  T-235: ARTSAI's drawing under the station's visitor log, once the TRANSCRIPT has been read and until it is bound.
    if (e->kind <= BG_KIND_SIGN_LAST && e->bgUnion.script == Route21_North_Station_EventScript_VisitorLog)
        return FlagGet(FLAG_ARTSAI_PAGE) && !FlagGet(FLAG_ARTSAI_BOUND);
    if (e->kind <= BG_KIND_SIGN_LAST)
    {
        for (i = 0; i < ARRAY_COUNT(sGroveTrees); i++)
        {
            if (e->bgUnion.script == sGroveTrees[i])
                return TRUE;
        }
    }
    return FALSE;
}

void Reveal_OnMapLoad(void)
{
    sRevealTimer = REVEAL_PERIOD - REVEAL_FIRST;
}

void Reveal_Update(void)
{
    const struct MapEvents *events;
    const struct BgEvent *e;
    s16 px, py, dx, dy;
    u32 i;

    if (!FlagGet(FLAG_GOT_REVEAL))
        return;
    if (++sRevealTimer < REVEAL_PERIOD)
        return;
    sRevealTimer = 0;

    events = gMapHeader.events;
    PlayerGetDestCoords(&px, &py);
    for (i = 0; i < events->bgEventCount; i++)
    {
        e = &events->bgEvents[i];
        if (!IsShown(e))
            continue;
        dx = e->x + 7 - px;
        dy = e->y + 7 - py;
        if (dx < -8 || dx > 8 || dy < -6 || dy > 6)
            continue;
        gFieldEffectArguments[0] = e->x;
        gFieldEffectArguments[1] = e->y;
        gFieldEffectArguments[2] = 1;
        FieldEffectStart(FLDEFF_SPARKLE);
    }
}
