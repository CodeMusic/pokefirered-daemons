Route21_North_Station_MapScripts::
	.byte 0

@  T-235: THE DRAWING UNDER THE LOG. Once the TRANSCRIPT is in the NOTEBOOK and REVEAL is installed, the drawing
@  can be read -- and reading it is meeting ARTSAI. Only binding ends it: beaten or fled from, the drawing is still
@  there and the next reading is another try. Bound, the paper stays on the table, blank.
Route21_North_Station_EventScript_VisitorLog::
	goto_if_set FLAG_ARTSAI_BOUND, Route21_North_Station_EventScript_LogBlank
	goto_if_unset FLAG_ARTSAI_PAGE, Route21_North_Station_EventScript_LogOnly
	goto_if_unset FLAG_GOT_REVEAL, Route21_North_Station_EventScript_LogOnly
	lock
	msgbox Route21_North_Station_Text_VisitorLog
	waitse
	playmoncry SPECIES_MEW, CRY_MODE_ENCOUNTER
	waitmoncry
	setwildbattle SPECIES_MEW, 60
	setflag FLAG_SYS_SPECIAL_WILD_BATTLE
	special StartLegendaryBattle
	waitstate
	clearflag FLAG_SYS_SPECIAL_WILD_BATTLE
	specialvar VAR_RESULT, GetBattleOutcome
	goto_if_eq VAR_RESULT, B_OUTCOME_CAUGHT, Route21_North_Station_EventScript_ArtsaiBound
	release
	end

Route21_North_Station_EventScript_ArtsaiBound::
	setflag FLAG_ARTSAI_BOUND
	release
	end

Route21_North_Station_EventScript_LogOnly::
	msgbox Route21_North_Station_Text_VisitorLog, MSGBOX_SIGN
	end

Route21_North_Station_EventScript_LogBlank::
	msgbox Route21_North_Station_Text_VisitorLogBlank, MSGBOX_SIGN
	end

Route21_North_Station_EventScript_DarkScreen::
	msgbox Route21_North_Station_Text_DarkScreen, MSGBOX_SIGN
	end
