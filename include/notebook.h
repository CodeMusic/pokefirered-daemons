#ifndef GUARD_NOTEBOOK_H
#define GUARD_NOTEBOOK_H

//  T-390: an insight arrives when a MARK and its understanding are both held (notebook.c).
bool8 Daemons_TryInsightArrives(void);
//  T-410: a MARK's balance (0-7, the MARKS' order) and an understanding's, once the MARK and its ROOT are both held.
const u8 *Daemons_MarkBalance(u8 mark);
const u8 *Daemons_RootBalance(u16 root);
//  T-412: the DREAM LOG -- noticed on map loads, said on a step, and a continue counts as a load on its day.
void DaemonsDreamLog_Notice(void);
void DaemonsDreamLog_Loaded(void);
bool8 Daemons_TryDreamArrives(void);
//  T-412: HER DREAMS -- where the attendant's dream for a step led, once the step is done (daemons_dreams.c); NULL till then.
const u8 *DaemonsDream_Led(u8 step);
s16 DaemonsDream_NextStep(void);                 // T-483 (5): THE GREAT HAND's road -- the earliest step undone, or -1
const u8 *DaemonsDream_Road(u8 step);           // ... and its plain dream, the one that names the place
#if DAEMONS_DEBUG
void DaemonsDreamLog_DebugDawn(void);
void DaemonsItsDream_DebugReceive(void);                 // T-462: a SYNC's new dream, faked
void DaemonsItsDream_DebugNext(u8 *image, u8 *quality);  // T-462: what the next one faked will be
#endif
#if DAEMONS_DEBUG
void Notebook_DebugFillAll(void);   // T-394
#endif

#endif // GUARD_NOTEBOOK_H
