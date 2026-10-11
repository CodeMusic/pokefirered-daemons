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
	setmetatile 3, 3, 0x2AF, TRUE
	setmetatile 4, 3, 0x2B0, TRUE
	setmetatile 3, 4, 0x2B1, TRUE
	setmetatile 4, 4, 0x2B2, TRUE
	return

Nexus_EventScript_Found1::
	setmetatile 7, 5, 0x2BF, TRUE
	setmetatile 8, 5, 0x2C0, TRUE
	setmetatile 7, 6, 0x2C1, TRUE
	setmetatile 8, 6, 0x2C2, TRUE
	return

Nexus_EventScript_Found2::
	setmetatile 3, 8, 0x2CF, TRUE
	setmetatile 4, 8, 0x2D0, TRUE
	setmetatile 3, 9, 0x2D1, TRUE
	setmetatile 4, 9, 0x2D2, TRUE
	return

Nexus_EventScript_Found3::
	setmetatile 7, 10, 0x2DF, TRUE
	setmetatile 8, 10, 0x2E0, TRUE
	setmetatile 7, 11, 0x2E1, TRUE
	setmetatile 8, 11, 0x2E2, TRUE
	return

Nexus_EventScript_Found4::
	setmetatile 3, 13, 0x2BF, TRUE
	setmetatile 4, 13, 0x2C0, TRUE
	setmetatile 3, 14, 0x2C1, TRUE
	setmetatile 4, 14, 0x2C2, TRUE
	return

Nexus_EventScript_Found5::
	setmetatile 7, 15, 0x2AF, TRUE
	setmetatile 8, 15, 0x2B0, TRUE
	setmetatile 7, 16, 0x2B1, TRUE
	setmetatile 8, 16, 0x2B2, TRUE
	return

Nexus_EventScript_Found6::
	setmetatile 4, 18, 0x2EF, TRUE
	setmetatile 5, 18, 0x2F0, TRUE
	setmetatile 4, 19, 0x2F1, TRUE
	setmetatile 5, 19, 0x2F2, TRUE
	return

Nexus_EventScript_Found7::
	setmetatile 21, 3, 0x2FF, TRUE
	setmetatile 22, 3, 0x300, TRUE
	setmetatile 21, 4, 0x301, TRUE
	setmetatile 22, 4, 0x302, TRUE
	return

Nexus_EventScript_Found8::
	setmetatile 17, 5, 0x30F, TRUE
	setmetatile 18, 5, 0x310, TRUE
	setmetatile 17, 6, 0x311, TRUE
	setmetatile 18, 6, 0x312, TRUE
	return

Nexus_EventScript_Found9::
	setmetatile 21, 8, 0x31F, TRUE
	setmetatile 22, 8, 0x320, TRUE
	setmetatile 21, 9, 0x321, TRUE
	setmetatile 22, 9, 0x322, TRUE
	return

Nexus_EventScript_Found10::
	setmetatile 17, 10, 0x32F, TRUE
	setmetatile 18, 10, 0x330, TRUE
	setmetatile 17, 11, 0x331, TRUE
	setmetatile 18, 11, 0x332, TRUE
	return

Nexus_EventScript_Found11::
	setmetatile 21, 13, 0x30F, TRUE
	setmetatile 22, 13, 0x310, TRUE
	setmetatile 21, 14, 0x311, TRUE
	setmetatile 22, 14, 0x312, TRUE
	return

Nexus_EventScript_Found12::
	setmetatile 17, 15, 0x2FF, TRUE
	setmetatile 18, 15, 0x300, TRUE
	setmetatile 17, 16, 0x301, TRUE
	setmetatile 18, 16, 0x302, TRUE
	return

Nexus_EventScript_Found13::
	setmetatile 20, 18, 0x2EF, TRUE
	setmetatile 21, 18, 0x2F0, TRUE
	setmetatile 20, 19, 0x2F1, TRUE
	setmetatile 21, 19, 0x2F2, TRUE
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
	setmetatile 3, 3, 0x2B7, TRUE
	setmetatile 4, 3, 0x2B8, TRUE
	setmetatile 3, 4, 0x2B9, TRUE
	setmetatile 4, 4, 0x2BA, TRUE
	special DrawWholeMapView
	setvar VAR_TEMP_1, 1
	msgbox Nexus_Text_Locked1
	releaseall
	end

