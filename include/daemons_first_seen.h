#ifndef GUARD_DAEMONS_FIRST_SEEN_H
#define GUARD_DAEMONS_FIRST_SEEN_H

//  T-406 (the user, 2026-10-19): where each daemon was first met -- the map section it was first SEEN in, a byte by
//  national number in the recap's old fourth scene (SaveBlock1.daemonsFirstSeen).

void DaemonsFirstSeen_Record(u16 nationalNum);   // the first time it is SEEN: the map section the player stands in
u8 DaemonsFirstSeen_Get(u16 nationalNum);        // its MAPSEC, or 0: not recorded (seen before this build, or never)
void DaemonsFirstSeen_RecordAt(u16 nationalNum, u8 mapsec);   // T-448: as Record, at a given place (a load has no map yet)
void DaemonsFirstSeen_RecordCompanion(u16 nationalNum);       // T-448: first met through the companion (a SYNC delivery)
bool8 DaemonsFirstSeen_IsCompanion(u16 nationalNum);
void DaemonsFirstSeen_RecordTrade(u16 nationalNum);           // T-473: first met by a trade with another player (C-219)
bool8 DaemonsFirstSeen_IsTrade(u16 nationalNum);
void DaemonsFirstSeen_RecordGift(u16 nationalNum);            // T-492: first met as a gift from another player (C-223)
bool8 DaemonsFirstSeen_IsGift(u16 nationalNum);
void DaemonsFirstSeen_Move(u16 from, u16 to);                 // T-450: an INDEX number that moved (MISSINGNO, 395 -> 398)

void DaemonsDreamDay_Record(u16 flag, u8 weekday);   // T-591: the weekday (0-6) a dream in the DREAM LOG came on
u8 DaemonsDreamDay_Get(u16 flag);                    // its weekday + 1, or 0: not recorded (filed before this build)

void DaemonsSummoned_Set(u16 nationalNum);           // T-598: it came to its song, played in PLAY's freestyle
bool8 DaemonsSummoned_Get(u16 nationalNum);          // ...and so the INDEX shows its QUOTATION

#endif // GUARD_DAEMONS_FIRST_SEEN_H
