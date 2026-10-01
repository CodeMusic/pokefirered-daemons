Route11_MapScripts::
	.byte 0

Route11_EventScript_DiglettsCaveSign::
	msgbox Route11_Text_DiglettsCave, MSGBOX_SIGN
	end

@ T-221: the tree that stands alone answers A -- the way into a grove. Nothing is said; REVEAL makes it
@ twinkle (src/reveal.c), and pressing A on the right tree finds it without.
Route11_EventScript_Grove::
	lockall
	playse SE_M_CUT
	warp MAP_ROUTE11_GROVE, 8, 8
	waitstate
	releaseall
	end
