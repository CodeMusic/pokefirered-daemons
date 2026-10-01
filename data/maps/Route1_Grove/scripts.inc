@ T-221: a grove (tools/gbagrove.py writes this).
Route1_Grove_MapScripts::
	.byte 0

@ The grove's own lone tree takes you back. Nothing is said.
Route1_Grove_EventScript_Leave::
	lockall
	playse SE_M_CUT
	warp MAP_ROUTE1, 4, 9
	waitstate
	releaseall
	end
