@  T-519 item 1: SPIRITS (vision 4.40, src/daemons_spirits.c). A party daemon crossed into or out of good or low
@  spirits, in a battle, a menu or the field; on the first step after, its screen plays on the black of a fadescreen --
@  the evolution's style, lighter -- and then its one line (DRAFT), which Daemons_TrySpiritsCrossing put in gStringVar4.
Daemons_EventScript_SpiritsCrossing::
	lockall
	fadescreen FADE_TO_BLACK
	special Spirits_PlayCrossing
	waitstate
	fadescreen FADE_FROM_BLACK
	msgbox gStringVar4
	releaseall
	end
