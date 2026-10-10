@ T-221: a grove (tools/gbagrove.py writes this).
FiveIsland_Meadow_Grove_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, FiveIsland_Meadow_Grove_OnTransition
	.byte 0

@ T-395: found, so its tree in THE NEXUS is an ordinary one; and the fir as it stands (flat until the key).
FiveIsland_Meadow_Grove_OnTransition::
	setflag FLAG_FIVE_ISLAND_MEADOW_GROVE_FOUND
	call EventScript_FirKeyFlags
	end

@ The grove's own lone tree takes you back. Nothing is said.
FiveIsland_Meadow_Grove_EventScript_Leave::
	lockall
	playse SE_M_CUT
	warp MAP_FIVE_ISLAND_MEADOW, 17, 15
	waitstate
	releaseall
	end
