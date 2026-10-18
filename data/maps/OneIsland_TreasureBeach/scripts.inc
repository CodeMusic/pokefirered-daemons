OneIsland_TreasureBeach_MapScripts::
	.byte 0

OneIsland_TreasureBeach_EventScript_Boy::
	msgbox OneIsland_TreasureBeach_Text_GoodThingsWashUpOnBeach, MSGBOX_NPC
	end

@ T-417 P3 (the user, 2026-10-17): the palm on the sand's northern point, drawn as an object (FRLG has no palm in any
@ tileset). In CONTEXT, CONTEXT's FLUTE lies in its shade (FLAG_GOT_INSTRUMENT); in CONTENT, and once it is taken, it
@ is only a palm. Every word DRAFT.
OneIsland_TreasureBeach_EventScript_Palm::
	lock
	.ifdef LEAFGREEN
	goto_if_set FLAG_GOT_INSTRUMENT, OneIsland_TreasureBeach_EventScript_JustAPalm
	msgbox OneIsland_TreasureBeach_Text_PalmShade
	giveitem_msg OneIsland_TreasureBeach_Text_FoundFlute, ITEM_FLUTE, 1, MUS_OBTAIN_KEY_ITEM
	setflag FLAG_GOT_INSTRUMENT
	release
	end
	.endif
OneIsland_TreasureBeach_EventScript_JustAPalm::
	msgbox OneIsland_TreasureBeach_Text_JustAPalm
	release
	end

