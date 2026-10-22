FiveIsland_ResortGorgeous_MapScripts::
	map_script MAP_SCRIPT_ON_WARP_INTO_MAP_TABLE, FiveIsland_ResortGorgeous_OnWarp
	map_script MAP_SCRIPT_ON_FRAME_TABLE, FiveIsland_ResortGorgeous_OnFrame
	.byte 0

FiveIsland_ResortGorgeous_OnWarp::
	map_script_2 VAR_MAP_SCENE_FIVE_ISLAND_RESORT_GORGEOUS, 1, FiveIsland_ResortGorgeous_EventScript_TurnPlayerNorth
	.2byte 0

FiveIsland_ResortGorgeous_EventScript_TurnPlayerNorth::
	turnobject LOCALID_PLAYER, DIR_NORTH
	end

FiveIsland_ResortGorgeous_OnFrame::
	map_script_2 VAR_MAP_SCENE_FIVE_ISLAND_RESORT_GORGEOUS, 1, FiveIsland_ResortGorgeous_EventScript_SelphyReturnHomeScene
	.2byte 0

FiveIsland_ResortGorgeous_EventScript_SelphyReturnHomeScene::
	lockall
	textcolor NPC_TEXT_COLOR_FEMALE
	msgbox FiveIsland_ResortGorgeous_Text_SelphyThanksYouMayGoNow
	closemessage
	applymovement LOCALID_RESORT_GORGEOUS_SELPHY, Common_Movement_WalkInPlaceFasterUp
	waitmovement 0
	opendoor 39, 8
	waitdooranim
	applymovement LOCALID_RESORT_GORGEOUS_SELPHY, FiveIsland_ResortGorgeous_Movement_SelphyEnterHome
	waitmovement 0
	closedoor 39, 8
	waitdooranim
	removeobject LOCALID_RESORT_GORGEOUS_SELPHY
	clearflag FLAG_HIDE_RESORT_GORGEOUS_INSIDE_SELPHY
	setvar VAR_MAP_SCENE_FIVE_ISLAND_RESORT_GORGEOUS, 2
	releaseall
	end

FiveIsland_ResortGorgeous_Movement_SelphyEnterHome::
	walk_up
	set_invisible
	step_end

FiveIsland_ResortGorgeous_EventScript_SelphysHouseSign::
	msgbox FiveIsland_ResortGorgeous_Text_SelphysHouse, MSGBOX_SIGN
	end

@  T-439 (the user, 2026-10-22; DAEMONS docs/private/plan-cast-2026-10-22.md, tools/gencast.py): a young ERMINE -- the
@  fur of kings, and a coat the season changes, not he (white in winter: PatchObjectPalette). He has hired the
@  PAINTERS to paint the sea. Once a season he asks to see a daemon of the season's type and pays for the look
@  (src/daemons_cast.c; VAR_ERMINE_SEASON). Every word DRAFT.
FiveIsland_ResortGorgeous_EventScript_Ermine::
	lock
	faceplayer
	msgbox FiveIsland_ResortGorgeous_Text_ErmineSea
	special DaemonsCast_ErmineAsks
	goto_if_eq VAR_RESULT, 0, FiveIsland_ResortGorgeous_EventScript_ErmineNextSeason
	msgbox FiveIsland_ResortGorgeous_Text_ErmineThisSeason
	goto_if_eq VAR_RESULT, 2, FiveIsland_ResortGorgeous_EventScript_ErmineNone
	msgbox FiveIsland_ResortGorgeous_Text_ErmineThatOne
	addmoney 1000
	special DaemonsCast_ErminePaid
	playse SE_SHOP
	msgbox FiveIsland_ResortGorgeous_Text_ErminePaid
	release
	end

FiveIsland_ResortGorgeous_EventScript_ErmineNone::
	msgbox FiveIsland_ResortGorgeous_Text_ErmineNone
	release
	end

FiveIsland_ResortGorgeous_EventScript_ErmineNextSeason::
	msgbox FiveIsland_ResortGorgeous_Text_ErmineNextSeason
	release
	end
