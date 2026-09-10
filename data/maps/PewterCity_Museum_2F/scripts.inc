PewterCity_Museum_2F_MapScripts::
	.byte 0

PewterCity_Museum_2F_EventScript_Man::
	msgbox PewterCity_Museum_1F_Text_WhatsSpecialAboutMoonStone, MSGBOX_NPC
	end

PewterCity_Museum_2F_EventScript_OldMan::
	msgbox PewterCity_Museum_1F_Text_BoughtColorTVForMoonLanding, MSGBOX_NPC
	end

PewterCity_Museum_2F_EventScript_Scientist::
	msgbox PewterCity_Museum_1F_Text_RunningSpaceExhibitThisMonth, MSGBOX_NPC
	end

PewterCity_Museum_2F_EventScript_LittleGirl::
	msgbox PewterCity_Museum_1F_Text_AskedDaddyToCatchPikachu, MSGBOX_NPC
	end

PewterCity_Museum_2F_EventScript_BaldingMan::
	msgbox PewterCity_Museum_1F_Text_PikachuSoonIPromise, MSGBOX_NPC
	end

PewterCity_Museum_2F_EventScript_SpaceShuttle::
	msgbox PewterCity_Museum_1F_Text_SpaceShuttle, MSGBOX_SIGN
	end

PewterCity_Museum_2F_EventScript_MoonStone::
	msgbox PewterCity_Museum_1F_Text_MeteoriteThatFellOnMtMoon, MSGBOX_SIGN
	end

@  T-09. The two ERRATA cards from lineage 3b, on the one exhibit block on
@  this floor that had no card at all -- 8-10 by 7-9, read from either side,
@  so the player meets them coming or going.
@
@  4.32's format, unchanged: OBJECT, provenance, KNOWN FAULT, and a closing
@  line that does not explain itself. And 4.32's rule, which is the whole
@  reason these are safe to write: THE FAULT IS THE MACHINE'S. No distortion
@  is ever named, and nobody in the room is being corrected.
PewterCity_Museum_2F_EventScript_TheDisplayUnit::
	msgbox PewterCity_Museum_2F_Text_TheDisplayUnit, MSGBOX_SIGN
	end

PewterCity_Museum_2F_EventScript_TheSortingFrame::
	msgbox PewterCity_Museum_2F_Text_TheSortingFrame, MSGBOX_SIGN
	end
