#include "global.h"
#include "battle.h"
#include "battle_anim.h"
#include "characters.h"
#include "daemon_saturation.h"
#include "daemon_streaks.h"
#include "daemons_spirits.h"
#include "event_data.h"
#include "item.h"
#include "main.h"
#include "malloc.h"
#include "palette.h"
#include "party_menu.h"
#include "pokemon.h"
#include "pokemon_storage_system.h"
#include "script.h"
#include "sound.h"
#include "sprite.h"
#include "string_util.h"
#include "task.h"
#include "trainer_pokemon_sprites.h"
#include "constants/battle.h"
#include "constants/items.h"
#include "constants/pokemon.h"
#include "constants/rgb.h"
#include "constants/songs.h"

//  T-519 item 1: SPIRITS (vision.md 4.40; the user, by multiple choice, 2026-11-01/02). DRAFT, every weight and word.
//
//  THE STATE. Two bits of class and two of lean, in the u16 at 0x1E (include/pokemon.h). The lean is read by class:
//
//      AS IT IS    lean 0 the middle, 1 one step towards good spirits, 2 one towards low, 3 two towards low
//      GOOD        lean 0 settled .. 3 three steps slipped back towards the middle
//      LOW         lean 0 deepest .. 3 three steps come back towards the middle
//
//  so the whole of it is one ladder of twelve rungs, and a step moves one rung:
//
//      LOW 0 1 2 3 | -2 -1 [0] +1 | GOOD 3 2 1 0
//
//  ...except where it crosses. Up from +1 is GOOD at 0 (settled); down from GOOD 3 is the middle, 0. Down from -2 is LOW
//  at 0; up from LOW 3 is the middle again. So two steps take a daemon from the middle into good spirits and FOUR take
//  it back out; three take it into low spirits and four bring it back. That is the hysteresis the brief asks for: a
//  daemon that has just crossed does not flicker back on the next small thing. The middle is the widest class, four
//  rungs, and lower is the longer way down -- the game errs on the kind side (craft rule 3: a daemon in low spirits
//  has been kept badly, and the way back is always open). Every step is reversible; nothing is ever locked.
//
//  WHAT MOVES IT. Each event is a CHANCE of one step, out of 256, so four bits can hold a slow drift: the expected
//  steps add up the way a running total would, with no total kept. The chance is drawn from the daemon's own
//  personality and the frame, never from Random(): the battle's and the field's random numbers are exactly what they
//  were, so no battle, encounter or theatre route comes out differently for this (the type chart is untouched too).
//
//      KEPT     a CHECKPOINT or a bed heals it            up    16   (1 in 16)
//               an item restores its HP                    up    16
//               a vitamin                                  up    16
//               the massage (CELADON's)                    up   128   (1 in 2: it is rare and it is care)
//               left fainted, every 128 steps              down  64   (about one step in 512 steps)
//               fainted from poison outside a battle       down 128
//               long in a box, every 128 steps             down   2   (about one step in 16,000: a playthrough)
//      BOND     friendship 220 or more, every 128 steps    up     8
//               friendship under 50, every 128 steps       down   8
//      WORK     a routine of its own type, in a battle     up     8   (1 in 32 uses)
//               sent out to face a leader, the ELITE or
//               the CHAMPION (as vanilla's friendship is)  up   128
//               a RARE CANDY                               down 128   (a level it did not work for)
//      PORTION  a fair fight: it fought, and won EXP from
//               a foe within 5 levels of its own           up    16
//               sent against far worse odds: it fainted
//               to a foe 10 or more levels above it        down 128
//
//  At these rates a lead that is healed, fought as itself and fairly comes into good spirits within an hour or two of
//  play, and one boxed all game drifts into low spirits about as the game ends. Food (only outside a battle, and only
//  what restores HP): in low spirits it refuses one time in three; in good spirits it thanks you one time in two.
//
//  THE COST. The four bits, which every copy of a BoxPokemon already carries (a box, a trade, the companion and back).
//  Resident: sPending, six u32, and sShowing, one, in IWRAM (28 bytes) -- the crossings waiting for the first step
//  in the field, since a crossing can happen in a battle or a menu where no screen can play. A soft reset loses them
//  (the class is already saved; only the screen is missed). The screen allocates from the heap.

