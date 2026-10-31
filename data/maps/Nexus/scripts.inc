@ T-395: THE NEXUS (docs/nexus.md) -- tools/gbanexusmap.py writes this file. Every word approved by the user, 2026-10-10.

Nexus_MapScripts::
	map_script MAP_SCRIPT_ON_LOAD, Nexus_OnLoad
	map_script MAP_SCRIPT_ON_TRANSITION, Nexus_OnTransition
	.byte 0

Nexus_OnTransition::
	setflag FLAG_NEXUS_VISITED
	end

@ The map is drawn with every door grey; a grove once arrived in is an ordinary tree again.
Nexus_OnLoad::
	call_if_set FLAG_ROUTE1_GROVE_FOUND, Nexus_EventScript_Found0
	call_if_set FLAG_VIRIDIAN_FOREST_GROVE_FOUND, Nexus_EventScript_Found1
	call_if_set FLAG_ROUTE24_GROVE_FOUND, Nexus_EventScript_Found2
	call_if_set FLAG_ROUTE25_GROVE_FOUND, Nexus_EventScript_Found3
	call_if_set FLAG_ROUTE11_GROVE_FOUND, Nexus_EventScript_Found4
	call_if_set FLAG_ROUTE8_GROVE_FOUND, Nexus_EventScript_Found5
	call_if_set FLAG_ROUTE13_GROVE_FOUND, Nexus_EventScript_Found6
	call_if_set FLAG_ONE_ISLAND_GROVE_FOUND, Nexus_EventScript_Found7
	call_if_set FLAG_TWO_ISLAND_GROVE_FOUND, Nexus_EventScript_Found8
	call_if_set FLAG_THREE_ISLAND_BERRY_FOREST_GROVE_FOUND, Nexus_EventScript_Found9
	call_if_set FLAG_FOUR_ISLAND_GROVE_FOUND, Nexus_EventScript_Found10
	call_if_set FLAG_FIVE_ISLAND_MEADOW_GROVE_FOUND, Nexus_EventScript_Found11
	call_if_set FLAG_SIX_ISLAND_WATER_PATH_GROVE_FOUND, Nexus_EventScript_Found12
	call_if_set FLAG_SEVEN_ISLAND_GROVE_FOUND, Nexus_EventScript_Found13
	end

Nexus_EventScript_Found0::
	setmetatile 3, 3, 0x1DB, TRUE
	setmetatile 4, 3, 0x1DC, TRUE
	setmetatile 3, 4, 0x1DD, TRUE
	setmetatile 4, 4, 0x1DE, TRUE
	return

Nexus_EventScript_Found1::
	setmetatile 7, 5, 0x1EB, TRUE
	setmetatile 8, 5, 0x1EC, TRUE
	setmetatile 7, 6, 0x1ED, TRUE
	setmetatile 8, 6, 0x1EE, TRUE
	return

Nexus_EventScript_Found2::
	setmetatile 3, 8, 0x1FB, TRUE
	setmetatile 4, 8, 0x1FC, TRUE
	setmetatile 3, 9, 0x1FD, TRUE
	setmetatile 4, 9, 0x1FE, TRUE
	return

Nexus_EventScript_Found3::
	setmetatile 7, 10, 0x20B, TRUE
	setmetatile 8, 10, 0x20C, TRUE
	setmetatile 7, 11, 0x20D, TRUE
	setmetatile 8, 11, 0x20E, TRUE
	return

Nexus_EventScript_Found4::
	setmetatile 3, 13, 0x21B, TRUE
	setmetatile 4, 13, 0x21C, TRUE
	setmetatile 3, 14, 0x21D, TRUE
	setmetatile 4, 14, 0x21E, TRUE
	return

Nexus_EventScript_Found5::
	setmetatile 7, 15, 0x22B, TRUE
	setmetatile 8, 15, 0x22C, TRUE
	setmetatile 7, 16, 0x22D, TRUE
	setmetatile 8, 16, 0x22E, TRUE
	return

Nexus_EventScript_Found6::
	setmetatile 4, 18, 0x23B, TRUE
	setmetatile 5, 18, 0x23C, TRUE
	setmetatile 4, 19, 0x23D, TRUE
	setmetatile 5, 19, 0x23E, TRUE
	return

Nexus_EventScript_Found7::
	setmetatile 21, 3, 0x24B, TRUE
	setmetatile 22, 3, 0x24C, TRUE
	setmetatile 21, 4, 0x24D, TRUE
	setmetatile 22, 4, 0x24E, TRUE
	return

Nexus_EventScript_Found8::
	setmetatile 17, 5, 0x25B, TRUE
	setmetatile 18, 5, 0x25C, TRUE
	setmetatile 17, 6, 0x25D, TRUE
	setmetatile 18, 6, 0x25E, TRUE
	return

