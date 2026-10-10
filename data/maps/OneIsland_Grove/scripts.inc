@ T-221: a grove (tools/gbagrove.py writes this).
OneIsland_Grove_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, OneIsland_Grove_OnTransition
	.byte 0

@ T-395: found, so its tree in THE NEXUS is an ordinary one; and the fir as it stands (flat until the key).
OneIsland_Grove_OnTransition::
	setflag FLAG_ONE_ISLAND_GROVE_FOUND
	call EventScript_FirKeyFlags
	end

@ The grove's own lone tree takes you back. Nothing is said.
OneIsland_Grove_EventScript_Leave::
	lockall
	playse SE_M_CUT
	warp MAP_ONE_ISLAND, 16, 11
	waitstate
	releaseall
	end
