@ T-221: a grove (tools/gbagrove.py writes this).
Route25_Grove_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, Route25_Grove_OnTransition
	.byte 0

@ T-395: found, so its tree in THE NEXUS is an ordinary one; and the fir as it stands (flat until the key).
Route25_Grove_OnTransition::
	setflag FLAG_ROUTE25_GROVE_FOUND
	call EventScript_FirKeyFlags
	end

@ The grove's own lone tree takes you back. Nothing is said.
Route25_Grove_EventScript_Leave::
	lockall
	playse SE_M_CUT
	warp MAP_ROUTE25, 18, 6
	waitstate
	releaseall
	end
