LavenderTown_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, LavenderTown_OnTransition
	.byte 0

LavenderTown_OnTransition::
	setworldmapflag FLAG_WORLD_MAP_LAVENDER_TOWN
	setvar VAR_MAP_SCENE_S_S_ANNE_2F_CORRIDOR, 1
	setflag FLAG_HIDE_SS_ANNE_RIVAL
	end

LavenderTown_EventScript_LittleGirl::
	lock
	faceplayer
	msgbox LavenderTown_Text_DoYouBelieveInGhosts, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, YES, LavenderTown_EventScript_LittleGirlBelieve
	msgbox LavenderTown_Text_JustImaginingWhiteHand
	release
	end

LavenderTown_EventScript_LittleGirlBelieve::
	msgbox LavenderTown_Text_SoThereAreBelievers
	release
	end

LavenderTown_EventScript_WorkerM::
	msgbox LavenderTown_Text_TownKnownAsMonGraveSite, MSGBOX_NPC
	end

LavenderTown_EventScript_Boy::
	msgbox LavenderTown_Text_GhostsAppearedInTower, MSGBOX_NPC
	end

LavenderTown_EventScript_TownSign::
	msgbox LavenderTown_Text_TownSign, MSGBOX_SIGN
	end

LavenderTown_EventScript_SilphScopeNotice::
	msgbox LavenderTown_Text_SilphScopeNotice, MSGBOX_SIGN
	end

LavenderTown_EventScript_VolunteerHouseSign::
	msgbox LavenderTown_Text_VolunteerPokemonHouse, MSGBOX_SIGN
	end

LavenderTown_EventScript_PokemonTowerSign::
	msgbox LavenderTown_Text_PokemonTowerSign, MSGBOX_SIGN
	end

@  T-439 (the user, 2026-10-22; DAEMONS docs/private/plan-cast-2026-10-22.md, tools/gencast.py): the KINGFISHER, a
@  photographer -- the bluest bird there is, in the one town that shows no blue. Since T-430 what you bring keeps its
@  colour here, and he notices, once: the photograph (the follower's own picture, framed) comes out in colour, and it
@  is filed in the NOTEBOOK. Every word DRAFT.
LavenderTown_EventScript_Kingfisher::
	lock
	faceplayer
	goto_if_set FLAG_NOTEBOOK_LOOSE_PHOTO, LavenderTown_EventScript_KingfisherAgain
	special DaemonsCast_FollowerSpecies
	goto_if_eq VAR_RESULT, FALSE, LavenderTown_EventScript_KingfisherNothing
	msgbox LavenderTown_Text_KingfisherHoldStill
	closemessage
	playse SE_BALL_OPEN
	showmonpic VAR_0x8004, 10, 3
	delay 30
	msgbox LavenderTown_Text_KingfisherInColour
	hidemonpic
	setflag FLAG_NOTEBOOK_LOOSE_PHOTO
	call EventScript_NotebookCopied
	release
	end

LavenderTown_EventScript_KingfisherAgain::
	msgbox LavenderTown_Text_KingfisherNothingEver
	release
	end

LavenderTown_EventScript_KingfisherNothing::
	msgbox LavenderTown_Text_KingfisherNothingToShoot
	release
	end

@  T-439 (the user, 2026-10-22; DAEMONS docs/private/plan-cast-2026-10-22.md, tools/gencast.py): the PEPPERED MOTH -- the
@  textbook animal whose colour her surroundings changed, and changed back. Gossip: the process, never a cause in
@  anyone. Every word DRAFT.
LavenderTown_EventScript_PepperedMoth::
	msgbox LavenderTown_Text_PepperedMoth, MSGBOX_NPC
	end