Nexus_EventScript_Found9::
	setmetatile 21, 8, 0x26B, TRUE
	setmetatile 22, 8, 0x26C, TRUE
	setmetatile 21, 9, 0x26D, TRUE
	setmetatile 22, 9, 0x26E, TRUE
	return

Nexus_EventScript_Found10::
	setmetatile 17, 10, 0x27B, TRUE
	setmetatile 18, 10, 0x27C, TRUE
	setmetatile 17, 11, 0x27D, TRUE
	setmetatile 18, 11, 0x27E, TRUE
	return

Nexus_EventScript_Found11::
	setmetatile 21, 13, 0x28B, TRUE
	setmetatile 22, 13, 0x28C, TRUE
	setmetatile 21, 14, 0x28D, TRUE
	setmetatile 22, 14, 0x28E, TRUE
	return

Nexus_EventScript_Found12::
	setmetatile 17, 15, 0x29B, TRUE
	setmetatile 18, 15, 0x29C, TRUE
	setmetatile 17, 16, 0x29D, TRUE
	setmetatile 18, 16, 0x29E, TRUE
	return

Nexus_EventScript_Found13::
	setmetatile 20, 18, 0x2AB, TRUE
	setmetatile 21, 18, 0x2AC, TRUE
	setmetatile 20, 19, 0x2AD, TRUE
	setmetatile 21, 19, 0x2AE, TRUE
	return

@ Route1_Grove
Nexus_EventScript_Door0::
	lockall
	goto_if_unset FLAG_ROUTE1_GROVE_FOUND, Nexus_EventScript_Locked0
	msgbox Nexus_Text_DoorOpen, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, Nexus_EventScript_Stay
	msgbox Nexus_Text_Voice
	msgbox Nexus_Text_Guardian0
	playse SE_WARP_IN
	warp MAP_ROUTE1_GROVE, 11, 4
	waitstate
	releaseall
	end

Nexus_EventScript_Locked0::
	switch VAR_TEMP_1
	case 0, Nexus_EventScript_Smudge0
	case 1, Nexus_EventScript_Lines0
	msgbox Nexus_Text_Locked3
	releaseall
	end

Nexus_EventScript_Smudge0::
	setmetatile 3, 3, 0x1E3, TRUE
	setmetatile 4, 3, 0x1E4, TRUE
	setmetatile 3, 4, 0x1E5, TRUE
	setmetatile 4, 4, 0x1E6, TRUE
	special DrawWholeMapView
	setvar VAR_TEMP_1, 1
	msgbox Nexus_Text_Locked1
	releaseall
	end

Nexus_EventScript_Lines0::
	setmetatile 3, 3, 0x1DF, TRUE
	setmetatile 4, 3, 0x1E0, TRUE
	setmetatile 3, 4, 0x1E1, TRUE
	setmetatile 4, 4, 0x1E2, TRUE
	special DrawWholeMapView
	setvar VAR_TEMP_1, 2
	msgbox Nexus_Text_Locked2
	releaseall
	end

@ ViridianForest_Grove
Nexus_EventScript_Door1::
	lockall
	goto_if_unset FLAG_VIRIDIAN_FOREST_GROVE_FOUND, Nexus_EventScript_Locked1
	msgbox Nexus_Text_DoorOpen, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, Nexus_EventScript_Stay
	msgbox Nexus_Text_Voice
	msgbox Nexus_Text_Guardian1
	playse SE_WARP_IN
	warp MAP_VIRIDIAN_FOREST_GROVE, 10, 5
	waitstate
	releaseall
	end

Nexus_EventScript_Locked1::
	switch VAR_TEMP_2
	case 0, Nexus_EventScript_Smudge1
	case 1, Nexus_EventScript_Lines1
	msgbox Nexus_Text_Locked3
	releaseall
	end

Nexus_EventScript_Smudge1::
	setmetatile 7, 5, 0x1F3, TRUE
	setmetatile 8, 5, 0x1F4, TRUE
	setmetatile 7, 6, 0x1F5, TRUE
	setmetatile 8, 6, 0x1F6, TRUE
	special DrawWholeMapView
	setvar VAR_TEMP_2, 1
	msgbox Nexus_Text_Locked1
	releaseall
	end

Nexus_EventScript_Lines1::
	setmetatile 7, 5, 0x1EF, TRUE
	setmetatile 8, 5, 0x1F0, TRUE
	setmetatile 7, 6, 0x1F1, TRUE
	setmetatile 8, 6, 0x1F2, TRUE
	special DrawWholeMapView
	setvar VAR_TEMP_2, 2
	msgbox Nexus_Text_Locked2
	releaseall
	end

