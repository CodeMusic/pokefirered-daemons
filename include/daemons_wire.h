#ifndef GUARD_DAEMONS_WIRE_H
#define GUARD_DAEMONS_WIRE_H

//  T-621: THE WIRE (a spike, DEBUG only). docs/wire-spike.md.

#define WIRE_HOST_MAGIC 0x45524957  // "WIRE": written every frame by a host that is listening

enum {
    WIRE_IDLE,
    WIRE_ASKED,      // the game asked; the host is writing
    WIRE_ANSWERED,   // the host wrote the line into gStringVar4
    WIRE_FAILED,     // the host could not; the written line is said
};

struct DaemonsWire
{
    u32 host;        // WIRE_HOST_MAGIC while a host is there; the game clears it to see it come back
    u8 state;
    u8 speaker;      // the local id of who was talked to
    u8 mapGroup;
    u8 mapNum;
    u16 seq;         // which ask this is
    u16 waited;      // frames the last ask waited
};

#if DAEMONS_DEBUG
extern struct DaemonsWire gDaemonsWire;
#endif

void DaemonsWire_Ask(void);   // special (waitstate): VAR_RESULT TRUE when a line was written into gStringVar4

#endif // GUARD_DAEMONS_WIRE_H
