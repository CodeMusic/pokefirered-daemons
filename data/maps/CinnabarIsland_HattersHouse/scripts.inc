@  T-485 (the user, 2026-10-29; bible 4.39): OSRIC HATTER's house, the leaning top hat on QUICKSILVER's south sand.
@  Every clock in it is stopped at half past three; a long tea table is laid for many, and only he sits at it, talking
@  to the empty chairs. Anyone may visit. Every word approved by the user 2026-10-31 (DAEMONS gfx/drafts/t485/lines.md).
@
@    - a visit's riddle, A1 -> A4 and round again: VAR_OSRIC_RIDDLE is the next one, moved on at the visit's first
@      talk; talking again in the same visit repeats it (VAR_TEMP_2 holds this visit's, + 1);
@    - with THE HERMETIC LAYER held (FLAG_GOT_HERMETIC_LAYER), the visit's riddle and then the next book riddle,
@      B1 -> B3 in turn (VAR_OSRIC_BOOK_RIDDLE; VAR_TEMP_3 this visit's, + 1) -- the book adds, it does not replace;
@    - from 3:30 to 3:59 by the clock, when his clocks are right, C instead of everything, the glitch between its
@      second and third boxes (T-486), and no counter moves.
@  And every entry from 3:00 to 3:59 glitches once, at that minute's strength (T-486; src/daemons_hatter.c).

CinnabarIsland_HattersHouse_MapScripts::
	map_script MAP_SCRIPT_ON_FRAME_TABLE, CinnabarIsland_HattersHouse_OnFrame
	.byte 0

CinnabarIsland_HattersHouse_OnFrame::
	map_script_2 VAR_TEMP_1, 0, CinnabarIsland_HattersHouse_EventScript_Entered
	.2byte 0

CinnabarIsland_HattersHouse_EventScript_Entered::
	lockall
	setvar VAR_TEMP_1, 1
	specialvar VAR_RESULT, Hatter_Glitch
	call_if_eq VAR_RESULT, TRUE, CinnabarIsland_HattersHouse_EventScript_WaitForGlitch
	releaseall
	end

CinnabarIsland_HattersHouse_EventScript_WaitForGlitch::
	delay 16
	return

CinnabarIsland_HattersHouse_EventScript_Osric::
	lock
	faceplayer
	specialvar VAR_RESULT, Hatter_ClocksAreRight
	goto_if_eq VAR_RESULT, TRUE, CinnabarIsland_HattersHouse_EventScript_HalfPastThree
	goto_if_ne VAR_TEMP_2, 0, CinnabarIsland_HattersHouse_EventScript_Riddles
	copyvar VAR_TEMP_2, VAR_OSRIC_RIDDLE
	addvar VAR_TEMP_2, 1
	addvar VAR_OSRIC_RIDDLE, 1
	call_if_ge VAR_OSRIC_RIDDLE, 4, CinnabarIsland_HattersHouse_EventScript_RiddlesRoundAgain
	goto_if_unset FLAG_GOT_HERMETIC_LAYER, CinnabarIsland_HattersHouse_EventScript_Riddles
	copyvar VAR_TEMP_3, VAR_OSRIC_BOOK_RIDDLE
	addvar VAR_TEMP_3, 1
	addvar VAR_OSRIC_BOOK_RIDDLE, 1
	call_if_ge VAR_OSRIC_BOOK_RIDDLE, 3, CinnabarIsland_HattersHouse_EventScript_BookRoundAgain
CinnabarIsland_HattersHouse_EventScript_Riddles::
	call_if_eq VAR_TEMP_2, 1, CinnabarIsland_HattersHouse_EventScript_A1
	call_if_eq VAR_TEMP_2, 2, CinnabarIsland_HattersHouse_EventScript_A2
	call_if_eq VAR_TEMP_2, 3, CinnabarIsland_HattersHouse_EventScript_A3
	call_if_eq VAR_TEMP_2, 4, CinnabarIsland_HattersHouse_EventScript_A4
	call_if_eq VAR_TEMP_3, 1, CinnabarIsland_HattersHouse_EventScript_B1
	call_if_eq VAR_TEMP_3, 2, CinnabarIsland_HattersHouse_EventScript_B2
	call_if_eq VAR_TEMP_3, 3, CinnabarIsland_HattersHouse_EventScript_B3
	release
	end

CinnabarIsland_HattersHouse_EventScript_RiddlesRoundAgain::
	setvar VAR_OSRIC_RIDDLE, 0
	return

CinnabarIsland_HattersHouse_EventScript_BookRoundAgain::
	setvar VAR_OSRIC_BOOK_RIDDLE, 0
	return

CinnabarIsland_HattersHouse_EventScript_A1::
	msgbox CinnabarIsland_HattersHouse_Text_A1
	return

CinnabarIsland_HattersHouse_EventScript_A2::
	msgbox CinnabarIsland_HattersHouse_Text_A2
	return

CinnabarIsland_HattersHouse_EventScript_A3::
	msgbox CinnabarIsland_HattersHouse_Text_A3
	return

CinnabarIsland_HattersHouse_EventScript_A4::
	msgbox CinnabarIsland_HattersHouse_Text_A4
	return

CinnabarIsland_HattersHouse_EventScript_B1::
	msgbox CinnabarIsland_HattersHouse_Text_B1
	return

CinnabarIsland_HattersHouse_EventScript_B2::
	msgbox CinnabarIsland_HattersHouse_Text_B2
	return

CinnabarIsland_HattersHouse_EventScript_B3::
	msgbox CinnabarIsland_HattersHouse_Text_B3
	return

@  3:30 to 3:59: for once every clock in the house is right. The glitch between the second box and the third.
CinnabarIsland_HattersHouse_EventScript_HalfPastThree::
	msgbox CinnabarIsland_HattersHouse_Text_HalfPastThree
	specialvar VAR_RESULT, Hatter_Glitch
	call_if_eq VAR_RESULT, TRUE, CinnabarIsland_HattersHouse_EventScript_WaitForGlitch
	msgbox CinnabarIsland_HattersHouse_Text_NothingBroke
	release
	end