@ Route24_Grove
Nexus_EventScript_Door2::
	lockall
	goto_if_unset FLAG_ROUTE24_GROVE_FOUND, Nexus_EventScript_Locked2
	msgbox Nexus_Text_DoorOpen, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, Nexus_EventScript_Stay
	msgbox Nexus_Text_Voice
	msgbox Nexus_Text_Guardian2
	playse SE_WARP_IN
	warp MAP_ROUTE24_GROVE, 11, 4
	waitstate
	releaseall
	end

Nexus_EventScript_Locked2::
	switch VAR_TEMP_3
	case 0, Nexus_EventScript_Smudge2
	case 1, Nexus_EventScript_Lines2
	msgbox Nexus_Text_Locked3
	releaseall
	end

Nexus_EventScript_Smudge2::
	setmetatile 3, 8, 0x203, TRUE
	setmetatile 4, 8, 0x204, TRUE
	setmetatile 3, 9, 0x205, TRUE
	setmetatile 4, 9, 0x206, TRUE
	special DrawWholeMapView
	setvar VAR_TEMP_3, 1
	msgbox Nexus_Text_Locked1
	releaseall
	end

Nexus_EventScript_Lines2::
	setmetatile 3, 8, 0x1FF, TRUE
	setmetatile 4, 8, 0x200, TRUE
	setmetatile 3, 9, 0x201, TRUE
	setmetatile 4, 9, 0x202, TRUE
	special DrawWholeMapView
	setvar VAR_TEMP_3, 2
	msgbox Nexus_Text_Locked2
	releaseall
	end

@ Route25_Grove
Nexus_EventScript_Door3::
	lockall
	goto_if_unset FLAG_ROUTE25_GROVE_FOUND, Nexus_EventScript_Locked3
	msgbox Nexus_Text_DoorOpen, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, Nexus_EventScript_Stay
	msgbox Nexus_Text_Voice
	msgbox Nexus_Text_Guardian3
	playse SE_WARP_IN
	warp MAP_ROUTE25_GROVE, 11, 4
	waitstate
	releaseall
	end

Nexus_EventScript_Locked3::
	switch VAR_TEMP_4
	case 0, Nexus_EventScript_Smudge3
	case 1, Nexus_EventScript_Lines3
	msgbox Nexus_Text_Locked3
	releaseall
	end

Nexus_EventScript_Smudge3::
	setmetatile 7, 10, 0x213, TRUE
	setmetatile 8, 10, 0x214, TRUE
	setmetatile 7, 11, 0x215, TRUE
	setmetatile 8, 11, 0x216, TRUE
	special DrawWholeMapView
	setvar VAR_TEMP_4, 1
	msgbox Nexus_Text_Locked1
	releaseall
	end

Nexus_EventScript_Lines3::
	setmetatile 7, 10, 0x20F, TRUE
	setmetatile 8, 10, 0x210, TRUE
	setmetatile 7, 11, 0x211, TRUE
	setmetatile 8, 11, 0x212, TRUE
	special DrawWholeMapView
	setvar VAR_TEMP_4, 2
	msgbox Nexus_Text_Locked2
	releaseall
	end

@ Route11_Grove
Nexus_EventScript_Door4::
	lockall
	goto_if_unset FLAG_ROUTE11_GROVE_FOUND, Nexus_EventScript_Locked4
	msgbox Nexus_Text_DoorOpen, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, Nexus_EventScript_Stay
	msgbox Nexus_Text_Voice
	msgbox Nexus_Text_Guardian4
	playse SE_WARP_IN
	warp MAP_ROUTE11_GROVE, 11, 4
	waitstate
	releaseall
	end

Nexus_EventScript_Locked4::
	switch VAR_TEMP_5
	case 0, Nexus_EventScript_Smudge4
	case 1, Nexus_EventScript_Lines4
	msgbox Nexus_Text_Locked3
	releaseall
	end

Nexus_EventScript_Smudge4::
	setmetatile 3, 13, 0x223, TRUE
	setmetatile 4, 13, 0x224, TRUE
	setmetatile 3, 14, 0x225, TRUE
	setmetatile 4, 14, 0x226, TRUE
	special DrawWholeMapView
	setvar VAR_TEMP_5, 1
	msgbox Nexus_Text_Locked1
	releaseall
	end

Nexus_EventScript_Lines4::
	setmetatile 3, 13, 0x21F, TRUE
	setmetatile 4, 13, 0x220, TRUE
	setmetatile 3, 14, 0x221, TRUE
	setmetatile 4, 14, 0x222, TRUE
	special DrawWholeMapView
	setvar VAR_TEMP_5, 2
	msgbox Nexus_Text_Locked2
	releaseall
	end

@ Route8_Grove
Nexus_EventScript_Door5::
	lockall
	goto_if_unset FLAG_ROUTE8_GROVE_FOUND, Nexus_EventScript_Locked5
	msgbox Nexus_Text_DoorOpen, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, Nexus_EventScript_Stay
	msgbox Nexus_Text_Voice
	msgbox Nexus_Text_Guardian5
	playse SE_WARP_IN
	warp MAP_ROUTE8_GROVE, 11, 4
	waitstate
	releaseall
	end

