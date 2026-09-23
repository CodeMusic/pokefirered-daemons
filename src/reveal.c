#include "global.h"
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
//  hidden item within a screen of the player shimmers where it lies.

#define REVEAL_PERIOD        90
#define REVEAL_FIRST         60       // frames after arrival before the first shimmer
#define BG_KIND_HIDDEN_ITEM  7        // the same literal itemfinder.c reads

static EWRAM_DATA u8 sRevealTimer = 0;

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
        if (e->kind != BG_KIND_HIDDEN_ITEM)
            continue;
        if (FlagGet(GetHiddenItemAttr(e->bgUnion.hiddenItem, HIDDEN_ITEM_FLAG)))
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
