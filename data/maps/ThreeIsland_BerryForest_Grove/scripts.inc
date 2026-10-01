@ T-221: a grove (tools/gbagrove.py writes this).
ThreeIsland_BerryForest_Grove_MapScripts::
	.byte 0

@ The grove's own lone tree takes you back. Nothing is said.
ThreeIsland_BerryForest_Grove_EventScript_Leave::
	lockall
	playse SE_M_CUT
	warp MAP_THREE_ISLAND_BERRY_FOREST, 15, 10
	waitstate
	releaseall
	end
