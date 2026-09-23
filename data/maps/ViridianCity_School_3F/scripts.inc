@ CALLOW SCHOOL, 3F -- MEMORY (T-219, batch 4; docs/school.md 3-4). The same five objects as 1F: the label
@ (map_name_popup.c), the PLACARD, the DESK, the BOARD, and three people.

ViridianCity_School_3F_MapScripts::
	.byte 0

ViridianCity_School_3F_EventScript_Teacher::
	msgbox ViridianCity_School_3F_Text_Teacher, MSGBOX_NPC
	end

ViridianCity_School_3F_EventScript_Eleventh::
	msgbox ViridianCity_School_3F_Text_Eleventh, MSGBOX_NPC
	end

ViridianCity_School_3F_EventScript_Friday::
	msgbox ViridianCity_School_3F_Text_Friday, MSGBOX_NPC
	end

ViridianCity_School_3F_EventScript_Placard::
	msgbox ViridianCity_School_3F_Text_Placard, MSGBOX_SIGN
	end

ViridianCity_School_3F_EventScript_Syllabus::
	lockall
	msgbox ViridianCity_School_3F_Text_Syllabus
	goto_if_set FLAG_SCHOOL_SYLLABUS_MEMORY, ViridianCity_School_3F_EventScript_SyllabusHeld
	setflag FLAG_SCHOOL_SYLLABUS_MEMORY
	playse SE_SELECT
	msgbox ViridianCity_School_Text_TookSyllabus
	goto_if_unset FLAG_GOT_NOTEBOOK, ViridianCity_School_3F_EventScript_SyllabusHeld
	msgbox ViridianCity_School_Text_SyllabusIntoNotebook
ViridianCity_School_3F_EventScript_SyllabusHeld::
	releaseall
	end

ViridianCity_School_3F_EventScript_Board::
	lockall
	msgbox ViridianCity_School_3F_Text_BoardIntro
ViridianCity_School_3F_EventScript_ChooseTopic::
	message ViridianCity_School_Text_ReadWhichTopic
	waitmessage
	multichoice 19, 0, MULTICHOICE_SCHOOL_MEMORY, FALSE
	switch VAR_RESULT
	case 0, ViridianCity_School_3F_EventScript_ReadStoring
	case 1, ViridianCity_School_3F_EventScript_ReadFading
	case 2, ViridianCity_School_3F_EventScript_ReadCrowding
	case 3, ViridianCity_School_3F_EventScript_ReadRecall
	case 4, ViridianCity_School_3F_EventScript_ReadSpacing
	case 5, ViridianCity_School_3F_EventScript_ExitBoard
	case 127, ViridianCity_School_3F_EventScript_ExitBoard
	end

ViridianCity_School_3F_EventScript_ReadStoring::
	msgbox ViridianCity_School_3F_Text_Storing
	goto ViridianCity_School_3F_EventScript_ChooseTopic
	end

ViridianCity_School_3F_EventScript_ReadFading::
	msgbox ViridianCity_School_3F_Text_Fading
	goto ViridianCity_School_3F_EventScript_ChooseTopic
	end

ViridianCity_School_3F_EventScript_ReadCrowding::
	msgbox ViridianCity_School_3F_Text_Crowding
	goto ViridianCity_School_3F_EventScript_ChooseTopic
	end

ViridianCity_School_3F_EventScript_ReadRecall::
	msgbox ViridianCity_School_3F_Text_Recall
	goto ViridianCity_School_3F_EventScript_ChooseTopic
	end

ViridianCity_School_3F_EventScript_ReadSpacing::
	msgbox ViridianCity_School_3F_Text_Spacing
	goto ViridianCity_School_3F_EventScript_ChooseTopic
	end

ViridianCity_School_3F_EventScript_ExitBoard::
	releaseall
	end
