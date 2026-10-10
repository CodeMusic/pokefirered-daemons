@ T-221: a grove (tools/gbagrove.py writes this).
ThreeIsland_BerryForest_Grove_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, ThreeIsland_BerryForest_Grove_OnTransition
	.byte 0

@ T-395: found, so its tree in THE NEXUS is an ordinary one; and the fir as it stands (flat until the key).
ThreeIsland_BerryForest_Grove_OnTransition::
	setflag FLAG_THREE_ISLAND_BERRY_FOREST_GROVE_FOUND
	call EventScript_FirKeyFlags
	end

@ The grove's own lone tree takes you back. Nothing is said.
ThreeIsland_BerryForest_Grove_EventScript_Leave::
	lockall
	playse SE_M_CUT
	warp MAP_THREE_ISLAND_BERRY_FOREST, 15, 10
	waitstate
	releaseall
	end
