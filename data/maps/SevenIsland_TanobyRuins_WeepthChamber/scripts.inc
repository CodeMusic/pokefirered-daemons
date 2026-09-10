SevenIsland_TanobyRuins_WeepthChamber_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, SevenIsland_TanobyRuins_WeepthChamber_OnTransition
	.byte 0

SevenIsland_TanobyRuins_WeepthChamber_OnTransition::
	call EventScript_TryDarkenRuins
	end

@  T-21. One of seven. Nobody here is lying.
SevenIsland_TanobyRuins_WeepthChamber_EventScript_Braille::
	lockall
	braillemessage Braille_Text_Chamber3
	waitbuttonpress
	braillemessage Braille_Text_Chamber3b
	waitbuttonpress
	braillemessage Braille_Text_Chamber3c
	waitbuttonpress
	releaseall
	end
