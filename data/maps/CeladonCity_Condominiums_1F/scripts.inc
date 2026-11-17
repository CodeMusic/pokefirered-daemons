CeladonCity_Condominiums_1F_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, CeladonCity_Condominiums_1F_OnTransition
	.byte 0

@  T-578: the badger is here only with THE SUBTLE LAYER held, and only until the cups are poured.
CeladonCity_Condominiums_1F_OnTransition::
	setflag FLAG_HIDE_KINDRED_CELADON
	goto_if_unset FLAG_GOT_SUBTLE_LAYER, CeladonCity_Condominiums_1F_OnTransition_End
	goto_if_set FLAG_KINDRED_POURED, CeladonCity_Condominiums_1F_OnTransition_End
	clearflag FLAG_HIDE_KINDRED_CELADON
CeladonCity_Condominiums_1F_OnTransition_End:
	end

@  T-578, DRAFT: KINDRED, the two cups. Two poured from one pot, one handed to you, the other carried out to LAVENDER
@  TOWN; you drink yours. Nothing travels after, and nobody says so (craft rule 1).
CeladonCity_Condominiums_1F_EventScript_KindredBadger::
	lock
	faceplayer
	msgbox CeladonCity_Condominiums_1F_Text_KindredPours
	msgbox CeladonCity_Condominiums_1F_Text_KindredGoes
	fadescreen FADE_TO_BLACK
	removeobject LOCALID_CELADON_KINDRED_BADGER
	setflag FLAG_HIDE_KINDRED_CELADON
	setflag FLAG_KINDRED_POURED
	fadescreen FADE_FROM_BLACK
	msgbox CeladonCity_Condominiums_1F_Text_KindredDrink
	release
	end

CeladonCity_Condominiums_1F_EventScript_TeaWoman::
	lock
	faceplayer
	goto_if_set FLAG_TALKED_TO_TEA_LADY_AFTER_HOF, CeladonCity_Condominiums_1F_EventScript_TeaWomanAfterTea
	goto_if_set FLAG_SYS_GAME_CLEAR, CeladonCity_Condominiums_1F_EventScript_TeaWomanMentionDaisy
	goto_if_set FLAG_GOT_TEA, CeladonCity_Condominiums_1F_EventScript_TeaWomanAfterTea
	msgbox CeladonCity_Condominiums_1F_Text_TryThisDrinkInstead
	setflag FLAG_GOT_TEA
	giveitem ITEM_TEA
	goto_if_eq VAR_RESULT, FALSE, EventScript_BagIsFull
	msgbox CeladonCity_Condominiums_1F_Text_NothingBeatsThirstLikeTea
	release
	end

CeladonCity_Condominiums_1F_EventScript_TeaWomanAfterTea::
	msgbox CeladonCity_Condominiums_1F_Text_MyDearMonsKeepMeCompany
	release
	end

CeladonCity_Condominiums_1F_EventScript_TeaWomanMentionDaisy::
	famechecker FAMECHECKER_DAISY, 4
	setflag FLAG_TALKED_TO_TEA_LADY_AFTER_HOF
	msgbox CeladonCity_Condominiums_1F_Text_DaisyComesToBuyTea
	release
	end

CeladonCity_Condominiums_1F_EventScript_Meowth::
	lock
	faceplayer
	waitse
	playmoncry SPECIES_MEOWTH, CRY_MODE_NORMAL
	msgbox CeladonCity_Condominiums_1F_Text_Meowth
	waitmoncry
	release
	end

CeladonCity_Condominiums_1F_EventScript_Clefairy::
	lock
	faceplayer
	waitse
	playmoncry SPECIES_CLEFAIRY, CRY_MODE_NORMAL
	msgbox CeladonCity_Condominiums_1F_Text_Clefairy
	waitmoncry
	release
	end

CeladonCity_Condominiums_1F_EventScript_Nidoran::
	lock
	faceplayer
	waitse
	playmoncry SPECIES_NIDORAN_F, CRY_MODE_NORMAL
	msgbox CeladonCity_Condominiums_1F_Text_Nidoran
	waitmoncry
	release
	end

CeladonCity_Condominiums_1F_EventScript_SuiteSign::
	msgbox CeladonCity_Condominiums_1F_Text_ManagersSuite, MSGBOX_SIGN
	end
