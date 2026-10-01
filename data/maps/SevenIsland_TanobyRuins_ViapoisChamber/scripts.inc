SevenIsland_TanobyRuins_ViapoisChamber_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, SevenIsland_TanobyRuins_ViapoisChamber_OnTransition
	.byte 0

SevenIsland_TanobyRuins_ViapoisChamber_OnTransition::
	call EventScript_TryDarkenRuins
	end

@  T-21. One of seven. Nobody here is lying.
SevenIsland_TanobyRuins_ViapoisChamber_EventScript_Braille::
	lockall
	braillemessage Braille_Text_Chamber7
	waitbuttonpress
	braillemessage Braille_Text_Chamber7b
	waitbuttonpress
@  T-252 (the user, 2026-10-01): THE FIRST UNDERSTANDING is arrived at here, reading the last of the seven -- set
@  silently, as every understanding is; it shows as a note in the GUIDE's DREAMS ("Read it twice. It stayed.") and as
@  DOLDRUM CAVE seen clearly.
	setflag FLAG_UNDERSTANDING_FIRST
	releaseall
	end
