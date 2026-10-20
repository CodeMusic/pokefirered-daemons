#include "global.h"
#include "gflib.h"
#include "task.h"
#include "random.h"
#include "pokemon.h"
#include "pokedex.h"
#include "pokedex_screen.h"
#include "pokemon_icon.h"
#include "play.h"
#include "m4a.h"
#include "daemons_casino.h"
#include "daemons_time.h"
#include "daemons_weather.h"
#include "daemons_holidays.h"
#include "constants/holidays.h"
#include "region_map.h"
#include "string_util.h"
#include "event_data.h"

// T-422 THE CASINO, IN THE GAME (the user, 2026-10-18, confirmed by phone): the GAME CORNER's reels are the companion's
// -- three reels of daemons (the lead the jackpot; the others from the INDEX seen, at home in the season or the hour
// first), a coin and the season's piece; the result decided at the pull from the companion's table, between 90 and
// 100 back of every 100 bet as the day's luck has it; the cat and the string; near misses; and its tunes on the day's
// note. The numbers are the companion's (server/src/slots.ts, firmware/common/slots.h), carried in by DAEMONS
// tools/gencasino.py and held to them there -- here in whole numbers: ten-thousandths of a return, hundredths of luck.
//
// The luck follows the GAME's world, never the real one's: the companion's devices follow the real world, the game its
// own -- its holiday (src/daemons_holidays.c), the sky over the city outside (src/daemons_weather.c), its watch and its
// season by edition (src/daemons_time.c).
#include "data/daemons_casino.h"

// ------------------------------------------------------------------------------------------------------------ the day

static s16 Clamp(s32 v, s32 lo, s32 hi)
{
    return v < lo ? lo : v > hi ? hi : v;
}

static u16 ReturnAt(s16 luck)
{
    return Clamp(CASINO_RETURN_MID + CASINO_RETURN_PER * Clamp(luck, -100, 100), CASINO_RETURN_LOW, CASINO_RETURN_HIGH);
}

static const u8 sDayNotes[WEEKDAY_COUNT] = {0, 2, 4, 5, 7, 9, 11};   // Sunday C .. Saturday B (T-274)

void DaemonsCasino_ReadDay(struct DaemonsCasinoDay *day)
{
    const struct CasinoHoliday *h = NULL;
    s16 effect, best = -1;
    bool8 dressed = FALSE;
    u8 abs;

    DaemonsTimeRefresh();
    day->holiday = DaemonsHoliday();
    day->sky = DaemonsSkyAt(gMapHeader.regionMapSectionId);   // the city's sky, whatever the hall's roof says
    day->watch = DaemonsWatch();
    day->season = DaemonsSeason();
    day->dayNote = sDayNotes[DaemonsWeekday() % WEEKDAY_COUNT];
    day->luck = 0;
    day->why = NULL;
    day->whyFrom = CASINO_WHY_NONE;

    // the holiday (context.ts: the lucky one is luck itself, any other a festive table, a quiet day left as it is)
    if (day->holiday != HOLIDAY_NONE && day->holiday <= HOLIDAY_COUNT)
    {
        h = &sCasinoHolidays[day->holiday];
        if (!(DaemonsHolidayFlags(day->holiday) & HF_QUIET))
        {
            dressed = TRUE;
            effect = h->lucky ? CASINO_LUCK_LUCKY_DAY : CASINO_LUCK_FESTIVE;
            day->luck += effect;
            best = effect;
            day->why = h->lucky ? sCasinoWhyLucky : sCasinoWhyFestive;
            day->whyFrom = CASINO_WHY_HOLIDAY;
        }
    }
    // the sky, the watch, the season -- and the biggest of them says why
    effect = sCasinoLuckSky[day->sky % DAEMONS_SKY_COUNT];
    day->luck += effect;
    abs = effect < 0 ? -effect : effect;
    if (abs > best)
        best = abs, day->why = sCasinoWhySky[day->sky % DAEMONS_SKY_COUNT], day->whyFrom = CASINO_WHY_SKY;
    effect = sCasinoLuckWatch[day->watch % WATCH_COUNT];
    day->luck += effect;
    abs = effect < 0 ? -effect : effect;
    if (abs > best)
        best = abs, day->why = sCasinoWhyWatch[day->watch % WATCH_COUNT], day->whyFrom = CASINO_WHY_WATCH;
    effect = sCasinoLuckSeason[day->season % SEASON_COUNT];
    day->luck += effect;
    abs = effect < 0 ? -effect : effect;
    if (abs > best)
        best = abs, day->why = sCasinoWhySeason[day->season % SEASON_COUNT], day->whyFrom = CASINO_WHY_SEASON;
    day->luck = Clamp(day->luck, -100, 100);
    day->ret = ReturnAt(day->luck);

    // how it is dressed (context.ts casinoDress): the holiday's look, else the weather's when it is weather to notice
    // (a clear night, its stars), else the season's own; the holiday's colours, else the season's; its hat
    if (dressed && h->look != CASINO_LOOK_NONE)
        day->look = h->look;
    else if (day->sky == DAEMONS_SKY_RAIN)
        day->look = CASINO_LOOK_RAIN;
    else if (day->sky == DAEMONS_SKY_STORM)
        day->look = CASINO_LOOK_STORM;
    else if (day->sky == DAEMONS_SKY_FOG)
        day->look = CASINO_LOOK_FOG;
    else if (day->sky == DAEMONS_SKY_SNOW)
        day->look = CASINO_LOOK_SNOW;
    else if (day->sky == DAEMONS_SKY_CLEAR && day->watch == WATCH_NIGHT)
        day->look = CASINO_LOOK_STARS;
    else
        day->look = sCasinoSeasonLook[day->season % SEASON_COUNT];
    day->colourA = dressed && h->a ? h->a : sCasinoSeasonColours[day->season % SEASON_COUNT][0];
    day->colourB = dressed && h->b ? h->b : sCasinoSeasonColours[day->season % SEASON_COUNT][1];
    day->hat = dressed ? h->hat : CASINO_HAT_NONE;
    day->flip = dressed && h->flip;
}

