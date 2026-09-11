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
	release
	end

SaffronCity_MrPsychicsHouse_EventScript_OwlWroteToThem::
	msgbox SaffronCity_MrPsychicsHouse_Text_OwlWroteToThem
	release
	end
