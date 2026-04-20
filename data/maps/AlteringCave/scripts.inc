# 1 "data/maps/AlteringCave/scripts.pory"

# 2 "data/maps/AlteringCave/scripts.pory"
AlteringCave_MapScripts::
# 3 "data/maps/AlteringCave/scripts.pory"
	map_script MAP_SCRIPT_ON_RESUME, AlteringCave_OnResume
# 4 "data/maps/AlteringCave/scripts.pory"
	map_script MAP_SCRIPT_ON_TRANSITION, AlteringCave_OnTransition
# 5 "data/maps/AlteringCave/scripts.pory"
	.byte 0
# 6 "data/maps/AlteringCave/scripts.pory"

# 7 "data/maps/AlteringCave/scripts.pory"
AlteringCave_OnTransition:
# 8 "data/maps/AlteringCave/scripts.pory"
	setflag FLAG_LANDMARK_ALTERING_CAVE
# 9 "data/maps/AlteringCave/scripts.pory"
	call_if_unset FLAG_DEFEATED_ZYGARDE, AlteringCave_EventScript_ShowZygarde
# 10 "data/maps/AlteringCave/scripts.pory"
	end
# 11 "data/maps/AlteringCave/scripts.pory"

# 12 "data/maps/AlteringCave/scripts.pory"
AlteringCave_OnResume:
# 13 "data/maps/AlteringCave/scripts.pory"
	call_if_set FLAG_SYS_CTRL_OBJ_DELETE, AlteringCave_EventScript_TryRemoveZygarde
# 14 "data/maps/AlteringCave/scripts.pory"
	end
# 15 "data/maps/AlteringCave/scripts.pory"

# 16 "data/maps/AlteringCave/scripts.pory"
AlteringCave_EventScript_TryRemoveZygarde::
# 17 "data/maps/AlteringCave/scripts.pory"
	specialvar VAR_RESULT, GetBattleOutcome
# 18 "data/maps/AlteringCave/scripts.pory"
	goto_if_ne VAR_RESULT, B_OUTCOME_CAUGHT, Common_EventScript_NopReturn
# 19 "data/maps/AlteringCave/scripts.pory"
	removeobject VAR_LAST_TALKED
# 20 "data/maps/AlteringCave/scripts.pory"
	return
# 21 "data/maps/AlteringCave/scripts.pory"

# 22 "data/maps/AlteringCave/scripts.pory"

# 23 "data/maps/AlteringCave/scripts.pory"
AlteringCave_EventScript_ShowZygarde::
# 24 "data/maps/AlteringCave/scripts.pory"
	clearflag FLAG_HIDE_ZYGARDE
# 25 "data/maps/AlteringCave/scripts.pory"
	return
# 26 "data/maps/AlteringCave/scripts.pory"

# 27 "data/maps/AlteringCave/scripts.pory"
AlteringCave_EventScript_Zygarde::
# 28 "data/maps/AlteringCave/scripts.pory"
	lock
# 29 "data/maps/AlteringCave/scripts.pory"
	faceplayer
# 30 "data/maps/AlteringCave/scripts.pory"
	waitse
# 31 "data/maps/AlteringCave/scripts.pory"
	playmoncry SPECIES_ZYGARDE, CRY_MODE_ENCOUNTER
# 32 "data/maps/AlteringCave/scripts.pory"
	delay 40
# 33 "data/maps/AlteringCave/scripts.pory"
	waitmoncry
# 34 "data/maps/AlteringCave/scripts.pory"
	setwildbattle SPECIES_ZYGARDE, 70
# 35 "data/maps/AlteringCave/scripts.pory"
	setflag FLAG_SYS_CTRL_OBJ_DELETE
# 36 "data/maps/AlteringCave/scripts.pory"
	special BattleSetup_StartLegendaryBattle
# 37 "data/maps/AlteringCave/scripts.pory"
	waitstate
# 38 "data/maps/AlteringCave/scripts.pory"
	clearflag FLAG_SYS_CTRL_OBJ_DELETE
# 39 "data/maps/AlteringCave/scripts.pory"
	specialvar VAR_RESULT, GetBattleOutcome
# 40 "data/maps/AlteringCave/scripts.pory"
	goto_if_eq VAR_RESULT, B_OUTCOME_WON, AlteringCave_EventScript_DefeatedZygarde
# 41 "data/maps/AlteringCave/scripts.pory"
	goto_if_eq VAR_RESULT, B_OUTCOME_RAN, AlteringCave_EventScript_RanFromZygarde
# 42 "data/maps/AlteringCave/scripts.pory"
	goto_if_eq VAR_RESULT, B_OUTCOME_PLAYER_TELEPORTED, AlteringCave_EventScript_RanFromZygarde
# 43 "data/maps/AlteringCave/scripts.pory"
	setflag FLAG_DEFEATED_ZYGARDE
# 44 "data/maps/AlteringCave/scripts.pory"
	release
# 45 "data/maps/AlteringCave/scripts.pory"
	end
# 46 "data/maps/AlteringCave/scripts.pory"

# 47 "data/maps/AlteringCave/scripts.pory"
AlteringCave_EventScript_DefeatedZygarde::
# 48 "data/maps/AlteringCave/scripts.pory"
	setflag FLAG_DEFEATED_ZYGARDE
# 49 "data/maps/AlteringCave/scripts.pory"
	goto Common_EventScript_RemoveStaticPokemon
# 50 "data/maps/AlteringCave/scripts.pory"
	end
# 51 "data/maps/AlteringCave/scripts.pory"

# 52 "data/maps/AlteringCave/scripts.pory"
AlteringCave_EventScript_RanFromZygarde::
# 53 "data/maps/AlteringCave/scripts.pory"
	setvar VAR_0x8004, SPECIES_ZYGARDE
# 54 "data/maps/AlteringCave/scripts.pory"
	goto Common_EventScript_LegendaryFlewAway
# 55 "data/maps/AlteringCave/scripts.pory"
	end
