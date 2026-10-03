@  T-348 (the user, 2026-10-03): the first shop clerk spoken to while the player holds OPUS notices it, says what it
@  does and how it is read, and hopes they enjoy it. This reverses vision.md's "nobody remarks on it, then or ever"
@  by the user's decision; who left it is still never answered. Called just before every clerk's "May I help you?",
@  so it is whichever shop comes first, and once only (FLAG_CLERK_NOTICED_OPUS). DRAFT.
EventScript_ClerkNoticesOpus::
	goto_if_unset FLAG_GOT_OPUS, EventScript_ClerkNoticesOpusDone
	goto_if_set FLAG_CLERK_NOTICED_OPUS, EventScript_ClerkNoticesOpusDone
	setflag FLAG_CLERK_NOTICED_OPUS
	msgbox Text_ClerkNoticesOpus
EventScript_ClerkNoticesOpusDone::
	return

Text_ClerkNoticesOpus::
	.string "Oh! Is that OPUS you are carrying?\n"
	.string "I have never seen one up close.\p"
	.string "Now and then it writes a line\n"
	.string "under an entry in your INDEX.\p"
	.string "When an entry has one, press SELECT\n"
	.string "there to turn the page and read it.\p"
	.string "I hope you enjoy it.$"
