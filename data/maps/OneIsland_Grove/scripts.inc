@ T-221: a grove (tools/gbagrove.py writes this).
OneIsland_Grove_MapScripts::
	.byte 0

@ The grove's own lone tree takes you back. Nothing is said.
OneIsland_Grove_EventScript_Leave::
	lockall
	playse SE_M_CUT
	warp MAP_ONE_ISLAND, 16, 11
	waitstate
	releaseall
	end
