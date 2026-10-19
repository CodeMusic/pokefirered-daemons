#ifndef GUARD_DAEMONS_SIGNATURE_H
#define GUARD_DAEMONS_SIGNATURE_H

// T-392 P1 (the user, 2026-10-18): every routine signed -- its own codeword, shown as code (nine 0s and 1s flying
// the family's path in the type's ink) and played as music (a three-note figure on the edition's instrument, its
// rhythm the word's 1s). One generated table (src/data/daemons_signatures.h, tools/gensignature.py) and one hook
// in the battle-animation launcher; the routine's own picture plays underneath, unchanged.

struct DaemonsSignature
{
    u16 word;     // the nine glyphs; the first is bit 8
    u16 rhythm;   // the nine steps that pluck a note (the word, with its last 0s filled if it has under three 1s)
    u16 figure;   // three degrees 0-7 (a major scale and its octave): bits 6-8, 3-5, 0-2
    s8 tonic;     // semitones from the instrument's middle C: the type's key, an achromatic type's register folded in
    u8 path;      // DAEMONS_GLYPH_* (include/constants/battle_anim.h)
    u8 mods;      // SIG_MOD_*
    u8 wave;      // pixels off the path
    u8 tempo;     // frames a glyph flies; the figure steps every tempo / 4
    u8 flags;     // SIG_*
};

#define SIG_APPROVED    (1 << 0)   // plays in the release ROMs (the debug ROMs play every row)
#define SIG_OWN_SCRIPT  (1 << 1)   // a union routine: its own script carries the signature (every ROM, 2026-10-18)
#define SIG_UNION       (1 << 2)   // the splash's own colours, and the harp and flute both
#define SIG_NOTES_LEAD  (1 << 3)   // the notes first, then the bits (the CONTEXT side); otherwise bits first

#define SIG_MOD_ARC     (1 << 0)   // VECTOR: the stream bows upward
#define SIG_MOD_FADE    (1 << 1)   // OPAQUE: gone halfway
#define SIG_MOD_BACK    (1 << 2)   // COPY: there and back again
#define SIG_MOD_FLIP    (1 << 3)   // CORRUPT: the bits flip as they fly
#define SIG_MOD_SLOW    (1 << 4)   // GROWTH, LEGACY
#define SIG_MOD_FAST    (1 << 5)   // SIGNAL
#define SIG_MOD_SELF    (1 << 6)   // RAISE, HEAL, PROTECT: on the user, not the target
#define SIG_MOD_ORBIT   (1 << 7)   // CONTEXT: round the target at one radius

// battle_anim.c: does this move's signature play here (and, if so, which glyph palettes it needs loaded)?
bool8 DaemonsSignature_Wants(u16 move);
// The tags it needs loaded: the glyph sheet and its ink (or the splash's colours), or on a holiday look the lanes.
void DaemonsSignature_Tags(u16 move, u16 *tiles, u16 *palette);
// battle_anim.c, after the tags are loaded and registered: start it.
void DaemonsSignature_Start(u16 move);
// battle_anim.c's `end`: TRUE while it still plays -- in full (the user, 2026-10-18: no cap).
bool8 DaemonsSignature_Holds(void);

#endif // GUARD_DAEMONS_SIGNATURE_H