// context.ts's reasons in full: the holiday's name before its words, the city after the sky's, the edition's year
// after the season's (T-422's fix, 2026-10-18: CLOVER DAY's card said only "the luck of the day"). DRAFT.
#if defined(FIRERED)
static const u8 sText_CasinoYear[] = _(" (CONTENT's year)");   // DRAFT
#else
static const u8 sText_CasinoYear[] = _(" (CONTEXT's year)");   // DRAFT
#endif
static const u8 sText_CasinoIn[] = _(" in ");                  // DRAFT
static const u8 sText_CasinoColon[] = _(": ");

void DaemonsCasino_WhyText(const struct DaemonsCasinoDay *day, u8 *dest)
{
    const u8 *name;

    *dest = EOS;
    if (day->why == NULL)
        return;
    switch (day->whyFrom)
    {
    case CASINO_WHY_HOLIDAY:
        name = DaemonsHolidayName(day->holiday);
        if (name != NULL)
            dest = StringAppend(StringCopy(dest, name), sText_CasinoColon);
        StringCopy(dest, day->why);
        break;
    case CASINO_WHY_SKY:
        dest = StringAppend(StringCopy(dest, day->why), sText_CasinoIn);
        GetMapNameGeneric(dest, gMapHeader.regionMapSectionId);
        break;
    case CASINO_WHY_SEASON:
        StringAppend(StringCopy(dest, day->why), sText_CasinoYear);
        break;
    default:
        StringCopy(dest, day->why);
        break;
    }
}

// ---------------------------------------------------------------------------------------------------------- the pull

u16 DaemonsCasino_PullReturn(const struct DaemonsCasinoDay *day, const struct DaemonsCasinoCat *cat)
{
    return Clamp(day->ret + CASINO_REACH_SPAN * cat->reach / 10000, CASINO_PULL_LOW, CASINO_PULL_HIGH);
}

// weightsFor: every win's weight scaled alike to the return, the loss taking up the rest (rounded half up, as
// Math.round does -- gencasino.py holds this to slots.ts for every return)
static void WeightsFor(u16 ret, u16 *w)
{
    u32 o, wins = 0;

    for (o = 1; o < CASINO_OUTCOME_COUNT; o++)
    {
        w[o] = (2 * sCasinoWeights[o] * 10 * ret + CASINO_BASE_RETURN) / (2 * CASINO_BASE_RETURN);
        wins += w[o];
    }
    w[0] = CASINO_FINE - wins;
}

