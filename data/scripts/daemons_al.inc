@  T-487 (the user, 2026-10-29, by multiple choice: "a plateau, with one crack"; bible 4.3, v11.326). After the REVIEW
@  BOARD AL stays the incumbent, unchanged -- no arc, no apology, no rescue (4.34). Once, late, he nearly notices, and
@  explains it away in his own terms: the first time he speaks to the player after the GLOBAL INDEX is handed over
@  (FLAG_SYS_NATIONAL_DEX). The build has him in three places then, so all three call this at the end of what he says
@  -- FOUR ISLAND's harbour, SIX ISLAND's CHECKPOINT, and the champion's room on every visit after the first -- and
@  whichever comes first says it. Once only (FLAG_AL_CRACKED); afterwards his ordinary lines. DRAFT, for the user.
EventScript_AlsCrack::
	goto_if_unset FLAG_SYS_NATIONAL_DEX, EventScript_AlsCrackNotYet
	goto_if_set FLAG_AL_CRACKED, EventScript_AlsCrackNotYet
	msgbox Text_AlsCrack
	setflag FLAG_AL_CRACKED
EventScript_AlsCrackNotYet::
	return

@  DRAFT (widest 154 of 208px as AL; 184 with a seven-letter name of the widest letters)
Text_AlsCrack::
    .string "{RIVAL}: The INDEX had you down\n"
    .string "as a long shot.\p"
    .string "...It was wrong about that.\n"
    .string "Probably a rounding error.$"
