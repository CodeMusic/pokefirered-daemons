OneIsland_House2_MapScripts::
	map_script MAP_SCRIPT_ON_LOAD, OneIsland_House2_OnLoad
	.byte 0

@  T-297: the map-hints pilot. ONE book in the bookcase is pulled out -- findable blind -- and once the Guide is
@  taken, the place it stood is a gap. Both are metatiles of GenericBuilding2 drawn by tools/gbaguideshelf.py,
@  set here and not in the layout, because LAYOUT_HOUSE3 is shared by fifteen island houses.
OneIsland_House2_OnLoad::
	call_if_unset FLAG_GOT_GUIDE, OneIsland_House2_EventScript_ShelfBookOut
	call_if_set FLAG_GOT_GUIDE, OneIsland_House2_EventScript_ShelfGap
	end

OneIsland_House2_EventScript_ShelfBookOut::
	setmetatile 1, 1, 0x338, TRUE    @ 824: the red spine pulled out
	return

OneIsland_House2_EventScript_ShelfGap::
	setmetatile 1, 1, 0x339, TRUE    @ 825: the gap it left
	return

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
	call OneIsland_House2_EventScript_ShelfGap
	special DrawWholeMapView
	releaseall
	end

OneIsland_House2_EventScript_ShelfWithAGap::
	msgbox OneIsland_House2_Text_ShelfWithAGap, MSGBOX_SIGN
	end