u8 DaemonsCasino_Draw(u16 ret)
{
    u16 w[CASINO_OUTCOME_COUNT];
    s32 r;
    u8 o;

    WeightsFor(ret, w);
    r = (u32)Random32() % CASINO_FINE;   // as u32: Random32()'s int goes negative half the time
    for (o = 0; o < CASINO_OUTCOME_COUNT; o++)
    {
        if (r < w[o])
            return o;
        r -= w[o];
    }
    return CASINO_LOSE;
}

// learn: a step against the error (a real win, 2x and up, against how often one should come), and a little noise
void DaemonsCasino_Learn(struct DaemonsCasinoCat *cat, u8 outcome)
{
    bool8 won = sCasinoPays[outcome] >= 2;
    s32 noise = (s32)(Random() % (2 * CASINO_NOISE + 1)) - CASINO_NOISE;

    cat->reach = Clamp(cat->reach + (won ? -CASINO_LEARN_WON : CASINO_LEARN_LOST) + noise, -10000, 10000);
    cat->dry = won ? 0 : (cat->dry < 0xFFFF ? cat->dry + 1 : cat->dry);
    cat->streak = won ? (cat->streak < 0xFFFF ? cat->streak + 1 : cat->streak) : 0;
    if (cat->pulls < 0xFFFF)
        cat->pulls++;
}

u16 DaemonsCasino_NearChance(const struct DaemonsCasinoCat *cat)
{
    s32 c = CASINO_NEAR_BASE + CASINO_NEAR_PER * (cat->reach > 0 ? cat->reach : 0) / 100;
    return c > CASINO_NEAR_MAX ? CASINO_NEAR_MAX : c;
}

u8 DaemonsCasino_Pays(u8 outcome)
{
    return sCasinoPays[outcome % CASINO_OUTCOME_COUNT];
}

u8 DaemonsCasino_OutcomeOf(u8 a, u8 b, u8 c)
{
    if (a == b && b == c)
        return a == CASINO_JACK ? CASINO_JACK3 : a == CASINO_COIN ? CASINO_COIN3 : a == CASINO_SEASON ? CASINO_SEASON3 : CASINO_FRIEND3;
    if (a == CASINO_JACK && b == CASINO_JACK)
        return CASINO_JACK2;
    if (a == CASINO_SEASON)
        return CASINO_SEASON1;
    return CASINO_LOSE;
}

void DaemonsCasino_PiecesFor(u8 o, u8 out[3])
{
    u32 tries;

    switch (o)
    {
    case CASINO_JACK3:   out[0] = out[1] = out[2] = CASINO_JACK; return;
    case CASINO_FRIEND3: out[0] = out[1] = out[2] = CASINO_F1 + Random() % 3; return;
    case CASINO_COIN3:   out[0] = out[1] = out[2] = CASINO_COIN; return;
    case CASINO_SEASON3: out[0] = out[1] = out[2] = CASINO_SEASON; return;
    case CASINO_JACK2:
        out[0] = out[1] = CASINO_JACK;
        do { out[2] = Random() % CASINO_PIECE_COUNT; } while (out[2] == CASINO_JACK);
        return;
    }
    for (tries = 0; tries < 500; tries++)
    {
        out[0] = o == CASINO_SEASON1 ? CASINO_SEASON : Random() % CASINO_PIECE_COUNT;
        out[1] = Random() % CASINO_PIECE_COUNT;
        out[2] = Random() % CASINO_PIECE_COUNT;
        if (DaemonsCasino_OutcomeOf(out[0], out[1], out[2]) == o)
            return;
    }
    out[0] = CASINO_COIN, out[1] = CASINO_F1, out[2] = CASINO_F2;   // a loss, whatever the dice did
}

// the pieces beside each place the wanted one sits on the third reel
static u8 BesideOn(const u8 *reel, u8 wanted, u8 *out)
{
    u8 i, n = 0;

    for (i = 0; i < CASINO_STRIP; i++)
    {
        if (reel[i] == wanted)
        {
            out[n++] = reel[(i + CASINO_STRIP - 1) % CASINO_STRIP];
            out[n++] = reel[(i + 1) % CASINO_STRIP];
        }
    }
    return n;
}

