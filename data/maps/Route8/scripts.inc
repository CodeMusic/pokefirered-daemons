Route8_MapScripts::
	.byte 0

Route8_EventScript_UndergroundPathSign::
	msgbox Route8_Text_UndergroundPathSign, MSGBOX_SIGN
	end

@ T-221: the tree that stands alone answers A -- the way into a grove. Nothing is said; REVEAL makes it
@ twinkle (src/reveal.c), and pressing A on the right tree finds it without.
Route8_EventScript_Grove::
	lockall
	playse SE_M_CUT
	warp MAP_ROUTE8_GROVE, 8, 8
	waitstate
	releaseall
	end
