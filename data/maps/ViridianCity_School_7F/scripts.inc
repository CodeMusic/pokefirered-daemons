@ CALLOW SCHOOL, 7F -- ERROR (T-219, batch 5; docs/school.md 3-4).

ViridianCity_School_7F_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, ViridianCity_School_7F_OnTransition
	.byte 0

ViridianCity_School_7F_OnTransition::
	setflag FLAG_SCHOOL_VISITED_7F
	end

@ The top floor's teacher is the marker delighted by a wrong answer, and she hands over the TEXTBOOK.
@ Taking it arms exam season: the next time the player leaves by the ground floor's door, it begins.
ViridianCity_School_7F_EventScript_Teacher::
	lock
	faceplayer
	goto_if_set FLAG_SCHOOL_GOT_TEXTBOOK, ViridianCity_School_7F_EventScript_TeacherJoke
	msgbox ViridianCity_School_7F_Text_TeacherGive
	giveitem_msg ViridianCity_School_7F_Text_ReceivedTextbook, ITEM_TEXTBOOK
	setflag FLAG_SCHOOL_GOT_TEXTBOOK
	setvar VAR_SCHOOL_SEASON, 1
	release
	end
ViridianCity_School_7F_EventScript_TeacherJoke::
	msgbox ViridianCity_School_7F_Text_TeacherJoke
	release
	end

ViridianCity_School_7F_EventScript_Onpurpose::
	msgbox ViridianCity_School_7F_Text_Onpurpose, MSGBOX_NPC
	end

ViridianCity_School_7F_EventScript_Certain::
	msgbox ViridianCity_School_7F_Text_Certain, MSGBOX_NPC
	end

ViridianCity_School_7F_EventScript_Lift::
	setvar VAR_0x8004, 7
	goto School_EventScript_Lift
	end

ViridianCity_School_7F_EventScript_Placard::
	msgbox ViridianCity_School_7F_Text_Placard, MSGBOX_SIGN
	end

ViridianCity_School_7F_EventScript_Syllabus::
	lockall
	msgbox ViridianCity_School_7F_Text_Syllabus
	goto_if_set FLAG_SCHOOL_SYLLABUS_ERROR, ViridianCity_School_7F_EventScript_SyllabusHeld
	setflag FLAG_SCHOOL_SYLLABUS_ERROR
	playse SE_SELECT
	msgbox ViridianCity_School_Text_TookSyllabus
	goto_if_unset FLAG_GOT_NOTEBOOK, ViridianCity_School_7F_EventScript_SyllabusHeld
	msgbox ViridianCity_School_Text_SyllabusIntoNotebook
ViridianCity_School_7F_EventScript_SyllabusHeld::
	releaseall
	end

ViridianCity_School_7F_EventScript_Board::
	lockall
	msgbox ViridianCity_School_2F_Text_BoardIntro
ViridianCity_School_7F_EventScript_ChooseTopic::
	message ViridianCity_School_Text_ReadWhichTopic
	waitmessage
	multichoice 19, 0, MULTICHOICE_SCHOOL_ERROR, FALSE
	switch VAR_RESULT
	case 0, ViridianCity_School_7F_EventScript_ReadSurprise
	case 1, ViridianCity_School_7F_EventScript_ReadCorrection
	case 2, ViridianCity_School_7F_EventScript_ReadSteps
	case 3, ViridianCity_School_7F_EventScript_ReadMistakes
	case 4, ViridianCity_School_7F_EventScript_ReadCertainty
	case 5, ViridianCity_School_7F_EventScript_ExitBoard
	case 127, ViridianCity_School_7F_EventScript_ExitBoard
	end

ViridianCity_School_7F_EventScript_ReadSurprise::
	msgbox ViridianCity_School_7F_Text_Surprise
	goto ViridianCity_School_7F_EventScript_ChooseTopic
	end

ViridianCity_School_7F_EventScript_ReadCorrection::
	msgbox ViridianCity_School_7F_Text_Correction
	goto ViridianCity_School_7F_EventScript_ChooseTopic
	end

ViridianCity_School_7F_EventScript_ReadSteps::
	msgbox ViridianCity_School_7F_Text_Steps
	goto ViridianCity_School_7F_EventScript_ChooseTopic
	end

ViridianCity_School_7F_EventScript_ReadMistakes::
	msgbox ViridianCity_School_7F_Text_Mistakes
	goto ViridianCity_School_7F_EventScript_ChooseTopic
	end

ViridianCity_School_7F_EventScript_ReadCertainty::
	msgbox ViridianCity_School_7F_Text_Certainty
	goto ViridianCity_School_7F_EventScript_ChooseTopic
	end

ViridianCity_School_7F_EventScript_ExitBoard::
	releaseall
	end
