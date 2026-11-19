// T-598 / T-599: each daemon's QUOTATION -- what it says when it comes to its song, played in PLAY's freestyle, and
// what its INDEX page shows from then on (src/daemon_song.c: DaemonSong_Quotation, DaemonSong_IndexQuotation).
//
// A STUB, written by hand for T-598: every entry NULL, so a daemon that comes says nothing, listens and goes. T-599's
// generator (the narrative lane) will own this file and write it whole -- [SPECIES_X] = sText_..., each a message box's
// lines of at most 208px, every word DRAFT. Until then, do not add words here by hand.

static const u8 *const sDaemonQuotations[NUM_SPECIES] = {
    [SPECIES_NONE] = NULL,
};
