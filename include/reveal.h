#ifndef GUARD_REVEAL_H
#define GUARD_REVEAL_H

// T-220: REVEAL, the ninth DRIVER, installed in the TOOLKIT (docs/school.md 8a).
void Reveal_Update(void);      // every overworld frame
void Reveal_OnMapLoad(void);   // on arrival: shimmer soon, then keep on
// T-459: the nearest unfound thing within a screen -- what the follower notices once THE SUBTLE LAYER is held.
bool8 Reveal_NearestUnfound(s16 *x, s16 *y);

#endif // GUARD_REVEAL_H
