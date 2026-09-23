@ T-219 and T-217: CALLOW SCHOOL, shared across its seven floors (docs/school.md 3-7).
@   THE LIFT        every floor, in the right-hand window. Each floor sets VAR_0x8004 to its number first.
@   EXAM SEASON     the TEXTBOOK arms it (VAR_SCHOOL_SEASON 1); leaving by the ground floor's door starts it (2).
@   THE EXAM DESK   in season, 1F's desk holds the paper, the class is seated, and the Owl is at the front.
@ DRAFT WORDING throughout, awaiting the user's approval.

School_EventScript_Lift::
	lockall
	goto_if_eq VAR_SCHOOL_SEASON, 2, School_EventScript_TakeYourSeatHere
	message School_Text_LiftWhichFloor
	waitmessage
	multichoice 17, 0, MULTICHOICE_SCHOOL_LIFT, FALSE
	goto_if_eq VAR_RESULT, 7, School_EventScript_LiftClose
	goto_if_eq VAR_RESULT, 127, School_EventScript_LiftClose
	addvar VAR_RESULT, 1
	compare VAR_RESULT, VAR_0x8004
	goto_if_eq School_EventScript_LiftHere
	closemessage
	playse SE_ELEVATOR
	waitse
	switch VAR_RESULT
	case 1, School_EventScript_LiftTo1F
	case 2, School_EventScript_LiftTo2F
	case 3, School_EventScript_LiftTo3F
	case 4, School_EventScript_LiftTo4F
	case 5, School_EventScript_LiftTo5F
	case 6, School_EventScript_LiftTo6F
	case 7, School_EventScript_LiftTo7F
	end

School_EventScript_LiftHere::
	msgbox School_Text_LiftAlreadyHere
School_EventScript_LiftClose::
	closemessage
	releaseall
	end

@ Every floor's lift opens onto (2,2), facing the doors it came out of.
School_EventScript_LiftTo1F::
	warp MAP_VIRIDIAN_CITY_SCHOOL, 2, 2
	waitstate
	end
School_EventScript_LiftTo2F::
	warp MAP_VIRIDIAN_CITY_SCHOOL_2F, 2, 2
	waitstate
	end
School_EventScript_LiftTo3F::
	warp MAP_VIRIDIAN_CITY_SCHOOL_3F, 2, 2
	waitstate
	end
School_EventScript_LiftTo4F::
	warp MAP_VIRIDIAN_CITY_SCHOOL_4F, 2, 2
	waitstate
	end
School_EventScript_LiftTo5F::
	warp MAP_VIRIDIAN_CITY_SCHOOL_5F, 2, 2
	waitstate
	end
School_EventScript_LiftTo6F::
	warp MAP_VIRIDIAN_CITY_SCHOOL_6F, 2, 2
	waitstate
	end
School_EventScript_LiftTo7F::
	warp MAP_VIRIDIAN_CITY_SCHOOL_7F, 2, 2
	waitstate
	end

@ Exam season begins on the way out: the player does not answer, and keeps walking.
School_EventScript_ExamSeasonBegins::
	lockall
	msgbox School_Text_OverheardExamSeason
	closemessage
	setvar VAR_SCHOOL_SEASON, 2
	warp MAP_VIRIDIAN_CITY, 3
	waitstate
	releaseall
	end

@ In season the stairs and the lift are refused -- but the door never is.
School_EventScript_TakeYourSeat::
	lockall
	msgbox School_Text_TakeYourSeat
	closemessage
	applymovement OBJ_EVENT_ID_PLAYER, School_Movement_StepBack
	waitmovement 0
	releaseall
	end

School_EventScript_TakeYourSeatHere::
	msgbox School_Text_TakeYourSeat
	closemessage
	releaseall
	end

School_Movement_StepBack:
	walk_down
	step_end

School_EventScript_ExamDesk::
	lockall
	special School_ExamInProgress
	goto_if_eq VAR_RESULT, TRUE, School_EventScript_ExamResume
	msgbox School_Text_ExamOnDesk, MSGBOX_YESNO
	goto School_EventScript_ExamDecide
