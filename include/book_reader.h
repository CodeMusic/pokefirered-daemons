#ifndef GUARD_BOOK_READER_H
#define GUARD_BOOK_READER_H

void BookReader_OpenNotebookPage(const u8 *title, const u8 *text);
void Textbook_Open(void);
void Guide_Open(void);
void Subtle_Open(void);   // T-423
void Hermetic_Open(void);   // T-483
void Eudaimon_Open(void);   // T-519 item 2
void Unity_Open(void);      // T-519 item 3
void Origin_Open(void);     // T-533: THE ORIGIN OF A DAEMON
u8 Origin_OpenSections(void);   // T-533: how many of its sections have come back (a run from the first; never shown)
u8 Origin_TryPageFills(void);   // T-533: the step hook's -- sections come back and not yet told, now told (0: none)
void Guide_OpenBoardCopy(void);

//  T-315: a QR code on the last page
#include "constants/book_qr.h"   // BOOK_QR_*: tools/gbaqr.py writes it
void BookReader_AddQr(u8 which);
void BookReader_AddSong(u8 song);          // T-417: a SONGBOOK page's notes
void DaemonsArriveAtUnderstandings(void);   // T-252
u8 DaemonsUnderstandingsMissing(void);      // T-317
void DaemonsClarity(u8 *w, u8 *h);          // T-317: the map's and the INDEX's mosaic

#endif // GUARD_BOOK_READER_H
