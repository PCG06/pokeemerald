# 1 "data/maps/HallofOrigin/scripts.pory"

# 2 "data/maps/HallofOrigin/scripts.pory"
HallofOrigin_MapScripts::
# 3 "data/maps/HallofOrigin/scripts.pory"
	map_script MAP_SCRIPT_ON_RESUME, HallofOrigin_OnResume
# 4 "data/maps/HallofOrigin/scripts.pory"
	map_script MAP_SCRIPT_ON_TRANSITION, HallofOrigin_OnTransition
# 5 "data/maps/HallofOrigin/scripts.pory"
	.byte 0
# 6 "data/maps/HallofOrigin/scripts.pory"

# 7 "data/maps/HallofOrigin/scripts.pory"
HallofOrigin_OnResume:
# 8 "data/maps/HallofOrigin/scripts.pory"
	call_if_set FLAG_SYS_CTRL_OBJ_DELETE, HallofOrigin_EventScript_TryRemoveArceus
# 9 "data/maps/HallofOrigin/scripts.pory"
	call_if_set FLAG_SYS_CTRL_OBJ_DELETE, HallofOrigin_EventScript_TryRemoveXerneas
# 10 "data/maps/HallofOrigin/scripts.pory"
	end
# 11 "data/maps/HallofOrigin/scripts.pory"

# 12 "data/maps/HallofOrigin/scripts.pory"
HallofOrigin_EventScript_TryRemoveArceus::
# 13 "data/maps/HallofOrigin/scripts.pory"
	specialvar VAR_RESULT, GetBattleOutcome
# 14 "data/maps/HallofOrigin/scripts.pory"
	goto_if_ne VAR_RESULT, B_OUTCOME_CAUGHT, Common_EventScript_NopReturn
# 15 "data/maps/HallofOrigin/scripts.pory"
	removeobject VAR_LAST_TALKED
# 16 "data/maps/HallofOrigin/scripts.pory"
	return
# 17 "data/maps/HallofOrigin/scripts.pory"

# 18 "data/maps/HallofOrigin/scripts.pory"
HallofOrigin_EventScript_TryRemoveXerneas::
# 19 "data/maps/HallofOrigin/scripts.pory"
	specialvar VAR_RESULT, GetBattleOutcome
# 20 "data/maps/HallofOrigin/scripts.pory"
	goto_if_ne VAR_RESULT, B_OUTCOME_CAUGHT, Common_EventScript_NopReturn
# 21 "data/maps/HallofOrigin/scripts.pory"
	removeobject VAR_LAST_TALKED
# 22 "data/maps/HallofOrigin/scripts.pory"
	return
# 23 "data/maps/HallofOrigin/scripts.pory"

# 24 "data/maps/HallofOrigin/scripts.pory"

# 25 "data/maps/HallofOrigin/scripts.pory"
HallofOrigin_OnTransition:
# 26 "data/maps/HallofOrigin/scripts.pory"
	call_if_unset FLAG_DEFEATED_ARCEUS, HallofOrigin_EventScript_ShowArceus
# 27 "data/maps/HallofOrigin/scripts.pory"
	call_if_unset FLAG_DEFEATED_XERNEAS, HallofOrigin_EventScript_ShowXerneas
# 28 "data/maps/HallofOrigin/scripts.pory"
	end
# 29 "data/maps/HallofOrigin/scripts.pory"

# 30 "data/maps/HallofOrigin/scripts.pory"
HallofOrigin_EventScript_ShowArceus::
# 31 "data/maps/HallofOrigin/scripts.pory"
	clearflag FLAG_HIDE_ARCEUS
# 32 "data/maps/HallofOrigin/scripts.pory"
	return
# 33 "data/maps/HallofOrigin/scripts.pory"

# 34 "data/maps/HallofOrigin/scripts.pory"
HallofOrigin_EventScript_ShowXerneas::
# 35 "data/maps/HallofOrigin/scripts.pory"
	clearflag FLAG_HIDE_XERNEAS
