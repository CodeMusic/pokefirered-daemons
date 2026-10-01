SixIsland_WaterPath_MapScripts::
	.byte 0

SixIsland_WaterPath_EventScript_HornWantedSign::
	msgbox SixIsland_WaterPath_Text_WantedUltimateHorn, MSGBOX_SIGN
	end

SixIsland_WaterPath_EventScript_RouteSign::
	msgbox SixIsland_WaterPath_Text_RouteSign, MSGBOX_SIGN
	end

@ T-221: the tree that stands alone answers A -- the way into a grove. Nothing is said; REVEAL makes it
@ twinkle (src/reveal.c), and pressing A on the right tree finds it without.
SixIsland_WaterPath_EventScript_Grove::
	lockall
	playse SE_M_CUT
	warp MAP_SIX_ISLAND_WATER_PATH_GROVE, 8, 8
	waitstate
	releaseall
	end
