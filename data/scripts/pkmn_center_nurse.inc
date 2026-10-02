EventScript_PkmnCenterNurse::
	goto_if_questlog EventScript_ReleaseEnd
	message Text_WelcomeWantToHealPkmn
	waitmessage
	multichoice 19, 8, MULTICHOICE_YES_NO, FALSE | (TRUE << 1)
	switch VAR_RESULT
	case 0, EventScript_PkmnCenterNurse_HealPkmn
	case 1 EventScript_PkmnCenterNurse_Goodbye
	case SCR_MENU_CANCEL, EventScript_PkmnCenterNurse_Goodbye
	end

EventScript_PkmnCenterNurse_HealPkmn::
	incrementgamestat GAME_STAT_USED_POKECENTER
	message Text_TakeYourPkmnForFewSeconds
	waitmessage
	call EventScript_PkmnCenterNurse_TakeAndHealPkmn
	special SetUsedPkmnCenterQuestLogEvent
	goto EventScript_PkmnCenterNurse_CheckTrainerTowerAndUnionRoom
	end

EventScript_PkmnCenterNurse_TakeAndHealPkmn::
	applymovement VAR_LAST_TALKED, Common_Movement_WalkInPlaceFasterLeft
	waitmovement 0
	dofieldeffect FLDEFF_POKECENTER_HEAL
	waitfieldeffect FLDEFF_POKECENTER_HEAL
	applymovement VAR_LAST_TALKED, Common_Movement_WalkInPlaceFasterDown
	waitmovement 0
	special HealPlayerParty
	return

EventScript_PkmnCenterNurse_CheckTrainerTowerAndUnionRoom::
	specialvar VAR_RESULT, IsPlayerNotInTrainerTowerLobby
	goto_if_eq VAR_RESULT, FALSE, EventScript_PkmnCenterNurse_ReturnPkmn
	specialvar VAR_RESULT, BufferUnionRoomPlayerName
	copyvar VAR_0x8008, VAR_RESULT
	goto_if_eq VAR_0x8008, 0, EventScript_PkmnCenterNurse_ReturnPkmn
	goto_if_eq VAR_0x8008, 1, EventScript_PkmnCenterNurse_PlayerWaitingInUnionRoom
	end

EventScript_PkmnCenterNurse_ReturnPkmn::
	message Text_RestoredPkmnToFullHealth
	waitmessage
	applymovement VAR_LAST_TALKED, Movement_Bow
	waitmovement 0
	call EventScript_PkmnCenterNurse_CheckMeme
	call EventScript_PkmnCenterNurse_CheckDream
	msgbox Text_WeHopeToSeeYouAgain
	return

EventScript_PkmnCenterNurse_PlayerWaitingInUnionRoom::
	goto_if_set FLAG_SYS_INFORMED_OF_LOCAL_WIRELESS_PLAYER, EventScript_PkmnCenterNurse_ReturnPkmn
	msgbox Text_RestoredPkmnToFullHealth
	setflag FLAG_SYS_INFORMED_OF_LOCAL_WIRELESS_PLAYER
	message Text_PlayerIsPlayingRightNowGoForIt
	waitmessage
	applymovement VAR_LAST_TALKED, Movement_Bow
	waitmovement 0
	msgbox Text_WeHopeToSeeYouAgain
	return

EventScript_PkmnCenterNurse_Goodbye::
	msgbox Text_WeHopeToSeeYouAgain
	return

Movement_Bow::
	nurse_joy_bow
	delay_4
	step_end

@ T-258: a MEME is caught from another daemon and passed on, the way an idea is. Said once. DRAFT wording.
EventScript_PkmnCenterNurse_CheckMeme::
	goto_if_set FLAG_MEME_EXPLAINED, EventScript_PkmnCenterNurse_CheckMemeEnd
	specialvar VAR_RESULT, IsPokerusInParty
	goto_if_eq VAR_RESULT, FALSE, EventScript_PkmnCenterNurse_CheckMemeEnd
	msgbox Text_DaemonCaughtAMeme
	setflag FLAG_MEME_EXPLAINED
EventScript_PkmnCenterNurse_CheckMemeEnd::
	return

@ T-347: after three heals with no main-path progress, she offers a dream about the next step (src/daemons_dreams.c,
@ the words from tools/gbadreams.py). YES tells it; NO waits another three heals. DRAFT wording.
EventScript_PkmnCenterNurse_CheckDream::
	special DaemonsDreamCountHeal
	goto_if_eq VAR_RESULT, FALSE, EventScript_PkmnCenterNurse_CheckDreamEnd
	msgbox Text_AttendantHadADream, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, EventScript_PkmnCenterNurse_DreamDeclined
	special DaemonsDreamTell
	msgbox gStringVar4
	msgbox Text_AttendantDreamAfter
	return
EventScript_PkmnCenterNurse_DreamDeclined::
	special DaemonsDreamDeclined
	msgbox Text_AttendantDreamKeeps
EventScript_PkmnCenterNurse_CheckDreamEnd::
	return
