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
