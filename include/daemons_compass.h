#ifndef GUARD_DAEMONS_COMPASS_H
#define GUARD_DAEMONS_COMPASS_H

// T-586: DAVID's COMPASS, its rose lettered in a mirror (src/daemons_compass.c; every word and the drawing DRAFT).

void Compass_Open(void);        // special (waitstate): the rose over the field, until A or B
void Compass_CurrentMap(void);  // special: the map stood on, as a MAP_* constant, in VAR_RESULT

#endif // GUARD_DAEMONS_COMPASS_H
