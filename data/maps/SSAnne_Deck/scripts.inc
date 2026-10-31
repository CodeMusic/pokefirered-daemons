SSAnne_Deck_MapScripts::
	.byte 0

SSAnne_Deck_EventScript_Youngster::
	msgbox SSAnne_Deck_Text_ShipDepartingSoon, MSGBOX_NPC
	end

SSAnne_Deck_EventScript_BaldingMan::
	msgbox SSAnne_Deck_Text_FeelSeasick, MSGBOX_NPC
	end

SSAnne_Deck_EventScript_Sailor::
	msgbox SSAnne_Deck_Text_ScrubbingDecksHardWork, MSGBOX_NPC
	end

SSAnne_Deck_EventScript_Edmond::
	trainerbattle_single TRAINER_SAILOR_EDMOND, SSAnne_Deck_Text_EdmondIntro, SSAnne_Deck_Text_EdmondDefeat
	msgbox SSAnne_Deck_Text_EdmondPostBattle, MSGBOX_AUTOCLOSE
	end

SSAnne_Deck_EventScript_Trevor::
	trainerbattle_single TRAINER_SAILOR_TREVOR, SSAnne_Deck_Text_TrevorIntro, SSAnne_Deck_Text_TrevorDefeat
	msgbox SSAnne_Deck_Text_TrevorPostBattle, MSGBOX_AUTOCLOSE
	end

@ T-494 and T-496 (DRAFT): the lioness on the deck, the last meeting -- the game she made, and a letter for the ship's
@ post. The ship sails; the answer comes home to MOM (PalletTown_PlayersHouse_1F_EventScript_MomLetterReply).
SSAnne_Deck_EventScript_Lioness::
	lock
	faceplayer
	goto_if_set FLAG_LIONESS_LETTER_SENT, SSAnne_Deck_EventScript_LionessAfter
	msgbox SSAnne_Deck_Text_LionessGame
	msgbox SSAnne_Deck_Text_LionessAsk, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, SSAnne_Deck_EventScript_LionessNotNow
	setflag FLAG_LIONESS_LETTER_SENT
	textcolor NPC_TEXT_COLOR_NEUTRAL
	playse SE_SELECT
	msgbox SSAnne_Deck_Text_LionessPosted
	call EventScript_RestorePrevTextColor
	msgbox SSAnne_Deck_Text_LionessThanks
	release
	end

SSAnne_Deck_EventScript_LionessNotNow::
	msgbox SSAnne_Deck_Text_LionessKeep
	release
	end

SSAnne_Deck_EventScript_LionessAfter::
	msgbox SSAnne_Deck_Text_LionessAfter
	release
	end
