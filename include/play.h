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

// T-417 P2: the songs PLAY listens for (src/play.c's sSongs), and the SONGBOOK's pages of them. P4: their ids are
// constants, so scripts can name them.
#include "constants/play.h"

void Play_Start(void);
void Play_WriteSongPage(u8 song, u8 *dest);
void Play_BookSong(u8 song, s16 left, s16 middleC);   // its notes as sprites on the NOTEBOOK's page (book_reader.c)
void Play_BookSongFree(void);
bool8 Play_IsActive(void);
// T-417 P4: specials (data/scripts/play.inc). VAR_0x8004 is the song.
bool8 Play_ShouldHear(void);     // its understanding held and the song not yet learned
void Play_Hear(void);            // its place plays it (waitstate)
void Play_BufferSongKey(void);   // STR_VAR_1: its key
void Play_Bell(void);            // the LYREBIRD's bell, on the HARP (waitstate)
void Play_ShowDayBanner(void);
void Play_NoteFled(void);        // EventScript_MonFlewAway
// T-417 P5: THE NEXUS's tree, lit and kept lit (FLAG_NEXUS_LIT).
void Play_OnTilesetPalettesLoaded(void);   // fieldmap.c, as the secondary tileset's palettes load
void Play_LightNexus(void);                 // the special, as it is lit
// A note: `semitone` from the instrument's middle C (negative, and past 12, are octaves by bend), on SE2 or SE3.
void Play_SoundNote(u8 instrument, s16 semitone, bool8 onSe3);
#if DAEMONS_DEBUG
void Play_DebugScale(u8 instrument);   // DEBUG -> SOUND: C major up three octaves
#endif

#endif // GUARD_PLAY_H
