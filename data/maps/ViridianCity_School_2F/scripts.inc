@ CALLOW SCHOOL, 2F -- ATTENTION (T-219, batch 4; docs/school.md 3-4). The same five objects as 1F: the label
@ (map_name_popup.c), the PLACARD, the DESK, the BOARD, and three people.

ViridianCity_School_2F_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, ViridianCity_School_2F_OnTransition
	.byte 0

ViridianCity_School_2F_OnTransition::
	setflag FLAG_SCHOOL_VISITED_2F
	end

ViridianCity_School_2F_EventScript_Teacher::
	msgbox ViridianCity_School_2F_Text_Teacher, MSGBOX_NPC
	end

ViridianCity_School_2F_EventScript_Wall::
	msgbox ViridianCity_School_2F_Text_Wall, MSGBOX_NPC
	end

ViridianCity_School_2F_EventScript_Windows::
	msgbox ViridianCity_School_2F_Text_Windows, MSGBOX_NPC
	end

ViridianCity_School_2F_EventScript_Placard::
	msgbox ViridianCity_School_2F_Text_Placard, MSGBOX_SIGN
	end

ViridianCity_School_2F_EventScript_Syllabus::
	lockall
	msgbox ViridianCity_School_2F_Text_Syllabus
	goto_if_set FLAG_SCHOOL_SYLLABUS_ATTENTION, ViridianCity_School_2F_EventScript_SyllabusHeld
	setflag FLAG_SCHOOL_SYLLABUS_ATTENTION
	playse SE_SELECT
	msgbox ViridianCity_School_Text_TookSyllabus
	goto_if_unset FLAG_GOT_NOTEBOOK, ViridianCity_School_2F_EventScript_SyllabusHeld
	msgbox ViridianCity_School_Text_SyllabusIntoNotebook
ViridianCity_School_2F_EventScript_SyllabusHeld::
	releaseall
	end

ViridianCity_School_2F_EventScript_Board::
	lockall
	msgbox ViridianCity_School_2F_Text_BoardIntro
ViridianCity_School_2F_EventScript_ChooseTopic::
	message ViridianCity_School_Text_ReadWhichTopic
	waitmessage
	multichoice 19, 0, MULTICHOICE_SCHOOL_ATTENTION, FALSE
	switch VAR_RESULT
	case 0, ViridianCity_School_2F_EventScript_ReadLimits
	case 1, ViridianCity_School_2F_EventScript_ReadCues
	case 2, ViridianCity_School_2F_EventScript_ReadSearch
	case 3, ViridianCity_School_2F_EventScript_ReadSwitching
	case 4, ViridianCity_School_2F_EventScript_ReadMissed
	case 5, ViridianCity_School_2F_EventScript_ExitBoard
	case 127, ViridianCity_School_2F_EventScript_ExitBoard
	end

ViridianCity_School_2F_EventScript_ReadLimits::
	msgbox ViridianCity_School_2F_Text_Limits
	goto ViridianCity_School_2F_EventScript_ChooseTopic
	end

ViridianCity_School_2F_EventScript_ReadCues::
	msgbox ViridianCity_School_2F_Text_Cues
	goto ViridianCity_School_2F_EventScript_ChooseTopic
	end

ViridianCity_School_2F_EventScript_ReadSearch::
	msgbox ViridianCity_School_2F_Text_Search
	goto ViridianCity_School_2F_EventScript_ChooseTopic
	end

ViridianCity_School_2F_EventScript_ReadSwitching::
	msgbox ViridianCity_School_2F_Text_Switching
	goto ViridianCity_School_2F_EventScript_ChooseTopic
	end

ViridianCity_School_2F_EventScript_ReadMissed::
	msgbox ViridianCity_School_2F_Text_Missed
	goto ViridianCity_School_2F_EventScript_ChooseTopic
	end

ViridianCity_School_2F_EventScript_ExitBoard::
	releaseall
	end

ViridianCity_School_2F_EventScript_Lift::
	setvar VAR_0x8004, 2
	goto School_EventScript_Lift
	end