// a loss that ALMOST won: two alike (a friend or the coin -- two JACKs would pay) and a third that sits beside the
// piece it wanted on the third reel, so that one shows one place off the line (slots.ts nearPieces)
bool8 DaemonsCasino_NearPieces(const u8 *reel3, u8 out[3], u8 *wanted)
{
    u8 order[4] = {CASINO_F1, CASINO_F2, CASINO_F3, CASINO_COIN};
    u8 near[2 * CASINO_STRIP], ok[2 * CASINO_STRIP];
    u8 i, j, k, n, m, t, pick;

    for (i = 3; i > 0; i--)
    {
        j = Random() % (i + 1);
        t = order[i], order[i] = order[j], order[j] = t;
    }
    for (i = 0; i < 4; i++)
    {
        pick = order[i];
        n = BesideOn(reel3, pick, near);
        for (k = 0, m = 0; k < n; k++)
            if (near[k] != pick && DaemonsCasino_OutcomeOf(pick, pick, near[k]) == CASINO_LOSE)
                ok[m++] = near[k];
        if (m)
        {
            out[0] = out[1] = pick;
            out[2] = ok[Random() % m];
            *wanted = pick;
            return TRUE;
        }
    }
    return FALSE;
}

// two JACKs (a 2x): the third reel's piece one beside a JACK -- almost the jackpot (slots.ts almostThird)
bool8 DaemonsCasino_AlmostThird(const u8 *reel3, u8 *third)
{
    u8 near[2 * CASINO_STRIP], ok[2 * CASINO_STRIP];
    u8 k, n = BesideOn(reel3, CASINO_JACK, near), m = 0;

    for (k = 0; k < n; k++)
        if (near[k] != CASINO_JACK)
            ok[m++] = near[k];
    if (!m)
        return FALSE;
    *third = ok[Random() % m];
    return TRUE;
}

// ----------------------------------------------------------------------------------------------------- the daemons

// context.ts reelSpecies: only daemons the INDEX has SEEN, those at home in this season or this watch first (four
// each), then the party's (two), and among equals a choice that turns each hour of the game's clock
void DaemonsCasino_ReelSpecies(const struct DaemonsCasinoDay *day, u16 species[4], u32 personality[4])
{
    u32 best[3] = {0, 0, 0};
    u16 sp, party[PARTY_SIZE], dayNum, minute;
    u32 i, k, n = 0, score, native, hour;

    species[0] = SPECIES_NONE;
    personality[0] = 0;
    for (i = 0; i < PARTY_SIZE; i++)
    {
        party[i] = GetMonData(&gPlayerParty[i], MON_DATA_SPECIES_OR_EGG);
        if (party[i] == SPECIES_EGG)
            party[i] = SPECIES_NONE;
        if (species[0] == SPECIES_NONE && party[i] != SPECIES_NONE)
        {
            species[0] = party[i];
            personality[0] = GetMonData(&gPlayerParty[i], MON_DATA_PERSONALITY);
        }
    }
    DaemonsDayAndMinute(&dayNum, &minute);
    hour = dayNum * 24 + minute / 60;
    for (k = 1; k < 4; k++)
        species[k] = SPECIES_NONE, personality[k] = 0;
    for (sp = 1; sp < NUM_SPECIES; sp++)
    {
        if (sp == species[0] || !GetSetPokedexFlag(SpeciesToNationalPokedexNum(sp), FLAG_GET_SEEN))
            continue;
        native = DaemonsNativeOf(sp);
        score = 0;
        if (native & (1 << (4 + day->season)))
            score += 4;
        if (native & (1 << (8 + day->watch)))
            score += 4;
        for (i = 0; i < PARTY_SIZE; i++)
            if (party[i] == sp)
                score += 2;
        score = score * 1000 + ((u32)(sp * 2654435761u + hour * 40503u)) % 1000;
        score++;   // 0 is "none yet"
        for (k = 0; k < 3; k++)
        {
            if (score > best[k])
            {
                for (i = 2; i > k; i--)
                    best[i] = best[i - 1], species[i + 1] = species[i];
                best[k] = score, species[k + 1] = sp;
                n++;
                break;
            }
        }
    }
    // fewer than three seen: the party's others, then the lead again
    for (k = 1; k < 4; k++)
    {
        if (species[k] != SPECIES_NONE)
            continue;
        for (i = 0; i < PARTY_SIZE; i++)
            if (party[i] != SPECIES_NONE && party[i] != species[0] && party[i] != species[1] && party[i] != species[2] && party[i] != species[3])
                break;
        species[k] = i < PARTY_SIZE ? party[i] : species[0];
    }
}