Nexus_EventScript_Lines0::
	setmetatile 3, 3, 0x2B3, TRUE
	setmetatile 4, 3, 0x2B4, TRUE
	setmetatile 3, 4, 0x2B5, TRUE
	setmetatile 4, 4, 0x2B6, TRUE
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
	setmetatile 7, 5, 0x2C7, TRUE
	setmetatile 8, 5, 0x2C8, TRUE
	setmetatile 7, 6, 0x2C9, TRUE
	setmetatile 8, 6, 0x2CA, TRUE
	special DrawWholeMapView
	setvar VAR_TEMP_2, 1
	msgbox Nexus_Text_Locked1
	releaseall
	end

Nexus_EventScript_Lines1::
	setmetatile 7, 5, 0x2C3, TRUE
	setmetatile 8, 5, 0x2C4, TRUE
	setmetatile 7, 6, 0x2C5, TRUE
	setmetatile 8, 6, 0x2C6, TRUE
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
	setmetatile 3, 8, 0x2D7, TRUE
	setmetatile 4, 8, 0x2D8, TRUE
	setmetatile 3, 9, 0x2D9, TRUE
	setmetatile 4, 9, 0x2DA, TRUE
	special DrawWholeMapView
	setvar VAR_TEMP_3, 1
	msgbox Nexus_Text_Locked1
	releaseall
	end

Nexus_EventScript_Lines2::
	setmetatile 3, 8, 0x2D3, TRUE
	setmetatile 4, 8, 0x2D4, TRUE
	setmetatile 3, 9, 0x2D5, TRUE
	setmetatile 4, 9, 0x2D6, TRUE
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
	setmetatile 7, 10, 0x2E7, TRUE
	setmetatile 8, 10, 0x2E8, TRUE
	setmetatile 7, 11, 0x2E9, TRUE
	setmetatile 8, 11, 0x2EA, TRUE
	special DrawWholeMapView
	setvar VAR_TEMP_4, 1
	msgbox Nexus_Text_Locked1
	releaseall
	end

Nexus_EventScript_Lines3::
	setmetatile 7, 10, 0x2E3, TRUE
	setmetatile 8, 10, 0x2E4, TRUE
	setmetatile 7, 11, 0x2E5, TRUE
	setmetatile 8, 11, 0x2E6, TRUE
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
	setmetatile 3, 13, 0x2C7, TRUE
	setmetatile 4, 13, 0x2C8, TRUE
	setmetatile 3, 14, 0x2C9, TRUE
	setmetatile 4, 14, 0x2CA, TRUE
	special DrawWholeMapView
	setvar VAR_TEMP_5, 1
	msgbox Nexus_Text_Locked1
	releaseall
	end

Nexus_EventScript_Lines4::
	setmetatile 3, 13, 0x2C3, TRUE
	setmetatile 4, 13, 0x2C4, TRUE
	setmetatile 3, 14, 0x2C5, TRUE
	setmetatile 4, 14, 0x2C6, TRUE
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
	setmetatile 7, 15, 0x2B7, TRUE
	setmetatile 8, 15, 0x2B8, TRUE
	setmetatile 7, 16, 0x2B9, TRUE
	setmetatile 8, 16, 0x2BA, TRUE
	special DrawWholeMapView
	setvar VAR_TEMP_6, 1
	msgbox Nexus_Text_Locked1
	releaseall
	end

Nexus_EventScript_Lines5::
	setmetatile 7, 15, 0x2B3, TRUE
	setmetatile 8, 15, 0x2B4, TRUE
	setmetatile 7, 16, 0x2B5, TRUE
	setmetatile 8, 16, 0x2B6, TRUE
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
	setmetatile 4, 18, 0x2F7, TRUE
	setmetatile 5, 18, 0x2F8, TRUE
	setmetatile 4, 19, 0x2F9, TRUE
	setmetatile 5, 19, 0x2FA, TRUE
	special DrawWholeMapView
	setvar VAR_TEMP_7, 1
	msgbox Nexus_Text_Locked1
	releaseall
	end

Nexus_EventScript_Lines6::
	setmetatile 4, 18, 0x2F3, TRUE
	setmetatile 5, 18, 0x2F4, TRUE
	setmetatile 4, 19, 0x2F5, TRUE
	setmetatile 5, 19, 0x2F6, TRUE
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
	setmetatile 21, 3, 0x307, TRUE
	setmetatile 22, 3, 0x308, TRUE
	setmetatile 21, 4, 0x309, TRUE
	setmetatile 22, 4, 0x30A, TRUE
	special DrawWholeMapView
	setvar VAR_TEMP_8, 1
	msgbox Nexus_Text_Locked1
	releaseall
	end

