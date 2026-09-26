PewterCity_Mart_MapScripts::
	.byte 0

PewterCity_Mart_EventScript_Youngster::
	msgbox PewterCity_Mart_Text_BoughtWeirdFishFromShadyGuy, MSGBOX_NPC
	end

PewterCity_Mart_EventScript_Boy::
	msgbox PewterCity_Mart_Text_GoodThingsIfRaiseMonsDiligently, MSGBOX_NPC
	end

@  T-312: the one person who went looking for OPUS and never saw it. CALLOW's REPO is two towns back; if the player
@  walked past the gold card on the bin, this is where they hear there was something to see. DRAFT wording.
PewterCity_Mart_EventScript_Collector::
	lock
	faceplayer
	goto_if_set FLAG_GOT_OPUS, PewterCity_Mart_EventScript_CollectorAfter
	msgbox PewterCity_Mart_Text_CollectorLooked
	release
	end

PewterCity_Mart_EventScript_CollectorAfter::
	msgbox PewterCity_Mart_Text_CollectorTooLate
	release
	end

PewterCity_Mart_EventScript_Clerk::
	goto_if_questlog EventScript_ReleaseEnd
	lock
	faceplayer
	message Text_MayIHelpYou
	waitmessage
	pokemart PewterCity_Mart_Items
	msgbox Text_PleaseComeAgain
	release
	end

	.align 2
PewterCity_Mart_Items::
	.2byte ITEM_POKE_BALL
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
