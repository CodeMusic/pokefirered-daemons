@ T-221: a grove (tools/gbagrove.py writes this).
SevenIsland_Grove_MapScripts::
	.byte 0

@ The grove's own lone tree takes you back. Nothing is said.
SevenIsland_Grove_EventScript_Leave::
	lockall
	playse SE_M_CUT
	warp MAP_SEVEN_ISLAND, 3, 11
	waitstate
	releaseall
	end
