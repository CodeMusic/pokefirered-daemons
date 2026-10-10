@ T-221: a grove (tools/gbagrove.py writes this).
ViridianForest_Grove_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, ViridianForest_Grove_OnTransition
	.byte 0

@ T-395: found, so its tree in THE NEXUS is an ordinary one; and the fir as it stands (flat until the key).
ViridianForest_Grove_OnTransition::
	setflag FLAG_VIRIDIAN_FOREST_GROVE_FOUND
	call EventScript_FirKeyFlags
	end

@ The grove's own lone tree takes you back. Nothing is said.
ViridianForest_Grove_EventScript_Leave::
	lockall
	playse SE_M_CUT
	warp MAP_VIRIDIAN_FOREST, 30, 58
	waitstate
	releaseall
	end
