@ T-221: a grove (tools/gbagrove.py writes this).
Route11_Grove_MapScripts::
	.byte 0

@ The grove's own lone tree takes you back. Nothing is said.
Route11_Grove_EventScript_Leave::
	lockall
	playse SE_M_CUT
	warp MAP_ROUTE11, 55, 5
	waitstate
	releaseall
	end
