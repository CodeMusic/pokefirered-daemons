@ T-371 (the user, 2026-10-04): on every CHECKPOINT's second floor, a lemur who has a companion for his daemon tells
@ you about it -- and that loading your save into its app teaches your party's menu SEND (T-370). Once the app has
@ synced this save (FLAG_COMPANION_LINKED), he notices you have it. Every word DRAFT.
CheckpointFloor2_EventScript_CompanionLemur::
	goto_if_set FLAG_COMPANION_LINKED, CheckpointFloor2_EventScript_CompanionLemurLinked
	msgbox CheckpointFloor2_Text_CompanionLemur, MSGBOX_NPC
	end

CheckpointFloor2_EventScript_CompanionLemurLinked::
	msgbox CheckpointFloor2_Text_CompanionLemurLinked, MSGBOX_NPC
	end

CheckpointFloor2_Text_CompanionLemur::
	.string "See this? It's a companion. My\n"
	.string "daemon rides along in it while I\l"
	.string "get things done.\p"
	.string "Load your save into its app, and\n"
	.string "your party's menu learns a new\l"
	.string "word: SEND.\p"
	.string "One goes along at a time. It\n"
	.string "comes home with whatever it\l"
	.string "picked up out there.$"

CheckpointFloor2_Text_CompanionLemurLinked::
	.string "Your save's been to the app\n"
	.string "already. You've got SEND, then.\p"
	.string "Only one goes along at a time.\n"
	.string "The rest wait here.\p"
	.string "They're good at waiting. Better\n"
	.string "than us.$"
