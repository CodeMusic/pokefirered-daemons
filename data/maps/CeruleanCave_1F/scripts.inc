CeruleanCave_1F_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, CeruleanCave_1F_OnTransition
	.byte 0

CeruleanCave_1F_OnTransition::
	setworldmapflag FLAG_WORLD_MAP_CERULEAN_CAVE_1F
	end

@  T-309, DRAFT: A DREAM SIGN. The Guide's chapter on dreams gives one reality check -- read a line, look away, read
@  it again -- and in a dream it will not say the same thing twice. The unread cave is a dream you wake from (T-254),
@  so its one plate reads differently every time; once the cave is understood it says one thing and keeps saying it.
@  Nothing here says why, and the Guide never names the cave.
CeruleanCave_1F_EventScript_Plate::
	lockall
	goto_if_set FLAG_UNDERSTANDING_FIRST, CeruleanCave_1F_EventScript_PlateSteady
	random 6
	switch VAR_RESULT
	case 0, CeruleanCave_1F_EventScript_Plate0
	case 1, CeruleanCave_1F_EventScript_Plate1
	case 2, CeruleanCave_1F_EventScript_Plate2
	case 3, CeruleanCave_1F_EventScript_Plate3
	case 4, CeruleanCave_1F_EventScript_Plate4
	msgbox CeruleanCave_1F_Text_Plate5
	releaseall
	end

CeruleanCave_1F_EventScript_PlateSteady::
	msgbox CeruleanCave_1F_Text_PlateSteady
	releaseall
	end

CeruleanCave_1F_EventScript_Plate0::
	msgbox CeruleanCave_1F_Text_Plate0
	releaseall
	end

CeruleanCave_1F_EventScript_Plate1::
	msgbox CeruleanCave_1F_Text_Plate1
	releaseall
	end

CeruleanCave_1F_EventScript_Plate2::
	msgbox CeruleanCave_1F_Text_Plate2
	releaseall
	end

CeruleanCave_1F_EventScript_Plate3::
	msgbox CeruleanCave_1F_Text_Plate3
	releaseall
	end

CeruleanCave_1F_EventScript_Plate4::
	msgbox CeruleanCave_1F_Text_Plate4
	releaseall
	end

CeruleanCave_1F_Text_PlateSteady::
	.string "DOLDRUM CAVE\n"
	.string "Surveyed and mapped. Three floors.\p"
	.string "Keep to the lit paths.$"

CeruleanCave_1F_Text_Plate0::
	.string "DOLDRUM CAVE\n"
	.string "Surveyed and mapped. Seven floors.\p"
	.string "Keep to the lit paths.$"

CeruleanCave_1F_Text_Plate1::
	.string "DOLDUM CAVE\n"
	.string "Surveyed and unmapped. Three floors.\p"
	.string "Keep to the paths you lit.$"

CeruleanCave_1F_Text_Plate2::
	.string "DOLDRUM CAVE\n"
	.string "Surveyed twice. The floors disagree.\p"
	.string "Keep to the paths.$"

CeruleanCave_1F_Text_Plate3::
	.string "DOLDRUM COVE\n"
	.string "Mapped and surveyed. No floors.\p"
	.string "Keep the lit paths to yourself.$"

CeruleanCave_1F_Text_Plate4::
	.string "CAVE DOLDRUM\n"
	.string "Surveyed and mapped. Three doors.\p"
	.string "Keep to the lit paths.$"

CeruleanCave_1F_Text_Plate5::
	.string "DOLDRUM CAVE\n"
	.string "Three floors. Surveyed by you.\p"
	.string "You were keeping to the lit paths.$"