School_EventScript_ExamResume::
	msgbox School_Text_ExamResume, MSGBOX_YESNO
School_EventScript_ExamDecide::
	goto_if_eq VAR_RESULT, NO, School_EventScript_ExamNotYet
	closemessage
	fadescreen FADE_TO_BLACK
	special School_OpenExam
	waitstate
	goto_if_eq VAR_RESULT, TRUE, School_EventScript_ExamMarked
	msgbox School_Text_ExamKeeps
	releaseall
	end

School_EventScript_ExamMarked::
	special School_BufferExamResult
	msgbox School_Text_ExamMarked
	goto_if_eq VAR_RESULT, TRUE, School_EventScript_ExamPassed
	msgbox School_Text_ExamNotYetPassed
	releaseall
	end

@ T-218: at 80 or better, the Owl signs a DIPLOMA without looking up, and the season is over (VAR 3): the
@ class goes back to its usual three the next time the room is entered.
School_EventScript_ExamPassed::
	msgbox School_Text_ExamPassed
	goto_if_set FLAG_GOT_DIPLOMA, School_EventScript_ExamDone
	msgbox School_Text_OwlSigns
	giveitem_msg School_Text_ReceivedDiploma, ITEM_DIPLOMA
	setflag FLAG_GOT_DIPLOMA
	setvar VAR_SCHOOL_SEASON, 3
	closemessage
	fadescreen FADE_TO_BLACK
	special School_ShowDiploma
	waitstate
School_EventScript_ExamDone::
	releaseall
	end

@ Using the DIPLOMA from the bag.
EventScript_Diploma::
	lockall
	fadescreen FADE_TO_BLACK
	special School_ShowDiploma
	waitstate
	releaseall
	end

School_EventScript_ExamNotYet::
	msgbox School_Text_ExamWhenever
	releaseall
	end

School_EventScript_Owl::
	msgbox School_Text_Owl, MSGBOX_NPC
	end

School_EventScript_SeatedStudent::
	msgbox School_Text_Shh, MSGBOX_NPC
	end

School_Text_LiftWhichFloor::
	.string "Which floor?$"

School_Text_LiftAlreadyHere::
	.string "This is that floor.$"

School_Text_OverheardExamSeason::
	.string "Behind you, the teacher's voice\n"
	.string "carries down the stairwell:\p"
	.string "“Exam season, everyone.\p"
	.string "When you are ready, come back and\n"
	.string "take your seat. The paper will be\l"
	.string "on your desk.”$"

School_Text_TakeYourSeat::
	.string "Take your seat, please.$"

School_Text_ExamOnDesk::
	.string "The paper is on the desk,\n"
	.string "face down.\p"
	.string "Begin?$"

School_Text_ExamResume::
	.string "Your paper, where you left it.\n"
	.string "Carry on?$"

School_Text_ExamKeeps::
	.string "The paper keeps. So does the seat.$"

School_Text_ExamMarked::
	.string "It is marked at the front, without\n"
	.string "a word.\p"
	.string "{STR_VAR_1} out of 100.\n"
	.string "{STR_VAR_2}.$"

School_Text_ExamPassed::
	.string "A pass.$"

School_Text_OwlSigns::
	.string "At the front, the OWL signs a\n"
	.string "DIPLOMA without looking up, and\l"
	.string "slides it across the desk.$"

School_Text_ReceivedDiploma::
	.string "{PLAYER} received a DIPLOMA.$"

School_Text_ExamNotYetPassed::
	.string "The pass mark is 80.\p"
	.string "Every board is in the TEXTBOOK.\n"
	.string "Sit it again whenever you like.$"

School_Text_ExamWhenever::
	.string "Whenever you are ready.$"

School_Text_Owl::
	.string "Take all the time you need.\n"
	.string "The paper is not going anywhere.$"

School_Text_Shh::
	.string "...Shh.$"
