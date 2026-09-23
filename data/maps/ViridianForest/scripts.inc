ViridianForest_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, ViridianForest_OnTransition
	.byte 0

ViridianForest_OnTransition::
	setworldmapflag FLAG_WORLD_MAP_VIRIDIAN_FOREST
	end

ViridianForest_EventScript_Youngster::
	msgbox ViridianForest_Text_FriendsItchingToBattle, MSGBOX_NPC
	end

ViridianForest_EventScript_Boy::
	msgbox ViridianForest_Text_RanOutOfPokeBalls, MSGBOX_NPC
	end

ViridianForest_EventScript_TrainerTips1::
	msgbox ViridianForest_Text_AvoidGrassyAreasWhenWeak, MSGBOX_SIGN
	end

ViridianForest_EventScript_TrainerTips2::
	msgbox ViridianForest_Text_UseAntidoteForPoison, MSGBOX_SIGN
	end

ViridianForest_EventScript_TrainerTips3::
	msgbox ViridianForest_Text_ContactOakViaPCToRatePokedex, MSGBOX_SIGN
	end

@ THE UNDERTONE's carving (4.20). T-216: reading it records LOOSE PAGES 1 -- a flag, so a player who reads it
@ before CAIRN hands the notebook over (this forest comes first) finds the page already in it.
ViridianForest_EventScript_TrainerTips4::
	lockall
	msgbox ViridianForest_Text_CantCatchOwnedMons
	goto_if_set FLAG_NOTEBOOK_LOOSE_STONE, ViridianForest_EventScript_CarvingDone
	setflag FLAG_NOTEBOOK_LOOSE_STONE
	msgbox ViridianForest_Text_PageUnderStone
	goto_if_unset FLAG_GOT_NOTEBOOK, ViridianForest_EventScript_KeptPage
	msgbox ViridianForest_Text_PageIntoNotebook
	goto ViridianForest_EventScript_CarvingDone
ViridianForest_EventScript_KeptPage::
	msgbox ViridianForest_Text_KeptPage
ViridianForest_EventScript_CarvingDone::
	releaseall
	end

ViridianForest_EventScript_TrainerTips5::
	msgbox ViridianForest_Text_WeakenMonsBeforeCapture, MSGBOX_SIGN
	end

ViridianForest_EventScript_ExitSign::
	msgbox ViridianForest_Text_LeavingViridianForest, MSGBOX_SIGN
	end

ViridianForest_EventScript_Rick::
	trainerbattle_single TRAINER_BUG_CATCHER_RICK, ViridianForest_Text_RickIntro, ViridianForest_Text_RickDefeat
	msgbox ViridianForest_Text_RickPostBattle, MSGBOX_AUTOCLOSE
	end

ViridianForest_EventScript_Doug::
	trainerbattle_single TRAINER_BUG_CATCHER_DOUG, ViridianForest_Text_DougIntro, ViridianForest_Text_DougDefeat
	msgbox ViridianForest_Text_DougPostBattle, MSGBOX_AUTOCLOSE
	end

ViridianForest_EventScript_Sammy::
	trainerbattle_single TRAINER_BUG_CATCHER_SAMMY, ViridianForest_Text_SammyIntro, ViridianForest_Text_SammyDefeat
	msgbox ViridianForest_Text_SammyPostBattle, MSGBOX_AUTOCLOSE
	end

ViridianForest_EventScript_Anthony::
	trainerbattle_single TRAINER_BUG_CATCHER_ANTHONY, ViridianForest_Text_AnthonyIntro, ViridianForest_Text_AnthonyDefeat
	msgbox ViridianForest_Text_AnthonyPostBattle, MSGBOX_AUTOCLOSE
	end

ViridianForest_EventScript_Charlie::
	trainerbattle_single TRAINER_BUG_CATCHER_CHARLIE, ViridianForest_Text_CharlieIntro, ViridianForest_Text_CharlieDefeat
	msgbox ViridianForest_Text_CharliePostBattle, MSGBOX_AUTOCLOSE
	end

@ T-221: the tree that stands alone answers A -- the way into a grove. Nothing is said; REVEAL makes it
@ twinkle (src/reveal.c), and pressing A on the right tree finds it without.
ViridianForest_EventScript_Grove::
	lockall
	playse SE_M_CUT
	warp MAP_VIRIDIAN_FOREST_GROVE, 4, 6
	waitstate
	releaseall
	end
