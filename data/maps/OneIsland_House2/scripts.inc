OneIsland_House2_MapScripts::
	.byte 0

OneIsland_House2_EventScript_Lass::
	msgbox OneIsland_House2_Text_IWantToStayHereForever, MSGBOX_NPC
	end

@ T-300: THE PROGRAMMER'S GUIDE TO THE HUMAN MIND, on the bookshelf. The islands teach the mind by reaching for
@ machine learning (vision.md 0.6), and this is that direction as one book, so it is found on the first of them.
OneIsland_House2_EventScript_Guide::
	goto_if_set FLAG_GOT_GUIDE, OneIsland_House2_EventScript_ShelfWithAGap
	lockall
	msgbox OneIsland_House2_Text_GuideOnTheShelf
	giveitem ITEM_GUIDE
	setflag FLAG_GOT_GUIDE
	releaseall
	end

OneIsland_House2_EventScript_ShelfWithAGap::
	msgbox OneIsland_House2_Text_ShelfWithAGap, MSGBOX_SIGN
	end
