SevenIsland_TanobyRuins_MoneanChamber_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, SevenIsland_TanobyRuins_MoneanChamber_OnTransition
	.byte 0

SevenIsland_TanobyRuins_MoneanChamber_OnTransition::
	setworldmapflag FLAG_WORLD_MAP_SEVEN_ISLAND_TANOBY_RUINS_MONEAN_CHAMBER
	call EventScript_TryDarkenRuins
	end

@  T-21. One of seven. Nobody here is lying.
SevenIsland_TanobyRuins_MoneanChamber_EventScript_Braille::
	lockall
	braillemessage Braille_Text_Chamber1
	waitbuttonpress
	braillemessage Braille_Text_Chamber1b
	waitbuttonpress
	releaseall
	end
