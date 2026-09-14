CeruleanCave_B1F_MapScripts::
	map_script MAP_SCRIPT_ON_RESUME, CeruleanCave_B1F_OnResume
	map_script MAP_SCRIPT_ON_TRANSITION, CeruleanCave_B1F_OnTransition
	.byte 0

CeruleanCave_B1F_OnResume::
	call_if_set FLAG_SYS_SPECIAL_WILD_BATTLE, CeruleanCave_B1F_EventScript_TryRemoveMewtwo
	end

CeruleanCave_B1F_EventScript_TryRemoveMewtwo::
	specialvar VAR_RESULT, GetBattleOutcome
	goto_if_ne VAR_RESULT, B_OUTCOME_CAUGHT, EventScript_Return
	removeobject VAR_LAST_TALKED
	return

CeruleanCave_B1F_OnTransition::
	call_if_unset FLAG_FOUGHT_MEWTWO, CeruleanCave_B1F_EventScript_ShowMewtwo
	end

CeruleanCave_B1F_EventScript_ShowMewtwo::
	clearflag FLAG_HIDE_MEWTWO
	return

CeruleanCave_B1F_EventScript_Mewtwo::
	goto_if_questlog EventScript_ReleaseEnd
	special QuestLog_CutRecording
	lock
	faceplayer
	waitse
	playmoncry SPECIES_MEWTWO, CRY_MODE_ENCOUNTER
	message CeruleanCave_B1F_Text_Mew
	waitmessage
	waitmoncry
	delay 20
	playbgm MUS_ENCOUNTER_GYM_LEADER, 0
	waitbuttonpress
	setwildbattle SPECIES_MEWTWO, 70
	setflag FLAG_SYS_SPECIAL_WILD_BATTLE
	special StartLegendaryBattle
	waitstate
	clearflag FLAG_SYS_SPECIAL_WILD_BATTLE
	specialvar VAR_RESULT, GetBattleOutcome
	goto_if_eq VAR_RESULT, B_OUTCOME_CAUGHT, CeruleanCave_B1F_EventScript_CaughtStarr
	goto_if_eq VAR_RESULT, B_OUTCOME_WON, CeruleanCave_B1F_EventScript_DefeatedMewtwo
	goto_if_eq VAR_RESULT, B_OUTCOME_RAN, CeruleanCave_B1F_EventScript_RanFromMewtwo
	goto_if_eq VAR_RESULT, B_OUTCOME_PLAYER_TELEPORTED, CeruleanCave_B1F_EventScript_RanFromMewtwo
	setflag FLAG_FOUGHT_MEWTWO
	release
	end

@  4.34: it went to DOLDRUM CAVE "and waits for someone who can get through
@  both gates" -- and then the player had no way to know where it wanted to
@  go. So it says, once, the moment it is caught. The words are its own
@  terminal's register, and they name a place and a person and nothing more:
@  never what the argument is, only that one reader has not decided.
CeruleanCave_B1F_EventScript_CaughtStarr::
	setflag FLAG_FOUGHT_MEWTWO
	msgbox CeruleanCave_B1F_Text_StarrLooksAtYou
	msgbox CeruleanCave_B1F_Text_StarrOneReader
	msgbox CeruleanCave_B1F_Text_StarrTakeMe
	release
	end

CeruleanCave_B1F_EventScript_DefeatedMewtwo::
	setflag FLAG_FOUGHT_MEWTWO
	goto EventScript_RemoveStaticMon
	end

CeruleanCave_B1F_EventScript_RanFromMewtwo::
	setvar VAR_0x8004, SPECIES_MEWTWO
	goto EventScript_MonFlewAway
	end
