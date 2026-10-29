CinnabarIsland_Mart_MapScripts::
	.byte 0

CinnabarIsland_Mart_EventScript_Woman::
	msgbox CinnabarIsland_Mart_Text_DontTheyHaveXAttack, MSGBOX_NPC
	end

CinnabarIsland_Mart_EventScript_Scientist::
	msgbox CinnabarIsland_Mart_Text_ExtraItemsNeverHurt, MSGBOX_NPC
	end

CinnabarIsland_Mart_EventScript_Clerk::
	goto_if_questlog EventScript_ReleaseEnd
	lock
	faceplayer
	call EventScript_ClerkNoticesOpus    @ T-348
	message Text_MayIHelpYou
	waitmessage
	goto_if_set FLAG_BADGE07_GET, CinnabarIsland_Mart_EventScript_ClerkSinks   @ T-488
	pokemart CinnabarIsland_Mart_Items
	msgbox Text_PleaseComeAgain
	release
	end

@  T-488 (the user, 2026-10-29: sold by type, late): once this town's MARK is held, the stock gains the * SINKs of
@  its leader's type, ENTROPY, and of every type super-effective against it. DRAFT.
CinnabarIsland_Mart_EventScript_ClerkSinks::
	pokemart CinnabarIsland_Mart_ItemsSinks
	msgbox Text_PleaseComeAgain
	release
	end

	.align 2
CinnabarIsland_Mart_Items::
	.2byte ITEM_ULTRA_BALL
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
CinnabarIsland_Mart_ItemsSinks::
	.2byte ITEM_ULTRA_BALL
	.2byte ITEM_GREAT_BALL
	.2byte ITEM_HYPER_POTION
	.2byte ITEM_REVIVE
	.2byte ITEM_FULL_HEAL
	.2byte ITEM_ESCAPE_ROPE
	.2byte ITEM_MAX_REPEL
	.2byte ITEM_ENTROPY_SINK
	.2byte ITEM_FLOW_SINK
	.2byte ITEM_STRATUM_SINK
	.2byte ITEM_LEGACY_SINK
	.2byte ITEM_NONE
