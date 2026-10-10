@ T-221: a grove (tools/gbagrove.py writes this).
Route1_Grove_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, Route1_Grove_OnTransition
	.byte 0

@ T-395: found, so its tree in THE NEXUS is an ordinary one; and the fir as it stands (flat until the key).
Route1_Grove_OnTransition::
	setflag FLAG_ROUTE1_GROVE_FOUND
	call EventScript_FirKeyFlags
	end

@ The grove's own lone tree takes you back. Nothing is said.
Route1_Grove_EventScript_Leave::
	lockall
	playse SE_M_CUT
	warp MAP_ROUTE1, 4, 9
	waitstate
	releaseall
	end
