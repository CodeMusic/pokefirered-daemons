// This determines how much higher above the usual position the enemy Pokémon
// is during battle. Species that float or fly have nonzero values.
// DAEMONS: written by tools/gbaelevation.py from tools/daemon_float.json -- only a daemon DRAWN flying, hovering or
// levitating is lifted (and gets the shadow); everything standing, sitting or perched is 0. Edit the json, not this.
const u8 gEnemyMonElevation[NUM_SPECIES] =
{
    [SPECIES_BUTTERFREE] = 8,
    [SPECIES_ZUBAT] = 8,
    [SPECIES_GASTLY] = 4,
    [SPECIES_HAUNTER] = 4,
    [SPECIES_CROBAT] = 6,
    [SPECIES_YANMA] = 8,
    [SPECIES_MISDREAVUS] = 8,
    [SPECIES_CELEBI] = 15,
    [SPECIES_DUSTOX] = 10,
    [SPECIES_NINJASK] = 10,
    [SPECIES_SHEDINJA] = 8,
    [SPECIES_MASQUERAIN] = 10,
    [SPECIES_FLYGON] = 7,
    [SPECIES_DUSKULL] = 9,
    [SPECIES_SHUPPET] = 12,
    [SPECIES_CASTFORM] = 16,
    [SPECIES_BELDUM] = 8,
    [SPECIES_RAYQUAZA] = 6,
    [SPECIES_DEOXYS] = 8,
};