Nexus_EventScript_Locked5::
	switch VAR_TEMP_6
	case 0, Nexus_EventScript_Smudge5
	case 1, Nexus_EventScript_Lines5
	msgbox Nexus_Text_Locked3
	releaseall
	end

Nexus_EventScript_Smudge5::
	setmetatile 7, 15, 0x233, TRUE
	setmetatile 8, 15, 0x234, TRUE
	setmetatile 7, 16, 0x235, TRUE
	setmetatile 8, 16, 0x236, TRUE
	special DrawWholeMapView
	setvar VAR_TEMP_6, 1
	msgbox Nexus_Text_Locked1
	releaseall
	end

Nexus_EventScript_Lines5::
	setmetatile 7, 15, 0x22F, TRUE
	setmetatile 8, 15, 0x230, TRUE
	setmetatile 7, 16, 0x231, TRUE
	setmetatile 8, 16, 0x232, TRUE
	special DrawWholeMapView
	setvar VAR_TEMP_6, 2
	msgbox Nexus_Text_Locked2
	releaseall
	end

@ Route13_Grove
Nexus_EventScript_Door6::
	lockall
	goto_if_unset FLAG_ROUTE13_GROVE_FOUND, Nexus_EventScript_Locked6
	msgbox Nexus_Text_DoorOpen, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, Nexus_EventScript_Stay
	msgbox Nexus_Text_Voice
	msgbox Nexus_Text_Guardian6
	playse SE_WARP_IN
	warp MAP_ROUTE13_GROVE, 11, 4
	waitstate
	releaseall
	end

Nexus_EventScript_Locked6::
	switch VAR_TEMP_7
	case 0, Nexus_EventScript_Smudge6
	case 1, Nexus_EventScript_Lines6
	msgbox Nexus_Text_Locked3
	releaseall
	end

Nexus_EventScript_Smudge6::
	setmetatile 4, 18, 0x243, TRUE
	setmetatile 5, 18, 0x244, TRUE
	setmetatile 4, 19, 0x245, TRUE
	setmetatile 5, 19, 0x246, TRUE
	special DrawWholeMapView
	setvar VAR_TEMP_7, 1
	msgbox Nexus_Text_Locked1
	releaseall
	end

Nexus_EventScript_Lines6::
	setmetatile 4, 18, 0x23F, TRUE
	setmetatile 5, 18, 0x240, TRUE
	setmetatile 4, 19, 0x241, TRUE
	setmetatile 5, 19, 0x242, TRUE
	special DrawWholeMapView
	setvar VAR_TEMP_7, 2
	msgbox Nexus_Text_Locked2
	releaseall
	end

@ OneIsland_Grove
Nexus_EventScript_Door7::
	lockall
	goto_if_unset FLAG_ONE_ISLAND_GROVE_FOUND, Nexus_EventScript_Locked7
	msgbox Nexus_Text_DoorOpen, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, Nexus_EventScript_Stay
	msgbox Nexus_Text_Voice
	msgbox Nexus_Text_Guardian7
	playse SE_WARP_IN
	warp MAP_ONE_ISLAND_GROVE, 11, 4
	waitstate
	releaseall
	end

Nexus_EventScript_Locked7::
	switch VAR_TEMP_8
	case 0, Nexus_EventScript_Smudge7
	case 1, Nexus_EventScript_Lines7
	msgbox Nexus_Text_Locked3
	releaseall
	end

Nexus_EventScript_Smudge7::
	setmetatile 21, 3, 0x253, TRUE
	setmetatile 22, 3, 0x254, TRUE
	setmetatile 21, 4, 0x255, TRUE
	setmetatile 22, 4, 0x256, TRUE
	special DrawWholeMapView
	setvar VAR_TEMP_8, 1
	msgbox Nexus_Text_Locked1
	releaseall
	end

Nexus_EventScript_Lines7::
	setmetatile 21, 3, 0x24F, TRUE
	setmetatile 22, 3, 0x250, TRUE
	setmetatile 21, 4, 0x251, TRUE
	setmetatile 22, 4, 0x252, TRUE
	special DrawWholeMapView
	setvar VAR_TEMP_8, 2
	msgbox Nexus_Text_Locked2
	releaseall
	end

@ TwoIsland_Grove
Nexus_EventScript_Door8::
	lockall
	goto_if_unset FLAG_TWO_ISLAND_GROVE_FOUND, Nexus_EventScript_Locked8
	msgbox Nexus_Text_DoorOpen, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, Nexus_EventScript_Stay
	msgbox Nexus_Text_Voice
	msgbox Nexus_Text_Guardian8
	playse SE_WARP_IN
	warp MAP_TWO_ISLAND_GROVE, 11, 4
	waitstate
	releaseall
	end

