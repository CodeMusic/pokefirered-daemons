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

// T-597: COLOUR BY SONG (built 2026-11-19, DRAFT). The key is no longer friendship: each palette slot keeps as much
// of its colour as the notes of the daemon's own part that bring it in are learned (src/daemon_song.c,
// DaemonSong_ColourKeeps; the table is DAEMONS tools/gencolourbysong.py's). Friendship still scales over it only for
// the Corpus's parties and AL's (Saturation_IsStoryParty). The INDEX keeps SATURATION_INDEX above: every daemon grey.
#define SATURATION_BODY_SLOT 3   // the body ramp's mid tone: the type's own colour (tools/gbasprite.py)

// THE (species, share) ENTRY: a decompressed palette of SPECIES at SHARE of its own part (0..255).
void Saturation_SongToBuffer(u16 *palette, u16 species, u8 share);
// A daemon's own: its song share from its experience, and its friendship over that if it is a story party's. SHOWN is
// the species whose palette it is (its own; a later form of it in the evolution scene; another under PERSPECTIVE).
void Saturation_MonToBuffer(u16 *palette, struct Pokemon *mon, u16 shown);
void Saturation_MonToLoaded(u16 paletteOffset, struct Pokemon *mon, u16 shown);
void Saturation_MonToLoadedRange(u16 paletteOffset, u8 first, u8 count, struct Pokemon *mon, u16 shown);
// One keep for a palette whose slots are not its picture's (the follower's remapped colours, its emoji): its body's.
u16 Saturation_MonKeep(struct Pokemon *mon);
void Saturation_KeepToBuffer(u16 *palette, u16 keep);
// A battler's own party daemon, on whichever side.
struct Pokemon *Saturation_MonOfBattler(u8 battlerId);
// An enemy daemon of the Corpus or of AL, in the trainer battle being fought: friendship still greys it (T-424, T-559).
bool8 Saturation_IsStoryParty(struct Pokemon *mon);

#endif // GUARD_DAEMON_SATURATION_H