#define W_HEAL          16
#define W_FED           16
#define W_VITAMIN       16
#define W_MASSAGE      128
#define W_LEFT_FAINTED  64
#define W_FIELD_FAINT  128
#define W_BOXED          2
#define W_BOND           8
#define W_OWN_TYPE       8
#define W_LEAGUE       128
#define W_RARE_CANDY   128
#define W_FAIR          16
#define W_BAD_ODDS     128

#define BOND_HIGH      220
#define BOND_LOW        50
#define FAIR_LEVELS      5
#define BAD_ODDS_LEVELS 10

#define FOOD_REFUSE     85      // in 256: one in three
#define FOOD_THANKS    128      // one in two

//  The tone: this share (of 32) of the way to white or to black. Saturation (friendship) is applied first, then this,
//  so a daemon keeps its type's colour at its own friendship's strength and is only lighter or darker over it.
#define TONE_LIGHT      10      // the user, 2026-11-04: plainer (was 6)
#define TONE_DARK       10

//  Each kind of event rolls on its own salt, so two in one frame are not the same draw.
enum { SALT_HEAL = 1, SALT_FED, SALT_VITAMIN, SALT_MASSAGE, SALT_FAINTED, SALT_FIELD_FAINT, SALT_BOXED, SALT_BOND,
       SALT_OWN_TYPE, SALT_LEAGUE, SALT_CANDY, SALT_FAIR, SALT_BAD_ODDS, SALT_REFUSE, SALT_THANKS };

enum { CROSS_NONE, CROSS_INTO_GOOD, CROSS_INTO_LOW, CROSS_OUT_OF_GOOD, CROSS_OUT_OF_LOW };

static u32 sPending[PARTY_SIZE];    // each: the personality's low 24 bits, and the crossing in the top 8
static u32 sShowing;                // the one the screen is playing

#if DAEMONS_DEBUG
static EWRAM_DATA bool8 sDebugSure = FALSE;
#endif

//  ---------------------------------------------------------------- the words (DRAFT, each line at most 208px)

static const u8 sText_InGood[] = _("In good spirits.");                                 // DRAFT
static const u8 sText_InLow[]  = _("In low spirits.");                                  // DRAFT
static const u8 sText_CrossIntoGood[] = _("{STR_VAR_1} is in good spirits.");           // DRAFT
static const u8 sText_CrossIntoLow[]  = _("{STR_VAR_1} is in low spirits.");            // DRAFT
static const u8 sText_CrossOutOfGood[] = _("{STR_VAR_1} is no longer\nin good spirits.");   // DRAFT
static const u8 sText_CrossOutOfLow[]  = _("{STR_VAR_1} is no longer\nin low spirits.");    // DRAFT
const u8 gText_SpiritsRefusesFood[] = _("{STR_VAR_1} turns away from it.\nIt will not take it now.{PAUSE_UNTIL_PRESS}");   // DRAFT: held for A, as
                                                     // vanilla's "It won't have any effect." is -- the menu closes after it
const u8 gText_SpiritsHPRestoredThanks[] = _("{STR_VAR_1}'s HP was restored\nby {STR_VAR_2} point(s).\p"
                                             "{STR_VAR_1} thanks you, brightly.{PAUSE_UNTIL_PRESS}");   // DRAFT

//  ---------------------------------------------------------------- the state

u8 Spirits_Class(const struct BoxPokemon *boxMon)
{
    return boxMon->daemonsSpirits == SPIRITS_GOOD || boxMon->daemonsSpirits == SPIRITS_LOW
         ? boxMon->daemonsSpirits : SPIRITS_AS_IT_IS;
}

u8 Spirits_Lean(const struct BoxPokemon *boxMon)
{
    return boxMon->daemonsLean;
}

