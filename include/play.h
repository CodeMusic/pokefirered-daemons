#ifndef GUARD_PLAY_H
#define GUARD_PLAY_H

// T-417: PLAY -- the instrument, the KEY WHEEL and the STAFF (src/play.c).

enum { PLAY_HARP, PLAY_FLUTE };

// The one OBJ palette every colour of a note is drawn from (tools/genplaynotes.py): 1-12 the twelve keys, C to B.
#define PALTAG_KEY_COLOURS 0x1042

#ifdef FIRERED
#define PLAY_INSTRUMENT PLAY_HARP    // CONTENT's
#define PLAY_ITEM       ITEM_HARP
#else
#define PLAY_INSTRUMENT PLAY_FLUTE   // CONTEXT's
#define PLAY_ITEM       ITEM_FLUTE
#endif

// T-417 P2: the songs PLAY listens for (src/play.c's sSongs), and the SONGBOOK's pages of them.
enum { PLAY_SONG_FIR, PLAY_SONG_1, PLAY_SONG_COUNT = PLAY_SONG_1 + 7 };

void Play_Start(void);
void Play_WriteSongPage(u8 song, u8 *dest);
void Play_BookSong(u8 song, s16 left, s16 middleC);   // its notes as sprites on the NOTEBOOK's page (book_reader.c)
void Play_BookSongFree(void);
bool8 Play_IsActive(void);
// A note: `semitone` from the instrument's middle C (negative, and past 12, are octaves by bend), on SE2 or SE3.
void Play_SoundNote(u8 instrument, s16 semitone, bool8 onSe3);
#if DAEMONS_DEBUG
void Play_DebugScale(u8 instrument);   // DEBUG -> SOUND: C major up three octaves
#endif

#endif // GUARD_PLAY_H
