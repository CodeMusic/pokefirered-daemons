@ CALLOW SCHOOL, 4F -- CATEGORIES (T-219, batch 4; docs/school.md 3-4). The same five objects as 1F: the label
@ (map_name_popup.c), the PLACARD, the DESK, the BOARD, and three people.

ViridianCity_School_4F_MapScripts::
	.byte 0

ViridianCity_School_4F_EventScript_Teacher::
	msgbox ViridianCity_School_4F_Text_Teacher, MSGBOX_NPC
	end

ViridianCity_School_4F_EventScript_Drawers::
	msgbox ViridianCity_School_4F_Text_Drawers, MSGBOX_NPC
	end

ViridianCity_School_4F_EventScript_Stool::
	msgbox ViridianCity_School_4F_Text_Stool, MSGBOX_NPC
	end

ViridianCity_School_4F_EventScript_Placard::
	msgbox ViridianCity_School_4F_Text_Placard, MSGBOX_SIGN
	end

ViridianCity_School_4F_EventScript_Syllabus::
	lockall
	msgbox ViridianCity_School_4F_Text_Syllabus
	goto_if_set FLAG_SCHOOL_SYLLABUS_CATEGORIES, ViridianCity_School_4F_EventScript_SyllabusHeld
	setflag FLAG_SCHOOL_SYLLABUS_CATEGORIES
	playse SE_SELECT
	msgbox ViridianCity_School_Text_TookSyllabus
	goto_if_unset FLAG_GOT_NOTEBOOK, ViridianCity_School_4F_EventScript_SyllabusHeld
	msgbox ViridianCity_School_Text_SyllabusIntoNotebook
ViridianCity_School_4F_EventScript_SyllabusHeld::
	releaseall
	end

ViridianCity_School_4F_EventScript_Board::
	lockall
	msgbox ViridianCity_School_4F_Text_BoardIntro
ViridianCity_School_4F_EventScript_ChooseTopic::
	message ViridianCity_School_Text_ReadWhichTopic
	waitmessage
	multichoice 19, 0, MULTICHOICE_SCHOOL_CATEGORIES, FALSE
	switch VAR_RESULT
	case 0, ViridianCity_School_4F_EventScript_ReadMiddles
	case 1, ViridianCity_School_4F_EventScript_ReadEdges
	case 2, ViridianCity_School_4F_EventScript_ReadFeatures
	case 3, ViridianCity_School_4F_EventScript_ReadLevels
	case 4, ViridianCity_School_4F_EventScript_ReadOther
	case 5, ViridianCity_School_4F_EventScript_ExitBoard
	case 127, ViridianCity_School_4F_EventScript_ExitBoard
	end

ViridianCity_School_4F_EventScript_ReadMiddles::
	msgbox ViridianCity_School_4F_Text_Middles
	goto ViridianCity_School_4F_EventScript_ChooseTopic
	end

ViridianCity_School_4F_EventScript_ReadEdges::
	msgbox ViridianCity_School_4F_Text_Edges
	goto ViridianCity_School_4F_EventScript_ChooseTopic
	end

ViridianCity_School_4F_EventScript_ReadFeatures::
	msgbox ViridianCity_School_4F_Text_Features
	goto ViridianCity_School_4F_EventScript_ChooseTopic
	end

ViridianCity_School_4F_EventScript_ReadLevels::
	msgbox ViridianCity_School_4F_Text_Levels
	goto ViridianCity_School_4F_EventScript_ChooseTopic
	end

ViridianCity_School_4F_EventScript_ReadOther::
	msgbox ViridianCity_School_4F_Text_Other
	goto ViridianCity_School_4F_EventScript_ChooseTopic
	end

ViridianCity_School_4F_EventScript_ExitBoard::
	releaseall
	end