// --------------------------------------------------------------------------------------------------------- the tunes

struct CasinoTune { const struct CasinoNote *notes; u8 n; };
#define TUNE(t) {t, ARRAY_COUNT(t)}
static const struct CasinoTune sTunes[CASINO_TUNE_COUNT] = {
    [CASINO_TUNE_PULL]     = TUNE(sTuneSlotsPull),
    [CASINO_TUNE_LOSE]     = TUNE(sTuneSlotsLose),
    [CASINO_TUNE_WIN]      = TUNE(sTuneSlotsWin),
    [CASINO_TUNE_BIG]      = TUNE(sTuneSlotsBig),
    [CASINO_TUNE_JACKPOT]  = TUNE(sTuneSlotsJackpot),
    [CASINO_TUNE_SPOOKY]   = TUNE(sTuneSlotsSpooky),
    [CASINO_TUNE_JINGLE]   = TUNE(sTuneSlotsJingle),
    [CASINO_TUNE_FIREWORK] = TUNE(sTuneSlotsFirework),
    [CASINO_TUNE_SWEET]    = TUNE(sTuneSlotsSweet),
    [CASINO_TUNE_CHEER]    = TUNE(sTuneSlotsCheer),
};

#define tTune  data[0]
#define tAt    data[1]   // the next entry
#define tFrame data[2]
#define tBase  data[3]

// The companion plays a tune's notes over the day's note in the octave above middle C (tunes.h dayRoot: MIDI 72 + the
// note). PLAY's HARP has its middle C at MIDI 60 and its FLUTE at 72 (tools/genplaysounds.py), so the same pitches are
// the HARP's semitone 12 + the note and the FLUTE's the note itself: the game and the devices sound the same notes.
// The last entry of a tune is a rest at its length: the FLUTE, which would hold, is stopped there; the HARP rings on.
static void Task_CasinoTune(u8 taskId)
{
    s16 *data = gTasks[taskId].data;
    const struct CasinoTune *t = &sTunes[tTune];

    while (tAt < t->n && t->notes[tAt].frame <= tFrame)
    {
        const struct CasinoNote *note = &t->notes[tAt++];
        if (note->semis == 127)
        {
            if (tAt < t->n || PLAY_INSTRUMENT == PLAY_FLUTE)
                m4aMPlayStop(&gMPlayInfo_SE2);
        }
        else
        {
            Play_SoundNote(PLAY_INSTRUMENT, tBase + note->semis, FALSE);
            m4aMPlayVolumeControl(&gMPlayInfo_SE2, TRACKS_ALL, 256);   // the FLUTE starts soft for PLAY's breath
        }
    }
    tFrame++;
    if (tAt >= t->n)
        DestroyTask(taskId);
}

void DaemonsCasino_PlayTune(u8 tune, u8 dayNote)
{
    u8 taskId = FindTaskIdByFunc(Task_CasinoTune);
    s16 *data;

    if (taskId == TASK_NONE)
        taskId = CreateTask(Task_CasinoTune, 90);
    data = gTasks[taskId].data;
    tTune = tune % CASINO_TUNE_COUNT;
    tAt = 0;
    tFrame = 0;
    tBase = (PLAY_INSTRUMENT == PLAY_HARP ? 12 : 0) + dayNote;
    Task_CasinoTune(taskId);   // its first note now
}

bool8 DaemonsCasino_TunePlaying(void)
{
    return FuncIsActiveTask(Task_CasinoTune);
}

void DaemonsCasino_StopTune(void)
{
    u8 taskId = FindTaskIdByFunc(Task_CasinoTune);

    if (taskId != TASK_NONE)
        DestroyTask(taskId);
    m4aMPlayStop(&gMPlayInfo_SE2);
}

// casino.cpp soundSlots: a win small or large, the jackpot -- and on a big one a holiday's own phrase
u8 DaemonsCasino_WinTune(u8 outcome, u8 look)
{
    if (outcome == CASINO_JACK3 || sCasinoPays[outcome] >= 12)
    {
        switch (look)
        {
        case CASINO_LOOK_PUMPKINS:  return CASINO_TUNE_SPOOKY;
        case CASINO_LOOK_PRESENTS:  return CASINO_TUNE_JINGLE;
        case CASINO_LOOK_FIREWORKS: return CASINO_TUNE_FIREWORK;
        case CASINO_LOOK_HEARTS:    return CASINO_TUNE_SWEET;
        }
        return outcome == CASINO_JACK3 ? CASINO_TUNE_JACKPOT : CASINO_TUNE_BIG;
    }
    return CASINO_TUNE_WIN;
}

