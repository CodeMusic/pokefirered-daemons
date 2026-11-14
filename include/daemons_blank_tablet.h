#ifndef GUARD_DAEMONS_BLANK_TABLET_H
#define GUARD_DAEMONS_BLANK_TABLET_H

// T-547: THE BLANK TABLET, the stone by BLANCHE's path that the player writes without knowing, and the old toad by the
// pond who says the player's lean (src/daemons_blank_tablet.c; vision 4.39 and 4.40; every word DRAFT).

void BlankTablet_Begin(void);       // special: the face read is the side the player stands on
u16 BlankTablet_Next(void);         // specialvar: TRUE with the next page in gStringVar4, FALSE when the stone is read
void OldToad_Line(void);            // special: her one line, in gStringVar4

#endif // GUARD_DAEMONS_BLANK_TABLET_H
