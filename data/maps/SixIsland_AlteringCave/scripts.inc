SixIsland_AlteringCave_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, SixIsland_AlteringCave_OnTransition
	.byte 0

SixIsland_AlteringCave_OnTransition::
	setworldmapflag FLAG_WORLD_MAP_SIX_ISLAND_ALTERING_CAVE
	call SixIsland_AlteringCave_EventScript_NextLight
	end

@ T-438: OTHER CAVE's light changes on each entry, in a cycle of four a player can learn (teal, amber, chrysoprase,
@ rose quartz), and its wild daemons follow the light: visit n (1..4, then round again) is light n - 1 and wild set
@ n - 1. Stepped here, once per entry, never when the palettes reload (fieldmap.c reads the var).
SixIsland_AlteringCave_EventScript_NextLight::
	addvar VAR_OTHER_CAVE_LIGHT, 1
	goto_if_le VAR_OTHER_CAVE_LIGHT, 4, SixIsland_AlteringCave_EventScript_SetWildSet
	setvar VAR_OTHER_CAVE_LIGHT, 1
SixIsland_AlteringCave_EventScript_SetWildSet::
	copyvar VAR_ALTERING_CAVE_WILD_SET, VAR_OTHER_CAVE_LIGHT
	subvar VAR_ALTERING_CAVE_WILD_SET, 1
	return
