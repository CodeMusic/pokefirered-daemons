SaffronCity_MrPsychicsHouse_MapScripts::
	.byte 0

SaffronCity_MrPsychicsHouse_EventScript_MrPsychic::
	lock
	faceplayer
	goto_if_set FLAG_OWL_CONCEDED, SaffronCity_MrPsychicsHouse_EventScript_OwlWroteToThem
	specialvar VAR_RESULT, DaemonsPartyHasStarr
	goto_if_eq VAR_RESULT, TRUE, SaffronCity_MrPsychicsHouse_EventScript_OwlScene
	goto_if_set FLAG_GOT_TM29_FROM_MR_PSYCHIC, SaffronCity_MrPsychicsHouse_EventScript_AlreadyGotTM29
	msgbox SaffronCity_MrPsychicsHouse_Text_YouWantedThis
	checkitemspace ITEM_TM29
	goto_if_eq VAR_RESULT, FALSE, SaffronCity_MrPsychicsHouse_EventScript_NoRoomForTM29
	giveitem_msg SaffronCity_MrPsychicsHouse_Text_ReceivedTM29FromMrPsychic, ITEM_TM29
	msgbox SaffronCity_MrPsychicsHouse_Text_ExplainTM29
	setflag FLAG_GOT_TM29_FROM_MR_PSYCHIC
	release
	end

SaffronCity_MrPsychicsHouse_EventScript_NoRoomForTM29::
	msgbox SaffronCity_MrPsychicsHouse_Text_YouveNoRoom
	release
	end

SaffronCity_MrPsychicsHouse_EventScript_AlreadyGotTM29::
	msgbox SaffronCity_MrPsychicsHouse_Text_ExplainTM29
	release
	end

@  T-28. 4.24 staged, and it is msgboxes and nothing else -- no item, no
@  document, no record. 4.34's first ruling. The flag exists so it does not
@  repeat and so T-29 knows it happened; nothing shows it to the player.
SaffronCity_MrPsychicsHouse_EventScript_OwlScene::
	msgbox SaffronCity_MrPsychicsHouse_Text_OwlSitDown
	msgbox SaffronCity_MrPsychicsHouse_Text_OwlThatHand
	msgbox SaffronCity_MrPsychicsHouse_Text_OwlThePosition
	msgbox SaffronCity_MrPsychicsHouse_Text_OwlThermostat
	msgbox SaffronCity_MrPsychicsHouse_Text_OwlSharedBias
	msgbox SaffronCity_MrPsychicsHouse_Text_OwlConcession
	msgbox SaffronCity_MrPsychicsHouse_Text_OwlSomethingInTheRoom
	setflag FLAG_OWL_CONCEDED
	goto SaffronCity_MrPsychicsHouse_EventScript_TryFlight
	end

SaffronCity_MrPsychicsHouse_EventScript_OwlWroteToThem::
	msgbox SaffronCity_MrPsychicsHouse_Text_OwlWroteToThem
	goto SaffronCity_MrPsychicsHouse_EventScript_TryFlight
	end

@  T-29. The flight, and it has TWO entry points on purpose. The package
@  comes from the Warehouse and the concession happens here, and nothing in
@  the game forces that order -- so a player who reaches the Owl without the
@  package is not stranded: the flight simply waits for the next time they
@  speak to him.
@
@  4.34: the argument comes before the reunion, because the argument is what
@  causes it. There is no takeoff to look at. It works out where she is and
@  it goes, which is what 4.14 says it does.
SaffronCity_MrPsychicsHouse_EventScript_TryFlight::
	goto_if_set FLAG_CRYSTAL_READ_PAYLOAD, SaffronCity_MrPsychicsHouse_EventScript_NoFlight
	checkitem ITEM_LETTER
	goto_if_eq VAR_RESULT, FALSE, SaffronCity_MrPsychicsHouse_EventScript_NoFlight
	closemessage
	delay 60
	fadescreen FADE_TO_BLACK
	warp MAP_BIRTH_ISLAND_EXTERIOR, 15, 22
	waitstate
	release
	end

SaffronCity_MrPsychicsHouse_EventScript_NoFlight::
	release
	end