static void Set(struct BoxPokemon *boxMon, u8 class, u8 lean)
{
    boxMon->daemonsSpirits = class;
    boxMon->daemonsLean = lean;
}

//  The middle's lean as a position: 0 the middle, 1 up, 2 one down, 3 two down.
static const s8 sMiddlePos[4] = { 0, 1, -1, -2 };

static u8 MiddleLean(s8 pos)
{
    return pos == 1 ? 1 : pos == -1 ? 2 : pos == -2 ? 3 : 0;
}

//  One step up or down the ladder (the top of this file); what it crossed, if anything.
static u8 Step(struct BoxPokemon *boxMon, bool8 up)
{
    u8 lean = boxMon->daemonsLean;
    s8 pos;

    switch (Spirits_Class(boxMon))
    {
    case SPIRITS_GOOD:
        if (up)
        {
            if (lean > 0)
                boxMon->daemonsLean = lean - 1;
            return CROSS_NONE;
        }
        if (lean < 3)
        {
            boxMon->daemonsLean = lean + 1;
            return CROSS_NONE;
        }
        Set(boxMon, SPIRITS_AS_IT_IS, 0);
        return CROSS_OUT_OF_GOOD;
    case SPIRITS_LOW:
        if (!up)
        {
            if (lean > 0)
                boxMon->daemonsLean = lean - 1;
            return CROSS_NONE;
        }
        if (lean < 3)
        {
            boxMon->daemonsLean = lean + 1;
            return CROSS_NONE;
        }
        Set(boxMon, SPIRITS_AS_IT_IS, 0);
        return CROSS_OUT_OF_LOW;
    default:
        pos = sMiddlePos[lean] + (up ? 1 : -1);
        if (pos > 1)
        {
            Set(boxMon, SPIRITS_GOOD, 0);
            return CROSS_INTO_GOOD;
        }
        if (pos < -2)
        {
            Set(boxMon, SPIRITS_LOW, 0);
            return CROSS_INTO_LOW;
        }
        Set(boxMon, SPIRITS_AS_IT_IS, MiddleLean(pos));
        return CROSS_NONE;
    }
}

//  A draw out of 256 from the daemon and the frame -- never Random() (the top of this file says why).
static u8 Roll(const struct BoxPokemon *boxMon, u8 salt)
{
    u32 x = boxMon->personality ^ (gMain.vblankCounter2 * 0x9E3779B1) ^ (salt * 0x85EBCA6B);

    x ^= x >> 15;
    x *= 0x2C1B3C6D;
    x ^= x >> 12;
    x *= 0x297A2D39;
    x ^= x >> 15;
    return x >> 24;
}

static bool8 IsReal(const struct BoxPokemon *boxMon)
{
    return boxMon->hasSpecies && !boxMon->isEgg && !boxMon->isBadEgg;
}

static void Queue(struct BoxPokemon *boxMon, u8 cross)
{
    u32 id = boxMon->personality & 0xFFFFFF;
    s32 i, free = -1;

    for (i = 0; i < PARTY_SIZE; i++)
    {
        if (sPending[i] != 0 && (sPending[i] & 0xFFFFFF) == id)
        {
            u8 was = sPending[i] >> 24;

            // into and straight back out before the screen could play: nothing to show
            if ((was == CROSS_INTO_GOOD && cross == CROSS_OUT_OF_GOOD) || (was == CROSS_INTO_LOW && cross == CROSS_OUT_OF_LOW)
             || (was == CROSS_OUT_OF_GOOD && cross == CROSS_INTO_GOOD) || (was == CROSS_OUT_OF_LOW && cross == CROSS_INTO_LOW))
                sPending[i] = 0;
            else
                sPending[i] = id | ((u32)cross << 24);
            return;
        }
        if (sPending[i] == 0 && free < 0)
            free = i;
    }
    if (free >= 0)
        sPending[free] = id | ((u32)cross << 24);
}

