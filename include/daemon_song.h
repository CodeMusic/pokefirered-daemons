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

// T-596: THE WISH TO SING. A daemon near its evolution hums one bar of its own part, quietly under the map's music
// (the follower, src/daemons_follower.c); each note it sings can be read back as it sounds, to draw it.
#define SONG_WISH_FROM  160    // its own part's share from which it wishes to sing (of 255: five-eighths learned)

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

// T-596: how much it wishes to sing, 0 (not at all: an egg, a final form, under SONG_WISH_FROM) to 255 (perfect).
u8 DaemonSong_Wish(struct Pokemon *mon);
// One bar of its own part as it knows it: the bar it is learning, or now and then one it has already, by `seed`.
bool8 DaemonSong_Hum(u16 species, u8 share, u16 seed);
bool8 DaemonSong_IsHumming(void);
// The note sounding: a number new with every note, its pitch heard this frame and its true pitch (semitones from middle
// C), and how near it is to learned (0..255, or -1: learned). FALSE when nothing is sung.
bool8 DaemonSong_Heard(u16 *serial, s16 *heard, s16 *truePitch, s16 *near);
// T-597: COLOUR BY SONG. How much of each of its 16 palette slots' colour a form shows at this share of its own part,
// in 256ths (SONG_COLOUR_KEEP_LOW .. 256; slot 0 and every slot not tied to a note: 256).
void DaemonSong_ColourKeeps(u16 species, u8 share, u16 *keeps);
// `later` is a form after `earlier` in its line (its song continues earlier's parts)
bool8 DaemonSong_Continues(u16 earlier, u16 later);
// T-598: PLAY A DAEMON'S SONG AND IT COMES. After every note in PLAY (play.c's Listen, once no SONGBOOK song matched):
// KEY the wheel's key (0 C .. 11 B), RING the last COUNT notes played (semitones from middle C, oldest first). The
// summon's script when they end with the opening of a daemon the INDEX has seen (VAR_0x8004 its species, its SUMMONED
// bit set), else NULL.
const u8 *DaemonSong_Listen(u8 key, const s8 *ring, u8 count);
const u8 *DaemonSong_Quotation(u16 species);        // T-599's QUOTATION, or NULL (every one, until it lands)
const u8 *DaemonSong_IndexQuotation(u16 species);   // ...only once it has come to its song: the INDEX's
#if DAEMONS_DEBUG
u32 DaemonSong_ExpAtShare(u16 species, u8 share);   // DEBUG -> FOLLOWER: the experience its part is this far at
#endif

#endif // GUARD_DAEMON_SONG_H
