Route13_MapScripts::
	.byte 0

Route13_EventScript_TrainerTips1::
	msgbox Route13_Text_LookToLeftOfThatPost, MSGBOX_SIGN
	end

Route13_EventScript_TrainerTips2::
	msgbox Route13_Text_SelectToSwitchItems, MSGBOX_SIGN
	end

Route13_EventScript_RouteSign::
	msgbox Route13_Text_RouteSign, MSGBOX_SIGN
	end

@ T-221: the tree that stands alone answers A -- the way into a grove. Nothing is said; REVEAL makes it
@ twinkle (src/reveal.c), and pressing A on the right tree finds it without.
Route13_EventScript_Grove::
	lockall
	playse SE_M_CUT
	warp MAP_ROUTE13_GROVE, 8, 8
	waitstate
	releaseall
	end
