SSAnne_1F_Corridor_MapScripts::
	.byte 0

SSAnne_1F_Corridor_EventScript_WorkerM::
	msgbox SSAnne_1F_Corridor_Text_LeStrongSilentType, MSGBOX_NPC
	end

SSAnne_1F_Corridor_EventScript_Sailor::
	msgbox SSAnne_1F_Corridor_Text_PassengersAreRestless, MSGBOX_NPC
	end

@ T-496 (DRAFT): the lioness, the third meeting -- checking everything that comes aboard.
SSAnne_1F_Corridor_EventScript_Lioness::
	msgbox SSAnne_1F_Corridor_Text_Lioness, MSGBOX_NPC
	end

@ T-493 (DRAFT): the painting anyone may look at. Its words are scrambled -- the alphabet in a mirror -- and only its
@ twin, on THE NEXUS's easel, holds them still. Looking files them in the NOTEBOOK, as they are.
SSAnne_1F_Corridor_EventScript_Painting::
	lockall
	msgbox SSAnne_1F_Corridor_Text_Painting
	goto_if_set FLAG_NOTEBOOK_LOOSE_PAINTING, SSAnne_1F_Corridor_EventScript_PaintingDone
	goto_if_set FLAG_NOTEBOOK_LOOSE_PAINTING_READ, SSAnne_1F_Corridor_EventScript_PaintingDone
	setflag FLAG_NOTEBOOK_LOOSE_PAINTING
	goto_if_unset FLAG_GOT_NOTEBOOK, SSAnne_1F_Corridor_EventScript_PaintingDone
	playse SE_SELECT
	msgbox SSAnne_1F_Corridor_Text_PaintingIntoNotebook
SSAnne_1F_Corridor_EventScript_PaintingDone::
	releaseall
	end
