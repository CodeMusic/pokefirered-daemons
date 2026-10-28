#ifndef GUARD_DAEMON_STREAKS_H
#define GUARD_DAEMON_STREAKS_H

// The streaks (vision.md 9.4, amended 2026-09-17; T-132). A daemon's palette indices 11..14 are
// its four streaks, one per move slot, each the type of the move in that slot. The species palette
// ships them grey; these write them from a real daemon's moves whenever one is shown. A species the
// build did not give streaks is never touched -- a vanilla palette uses all sixteen colours.

#define STREAK_FIRST_INDEX 11

// Write the streaks into a decompressed 16-colour palette BEFORE it is loaded.
void Streaks_ApplyToBuffer(u16 *palette, u16 species, const u16 *moves);
// Write the streaks into a palette already loaded at paletteOffset (both palette buffers).
void Streaks_ApplyToLoaded(u16 paletteOffset, u16 species, const u16 *moves);
// The same, on a body that is not the species' first type: CASTFORM's form palettes, each drawn in the type Forecast
// gives that form (tools/gencastform.py). The streak colour depends on the body it sits on.
void Streaks_ApplyToLoadedOnBody(u16 paletteOffset, u16 species, u8 body, const u16 *moves);
// The battle's CASTFORM palette: castformPalette[form] loaded whole, then its streaks on that form's body. The species
// palette the buffer path streaked is loaded first and then overwritten, so the streaks must be written again here.
void Streaks_ApplyToCastform(u16 paletteOffset, u8 form, const u16 *moves);

// The four moves of a daemon, wherever it lives. A battler's are its CURRENT moves, which
// PERSPECTIVE and CLONE change mid-battle; a party daemon's are its own.
void Streaks_MovesOfMon(struct Pokemon *mon, u16 *moves);
void Streaks_MovesOfBoxMon(struct BoxPokemon *boxMon, u16 *moves);
void Streaks_MovesOfBattler(u8 battlerId, u16 *moves);

// A type's own streak colour on no body in particular (its light step): what CAST and RECAST blend the
// user toward once its type has changed, so the change is shown in the colour the streaks already use.
u16 Streaks_TypeColour(u8 type);

#endif // GUARD_DAEMON_STREAKS_H
