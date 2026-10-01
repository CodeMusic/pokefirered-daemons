@ T-221: a grove (tools/gbagrove.py writes this).
Route8_Grove_MapScripts::
	.byte 0

@ The grove's own lone tree takes you back. Nothing is said.
Route8_Grove_EventScript_Leave::
	lockall
	playse SE_M_CUT
	warp MAP_ROUTE8, 63, 10
	waitstate
	releaseall
	end
