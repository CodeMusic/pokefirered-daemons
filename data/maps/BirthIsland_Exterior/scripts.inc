BirthIsland_Exterior_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, BirthIsland_Exterior_OnTransition
	map_script MAP_SCRIPT_ON_RESUME, BirthIsland_Exterior_OnResume
	map_script MAP_SCRIPT_ON_RETURN_TO_FIELD, BirthIsland_Exterior_OnReturnToField
	.byte 0

BirthIsland_Exterior_OnReturnToField::
	special SetDeoxysTrianglePalette
	end

BirthIsland_Exterior_OnTransition::
	@  T-29 hid the puzzle outright, and that was wrong. In VANILLA this island
	@  needs the AURORA TICKET, an event item that was never distributed, so
	@  PROTEUS was unobtainable on any ordinary cartridge -- and OUR change is
	@  what made the island reachable at all. Turning the daemon off meant
	@  taking away something we had just made possible, which is the one thing
	@  a total conversion should not do.
	@
	@  So it is TIMED rather than removed. While 4.34's scene is unfinished the
	@  island is bare and she is the only thing on it; a legendary standing ten
	@  tiles from the last scene really would be absurd. Once she has read it
	@  and gone home, the island has its puzzle back.
	@
	@  The place where a woman was left, and afterwards something else is
	@  standing on it. Nobody remarks on that either.
	setworldmapflag FLAG_WORLD_MAP_BIRTH_ISLAND_EXTERIOR
	setvar VAR_OBJ_GFX_ID_0, OBJ_EVENT_GFX_DEOXYS_N
	setvar VAR_DEOXYS_INTERACTION_STEP_COUNTER, 0
	setvar VAR_DEOXYS_INTERACTION_NUM, 0
	call_if_set FLAG_FOUGHT_DEOXYS, BirthIsland_Exterior_EventScript_HideDeoxysAndPuzzle
	call_if_unset FLAG_FOUGHT_DEOXYS, BirthIsland_Exterior_EventScript_TryShowDeoxysPuzzle
	@  ...and OURS runs LAST, because TryShowDeoxysPuzzle above CLEARS the
	@  meteorite's hide flag. Gating before it ran was silently undone.
	call_if_unset FLAG_CRYSTAL_READ_PAYLOAD, BirthIsland_Exterior_EventScript_HideDeoxysAndPuzzle
	end

BirthIsland_Exterior_EventScript_HideDeoxysAndPuzzle::
	setflag FLAG_HIDE_DEOXYS
	setflag FLAG_HIDE_BIRTH_ISLAND_METEORITE
	return

BirthIsland_Exterior_EventScript_TryShowDeoxysPuzzle::
	goto_if_set FLAG_DEOXYS_FLEW_AWAY, EventScript_Return
	clearflag FLAG_HIDE_BIRTH_ISLAND_METEORITE
	clearflag FLAG_SYS_DEOXYS_AWAKENED
	return

BirthIsland_Exterior_OnResume::
	call_if_set FLAG_SYS_SPECIAL_WILD_BATTLE, BirthIsland_Exterior_EventScript_TryRemoveDeoxys
	end

BirthIsland_Exterior_EventScript_TryRemoveDeoxys::
	specialvar VAR_RESULT, GetBattleOutcome
	goto_if_ne VAR_RESULT, B_OUTCOME_CAUGHT, EventScript_Return
	removeobject LOCALID_BIRTH_ISLAND_DEOXYS
	return

BirthIsland_Exterior_EventScript_Triangle::
	lock
	faceplayer
	goto_if_questlog EventScript_ReleaseEnd
	special QuestLog_CutRecording
	special DoDeoxysTriangleInteraction
	waitstate
	switch VAR_RESULT
	case 0, BirthIsland_Exterior_EventScript_NotSolved1
	case 1, BirthIsland_Exterior_EventScript_NotSolved2
	case 2, BirthIsland_Exterior_EventScript_Deoxys
	case 3, BirthIsland_Exterior_EventScript_NotSolved3
	end

BirthIsland_Exterior_EventScript_NotSolved1::
	release
	end

BirthIsland_Exterior_EventScript_NotSolved2::
	release
	end

BirthIsland_Exterior_EventScript_NotSolved3::
	release
	end

