#ifndef GUARD_DAEMONS_CASINO_H
#define GUARD_DAEMONS_CASINO_H

// T-422 (the user, 2026-10-18): the GAME CORNER plays as the companion's CASINO -- see src/daemons_casino.c. The table,
// the luck and the tunes are the companion's, carried in by DAEMONS tools/gencasino.py (src/data/daemons_casino.h).

// slots.ts's results and pieces, in its order (DAEMONS tools/gencasino.py checks the order has not moved)
enum { CASINO_LOSE, CASINO_SEASON1, CASINO_JACK2, CASINO_SEASON3, CASINO_COIN3, CASINO_FRIEND3, CASINO_JACK3, CASINO_OUTCOME_COUNT };
enum { CASINO_JACK, CASINO_F1, CASINO_F2, CASINO_F3, CASINO_COIN, CASINO_SEASON, CASINO_PIECE_COUNT };
#define CASINO_STRIP_LENGTH 12   // each piece twice round a reel (slots.ts STRIP; src/data/daemons_casino.h asserts it)

// The day at the reels, read from the GAME's world as the machine opens: its holiday, the sky over the city, the
// watch and the season by edition -- never the real world's (the companion's devices follow that one).
struct DaemonsCasinoDay
{
    u16 ret;          // the day's return, in ten-thousandths (9000..10000)
    s16 luck;         // in hundredths, -100..100
    u8 holiday;       // HOLIDAY_*, or HOLIDAY_NONE
    u8 sky, watch, season;
    u8 look;          // CASINO_LOOK_*: the season's piece, and what falls over the reels
    u8 hat;           // CASINO_HAT_*
    bool8 flip;       // the daemons stand on their heads (FOOLS' DAY; casino.cpp drawPicture)
    u8 dayNote;       // 0-11: the day's note, C (Sunday) .. B (Saturday)
    u8 whyFrom;       // CASINO_WHY_*: which part of the day `why` is, so the help card can name it as the companion does
    u32 colourA, colourB;   // 0xRRGGBB
    const u8 *why;    // the biggest reason, in words (DRAFT)
};

enum { CASINO_WHY_NONE, CASINO_WHY_HOLIDAY, CASINO_WHY_SKY, CASINO_WHY_WATCH, CASINO_WHY_SEASON };

// The day's biggest reason in full, as the companion's context.ts words it: "CLOVER DAY: the luck of the day", "rain on
// the windows in CELADON CITY", "spring: things growing (CONTENT's year)" (T-422's fix, 2026-10-18). DRAFT.
void DaemonsCasino_WhyText(const struct DaemonsCasinoDay *day, u8 *dest);

// The string the cat plays with (slots.ts Cat), the player's own: kept while the player sits at the machine.
struct DaemonsCasinoCat
{
    s16 reach;        // ten-thousandths, -10000..10000
    u16 dry, streak, pulls;
};

void DaemonsCasino_ReadDay(struct DaemonsCasinoDay *day);
u16 DaemonsCasino_PullReturn(const struct DaemonsCasinoDay *day, const struct DaemonsCasinoCat *cat);
u8 DaemonsCasino_Draw(u16 ret);                           // a result (CASINO_*), from the table for this return
void DaemonsCasino_Learn(struct DaemonsCasinoCat *cat, u8 outcome);
u16 DaemonsCasino_NearChance(const struct DaemonsCasinoCat *cat);   // ten-thousandths
u8 DaemonsCasino_OutcomeOf(u8 a, u8 b, u8 c);
void DaemonsCasino_PiecesFor(u8 outcome, u8 out[3]);
bool8 DaemonsCasino_NearPieces(const u8 *reel3, u8 out[3], u8 *wanted);
bool8 DaemonsCasino_AlmostThird(const u8 *reel3, u8 *third);
u8 DaemonsCasino_Pays(u8 outcome);

// The reels' daemons: [0] the lead (the jackpot), [1..3] from the INDEX's seen, at home in this season or watch first.
void DaemonsCasino_ReelSpecies(const struct DaemonsCasinoDay *day, u16 species[4], u32 personality[4]);

// The tunes, on the day's note (PLAY's HARP in CONTENT, FLUTE in CONTEXT), as the companion's devices play them.
enum {
    CASINO_TUNE_PULL,
    CASINO_TUNE_LOSE,
    CASINO_TUNE_WIN,
    CASINO_TUNE_BIG,
    CASINO_TUNE_JACKPOT,
    CASINO_TUNE_SPOOKY,
    CASINO_TUNE_JINGLE,
    CASINO_TUNE_FIREWORK,
    CASINO_TUNE_SWEET,
    CASINO_TUNE_CHEER,
    CASINO_TUNE_COUNT
};
void DaemonsCasino_PlayTune(u8 tune, u8 dayNote);
bool8 DaemonsCasino_TunePlaying(void);
void DaemonsCasino_StopTune(void);
bool8 DaemonsCasino_TableCheers(u8 outcome);             // a big win (tableAct): the other machines cheer it
u8 DaemonsCasino_WinTune(u8 outcome, u8 look);           // the tune a win plays, a holiday's own on a big one

// The pieces and the season over the reels (src/slot_machine.c draws them).
void DaemonsCasino_BuildPieces(const struct DaemonsCasinoDay *day, const u16 species[4], const u32 personality[4],
                               u8 *tiles, u8 pal[6], u16 artPals[2][16]);
// The machine's dancers (vanilla's Clefairy, T-422's surround): the lead's icon's second frame, its hat on, into
// `frame` (512 bytes) -- the first is the JACK piece itself, so the pair dance the icon's own two steps.
void DaemonsCasino_BuildDancer(const struct DaemonsCasinoDay *day, u16 species, u32 personality, u8 *frame);
const u8 *DaemonsCasino_BitTiles(u32 *size);
void DaemonsCasino_BitsPalette(const struct DaemonsCasinoDay *day, u16 pal[16]);
u8 DaemonsCasino_BitTile(u8 look, u8 *motion);          // motion: 0 falls, 1 rises, 2 twinkles (its tile + 0..3)
extern const u8 gText_CasinoSoClose[];
extern const u8 gText_CasinoAlmost[];
extern const u8 gText_CasinoBetBack[];
extern const u8 gText_CasinoCheer[];

#endif // GUARD_DAEMONS_CASINO_H
