SaffronCity_House_MapScripts::
	.byte 0

SaffronCity_House_EventScript_Lass::
	msgbox SaffronCity_House_Text_DontLookAtMyLetter, MSGBOX_NPC
	end

SaffronCity_House_EventScript_Youngster::
	msgbox SaffronCity_House_Text_GettingCopycatPokeDoll, MSGBOX_NPC
	end

SaffronCity_House_EventScript_Pidgey::
	lock
	faceplayer
	waitse
	playmoncry SPECIES_PIDGEY, CRY_MODE_NORMAL
	msgbox SaffronCity_House_Text_Pidgey
	waitmoncry
	release
	end

@ T-423: THE SUBTLE LAYER, an INTERPRETER's book, found on the shelf of an INTERPRETER's home (the user, 2026-10-18).
@ The owner says little and never explains it. The shelf sparkles (reveal.c's beacons and finds) until it is taken.
SaffronCity_House_EventScript_Interpreter::
	msgbox SaffronCity_House_Text_ReadItOrDont, MSGBOX_NPC
	end

SaffronCity_House_EventScript_SubtleLayer::
	goto_if_set FLAG_GOT_SUBTLE_LAYER, SaffronCity_House_EventScript_ShelfWithAGap
	lockall
	msgbox SaffronCity_House_Text_SubtleOnTheShelf
	giveitem ITEM_SUBTLE_LAYER
	setflag FLAG_GOT_SUBTLE_LAYER
	releaseall
	end

SaffronCity_House_EventScript_ShelfWithAGap::
	msgbox SaffronCity_House_Text_ShelfWithAGap, MSGBOX_SIGN
	end

SaffronCity_House_EventScript_Letter::
	msgbox SaffronCity_House_Text_ExplainPPUp, MSGBOX_SIGN
	end
