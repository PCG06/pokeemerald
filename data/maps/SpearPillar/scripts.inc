# 1 "data/maps/SpearPillar/scripts.pory"

# 2 "data/maps/SpearPillar/scripts.pory"
SpearPillar_MapScripts::
# 3 "data/maps/SpearPillar/scripts.pory"
	map_script MAP_SCRIPT_ON_RESUME, SpearPillar_OnResume
# 4 "data/maps/SpearPillar/scripts.pory"
	map_script MAP_SCRIPT_ON_TRANSITION, SpearPillar_OnTransition
# 5 "data/maps/SpearPillar/scripts.pory"
	.byte 0
# 6 "data/maps/SpearPillar/scripts.pory"

# 7 "data/maps/SpearPillar/scripts.pory"
SpearPillar_OnResume:
# 8 "data/maps/SpearPillar/scripts.pory"
	call_if_set FLAG_SYS_CTRL_OBJ_DELETE, SpearPillar_EventScript_TryRemoveDialga
# 9 "data/maps/SpearPillar/scripts.pory"
	call_if_set FLAG_SYS_CTRL_OBJ_DELETE, SpearPillar_EventScript_TryRemovePalkia
# 10 "data/maps/SpearPillar/scripts.pory"
	end
# 11 "data/maps/SpearPillar/scripts.pory"

# 12 "data/maps/SpearPillar/scripts.pory"
SpearPillar_EventScript_TryRemoveDialga::
# 13 "data/maps/SpearPillar/scripts.pory"
	specialvar VAR_RESULT, GetBattleOutcome
# 14 "data/maps/SpearPillar/scripts.pory"
	goto_if_ne VAR_RESULT, B_OUTCOME_CAUGHT, Common_EventScript_NopReturn
# 15 "data/maps/SpearPillar/scripts.pory"
	removeobject VAR_LAST_TALKED
# 16 "data/maps/SpearPillar/scripts.pory"
	return
# 17 "data/maps/SpearPillar/scripts.pory"

# 18 "data/maps/SpearPillar/scripts.pory"
SpearPillar_EventScript_TryRemovePalkia::
# 19 "data/maps/SpearPillar/scripts.pory"
	specialvar VAR_RESULT, GetBattleOutcome
# 20 "data/maps/SpearPillar/scripts.pory"
	goto_if_ne VAR_RESULT, B_OUTCOME_CAUGHT, Common_EventScript_NopReturn
# 21 "data/maps/SpearPillar/scripts.pory"
	removeobject VAR_LAST_TALKED
# 22 "data/maps/SpearPillar/scripts.pory"
	return
# 23 "data/maps/SpearPillar/scripts.pory"

# 24 "data/maps/SpearPillar/scripts.pory"

# 25 "data/maps/SpearPillar/scripts.pory"
SpearPillar_OnTransition:
# 26 "data/maps/SpearPillar/scripts.pory"
	call_if_unset FLAG_DEFEATED_DIALGA, SpearPillar_EventScript_ShowDialga
# 27 "data/maps/SpearPillar/scripts.pory"
	call_if_unset FLAG_DEFEATED_PALKIA, SpearPillar_EventScript_ShowPalkia
# 28 "data/maps/SpearPillar/scripts.pory"
	end
# 29 "data/maps/SpearPillar/scripts.pory"

# 30 "data/maps/SpearPillar/scripts.pory"
SpearPillar_EventScript_ShowDialga::
# 31 "data/maps/SpearPillar/scripts.pory"
	clearflag FLAG_HIDE_DIALGA
# 32 "data/maps/SpearPillar/scripts.pory"
	return
# 33 "data/maps/SpearPillar/scripts.pory"

# 34 "data/maps/SpearPillar/scripts.pory"
SpearPillar_EventScript_ShowPalkia::
# 35 "data/maps/SpearPillar/scripts.pory"
	clearflag FLAG_HIDE_PALKIA
# 36 "data/maps/SpearPillar/scripts.pory"
	return
# 37 "data/maps/SpearPillar/scripts.pory"

# 38 "data/maps/SpearPillar/scripts.pory"
SpearPillar_EventScript_Dialga::
# 39 "data/maps/SpearPillar/scripts.pory"
	lock
# 40 "data/maps/SpearPillar/scripts.pory"
	faceplayer
# 41 "data/maps/SpearPillar/scripts.pory"
	waitse
# 42 "data/maps/SpearPillar/scripts.pory"
	playmoncry SPECIES_DIALGA, CRY_MODE_ENCOUNTER
# 43 "data/maps/SpearPillar/scripts.pory"
	delay 40
# 44 "data/maps/SpearPillar/scripts.pory"
	waitmoncry
# 45 "data/maps/SpearPillar/scripts.pory"
	setwildbattle SPECIES_DIALGA, 70
# 46 "data/maps/SpearPillar/scripts.pory"
	setflag FLAG_SYS_CTRL_OBJ_DELETE
# 47 "data/maps/SpearPillar/scripts.pory"
	special BattleSetup_StartLegendaryBattle
# 48 "data/maps/SpearPillar/scripts.pory"
	waitstate
# 49 "data/maps/SpearPillar/scripts.pory"
	clearflag FLAG_SYS_CTRL_OBJ_DELETE