// C-151 / T-422 (the user, 2026-10-19): a win as big as slots.ts's tableAct calls to the table -- the JACKPOT, or a pay
// of CASINO_CHEER_PAYS and up -- is cheered by the crowd at the GAME CORNER's other machines (TUNE_SLOTS_CHEER)
bool8 DaemonsCasino_TableCheers(u8 outcome)
{
    return outcome == CASINO_JACK3 || sCasinoPays[outcome] >= CASINO_CHEER_PAYS;
}

// ------------------------------------------------------------------------------------------------------- the pieces

// The coin and the season's pieces, half the companion's size and outlined (gencasino.py), and what falls.
static const u8 sCasinoPieceTiles[] = INCBIN_U8("graphics/slot_machine/daemons/pieces.4bpp");
static const u8 sCasinoBitTiles[]   = INCBIN_U8("graphics/slot_machine/daemons/bits.4bpp");

#define FRAME_BYTES (32 * 32 / 2)

static u16 Rgb15(u32 rgb)
{
    return RGB((rgb >> 19) & 31, (rgb >> 11) & 31, (rgb >> 3) & 31);
}

static void FillDayColours(u16 *pal, const struct DaemonsCasinoDay *day)
{
    pal[1] = Rgb15(day->colourA);
    pal[2] = Rgb15(day->colourB);
    pal[3] = DaemonsMonthColour(DaemonsMonth());
}

static u8 GetPix(const u8 *frame, u8 x, u8 y)
{
    u8 b = frame[((y / 8) * 4 + x / 8) * 32 + (y % 8) * 4 + (x % 8) / 2];
    return (x & 1) ? b >> 4 : b & 15;
}

static void SetPix(u8 *frame, u8 x, u8 y, u8 v)
{
    u8 *b = &frame[((y / 8) * 4 + x / 8) * 32 + (y % 8) * 4 + (x % 8) / 2];
    *b = (x & 1) ? (*b & 0x0F) | (v << 4) : (*b & 0xF0) | v;
}

// the colour of an icon palette nearest a hat's (squared distance on the GBA's five bits a channel)
static u8 NearestIn(const u16 *pal, u16 c)
{
    u32 i, best = 1, bestD = 0xFFFFFFFF;

    for (i = 1; i < 16; i++)
    {
        s32 dr = (s32)(pal[i] & 31) - (c & 31);
        s32 dg = (s32)((pal[i] >> 5) & 31) - ((c >> 5) & 31);
        s32 db = (s32)((pal[i] >> 10) & 31) - ((c >> 10) & 31);
        u32 d = dr * dr + dg * dg + db * db;
        if (d < bestD)
            bestD = d, best = i;
    }
    return best;
}

