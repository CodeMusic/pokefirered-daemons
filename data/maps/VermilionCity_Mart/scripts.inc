VermilionCity_Mart_MapScripts::
	.byte 0

VermilionCity_Mart_EventScript_CooltrainerF::
	msgbox VermilionCity_Mart_Text_MonsGoodOrBadDependingOnTrainer, MSGBOX_NPC
	end

VermilionCity_Mart_EventScript_BaldingMan::
	msgbox VermilionCity_Mart_Text_TeamRocketAreWickedPeople, MSGBOX_NPC
	end

VermilionCity_Mart_EventScript_Clerk::
	goto_if_questlog EventScript_ReleaseEnd
	lock
	faceplayer
	call EventScript_ClerkNoticesOpus    @ T-348
	message Text_MayIHelpYou
	waitmessage
	goto_if_set FLAG_BADGE03_GET, VermilionCity_Mart_EventScript_ClerkSinks   @ T-488
	pokemart VermilionCity_Mart_Items
	msgbox Text_PleaseComeAgain
	release
	end

@  T-488 (the user, 2026-10-29: sold by type, late): once this town's MARK is held, the stock gains the * SINKs of
@  its leader's type, SIGNAL, and of every type super-effective against it. DRAFT.
VermilionCity_Mart_EventScript_ClerkSinks::
	pokemart VermilionCity_Mart_ItemsSinks
	msgbox Text_PleaseComeAgain
	release
	end

	.align 2
VermilionCity_Mart_Items::
	.2byte ITEM_POKE_BALL
	.2byte ITEM_SUPER_POTION
	.2byte ITEM_ANTIDOTE
	.2byte ITEM_PARALYZE_HEAL
	.2byte ITEM_AWAKENING
	.2byte ITEM_ICE_HEAL
	.2byte ITEM_REPEL
	.2byte ITEM_NONE
	release
	end

	.align 2
VermilionCity_Mart_ItemsSinks::
	.2byte ITEM_POKE_BALL
	.2byte ITEM_SUPER_POTION
	.2byte ITEM_ANTIDOTE
	.2byte ITEM_PARALYZE_HEAL
	.2byte ITEM_AWAKENING
	.2byte ITEM_ICE_HEAL
	.2byte ITEM_REPEL
	.2byte ITEM_SIGNAL_SINK
	.2byte ITEM_STRATUM_SINK
	.2byte ITEM_NONE
