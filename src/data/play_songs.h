// T-417 PLAY's songs -- written by DAEMONS tools/genplaysongs.py from docs/play_songs.json. Do not edit:
// edit the json and run the tool. Every page is DRAFT; the titles are src/notebook.c's.

// O CHRISTMAS TREE -- the fir, in Bb major
static const s8 sSong_Fir_Notes[] = {5, 10, 10, 10, 12, 14, 14, 14};
static const u8 sSong_Fir_Pcs[] = {5, 10, 10, 10, 0, 2, 2, 2};
static const s8 sSong_Fir_Steps[] = {3, 6, 6, 6, 7, 8, 8, 8};
static const u8 sSong_Fir_Frames[] = {30, 22, 8, 40, 18, 22, 8, 40};

// TWELVE DAYS OF DECEMBER -- SLATE / NAMING, in E major (motif NAMING-B, chosen by the user by ear)
static const s8 sSong_Naming_Notes[] = {4, 9, 11, 11, 8, 9, 11, 11, 6, 8, 6};
static const u8 sSong_Naming_Pcs[] = {4, 9, 11, 11, 8, 9, 11, 11, 6, 8, 6};
static const s8 sSong_Naming_Steps[] = {2, 5, 6, 6, 4, 5, 6, 6, 3, 4, 3};
static const u8 sSong_Naming_Frames[] = {15, 7, 15, 22, 15, 7, 15, 7, 7, 22, 30};
static const u8 sSong_Naming_Page[] = _("Heard at CALLOW SCHOOL.");

// THE REALIZATION -- SLOPE / STILL, in Eb major (motif STILL-B, chosen by the user by ear)
static const s8 sSong_Still_Notes[] = {-2, 3, 5, 7, 7, 5, 7, 5, 7};
static const u8 sSong_Still_Pcs[] = {10, 3, 5, 7, 7, 5, 7, 5, 7};
static const s8 sSong_Still_Steps[] = {-1, 2, 3, 4, 4, 3, 4, 3, 4};
static const u8 sSong_Still_Frames[] = {16, 11, 16, 22, 11, 11, 22, 22, 22};
static const u8 sSong_Still_Page[] = _("Heard in the TANOBY RUINS.");

// FORGIVENESS OFFERED -- SENSE / MEND, in D major (motif MEND-A, chosen by the user by ear)
static const s8 sSong_Mend_Notes[] = {2, 2, 4, 6, -1, 2};
static const u8 sSong_Mend_Pcs[] = {2, 2, 4, 6, 11, 2};
static const s8 sSong_Mend_Steps[] = {8, 8, 9, 10, 6, 8};
static const u8 sSong_Mend_Frames[] = {20, 20, 10, 10, 10, 39};
static const u8 sSong_Mend_Page[] = _("Hummed by CRYSTAL.");

// SEQUEL THE GUARDIAN -- FIT / PRACTICE, in G major (motif PRACTICE-A, chosen by the user by ear)
static const s8 sSong_Practice_Notes[] = {-1, 2, -1, 2, 4, -3, -1};
static const u8 sSong_Practice_Pcs[] = {11, 2, 11, 2, 4, 9, 11};
static const s8 sSong_Practice_Steps[] = {6, 8, 6, 8, 9, 5, 6};
static const u8 sSong_Practice_Frames[] = {8, 16, 8, 16, 16, 8, 33};
static const u8 sSong_Practice_Page[] = _("Heard where the GUIDE was kept.");

// HARMONY IN BALANCE -- SKEW / STANCE, in D major (motif STANCE-B, chosen by the user by ear)
static const s8 sSong_Stance_Notes[] = {6, 4, 2, -3, -3, 6, 4, 2, 4, -1};
static const u8 sSong_Stance_Pcs[] = {6, 4, 2, 9, 9, 6, 4, 2, 4, 11};
static const s8 sSong_Stance_Steps[] = {10, 9, 8, 5, 5, 10, 9, 8, 9, 6};
static const u8 sSong_Stance_Frames[] = {20, 10, 29, 10, 10, 10, 10, 10, 10, 39};
static const u8 sSong_Stance_Page[] = _("Heard where SCORN was beaten.");

// THE SHIMMERING CANVAS -- FRAME / NOTICE, in D major (motif NOTICE-B, chosen by the user by ear)
static const s8 sSong_Notice_Notes[] = {2, 1, -1, -3, -1, 1, 2, -1, 1, 2};
static const u8 sSong_Notice_Pcs[] = {2, 1, 11, 9, 11, 1, 2, 11, 1, 2};
static const s8 sSong_Notice_Steps[] = {8, 7, 6, 5, 6, 7, 8, 6, 7, 8};
static const u8 sSong_Notice_Frames[] = {10, 10, 10, 10, 10, 10, 20, 10, 10, 40};
static const u8 sSong_Notice_Page[] = _("Heard in the READING ROOM.");

