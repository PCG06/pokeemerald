# 1 "data/maps/SlumberingWeald/scripts.pory"

# 2 "data/maps/SlumberingWeald/scripts.pory"
SlumberingWeald_MapScripts::
# 3 "data/maps/SlumberingWeald/scripts.pory"
	map_script MAP_SCRIPT_ON_RESUME, SlumberingWeald_OnResume
# 4 "data/maps/SlumberingWeald/scripts.pory"
	map_script MAP_SCRIPT_ON_TRANSITION, SlumberingWeald_OnTransition
# 5 "data/maps/SlumberingWeald/scripts.pory"
	.byte 0
# 6 "data/maps/SlumberingWeald/scripts.pory"

# 7 "data/maps/SlumberingWeald/scripts.pory"
SlumberingWeald_OnResume:
# 8 "data/maps/SlumberingWeald/scripts.pory"
	call_if_set FLAG_SYS_CTRL_OBJ_DELETE, SlumberingWeald_EventScript_TryRemoveZacian
# 9 "data/maps/SlumberingWeald/scripts.pory"
	call_if_set FLAG_SYS_CTRL_OBJ_DELETE, SlumberingWeald_EventScript_TryRemoveZamazenta
# 10 "data/maps/SlumberingWeald/scripts.pory"
	end
# 11 "data/maps/SlumberingWeald/scripts.pory"

# 12 "data/maps/SlumberingWeald/scripts.pory"
SlumberingWeald_EventScript_TryRemoveZacian::
# 13 "data/maps/SlumberingWeald/scripts.pory"
	specialvar VAR_RESULT, GetBattleOutcome
# 14 "data/maps/SlumberingWeald/scripts.pory"
	goto_if_ne VAR_RESULT, B_OUTCOME_CAUGHT, Common_EventScript_NopReturn
# 15 "data/maps/SlumberingWeald/scripts.pory"
	removeobject VAR_LAST_TALKED
# 16 "data/maps/SlumberingWeald/scripts.pory"
	return
# 17 "data/maps/SlumberingWeald/scripts.pory"

# 18 "data/maps/SlumberingWeald/scripts.pory"
SlumberingWeald_EventScript_TryRemoveZamazenta::
# 19 "data/maps/SlumberingWeald/scripts.pory"
	specialvar VAR_RESULT, GetBattleOutcome
# 20 "data/maps/SlumberingWeald/scripts.pory"
	goto_if_ne VAR_RESULT, B_OUTCOME_CAUGHT, Common_EventScript_NopReturn
# 21 "data/maps/SlumberingWeald/scripts.pory"
	removeobject VAR_LAST_TALKED
# 22 "data/maps/SlumberingWeald/scripts.pory"
	return
# 23 "data/maps/SlumberingWeald/scripts.pory"

# 24 "data/maps/SlumberingWeald/scripts.pory"

# 25 "data/maps/SlumberingWeald/scripts.pory"
SlumberingWeald_OnTransition:
# 26 "data/maps/SlumberingWeald/scripts.pory"
	call_if_unset FLAG_DEFEATED_ZACIAN, SlumberingWeald_EventScript_ShowZacian
# 27 "data/maps/SlumberingWeald/scripts.pory"
	call_if_unset FLAG_DEFEATED_ZAMAZENTA, SlumberingWeald_EventScript_ShowZamazenta
# 28 "data/maps/SlumberingWeald/scripts.pory"
	end
# 29 "data/maps/SlumberingWeald/scripts.pory"

# 30 "data/maps/SlumberingWeald/scripts.pory"
SlumberingWeald_EventScript_ShowZacian::
# 31 "data/maps/SlumberingWeald/scripts.pory"
	clearflag FLAG_HIDE_ZACIAN
# 32 "data/maps/SlumberingWeald/scripts.pory"
	return
# 33 "data/maps/SlumberingWeald/scripts.pory"

# 34 "data/maps/SlumberingWeald/scripts.pory"
SlumberingWeald_EventScript_ShowZamazenta::
# 35 "data/maps/SlumberingWeald/scripts.pory"
	clearflag FLAG_HIDE_ZAMAZENTA
# 36 "data/maps/SlumberingWeald/scripts.pory"
	return
# 37 "data/maps/SlumberingWeald/scripts.pory"

# 38 "data/maps/SlumberingWeald/scripts.pory"
SlumberingWeald_EventScript_Zacian::
# 39 "data/maps/SlumberingWeald/scripts.pory"
	lock
# 40 "data/maps/SlumberingWeald/scripts.pory"
	faceplayer
# 41 "data/maps/SlumberingWeald/scripts.pory"
	waitse
# 42 "data/maps/SlumberingWeald/scripts.pory"
	playmoncry SPECIES_ZACIAN, CRY_MODE_ENCOUNTER
# 43 "data/maps/SlumberingWeald/scripts.pory"
	delay 40
# 44 "data/maps/SlumberingWeald/scripts.pory"
	waitmoncry
# 45 "data/maps/SlumberingWeald/scripts.pory"
	setwildbattle SPECIES_ZACIAN, 70
# 46 "data/maps/SlumberingWeald/scripts.pory"
	setflag FLAG_SYS_CTRL_OBJ_DELETE
# 47 "data/maps/SlumberingWeald/scripts.pory"
	special BattleSetup_StartLegendaryBattle
# 48 "data/maps/SlumberingWeald/scripts.pory"
	waitstate
# 49 "data/maps/SlumberingWeald/scripts.pory"
	clearflag FLAG_SYS_CTRL_OBJ_DELETE
