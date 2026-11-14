#include "global.h"
#include "event_data.h"
#include "event_object_movement.h"
#include "field_player_avatar.h"
#include "string_util.h"
#include "constants/event_object_movement.h"
#include "daemons_blank_tablet.h"
#include "daemons_eudaimon.h"
#include "daemons_hermetic.h"
#include "daemons_spellbook.h"
#include "daemons_time.h"
#include "daemons_unity.h"

//  T-547: THE BLANK TABLET (the user, 2026-11-12, by multiple choice; vision 4.39, 4.40; every word DRAFT).
//
//  A stone by the path in BLANCHE with nothing on it. It is written by the player without their knowing: each of the
//  twelve spells, the first time it is cast (VAR_SPELLS_CAST), puts its saying on the stone, in the voice the ledger
//  leans toward -- reason, exact and numbered, or intuition, felt and open -- so a line is added, and reshaped as the
//  lean moves. THE PULL, THE WITCHING HOUR and THE HORIZON write nothing.
//
//  ONE STONE, TWO FACES. The face read is the side stood on: from the south the front, from the north the back, from
//  either side whichever face can be read (the front first). THE HERMETIC LAYER reads the front, in its terms (what a
//  spell cost); THE EUDAIMON LAYER the back, in its own (the same thing, given); THE UNITY BOOK both. A face that cannot
//  be read is blank, and says so plainly to one who holds the other half.
//
//  THE THIRTEENTH LINE: the player's name, then one saying for the pole leaned on hardest of all four. The front's
//  shows only in the witching half-hour (3:30-3:59, Spellbook_HalfHour), the back's only in THE CREATOR'S HOUR
//  (4:24-5:12); each needs a spell cast, and once read it never shows again (FLAG_BLANK_TABLET_*_READ -- the brief's
//  "one window ever" would almost never be seen; the draft says so).
//
//  THE OLD TOAD by the pond says only the player's current lean -- the pole leaned on hardest -- and nothing else; the
//  kinder wording in the dawn watch or THE CREATOR'S HOUR. Before any cast she says she can wait.
//
//  RAM: IWRAM 3 bytes (the face, the page, the thirteenth due), for the length of one reading. Save: two flags.

enum
{
    BLANK_AWAKENING, BLANK_CONTRAST, BLANK_ROOM, BLANK_MIRROR, BLANK_PATTERN, BLANK_RETURN, BLANK_SMALL_HAND,
    BLANK_GREAT_HAND, BLANK_PROCESS, BLANK_SEAM, BLANK_LENS, BLANK_FIELD, BLANK_SAYINGS
};

#include "data/blank_tablet_words.h"

static const u8 sBlankSpell[BLANK_SAYINGS] =
{
    SPELL_AWAKENING, SPELL_CONTRAST, SPELL_ROOM, SPELL_MIRROR, SPELL_PATTERN, SPELL_RETURN, SPELL_SMALL_HAND,
    SPELL_GREAT_HAND, SPELL_PROCESS, SPELL_SEAM, SPELL_LENS, SPELL_FIELD,
};

enum { FACE_FRONT, FACE_BACK };
enum { STONE_NOTHING_HELD, STONE_NOTHING_YET, STONE_BACK_BLANK, STONE_FRONT_BLANK };

// The page after the sayings: the thirteenth line's name, then its saying, then nothing.
#define PAGE_NAME   BLANK_SAYINGS
#define PAGE_LINE13 (BLANK_SAYINGS + 1)
#define PAGE_DONE   (BLANK_SAYINGS + 2)
#define PAGE_SAID   0xFF    // the stone with nothing to read has said its one page

// The thirteenth line opens with the name alone, on a page of its own.
static const u8 sText_Name[] = _("{PLAYER}");

static u8 sFace;
static u8 sPage;
static bool8 sThirteenthDue;

static bool8 CanRead(u8 face)
{
    if (Unity_Mended())
        return TRUE;
    return face == FACE_FRONT ? Hermetic_Held() : Eudaimon_Held();
}

static bool8 AnyCast(void)
{
    u8 i;

    for (i = 0; i < BLANK_SAYINGS; i++)
        if (VarGet(VAR_SPELLS_CAST) & (1 << sBlankSpell[i]))
            return TRUE;
    return FALSE;
}

// The pole leaned on hardest, or LEDGER_COUNT when the ledger is empty. Ties go to the first (the book's order).
static u8 Hardest(void)
{
    u8 axis, best = LEDGER_COUNT, most = 0;

    for (axis = 0; axis < LEDGER_COUNT; axis++)
    {
        if (Spellbook_Ledger(axis) > most)
        {
            most = Spellbook_Ledger(axis);
            best = axis;
        }
    }
    return best;
}

static u16 ThirteenthFlag(u8 face)
{
    return face == FACE_FRONT ? FLAG_BLANK_TABLET_FRONT_READ : FLAG_BLANK_TABLET_BACK_READ;
}

static bool8 InFaceHour(u8 face)
{
    return face == FACE_FRONT ? Spellbook_HalfHour() : DaemonsCreatorSecond() >= 0;
}

void BlankTablet_Begin(void)
{
    u8 facing = gObjectEvents[gPlayerAvatar.objectEventId].facingDirection;

    if (facing == DIR_NORTH)
        sFace = FACE_FRONT;
    else if (facing == DIR_SOUTH)
        sFace = FACE_BACK;
    else
        sFace = CanRead(FACE_FRONT) ? FACE_FRONT : FACE_BACK;
    sPage = 0;
    sThirteenthDue = CanRead(sFace) && AnyCast() && InFaceHour(sFace) && !FlagGet(ThirteenthFlag(sFace))
                  && Hardest() != LEDGER_COUNT;
}

static void Say(const u8 *text)
{
    StringExpandPlaceholders(gStringVar4, text);
}

u16 BlankTablet_Next(void)
{
    if (sPage == PAGE_SAID)
        return FALSE;
    if (!CanRead(sFace))
    {
        if (CanRead(sFace == FACE_FRONT ? FACE_BACK : FACE_FRONT))
            Say(sBlankStone[sFace == FACE_FRONT ? STONE_FRONT_BLANK : STONE_BACK_BLANK]);
        else
            Say(sBlankStone[STONE_NOTHING_HELD]);
        sPage = PAGE_SAID;
        return TRUE;
    }
    if (!AnyCast())
    {
        Say(sBlankStone[STONE_NOTHING_YET]);
        sPage = PAGE_SAID;
        return TRUE;
    }
    while (sPage < BLANK_SAYINGS)
    {
        u8 i = sPage++;

        if (VarGet(VAR_SPELLS_CAST) & (1 << sBlankSpell[i]))
        {
            bool8 intuition = Spellbook_Ledger(LEDGER_REASON) <= Spellbook_Ledger(LEDGER_INTUITION);

            Say(sBlankSayings[i][sFace][intuition]);
            return TRUE;
        }
    }
    if (!sThirteenthDue)
        return FALSE;
    if (sPage == PAGE_NAME)
    {
        sPage = PAGE_LINE13;
        Say(sText_Name);
        return TRUE;
    }
    if (sPage == PAGE_LINE13)
    {
        sPage = PAGE_DONE;
        FlagSet(ThirteenthFlag(sFace));
        Say(sBlankThirteenth[Hardest()][sFace]);
        return TRUE;
    }
    return FALSE;
}

void OldToad_Line(void)
{
    u8 pole = Hardest();
    bool8 dawn = DaemonsWatch() == WATCH_DAWN || DaemonsCreatorSecond() >= 0;

    Say(sOldToad[pole == LEDGER_COUNT ? 0 : 1 + pole][dawn]);
}
