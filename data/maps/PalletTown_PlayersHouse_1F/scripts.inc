PalletTown_PlayersHouse_1F_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, PalletTown_PlayersHouse_1F_OnTransition
	.byte 0

@  T-256: MOM AND DAD are at the table until one of them has given you the RUNNING SHOES; after that they move about
@  the room like people who live here.
PalletTown_PlayersHouse_1F_OnTransition::
	call_if_set FLAG_SYS_B_DASH, PalletTown_PlayersHouse_1F_EventScript_ParentsWander
	end

PalletTown_PlayersHouse_1F_EventScript_ParentsWander::
	setobjectmovementtype LOCALID_MOM, MOVEMENT_TYPE_WANDER_AROUND
	setobjectmovementtype LOCALID_DAD, MOVEMENT_TYPE_WANDER_AROUND
	return

PalletTown_PlayersHouse_1F_EventScript_Mom::
	lock
	faceplayer
	goto_if_unset FLAG_SYS_B_DASH, PalletTown_PlayersHouse_1F_EventScript_MomGiveRunningShoes
	goto_if_set FLAG_BEAT_RIVAL_IN_OAKS_LAB, PalletTown_PlayersHouse_1F_EventScript_MomHeal
	checkplayergender
	call_if_eq VAR_RESULT, MALE, PalletTown_PlayersHouse_1F_EventScript_MomOakLookingForYouMale
	call_if_eq VAR_RESULT, FEMALE, PalletTown_PlayersHouse_1F_EventScript_MomOakLookingForYouFemale
	closemessage
	applymovement LOCALID_MOM, Common_Movement_FaceOriginalDirection
	waitmovement 0
	release
	end

PalletTown_PlayersHouse_1F_EventScript_MomOakLookingForYouMale::
	msgbox PalletTown_PlayersHouse_1F_Text_AllBoysLeaveOakLookingForYou
	return

PalletTown_PlayersHouse_1F_EventScript_MomOakLookingForYouFemale::
	msgbox PalletTown_PlayersHouse_1F_Text_AllGirlsLeaveOakLookingForYou
	return

@ T-67: the RUNNING SHOES are Mom's to give, not an aide's in Pewter.
PalletTown_PlayersHouse_1F_EventScript_MomGiveRunningShoes::
	msgbox PalletTown_PlayersHouse_1F_Text_MomTakeTheseShoes
	textcolor NPC_TEXT_COLOR_NEUTRAL
	playfanfare MUS_OBTAIN_KEY_ITEM
	message PalletTown_PlayersHouse_1F_Text_ReceivedRunningShoesFromMom
	waitmessage
	waitfanfare
	setflag FLAG_SYS_B_DASH
	special DaemonsRepaintPlayerShoes   @ T-256: MOM's, yellow with black trim
	call EventScript_RestorePrevTextColor
	msgbox PalletTown_PlayersHouse_1F_Text_ExplainRunningShoes
	closemessage
	applymovement LOCALID_MOM, Common_Movement_FaceOriginalDirection
	waitmovement 0
	release
	end

@  T-256: DAD. Whichever parent you go to first gives you THEIR shoes, and after that they are simply your shoes --
@  the choice shows only in them. His lines are DRAFT.
PalletTown_PlayersHouse_1F_EventScript_Dad::
	lock
	faceplayer
	goto_if_unset FLAG_SYS_B_DASH, PalletTown_PlayersHouse_1F_EventScript_DadGiveRunningShoes
	@  T-313, DRAFT: the user's father signs DR HICKS, and it reads like a title. One time in two he says so.
	random 2
	goto_if_eq VAR_RESULT, 1, PalletTown_PlayersHouse_1F_EventScript_DadTheInitials
	msgbox PalletTown_PlayersHouse_1F_Text_DadQuieter
	closemessage
	applymovement LOCALID_DAD, Common_Movement_FaceOriginalDirection
	waitmovement 0
	release
	end

PalletTown_PlayersHouse_1F_EventScript_DadTheInitials::
	msgbox PalletTown_PlayersHouse_1F_Text_DadTheInitials
	closemessage
	applymovement LOCALID_DAD, Common_Movement_FaceOriginalDirection
	waitmovement 0
	release
	end

PalletTown_PlayersHouse_1F_EventScript_DadGiveRunningShoes::
	msgbox PalletTown_PlayersHouse_1F_Text_DadTakeTheseShoes
	textcolor NPC_TEXT_COLOR_NEUTRAL
	playfanfare MUS_OBTAIN_KEY_ITEM
	message PalletTown_PlayersHouse_1F_Text_ReceivedRunningShoesFromDad
	waitmessage
	waitfanfare
	setflag FLAG_SYS_B_DASH
	setflag FLAG_SHOES_FROM_DAD
	special DaemonsRepaintPlayerShoes   @ T-256: DAD's, black with red trim
	call EventScript_RestorePrevTextColor
	msgbox PalletTown_PlayersHouse_1F_Text_DadExplainRunningShoes
	closemessage
	applymovement LOCALID_DAD, Common_Movement_FaceOriginalDirection
	waitmovement 0
	release
	end

PalletTown_PlayersHouse_1F_EventScript_MomHeal::
	msgbox PalletTown_PlayersHouse_1F_Text_YouShouldTakeQuickRest
	closemessage
	call EventScript_OutOfCenterPartyHeal
	goto_if_set FLAG_GOT_SS_TICKET, PalletTown_PlayersHouse_1F_EventScript_MomTheShip
	msgbox PalletTown_PlayersHouse_1F_Text_LookingGreatTakeCare
	release
	end

@  T-313, DRAFT: MOM plans cruises for a living (the user's mother, a cruise specialist), and the ship's captain is
@  the player's brother. Only once the player holds a ticket for it.
PalletTown_PlayersHouse_1F_EventScript_MomTheShip::
	msgbox PalletTown_PlayersHouse_1F_Text_MomTheShip
	release
	end

@  T-313, DRAFT: the player's brother is never home -- the house is full -- so he is a note on the wall.
PalletTown_PlayersHouse_1F_EventScript_DavidsNote::
	msgbox PalletTown_PlayersHouse_1F_Text_DavidsNote, MSGBOX_SIGN
	end

@ Displays special text if interacted with from side or back (which are normally inaccessible)
PalletTown_PlayersHouse_1F_EventScript_TV::
	lockall
	goto_if_eq VAR_FACING, DIR_NORTH, PalletTown_PlayersHouse_1F_EventScript_TVScreen
	msgbox PalletTown_PlayersHouse_1F_Text_OopsWrongSide
	releaseall
	end

PalletTown_PlayersHouse_1F_EventScript_TVScreen::
	checkplayergender
	call_if_eq VAR_RESULT, MALE, PalletTown_PlayersHouse_1F_EventScript_TVScreenMale
	call_if_eq VAR_RESULT, FEMALE, PalletTown_PlayersHouse_1F_EventScript_TVScreenFemale
	releaseall
	end

PalletTown_PlayersHouse_1F_EventScript_TVScreenMale::
	msgbox PalletTown_PlayersHouse_1F_Text_MovieOnTVFourBoysOnRailroad
	return

PalletTown_PlayersHouse_1F_EventScript_TVScreenFemale::
	msgbox PalletTown_PlayersHouse_1F_Text_MovieOnTVGirlOnBrickRoad
	return
