SaffronCity_Mart_MapScripts::
	.byte 0

SaffronCity_Mart_EventScript_Lass::
	msgbox SaffronCity_Mart_Text_ReviveIsCostly, MSGBOX_NPC
	end

SaffronCity_Mart_EventScript_Youngster::
	msgbox SaffronCity_Mart_Text_MaxRepelMoreEffectiveThanSuper, MSGBOX_NPC
	end

SaffronCity_Mart_EventScript_Clerk::
	goto_if_questlog EventScript_ReleaseEnd
	lock
	faceplayer
	call EventScript_ClerkNoticesOpus    @ T-348
	message Text_MayIHelpYou
	waitmessage
	goto_if_set FLAG_BADGE06_GET, SaffronCity_Mart_EventScript_ClerkSinks   @ T-488
	pokemart SaffronCity_Mart_Items
	msgbox Text_PleaseComeAgain
	release
	end

@  T-488 (the user, 2026-10-29: sold by type, late): once this town's MARK is held, the stock gains the * SINKs of
@  its leader's type, CONTEXT, and of every type super-effective against it. DRAFT.
SaffronCity_Mart_EventScript_ClerkSinks::
	pokemart SaffronCity_Mart_ItemsSinks
	msgbox Text_PleaseComeAgain
	release
	end

	.align 2
SaffronCity_Mart_Items::
	.2byte ITEM_GREAT_BALL
	.2byte ITEM_HYPER_POTION
	.2byte ITEM_REVIVE
	.2byte ITEM_FULL_HEAL
	.2byte ITEM_ESCAPE_ROPE
	.2byte ITEM_MAX_REPEL
	.2byte ITEM_NONE
	release
	end

	.align 2
SaffronCity_Mart_ItemsSinks::
	.2byte ITEM_GREAT_BALL
	.2byte ITEM_HYPER_POTION
	.2byte ITEM_REVIVE
	.2byte ITEM_FULL_HEAL
	.2byte ITEM_ESCAPE_ROPE
	.2byte ITEM_MAX_REPEL
	.2byte ITEM_CONTEXT_SINK
	.2byte ITEM_SWARM_SINK
	.2byte ITEM_LATENT_SINK
	.2byte ITEM_OPAQUE_SINK
	.2byte ITEM_NONE
