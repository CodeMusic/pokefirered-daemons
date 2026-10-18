// T-417 PLAY's songs -- written by DAEMONS tools/genplaysongs.py from docs/play_songs.json. Do not edit:
// edit the json and run the tool. Every page is DRAFT; the titles are src/notebook.c's.

// O CHRISTMAS TREE -- the fir, in Bb major
static const s8 sSong_Fir_Notes[] = {5, 10, 10, 10, 12, 14, 14, 14};
static const u8 sSong_Fir_Pcs[] = {5, 10, 10, 10, 0, 2, 2, 2};
static const s8 sSong_Fir_Steps[] = {3, 6, 6, 6, 7, 8, 8, 8};
static const u8 sSong_Fir_Frames[] = {30, 22, 8, 40, 18, 22, 8, 40};

// TWELVE DAYS OF DECEMBER -- SLATE / NAMING, in E major (motif SLATE-3, chosen by the user by ear)
static const s8 sSong_Naming_Notes[] = {13, 11, 8, 9, 6, 8, 6, 4, 3, 4};
static const u8 sSong_Naming_Pcs[] = {1, 11, 8, 9, 6, 8, 6, 4, 3, 4};
static const s8 sSong_Naming_Steps[] = {7, 6, 4, 5, 3, 4, 3, 2, 1, 2};
static const u8 sSong_Naming_Frames[] = {15, 15, 15, 15, 15, 15, 15, 15, 15, 40};
static const u8 sSong_Naming_Page[] = _("Heard at CALLOW SCHOOL.");

// THE REALIZATION -- SLOPE / STILL, in Eb major (motif SLOPE-1, chosen by the user by ear)
static const s8 sSong_Still_Notes[] = {0, -2, 0, 2, 3, 2, 0, 5, 3, 5};
static const u8 sSong_Still_Pcs[] = {0, 10, 0, 2, 3, 2, 0, 5, 3, 5};
static const s8 sSong_Still_Steps[] = {7, 6, 7, 8, 9, 8, 7, 10, 9, 10};
static const u8 sSong_Still_Frames[] = {22, 22, 22, 22, 22, 22, 22, 22, 22, 48};
static const u8 sSong_Still_Page[] = _("Heard in the TANOBY RUINS.");

// FORGIVENESS OFFERED -- SENSE / MEND, in D major (motif SENSE-1, chosen by the user by ear)
static const s8 sSong_Mend_Notes[] = {4, 2, -1, 4, 6, 4, 1, 4, 2, -1, -3};
static const u8 sSong_Mend_Pcs[] = {4, 2, 11, 4, 6, 4, 1, 4, 2, 11, 9};
static const s8 sSong_Mend_Steps[] = {9, 8, 6, 9, 10, 9, 7, 9, 8, 6, 5};
static const u8 sSong_Mend_Frames[] = {20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 48};
static const u8 sSong_Mend_Page[] = _("Hummed by CRYSTAL.");

// SEQUEL THE GUARDIAN -- FIT / PRACTICE, in G major (motif FIT-1, chosen by the user by ear)
static const s8 sSong_Practice_Notes[] = {-1, -3, 2, 0, 2, 4, -5, -1, -3};
static const u8 sSong_Practice_Pcs[] = {11, 9, 2, 0, 2, 4, 7, 11, 9};
static const s8 sSong_Practice_Steps[] = {6, 5, 8, 7, 8, 9, 4, 6, 5};
static const u8 sSong_Practice_Frames[] = {17, 17, 17, 17, 17, 17, 17, 17, 44};
static const u8 sSong_Practice_Page[] = _("Heard where the GUIDE was kept.");

// HARMONY IN BALANCE -- SKEW / STANCE, in D major (motif SKEW-1, chosen by the user by ear)
static const s8 sSong_Stance_Notes[] = {9, 11, 13, 9, 6, 4, 6, 2};
static const u8 sSong_Stance_Pcs[] = {9, 11, 1, 9, 6, 4, 6, 2};
static const s8 sSong_Stance_Steps[] = {5, 6, 7, 5, 3, 2, 3, 1};
static const u8 sSong_Stance_Frames[] = {20, 20, 20, 20, 20, 20, 20, 48};
static const u8 sSong_Stance_Page[] = _("Heard where SCORN was beaten.");

