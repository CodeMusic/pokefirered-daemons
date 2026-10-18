@ T-416 HOLIDAYS (the user, 2026-10-17/18): a holiday's gifts, at the giver it names -- src/daemons_holidays.c, the
@ table DAEMONS tools/genholidays.py's. VAR_0x8006 is the giver (GIVER_*). An item once per holiday per year (a full
@ bag keeps it for the next visit: giveitem has said so), a line for THE YEAR once ever. The special hands one gift at
@ a time, so a day that holds two (ROLLOVER's line and its STARDUST; the edition's and the COMMON one's) gives both.
@ Every word is DRAFT.
EventScript_HolidayGifts::
	special DaemonsHolidayGiftBegin
EventScript_HolidayGifts_Next::
	specialvar VAR_RESULT, DaemonsHolidayGift
	goto_if_eq VAR_RESULT, 1, EventScript_HolidayGifts_Item
	goto_if_eq VAR_RESULT, 2, EventScript_HolidayGifts_Line
	return

EventScript_HolidayGifts_Item::
	msgbox gStringVar4
	giveitem VAR_0x8004, VAR_0x8005
	goto_if_eq VAR_0x8007, FALSE, EventScript_HolidayGifts_BagFull
	special DaemonsHolidayGiftTaken
	goto EventScript_HolidayGifts_Next

EventScript_HolidayGifts_BagFull::
	return

@ A line arrives as an INSIGHT does, and is filed under THE YEAR as it is said.
@ In the NOTEBOOK's own ink, not the giver's voice (as an obtained item's message is).
EventScript_HolidayGifts_Line::
	copyvar VAR_PREV_TEXT_COLOR, VAR_TEXT_COLOR
	textcolor NPC_TEXT_COLOR_NEUTRAL
	playse SE_PIN
	msgbox gStringVar4
	copyvar VAR_TEXT_COLOR, VAR_PREV_TEXT_COLOR
	goto EventScript_HolidayGifts_Next
