#ifndef GUARD_BOOK_READER_H
#define GUARD_BOOK_READER_H

void BookReader_OpenNotebookPage(const u8 *title, const u8 *text);
void Textbook_Open(void);
void Guide_Open(void);
void Guide_OpenBoardCopy(void);

//  T-315: a QR code on the last page
enum { BOOK_QR_NONE, BOOK_QR_FOLDS, BOOK_QR_GUIDE };
void BookReader_AddQr(u8 which);

#endif // GUARD_BOOK_READER_H