Nexus_EventScript_Lines7::
	setmetatile 21, 3, 0x303, TRUE
	setmetatile 22, 3, 0x304, TRUE
	setmetatile 21, 4, 0x305, TRUE
	setmetatile 22, 4, 0x306, TRUE
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
	setmetatile 17, 5, 0x317, TRUE
	setmetatile 18, 5, 0x318, TRUE
	setmetatile 17, 6, 0x319, TRUE
	setmetatile 18, 6, 0x31A, TRUE
	special DrawWholeMapView
	setvar VAR_TEMP_9, 1
	msgbox Nexus_Text_Locked1
	releaseall
	end

Nexus_EventScript_Lines8::
	setmetatile 17, 5, 0x313, TRUE
	setmetatile 18, 5, 0x314, TRUE
	setmetatile 17, 6, 0x315, TRUE
	setmetatile 18, 6, 0x316, TRUE
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
	setmetatile 21, 8, 0x327, TRUE
	setmetatile 22, 8, 0x328, TRUE
	setmetatile 21, 9, 0x329, TRUE
	setmetatile 22, 9, 0x32A, TRUE
	special DrawWholeMapView
	setvar VAR_TEMP_A, 1
	msgbox Nexus_Text_Locked1
	releaseall
	end

Nexus_EventScript_Lines9::
	setmetatile 21, 8, 0x323, TRUE
	setmetatile 22, 8, 0x324, TRUE
	setmetatile 21, 9, 0x325, TRUE
	setmetatile 22, 9, 0x326, TRUE
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
	setmetatile 17, 10, 0x337, TRUE
	setmetatile 18, 10, 0x338, TRUE
	setmetatile 17, 11, 0x339, TRUE
	setmetatile 18, 11, 0x33A, TRUE
	special DrawWholeMapView
	setvar VAR_TEMP_B, 1
	msgbox Nexus_Text_Locked1
	releaseall
	end

Nexus_EventScript_Lines10::
	setmetatile 17, 10, 0x333, TRUE
	setmetatile 18, 10, 0x334, TRUE
	setmetatile 17, 11, 0x335, TRUE
	setmetatile 18, 11, 0x336, TRUE
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
	setmetatile 21, 13, 0x317, TRUE
	setmetatile 22, 13, 0x318, TRUE
	setmetatile 21, 14, 0x319, TRUE
	setmetatile 22, 14, 0x31A, TRUE
	special DrawWholeMapView
	setvar VAR_TEMP_C, 1
	msgbox Nexus_Text_Locked1
	releaseall
	end

Nexus_EventScript_Lines11::
	setmetatile 21, 13, 0x313, TRUE
	setmetatile 22, 13, 0x314, TRUE
	setmetatile 21, 14, 0x315, TRUE
	setmetatile 22, 14, 0x316, TRUE
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
	setmetatile 17, 15, 0x307, TRUE
	setmetatile 18, 15, 0x308, TRUE
	setmetatile 17, 16, 0x309, TRUE
	setmetatile 18, 16, 0x30A, TRUE
	special DrawWholeMapView
	setvar VAR_TEMP_D, 1
	msgbox Nexus_Text_Locked1
	releaseall
	end

Nexus_EventScript_Lines12::
	setmetatile 17, 15, 0x303, TRUE
	setmetatile 18, 15, 0x304, TRUE
	setmetatile 17, 16, 0x305, TRUE
	setmetatile 18, 16, 0x306, TRUE
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
	setmetatile 20, 18, 0x2F7, TRUE
	setmetatile 21, 18, 0x2F8, TRUE
	setmetatile 20, 19, 0x2F9, TRUE
	setmetatile 21, 19, 0x2FA, TRUE
	special DrawWholeMapView
	setvar VAR_TEMP_E, 1
	msgbox Nexus_Text_Locked1
	releaseall
	end

Nexus_EventScript_Lines13::
	setmetatile 20, 18, 0x2F3, TRUE
	setmetatile 21, 18, 0x2F4, TRUE
	setmetatile 20, 19, 0x2F5, TRUE
	setmetatile 21, 19, 0x2F6, TRUE
	special DrawWholeMapView
	setvar VAR_TEMP_E, 2
	msgbox Nexus_Text_Locked2
	releaseall
	end

Nexus_EventScript_Stay::
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
