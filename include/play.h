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

void Play_Start(void);
bool8 Play_IsActive(void);
// A note: `semitone` from the instrument's middle C (negative, and past 12, are octaves by bend), on SE2 or SE3.
void Play_SoundNote(u8 instrument, s16 semitone, bool8 onSe3);
#if DAEMONS_DEBUG
void Play_DebugScale(u8 instrument);   // DEBUG -> SOUND: C major up three octaves
#endif

#endif // GUARD_PLAY_H
