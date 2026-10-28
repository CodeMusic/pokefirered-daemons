@  T-12. Shown on the first step after a MARK unlocks a show on the STREAM.
@  Craft rule 1: it says a thing arrived and where to find it, and nothing
@  about what the thing is for.
EventScript_StreamHasANewShow::
	lockall
	msgbox EventScript_StreamHasANewShow_Text, MSGBOX_DEFAULT
	closemessage
	releaseall
	end

EventScript_StreamHasANewShow_Text::
	.string "The STREAM in the POOL has\n"
	.string "something on it that was not there\l"
	.string "before.$"

@  T-179, R = AGAIN. The caller has already copied the line back into gStringVar4; this is only the
@  script context a field message needs in order to wait for a button and close itself. lockall,
@  because the player should not walk out from under a line they asked to read again.
Daemons_EventScript_Again::
	lockall
	msgbox gStringVar4
	releaseall
	end

@  T-203: the one beat OPUS gets. It does not name itself, and nobody in the game ever mentions it again.
@  A player who reads this and then looks at what they are carrying has worked it out themselves, which is
@  the whole of the restraint: not explaining it, but not leaving them nothing to connect either.
Daemons_EventScript_OpusWrote::
	lockall
	msgbox Daemons_Text_OpusWrote
	releaseall
	end

@  T-390 (bible 0.7, Wisdom): an insight arrives, the first step after a MARK and its understanding are both held.
@  notebook.c's Daemons_TryInsightArrives leaves the line in gStringVar4; it is filed under INSIGHT as it is said.
Daemons_EventScript_Insight::
	lockall
	playse SE_PIN
	msgbox gStringVar4
	releaseall
	end

@  T-459 B: a read, the first step on a map, now and then, once THE SUBTLE LAYER is held. daemons_reads.c's
@  Daemons_TryReadArrives leaves it in gStringVar4, in the book's purple; it is written in the NOTEBOOK's log as it is said.
Daemons_EventScript_Read::
	lockall
	msgbox gStringVar4
	releaseall
	end

@  T-412: a dream, the first step after the first dawn after a milestone (or after a load on a later day). notebook.c's
@  Daemons_TryDreamArrives leaves its line and the dream in gStringVar4; it is filed in the DREAM LOG as it is said.
Daemons_EventScript_Dream::
	lockall
	msgbox gStringVar4
	releaseall
	end

@  T-448 / T-455: the line on the first step after a load that finished a daemon the companion grew or delivered
@  (daemons_carried.c leaves it in gStringVar4); then, if a party daemon came home evolved and its level teaches its new
@  species a routine, the party menu offers it as a level-up does.
Daemons_EventScript_CarriedArrives::
	lockall
	msgbox gStringVar4
	specialvar VAR_RESULT, Daemons_TryCarriedMoves
	goto_if_eq VAR_RESULT, FALSE, Daemons_EventScript_CarriedDone
	closemessage
	special Daemons_OfferCarriedMoves
	waitstate
Daemons_EventScript_CarriedDone:
	releaseall
	end

@  T-455: one that waited in a box, offered on the first step after it is withdrawn (or a second in the party).
Daemons_EventScript_CarriedMoves::
	lockall
	special Daemons_OfferCarriedMoves
	waitstate
	releaseall
	end

Daemons_Text_OpusWrote:
	.string "Something has been written in the\nINDEX.$"
