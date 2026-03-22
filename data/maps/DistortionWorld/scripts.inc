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
	end
# 10 "data/maps/DistortionWorld/scripts.pory"

# 11 "data/maps/DistortionWorld/scripts.pory"
DistortionWorld_EventScript_TryRemoveGiratina::
# 12 "data/maps/DistortionWorld/scripts.pory"
	specialvar VAR_RESULT, GetBattleOutcome
# 13 "data/maps/DistortionWorld/scripts.pory"
	goto_if_ne VAR_RESULT, B_OUTCOME_CAUGHT, Common_EventScript_NopReturn
# 14 "data/maps/DistortionWorld/scripts.pory"
	removeobject VAR_LAST_TALKED
# 15 "data/maps/DistortionWorld/scripts.pory"
	return
# 16 "data/maps/DistortionWorld/scripts.pory"

# 17 "data/maps/DistortionWorld/scripts.pory"
DistortionWorld_OnTransition:
# 18 "data/maps/DistortionWorld/scripts.pory"
	call_if_unset FLAG_DEFEATED_GIRATINA, DistortionWorld_EventScript_ShowGiratina
# 19 "data/maps/DistortionWorld/scripts.pory"
	end
# 20 "data/maps/DistortionWorld/scripts.pory"

# 21 "data/maps/DistortionWorld/scripts.pory"
DistortionWorld_EventScript_ShowGiratina::
# 22 "data/maps/DistortionWorld/scripts.pory"
	clearflag FLAG_HIDE_GIRATINA
# 23 "data/maps/DistortionWorld/scripts.pory"
	return
# 24 "data/maps/DistortionWorld/scripts.pory"

# 25 "data/maps/DistortionWorld/scripts.pory"
DistortionWorld_EventScript_Giratina::
# 26 "data/maps/DistortionWorld/scripts.pory"
	lock
# 27 "data/maps/DistortionWorld/scripts.pory"
	faceplayer
# 28 "data/maps/DistortionWorld/scripts.pory"
	waitse
# 29 "data/maps/DistortionWorld/scripts.pory"
	playmoncry SPECIES_GIRATINA_ORIGIN, CRY_MODE_ENCOUNTER
# 30 "data/maps/DistortionWorld/scripts.pory"
	delay 40
# 31 "data/maps/DistortionWorld/scripts.pory"
	waitmoncry
# 32 "data/maps/DistortionWorld/scripts.pory"
	setwildbattle SPECIES_GIRATINA_ORIGIN, 70
# 33 "data/maps/DistortionWorld/scripts.pory"
	setflag FLAG_SYS_CTRL_OBJ_DELETE
# 34 "data/maps/DistortionWorld/scripts.pory"
	special BattleSetup_StartLegendaryBattle
# 35 "data/maps/DistortionWorld/scripts.pory"
	waitstate
# 36 "data/maps/DistortionWorld/scripts.pory"
	clearflag FLAG_SYS_CTRL_OBJ_DELETE
# 37 "data/maps/DistortionWorld/scripts.pory"
	specialvar VAR_RESULT, GetBattleOutcome
# 38 "data/maps/DistortionWorld/scripts.pory"
	goto_if_eq VAR_RESULT, B_OUTCOME_WON, DistortionWorld_EventScript_DefeatedGiratina
# 39 "data/maps/DistortionWorld/scripts.pory"
	goto_if_eq VAR_RESULT, B_OUTCOME_RAN, DistortionWorld_EventScript_RanFromGiratina
# 40 "data/maps/DistortionWorld/scripts.pory"
	goto_if_eq VAR_RESULT, B_OUTCOME_PLAYER_TELEPORTED, DistortionWorld_EventScript_RanFromGiratina
# 41 "data/maps/DistortionWorld/scripts.pory"
	setflag FLAG_DEFEATED_GIRATINA
# 42 "data/maps/DistortionWorld/scripts.pory"
	release
# 43 "data/maps/DistortionWorld/scripts.pory"
	end
# 44 "data/maps/DistortionWorld/scripts.pory"

# 45 "data/maps/DistortionWorld/scripts.pory"
DistortionWorld_EventScript_DefeatedGiratina::
# 46 "data/maps/DistortionWorld/scripts.pory"
	setflag FLAG_DEFEATED_GIRATINA
# 47 "data/maps/DistortionWorld/scripts.pory"
	goto Common_EventScript_RemoveStaticPokemon
# 48 "data/maps/DistortionWorld/scripts.pory"
	end
# 49 "data/maps/DistortionWorld/scripts.pory"

# 50 "data/maps/DistortionWorld/scripts.pory"
DistortionWorld_EventScript_RanFromGiratina::
# 51 "data/maps/DistortionWorld/scripts.pory"
	setvar VAR_0x8004, SPECIES_GIRATINA_ORIGIN
# 52 "data/maps/DistortionWorld/scripts.pory"
	goto Common_EventScript_LegendaryFlewAway
# 53 "data/maps/DistortionWorld/scripts.pory"
	end