Nexus_EventScript_Locked8::
	switch VAR_TEMP_9
	case 0, Nexus_EventScript_Smudge8
	case 1, Nexus_EventScript_Lines8
	msgbox Nexus_Text_Locked3
	releaseall
	end

Nexus_EventScript_Smudge8::
	setmetatile 17, 5, 0x263, TRUE
	setmetatile 18, 5, 0x264, TRUE
	setmetatile 17, 6, 0x265, TRUE
	setmetatile 18, 6, 0x266, TRUE
	special DrawWholeMapView
	setvar VAR_TEMP_9, 1
	msgbox Nexus_Text_Locked1
	releaseall
	end

Nexus_EventScript_Lines8::
	setmetatile 17, 5, 0x25F, TRUE
	setmetatile 18, 5, 0x260, TRUE
	setmetatile 17, 6, 0x261, TRUE
	setmetatile 18, 6, 0x262, TRUE
	special DrawWholeMapView
	setvar VAR_TEMP_9, 2
	msgbox Nexus_Text_Locked2
	releaseall
	end

@ ThreeIsland_BerryForest_Grove
Nexus_EventScript_Door9::
	lockall
	goto_if_unset FLAG_THREE_ISLAND_BERRY_FOREST_GROVE_FOUND, Nexus_EventScript_Locked9
	msgbox Nexus_Text_DoorOpen, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, Nexus_EventScript_Stay
	msgbox Nexus_Text_Voice
	msgbox Nexus_Text_Guardian9
	playse SE_WARP_IN
	warp MAP_THREE_ISLAND_BERRY_FOREST_GROVE, 11, 4
	waitstate
	releaseall
	end

Nexus_EventScript_Locked9::
	switch VAR_TEMP_A
	case 0, Nexus_EventScript_Smudge9
	case 1, Nexus_EventScript_Lines9
	msgbox Nexus_Text_Locked3
	releaseall
	end

Nexus_EventScript_Smudge9::
	setmetatile 21, 8, 0x273, TRUE
	setmetatile 22, 8, 0x274, TRUE
	setmetatile 21, 9, 0x275, TRUE
	setmetatile 22, 9, 0x276, TRUE
	special DrawWholeMapView
	setvar VAR_TEMP_A, 1
	msgbox Nexus_Text_Locked1
	releaseall
	end

Nexus_EventScript_Lines9::
	setmetatile 21, 8, 0x26F, TRUE
	setmetatile 22, 8, 0x270, TRUE
	setmetatile 21, 9, 0x271, TRUE
	setmetatile 22, 9, 0x272, TRUE
	special DrawWholeMapView
	setvar VAR_TEMP_A, 2
	msgbox Nexus_Text_Locked2
	releaseall
	end

@ FourIsland_Grove
Nexus_EventScript_Door10::
	lockall
	goto_if_unset FLAG_FOUR_ISLAND_GROVE_FOUND, Nexus_EventScript_Locked10
	msgbox Nexus_Text_DoorOpen, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, Nexus_EventScript_Stay
	msgbox Nexus_Text_Voice
	msgbox Nexus_Text_Guardian10
	playse SE_WARP_IN
	warp MAP_FOUR_ISLAND_GROVE, 11, 4
	waitstate
	releaseall
	end

Nexus_EventScript_Locked10::
	switch VAR_TEMP_B
	case 0, Nexus_EventScript_Smudge10
	case 1, Nexus_EventScript_Lines10
	msgbox Nexus_Text_Locked3
	releaseall
	end

Nexus_EventScript_Smudge10::
	setmetatile 17, 10, 0x283, TRUE
	setmetatile 18, 10, 0x284, TRUE
	setmetatile 17, 11, 0x285, TRUE
	setmetatile 18, 11, 0x286, TRUE
	special DrawWholeMapView
	setvar VAR_TEMP_B, 1
	msgbox Nexus_Text_Locked1
	releaseall
	end

Nexus_EventScript_Lines10::
	setmetatile 17, 10, 0x27F, TRUE
	setmetatile 18, 10, 0x280, TRUE
	setmetatile 17, 11, 0x281, TRUE
	setmetatile 18, 11, 0x282, TRUE
	special DrawWholeMapView
	setvar VAR_TEMP_B, 2
	msgbox Nexus_Text_Locked2
	releaseall
	end

@ FiveIsland_Meadow_Grove
Nexus_EventScript_Door11::
	lockall
	goto_if_unset FLAG_FIVE_ISLAND_MEADOW_GROVE_FOUND, Nexus_EventScript_Locked11
	msgbox Nexus_Text_DoorOpen, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, Nexus_EventScript_Stay
	msgbox Nexus_Text_Voice
	msgbox Nexus_Text_Guardian11
	playse SE_WARP_IN
	warp MAP_FIVE_ISLAND_MEADOW_GROVE, 11, 4
	waitstate
	releaseall
	end

