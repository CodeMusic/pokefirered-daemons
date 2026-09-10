@  The DEBUG submenu's two field entries. 9.3's scaffolding is a start-menu
@  submenu, and everything in it so far is one C call -- heal, restock, play a
@  track. These two are not: one warps and one wants a yes/no box, and both of
@  those are things the script engine already does properly.
@
@  Assembled only into the debug ROM. The Makefile passes DAEMONS_DEBUG to the
@  assembler as well as to the C preprocessor, so a script can be fenced the
@  same way a function is.
.if DAEMONS_DEBUG == 1

@  THE RECORD. Warps to the Hall of Fame and lets that map's own on-frame
@  script do all of it: CRYSTAL CLEAR congratulates you, FLDEFF_HALL_OF_FAME_
@  RECORD puts your name in, EventScript_SetDefeatedEliteFourFlagsVars sets
@  FLAG_SYS_GAME_CLEAR, the respawn moves to BLANCHE, and EnterHallOfFame runs
@  the credits and wakes you at home. Nothing here reimplements any of that.
DaemonsDebug_EventScript_TheRecord::
	lockall
	msgbox DaemonsDebug_Text_TheRecord, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, DaemonsDebug_EventScript_Cancel
	closemessage
	warp MAP_POKEMON_LEAGUE_HALL_OF_FAME, 5, 12
	waitstate
	releaseall
	end

@  THE ISLANDS. The ferry at ARDOR PORT is the general route -- it needs the
@  S.S. ANNE gone (VERMILION scene 3) and CELIO's quest at 5, which is the
@  point the RAINBOW PASS is handed over. The two map flags are the region
@  map's, and the two passes are the items the sailor never actually checks.
@
@  Deliberately NOT set past 5: recovering the SAPPHIRE is real content, and a
@  debug jump that skips content is a debug jump that stops you testing it.
DaemonsDebug_EventScript_TheIslands::
	lockall
	msgbox DaemonsDebug_Text_TheIslands, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, DaemonsDebug_EventScript_Cancel
	setflag FLAG_SYS_SEVII_MAP_123
	setflag FLAG_SYS_SEVII_MAP_4567
	setflag FLAG_WORLD_MAP_ONE_ISLAND
	additem ITEM_TRI_PASS
	additem ITEM_RAINBOW_PASS
	setvar VAR_MAP_SCENE_VERMILION_CITY, 3
	setvar VAR_MAP_SCENE_ONE_ISLAND_POKEMON_CENTER_1F, 5
	playfanfare MUS_OBTAIN_KEY_ITEM
	message DaemonsDebug_Text_IslandsOpened
	waitmessage
	waitfanfare
	closemessage
	releaseall
	end

DaemonsDebug_EventScript_Cancel::
	closemessage
	releaseall
	end

DaemonsDebug_Text_TheRecord::
	.string "DEBUG: enter the RECORD?\p"
	.string "You will be taken to the HALL, your\n"
	.string "party will be entered, and the\l"
	.string "credits will run.$"

DaemonsDebug_Text_TheIslands::
	.string "DEBUG: open the SEVII ISLANDS?\p"
	.string "The ferry at ARDOR PORT will sail,\n"
	.string "and both passes go in the BAG.$"

DaemonsDebug_Text_IslandsOpened::
	.string "The ferry at ARDOR PORT is running.$"

.endif
