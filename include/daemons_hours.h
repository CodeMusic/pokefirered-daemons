#ifndef GUARD_DAEMONS_HOURS_H
#define GUARD_DAEMONS_HOURS_H

// T-519: THE TWO HOURS (src/daemons_hours.c) -- the catch-up ramp the hours' effects wear when play begins inside
// one. No save bytes: the moment play began is RAM only.

// When play begins (a new game, a save continued -- after power on or a soft reset). Never on passing into an hour.
void Hours_PlayBegan(void);
// DEBUG's CATCH-UP row: as if play began this frame.
void Hours_DebugRestart(void);
// 0-256: nothing for the first minute of play, a smoothstep over the next two, then 256 for good.
u16 Hours_CatchUp(void);
// 0-256: the witching hour's effects' share now -- the catch-up inside the hour, 256 outside it (whatever else asks).
u16 Hours_WitchingEase(void);

#endif // GUARD_DAEMONS_HOURS_H
