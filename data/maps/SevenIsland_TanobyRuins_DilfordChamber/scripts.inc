SevenIsland_TanobyRuins_DilfordChamber_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, SevenIsland_TanobyRuins_DilfordChamber_OnTransition
	.byte 0

SevenIsland_TanobyRuins_DilfordChamber_OnTransition::
	call EventScript_TryDarkenRuins
	end

@  T-21. One of seven. Nobody here is lying.
SevenIsland_TanobyRuins_DilfordChamber_EventScript_Braille::
	lockall
	braillemessage Braille_Text_Chamber4
	waitbuttonpress
	braillemessage Braille_Text_Chamber4b
	waitbuttonpress
	releaseall
	end