// A daemon's icon (the party icon, which gbasprite.py derives from its own portrait), with the holiday's hat on its
// head (casino.cpp hat(): set on the top of the head, in the middle of it), its colours taken from the icon's palette.
static void PutHat(u8 *frame, u8 palIndex, const struct DaemonsCasinoDay *day)
{
    const struct CasinoHat *hat = &sCasinoHats[day->hat];
    const u16 *pal = gMonIconPalettes[palIndex];
    u8 map[4];
    s32 x, y, top = -1, lo = 32, hi = -1, base, empty, shift, hx, hy;

    for (y = 0; y < 32 && top < 0; y++)
        for (x = 0; x < 32; x++)
            if (GetPix(frame, x, y))
                top = y;
    if (top < 0)
        return;
    for (y = top; y < top + 3 && y < 32; y++)
        for (x = 0; x < 32; x++)
            if (GetPix(frame, x, y))
            {
                if (x < lo) lo = x;
                if (x > hi) hi = x;
            }
    // room for the hat: the icon moved down into the empty rows under it
    for (empty = 0, y = 31; y >= 0; y--, empty++)
    {
        for (x = 0; x < 32 && !GetPix(frame, x, y); x++)
            ;
        if (x < 32)
            break;
    }
    base = (top < 7 ? 7 : top) + 1;
    shift = hat->y - base;
    if (shift > empty)
        shift = empty;
    if (shift > 0)
    {
        for (y = 31; y >= shift; y--)
            for (x = 0; x < 32; x++)
                SetPix(frame, x, y, GetPix(frame, x, y - shift));
        for (y = 0; y < shift; y++)
            for (x = 0; x < 32; x++)
                SetPix(frame, x, y, 0);
        base += shift;
    }
    for (x = 1; x < 4; x++)
    {
        u32 c = hat->colours[x];
        map[x] = NearestIn(pal, c == 0xFF000001 ? Rgb15(day->colourA) : c == 0xFF000002 ? Rgb15(day->colourB) : Rgb15(c));
    }
    for (hy = 0; hy < CASINO_HAT_H; hy++)
    {
        for (hx = 0; hx < CASINO_HAT_W; hx++)
        {
            u8 v = hat->pix[hy][hx];
            x = (lo + hi) / 2 - hat->x + hx;
            y = base - hat->y + hy;
            if (v && x >= 0 && x < 32 && y >= 0 && y < 32)
                SetPix(frame, x, y, map[v]);
        }
    }
}

// FOOLS' DAY (casino.cpp drawPicture: "upside down on April Fools' Day"): a daemon's frame turned over top to bottom,
// so it stands on its head -- the daemons only; the coin and the season's piece stay as they are
static void FlipFrame(u8 *frame)
{
    u8 x, y, a;

    for (y = 0; y < 16; y++)
        for (x = 0; x < 32; x++)
        {
            a = GetPix(frame, x, y);
            SetPix(frame, x, y, GetPix(frame, x, 31 - y));
            SetPix(frame, x, 31 - y, a);
        }
}

// The six pieces' frames into `tiles` (6 x 512 bytes, slots.ts's order), and each one's palette: pal[k] 0-2 an icon
// palette (gMonIconPalettes), 3 the coin's and 4 the season's (artPals[0], artPals[1], their day's colours filled in).
void DaemonsCasino_BuildPieces(const struct DaemonsCasinoDay *day, const u16 species[4], const u32 personality[4],
                               u8 *tiles, u8 pal[CASINO_PIECE_COUNT], u16 artPals[2][16])
{
    u32 k;
    u8 art;

    for (k = 0; k < 4; k++)
    {
        u8 *frame = tiles + k * FRAME_BYTES;
        if (species[k] != SPECIES_NONE)
        {
            CpuCopy16(GetMonIconPtr(species[k], personality[k], FALSE), frame, FRAME_BYTES);
            pal[k] = GetValidMonIconPalIndex(species[k]);
            if (day->hat != CASINO_HAT_NONE)
                PutHat(frame, pal[k], day);
            if (day->flip)
                FlipFrame(frame);
        }
        else
        {
            CpuCopy16(sCasinoPieceTiles + CASINO_ART_MARK * FRAME_BYTES, frame, FRAME_BYTES);   // a MARK stands in
            pal[k] = 4;
        }
    }
    CpuCopy16(sCasinoPieceTiles + CASINO_ART_COIN * FRAME_BYTES, tiles + CASINO_COIN * FRAME_BYTES, FRAME_BYTES);
    pal[CASINO_COIN] = 3;
    CpuCopy16(sCasinoArtPals[CASINO_ART_COIN], artPals[0], 32);
    FillDayColours(artPals[0], day);
    art = sCasinoLookArt[day->look % CASINO_LOOK_COUNT];
    CpuCopy16(sCasinoPieceTiles + art * FRAME_BYTES, tiles + CASINO_SEASON * FRAME_BYTES, FRAME_BYTES);
    pal[CASINO_SEASON] = 4;
    CpuCopy16(sCasinoArtPals[art], artPals[1], 32);
    FillDayColours(artPals[1], day);
    if (species[0] == SPECIES_NONE)
    {
        CpuCopy16(sCasinoArtPals[CASINO_ART_MARK], artPals[1], 32);
        FillDayColours(artPals[1], day);
    }
}

