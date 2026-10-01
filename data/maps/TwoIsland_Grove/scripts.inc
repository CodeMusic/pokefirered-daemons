@ T-221: a grove (tools/gbagrove.py writes this).
TwoIsland_Grove_MapScripts::
	.byte 0

@ The grove's own lone tree takes you back. Nothing is said.
TwoIsland_Grove_EventScript_Leave::
	lockall
	playse SE_M_CUT
	warp MAP_TWO_ISLAND, 35, 6
	waitstate
	releaseall
	end
