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
	.string "The STREAM in the BAG has\n"
	.string "something on it that was not\l"
	.string "there before.$"

@  T-179, R = AGAIN. The caller has already copied the line back into gStringVar4; this is only the
@  script context a field message needs in order to wait for a button and close itself. lockall,
@  because the player should not walk out from under a line they asked to read again.
Daemons_EventScript_Again::
	lockall
	msgbox gStringVar4
	releaseall
	end
