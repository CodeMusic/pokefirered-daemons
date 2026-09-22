ViridianCity_Mart_MapScripts::
	map_script MAP_SCRIPT_ON_LOAD, ViridianCity_Mart_OnLoad
	map_script MAP_SCRIPT_ON_FRAME_TABLE, ViridianCity_Mart_OnFrame
	.byte 0

ViridianCity_Mart_OnLoad::
	goto_if_unset FLAG_SYS_POKEDEX_GET, ViridianCity_Mart_EventScript_HideQuestionnaire
	end

ViridianCity_Mart_EventScript_HideQuestionnaire::
	setmetatile 1, 3, METATILE_Mart_CounterMid_Top, 1
	setmetatile 1, 4, METATILE_Mart_CounterMid_Bottom, 1
	end

ViridianCity_Mart_OnFrame::
	map_script_2 VAR_MAP_SCENE_VIRIDIAN_CITY_MART, 0, ViridianCity_Mart_EventScript_ParcelScene
	.2byte 0

ViridianCity_Mart_EventScript_ParcelScene::
	lockall
	textcolor NPC_TEXT_COLOR_MALE
	applymovement LOCALID_VIRIDIAN_MART_CLERK, Common_Movement_WalkInPlaceFasterDown
	waitmovement 0
	msgbox ViridianCity_Mart_Text_YouCameFromPallet
	closemessage
	applymovement LOCALID_VIRIDIAN_MART_CLERK, ViridianCity_Mart_Movement_FacePlayer
	applymovement LOCALID_PLAYER, ViridianCity_Mart_Movement_ApproachCounter
	waitmovement 0
	msgbox ViridianCity_Mart_Text_TakeThisToProfOak
	setvar VAR_MAP_SCENE_VIRIDIAN_CITY_MART, 1
	giveitem_msg ViridianCity_Mart_Text_ReceivedOaksParcelFromClerk, ITEM_OAKS_PARCEL, 1, MUS_OBTAIN_KEY_ITEM
	setvar VAR_MAP_SCENE_PALLET_TOWN_PROFESSOR_OAKS_LAB, 5
	releaseall
	end

ViridianCity_Mart_Movement_ApproachCounter::
	walk_up
	walk_up
	walk_up
	walk_up
	walk_in_place_faster_left
	step_end

ViridianCity_Mart_Movement_FacePlayer::
	delay_16
	delay_16
	delay_16
	delay_16
	walk_in_place_faster_right
	step_end

ViridianCity_Mart_EventScript_Clerk::
	lock
	faceplayer
	goto_if_eq VAR_MAP_SCENE_VIRIDIAN_CITY_MART, 1, ViridianCity_Mart_EventScript_SayHiToOak
	goto_if_questlog EventScript_ReleaseEnd
	message Text_MayIHelpYou
	waitmessage
	pokemart ViridianCity_Mart_Items
	msgbox Text_PleaseComeAgain
	release
	end

	.align 2
ViridianCity_Mart_Items::
	.2byte ITEM_POKE_BALL
	.2byte ITEM_POTION
	.2byte ITEM_ANTIDOTE
	.2byte ITEM_PARALYZE_HEAL
	.2byte ITEM_NONE
	release
	end

ViridianCity_Mart_EventScript_SayHiToOak::
	msgbox ViridianCity_Mart_Text_SayHiToOakForMe
	release
	end

ViridianCity_Mart_EventScript_Woman::
	msgbox ViridianCity_Mart_Text_ShopDoesGoodBusinessInAntidotes, MSGBOX_NPC
	end

ViridianCity_Mart_EventScript_Youngster::
	msgbox ViridianCity_Mart_Text_GotToBuySomePotions, MSGBOX_NPC
	end

@  T-197: OPUS. It is on the end of a shelf with no price on it, and nobody in the room remarks on it --
@  not then and not afterwards. Who left it is never answered.
@
@  The other three shelves are the reason this is findable at all: a thing nobody points at needs a room
@  worth reading, so every shelf in the REPO says something and one of them has the thing on it. That is
@  curiosity as the mechanism rather than a marker over its head.
@
@  additem and a msgbox of our own, NOT giveitem: giveitem plays the fanfare and announces a prize, which
@  is the one thing this must not do.
ViridianCity_Mart_EventScript_ShelfEnd::
	lockall
	goto_if_set FLAG_GOT_OPUS, ViridianCity_Mart_EventScript_ShelfEndTaken
	msgbox ViridianCity_Mart_Text_ShelfEnd
	additem ITEM_OPUS
	setflag FLAG_GOT_OPUS
	msgbox ViridianCity_Mart_Text_TookOpus
	releaseall
	end

ViridianCity_Mart_EventScript_ShelfEndTaken::
	msgbox ViridianCity_Mart_Text_ShelfEndTaken
	releaseall
	end

ViridianCity_Mart_EventScript_Shelf1::
	lockall
	msgbox ViridianCity_Mart_Text_Shelf1
	releaseall
	end

ViridianCity_Mart_EventScript_Shelf2::
	lockall
	msgbox ViridianCity_Mart_Text_Shelf2
	releaseall
	end

ViridianCity_Mart_EventScript_Shelf3::
	lockall
	msgbox ViridianCity_Mart_Text_Shelf3
	releaseall
	end
