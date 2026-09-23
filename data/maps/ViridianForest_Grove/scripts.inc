@ T-221: a grove -- PROVISIONAL, one built to prove the mechanism (tools/gbagrove.py writes this).
ViridianForest_Grove_MapScripts::
	.byte 0

@ The grove's own lone tree takes you back. Nothing is said.
ViridianForest_Grove_EventScript_Leave::
	lockall
	playse SE_M_CUT
	warp MAP_VIRIDIAN_FOREST, 30, 58
	waitstate
	releaseall
	end
