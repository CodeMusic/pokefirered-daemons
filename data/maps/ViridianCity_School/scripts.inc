@ CALLOW SCHOOL, 1F -- LANGUAGE (T-215; docs/school.md 3-4).
@ The floor is five objects: the label (map_name_popup.c, on arrival), the PLACARD (the framed notice by the
@ board), the DESK (the syllabus is the right-hand paper; the left one is the girl's own notes), the BOARD
@ (the lesson, and the only thing the exam will draw from), and three people.

ViridianCity_School_MapScripts::
	.byte 0

ViridianCity_School_EventScript_Lass::
	lock
	faceplayer
	msgbox ViridianCity_School_Text_TryingToMemorizeNotes
	closemessage
	applymovement LOCALID_SCHOOL_LASS, Common_Movement_WalkInPlaceFasterUp
	waitmovement 0
	release
	end

ViridianCity_School_EventScript_Woman::
	lock
	faceplayer
	msgbox ViridianCity_School_Text_ReadBlackboardCarefully
	closemessage
	applymovement LOCALID_SCHOOL_WOMAN, Common_Movement_FaceOriginalDirection
	waitmovement 0
	release
	end

@ The one who is wrong in the right direction (craft rule 2).
ViridianCity_School_EventScript_Speller::
	msgbox ViridianCity_School_Text_Spelling, MSGBOX_NPC
	end

ViridianCity_School_EventScript_Placard::
	msgbox ViridianCity_School_Text_Placard, MSGBOX_SIGN
	end

@ The pass mark is on the syllabus, before the exam exists. Taking it sets a flag, not an item: the RESEARCH
@ NOTEBOOK does not exist until CAIRN hands it over, and when it does, this is already in it.
ViridianCity_School_EventScript_Syllabus::
	lockall
	msgbox ViridianCity_School_Text_Syllabus
	goto_if_set FLAG_SCHOOL_SYLLABUS_LANGUAGE, ViridianCity_School_EventScript_SyllabusHeld
	setflag FLAG_SCHOOL_SYLLABUS_LANGUAGE
	playse SE_SELECT
	msgbox ViridianCity_School_Text_TookSyllabus
	goto_if_unset FLAG_GOT_NOTEBOOK, ViridianCity_School_EventScript_SyllabusHeld
	msgbox ViridianCity_School_Text_SyllabusIntoNotebook
ViridianCity_School_EventScript_SyllabusHeld::
	releaseall
	end

ViridianCity_School_EventScript_Notebook::
	lockall
	msgbox ViridianCity_School_Text_NotebookFirstPage
	msgbox ViridianCity_School_Text_TurnThePage, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, ViridianCity_School_EventScript_StopReadingNotebook
	msgbox ViridianCity_School_Text_NotebookSecondPage
	msgbox ViridianCity_School_Text_TurnThePage, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, ViridianCity_School_EventScript_StopReadingNotebook
	msgbox ViridianCity_School_Text_NotebookThirdPage
	msgbox ViridianCity_School_Text_TurnThePage, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, ViridianCity_School_EventScript_StopReadingNotebook
	msgbox ViridianCity_School_Text_NotebookFourthPage
	applymovement LOCALID_SCHOOL_LASS, Common_Movement_WalkInPlaceFasterUp
	waitmovement 0
	textcolor NPC_TEXT_COLOR_FEMALE
	msgbox ViridianCity_School_Text_HeyDontLookAtMyNotes
	releaseall
	end

ViridianCity_School_EventScript_StopReadingNotebook::
	releaseall
	end

ViridianCity_School_EventScript_Blackboard::
	lockall
	msgbox ViridianCity_School_Text_BoardIntro
	goto ViridianCity_School_EventScript_ChooseBlackboardTopic
	end

ViridianCity_School_EventScript_ChooseBlackboardTopic::
	message ViridianCity_School_Text_ReadWhichTopic
	waitmessage
	multichoice 19, 0, MULTICHOICE_SCHOOL_LANGUAGE, FALSE
	switch VAR_RESULT
	case 0, ViridianCity_School_EventScript_ReadNames
	case 1, ViridianCity_School_EventScript_ReadPieces
	case 2, ViridianCity_School_EventScript_ReadOrder
	case 3, ViridianCity_School_EventScript_ReadAmbiguity
	case 4, ViridianCity_School_EventScript_ReadSpelling
	case 5, ViridianCity_School_EventScript_ExitBlackboard
	case 127, ViridianCity_School_EventScript_ExitBlackboard
	end

ViridianCity_School_EventScript_ReadNames::
	msgbox ViridianCity_School_Text_Names
	goto ViridianCity_School_EventScript_ChooseBlackboardTopic
	end

ViridianCity_School_EventScript_ReadPieces::
	msgbox ViridianCity_School_Text_Pieces
	goto ViridianCity_School_EventScript_ChooseBlackboardTopic
	end

ViridianCity_School_EventScript_ReadOrder::
	msgbox ViridianCity_School_Text_Order
	goto ViridianCity_School_EventScript_ChooseBlackboardTopic
	end

ViridianCity_School_EventScript_ReadAmbiguity::
	msgbox ViridianCity_School_Text_Ambiguity
	goto ViridianCity_School_EventScript_ChooseBlackboardTopic
	end

ViridianCity_School_EventScript_ReadSpelling::
	msgbox ViridianCity_School_Text_Spelling_Board
	goto ViridianCity_School_EventScript_ChooseBlackboardTopic
	end

ViridianCity_School_EventScript_ExitBlackboard::
	releaseall
	end
