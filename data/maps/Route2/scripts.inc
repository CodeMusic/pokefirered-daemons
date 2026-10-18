Route2_MapScripts::
	.byte 0

Route2_EventScript_RouteSign::
	msgbox Route2_Text_RouteSign, MSGBOX_SIGN
	end

Route2_EventScript_DiglettsCaveSign::
	msgbox Route2_Text_DiglettsCave, MSGBOX_SIGN
	end

@ T-417 (the user, 2026-10-18): the lone palm just north of CALLOW answers A -- the way to BREATHVERSE, as a grove's
@ tree is the way into its grove. Nothing is said; REVEAL makes it twinkle (tools/gbagrove.py lists it), and pressing
@ A on it finds it without. The palm and its sign are tools/gbabreathverse.py's.
Route2_EventScript_Breathverse::
	lockall
	playse SE_M_CUT
	warp MAP_BREATHVERSE, 13, 12
	waitstate
	releaseall
	end
