# 1 "data/maps/DistortionWorld/scripts.pory"

# 2 "data/maps/DistortionWorld/scripts.pory"
DistortionWorld_MapScripts::
# 3 "data/maps/DistortionWorld/scripts.pory"
	map_script MAP_SCRIPT_ON_RESUME, DistortionWorld_OnResume
# 4 "data/maps/DistortionWorld/scripts.pory"
	map_script MAP_SCRIPT_ON_TRANSITION, DistortionWorld_OnTransition
# 5 "data/maps/DistortionWorld/scripts.pory"
	.byte 0
# 6 "data/maps/DistortionWorld/scripts.pory"

# 7 "data/maps/DistortionWorld/scripts.pory"
DistortionWorld_OnResume:
# 8 "data/maps/DistortionWorld/scripts.pory"
	call_if_set FLAG_SYS_CTRL_OBJ_DELETE, DistortionWorld_EventScript_TryRemoveGiratina
# 9 "data/maps/DistortionWorld/scripts.pory"
	call_if_set FLAG_SYS_CTRL_OBJ_DELETE, DistortionWorld_EventScript_TryRemoveYveltal
# 10 "data/maps/DistortionWorld/scripts.pory"
	end
# 11 "data/maps/DistortionWorld/scripts.pory"

# 12 "data/maps/DistortionWorld/scripts.pory"
DistortionWorld_EventScript_TryRemoveGiratina::
# 13 "data/maps/DistortionWorld/scripts.pory"
	specialvar VAR_RESULT, GetBattleOutcome
# 14 "data/maps/DistortionWorld/scripts.pory"
	goto_if_ne VAR_RESULT, B_OUTCOME_CAUGHT, Common_EventScript_NopReturn
# 15 "data/maps/DistortionWorld/scripts.pory"
	removeobject VAR_LAST_TALKED
# 16 "data/maps/DistortionWorld/scripts.pory"
	return
# 17 "data/maps/DistortionWorld/scripts.pory"

# 18 "data/maps/DistortionWorld/scripts.pory"
DistortionWorld_EventScript_TryRemoveYveltal::
# 19 "data/maps/DistortionWorld/scripts.pory"
	specialvar VAR_RESULT, GetBattleOutcome
# 20 "data/maps/DistortionWorld/scripts.pory"
	goto_if_ne VAR_RESULT, B_OUTCOME_CAUGHT, Common_EventScript_NopReturn
# 21 "data/maps/DistortionWorld/scripts.pory"
	removeobject VAR_LAST_TALKED
# 22 "data/maps/DistortionWorld/scripts.pory"
	return
# 23 "data/maps/DistortionWorld/scripts.pory"

# 24 "data/maps/DistortionWorld/scripts.pory"

# 25 "data/maps/DistortionWorld/scripts.pory"
DistortionWorld_OnTransition:
# 26 "data/maps/DistortionWorld/scripts.pory"
	call_if_unset FLAG_DEFEATED_GIRATINA, DistortionWorld_EventScript_ShowGiratina
# 27 "data/maps/DistortionWorld/scripts.pory"
	call_if_unset FLAG_DEFEATED_YVELTAL, DistortionWorld_EventScript_ShowYveltal
# 28 "data/maps/DistortionWorld/scripts.pory"
	end
# 29 "data/maps/DistortionWorld/scripts.pory"

# 30 "data/maps/DistortionWorld/scripts.pory"
DistortionWorld_EventScript_ShowGiratina::
# 31 "data/maps/DistortionWorld/scripts.pory"
	clearflag FLAG_HIDE_GIRATINA
# 32 "data/maps/DistortionWorld/scripts.pory"
	return
# 33 "data/maps/DistortionWorld/scripts.pory"

# 34 "data/maps/DistortionWorld/scripts.pory"
DistortionWorld_EventScript_ShowYveltal::
# 35 "data/maps/DistortionWorld/scripts.pory"
	clearflag FLAG_HIDE_YVELTAL
# 36 "data/maps/DistortionWorld/scripts.pory"
	return
# 37 "data/maps/DistortionWorld/scripts.pory"

# 38 "data/maps/DistortionWorld/scripts.pory"
DistortionWorld_EventScript_Giratina::
# 39 "data/maps/DistortionWorld/scripts.pory"
	lock
# 40 "data/maps/DistortionWorld/scripts.pory"
	faceplayer
# 41 "data/maps/DistortionWorld/scripts.pory"
	waitse
# 42 "data/maps/DistortionWorld/scripts.pory"
	playmoncry SPECIES_GIRATINA_ORIGIN, CRY_MODE_ENCOUNTER
# 43 "data/maps/DistortionWorld/scripts.pory"
	delay 40
# 44 "data/maps/DistortionWorld/scripts.pory"
	waitmoncry
# 45 "data/maps/DistortionWorld/scripts.pory"
	setwildbattle SPECIES_GIRATINA_ORIGIN, 70
# 46 "data/maps/DistortionWorld/scripts.pory"
	setflag FLAG_SYS_CTRL_OBJ_DELETE
# 47 "data/maps/DistortionWorld/scripts.pory"
	special BattleSetup_StartLegendaryBattle
# 48 "data/maps/DistortionWorld/scripts.pory"
	waitstate
# 49 "data/maps/DistortionWorld/scripts.pory"
	clearflag FLAG_SYS_CTRL_OBJ_DELETE