# 36 "data/maps/HallofOrigin/scripts.pory"
	return
# 37 "data/maps/HallofOrigin/scripts.pory"

# 38 "data/maps/HallofOrigin/scripts.pory"
HallofOrigin_EventScript_Arceus::
# 39 "data/maps/HallofOrigin/scripts.pory"
	lock
# 40 "data/maps/HallofOrigin/scripts.pory"
	faceplayer
# 41 "data/maps/HallofOrigin/scripts.pory"
	waitse
# 42 "data/maps/HallofOrigin/scripts.pory"
	playmoncry SPECIES_ARCEUS, CRY_MODE_ENCOUNTER
# 43 "data/maps/HallofOrigin/scripts.pory"
	delay 40
# 44 "data/maps/HallofOrigin/scripts.pory"
	waitmoncry
# 45 "data/maps/HallofOrigin/scripts.pory"
	setwildbattle SPECIES_ARCEUS, 70
# 46 "data/maps/HallofOrigin/scripts.pory"
	setflag FLAG_SYS_CTRL_OBJ_DELETE
# 47 "data/maps/HallofOrigin/scripts.pory"
	special BattleSetup_StartLegendaryBattle
# 48 "data/maps/HallofOrigin/scripts.pory"
	waitstate
# 49 "data/maps/HallofOrigin/scripts.pory"
	clearflag FLAG_SYS_CTRL_OBJ_DELETE
# 50 "data/maps/HallofOrigin/scripts.pory"
	specialvar VAR_RESULT, GetBattleOutcome
# 51 "data/maps/HallofOrigin/scripts.pory"
	goto_if_eq VAR_RESULT, B_OUTCOME_WON, HallofOrigin_EventScript_DefeatedArceus
# 52 "data/maps/HallofOrigin/scripts.pory"
	goto_if_eq VAR_RESULT, B_OUTCOME_RAN, HallofOrigin_EventScript_RanFromArceus
# 53 "data/maps/HallofOrigin/scripts.pory"
	goto_if_eq VAR_RESULT, B_OUTCOME_PLAYER_TELEPORTED, HallofOrigin_EventScript_RanFromArceus
# 54 "data/maps/HallofOrigin/scripts.pory"
	setflag FLAG_DEFEATED_ARCEUS
# 55 "data/maps/HallofOrigin/scripts.pory"
	release
# 56 "data/maps/HallofOrigin/scripts.pory"
	end
# 57 "data/maps/HallofOrigin/scripts.pory"

# 58 "data/maps/HallofOrigin/scripts.pory"
HallofOrigin_EventScript_DefeatedArceus::
# 59 "data/maps/HallofOrigin/scripts.pory"
	setflag FLAG_DEFEATED_ARCEUS
# 60 "data/maps/HallofOrigin/scripts.pory"
	goto Common_EventScript_RemoveStaticPokemon
# 61 "data/maps/HallofOrigin/scripts.pory"
	end
# 62 "data/maps/HallofOrigin/scripts.pory"

# 63 "data/maps/HallofOrigin/scripts.pory"
HallofOrigin_EventScript_RanFromArceus::
# 64 "data/maps/HallofOrigin/scripts.pory"
	setvar VAR_0x8004, SPECIES_ARCEUS
# 65 "data/maps/HallofOrigin/scripts.pory"
	goto Common_EventScript_LegendaryFlewAway
# 66 "data/maps/HallofOrigin/scripts.pory"
	end
# 67 "data/maps/HallofOrigin/scripts.pory"

# 68 "data/maps/HallofOrigin/scripts.pory"

# 69 "data/maps/HallofOrigin/scripts.pory"
HallofOrigin_EventScript_Xerneas::
# 70 "data/maps/HallofOrigin/scripts.pory"
	lock
# 71 "data/maps/HallofOrigin/scripts.pory"
	faceplayer
# 72 "data/maps/HallofOrigin/scripts.pory"
	waitse
