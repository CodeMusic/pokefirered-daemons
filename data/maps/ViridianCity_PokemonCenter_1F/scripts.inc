ViridianCity_PokemonCenter_1F_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, ViridianCity_PokemonCenter_1F_OnTransition
	map_script MAP_SCRIPT_ON_RESUME, CableClub_OnResume
	.byte 0

ViridianCity_PokemonCenter_1F_OnTransition::
	setrespawn HEAL_LOCATION_VIRIDIAN_CITY
	end

ViridianCity_PokemonCenter_1F_EventScript_Nurse::
	lock
	faceplayer
	call EventScript_PkmnCenterNurse
	release
	end

ViridianCity_PokemonCenter_1F_EventScript_Boy::
	msgbox ViridianCity_PokemonCenter_1F_Text_PokeCenterInEveryTown, MSGBOX_NPC
	end

ViridianCity_PokemonCenter_1F_EventScript_Gentleman::
	msgbox ViridianCity_PokemonCenter_1F_Text_FeelFreeToUsePC, MSGBOX_NPC
	end

ViridianCity_PokemonCenter_1F_EventScript_Youngster::
	msgbox ViridianCity_PokemonCenter_1F_Text_PokeCentersHealMons, MSGBOX_NPC
	end

@  T-439 (the user, 2026-10-22; DAEMONS docs/private/plan-cast-2026-10-22.md, tools/gencast.py): a MYNAH, the STREAM's
@  superfan -- the bird that repeats every word it hears. He names one lecture you hold and have not watched (a MARK's
@  or an understanding's, src/daemons_cast.c); with none left he recites CRYSTAL's sign-off. Every word DRAFT.
ViridianCity_PokemonCenter_1F_EventScript_Mynah::
	lock
	faceplayer
	special DaemonsCast_MynahTip
	goto_if_eq VAR_RESULT, 1, ViridianCity_PokemonCenter_1F_EventScript_MynahTip
	goto_if_eq VAR_RESULT, 2, ViridianCity_PokemonCenter_1F_EventScript_MynahSignOff
	msgbox ViridianCity_PokemonCenter_1F_Text_MynahEveryStream
	release
	end

ViridianCity_PokemonCenter_1F_EventScript_MynahTip::
	msgbox ViridianCity_PokemonCenter_1F_Text_MynahTip
	release
	end

ViridianCity_PokemonCenter_1F_EventScript_MynahSignOff::
	msgbox ViridianCity_PokemonCenter_1F_Text_MynahSignOff
	release
	end

@ T-496 (DRAFT): the lioness, the first of four meetings -- the helper beside the NURSE. Gone once the first MARK is held.
ViridianCity_PokemonCenter_1F_EventScript_Lioness::
	msgbox ViridianCity_PokemonCenter_1F_Text_Lioness, MSGBOX_NPC
	end