Nexus_EventScript_Locked11::
	switch VAR_TEMP_C
	case 0, Nexus_EventScript_Smudge11
	case 1, Nexus_EventScript_Lines11
	msgbox Nexus_Text_Locked3
	releaseall
	end

Nexus_EventScript_Smudge11::
	setmetatile 21, 13, 0x293, TRUE
	setmetatile 22, 13, 0x294, TRUE
	setmetatile 21, 14, 0x295, TRUE
	setmetatile 22, 14, 0x296, TRUE
	special DrawWholeMapView
	setvar VAR_TEMP_C, 1
	msgbox Nexus_Text_Locked1
	releaseall
	end

Nexus_EventScript_Lines11::
	setmetatile 21, 13, 0x28F, TRUE
	setmetatile 22, 13, 0x290, TRUE
	setmetatile 21, 14, 0x291, TRUE
	setmetatile 22, 14, 0x292, TRUE
	special DrawWholeMapView
	setvar VAR_TEMP_C, 2
	msgbox Nexus_Text_Locked2
	releaseall
	end

@ SixIsland_WaterPath_Grove
Nexus_EventScript_Door12::
	lockall
	goto_if_unset FLAG_SIX_ISLAND_WATER_PATH_GROVE_FOUND, Nexus_EventScript_Locked12
	msgbox Nexus_Text_DoorOpen, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, Nexus_EventScript_Stay
	msgbox Nexus_Text_Voice
	msgbox Nexus_Text_Guardian12
	playse SE_WARP_IN
	warp MAP_SIX_ISLAND_WATER_PATH_GROVE, 11, 4
	waitstate
	releaseall
	end

Nexus_EventScript_Locked12::
	switch VAR_TEMP_D
	case 0, Nexus_EventScript_Smudge12
	case 1, Nexus_EventScript_Lines12
	msgbox Nexus_Text_Locked3
	releaseall
	end

Nexus_EventScript_Smudge12::
	setmetatile 17, 15, 0x2A3, TRUE
	setmetatile 18, 15, 0x2A4, TRUE
	setmetatile 17, 16, 0x2A5, TRUE
	setmetatile 18, 16, 0x2A6, TRUE
	special DrawWholeMapView
	setvar VAR_TEMP_D, 1
	msgbox Nexus_Text_Locked1
	releaseall
	end

Nexus_EventScript_Lines12::
	setmetatile 17, 15, 0x29F, TRUE
	setmetatile 18, 15, 0x2A0, TRUE
	setmetatile 17, 16, 0x2A1, TRUE
	setmetatile 18, 16, 0x2A2, TRUE
	special DrawWholeMapView
	setvar VAR_TEMP_D, 2
	msgbox Nexus_Text_Locked2
	releaseall
	end

@ SevenIsland_Grove
Nexus_EventScript_Door13::
	lockall
	goto_if_unset FLAG_SEVEN_ISLAND_GROVE_FOUND, Nexus_EventScript_Locked13
	msgbox Nexus_Text_DoorOpen, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, Nexus_EventScript_Stay
	msgbox Nexus_Text_Voice
	msgbox Nexus_Text_Guardian13
	playse SE_WARP_IN
	warp MAP_SEVEN_ISLAND_GROVE, 11, 4
	waitstate
	releaseall
	end

Nexus_EventScript_Locked13::
	switch VAR_TEMP_E
	case 0, Nexus_EventScript_Smudge13
	case 1, Nexus_EventScript_Lines13
	msgbox Nexus_Text_Locked3
	releaseall
	end

Nexus_EventScript_Smudge13::
	setmetatile 20, 18, 0x2B3, TRUE
	setmetatile 21, 18, 0x2B4, TRUE
	setmetatile 20, 19, 0x2B5, TRUE
	setmetatile 21, 19, 0x2B6, TRUE
	special DrawWholeMapView
	setvar VAR_TEMP_E, 1
	msgbox Nexus_Text_Locked1
	releaseall
	end

Nexus_EventScript_Lines13::
	setmetatile 20, 18, 0x2AF, TRUE
	setmetatile 21, 18, 0x2B0, TRUE
	setmetatile 20, 19, 0x2B1, TRUE
	setmetatile 21, 19, 0x2B2, TRUE
	special DrawWholeMapView
	setvar VAR_TEMP_E, 2
	msgbox Nexus_Text_Locked2
	releaseall
	end

Nexus_EventScript_Stay::
	releaseall
	end

