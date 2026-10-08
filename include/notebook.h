#ifndef GUARD_NOTEBOOK_H
#define GUARD_NOTEBOOK_H

//  T-390: an insight arrives when a MARK and its understanding are both held (notebook.c).
bool8 Daemons_TryInsightArrives(void);
#if DAEMONS_DEBUG
void Notebook_DebugFillAll(void);   // T-394
#endif

#endif // GUARD_NOTEBOOK_H