static bool8 InParty(const struct BoxPokemon *boxMon)
{
    return (const void *)boxMon >= (const void *)&gPlayerParty[0] && (const void *)boxMon < (const void *)&gPlayerParty[PARTY_SIZE];
}

//  The chance of one step; a party daemon's crossing waits for the field. A boxed daemon crosses quietly: it is seen
//  in its words and its tone the next time it is looked at.
static void Nudge(struct BoxPokemon *boxMon, bool8 up, u16 weight, u8 salt)
{
    u8 cross;

    if (!IsReal(boxMon) || boxMon->away || boxMon->trading)   // on the player's device, the companion keeps it
        return;
    if (Roll(boxMon, salt) >= weight)
        return;
    cross = Step(boxMon, up);
    if (cross != CROSS_NONE && InParty(boxMon))
        Queue(boxMon, cross);
}

//  ---------------------------------------------------------------- what moves it

void Spirits_OnHeal(void)
{
    s32 i;

    for (i = 0; i < PARTY_SIZE; i++)
        Nudge(&gPlayerParty[i].box, TRUE, W_HEAL, SALT_HEAL);
}

void Spirits_OnWalk(void)
{
    s32 i, j;

    for (i = 0; i < PARTY_SIZE; i++)
    {
        struct Pokemon *mon = &gPlayerParty[i];
        u8 friendship;

        if (!IsReal(&mon->box))
            continue;
        if (GetMonData(mon, MON_DATA_HP) == 0)
        {
            Nudge(&mon->box, FALSE, W_LEFT_FAINTED, SALT_FAINTED);
            continue;
        }
        friendship = GetMonData(mon, MON_DATA_FRIENDSHIP);
        if (friendship >= BOND_HIGH)
            Nudge(&mon->box, TRUE, W_BOND, SALT_BOND);
        else if (friendship < BOND_LOW)
            Nudge(&mon->box, FALSE, W_BOND, SALT_BOND);
    }
    // long in a box: the bits are outside the encrypted record and its checksum, so this reads and writes them bare
    for (i = 0; i < TOTAL_BOXES_COUNT; i++)
        for (j = 0; j < IN_BOX_COUNT; j++)
            Nudge(GetBoxedMonPtr(i, j), FALSE, W_BOXED, SALT_BOXED);
}

void Spirits_OnFriendshipEvent(struct Pokemon *mon, u8 event)
{
    switch (event)
    {
    case FRIENDSHIP_EVENT_VITAMIN:
        Nudge(&mon->box, TRUE, W_VITAMIN, SALT_VITAMIN);
        break;
    case FRIENDSHIP_EVENT_MASSAGE:
        Nudge(&mon->box, TRUE, W_MASSAGE, SALT_MASSAGE);
        break;
    case FRIENDSHIP_EVENT_FAINT_OUTSIDE_BATTLE:
        Nudge(&mon->box, FALSE, W_FIELD_FAINT, SALT_FIELD_FAINT);
        break;
    case FRIENDSHIP_EVENT_LEAGUE_BATTLE:    // at a battle's start; AdjustFriendship returned unless a leader, ELITE, CHAMPION
        Nudge(&mon->box, TRUE, W_LEAGUE, SALT_LEAGUE);
        break;
    }
}

static bool8 OwnBattle(void)
{
    return !(gBattleTypeFlags & (BATTLE_TYPE_LINK | BATTLE_TYPE_MULTI | BATTLE_TYPE_POKEDUDE | BATTLE_TYPE_SAFARI));
}

void Spirits_OnOwnType(u8 battler)
{
    if (GetBattlerSide(battler) != B_SIDE_PLAYER || !OwnBattle())
        return;
    Nudge(&gPlayerParty[gBattlerPartyIndexes[battler]].box, TRUE, W_OWN_TYPE, SALT_OWN_TYPE);
}

void Spirits_OnFairFight(struct Pokemon *mon, u8 foeLevel)
{
    s32 gap = (s32)foeLevel - (s32)GetMonData(mon, MON_DATA_LEVEL);

    if (!OwnBattle() || gap > FAIR_LEVELS || gap < -FAIR_LEVELS)
        return;
    Nudge(&mon->box, TRUE, W_FAIR, SALT_FAIR);
}

