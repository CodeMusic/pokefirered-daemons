#ifndef GUARD_DAEMONS_SEASONS_H
#define GUARD_DAEMONS_SEASONS_H

// T-532 (vision 9.34): the MUSAI branch keeps the seasons, and CODEMUSAI's tongue.

// The season a picture is drawn in: DaemonsSeason(), except inside a battle, which keeps the season it began in.
u8 DaemonsDisplaySeason(void);
// A MUSAI-branch species -> its picture-only id for the season (summer is its own); every other species, itself.
// Called by the pic loaders (decompress.c), the two sprite-palette look-ups (pokemon.c) and GetIconSpecies.
// And by every gMonFrontPicCoords / gMonBackPicCoords look-up (battle, trade, hatching, the jump game): each form
// has its own rows (tools/genmusaiseasons.py), since a vehicle makes it taller and its feet sit lower in the frame.
u16 DaemonsSeasonalSpecies(u16 species);
bool8 DaemonsIsMusaiBranch(u16 species);
// The INDEX entry for the season's form of SPECIES, or NULL for a species with one entry.
const u8 *DaemonsSeasonalIndexText(u16 species);
// CRYPTOPHASIA's power for BATTLER: 40, +20 for each OTHER MUSAI-branch daemon in its party able to fight (to 140).
u16 Cryptophasia_Power(u8 battler);

#endif // GUARD_DAEMONS_SEASONS_H
