SSAnne_Exterior_MapScripts::
	map_script MAP_SCRIPT_ON_LOAD, SSAnne_Exterior_OnLoad
	map_script MAP_SCRIPT_ON_TRANSITION, SSAnne_Exterior_OnTransition
	map_script MAP_SCRIPT_ON_FRAME_TABLE, SSAnne_Exterior_OnFrame
	.byte 0

@ T-10: with the ship gone, the pier's end is plain boards -- its arrow would otherwise walk the player into a
@ ship that has sailed (0x1C1 is the pier tile one step up).
SSAnne_Exterior_OnLoad::
	call_if_ge VAR_MAP_SCENE_VERMILION_CITY, 2, SSAnne_Exterior_EventScript_PierEnd
	end

SSAnne_Exterior_EventScript_PierEnd::
	setmetatile 32, 14, 0x1C1, FALSE
@  T-325 (the user, 2026-10-01): and a gangway the ship left behind, from the pier to the far quay, so the singing fir
@  can be walked to. Row 9 is raised to the pier's height in map.bin and stays blocked water while the ship is in.
@  TRAVERSE still reaches it the old way.
	setmetatile 34, 9, 0x1C1, FALSE
	setmetatile 35, 9, 0x1C1, FALSE
	setmetatile 36, 9, 0x1C1, FALSE
	setmetatile 37, 9, 0x1C1, FALSE
	setmetatile 38, 9, 0x1C1, FALSE
	setmetatile 39, 9, 0x1C1, FALSE
	setmetatile 40, 9, 0x1C1, FALSE
	setmetatile 41, 9, 0x1C1, FALSE
	setmetatile 42, 9, 0x1C1, FALSE
	setmetatile 43, 9, 0x1C1, FALSE
	setmetatile 44, 9, 0x1C1, FALSE
	setmetatile 45, 9, 0x1C1, FALSE
	setmetatile 46, 9, 0x1C1, FALSE
	setmetatile 47, 9, 0x1C1, FALSE
	setmetatile 48, 9, 0x1C1, FALSE
	setmetatile 49, 9, 0x1C1, FALSE
	setmetatile 50, 9, 0x1C1, FALSE
	setmetatile 51, 9, 0x1C1, FALSE
	setmetatile 52, 9, 0x1C1, FALSE
	setmetatile 53, 9, 0x1C1, FALSE
	setmetatile 54, 9, 0x1C1, FALSE
	setmetatile 55, 9, 0x1C1, FALSE
	setmetatile 56, 9, 0x1C1, FALSE
	return

SSAnne_Exterior_OnTransition::
	setworldmapflag FLAG_WORLD_MAP_SSANNE_EXTERIOR
	call_if_ge VAR_MAP_SCENE_VERMILION_CITY, 2, SSAnne_Exterior_EventScript_ShipGone
	end

@ T-10: the ship was only ever removed for the length of the departure scene; now the pier is walkable after it
@ sails, it stays gone.
SSAnne_Exterior_EventScript_ShipGone::
	setflag FLAG_HIDE_SS_ANNE
	return

SSAnne_Exterior_OnFrame::
	map_script_2 VAR_MAP_SCENE_VERMILION_CITY, 1, SSAnne_Exterior_ExitSSAnne
	.2byte 0

SSAnne_Exterior_ExitSSAnne::
	lockall
	getplayerxy VAR_0x8004, VAR_0x8005
	call_if_le VAR_0x8005, 6, SSAnne_Exterior_WalkDown
	call_if_ge VAR_0x8005, 7, SSAnne_Exterior_WalkInPlaceDown
	fadenewbgm MUS_SURF
	delay 50
	special DoSSAnneDepartureCutscene
	waitstate
	removeobject LOCALID_SS_ANNE
	applymovement LOCALID_PLAYER, SSAnne_Exterior_Movement_Exit
	waitmovement 0
	setvar VAR_MAP_SCENE_VERMILION_CITY, 2
	setvar VAR_VERMILION_CITY_TICKET_CHECK_TRIGGER, 0
	warp MAP_VERMILION_CITY, 23, 34
	waitstate
	releaseall
	end

SSAnne_Exterior_WalkDown::
	applymovement LOCALID_PLAYER, SSAnne_Exterior_Movement_WalkDown
	waitmovement 0
	return

SSAnne_Exterior_WalkInPlaceDown::
	applymovement LOCALID_PLAYER, Common_Movement_WalkInPlaceFasterDown
	waitmovement 0
	return

SSAnne_Exterior_Movement_Exit::
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	step_end

SSAnne_Exterior_Movement_WalkDown::
	walk_down
	walk_down
	walk_down
	walk_down
	walk_down
	walk_down
	walk_down
	walk_down
	walk_down
	step_end

@ T-10: THE SINGING FIR (the user, 2026-09-26/27; DRAFT wording). Where the old legend's truck stood, reached by SURF
@ from the empty pier. It sings O CHRISTMAS TREE without its key signature -- the yellow and purple notes of the
@ source painting a semitone sharp -- and, given the key (the band's sheet, LOOSE PAGES 8), sings it right. The
@ first time with the sheet it sings both, one after the other; after that, only the right one.
SSAnne_Exterior_EventScript_SingingFir::
	lock
	goto_if_set FLAG_FIR_IN_KEY, SSAnne_Exterior_EventScript_FirInKey
	msgbox SSAnne_Exterior_Text_FirHums
	setvar VAR_0x8004, FALSE
	special SingingFir_Sing
	waitstate
	goto_if_set FLAG_NOTEBOOK_LOOSE_KEY, SSAnne_Exterior_EventScript_FirGivenKey
	msgbox SSAnne_Exterior_Text_FirOff
	release
	end

SSAnne_Exterior_EventScript_FirGivenKey::
	msgbox SSAnne_Exterior_Text_HoldUpSheet
	setvar VAR_0x8004, TRUE
	special SingingFir_Sing
	waitstate
	setflag FLAG_FIR_IN_KEY
	msgbox SSAnne_Exterior_Text_FirRight
	release
	end

SSAnne_Exterior_EventScript_FirInKey::
	msgbox SSAnne_Exterior_Text_FirHums
	setvar VAR_0x8004, TRUE
	special SingingFir_Sing
	waitstate
	release
	end

@  DRAFT (T-10). This map has no text.inc of its own; its few lines live here.
SSAnne_Exterior_Text_FirHums::
    .string "The fir's branches stir.\n"
    .string "It hums an old tune.$"

SSAnne_Exterior_Text_FirOff::
    .string "Close to right, all the way\n"
    .string "through. Something is missing.$"

SSAnne_Exterior_Text_HoldUpSheet::
    .string "{PLAYER} held up the band's sheet.\p"
    .string "The fir hums the tune again.$"

SSAnne_Exterior_Text_FirRight::
    .string "The same notes.\n"
    .string "This time, every one is home.$"