void Spirits_OnFaint(struct Pokemon *mon, u8 foeLevel, u8 ownLevel)
{
    if (!OwnBattle() || foeLevel < ownLevel + BAD_ODDS_LEVELS)
        return;
    Nudge(&mon->box, FALSE, W_BAD_ODDS, SALT_BAD_ODDS);
}

void Spirits_OnFed(struct Pokemon *mon)
{
    Nudge(&mon->box, TRUE, W_FED, SALT_FED);
}

void Spirits_OnRareCandy(struct Pokemon *mon)
{
    Nudge(&mon->box, FALSE, W_RARE_CANDY, SALT_CANDY);
}

//  ---------------------------------------------------------------- how it behaves

static bool8 Chance(struct Pokemon *mon, u16 weight, u8 salt)
{
#if DAEMONS_DEBUG
    if (sDebugSure)
        return TRUE;
#endif
    return Roll(&mon->box, salt) < weight;
}

//  Food is what restores HP, given outside a battle to a daemon still standing: a revive is never refused.
//  (party_menu.c asks only for an item that restores HP.)
bool8 Spirits_RefusesFood(struct Pokemon *mon)
{
    if (gMain.inBattle || !IsReal(&mon->box) || Spirits_Class(&mon->box) != SPIRITS_LOW)
        return FALSE;
    if (GetMonData(mon, MON_DATA_HP) == 0)
        return FALSE;
    return Chance(mon, FOOD_REFUSE, SALT_REFUSE);
}

bool8 Spirits_Thanks(struct Pokemon *mon)
{
    if (gMain.inBattle || !IsReal(&mon->box) || Spirits_Class(&mon->box) != SPIRITS_GOOD)
        return FALSE;
    return Chance(mon, FOOD_THANKS, SALT_THANKS);
}

//  ---------------------------------------------------------------- how it shows

u8 Spirits_ToneOf(const struct BoxPokemon *boxMon)
{
    switch (Spirits_Class(boxMon))
    {
    case SPIRITS_GOOD: return SPIRITS_TONE_LIGHT;
    case SPIRITS_LOW:  return SPIRITS_TONE_DARK;
    }
    return SPIRITS_TONE_NONE;
}

const u8 *Spirits_Words(const struct BoxPokemon *boxMon)
{
    switch (Spirits_Class(boxMon))
    {
    case SPIRITS_GOOD: return sText_InGood;
    case SPIRITS_LOW:  return sText_InLow;
    }
    return NULL;
}

//  Toward white (or black) by share/32 in each channel: the hue, the angle between the channels, is kept.
static u16 Blend(u16 colour, u16 target, s32 share)
{
    s32 r = colour & 31, g = (colour >> 5) & 31, b = (colour >> 10) & 31;
    s32 tr = target & 31, tg = (target >> 5) & 31, tb = (target >> 10) & 31;

    r += (tr - r) * share / 32;
    g += (tg - g) * share / 32;
    b += (tb - b) * share / 32;
    return RGB(r, g, b) | (colour & 0x8000);
}

static u16 ToneColour(u16 colour, u8 tone)
{
    if (tone == SPIRITS_TONE_LIGHT)
        return Blend(colour, RGB_WHITE, TONE_LIGHT);
    if (tone == SPIRITS_TONE_DARK)
        return Blend(colour, RGB_BLACK, TONE_DARK);
    return colour;
}

void Spirits_ToneBuffer(u16 *palette, u8 tone)
{
    s32 i;

    if (tone == SPIRITS_TONE_NONE)
        return;
    for (i = 1; i < 16; i++)
        palette[i] = ToneColour(palette[i], tone);
}

