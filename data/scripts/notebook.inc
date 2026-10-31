@ T-216: THE RESEARCH NOTEBOOK. Opened from the bag (item_use.c) or from SELECT once registered.
@ src/notebook.c builds each list from whatever is held and hands the choice back in VAR_RESULT;
@ a chosen entry opens full screen on a ruled sheet (book_reader.c, T-223), and B brings you back here.

EventScript_Notebook::
	lockall
	special Notebook_HasEntries
	goto_if_eq VAR_RESULT, FALSE, EventScript_NotebookEmpty
EventScript_NotebookSections::
	message Text_NotebookWhichSection
	waitmessage
	special Notebook_ChooseSection
	waitstate
	goto_if_eq VAR_RESULT, 0x7F, EventScript_NotebookClose
	copyvar VAR_0x8005, VAR_RESULT
EventScript_NotebookEntries::
	message Text_NotebookWhichPage
	waitmessage
	special Notebook_ChooseEntry
	waitstate
	goto_if_eq VAR_RESULT, 0x7F, EventScript_NotebookSections
	goto_if_eq VAR_RESULT, 0x7E, EventScript_NotebookShowMap   @ T-403: R -- where the page was found
	copyvar VAR_0x8006, VAR_RESULT
	closemessage
	fadescreen FADE_TO_BLACK
	special Notebook_ReadEntry
	waitstate
	goto EventScript_NotebookEntries

@ T-403: the TOWN MAP, its cursor on the place the page in VAR_0x8006 was found; closing it comes back to the list.
EventScript_NotebookShowMap::
	closemessage
	fadescreen FADE_TO_BLACK
	special Notebook_ShowOnMap
	waitstate
	goto EventScript_NotebookEntries

@ T-223: the TEXTBOOK is its own book now -- CONTENTS, a chapter a floor, a topic a board (book_reader.c).
EventScript_Textbook::
	lockall
	fadescreen FADE_TO_BLACK
	special Textbook_Open
	waitstate
	releaseall
	end

@ T-300: THE PROGRAMMER'S GUIDE TO THE HUMAN MIND, opened at its cover.
EventScript_Guide::
	lockall
	fadescreen FADE_TO_BLACK
	special Guide_Open
	waitstate
	releaseall
	end

@ T-423: THE SUBTLE LAYER, opened at its cover.
EventScript_SubtleLayer::
	lockall
	fadescreen FADE_TO_BLACK
	special Subtle_Open
	waitstate
	releaseall
	end

@ T-483: THE HERMETIC LAYER, opened at its cover -- only at night (src/daemons_hermetic.c). DRAFT, every word.
EventScript_HermeticLayer::
	lockall
	specialvar VAR_RESULT, Hermetic_IsNight
	goto_if_eq VAR_RESULT, FALSE, EventScript_HermeticByDay
	fadescreen FADE_TO_BLACK
	special Hermetic_Open
	waitstate
	specialvar VAR_RESULT, Spellbook_TakeNews
	goto_if_eq VAR_RESULT, FALSE, EventScript_HermeticClosed
	msgbox gStringVar4
EventScript_HermeticClosed::
	releaseall
	end

EventScript_HermeticByDay::
	msgbox Text_HermeticByDay
	releaseall
	end

@ T-483 (5): THE SPELLBOOK, from the START menu once THE HERMETIC LAYER is held -- only at night, by the clock, as the
@ book (src/daemons_spellbook.c). The list hands back the spell, or 127 for B (SPELLBOOK_CANCEL); the cast leaves what
@ it did in gStringVar4. DRAFT, every word.
EventScript_Spellbook::
	lockall
	specialvar VAR_RESULT, Hermetic_IsNight
	goto_if_eq VAR_RESULT, FALSE, EventScript_SpellbookByDay
	special Spellbook_Choose
	waitstate
	goto_if_eq VAR_RESULT, 127, EventScript_SpellbookShut
	special Spellbook_Cast
	playse SE_M_MORNING_SUN
	msgbox gStringVar4
EventScript_SpellbookShut::
	releaseall
	end

EventScript_SpellbookByDay::
	msgbox Text_SpellbookByDay
	releaseall
	end

Text_SpellbookByDay::
	.string "The SPELLBOOK's ink is dry. It is\n"
	.string "still light, and spells wait for night.$"

@ T-483: the book arrives -- the first daemon to use its hour's routine with both other books held, or a SYNC from the
@ companion. Said once, in the field, the next second the player has the controls.
EventScript_HermeticArrives::
	lockall
	msgbox Text_HermeticArrives
	releaseall
	end

Text_HermeticByDay::
	.string "The pages will not part. It is still\n"
	.string "light, and this book reads by night.$"

@ T-483 (7): TRICK OR TREAT -- on Halloween with THE HERMETIC LAYER, a daemon met leaves a treat (DRAFT)
EventScript_HalloweenTreat::
	lockall
	playse SE_SELECT
	msgbox Text_HalloweenTreat
	releaseall
	end

Text_HalloweenTreat::
	.string "{STR_VAR_1} left a treat:\n"
	.string "{STR_VAR_2}!$"

Text_HermeticArrives::
	.string "There is a book in the bag that was\n"
	.string "not there before.\p"
	.string "THE HERMETIC LAYER. Its pages are\n"
	.string "cool, and it opens only at night.$"

@ T-218/T-219: a document the player has just read is filed. Said once, and only if the NOTEBOOK is held --
@ before CAIRN hands it over, the page is still copied out (every entry is a flag), just not into anything.
EventScript_NotebookCopied::
	playse SE_SELECT
	goto_if_unset FLAG_GOT_NOTEBOOK, EventScript_NotebookCopiedOut
	msgbox Text_NotebookCopied
	return

EventScript_NotebookCopiedOut::
	msgbox Text_NotebookCopiedOut
	return

@ A document read again: it is already filed, and nothing more is said.
EventScript_NotebookAlreadyFiled::
	releaseall
	end

EventScript_NotebookEmpty::
	msgbox Text_NotebookEmpty
EventScript_NotebookClose::
	closemessage
	releaseall
	end

Text_NotebookWhichSection::
	.string "NOTEBOOK. Which part?$"

Text_NotebookWhichPage::
	.string "Which page?$"

Text_NotebookEmpty::
	.string "Nothing has been written in it\n"
	.string "yet.$"

Text_NotebookCopied::
	.string "{PLAYER} copied it into the\n"
	.string "NOTEBOOK.$"

Text_NotebookCopiedOut::
	.string "{PLAYER} copied it out.$"
