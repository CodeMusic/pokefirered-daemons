@ T-221: a grove (tools/gbagrove.py writes this).
Route13_Grove_MapScripts::
	.byte 0

@ The grove's own lone tree takes you back. Nothing is said.
Route13_Grove_EventScript_Leave::
	lockall
	playse SE_M_CUT
	warp MAP_ROUTE13, 44, 11
	waitstate
	releaseall
	end