void Spirits_ToneLoaded(u16 paletteOffset, u8 tone)
{
    s32 i;

    if (tone == SPIRITS_TONE_NONE)
        return;
    for (i = 1; i < 16; i++)
    {
        u16 colour = ToneColour(gPlttBufferUnfaded[paletteOffset + i], tone);

        gPlttBufferUnfaded[paletteOffset + i] = colour;
        gPlttBufferFaded[paletteOffset + i] = colour;
    }
}

//  ---------------------------------------------------------------- the crossing

static struct Pokemon *FindPending(u32 entry)
{
    u8 cross = entry >> 24;
    s32 i;

    for (i = 0; i < PARTY_SIZE; i++)
    {
        struct Pokemon *mon = &gPlayerParty[i];
        u8 class;

        if (!IsReal(&mon->box) || (mon->box.personality & 0xFFFFFF) != (entry & 0xFFFFFF))
            continue;
        class = Spirits_Class(&mon->box);
        // still where the crossing left it (a stale entry, or one undone since, shows nothing)
        if ((cross == CROSS_INTO_GOOD && class == SPIRITS_GOOD) || (cross == CROSS_INTO_LOW && class == SPIRITS_LOW)
         || ((cross == CROSS_OUT_OF_GOOD || cross == CROSS_OUT_OF_LOW) && class == SPIRITS_AS_IT_IS))
            return mon;
        return NULL;
    }
    return NULL;
}

bool8 Daemons_TrySpiritsCrossing(void)
{
    static const u8 *const sLines[] = {
        [CROSS_INTO_GOOD] = sText_CrossIntoGood, [CROSS_INTO_LOW] = sText_CrossIntoLow,
        [CROSS_OUT_OF_GOOD] = sText_CrossOutOfGood, [CROSS_OUT_OF_LOW] = sText_CrossOutOfLow,
    };
    s32 i;

    for (i = 0; i < PARTY_SIZE; i++)
    {
        u32 entry = sPending[i];
        u8 cross = entry >> 24;
        struct Pokemon *mon;

        if (entry == 0)
            continue;
        sPending[i] = 0;
        if (cross < CROSS_INTO_GOOD || cross > CROSS_OUT_OF_LOW || (mon = FindPending(entry)) == NULL)
            continue;
        sShowing = entry;
        GetMonNickname(mon, gStringVar1);
        StringExpandPlaceholders(gStringVar4, sLines[cross]);
        return TRUE;
    }
    return FALSE;
}

//  The screen, in the evolution's style but lighter (the user: "an evolution-like screen"): on the black of a
//  fadescreen, the daemon as it was, its cry; it pulses -- to white rising, to black falling, quicker each time, as an
//  evolution flickers -- and at the last pulse's height it is its new tone; then a fanfare rising, a hush falling.
#define TAG_SPIRITS 0x5A19

#define tState   data[0]
#define tTimer   data[1]
#define tSprite  data[2]
#define tPulse   data[3]
#define tSlot    data[4]
#define tCross   data[5]
#define tSpecies data[6]
#define tBufLo   data[7]
#define tBufHi   data[8]

static const u8 sPulseFrames[] = { 40, 32, 24, 18, 14, 12, 10 };

static u16 *Buffers(u8 taskId)
{
    return (u16 *)((u16)gTasks[taskId].tBufLo | ((u32)(u16)gTasks[taskId].tBufHi << 16));
}

static void WritePalette(u8 slot, const u16 *colours)
{
    s32 i;

    for (i = 0; i < 16; i++)
    {
        gPlttBufferUnfaded[OBJ_PLTT_ID(slot) + i] = colours[i];
        gPlttBufferFaded[OBJ_PLTT_ID(slot) + i] = colours[i];
    }
}

static void Task_SpiritsCrossing(u8 taskId);

void Spirits_PlayCrossing(void)
{
    u8 taskId = CreateTask(Task_SpiritsCrossing, 80);

    gTasks[taskId].tCross = sShowing >> 24;
}

