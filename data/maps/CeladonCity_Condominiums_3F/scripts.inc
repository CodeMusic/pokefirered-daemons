CeladonCity_Condominiums_3F_MapScripts::
	.byte 0

CeladonCity_Condominiums_3F_EventScript_Programmer::
	msgbox CeladonCity_Condominiums_3F_Text_ImTheProgrammer, MSGBOX_NPC
	end

@  T-165. PIXELBYTE is the model that drew this game's daemons, and this is
@  the room where the game was made -- so the artist who "drew you" hands
@  over the thing that actually did. Once; the flag is ours (flags.h).
CeladonCity_Condominiums_3F_EventScript_GraphicArtist::
	lock
	faceplayer
	goto_if_set FLAG_GOT_PIXELBYTE, CeladonCity_Condominiums_3F_EventScript_GraphicArtistAfter
	msgbox CeladonCity_Condominiums_3F_Text_ImTheGraphicArtist
	setvar VAR_TEMP_1, SPECIES_NUMEL
	givemon SPECIES_NUMEL, 25
	goto_if_eq VAR_RESULT, 0, CeladonCity_Condominiums_3F_EventScript_GetPixelbyteParty
	goto_if_eq VAR_RESULT, 1, CeladonCity_Condominiums_3F_EventScript_GetPixelbytePC
	goto_if_eq VAR_RESULT, 2, EventScript_NoMoreRoomForPokemon
	release
	end

CeladonCity_Condominiums_3F_EventScript_GetPixelbyteParty::
	playfanfare MUS_LEVEL_UP
	message CeladonCity_Condominiums_3F_Text_ObtainedPixelbyte
	waitmessage
	waitfanfare
	bufferspeciesname STR_VAR_1, SPECIES_NUMEL
	msgbox Text_GiveNicknameToThisMon, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, CeladonCity_Condominiums_3F_EventScript_SetGotPixelbyte
	call EventScript_GetGiftMonPartySlot
	call EventScript_ChangePokemonNickname
	goto CeladonCity_Condominiums_3F_EventScript_SetGotPixelbyte
	end

CeladonCity_Condominiums_3F_EventScript_GetPixelbytePC::
	playfanfare MUS_LEVEL_UP
	message CeladonCity_Condominiums_3F_Text_ObtainedPixelbyte
	waitmessage
	waitfanfare
	bufferspeciesname STR_VAR_1, SPECIES_NUMEL
	msgbox Text_GiveNicknameToThisMon, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, CeladonCity_Condominiums_3F_EventScript_TransferPixelbyteToPC
	call EventScript_NameReceivedBoxMon
	goto CeladonCity_Condominiums_3F_EventScript_TransferPixelbyteToPC
	end

CeladonCity_Condominiums_3F_EventScript_TransferPixelbyteToPC::
	call EventScript_TransferredToPC
	goto CeladonCity_Condominiums_3F_EventScript_SetGotPixelbyte
	end

CeladonCity_Condominiums_3F_EventScript_SetGotPixelbyte::
	setflag FLAG_GOT_PIXELBYTE
	release
	end

CeladonCity_Condominiums_3F_EventScript_GraphicArtistAfter::
	msgbox CeladonCity_Condominiums_3F_Text_ItChecksItsWork
	release
	end

CeladonCity_Condominiums_3F_EventScript_Writer::
	msgbox CeladonCity_Condominiums_3F_Text_IWroteTheStory, MSGBOX_NPC
	end

CeladonCity_Condominiums_3F_EventScript_Designer::
	lock
	faceplayer
	specialvar VAR_RESULT, HasAllKantoMons
	goto_if_eq VAR_RESULT, TRUE, CeladonCity_Condominiums_3F_EventScript_CompletedPokedex
	msgbox CeladonCity_Condominiums_3F_Text_ImGameDesignerShowMeFinishedPokedex
	release
	end

CeladonCity_Condominiums_3F_EventScript_CompletedPokedex::
	goto CeladonCity_Condominiums_3F_EventScript_ShowDiploma
	end

CeladonCity_Condominiums_3F_EventScript_ShowDiploma::
	message CeladonCity_Condominiums_3F_Text_CompletedPokedexCongratulations
	waitmessage
	delay 60
	goto_if_questlog EventScript_ReleaseEnd
	special ShowDiploma
	waitstate
	release
	end

CeladonCity_Condominiums_3F_EventScript_DevelopmentRoomSign::
	msgbox CeladonCity_Condominiums_3F_Text_GameFreakDevelopmentRoom, MSGBOX_SIGN
	end

CeladonCity_Condominiums_3F_EventScript_Computer1::
	msgbox CeladonCity_Condominiums_3F_Text_ItsTheGameProgram, MSGBOX_SIGN
	end

CeladonCity_Condominiums_3F_EventScript_Computer2::
	msgbox CeladonCity_Condominiums_3F_Text_SomeonesPlayingGame, MSGBOX_SIGN
	end

CeladonCity_Condominiums_3F_EventScript_Computer3::
	msgbox CeladonCity_Condominiums_3F_Text_ItsScriptBetterNotLookAtEnding, MSGBOX_SIGN
	end
