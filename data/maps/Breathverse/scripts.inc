@ T-417 (the user, 2026-10-18): BREATHVERSE (DRAFT name), the islet where CONTEXT's FLUTE lies. Reached by pressing A on
@ the lone palm on ROUTE 2; the islet's own palm takes you back. Nothing is said either way, as with a grove's trees.
@ The map's bytes and events are tools/gbabreathverse.py's; these scripts and their words are written by hand.
Breathverse_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, Breathverse_OnTransition
	.byte 0

@ The FLUTE lies in the sand in CONTEXT until it is taken; CONTENT's islet has none (its instrument is the HARP).
Breathverse_OnTransition::
	setflag FLAG_HIDE_BREATHVERSE_FLUTE
	.ifdef LEAFGREEN
	call_if_unset FLAG_GOT_INSTRUMENT, Breathverse_EventScript_FluteLies
	.endif
	end

Breathverse_EventScript_FluteLies::
	clearflag FLAG_HIDE_BREATHVERSE_FLUTE
	return

@ The palm beside the arrival takes you back to ROUTE 2's.
Breathverse_EventScript_Leave::
	lockall
	playse SE_M_CUT
	warp MAP_ROUTE2, 12, 76
	waitstate
	releaseall
	end

@ Every word DRAFT.
Breathverse_EventScript_Flute::
	lock
	msgbox Breathverse_Text_FluteInSand
	giveitem_msg Breathverse_Text_FoundFlute, ITEM_FLUTE, 1, MUS_OBTAIN_KEY_ITEM
	setflag FLAG_GOT_INSTRUMENT
	removeobject VAR_LAST_TALKED
	release
	end