// T-422's surround (the user, 2026-10-18): the machine's dancers are the lead -- the JACK piece is one frame, its icon's
// second frame the other, a holiday's hat on it too. With no daemon to lead, the MARK the JACK shows stands still.
void DaemonsCasino_BuildDancer(const struct DaemonsCasinoDay *day, u16 species, u32 personality, u8 *frame)
{
    if (species == SPECIES_NONE)
    {
        CpuCopy16(sCasinoPieceTiles + CASINO_ART_MARK * FRAME_BYTES, frame, FRAME_BYTES);
        return;
    }
    CpuCopy16(GetMonIconPtr(species, personality, FALSE) + FRAME_BYTES, frame, FRAME_BYTES);   // an icon is two frames, 512 bytes each
    if (day->hat != CASINO_HAT_NONE)
        PutHat(frame, GetValidMonIconPalIndex(species), day);
    if (day->flip)
        FlipFrame(frame);   // the dancer is the lead, and on FOOLS' DAY it dances on its head too
}

// ---------------------------------------------------------------------------------------- the season over the reels

const u8 *DaemonsCasino_BitTiles(u32 *size)
{
    *size = sizeof(sCasinoBitTiles);
    return sCasinoBitTiles;
}

void DaemonsCasino_BitsPalette(const struct DaemonsCasinoDay *day, u16 pal[16])
{
    CpuCopy16(sCasinoBitsPal, pal, 32);
    pal[1] = Rgb15(day->colourA);   // its 3 is white (snow), not the day's colour
    pal[2] = Rgb15(day->colourB);
}

// casino.cpp bitReset/drawSlots: a bit's tile for this look (its colour A or B, as the dice say), how it moves
// (0 falls, 1 rises, 2 twinkles where it is), and for a twinkle its four sizes
u8 DaemonsCasino_BitTile(u8 look, u8 *motion)
{
    bool8 b = Random() & 1;

    *motion = sCasinoLookRises[look % CASINO_LOOK_COUNT] ? 1 : 0;
    switch (sCasinoLookFall[look % CASINO_LOOK_COUNT])
    {
    case CASINO_FALL_SNOW:     return (Random() & 1) ? CASINO_BIT_DOT2_WHITE : CASINO_BIT_DOT1_WHITE;
    case CASINO_FALL_DOT:      return (Random() & 1) ? (b ? CASINO_BIT_DOT2_B : CASINO_BIT_DOT2_A) : (b ? CASINO_BIT_DOT1_B : CASINO_BIT_DOT1_A);
    case CASINO_FALL_SPARK:    return b ? CASINO_BIT_DOT1_B : CASINO_BIT_DOT1_A;
    case CASINO_FALL_PETAL:    return b ? CASINO_BIT_PETAL_B : CASINO_BIT_PETAL_A;
    case CASINO_FALL_LEAF:     return b ? CASINO_BIT_LEAF_B : CASINO_BIT_LEAF_A;
    case CASINO_FALL_HEART:    return b ? CASINO_BIT_HEART_B : CASINO_BIT_HEART_A;
    case CASINO_FALL_EGG:      return b ? CASINO_BIT_EGG_B : CASINO_BIT_EGG_A;
    case CASINO_FALL_PRESENT:  return b ? CASINO_BIT_PRESENT_B : CASINO_BIT_PRESENT_A;
    case CASINO_FALL_CLOVER:   return CASINO_BIT_CLOVER;
    case CASINO_FALL_PUMPKIN:  return CASINO_BIT_PUMPKIN;
    case CASINO_FALL_RAIN:     return CASINO_BIT_RAIN;
    case CASINO_FALL_FOG:      return CASINO_BIT_FOG;
    case CASINO_FALL_CONFETTI: return CASINO_BIT_CONFETTI0 + Random() % 5;
    }
    *motion = 2;
    return b ? CASINO_BIT_TWINKLE0_B : CASINO_BIT_TWINKLE0_A;   // + 0..3, its size
}

// the words a near miss, two JACKs and a bet back say (the phone's and the boards', C-158) -- DRAFT
const u8 gText_CasinoSoClose[]    = _("SO CLOSE");
const u8 gText_CasinoAlmost[]     = _("ALMOST THE JACKPOT");
const u8 gText_CasinoBetBack[]    = _("BET BACK");
// T-422: the other machines' crowd, cheering a big win -- every one of them an animal (DRAFT; 198px in the small font)
const u8 gText_CasinoCheer[]      = _("THE OTHER MACHINES CHEER: HOOT! YIP! MOO!");
