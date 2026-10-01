@ T-221: a grove (tools/gbagrove.py writes this).
Route24_Grove_MapScripts::
	.byte 0

@ The grove's own lone tree takes you back. Nothing is said.
Route24_Grove_EventScript_Leave::
	lockall
	playse SE_M_CUT
	warp MAP_ROUTE24, 6, 13
	waitstate
	releaseall
	end
