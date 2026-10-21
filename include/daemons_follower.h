#ifndef GUARD_DAEMONS_FOLLOWER_H
#define GUARD_DAEMONS_FOLLOWER_H

// T-18 P1 (planned and decided 2026-10-20, docs/private/plan-follower-2026-10-20.md): THE FOLLOWER. The lead daemon
// walks one tile behind the player, drawn as its own party icon in its own type colours, greyed by its friendship.
// It is a SPRITE, not an object event: it takes no object slot, blocks nothing, is never saved and never recorded.

void Follower_Update(void);          // once a frame in the field, after the sprites have moved (overworld.c)
void Follower_RetintPalette(void);   // the light changed (DaemonsRetintObjectEventPalettes)
s8 Follower_LeadSlot(void);          // the party slot that follows, or -1: not an egg, not HALTED, not AWAY

// T-18 P2: A on it. The script when the tile the player faces is where it stands (field_control_avatar.c, after the
// people), and the special that script runs: its lines into gStringVar4, a bubble over it.
const u8 *Follower_GetInteractionScript(struct MapPosition *position);
void Follower_Talk(void);

// T-18 P3: hidefollower / showfollower (specials, for a scene that reads wrong with it there), and Continue.
void Follower_SceneHide(void);
void Follower_SceneShow(void);
void Follower_OnContinue(void);

// T-430: the OBJ palette slot it is drawn in, or 0xFF; HALFTONE leaves that slot in colour.
u8 Follower_PaletteSlot(void);

#endif // GUARD_DAEMONS_FOLLOWER_H