@ T-493 (DRAFT): the twin of the S.S. ANNE's painting, facing east, so the engine draws it mirrored. With the
@ painting's scrambled words in the NOTEBOOK, standing before it files the page that reads them.
Nexus_EventScript_Easel::
	lockall
	msgbox Nexus_Text_Easel
	goto_if_set FLAG_NOTEBOOK_LOOSE_PAINTING_READ, Nexus_EventScript_EaselWords
	goto_if_unset FLAG_NOTEBOOK_LOOSE_PAINTING, Nexus_EventScript_EaselDone
	goto_if_unset FLAG_GOT_NOTEBOOK, Nexus_EventScript_EaselDone
	msgbox Nexus_Text_EaselTwin
	clearflag FLAG_NOTEBOOK_LOOSE_PAINTING
	setflag FLAG_NOTEBOOK_LOOSE_PAINTING_READ
	playse SE_SELECT
Nexus_EventScript_EaselWords::
	msgbox Nexus_Text_EaselWords
Nexus_EventScript_EaselDone::
	releaseall
	end

@ The one Christmas tree: the way home, to the S.S. ANNE's pier.
Nexus_EventScript_ChristmasTree::
	lockall
	setvar VAR_0x8004, TRUE
	special SingingFir_Sing
	waitstate
	msgbox Nexus_Text_Christmas, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, Nexus_EventScript_Stay
	msgbox Nexus_Text_Voice
	msgbox Nexus_Text_GuardianHome
	playse SE_WARP_IN
	warp MAP_SSANNE_EXTERIOR, 57, 27
	waitstate
	releaseall
	end

@ THE (NOT A) HUNTER, a bear in red plaid.
Nexus_EventScript_Hunter::
	lock
	faceplayer
	goto_if_set FLAG_NEXUS_HUNTER_MET, Nexus_EventScript_HunterAgain
	setflag FLAG_NEXUS_HUNTER_MET
	msgbox Nexus_Text_HunterFirst
	release
	end

Nexus_EventScript_HunterAgain::
	goto_if_unset FLAG_ROUTE1_GROVE_FOUND, Nexus_EventScript_HunterNotAll
	goto_if_unset FLAG_VIRIDIAN_FOREST_GROVE_FOUND, Nexus_EventScript_HunterNotAll
	goto_if_unset FLAG_ROUTE24_GROVE_FOUND, Nexus_EventScript_HunterNotAll
	goto_if_unset FLAG_ROUTE25_GROVE_FOUND, Nexus_EventScript_HunterNotAll
	goto_if_unset FLAG_ROUTE11_GROVE_FOUND, Nexus_EventScript_HunterNotAll
	goto_if_unset FLAG_ROUTE8_GROVE_FOUND, Nexus_EventScript_HunterNotAll
	goto_if_unset FLAG_ROUTE13_GROVE_FOUND, Nexus_EventScript_HunterNotAll
	goto_if_unset FLAG_ONE_ISLAND_GROVE_FOUND, Nexus_EventScript_HunterNotAll
	goto_if_unset FLAG_TWO_ISLAND_GROVE_FOUND, Nexus_EventScript_HunterNotAll
	goto_if_unset FLAG_THREE_ISLAND_BERRY_FOREST_GROVE_FOUND, Nexus_EventScript_HunterNotAll
	goto_if_unset FLAG_FOUR_ISLAND_GROVE_FOUND, Nexus_EventScript_HunterNotAll
	goto_if_unset FLAG_FIVE_ISLAND_MEADOW_GROVE_FOUND, Nexus_EventScript_HunterNotAll
	goto_if_unset FLAG_SIX_ISLAND_WATER_PATH_GROVE_FOUND, Nexus_EventScript_HunterNotAll
	goto_if_unset FLAG_SEVEN_ISLAND_GROVE_FOUND, Nexus_EventScript_HunterNotAll
	msgbox Nexus_Text_HunterAll
	release
	end

Nexus_EventScript_HunterNotAll::
	msgbox Nexus_Text_Hunter
	release
	end

@ PHOENIX, once, waiting in the paint.
Nexus_EventScript_Phoenix::
	goto_if_questlog EventScript_ReleaseEnd
	special QuestLog_CutRecording
	lock
	faceplayer
	setwildbattle SPECIES_HO_OH, 35
	waitse
	playmoncry SPECIES_HO_OH, CRY_MODE_ENCOUNTER
	message Nexus_Text_Phoenix
	waitmessage
	waitmoncry
	delay 10
	playbgm MUS_ENCOUNTER_GYM_LEADER, 0
	waitbuttonpress
	setflag FLAG_SYS_SPECIAL_WILD_BATTLE
	special StartLegendaryBattle
	waitstate
	clearflag FLAG_SYS_SPECIAL_WILD_BATTLE
	specialvar VAR_RESULT, GetBattleOutcome
	goto_if_eq VAR_RESULT, B_OUTCOME_WON, Nexus_EventScript_PhoenixGone
	goto_if_eq VAR_RESULT, B_OUTCOME_RAN, Nexus_EventScript_PhoenixFlew
	goto_if_eq VAR_RESULT, B_OUTCOME_PLAYER_TELEPORTED, Nexus_EventScript_PhoenixFlew
	release
	end

