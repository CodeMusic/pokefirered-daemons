#ifndef GUARD_DAEMONS_AWAY_H
#define GUARD_DAEMONS_AWAY_H

// T-358 (vision 9.25): a daemon AWAY on the player's device is washed out wherever it is drawn -- party, PORT,
// summary -- "so it is never lost". The bits themselves are in pokemon.h (DaemonIsAway, DaemonBoxIsAway).

void DaemonsWashAway(u16 *palette, u16 count);
void DaemonsWashAwayLoaded(u16 paletteOffset, u16 count);
void DaemonsWashAwayIcon(struct Sprite *sprite, u16 species);
void DaemonsWashAwayIconIf(struct Sprite *sprite, bool8 away);
void DaemonsUnwashAwayIcon(struct Sprite *sprite, u16 species);

#endif // GUARD_DAEMONS_AWAY_H
