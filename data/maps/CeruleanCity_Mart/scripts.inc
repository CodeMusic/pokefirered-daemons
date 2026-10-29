CeruleanCity_Mart_MapScripts::
	.byte 0

CeruleanCity_Mart_EventScript_Youngster::
	msgbox CeruleanCity_Mart_Text_RepelWorksOnWeakMons, MSGBOX_NPC
	end

CeruleanCity_Mart_EventScript_Woman::
	msgbox CeruleanCity_Mart_Text_DoYouKnowAboutRareCandy, MSGBOX_NPC
	end

CeruleanCity_Mart_EventScript_Clerk::
	goto_if_questlog EventScript_ReleaseEnd
	lock
	faceplayer
	call EventScript_ClerkNoticesOpus    @ T-348
	message Text_MayIHelpYou
	waitmessage
	goto_if_set FLAG_BADGE02_GET, CeruleanCity_Mart_EventScript_ClerkSinks   @ T-488
	pokemart CeruleanCity_Mart_Items
	msgbox Text_PleaseComeAgain
	release
	end

@  T-488 (the user, 2026-10-29: sold by type, late): once this town's MARK is held, the stock gains the * SINKs of
@  its leader's type, FLOW, and of every type super-effective against it. DRAFT.
CeruleanCity_Mart_EventScript_ClerkSinks::
	pokemart CeruleanCity_Mart_ItemsSinks
	msgbox Text_PleaseComeAgain
	release
	end

	.align 2
CeruleanCity_Mart_Items::
	.2byte ITEM_POKE_BALL
	.2byte ITEM_SUPER_POTION
	.2byte ITEM_POTION
	.2byte ITEM_ANTIDOTE
	.2byte ITEM_PARALYZE_HEAL
	.2byte ITEM_AWAKENING
	.2byte ITEM_BURN_HEAL
	.2byte ITEM_ESCAPE_ROPE
	.2byte ITEM_REPEL
	.2byte ITEM_NONE
	release
	end

	.align 2
CeruleanCity_Mart_ItemsSinks::
	.2byte ITEM_POKE_BALL
	.2byte ITEM_SUPER_POTION
	.2byte ITEM_POTION
	.2byte ITEM_ANTIDOTE
	.2byte ITEM_PARALYZE_HEAL
	.2byte ITEM_AWAKENING
	.2byte ITEM_BURN_HEAL
	.2byte ITEM_ESCAPE_ROPE
	.2byte ITEM_REPEL
	.2byte ITEM_FLOW_SINK
	.2byte ITEM_SIGNAL_SINK
	.2byte ITEM_GROWTH_SINK
	.2byte ITEM_NONE
