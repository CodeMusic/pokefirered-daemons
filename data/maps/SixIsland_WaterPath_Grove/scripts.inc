@ T-221: a grove (tools/gbagrove.py writes this).
SixIsland_WaterPath_Grove_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, SixIsland_WaterPath_Grove_OnTransition
	.byte 0

@ T-395: found, so its tree in THE NEXUS is an ordinary one; and the fir as it stands (flat until the key).
SixIsland_WaterPath_Grove_OnTransition::
	setflag FLAG_SIX_ISLAND_WATER_PATH_GROVE_FOUND
	call EventScript_FirKeyFlags
	end

@ The grove's own lone tree takes you back. Nothing is said.
SixIsland_WaterPath_Grove_EventScript_Leave::
	lockall
	playse SE_M_CUT
	warp MAP_SIX_ISLAND_WATER_PATH, 11, 39
	waitstate
	releaseall
	end
