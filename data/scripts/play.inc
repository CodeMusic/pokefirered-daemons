@ T-417 P2 (the user, 2026-10-17; DAEMONS docs/private/draft-play-2026-10-17.md 8): what a song PLAYED opens.
@ src/play.c listens, echoes the song, puts the instrument away and starts one of these. Every word DRAFT.

@ O CHRISTMAS TREE in B-flat, played facing a singing fir (it is the one talked to now). A flat fir sings it back in
@ its key and is keyed, as the band's sheet keys it (EventScript_SingingFir_GivenKey); then the door opens with no
@ YES/NO -- the player played it open.
EventScript_Play_FirOpened::
	lock
	goto_if_set FLAG_FIR_IN_KEY, EventScript_Play_FirDoor
	setvar VAR_0x8004, TRUE
	special SingingFir_Sing
	waitstate
	setflag FLAG_FIR_IN_KEY
	clearflag FLAG_HIDE_FIR_UNKEYED
	copyvar VAR_0x8005, VAR_LAST_TALKED
	removeobject VAR_0x8005
	addvar VAR_0x8005, 1
	addobject VAR_0x8005
	msgbox Text_SingingFir_Right
EventScript_Play_FirDoor::
	msgbox Text_Play_FirParts
	call_if_unset FLAG_NEXUS_VISITED, EventScript_SingingFir_FirstTime
	playse SE_WARP_IN
	warp MAP_NEXUS, 12, 5
	waitstate
	release
	end

@ The right shape in the wrong key, to a fir: it hums its flat version back.
EventScript_Play_FirAlmost::
	lock
	setvar VAR_0x8004, FALSE
	special SingingFir_Sing
	waitstate
	msgbox Text_Play_FirAlmost
	release
	end

@ The right shape in the wrong key, anywhere else.
EventScript_Play_Almost::
	lockall
	msgbox Text_Play_Almost
	releaseall
	end

@ O CHRISTMAS TREE in B-flat, played to THE NEXUS's Christmas tree: it lights, note by note, in its key.
EventScript_Play_NexusLit::
	lockall
	setvar VAR_0x8004, TRUE
	special SingingFir_Sing
	waitstate
	setflag FLAG_NEXUS_LIT
	special Play_LightNexus
	msgbox Text_Play_ThankYou
	releaseall
	end

@ ------------------------------------------------------------------------------------------------------------------
@ T-417 P4 (the user, 2026-10-17): THE SEVEN. Each is HEARD in its ROOT's place once that understanding is held --
@ the place calls this with VAR_0x8004 the song (include/constants/play.h). The game plays it, its notes drifting up
@ from what is singing. Heard with the instrument in hand, it can be played back, in its key, to be learned; heard
@ without one it is only a tune, and its place plays it again next time. Learned, it is not played here again.
EventScript_Play_Hear::
	specialvar VAR_RESULT, Play_ShouldHear
	goto_if_eq VAR_RESULT, FALSE, EventScript_Play_HearDone
	closemessage
	special Play_Hear
	waitstate
	goto_if_unset FLAG_GOT_INSTRUMENT, EventScript_Play_HumAlong
	special Play_BufferSongKey
	msgbox Text_Play_HeardKey
	return

EventScript_Play_HumAlong::
	msgbox Text_Play_HumAlong
EventScript_Play_HearDone::
	return

@ Played back in its key: the notes joined, the echo -- and now it is written into the SONGBOOK.
EventScript_Play_Learned::
	lockall
	playse SE_SELECT
	msgbox Text_Play_Learned
	releaseall
	end

@ TWELVE DAYS OF DECEMBER, learned and played: the calendar, sung -- the day's banner.
EventScript_Play_Calendar::
	lockall
	special Play_ShowDayBanner
	releaseall
	end

@ THE REALIZATION: the grass goes still -- no wild daemons for 200 steps (vanilla's REPEL counter).
EventScript_Play_Still::
	lockall
	setvar VAR_REPEL_STEP_COUNT, 200
	msgbox Text_Play_Still
	releaseall
	end

@ FORGIVENESS OFFERED, where a daemon flew away (play.c cleared its flag; VAR_0x8005 is it): it is there again.
EventScript_Play_CallBack::
	lockall
	fadescreen FADE_TO_BLACK
	addobject VAR_0x8005
	fadescreen FADE_FROM_BLACK
	msgbox Text_Play_CallBack
	releaseall
	end

@ SEQUEL THE GUARDIAN, played in THE NEXUS: the GUARDIAN answers, in the words its doors already say.
EventScript_Play_Guardian::
	lockall
	msgbox Nexus_Text_Voice
	msgbox Nexus_Text_Guardian0
	releaseall
	end

@ REFLECTIONS WITHIN, facing water, once a day: a REFLECTION rises from it.
EventScript_Play_Reflection::
	lockall
	msgbox Text_Play_Rises
	setwildbattle SPECIES_REFLECTION, 40
	dowildbattle
	releaseall
	end

Text_Play_HeardKey::
	.string "The notes hang in the air,\n"
	.string "in the key of {STR_VAR_1}.$"

Text_Play_HumAlong::
	.string "You hum along.$"

Text_Play_Learned::
	.string "{PLAYER} wrote it into the\n"
	.string "SONGBOOK.$"

Text_Play_Still::
	.string "Everything goes quiet.$"

Text_Play_CallBack::
	.string "Something that left\n"
	.string "has come back.$"

Text_Play_Rises::
	.string "Something rises from the water.$"

Text_Play_FirParts::
	.string "The branches part, like a door,\n"
	.string "and {PLAYER} steps through.$"

Text_Play_FirAlmost::
	.string "Close. The fir hums it back,\n"
	.string "and something in it sits wrong.$"

Text_Play_Almost::
	.string "Close. The shape is right, and\n"
	.string "something in it sits wrong.$"

Text_Play_ThankYou::
	.string "Thank you.$"