Nexus_EventScript_PhoenixGone::
	goto EventScript_RemoveStaticMon
	end

Nexus_EventScript_PhoenixFlew::
	setvar VAR_0x8004, SPECIES_HO_OH
	goto EventScript_MonFlewAway
	end

@ ------------------------------------------------------------------------------------------------
@ THE SINGING FIR, everywhere it stands: every grove (tools/gbagrove.py) and the S.S. ANNE's pier. Flat
@ until the band's sheet gives it its key; then a door into THE NEXUS. The fir before the key and the fir
@ after it are two objects on one tile, the bright one the next local id: swapped here, the first time.
EventScript_SingingFir::
	lock
	goto_if_set FLAG_FIR_IN_KEY, EventScript_SingingFir_InKey
	msgbox Text_SingingFir_Hums
	setvar VAR_0x8004, FALSE
	special SingingFir_Sing
	waitstate
	goto_if_set FLAG_NOTEBOOK_LOOSE_KEY, EventScript_SingingFir_GivenKey
	msgbox Text_SingingFir_Off
	release
	end

EventScript_SingingFir_GivenKey::
	msgbox Text_SingingFir_HoldUpSheet
	setvar VAR_0x8004, TRUE
	special SingingFir_Sing
	waitstate
	setflag FLAG_FIR_IN_KEY
	clearflag FLAG_HIDE_FIR_UNKEYED
	copyvar VAR_0x8005, VAR_LAST_TALKED
	removeobject VAR_0x8005
	addvar VAR_0x8005, 1
	addobject VAR_0x8005
	msgbox Text_SingingFir_Right
	goto EventScript_SingingFir_Door

EventScript_SingingFir_InKey::
	msgbox Text_SingingFir_Hums
	setvar VAR_0x8004, TRUE
	special SingingFir_Sing
	waitstate
EventScript_SingingFir_Door::
	msgbox Text_SingingFir_Door, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, EventScript_SingingFir_Stay
	call_if_unset FLAG_NEXUS_VISITED, EventScript_SingingFir_FirstTime
	playse SE_WARP_IN
	warp MAP_NEXUS, 12, 5
	waitstate
	release
	end

EventScript_SingingFir_FirstTime::
	msgbox Text_SingingFir_FirstTime
	return

EventScript_SingingFir_Stay::
	release
	end

@ On every arrival where a fir stands: the bright one stays hidden until the key.
EventScript_FirKeyFlags::
	goto_if_set FLAG_FIR_IN_KEY, EventScript_FirKeyFlags_Keyed
	setflag FLAG_HIDE_FIR_UNKEYED
	return

EventScript_FirKeyFlags_Keyed::
	clearflag FLAG_HIDE_FIR_UNKEYED
	return

Nexus_EventScript_Sign0::
	msgbox Nexus_Text_Sign0, MSGBOX_SIGN
	end

Nexus_EventScript_Sign1::
	msgbox Nexus_Text_Sign1, MSGBOX_SIGN
	end

Nexus_EventScript_Sign2::
	msgbox Nexus_Text_Sign2, MSGBOX_SIGN
	end

Nexus_EventScript_Sign3::
	msgbox Nexus_Text_Sign3, MSGBOX_SIGN
	end

Nexus_EventScript_Sign4::
	msgbox Nexus_Text_Sign4, MSGBOX_SIGN
	end

Nexus_EventScript_Sign5::
	msgbox Nexus_Text_Sign5, MSGBOX_SIGN
	end

Nexus_EventScript_Sign6::
	msgbox Nexus_Text_Sign6, MSGBOX_SIGN
	end

Nexus_EventScript_Sign7::
	msgbox Nexus_Text_Sign7, MSGBOX_SIGN
	end

Nexus_EventScript_Sign8::
	msgbox Nexus_Text_Sign8, MSGBOX_SIGN
	end

Nexus_EventScript_Sign9::
	msgbox Nexus_Text_Sign9, MSGBOX_SIGN
	end

Nexus_EventScript_Sign10::
	msgbox Nexus_Text_Sign10, MSGBOX_SIGN
	end

Nexus_EventScript_Sign11::
	msgbox Nexus_Text_Sign11, MSGBOX_SIGN
	end

Nexus_EventScript_Sign12::
	msgbox Nexus_Text_Sign12, MSGBOX_SIGN
	end

Nexus_EventScript_Sign13::
	msgbox Nexus_Text_Sign13, MSGBOX_SIGN
	end
