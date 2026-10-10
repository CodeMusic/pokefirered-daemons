@ T-221: a grove (tools/gbagrove.py writes this).
TwoIsland_Grove_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, TwoIsland_Grove_OnTransition
	.byte 0

@ T-395: found, so its tree in THE NEXUS is an ordinary one; and the fir as it stands (flat until the key).
TwoIsland_Grove_OnTransition::
	setflag FLAG_TWO_ISLAND_GROVE_FOUND
	call EventScript_FirKeyFlags
	end

@ The grove's own lone tree takes you back. Nothing is said.
TwoIsland_Grove_EventScript_Leave::
	lockall
	playse SE_M_CUT
	warp MAP_TWO_ISLAND, 35, 6
	waitstate
	releaseall
	end