# 50 "data/maps/DistortionWorld/scripts.pory"
	specialvar VAR_RESULT, GetBattleOutcome
# 51 "data/maps/DistortionWorld/scripts.pory"
	goto_if_eq VAR_RESULT, B_OUTCOME_WON, DistortionWorld_EventScript_DefeatedGiratina
# 52 "data/maps/DistortionWorld/scripts.pory"
	goto_if_eq VAR_RESULT, B_OUTCOME_RAN, DistortionWorld_EventScript_RanFromGiratina
# 53 "data/maps/DistortionWorld/scripts.pory"
	goto_if_eq VAR_RESULT, B_OUTCOME_PLAYER_TELEPORTED, DistortionWorld_EventScript_RanFromGiratina
# 54 "data/maps/DistortionWorld/scripts.pory"
	setflag FLAG_DEFEATED_GIRATINA
# 55 "data/maps/DistortionWorld/scripts.pory"
	release
# 56 "data/maps/DistortionWorld/scripts.pory"
	end
# 57 "data/maps/DistortionWorld/scripts.pory"

# 58 "data/maps/DistortionWorld/scripts.pory"
DistortionWorld_EventScript_DefeatedGiratina::
# 59 "data/maps/DistortionWorld/scripts.pory"
	setflag FLAG_DEFEATED_GIRATINA
# 60 "data/maps/DistortionWorld/scripts.pory"
	goto Common_EventScript_RemoveStaticPokemon
# 61 "data/maps/DistortionWorld/scripts.pory"
	end
# 62 "data/maps/DistortionWorld/scripts.pory"

# 63 "data/maps/DistortionWorld/scripts.pory"
DistortionWorld_EventScript_RanFromGiratina::
# 64 "data/maps/DistortionWorld/scripts.pory"
	setvar VAR_0x8004, SPECIES_GIRATINA_ORIGIN
# 65 "data/maps/DistortionWorld/scripts.pory"
	goto Common_EventScript_LegendaryFlewAway
# 66 "data/maps/DistortionWorld/scripts.pory"
	end
# 67 "data/maps/DistortionWorld/scripts.pory"

# 68 "data/maps/DistortionWorld/scripts.pory"

# 69 "data/maps/DistortionWorld/scripts.pory"
DistortionWorld_EventScript_Yveltal::
# 70 "data/maps/DistortionWorld/scripts.pory"
	lock
# 71 "data/maps/DistortionWorld/scripts.pory"
	faceplayer
# 72 "data/maps/DistortionWorld/scripts.pory"
	waitse
# 73 "data/maps/DistortionWorld/scripts.pory"
	playmoncry SPECIES_YVELTAL, CRY_MODE_ENCOUNTER
# 74 "data/maps/DistortionWorld/scripts.pory"
	delay 40
# 75 "data/maps/DistortionWorld/scripts.pory"
	waitmoncry
# 76 "data/maps/DistortionWorld/scripts.pory"
	setwildbattle SPECIES_YVELTAL, 70
# 77 "data/maps/DistortionWorld/scripts.pory"
	setflag FLAG_SYS_CTRL_OBJ_DELETE
# 78 "data/maps/DistortionWorld/scripts.pory"
	special BattleSetup_StartLegendaryBattle
# 79 "data/maps/DistortionWorld/scripts.pory"
	waitstate
# 80 "data/maps/DistortionWorld/scripts.pory"
	clearflag FLAG_SYS_CTRL_OBJ_DELETE
# 81 "data/maps/DistortionWorld/scripts.pory"
	specialvar VAR_RESULT, GetBattleOutcome
# 82 "data/maps/DistortionWorld/scripts.pory"
	goto_if_eq VAR_RESULT, B_OUTCOME_WON, DistortionWorld_EventScript_DefeatedYveltal
# 83 "data/maps/DistortionWorld/scripts.pory"
	goto_if_eq VAR_RESULT, B_OUTCOME_RAN, DistortionWorld_EventScript_RanFromYveltal
# 84 "data/maps/DistortionWorld/scripts.pory"
	goto_if_eq VAR_RESULT, B_OUTCOME_PLAYER_TELEPORTED, DistortionWorld_EventScript_RanFromYveltal
# 85 "data/maps/DistortionWorld/scripts.pory"
	setflag FLAG_DEFEATED_YVELTAL
# 86 "data/maps/DistortionWorld/scripts.pory"
	release
# 87 "data/maps/DistortionWorld/scripts.pory"
	end
# 88 "data/maps/DistortionWorld/scripts.pory"

# 89 "data/maps/DistortionWorld/scripts.pory"
DistortionWorld_EventScript_DefeatedYveltal::
# 90 "data/maps/DistortionWorld/scripts.pory"
	setflag FLAG_DEFEATED_YVELTAL
# 91 "data/maps/DistortionWorld/scripts.pory"
	goto Common_EventScript_RemoveStaticPokemon
# 92 "data/maps/DistortionWorld/scripts.pory"
	end
# 93 "data/maps/DistortionWorld/scripts.pory"

# 94 "data/maps/DistortionWorld/scripts.pory"
DistortionWorld_EventScript_RanFromYveltal::
# 95 "data/maps/DistortionWorld/scripts.pory"
	setvar VAR_0x8004, SPECIES_YVELTAL
# 96 "data/maps/DistortionWorld/scripts.pory"
	goto Common_EventScript_LegendaryFlewAway
# 97 "data/maps/DistortionWorld/scripts.pory"
	end