# 73 "data/maps/HallofOrigin/scripts.pory"
	playmoncry SPECIES_XERNEAS_ACTIVE, CRY_MODE_ENCOUNTER
# 74 "data/maps/HallofOrigin/scripts.pory"
	delay 40
# 75 "data/maps/HallofOrigin/scripts.pory"
	waitmoncry
# 76 "data/maps/HallofOrigin/scripts.pory"
	setwildbattle SPECIES_XERNEAS_ACTIVE, 70
# 77 "data/maps/HallofOrigin/scripts.pory"
	setflag FLAG_SYS_CTRL_OBJ_DELETE
# 78 "data/maps/HallofOrigin/scripts.pory"
	special BattleSetup_StartLegendaryBattle
# 79 "data/maps/HallofOrigin/scripts.pory"
	waitstate
# 80 "data/maps/HallofOrigin/scripts.pory"
	clearflag FLAG_SYS_CTRL_OBJ_DELETE
# 81 "data/maps/HallofOrigin/scripts.pory"
	specialvar VAR_RESULT, GetBattleOutcome
# 82 "data/maps/HallofOrigin/scripts.pory"
	goto_if_eq VAR_RESULT, B_OUTCOME_WON, HallofOrigin_EventScript_DefeatedXerneas
# 83 "data/maps/HallofOrigin/scripts.pory"
	goto_if_eq VAR_RESULT, B_OUTCOME_RAN, HallofOrigin_EventScript_RanFromXerneas
# 84 "data/maps/HallofOrigin/scripts.pory"
	goto_if_eq VAR_RESULT, B_OUTCOME_PLAYER_TELEPORTED, HallofOrigin_EventScript_RanFromXerneas
# 85 "data/maps/HallofOrigin/scripts.pory"
	setflag FLAG_DEFEATED_XERNEAS
# 86 "data/maps/HallofOrigin/scripts.pory"
	release
# 87 "data/maps/HallofOrigin/scripts.pory"
	end
# 88 "data/maps/HallofOrigin/scripts.pory"

# 89 "data/maps/HallofOrigin/scripts.pory"
HallofOrigin_EventScript_DefeatedXerneas::
# 90 "data/maps/HallofOrigin/scripts.pory"
	setflag FLAG_DEFEATED_XERNEAS
# 91 "data/maps/HallofOrigin/scripts.pory"
	goto Common_EventScript_RemoveStaticPokemon
# 92 "data/maps/HallofOrigin/scripts.pory"
	end
# 93 "data/maps/HallofOrigin/scripts.pory"

# 94 "data/maps/HallofOrigin/scripts.pory"
HallofOrigin_EventScript_RanFromXerneas::
# 95 "data/maps/HallofOrigin/scripts.pory"
	setvar VAR_0x8004, SPECIES_XERNEAS
# 96 "data/maps/HallofOrigin/scripts.pory"
	goto Common_EventScript_LegendaryFlewAway
# 97 "data/maps/HallofOrigin/scripts.pory"
	end
# 98 "data/maps/HallofOrigin/scripts.pory"

# 99 "data/maps/HallofOrigin/scripts.pory"
HallofOrigin_EventScript_BigAssRockSign::
# 100 "data/maps/HallofOrigin/scripts.pory"
	msgbox HallofOrigin_Text_BigAssRockSign, MSGBOX_SIGN
# 101 "data/maps/HallofOrigin/scripts.pory"
	end
# 102 "data/maps/HallofOrigin/scripts.pory"

# 103 "data/maps/HallofOrigin/scripts.pory"
HallofOrigin_Text_BigAssRockSign:
# 104 "data/maps/HallofOrigin/scripts.pory"
	.string "The flow of time never stops...\n"
# 105 "data/maps/HallofOrigin/scripts.pory"
	.string "“The past, future, and present...\l"
# 106 "data/maps/HallofOrigin/scripts.pory"
	.string "“Space is ever-expanding dimensions...\l"
# 107 "data/maps/HallofOrigin/scripts.pory"
	.string "Our spirits, too, are as space...”$"
