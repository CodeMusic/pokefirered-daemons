OneIsland_TreasureBeach_MapScripts::
	.byte 0

OneIsland_TreasureBeach_EventScript_Boy::
	msgbox OneIsland_TreasureBeach_Text_GoodThingsWashUpOnBeach, MSGBOX_NPC
	end

@ T-417 P3 (the user, 2026-10-17): the palm on the sand's northern point, drawn as an object (FRLG has no palm in any
@ tileset). The user, 2026-10-18: CONTEXT's FLUTE moved to BREATHVERSE (through ROUTE 2's palm), so in both editions
@ this is only a palm, scenery. Its line DRAFT.
OneIsland_TreasureBeach_EventScript_Palm::
	lock
	msgbox OneIsland_TreasureBeach_Text_JustAPalm
	release
	end