static void EndCrossing(u8 taskId)
{
    struct Task *task = &gTasks[taskId];
    u16 *buf = Buffers(taskId);

    if (task->tSprite != MAX_SPRITES)
        FreeAndDestroyMonPicSprite(task->tSprite);
    FreeSpritePaletteByTag(TAG_SPIRITS);
    if (buf != NULL)
        Free(buf);
    sShowing = 0;
    DestroyTask(taskId);
    ScriptContext_Enable();
}

static void Task_SpiritsCrossing(u8 taskId)
{
    struct Task *task = &gTasks[taskId];
    bool8 rising = task->tCross == CROSS_INTO_GOOD || task->tCross == CROSS_OUT_OF_LOW;
    u16 *buf;
    s32 i, k, half;

    switch (task->tState)
    {
    case 0:
    {
        struct Pokemon *mon = FindPending(sShowing);
        u16 species, moves[MAX_MON_MOVES];
        u8 before, after;
        u32 otId, personality;

        task->tSprite = MAX_SPRITES;
        buf = AllocZeroed(16 * 2 * sizeof(u16));    // [0..15] as it was, [16..31] as it is now
        task->tBufLo = (u32)buf;
        task->tBufHi = (u32)buf >> 16;
        task->tSlot = AllocSpritePalette(TAG_SPIRITS);
        if (mon == NULL || buf == NULL || task->tSlot == 0xFF)
        {
            EndCrossing(taskId);
            return;
        }
        species = GetMonData(mon, MON_DATA_SPECIES);
        otId = GetMonData(mon, MON_DATA_OT_ID);
        personality = GetMonData(mon, MON_DATA_PERSONALITY);
        task->tSpecies = species;
        task->tSprite = CreateMonPicSprite(species, otId, personality, TRUE, 120, 64, task->tSlot, TAG_NONE, FALSE);
        if ((u16)task->tSprite == 0xFFFF)
        {
            task->tSprite = MAX_SPRITES;
            EndCrossing(taskId);
            return;
        }
        // its own look: streaks, then its friendship's saturation (as the summary draws it), then each tone
        Streaks_MovesOfMon(mon, moves);
        Streaks_ApplyToLoaded(OBJ_PLTT_ID(task->tSlot), species, moves);
        Saturation_ApplyToLoaded(OBJ_PLTT_ID(task->tSlot), GetMonData(mon, MON_DATA_FRIENDSHIP));
        before = task->tCross == CROSS_OUT_OF_GOOD ? SPIRITS_TONE_LIGHT : task->tCross == CROSS_OUT_OF_LOW ? SPIRITS_TONE_DARK : SPIRITS_TONE_NONE;
        after = Spirits_ToneOf(&mon->box);
        for (i = 0; i < 16; i++)
        {
            buf[i] = i == 0 ? gPlttBufferUnfaded[OBJ_PLTT_ID(task->tSlot)] : ToneColour(gPlttBufferUnfaded[OBJ_PLTT_ID(task->tSlot) + i], before);
            buf[16 + i] = i == 0 ? buf[0] : ToneColour(gPlttBufferUnfaded[OBJ_PLTT_ID(task->tSlot) + i], after);
        }
        WritePalette(task->tSlot, buf);
        PlayCry_Normal(species, 0);
        task->tTimer = 0;
        task->tState++;
        break;
    }
    case 1:     // the daemon as it was, a moment
        if (++task->tTimer >= 60)
        {
            task->tTimer = 0;
            task->tPulse = 0;
            task->tState++;
        }
        break;
    case 2:     // the pulses
    {
        u16 colours[16];
        u8 frames = sPulseFrames[task->tPulse];
        bool8 last = task->tPulse == ARRAY_COUNT(sPulseFrames) - 1;

        buf = Buffers(taskId);
        half = frames / 2;
        k = task->tTimer <= half ? task->tTimer * 32 / half : (frames - task->tTimer) * 32 / half;
        for (i = 0; i < 16; i++)
        {
            const u16 *base = (last && task->tTimer > half) ? &buf[16] : &buf[0];

            colours[i] = i == 0 ? base[0] : Blend(base[i], rising ? RGB_WHITE : RGB_BLACK, k * 3 / 4);
        }
        WritePalette(task->tSlot, colours);
        if (++task->tTimer > frames)
        {
            task->tTimer = 0;
            if (last)
                task->tState++;
            else
                task->tPulse++;
        }
        break;
    }
    case 3:     // as it is now, and the sound of it
        WritePalette(task->tSlot, &Buffers(taskId)[16]);
        if (task->tCross == CROSS_INTO_GOOD)
            PlayFanfare(MUS_EVOLVED);
        else if (task->tCross == CROSS_OUT_OF_LOW)
            PlaySE(SE_M_HEAL_BELL);
        else
            PlaySE(SE_M_HAZE);
        task->tTimer = 0;
        task->tState++;
        break;
    case 4:
        if (++task->tTimer >= 90 && IsFanfareTaskInactive() && !IsSEPlaying())
            EndCrossing(taskId);
        break;
    }
}

