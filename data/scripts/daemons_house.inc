@ T-427 (the user, 2026-10-18; confirmed 2026-10-19): what the house keeps goes to CORPUS. src/daemons_house.c keeps
@ the number; this is where it shows -- the ledger on B1F's desk, and the plastic plants it buys (T-381's words).
@ Never explained. Every word DRAFT.
EventScript_HouseLedger::
	lockall
	special DaemonsHouse_ReadTake
	goto_if_eq VAR_RESULT, HOUSE_TAKE_KEPT, EventScript_HouseLedgerKept
	goto_if_eq VAR_RESULT, HOUSE_TAKE_OWED, EventScript_HouseLedgerOwed
	msgbox Text_HouseLedgerNone
	releaseall
	end

EventScript_HouseLedgerKept::
	msgbox Text_HouseLedgerKept
	releaseall
	end

EventScript_HouseLedgerOwed::
	msgbox Text_HouseLedgerOwed
	releaseall
	end

@ A plant's sign answers only once the take has paid for the plant
EventScript_HousePlant::
	special DaemonsHouse_PlantHere
	goto_if_eq VAR_RESULT, HOUSE_PLANT_PLASTIC, EventScript_PlasticPlant
	goto_if_eq VAR_RESULT, HOUSE_PLANT_SCORNS, EventScript_ScornsPlant
	end

@  T-427, DRAFT.
Text_HouseLedgerKept::
	.string "A ledger lies open on the desk.\p"
	.string "UP FROM THE GAME CORNER\n"
	.string "AND ITS BRANCHES: ¥{STR_VAR_1}\p"
	.string "Every coin counted, and every\n"
	.string "coin welcome!\p"
	.string "Thank you for playing!\p"
	.string "Kept with care by ACCOUNTS.$"

@  T-427, DRAFT.
Text_HouseLedgerOwed::
	.string "A ledger lies open on the desk.\p"
	.string "UP FROM THE GAME CORNER\n"
	.string "AND ITS BRANCHES: -¥{STR_VAR_1}\p"
	.string "OWED BY {PLAYER}: ¥{STR_VAR_1}\n"
	.string "We look forward to your visit!\p"
	.string "Kept with care by ACCOUNTS.$"

@  T-427, DRAFT.
Text_HouseLedgerNone::
	.string "A ledger lies open on the desk.\p"
	.string "UP FROM THE GAME CORNER\n"
	.string "AND ITS BRANCHES: ¥0\p"
	.string "A fresh page! Thank you for\n"
	.string "playing!\p"
	.string "Kept with care by ACCOUNTS.$"
