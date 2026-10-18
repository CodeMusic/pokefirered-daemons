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
#if DAEMONS_DEBUG
void DaemonsDreamLog_DebugDawn(void);
#endif
#if DAEMONS_DEBUG
void Notebook_DebugFillAll(void);   // T-394
#endif

#endif // GUARD_NOTEBOOK_H
