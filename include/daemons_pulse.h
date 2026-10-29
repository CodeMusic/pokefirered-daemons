#ifndef GUARD_DAEMONS_PULSE_H
#define GUARD_DAEMONS_PULSE_H

// T-483 / T-482: THE PULSE, one breath for the game, the app and the watches (src/daemons_pulse.c).

#define PULSE_BG_WEIGHT  77     // 256ths: a background takes about 30% of the pulse

u16 Pulse_Ms(void);                     // where in the eight-second breath it is, 0-7999
u8 Pulse_Phase(void);                   // (1 - cos(2 pi t / 8000)) / 2, as 0-255: 0 red, 128 purple, 255 blue
u16 Pulse_Colour(u8 phase);             // red -> purple -> blue, three stops, never a straight blend
u16 Pulse_Now(void);                    // Pulse_Colour(Pulse_Phase())
u16 Pulse_Emerald(u8 phase);            // an inscription's ink: the tablet's emerald, lit at the breath's height
u16 Pulse_Blend(u16 base, u16 target, u8 weight);   // weight in 256ths

// Once a frame from main.c: whether the world (or the battle's ground) is pulsing now, and in which colour.
void Pulse_Tick(void);
// After every copy of the faded palette to PLTT (palette.c): the pulsing rows written again, blended from the faded
// buffer, which is never touched -- so nothing compounds, and the weather and the fades stay underneath.
void Pulse_ApplyToPltt(void);

#endif // GUARD_DAEMONS_PULSE_H