BirthIsland_Exterior_EventScript_Deoxys::
	addobject LOCALID_BIRTH_ISLAND_DEOXYS
	waitse
	setfieldeffectargument 0, LOCALID_BIRTH_ISLAND_EXTERIOR_ROCK
	setfieldeffectargument 1, MAP_NUM(MAP_BIRTH_ISLAND_EXTERIOR)
	setfieldeffectargument 2, MAP_GROUP(MAP_BIRTH_ISLAND_EXTERIOR)
	dofieldeffect FLDEFF_DESTROY_DEOXYS_ROCK
	playbgm MUS_ENCOUNTER_DEOXYS, 0
	waitfieldeffect FLDEFF_DESTROY_DEOXYS_ROCK
	applymovement LOCALID_BIRTH_ISLAND_DEOXYS, Movement_DeoxysApproach
	waitmovement 0
	waitse
	playmoncry SPECIES_DEOXYS, CRY_MODE_ENCOUNTER
	delay 40
	waitmoncry
	setvar VAR_LAST_TALKED, LOCALID_BIRTH_ISLAND_DEOXYS
	seteventmon SPECIES_DEOXYS, 30
	setflag FLAG_SYS_SPECIAL_WILD_BATTLE
	special StartLegendaryBattle
	waitstate
	clearflag FLAG_SYS_SPECIAL_WILD_BATTLE
	specialvar VAR_RESULT, GetBattleOutcome
	goto_if_eq VAR_RESULT, B_OUTCOME_WON, BirthIsland_Exterior_EventScript_DefeatedDeoxys
	goto_if_eq VAR_RESULT, B_OUTCOME_RAN, BirthIsland_Exterior_EventScript_RanFromDeoxys
	goto_if_eq VAR_RESULT, B_OUTCOME_PLAYER_TELEPORTED, BirthIsland_Exterior_EventScript_RanFromDeoxys
	setflag FLAG_FOUGHT_DEOXYS
	release
	end

BirthIsland_Exterior_EventScript_DefeatedDeoxys::
	setflag FLAG_DEOXYS_FLEW_AWAY
	setvar VAR_0x8004, SPECIES_DEOXYS
	goto EventScript_MonFlewAway
	end

BirthIsland_Exterior_EventScript_RanFromDeoxys::
	setvar VAR_0x8004, SPECIES_DEOXYS
	goto EventScript_MonFlewAway
	end

Movement_DeoxysApproach:
	walk_slower_down
	walk_slower_down
	walk_slower_down
	walk_slower_down
	walk_slower_down
	walk_slower_down
	walk_slower_down
	step_end

@  T-29 / 4.34. The last scene, moved here from the lab: 8.2a had her read it
@  in a room she had been thrown out of, which was the one thing that section
@  got wrong. Every other clause of it survives -- it goes to CRYSTAL and TY
@  does not, and the repair is a fact delivered rather than a feeling
@  exchanged (4.14), because a feeling exchanged is what 4.3 says does not
@  transmit.
BirthIsland_Exterior_EventScript_Crystal::
	lock
	faceplayer
	msgbox BirthIsland_Exterior_Text_CrystalDoesNotAsk
	closemessage
	delay 30
	msgbox BirthIsland_Exterior_Text_PayloadTurnedItOver
	closemessage
	removeitem ITEM_LETTER
	delay 30
	msgbox BirthIsland_Exterior_Text_PayloadWhere
	closemessage
	delay 30
	applymovement LOCALID_ANNEX_CRYSTAL, BirthIsland_Exterior_Movement_CrystalWalksToTheWater
	waitmovement 0
	delay 150
	@  She goes home, and the building she was locked out of stops holding its
	@  new name (T-30). Neither is announced.
	setflag FLAG_CRYSTAL_READ_PAYLOAD
	clearflag FLAG_CRYSTAL_GONE
	clearflag FLAG_HIDE_OAK_IN_HIS_LAB
	fadescreen FADE_TO_BLACK
	warp MAP_PALLET_TOWN_PLAYERS_HOUSE_2F, 10, 4
	waitstate
	releaseall
	end

BirthIsland_Exterior_Movement_CrystalWalksToTheWater::
	walk_up
	walk_up
	walk_up
	face_up
	step_end
