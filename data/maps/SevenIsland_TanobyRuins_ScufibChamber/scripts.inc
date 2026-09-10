SevenIsland_TanobyRuins_ScufibChamber_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, SevenIsland_TanobyRuins_ScufibChamber_OnTransition
	.byte 0

SevenIsland_TanobyRuins_ScufibChamber_OnTransition::
	call EventScript_TryDarkenRuins
	end

@  T-21. One of seven. Nobody here is lying.
SevenIsland_TanobyRuins_ScufibChamber_EventScript_Braille::
	lockall
	braillemessage Braille_Text_Chamber5
	waitbuttonpress
	braillemessage Braille_Text_Chamber5b
	waitbuttonpress
	releaseall
	end
