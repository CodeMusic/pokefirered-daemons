@ T-221: a grove (tools/gbagrove.py writes this).
SixIsland_WaterPath_Grove_MapScripts::
	.byte 0

@ The grove's own lone tree takes you back. Nothing is said.
SixIsland_WaterPath_Grove_EventScript_Leave::
	lockall
	playse SE_M_CUT
	warp MAP_SIX_ISLAND_WATER_PATH, 21, 34
	waitstate
	releaseall
	end
