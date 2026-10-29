CeladonCity_DepartmentStore_2F_MapScripts::
	.byte 0

CeladonCity_DepartmentStore_2F_EventScript_UnusedNPC::
	msgbox CeladonCity_DepartmentStore_2F_Text_SuperRepelMorePowerfulRepel, MSGBOX_NPC
	end

CeladonCity_DepartmentStore_2F_EventScript_Lass::
	msgbox CeladonCity_DepartmentStore_2F_Text_BuyReviveForLongOutings, MSGBOX_NPC
	end

CeladonCity_DepartmentStore_2F_EventScript_FloorSign::
	msgbox CeladonCity_DepartmentStore_2F_Text_FloorSign, MSGBOX_SIGN
	end

CeladonCity_DepartmentStore_2F_EventScript_ClerkItems::
	goto_if_questlog EventScript_ReleaseEnd
	lock
	faceplayer
	call EventScript_ClerkNoticesOpus    @ T-348
	message Text_MayIHelpYou
	waitmessage
	goto_if_set FLAG_BADGE04_GET, CeladonCity_DepartmentStore_2F_EventScript_ClerkSinks   @ T-488
	pokemart CeladonCity_DepartmentStore_2F_Items
	msgbox Text_PleaseComeAgain
	release
	end

@  T-488 (the user, 2026-10-29: sold by type, late): once this town's MARK is held, the stock gains the * SINKs of
@  its leader's type, GROWTH, and of every type super-effective against it. DRAFT.
CeladonCity_DepartmentStore_2F_EventScript_ClerkSinks::
	pokemart CeladonCity_DepartmentStore_2F_ItemsSinks
	msgbox Text_PleaseComeAgain
	release
	end

	.align 2
CeladonCity_DepartmentStore_2F_Items::
	.2byte ITEM_GREAT_BALL
	.2byte ITEM_SUPER_POTION
	.2byte ITEM_REVIVE
	.2byte ITEM_ANTIDOTE
	.2byte ITEM_PARALYZE_HEAL
	.2byte ITEM_AWAKENING
	.2byte ITEM_BURN_HEAL
	.2byte ITEM_ICE_HEAL
	.2byte ITEM_SUPER_REPEL
	.2byte ITEM_NONE
	release
	end

	.align 2
CeladonCity_DepartmentStore_2F_ItemsSinks::
	.2byte ITEM_GREAT_BALL
	.2byte ITEM_SUPER_POTION
	.2byte ITEM_REVIVE
	.2byte ITEM_ANTIDOTE
	.2byte ITEM_PARALYZE_HEAL
	.2byte ITEM_AWAKENING
	.2byte ITEM_BURN_HEAL
	.2byte ITEM_ICE_HEAL
	.2byte ITEM_SUPER_REPEL
	.2byte ITEM_GROWTH_SINK
	.2byte ITEM_ENTROPY_SINK
	.2byte ITEM_FROZEN_SINK
	.2byte ITEM_CORRUPT_SINK
	.2byte ITEM_VECTOR_SINK
	.2byte ITEM_SWARM_SINK
	.2byte ITEM_NONE

CeladonCity_DepartmentStore_2F_EventScript_ClerkTMs::
	goto_if_questlog EventScript_ReleaseEnd
	lock
	faceplayer
	call EventScript_ClerkNoticesOpus    @ T-348
	message Text_MayIHelpYou
	waitmessage
	pokemart CeladonCity_DepartmentStore_2F_TMs
	msgbox Text_PleaseComeAgain
	release
	end

	.align 2
CeladonCity_DepartmentStore_2F_TMs::
	.2byte ITEM_TM15
	.2byte ITEM_TM28
	.2byte ITEM_TM31
	.2byte ITEM_NONE
	release
	end
