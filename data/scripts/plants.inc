@ T-381 (the user, 2026-10-06, approved 2026-10-11; bible 0.7): ARTIFICIAL FLOWERS. tools/gbaplants.py writes this
@ file and places the signs. CORPUS's plants are plastic; the CHECKPOINTS' are real. Nobody says what either means.
EventScript_PlasticPlant::
	msgbox Text_PlasticPlant, MSGBOX_SIGN
	end

EventScript_ScornsPlant::
	msgbox Text_ScornsPlant, MSGBOX_SIGN
	end

EventScript_RealPlant::
	msgbox Text_RealPlant, MSGBOX_SIGN
	end

Text_PlasticPlant::
	.string "A tag on the pot says:\n"
	.string "PLEASE DO NOT WATER ME!$"

Text_ScornsPlant::
	.string "The finest plant in the\n"
	.string "building. It has never\p"
	.string "dropped a leaf.\n"
	.string "It has never grown one, either.$"

Text_RealPlant::
	.string "The soil is still damp.\n"
	.string "A new leaf is coming in.$"
