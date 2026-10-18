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
	msgbox Text_Play_ThankYou
	releaseall
	end

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
