#ifndef GUARD_DAEMONS_PULSE_H
#define GUARD_DAEMONS_PULSE_H

// T-483 / T-482: THE PULSE, one breath for the game, the app and the watches (src/daemons_pulse.c).

#define PULSE_BG_WEIGHT  77     // 256ths: a background takes about 30% of the pulse

u16 Pulse_Ms(void);                     // where in the eight-second breath it is, 0-7999
u8 Pulse_Phase(void);
u8 Pulse_GoldBreath(void);              // T-519: (1 - cos(2 pi t / 16000)) / 2, as 0-255, on the same clock                   // (1 - cos(2 pi t / 8000)) / 2, as 0-255: 0 red, 128 purple, 255 blue
u16 Pulse_Colour(u8 phase);             // red -> purple -> blue, three stops, never a straight blend
u16 Pulse_Now(void);                    // Pulse_Colour(Pulse_Phase())
u16 Pulse_Emerald(u8 phase);            // an inscription's ink: the tablet's emerald, lit at the breath's height
u16 Pulse_Blend(u16 base, u16 target, u8 weight);   // weight in 256ths

// Once a frame from main.c: whether the world (or the battle's ground) is pulsing now, and in which colour.
void Pulse_Tick(void);
// After every copy of the faded palette to PLTT (palette.c): the pulsing rows written again, blended from the faded
// buffer, which is never touched -- so nothing compounds, and the weather and the fades stay underneath.
void Pulse_ApplyToPltt(void);
// T-546: the same into any palette over entries lo..hi; after THE FIELD STAGE, only from `first` on; and whether the
// pulse touches any entry below hi this frame
void Pulse_LayTo(u16 *pltt, u32 lo, u32 hi);
void Pulse_ApplyToPlttFrom(u32 first);
bool8 Pulse_Touches(u32 hi);

#endif // GUARD_DAEMONS_PULSE_H
