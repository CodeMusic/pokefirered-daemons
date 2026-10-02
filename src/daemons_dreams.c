#include "global.h"
#include "event_data.h"
#include "string_util.h"
#include "characters.h"
#include "constants/flags.h"
#include "constants/vars.h"

//  T-347 (the user, 2026-10-02): THE CHECKPOINT ATTENDANT'S DREAM. A player who has healed three times with no
//  main-path progress is offered a dream about the next step: the first pure image, the second -- asked again while
//  still stuck -- plainer, naming the place. The steps are the AI playtester's progress list, so the two always agree
//  about what comes next (tools/gbadreams.py writes the table; the GUIDE and the first UNDERSTANDING are never in it).
//
//  PROGRESS is the number of steps done, not the first one undone: a player who skips one step and carries on is
//  moving, not stuck. The dream is about the earliest step still undone.
//
//  VAR_DREAM_PROGRESS: steps done + 1 (0 never seen), and DREAM_TOLD_FIRST once the first dream for this stretch
//  has been told. VAR_DREAM_HEALS: heals since progress last moved, or since she last offered.

#include "data/attendant_dreams.h"

#define DREAM_AFTER_HEALS 3
#define DREAM_TOLD_FIRST  0x8000

static u16 StepsDone(void)
{
    u16 i, done = 0;

    for (i = 0; i < ARRAY_COUNT(sAttendantDreams); i++)
        if (FlagGet(sAttendantDreams[i].flag))
            done++;
    return done;
}

static s16 NextStep(void)
{
    u16 i;

    for (i = 0; i < ARRAY_COUNT(sAttendantDreams); i++)
        if (!FlagGet(sAttendantDreams[i].flag))
            return i;
    return -1;
}

//  Called after every heal. VAR_RESULT TRUE when she should offer a dream.
void DaemonsDreamCountHeal(void)
{
    u16 progress = StepsDone() + 1;
    u16 heals;

    gSpecialVar_Result = FALSE;
    if (NextStep() < 0)
        return;
    if ((VarGet(VAR_DREAM_PROGRESS) & ~DREAM_TOLD_FIRST) != progress)
    {
        VarSet(VAR_DREAM_PROGRESS, progress);
        VarSet(VAR_DREAM_HEALS, 0);
    }
    heals = VarGet(VAR_DREAM_HEALS) + 1;
    VarSet(VAR_DREAM_HEALS, heals);
    if (heals >= DREAM_AFTER_HEALS)
        gSpecialVar_Result = TRUE;
}

//  YES: the dream goes into gStringVar4 for the script to show. The first time in a stretch, the image; after that,
//  the plain one. She will not offer again for another three heals.
void DaemonsDreamTell(void)
{
    s16 step = NextStep();
    u16 progress = VarGet(VAR_DREAM_PROGRESS);

    if (step < 0)
    {
        gStringVar4[0] = EOS;
        return;
    }
    StringExpandPlaceholders(gStringVar4, (progress & DREAM_TOLD_FIRST) ? sAttendantDreams[step].second
                                                                         : sAttendantDreams[step].first);
    VarSet(VAR_DREAM_PROGRESS, progress | DREAM_TOLD_FIRST);
    VarSet(VAR_DREAM_HEALS, 0);
}

//  NO: no nagging -- she waits another three heals before asking again.
void DaemonsDreamDeclined(void)
{
    VarSet(VAR_DREAM_HEALS, 0);
}
