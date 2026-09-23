@ T-216: THE RESEARCH NOTEBOOK. Opened from the bag (item_use.c) or from SELECT once registered.
@ src/notebook.c builds each list from whatever is held and hands the choice back in VAR_RESULT;
@ an entry is shown in the ordinary message box, so it pages like a sign and R = AGAIN repeats it.

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
	special Notebook_LoadEntry
	msgbox gStringVar4
	closemessage
	goto EventScript_NotebookEntries

@ T-219: the TEXTBOOK opens straight onto SCHOOL NOTES (section 0), where its boards are filed.
EventScript_Textbook::
	lockall
	setvar VAR_0x8005, 0
	goto EventScript_NotebookEntries

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