# 50 "data/maps/SpearPillar/scripts.pory"
	specialvar VAR_RESULT, GetBattleOutcome
# 51 "data/maps/SpearPillar/scripts.pory"
	goto_if_eq VAR_RESULT, B_OUTCOME_WON, SpearPillar_EventScript_DefeatedDialga
# 52 "data/maps/SpearPillar/scripts.pory"
	goto_if_eq VAR_RESULT, B_OUTCOME_RAN, SpearPillar_EventScript_RanFromDialga
# 53 "data/maps/SpearPillar/scripts.pory"
	goto_if_eq VAR_RESULT, B_OUTCOME_PLAYER_TELEPORTED, SpearPillar_EventScript_RanFromDialga
# 54 "data/maps/SpearPillar/scripts.pory"
	setflag FLAG_DEFEATED_DIALGA
# 55 "data/maps/SpearPillar/scripts.pory"
	release
# 56 "data/maps/SpearPillar/scripts.pory"
	end
# 57 "data/maps/SpearPillar/scripts.pory"

# 58 "data/maps/SpearPillar/scripts.pory"
SpearPillar_EventScript_DefeatedDialga::
# 59 "data/maps/SpearPillar/scripts.pory"
	setflag FLAG_DEFEATED_DIALGA
# 60 "data/maps/SpearPillar/scripts.pory"
	goto Common_EventScript_RemoveStaticPokemon
# 61 "data/maps/SpearPillar/scripts.pory"
	end
# 62 "data/maps/SpearPillar/scripts.pory"

# 63 "data/maps/SpearPillar/scripts.pory"
SpearPillar_EventScript_RanFromDialga::
# 64 "data/maps/SpearPillar/scripts.pory"
	setvar VAR_0x8004, SPECIES_DIALGA
# 65 "data/maps/SpearPillar/scripts.pory"
	goto Common_EventScript_LegendaryFlewAway
# 66 "data/maps/SpearPillar/scripts.pory"
	end
# 67 "data/maps/SpearPillar/scripts.pory"

# 68 "data/maps/SpearPillar/scripts.pory"

# 69 "data/maps/SpearPillar/scripts.pory"
SpearPillar_EventScript_Palkia::
# 70 "data/maps/SpearPillar/scripts.pory"
	lock
# 71 "data/maps/SpearPillar/scripts.pory"
	faceplayer
# 72 "data/maps/SpearPillar/scripts.pory"
	waitse
# 73 "data/maps/SpearPillar/scripts.pory"
	playmoncry SPECIES_PALKIA, CRY_MODE_ENCOUNTER
# 74 "data/maps/SpearPillar/scripts.pory"
	delay 40
# 75 "data/maps/SpearPillar/scripts.pory"
	waitmoncry
# 76 "data/maps/SpearPillar/scripts.pory"
	setwildbattle SPECIES_PALKIA, 70
# 77 "data/maps/SpearPillar/scripts.pory"
	setflag FLAG_SYS_CTRL_OBJ_DELETE
# 78 "data/maps/SpearPillar/scripts.pory"
	special BattleSetup_StartLegendaryBattle
# 79 "data/maps/SpearPillar/scripts.pory"
	waitstate
# 80 "data/maps/SpearPillar/scripts.pory"
	clearflag FLAG_SYS_CTRL_OBJ_DELETE
# 81 "data/maps/SpearPillar/scripts.pory"
	specialvar VAR_RESULT, GetBattleOutcome
# 82 "data/maps/SpearPillar/scripts.pory"
	goto_if_eq VAR_RESULT, B_OUTCOME_WON, SpearPillar_EventScript_DefeatedPalkia
# 83 "data/maps/SpearPillar/scripts.pory"
	goto_if_eq VAR_RESULT, B_OUTCOME_RAN, SpearPillar_EventScript_RanFromPalkia
# 84 "data/maps/SpearPillar/scripts.pory"
	goto_if_eq VAR_RESULT, B_OUTCOME_PLAYER_TELEPORTED, SpearPillar_EventScript_RanFromPalkia
# 85 "data/maps/SpearPillar/scripts.pory"
	setflag FLAG_DEFEATED_PALKIA
# 86 "data/maps/SpearPillar/scripts.pory"
	release
# 87 "data/maps/SpearPillar/scripts.pory"
	end
# 88 "data/maps/SpearPillar/scripts.pory"

# 89 "data/maps/SpearPillar/scripts.pory"
SpearPillar_EventScript_DefeatedPalkia::
# 90 "data/maps/SpearPillar/scripts.pory"
	setflag FLAG_DEFEATED_PALKIA
# 91 "data/maps/SpearPillar/scripts.pory"
	goto Common_EventScript_RemoveStaticPokemon
# 92 "data/maps/SpearPillar/scripts.pory"
	end
# 93 "data/maps/SpearPillar/scripts.pory"

# 94 "data/maps/SpearPillar/scripts.pory"
SpearPillar_EventScript_RanFromPalkia::
# 95 "data/maps/SpearPillar/scripts.pory"
	setvar VAR_0x8004, SPECIES_PALKIA
# 96 "data/maps/SpearPillar/scripts.pory"
	goto Common_EventScript_LegendaryFlewAway
# 97 "data/maps/SpearPillar/scripts.pory"
	end
