SixIsland_GreenPath_MapScripts::
	.byte 0

SixIsland_GreenPath_EventScript_RightRouteSign::
	msgbox SixIsland_GreenPath_Text_LeftRouteSign, MSGBOX_SIGN
	end

SixIsland_GreenPath_EventScript_LeftRouteSign::
	msgbox SixIsland_GreenPath_Text_RightRouteSign, MSGBOX_SIGN
	end

@  T-22 / 4.33. He is already talking when you arrive and he does not stop for
@  you. Each visit gives one step; the ninth clears the eight flags and he
@  begins again, because nobody checks him and nothing has changed.
@
@  4.33's PROHIBITIONS are the whole point and they are all absences, so the
@  only way to see them is to say what is deliberately not here:
@    * no yes/no, no dialogue choice, and he never asks whether you agree
@    * no MARK, no item, no Index entry, no flag anything else reads
@    * no NPC on this map or any other refers to him
@    * he is not in the ERRATA -- a card that matched his reasoning would be
@      the game calling him wrong by filing
@    * nothing he predicts happens, and nothing he predicts fails
@
@  THE TEST: delete him and see whether anything else changes. Nothing does.
SixIsland_GreenPath_EventScript_TheOneDaemon::
	lock
	goto_if_set FLAG_DOCTRINE_STEP8, SixIsland_GreenPath_EventScript_Doctrine_Again
	goto_if_unset FLAG_DOCTRINE_STEP1, SixIsland_GreenPath_EventScript_Doctrine1
	goto_if_unset FLAG_DOCTRINE_STEP2, SixIsland_GreenPath_EventScript_Doctrine2
	goto_if_unset FLAG_DOCTRINE_STEP3, SixIsland_GreenPath_EventScript_Doctrine3
	goto_if_unset FLAG_DOCTRINE_STEP4, SixIsland_GreenPath_EventScript_Doctrine4
	goto_if_unset FLAG_DOCTRINE_STEP5, SixIsland_GreenPath_EventScript_Doctrine5
	goto_if_unset FLAG_DOCTRINE_STEP6, SixIsland_GreenPath_EventScript_Doctrine6
	goto_if_unset FLAG_DOCTRINE_STEP7, SixIsland_GreenPath_EventScript_Doctrine7
	goto SixIsland_GreenPath_EventScript_Doctrine8
	end

SixIsland_GreenPath_EventScript_Doctrine_Again::
	clearflag FLAG_DOCTRINE_STEP1
	clearflag FLAG_DOCTRINE_STEP2
	clearflag FLAG_DOCTRINE_STEP3
	clearflag FLAG_DOCTRINE_STEP4
	clearflag FLAG_DOCTRINE_STEP5
	clearflag FLAG_DOCTRINE_STEP6
	clearflag FLAG_DOCTRINE_STEP7
	clearflag FLAG_DOCTRINE_STEP8
	goto SixIsland_GreenPath_EventScript_Doctrine1
	end

SixIsland_GreenPath_EventScript_Doctrine1::
	setflag FLAG_DOCTRINE_STEP1
	msgbox SixIsland_GreenPath_Text_Doctrine1
	release
	end

SixIsland_GreenPath_EventScript_Doctrine2::
	setflag FLAG_DOCTRINE_STEP2
	msgbox SixIsland_GreenPath_Text_Doctrine2
	release
	end

SixIsland_GreenPath_EventScript_Doctrine3::
	setflag FLAG_DOCTRINE_STEP3
	msgbox SixIsland_GreenPath_Text_Doctrine3
	release
	end

SixIsland_GreenPath_EventScript_Doctrine4::
	setflag FLAG_DOCTRINE_STEP4
	msgbox SixIsland_GreenPath_Text_Doctrine4
	release
	end

SixIsland_GreenPath_EventScript_Doctrine5::
	setflag FLAG_DOCTRINE_STEP5
	msgbox SixIsland_GreenPath_Text_Doctrine5
	release
	end

SixIsland_GreenPath_EventScript_Doctrine6::
	setflag FLAG_DOCTRINE_STEP6
	msgbox SixIsland_GreenPath_Text_Doctrine6
	release
	end

SixIsland_GreenPath_EventScript_Doctrine7::
	setflag FLAG_DOCTRINE_STEP7
	msgbox SixIsland_GreenPath_Text_Doctrine7
	release
	end

SixIsland_GreenPath_EventScript_Doctrine8::
	setflag FLAG_DOCTRINE_STEP8
	msgbox SixIsland_GreenPath_Text_Doctrine8
	release
	end
