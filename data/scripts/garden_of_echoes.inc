@ T-585 THE GARDEN OF ECHOES (the user, 2026-11-17, by multiple choice: "A, the three at the fir"; DAEMONS
@ docs/garden-of-echoes-draft.md, A, and vision 3.4). ROUTE 24's grove, once its singing fir is in its key: a hare that
@ comes once, touches the fir and goes; a magpie beside a cairn with a pale stone on it; a mole half out of its hole
@ with a sheaf of pages that grows. Nobody explains the three. Optional; gates nothing. Every word DRAFT.
@
@ The grove's objects (local ids below) and the two hooks into its map scripts are written by DAEMONS
@ tools/gbagrove.py (GARDEN); the sprites by tools/gengarden.py. Before the key the grove is as it always was.

	.set LOCALID_GARDEN_FIR, 2      @ the fir in its key (gbagrove: the dim one is 1, the LARK 3)
	.set LOCALID_GARDEN_HARE, 4
	.set LOCALID_GARDEN_SHEAF, 8

@ From Route24_Grove_OnTransition, after EventScript_FirKeyFlags. Every hide flag is decided here on each arrival.
EventScript_Garden_OnTransition::
	setflag FLAG_HIDE_GARDEN
	setflag FLAG_HIDE_GARDEN_HARE
	setflag FLAG_HIDE_GARDEN_TRACKS
	goto_if_unset FLAG_FIR_IN_KEY, EventScript_Garden_Return
	clearflag FLAG_HIDE_GARDEN
	call_if_lt VAR_GARDEN_VISITS, 255, EventScript_Garden_CountVisit
	call_if_set FLAG_GARDEN_HARE_GONE, EventScript_Garden_ShowTracks
	call_if_unset FLAG_GARDEN_HARE_GONE, EventScript_Garden_ShowHare
	@ the sheaf: a few pages (its template faces south), then a pile, then a heap
	call_if_eq VAR_GARDEN_VISITS, 2, EventScript_Garden_SheafPile
	call_if_ge VAR_GARDEN_VISITS, 3, EventScript_Garden_SheafHeap
EventScript_Garden_Return::
	return

EventScript_Garden_CountVisit::
	addvar VAR_GARDEN_VISITS, 1
	return

EventScript_Garden_ShowTracks::
	clearflag FLAG_HIDE_GARDEN_TRACKS
	return

EventScript_Garden_ShowHare::
	clearflag FLAG_HIDE_GARDEN_HARE
	setvar VAR_TEMP_1, 1
	return

EventScript_Garden_SheafPile::
	setobjectmovementtype LOCALID_GARDEN_SHEAF, MOVEMENT_TYPE_FACE_UP
	return

EventScript_Garden_SheafHeap::
	setobjectmovementtype LOCALID_GARDEN_SHEAF, MOVEMENT_TYPE_FACE_LEFT
	return

@ The grove's frame table: the hare's one visit plays as the player arrives.
EventScript_Garden_OnFrame::
	map_script_2 VAR_TEMP_1, 1, EventScript_Garden_Hare
	.2byte 0

EventScript_Garden_Hare::
	lockall
	setvar VAR_TEMP_1, 0
	applymovement LOCALID_GARDEN_HARE, Movement_Garden_HareTouches
	waitmovement 0
	setvar VAR_LAST_TALKED, LOCALID_GARDEN_FIR
	setvar VAR_0x8004, TRUE
	special SingingFir_SingBar
	waitstate
	msgbox Text_Garden_HareThanks
	closemessage
	applymovement LOCALID_GARDEN_HARE, Movement_Garden_HareGoes
	waitmovement 0
	removeobject LOCALID_GARDEN_HARE
	setflag FLAG_GARDEN_HARE_GONE
	releaseall
	end

Movement_Garden_HareTouches:
	delay_16
	walk_in_place_left
	delay_16
	step_end

@ Out down the east side and into the long grass, where it is gone.
Movement_Garden_HareGoes:
	jump_right
	jump_down
	jump_down
	jump_down
	jump_down
	jump_down
	jump_down
	step_end

@ The magpie, every keyed visit. A second word in the same visit is the offer.
EventScript_Garden_Magpie::
	lock
	faceplayer
	goto_if_set FLAG_TEMP_2, EventScript_Garden_MagpieAgain
	setflag FLAG_TEMP_2
	msgbox Text_Garden_MagpieWarm
	msgbox Text_Garden_MagpieKeep
	release
	end

EventScript_Garden_MagpieAgain::
	msgbox Text_Garden_MagpieTouch
	release
	end

@ A on the cairn. Cold, whatever is held; THE SUBTLE LAYER adds TOUCH's read (daemons_reads.c Daemons_TouchStone).
EventScript_Garden_Cairn::
	lock
	msgbox Text_Garden_StoneCold
	special Daemons_TouchStone
	goto_if_eq VAR_RESULT, FALSE, EventScript_Garden_CairnEnd
	msgbox gStringVar4
EventScript_Garden_CairnEnd::
	release
	end

@ The mole: the first keyed visit, then one line a visit in turn, the last kept.
EventScript_Garden_Mole::
	lock
	goto_if_ge VAR_GARDEN_VISITS, 3, EventScript_Garden_MoleBeautiful
	goto_if_eq VAR_GARDEN_VISITS, 2, EventScript_Garden_MoleLight
	msgbox Text_Garden_MoleLeaf
	msgbox Text_Garden_MoleNearly
	release
	end

EventScript_Garden_MoleLight::
	msgbox Text_Garden_MoleLight
	release
	end

EventScript_Garden_MoleBeautiful::
	msgbox Text_Garden_MoleBeautiful
	release
	end

@ The tracks and the sheaf say nothing.
EventScript_Garden_Nothing::
	end

@ Called from EventScript_Play_FirDoor (play.inc): the fir's song played to it in its key, here, with the mole there.
@ The mole stops writing for one line, every time, before the branches part.
EventScript_Garden_MoleHears::
	special Compass_CurrentMap
	goto_if_ne VAR_RESULT, MAP_ROUTE24_GROVE, EventScript_Garden_Return
	goto_if_set FLAG_HIDE_GARDEN, EventScript_Garden_Return
	msgbox Text_Garden_MoleDots
	msgbox Text_Garden_MoleWhere
	return

@ ---- the words (DRAFT, every one; docs/garden-of-echoes-draft.md A) ----

Text_Garden_HareThanks::
	.string "Oh. Thank you.$"

Text_Garden_MagpieWarm::
	.string "It was warm, the day I found it.$"

Text_Garden_MagpieKeep::
	.string "I keep it here so it stays.$"

Text_Garden_MagpieTouch::
	.string "Touch it, if you like. Gently.$"

Text_Garden_StoneCold::
	.string "The stone is cold.$"

Text_Garden_MoleLeaf::
	.string "Leaf, leaf, leaf.\n"
	.string "North wind, two knots.$"

Text_Garden_MoleNearly::
	.string "I have nearly all of it.$"

Text_Garden_MoleLight::
	.string "The light was here.\n"
	.string "I have it down.$"

Text_Garden_MoleBeautiful::
	.string "I hear it is beautiful.\n"
	.string "I have that down too.$"

Text_Garden_MoleDots::
	.string "...$"

Text_Garden_MoleWhere::
	.string "Where was that? I had it written.$"
