#ifndef GUARD_DAEMONS_READS_H
#define GUARD_DAEMONS_READS_H

// T-459 B (the user, 2026-10-27: "A read:", the book's own word -- "every read is a lean, not a fact"): once THE SUBTLE
// LAYER is held, now and then the first step on a map brings a read in the book's purple -- "A read: / something by the
// water." -- about something still unfound there, or about nothing at all. Each is written in the NOTEBOOK's log (READS)
// and marked once the thing it was about is found. The log decides: the game never vouches (bible 4.33). Nothing in a
// battle reads it, and T-454's three values play no part: a read is the field's, a lean is the battle's.

// ---- THE TABLE (DRAFT, the user tunes) -------------------------------------------------------------------------------
#define READ_CHANCE         12      // percent of arrivals (the first step on a map that is not the last one) a read comes
#define READ_REST           4       // and never in the next this many arrivals after one
#define READ_TRUE           75      // percent of reads about something really there, when the map has anything unfound
#define READ_NEAR           6       // tiles (across plus down) within which a thing is "close by"
#define READ_LOOK           2       // tiles around a thing looked at for water, grass or sand
#define READ_LOG            4       // the NOTEBOOK keeps the last this many reads (VAR_READ_LOG_WHERE/_THING + 0..3)
// -----------------------------------------------------------------------------------------------------------------------

bool8 Daemons_TryReadArrives(void);         // field_control_avatar.c's step hook; the line is left in gStringVar4
void Daemons_WriteReadsLog(u8 *dest);       // notebook.c: THE LOG's page -- the reads and the leans, counted

#if DAEMONS_DEBUG
extern u8 gReadsDebugAlways;                // 1: a read on every arrival, 2: and always a true one (the theatre)
#endif

#endif // GUARD_DAEMONS_READS_H
