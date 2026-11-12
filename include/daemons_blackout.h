#ifndef GUARD_DAEMONS_BLACKOUT_H
#define GUARD_DAEMONS_BLACKOUT_H

// T-546: THE BLACKOUT's dark (src/daemons_blackout.c). Black but the green threads until VERBOSE; then the threads
// bright and the purple breathing on the pulse's breath.

// Once a frame from main.c: VERBOSE's light rising.
void Blackout_Tick(void);
// Last in Pulse_ApplyToPltt, after every copy of the faded palette to PLTT: the dark, or the breath, written over
// BG rows 0..12 from what is already there -- the buffers are never touched.
void Blackout_ApplyToPltt(void);
// From AnimateFlash(0): the purple rises over this many frames, in step with VERBOSE's circle.
void Blackout_Illuminate(u16 frames);
// The field is on one of THE BLACKOUT's maps (its secondary tileset is gTileset_RockTunnel).
bool8 Blackout_Here(void);
// THE BLACKOUT is not yet fully lit: the creator's hour's bloom stays off (it would grey the circle's black).
bool8 Blackout_IsDark(void);
// The field's WINOUT: on THE BLACKOUT's maps the map is drawn outside WIN0 too (in the dark palette), never the sprites.
u16 Blackout_WinOut(u16 field);

#endif // GUARD_DAEMONS_BLACKOUT_H
