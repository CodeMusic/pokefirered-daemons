#include "global.h"
#include "daemons_first_seen.h"
#include "constants/region_map_sections.h"

//  T-406 (the user, 2026-10-19): the INDEX says where each daemon was first met. The bytes came out of the quest log
//  -- the recap holds three scenes now, not four -- so the save did not grow (include/global.h, STATIC_ASSERTed in
//  event_data.c). An older save holds a recap scene in that space, so the bytes are ours only once the magic says so:
//  its first byte, 'F', is never a scene's startType (0-2). Until then they read as zero, and are zeroed on first use.
//  A daemon seen before this build therefore shows no place ("BEFORE RECORDS" on the INDEX); every one seen after
//  fills in as it is met.
#define FIRST_SEEN_MAGIC 0x4E455346   // "FSEN", little-endian

static bool8 FirstSeenIsOurs(void)
{
    return gSaveBlock1Ptr->daemonsFirstSeenMagic == FIRST_SEEN_MAGIC;
}

static void FirstSeenClaim(void)
{
    if (FirstSeenIsOurs())
        return;
    memset(gSaveBlock1Ptr->daemonsFirstSeen, 0, sizeof(gSaveBlock1Ptr->daemonsFirstSeen));
    memset(gSaveBlock1Ptr->questLogFreed, 0, sizeof(gSaveBlock1Ptr->questLogFreed));
    gSaveBlock1Ptr->daemonsFirstSeenMagic = FIRST_SEEN_MAGIC;
}

void DaemonsFirstSeen_Record(u16 nationalNum)
{
    u8 mapsec = gMapHeader.regionMapSectionId;

    FirstSeenClaim();
    if (nationalNum == 0 || nationalNum > DAEMONS_FIRST_SEEN_COUNT)
        return;
    if (mapsec < KANTO_MAPSEC_START || mapsec >= MAPSEC_NONE)
        return;
    if (gSaveBlock1Ptr->daemonsFirstSeen[nationalNum - 1] == 0)
        gSaveBlock1Ptr->daemonsFirstSeen[nationalNum - 1] = mapsec;
}

u8 DaemonsFirstSeen_Get(u16 nationalNum)
{
    u8 mapsec;

    if (!FirstSeenIsOurs() || nationalNum == 0 || nationalNum > DAEMONS_FIRST_SEEN_COUNT)
        return 0;
    mapsec = gSaveBlock1Ptr->daemonsFirstSeen[nationalNum - 1];
    return (mapsec >= KANTO_MAPSEC_START && mapsec < MAPSEC_NONE) ? mapsec : 0;
}

//  T-448: a daemon the companion delivered (metLocation METLOC_FATEFUL_ENCOUNTER) was first met there, not on a map; the
//  byte keeps 0xFF for it, which no MAPSEC is, so DaemonsFirstSeen_Get still reads it as no place.
#define FIRST_SEEN_COMPANION METLOC_FATEFUL_ENCOUNTER
//  T-473: one that arrived by a trade with another player (C-219), the same way -- 0xFE, no MAPSEC either
#define FIRST_SEEN_TRADE METLOC_IN_GAME_TRADE
//  T-492: one GIVEN by another player, nothing back (C-223) -- 0xFD, METLOC_SPECIAL_EGG's number, which FIRST MET never
//  recorded (an egg is first seen where it hatches); no MAPSEC either
#define FIRST_SEEN_GIFT METLOC_SPECIAL_EGG

void DaemonsFirstSeen_RecordAt(u16 nationalNum, u8 mapsec)
{
    FirstSeenClaim();
    if (nationalNum == 0 || nationalNum > DAEMONS_FIRST_SEEN_COUNT)
        return;
    if (mapsec != FIRST_SEEN_COMPANION && mapsec != FIRST_SEEN_TRADE && mapsec != FIRST_SEEN_GIFT
     && (mapsec < KANTO_MAPSEC_START || mapsec >= MAPSEC_NONE))
        return;
    if (gSaveBlock1Ptr->daemonsFirstSeen[nationalNum - 1] == 0)
        gSaveBlock1Ptr->daemonsFirstSeen[nationalNum - 1] = mapsec;
}

void DaemonsFirstSeen_RecordCompanion(u16 nationalNum)
{
    DaemonsFirstSeen_RecordAt(nationalNum, FIRST_SEEN_COMPANION);
}

void DaemonsFirstSeen_RecordTrade(u16 nationalNum)
{
    DaemonsFirstSeen_RecordAt(nationalNum, FIRST_SEEN_TRADE);
}

bool8 DaemonsFirstSeen_IsTrade(u16 nationalNum)
{
    if (!FirstSeenIsOurs() || nationalNum == 0 || nationalNum > DAEMONS_FIRST_SEEN_COUNT)
        return FALSE;
    return gSaveBlock1Ptr->daemonsFirstSeen[nationalNum - 1] == FIRST_SEEN_TRADE;
}

void DaemonsFirstSeen_RecordGift(u16 nationalNum)
{
    DaemonsFirstSeen_RecordAt(nationalNum, FIRST_SEEN_GIFT);
}

bool8 DaemonsFirstSeen_IsGift(u16 nationalNum)
{
    if (!FirstSeenIsOurs() || nationalNum == 0 || nationalNum > DAEMONS_FIRST_SEEN_COUNT)
        return FALSE;
    return gSaveBlock1Ptr->daemonsFirstSeen[nationalNum - 1] == FIRST_SEEN_GIFT;
}

bool8 DaemonsFirstSeen_IsCompanion(u16 nationalNum)
{
    if (!FirstSeenIsOurs() || nationalNum == 0 || nationalNum > DAEMONS_FIRST_SEEN_COUNT)
        return FALSE;
    return gSaveBlock1Ptr->daemonsFirstSeen[nationalNum - 1] == FIRST_SEEN_COMPANION;
}

void DaemonsFirstSeen_Move(u16 from, u16 to)
{
    if (!FirstSeenIsOurs() || from == 0 || to == 0 || from > DAEMONS_FIRST_SEEN_COUNT || to > DAEMONS_FIRST_SEEN_COUNT)
        return;
    gSaveBlock1Ptr->daemonsFirstSeen[to - 1] = gSaveBlock1Ptr->daemonsFirstSeen[from - 1];
    gSaveBlock1Ptr->daemonsFirstSeen[from - 1] = 0;
}