// REFLECTIONS WITHIN -- HEAT / REFLECT, in G major (motif REFLECT-A, chosen by the user by ear)
static const s8 sSong_Reflect_Notes[] = {2, 4, 4, 2, -1, -3, -5, -3, -1, -1};
static const u8 sSong_Reflect_Pcs[] = {2, 4, 4, 2, 11, 9, 7, 9, 11, 11};
static const s8 sSong_Reflect_Steps[] = {8, 9, 9, 8, 6, 5, 4, 5, 6, 6};
static const u8 sSong_Reflect_Frames[] = {14, 14, 43, 21, 7, 7, 7, 14, 14, 29};
static const u8 sSong_Reflect_Page[] = _("Heard on QUICKSILVER IS.");

// DUET -- the conjugate light, in C major (motif DUET-1, chosen by the user by ear)
static const s8 sSong_Duet_Notes[] = {4, 2, 2, 0, 2, 4};
static const u8 sSong_Duet_Pcs[] = {4, 2, 2, 0, 2, 4};
static const s8 sSong_Duet_Steps[] = {9, 8, 8, 7, 8, 9};
static const u8 sSong_Duet_Frames[] = {11, 22, 22, 22, 22, 30};
static const s8 sSong_Duet_PartNotes[] = {4, 5, 5, 7, 5, 4};
static const u8 sSong_Duet_PartFrames[] = {11, 22, 22, 22, 22, 30};
static const u8 sSong_Duet_Page[] = _("Played under both colours.");

static const struct PlaySong sSongs[PLAY_SONG_COUNT] =
{
    [PLAY_SONG_FIR] = {FLAG_SONG_FIR, 0, 0, 10, 8, sSong_Fir_Notes, sSong_Fir_Pcs, sSong_Fir_Steps, sSong_Fir_Frames, NULL},
    [PLAY_SONG_NAMING] = {FLAG_SONG_1, FLAG_SONG_HEARD_1, FLAG_UNDERSTANDING_SCHOOL, 4, 11, sSong_Naming_Notes, sSong_Naming_Pcs, sSong_Naming_Steps, sSong_Naming_Frames, sSong_Naming_Page},
    [PLAY_SONG_STILL] = {FLAG_SONG_2, FLAG_SONG_HEARD_2, FLAG_UNDERSTANDING_FIRST, 3, 9, sSong_Still_Notes, sSong_Still_Pcs, sSong_Still_Steps, sSong_Still_Frames, sSong_Still_Page},
    [PLAY_SONG_MEND] = {FLAG_SONG_3, FLAG_SONG_HEARD_3, FLAG_UNDERSTANDING_RETURN, 2, 6, sSong_Mend_Notes, sSong_Mend_Pcs, sSong_Mend_Steps, sSong_Mend_Frames, sSong_Mend_Page},
    [PLAY_SONG_PRACTICE] = {FLAG_SONG_4, FLAG_SONG_HEARD_4, FLAG_UNDERSTANDING_GUIDE, 7, 7, sSong_Practice_Notes, sSong_Practice_Pcs, sSong_Practice_Steps, sSong_Practice_Frames, sSong_Practice_Page},
    [PLAY_SONG_STANCE] = {FLAG_SONG_5, FLAG_SONG_HEARD_5, FLAG_UNDERSTANDING_SCORN, 2, 10, sSong_Stance_Notes, sSong_Stance_Pcs, sSong_Stance_Steps, sSong_Stance_Frames, sSong_Stance_Page},
    [PLAY_SONG_NOTICE] = {FLAG_SONG_6, FLAG_SONG_HEARD_6, FLAG_UNDERSTANDING_READING, 2, 10, sSong_Notice_Notes, sSong_Notice_Pcs, sSong_Notice_Steps, sSong_Notice_Frames, sSong_Notice_Page},
    [PLAY_SONG_REFLECT] = {FLAG_SONG_7, FLAG_SONG_HEARD_7, FLAG_UNDERSTANDING_NOTES, 7, 10, sSong_Reflect_Notes, sSong_Reflect_Pcs, sSong_Reflect_Steps, sSong_Reflect_Frames, sSong_Reflect_Page},
    [PLAY_SONG_DUET] = {FLAG_SONG_DUET, 0, 0, 0, 6, sSong_Duet_Notes, sSong_Duet_Pcs, sSong_Duet_Steps, sSong_Duet_Frames, sSong_Duet_Page, Aurora_ConjugateLight, 6, sSong_Duet_PartNotes, sSong_Duet_PartFrames},
};
