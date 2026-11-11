#ifndef GUARD_DAEMONS_AURORA_H
#define GUARD_DAEMONS_AURORA_H

// T-528 part 1: THE AURORA, the dawn that comes at night (vision.md 4.40; src/daemons_aurora.c). Once THE UNITY BOOK
// is mended: over DAEMON TOWER every night, in the lit groves' battle skies on the light's nights, and outdoors on
// those nights once every grove found is lit. CONTENT in oxygen's red line, CONTEXT in its green.

#define AURORA_GROVE_COUNT 14

// The light's nights: about one in three, from the date by the rule the companion copies (DAEMONS docs/TODO.md T-528).
bool8 Aurora_IsLightNight(void);
// The night the date rule is asked about: days since 2000-01-01 (play hours without a clock), the evening's date.
u16 Aurora_NightNumber(void);
// On every arrival on a map (overworld.c): a grove entered at night after the mending takes the light, and its pair.
void Aurora_OnArrive(void);
// Once a frame from main.c, after Pulse_Tick: whether the light shows this frame, where, and how strongly.
void Aurora_Tick(void);
// After every copy of the faded palette to PLTT, after the pulse (daemons_pulse.c): the light laid on PLTT, read from
// what is already there, so the buffers are never touched and nothing compounds.
void Aurora_ApplyToPltt(void);
// T-528 (f) THE CONJUGATE LIGHT: the book mended and a daemon in the party whose origin game (its met game, the
// version byte every mon carries) is the other edition -- by trade, or the companion's trade. Both lines show at once,
// green below and red crowning it; PLAY's SONGBOOK duet (play.c, the Musical Instruments lane) waits on it.
bool8 Aurora_ConjugateLight(void);
// The half of it that is the party's: a daemon whose met game is the other edition (eggs never count).
bool8 Aurora_PartyFromOtherEdition(void);
// T-528 (d) THE DAWN CHORUS: a lit grove, THE CREATOR'S HOUR, the morning after a night of light (daemons_hours.c).
bool8 Aurora_DawnChorus(void);
// T-528 (e) TULIKKO (specialvar): 0 before the mending, 1 after, 2 a night of light, 3 the conjugate light at night.
u16 Aurora_FoxSays(void);
// T-528 (e) THE SPARKS: once a frame from main.c, after Aurora_Tick -- on a night of light with the book mended, the
// snow TULIKKO flicks at FOUR ISLAND keeps rising as a few sparks (both lines under the conjugate light).
void Aurora_SparksTick(void);

#if DAEMONS_DEBUG
extern u8 gAuroraDebugNights;      // DEBUG -> GAME EVENTS: 0 the date's rule, 1 every night a light one, 2 none
void Aurora_DebugLightAll(bool8 on);
bool8 Aurora_DebugAllLit(void);
void Aurora_DebugOtherEdition(bool8 on);
#endif

#endif // GUARD_DAEMONS_AURORA_H
