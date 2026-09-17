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

// The four moves of a daemon, wherever it lives. A battler's are its CURRENT moves, which
// PERSPECTIVE and CLONE change mid-battle; a party daemon's are its own.
void Streaks_MovesOfMon(struct Pokemon *mon, u16 *moves);
void Streaks_MovesOfBoxMon(struct BoxPokemon *boxMon, u16 *moves);
void Streaks_MovesOfBattler(u8 battlerId, u16 *moves);

#endif // GUARD_DAEMON_STREAKS_H
