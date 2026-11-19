#ifndef GUARD_DAEMON_SONG_H
#define GUARD_DAEMON_SONG_H

// T-595: a daemon's SONG in the engine (src/daemon_song.c; the songs are T-594's, DAEMONS tools/gensongs.py --write
// writes src/data/daemon_songs.h). A form's song is its line's parts so far, oldest first; each part is learned from
// the experience between the level its form begins and the level it is perfect at, and a note not yet learned is
// sung as STATIC CLOSING ON THE NOTE. DRAFT.

#define SONG_SHARE_WHOLE 255   // a share of a part (0..255): every note learned

// Start's modes
#define SONG_LINE       0      // the whole line's song, the earlier parts whole and the last at its share
#define SONG_OWN_PART   1      // only the form's own part (the evolution scene)
#define SONG_ALLUSION   2      // with SONG_OWN_PART: only its opening, as its learning begins (a next form's hint)

bool8 DaemonSong_Has(u16 species);
bool8 DaemonSong_Start(u16 species, u8 share, u8 mode);   // FALSE: no song for it (play its cry instead)
bool8 DaemonSong_IsPlaying(void);
void DaemonSong_Stop(void);
u8 DaemonSong_ShareOf(u16 species, u32 exp);               // its own part, learned from its experience
u8 DaemonSong_MonShare(struct Pokemon *mon);
bool8 DaemonSong_GrowsOn(u16 species);                     // a form after it: it is still learning something new
// The INDEX's SONG: a held daemon of the species sings as it knows it (the party first, then the boxes); a form
// nobody holds -- a past form, or one only seen -- is sung whole. Pressed again, it stops.
void DaemonSong_PlayForIndex(u16 species);

#endif // GUARD_DAEMON_SONG_H
