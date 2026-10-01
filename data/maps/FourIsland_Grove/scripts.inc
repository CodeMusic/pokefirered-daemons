@ T-221: a grove (tools/gbagrove.py writes this).
FourIsland_Grove_MapScripts::
	.byte 0

@ The grove's own lone tree takes you back. Nothing is said.
FourIsland_Grove_EventScript_Leave::
	lockall
	playse SE_M_CUT
	warp MAP_FOUR_ISLAND, 10, 18
	waitstate
	releaseall
	end
