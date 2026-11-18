SSAnne_CaptainsOffice_MapScripts::
	.byte 0

@ T-586 (the user, 2026-11-17; every word DRAFT): rubbed, DAVID goes back to his book, and the player says what to do.
@ KEEP READING: still seasick, and the ship waits -- he can be asked again. LOOK OUT THE PORTHOLE: he steadies, and
@ gives PRUNE, the band's sheet and his COMPASS; only then is she set to sail. FLAG_TEMP_1: rubbed already this visit.
SSAnne_CaptainsOffice_EventScript_Captain::
	lock
	goto_if_set FLAG_GOT_HM01, SSAnne_CaptainsOffice_EventScript_AlreadyGotCut
	goto_if_set FLAG_TEMP_1, SSAnne_CaptainsOffice_EventScript_StillAtIt
	msgbox SSAnne_CaptainsOffice_Text_CaptainIFeelSeasick
	textcolor NPC_TEXT_COLOR_NEUTRAL
	message SSAnne_CaptainsOffice_Text_RubbedCaptainsBack
	waitmessage
	playfanfare MUS_HEAL
	waitfanfare
	call EventScript_RestorePrevTextColor
	setflag FLAG_TEMP_1
	delay 50
	msgbox SSAnne_CaptainsOffice_Text_HelpsABit
	textcolor NPC_TEXT_COLOR_NEUTRAL
	msgbox SSAnne_CaptainsOffice_Text_BackToTheBook
	call EventScript_RestorePrevTextColor
	goto SSAnne_CaptainsOffice_EventScript_Advice

SSAnne_CaptainsOffice_EventScript_StillAtIt::
	msgbox SSAnne_CaptainsOffice_Text_StillAtIt
SSAnne_CaptainsOffice_EventScript_Advice::
	message SSAnne_CaptainsOffice_Text_EveryStep
	waitmessage
	multichoice 9, 8, MULTICHOICE_DAVID_ADVICE, FALSE
	goto_if_eq VAR_RESULT, 1, SSAnne_CaptainsOffice_EventScript_Porthole
	msgbox SSAnne_CaptainsOffice_Text_KeepReading
	release
	end

SSAnne_CaptainsOffice_EventScript_Porthole::
	textcolor NPC_TEXT_COLOR_NEUTRAL
	msgbox SSAnne_CaptainsOffice_Text_LooksOut
	call EventScript_RestorePrevTextColor
	applymovement LOCALID_SS_ANNE_CAPTAIN, Common_Movement_FacePlayer
	waitmovement 0
	msgbox SSAnne_CaptainsOffice_Text_ThereTheWholeTime
	msgbox SSAnne_CaptainsOffice_Text_ThankYouHaveHMForCut
	giveitem_msg SSAnne_CaptainsOffice_Text_ObtainedHM01FromCaptain, ITEM_HM01, 1, MUS_OBTAIN_KEY_ITEM
	msgbox SSAnne_CaptainsOffice_Text_ExplainCut
	setflag FLAG_GOT_HM01
	msgbox SSAnne_CaptainsOffice_Text_BandsSheet
	setflag FLAG_NOTEBOOK_LOOSE_KEY
	msgbox SSAnne_CaptainsOffice_Text_SheetIntoNotebook
	msgbox SSAnne_CaptainsOffice_Text_Compass
	giveitem_msg SSAnne_CaptainsOffice_Text_ObtainedCompass, ITEM_COMPASS, 1, MUS_OBTAIN_KEY_ITEM
	setvar VAR_MAP_SCENE_VERMILION_CITY, 1
	release
	end

@ Unused
SSAnne_CaptainsOffice_EventScript_NoRoomForCut::
	msgbox SSAnne_CaptainsOffice_Text_YouHaveNoRoomForThis
	closemessage
	applymovement LOCALID_SS_ANNE_CAPTAIN, Common_Movement_FaceOriginalDirection
	waitmovement 0
	release
	end

SSAnne_CaptainsOffice_EventScript_AlreadyGotCut::
	applymovement LOCALID_SS_ANNE_CAPTAIN, Common_Movement_FacePlayer
	waitmovement 0
	msgbox SSAnne_CaptainsOffice_Text_SSAnneWillSetSailSoon
	release
	end

SSAnne_CaptainsOffice_EventScript_TrashCan::
	msgbox SSAnne_CaptainsOffice_Text_YuckShouldntHaveLooked, MSGBOX_SIGN
	end

SSAnne_CaptainsOffice_EventScript_Book::
	msgbox SSAnne_CaptainsOffice_Text_HowToConquerSeasickness, MSGBOX_SIGN
	end
