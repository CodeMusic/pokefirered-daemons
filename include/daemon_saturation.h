#ifndef GUARD_DAEMON_SATURATION_H
#define GUARD_DAEMON_SATURATION_H

// T-424: SATURATION IS FRIENDSHIP (vision.md 8.5, "Saturation is friendship, and that is the thesis made mechanical";
// built 2026-10-18, DRAFT). A daemon's own picture is drawn at a saturation its friendship sets: at 0 nearly grey,
// a hint of its type's colour left in it; at 255 the full ramp. Only a daemon's own sprite -- never the world, never
// a person (invariant 5; 8.6's greyscale question stays OPEN). The INDEX has no friendship and shows every daemon at
// 0. Applied once, as a palette is loaded -- never per frame -- in the palette slot it already has.

#define SATURATION_INDEX 0   // the friendship the INDEX draws every daemon at

// How much of its colour a daemon keeps at this friendship, in 256ths (31 at 0 .. 256 at 255).
u16 Saturation_OfFriendship(u8 friendship);

// Into a decompressed 16-colour palette BEFORE it is loaded (index 0, the transparent one, untouched).
void Saturation_ApplyToBuffer(u16 *palette, u8 friendship);
// Into a palette already loaded at paletteOffset (both palette buffers); first/count pick its colours (1, 15: all).
void Saturation_ApplyToLoaded(u16 paletteOffset, u8 friendship);
void Saturation_ApplyToLoadedRange(u16 paletteOffset, u8 first, u8 count, u8 friendship);

// A battler's friendship: its own party daemon's, on whichever side.
u8 Saturation_OfBattler(u8 battlerId);

#endif // GUARD_DAEMON_SATURATION_H