// THE SHIMMERING CANVAS -- FRAME / NOTICE, in D major (motif FRAME-1, chosen by the user by ear)
static const s8 sSong_Notice_Notes[] = {13, 11, 6, 2, 4, 2, 6, 4};
static const u8 sSong_Notice_Pcs[] = {1, 11, 6, 2, 4, 2, 6, 4};
static const s8 sSong_Notice_Steps[] = {7, 6, 3, 1, 2, 1, 3, 2};
static const u8 sSong_Notice_Frames[] = {20, 20, 20, 20, 20, 20, 20, 48};
static const u8 sSong_Notice_Page[] = _("Heard in the READING ROOM.");

// REFLECTIONS WITHIN -- HEAT / REFLECT, in G major (motif HEAT-1, chosen by the user by ear)
static const s8 sSong_Reflect_Notes[] = {-1, 4, 2, 0, -1, 2, -1, 0, -1, -5};
static const u8 sSong_Reflect_Pcs[] = {11, 4, 2, 0, 11, 2, 11, 0, 11, 7};
static const s8 sSong_Reflect_Steps[] = {6, 9, 8, 7, 6, 8, 6, 7, 6, 4};
static const u8 sSong_Reflect_Frames[] = {22, 22, 22, 22, 22, 22, 22, 22, 22, 48};
static const u8 sSong_Reflect_Page[] = _("Heard on QUICKSILVER IS.");

static const struct PlaySong sSongs[PLAY_SONG_COUNT] =
{
    [PLAY_SONG_FIR] = {FLAG_SONG_FIR, 0, 0, 10, 8, sSong_Fir_Notes, sSong_Fir_Pcs, sSong_Fir_Steps, sSong_Fir_Frames, NULL},
    [PLAY_SONG_NAMING] = {FLAG_SONG_1, FLAG_SONG_HEARD_1, FLAG_UNDERSTANDING_SCHOOL, 4, 10, sSong_Naming_Notes, sSong_Naming_Pcs, sSong_Naming_Steps, sSong_Naming_Frames, sSong_Naming_Page},
    [PLAY_SONG_STILL] = {FLAG_SONG_2, FLAG_SONG_HEARD_2, FLAG_UNDERSTANDING_FIRST, 3, 10, sSong_Still_Notes, sSong_Still_Pcs, sSong_Still_Steps, sSong_Still_Frames, sSong_Still_Page},
    [PLAY_SONG_MEND] = {FLAG_SONG_3, FLAG_SONG_HEARD_3, FLAG_UNDERSTANDING_RETURN, 2, 11, sSong_Mend_Notes, sSong_Mend_Pcs, sSong_Mend_Steps, sSong_Mend_Frames, sSong_Mend_Page},
    [PLAY_SONG_PRACTICE] = {FLAG_SONG_4, FLAG_SONG_HEARD_4, FLAG_UNDERSTANDING_GUIDE, 7, 9, sSong_Practice_Notes, sSong_Practice_Pcs, sSong_Practice_Steps, sSong_Practice_Frames, sSong_Practice_Page},
    [PLAY_SONG_STANCE] = {FLAG_SONG_5, FLAG_SONG_HEARD_5, FLAG_UNDERSTANDING_SCORN, 2, 8, sSong_Stance_Notes, sSong_Stance_Pcs, sSong_Stance_Steps, sSong_Stance_Frames, sSong_Stance_Page},
    [PLAY_SONG_NOTICE] = {FLAG_SONG_6, FLAG_SONG_HEARD_6, FLAG_UNDERSTANDING_READING, 2, 8, sSong_Notice_Notes, sSong_Notice_Pcs, sSong_Notice_Steps, sSong_Notice_Frames, sSong_Notice_Page},
    [PLAY_SONG_REFLECT] = {FLAG_SONG_7, FLAG_SONG_HEARD_7, FLAG_UNDERSTANDING_NOTES, 7, 10, sSong_Reflect_Notes, sSong_Reflect_Pcs, sSong_Reflect_Steps, sSong_Reflect_Frames, sSong_Reflect_Page},
};
