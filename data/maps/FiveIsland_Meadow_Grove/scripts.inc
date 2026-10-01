@ T-221: a grove (tools/gbagrove.py writes this).
FiveIsland_Meadow_Grove_MapScripts::
	.byte 0

@ The grove's own lone tree takes you back. Nothing is said.
FiveIsland_Meadow_Grove_EventScript_Leave::
	lockall
	playse SE_M_CUT
	warp MAP_FIVE_ISLAND_MEADOW, 17, 15
	waitstate
	releaseall
	end