# 50 "data/maps/SlumberingWeald/scripts.pory"
	specialvar VAR_RESULT, GetBattleOutcome
# 51 "data/maps/SlumberingWeald/scripts.pory"
	goto_if_eq VAR_RESULT, B_OUTCOME_WON, SlumberingWeald_EventScript_DefeatedZacian
# 52 "data/maps/SlumberingWeald/scripts.pory"
	goto_if_eq VAR_RESULT, B_OUTCOME_RAN, SlumberingWeald_EventScript_RanFromZacian
# 53 "data/maps/SlumberingWeald/scripts.pory"
	goto_if_eq VAR_RESULT, B_OUTCOME_PLAYER_TELEPORTED, SlumberingWeald_EventScript_RanFromZacian
# 54 "data/maps/SlumberingWeald/scripts.pory"
	setflag FLAG_DEFEATED_ZACIAN
# 55 "data/maps/SlumberingWeald/scripts.pory"
	release
# 56 "data/maps/SlumberingWeald/scripts.pory"
	end
# 57 "data/maps/SlumberingWeald/scripts.pory"

# 58 "data/maps/SlumberingWeald/scripts.pory"
SlumberingWeald_EventScript_DefeatedZacian::
# 59 "data/maps/SlumberingWeald/scripts.pory"
	setflag FLAG_DEFEATED_ZACIAN
# 60 "data/maps/SlumberingWeald/scripts.pory"
	goto Common_EventScript_RemoveStaticPokemon
# 61 "data/maps/SlumberingWeald/scripts.pory"
	end
# 62 "data/maps/SlumberingWeald/scripts.pory"

# 63 "data/maps/SlumberingWeald/scripts.pory"
SlumberingWeald_EventScript_RanFromZacian::
# 64 "data/maps/SlumberingWeald/scripts.pory"
	setvar VAR_0x8004, SPECIES_ZACIAN
# 65 "data/maps/SlumberingWeald/scripts.pory"
	goto Common_EventScript_LegendaryFlewAway
# 66 "data/maps/SlumberingWeald/scripts.pory"
	end
# 67 "data/maps/SlumberingWeald/scripts.pory"

# 68 "data/maps/SlumberingWeald/scripts.pory"

# 69 "data/maps/SlumberingWeald/scripts.pory"
SlumberingWeald_EventScript_Zamazenta::
# 70 "data/maps/SlumberingWeald/scripts.pory"
	lock
# 71 "data/maps/SlumberingWeald/scripts.pory"
	faceplayer
# 72 "data/maps/SlumberingWeald/scripts.pory"
	waitse
# 73 "data/maps/SlumberingWeald/scripts.pory"
	playmoncry SPECIES_ZAMAZENTA, CRY_MODE_ENCOUNTER
# 74 "data/maps/SlumberingWeald/scripts.pory"
	delay 40
# 75 "data/maps/SlumberingWeald/scripts.pory"
	waitmoncry
# 76 "data/maps/SlumberingWeald/scripts.pory"
	setwildbattle SPECIES_ZAMAZENTA, 70
# 77 "data/maps/SlumberingWeald/scripts.pory"
	setflag FLAG_SYS_CTRL_OBJ_DELETE
# 78 "data/maps/SlumberingWeald/scripts.pory"
	special BattleSetup_StartLegendaryBattle
# 79 "data/maps/SlumberingWeald/scripts.pory"
	waitstate
# 80 "data/maps/SlumberingWeald/scripts.pory"
	clearflag FLAG_SYS_CTRL_OBJ_DELETE
# 81 "data/maps/SlumberingWeald/scripts.pory"
	specialvar VAR_RESULT, GetBattleOutcome
# 82 "data/maps/SlumberingWeald/scripts.pory"
	goto_if_eq VAR_RESULT, B_OUTCOME_WON, SlumberingWeald_EventScript_DefeatedZamazenta
# 83 "data/maps/SlumberingWeald/scripts.pory"
	goto_if_eq VAR_RESULT, B_OUTCOME_RAN, SlumberingWeald_EventScript_RanFromZamazenta
# 84 "data/maps/SlumberingWeald/scripts.pory"
	goto_if_eq VAR_RESULT, B_OUTCOME_PLAYER_TELEPORTED, SlumberingWeald_EventScript_RanFromZamazenta
# 85 "data/maps/SlumberingWeald/scripts.pory"
	setflag FLAG_DEFEATED_ZAMAZENTA
# 86 "data/maps/SlumberingWeald/scripts.pory"
	release
# 87 "data/maps/SlumberingWeald/scripts.pory"
	end
# 88 "data/maps/SlumberingWeald/scripts.pory"

# 89 "data/maps/SlumberingWeald/scripts.pory"
SlumberingWeald_EventScript_DefeatedZamazenta::
# 90 "data/maps/SlumberingWeald/scripts.pory"
	setflag FLAG_DEFEATED_ZAMAZENTA
# 91 "data/maps/SlumberingWeald/scripts.pory"
	goto Common_EventScript_RemoveStaticPokemon
# 92 "data/maps/SlumberingWeald/scripts.pory"
	end
# 93 "data/maps/SlumberingWeald/scripts.pory"

# 94 "data/maps/SlumberingWeald/scripts.pory"
SlumberingWeald_EventScript_RanFromZamazenta::
# 95 "data/maps/SlumberingWeald/scripts.pory"
	setvar VAR_0x8004, SPECIES_ZAMAZENTA
# 96 "data/maps/SlumberingWeald/scripts.pory"
	goto Common_EventScript_LegendaryFlewAway
# 97 "data/maps/SlumberingWeald/scripts.pory"
	end
