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
	copyvar VAR_0x8006, VAR_RESULT
	closemessage
	fadescreen FADE_TO_BLACK
	special Notebook_ReadEntry
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
