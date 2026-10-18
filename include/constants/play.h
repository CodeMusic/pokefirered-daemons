#ifndef GUARD_CONSTANTS_PLAY_H
#define GUARD_CONSTANTS_PLAY_H

// T-417: the songs PLAY listens for (src/play.c's sSongs, written by DAEMONS tools/genplaysongs.py from
// docs/play_songs.json, in this order). Scripts name them: setvar VAR_0x8004, PLAY_SONG_NAMING.
#define PLAY_SONG_FIR       0   // O CHRISTMAS TREE
#define PLAY_SONG_NAMING    1   // SLATE / NAMING      TWELVE DAYS OF DECEMBER
#define PLAY_SONG_STILL     2   // SLOPE / STILL       THE REALIZATION
#define PLAY_SONG_MEND      3   // SENSE / MEND        FORGIVENESS OFFERED
#define PLAY_SONG_PRACTICE  4   // FIT / PRACTICE      SEQUEL THE GUARDIAN
#define PLAY_SONG_STANCE    5   // SKEW / STANCE       HARMONY IN BALANCE
#define PLAY_SONG_NOTICE    6   // FRAME / NOTICE      THE SHIMMERING CANVAS
#define PLAY_SONG_REFLECT   7   // HEAT / REFLECT      REFLECTIONS WITHIN
#define PLAY_SONG_COUNT     8
#define PLAY_SONG_1         PLAY_SONG_NAMING   // the first of the seven

#endif // GUARD_CONSTANTS_PLAY_H
