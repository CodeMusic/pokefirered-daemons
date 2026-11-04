#ifndef GUARD_DAEMONS_HOURS_H
#define GUARD_DAEMONS_HOURS_H

// T-519: THE TWO HOURS (src/daemons_hours.c) -- the catch-up ramp both hours' effects wear when play begins inside
// one, and THE CREATOR'S HOUR's light, motes, glow and tone. No save bytes: the moment play began is RAM only.

// When play begins (a new game, a save continued -- after power on or a soft reset). Never on passing into an hour.
void Hours_PlayBegan(void);
// DEBUG's CATCH-UP row: as if play began this frame.
void Hours_DebugRestart(void);
// 0-256: nothing for the first minute of play, a smoothstep over the next two, then 256 for good.
u16 Hours_CatchUp(void);
// 0-256: the witching hour's effects' share now -- the catch-up inside the hour, 256 outside it (whatever else asks).
u16 Hours_WitchingEase(void);

// 0-256: the creator's hour's strength now -- its shape on the clock (rising, holding, resolving), times the catch-up.
u16 Hours_CreatorStrength(void);
// Once a frame from main.c, after Pulse_Tick: the bloom, the motes and the tone.
void Hours_Tick(void);
// The creator's hour's gold over the map's palettes (the pulse lays it, src/daemons_pulse.c): TRUE and its colour and
// weight (256ths) while it shows.
bool8 Hours_CreatorTint(u16 *colour, u8 *weight);
// The follower's glow in the creator's hour: sixteenths of gold, 0 when there is none.
u8 Hours_FollowerGlow(void);
void Hours_GlowFadedInPlace(u16 palOffset, u8 weight);

#endif // GUARD_DAEMONS_HOURS_H
