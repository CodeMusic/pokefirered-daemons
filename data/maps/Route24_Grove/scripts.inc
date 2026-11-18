@ T-221: a grove (tools/gbagrove.py writes this).
Route24_Grove_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, Route24_Grove_OnTransition
	map_script MAP_SCRIPT_ON_FRAME_TABLE, EventScript_Garden_OnFrame
	.byte 0

@ T-395: found, so its tree in THE NEXUS is an ordinary one; and the fir as it stands (flat until the key).
@ T-585: and, the fir in its key, THE GARDEN OF ECHOES -- who is here this time.
Route24_Grove_OnTransition::
	setflag FLAG_ROUTE24_GROVE_FOUND
	call EventScript_FirKeyFlags
	call EventScript_Garden_OnTransition
	end

@ The grove's own lone tree takes you back. Nothing is said.
Route24_Grove_EventScript_Leave::
	lockall
	playse SE_M_CUT
	warp MAP_ROUTE24, 6, 13
	waitstate
	releaseall
	end