#undef tState
#undef tTimer
#undef tSprite
#undef tPulse
#undef tSlot
#undef tCross
#undef tSpecies
#undef tBufLo
#undef tBufHi

//  ---------------------------------------------------------------- DEBUG -> GAME EVENTS -> SPIRITS

#if DAEMONS_DEBUG

//  "1 MIDDLE 0" -- a party slot's class and lean, in plain words (the debug menu is not the game: still no Greek).
void Spirits_DebugLabel(u8 *label, u8 slot)
{
    static const u8 sMiddle[] = _("AS IT IS, LEAN ");
    static const u8 sGood[]   = _("GOOD, SLIPPED ");
    static const u8 sLow[]    = _("LOW, COME BACK ");
    static const u8 sEgg[]    = _("EGG");
    static const u8 sUp[]     = _(" (UP)");
    static const u8 sDown[]   = _(" (DOWN)");
    struct BoxPokemon *boxMon = &gPlayerParty[slot].box;
    u8 *p = label;
    u8 class;

    *p++ = CHAR_1 + slot;
    *p++ = CHAR_SPACE;
    *p = EOS;
    if (!boxMon->hasSpecies)
    {
        *p++ = CHAR_HYPHEN;
        *p = EOS;
        return;
    }
    if (boxMon->isEgg)
    {
        StringCopy(p, sEgg);
        return;
    }
    class = Spirits_Class(boxMon);
    p = StringCopy(p, class == SPIRITS_GOOD ? sGood : class == SPIRITS_LOW ? sLow : sMiddle);
    p = ConvertIntToDecimalStringN(p, boxMon->daemonsLean, STR_CONV_MODE_LEFT_ALIGN, 1);
    if (class == SPIRITS_AS_IT_IS && boxMon->daemonsLean != 0)
        StringCopy(p, boxMon->daemonsLean == 1 ? sUp : sDown);
}

void Spirits_DebugStepLead(bool8 up)
{
    struct BoxPokemon *boxMon = &gPlayerParty[0].box;
    u8 cross;

    if (!IsReal(boxMon))
        return;
    cross = Step(boxMon, up);
    if (cross != CROSS_NONE)
        Queue(boxMon, cross);
}

void Spirits_DebugSetLead(u8 class)
{
    struct BoxPokemon *boxMon = &gPlayerParty[0].box;
    u8 was;

    if (!IsReal(boxMon))
        return;
    was = Spirits_Class(boxMon);
    Set(boxMon, class, 0);
    if (was == class)
        return;
    if (was == SPIRITS_GOOD)
        Queue(boxMon, CROSS_OUT_OF_GOOD);
    else if (was == SPIRITS_LOW)
        Queue(boxMon, CROSS_OUT_OF_LOW);
    if (class == SPIRITS_GOOD)
        Queue(boxMon, CROSS_INTO_GOOD);
    else if (class == SPIRITS_LOW)
        Queue(boxMon, CROSS_INTO_LOW);
}

void Spirits_DebugToggleSure(void)
{
    sDebugSure = !sDebugSure;
}

bool8 Spirits_DebugSure(void)
{
    return sDebugSure;
}

#endif
