# 1 "data/maps/ArtisanCave_B1F/scripts.pory"

# 2 "data/maps/ArtisanCave_B1F/scripts.pory"
ArtisanCave_B1F_MapScripts::
# 3 "data/maps/ArtisanCave_B1F/scripts.pory"
	map_script MAP_SCRIPT_ON_RESUME, ArtisanCave_B1F_OnResume
# 4 "data/maps/ArtisanCave_B1F/scripts.pory"
	map_script MAP_SCRIPT_ON_TRANSITION, ArtisanCave_B1F_OnTransition
# 5 "data/maps/ArtisanCave_B1F/scripts.pory"
	.byte 0
# 6 "data/maps/ArtisanCave_B1F/scripts.pory"

# 7 "data/maps/ArtisanCave_B1F/scripts.pory"
ArtisanCave_B1F_OnResume:
# 8 "data/maps/ArtisanCave_B1F/scripts.pory"
	call_if_set FLAG_SYS_CTRL_OBJ_DELETE, ArtisanCave_B1F_EventScript_TryRemoveMiraidon
# 9 "data/maps/ArtisanCave_B1F/scripts.pory"
	call_if_set FLAG_SYS_CTRL_OBJ_DELETE, ArtisanCave_B1F_EventScript_TryRemoveKoraidon
# 10 "data/maps/ArtisanCave_B1F/scripts.pory"
	end
# 11 "data/maps/ArtisanCave_B1F/scripts.pory"

# 12 "data/maps/ArtisanCave_B1F/scripts.pory"
ArtisanCave_B1F_EventScript_TryRemoveMiraidon::
# 13 "data/maps/ArtisanCave_B1F/scripts.pory"
	specialvar VAR_RESULT, GetBattleOutcome
# 14 "data/maps/ArtisanCave_B1F/scripts.pory"
	goto_if_ne VAR_RESULT, B_OUTCOME_CAUGHT, Common_EventScript_NopReturn
# 15 "data/maps/ArtisanCave_B1F/scripts.pory"
	removeobject VAR_LAST_TALKED
# 16 "data/maps/ArtisanCave_B1F/scripts.pory"
	return
# 17 "data/maps/ArtisanCave_B1F/scripts.pory"

# 18 "data/maps/ArtisanCave_B1F/scripts.pory"
ArtisanCave_B1F_EventScript_TryRemoveKoraidon::
# 19 "data/maps/ArtisanCave_B1F/scripts.pory"
	specialvar VAR_RESULT, GetBattleOutcome
# 20 "data/maps/ArtisanCave_B1F/scripts.pory"
	goto_if_ne VAR_RESULT, B_OUTCOME_CAUGHT, Common_EventScript_NopReturn
# 21 "data/maps/ArtisanCave_B1F/scripts.pory"
	removeobject VAR_LAST_TALKED
# 22 "data/maps/ArtisanCave_B1F/scripts.pory"
	return
# 23 "data/maps/ArtisanCave_B1F/scripts.pory"

# 24 "data/maps/ArtisanCave_B1F/scripts.pory"

# 25 "data/maps/ArtisanCave_B1F/scripts.pory"
ArtisanCave_B1F_OnTransition:
# 26 "data/maps/ArtisanCave_B1F/scripts.pory"
	setflag FLAG_LANDMARK_ARTISAN_CAVE
# 27 "data/maps/ArtisanCave_B1F/scripts.pory"
	call_if_unset FLAG_DEFEATED_MIRAIDON, ArtisanCave_B1F_EventScript_ShowMiraidon
# 28 "data/maps/ArtisanCave_B1F/scripts.pory"
	call_if_unset FLAG_DEFEATED_KORAIDON, ArtisanCave_B1F_EventScript_ShowKoraidon
# 29 "data/maps/ArtisanCave_B1F/scripts.pory"
	end
# 30 "data/maps/ArtisanCave_B1F/scripts.pory"

# 31 "data/maps/ArtisanCave_B1F/scripts.pory"
ArtisanCave_B1F_EventScript_ShowMiraidon::
# 32 "data/maps/ArtisanCave_B1F/scripts.pory"
	clearflag FLAG_HIDE_MIRAIDON
# 33 "data/maps/ArtisanCave_B1F/scripts.pory"
	return
# 34 "data/maps/ArtisanCave_B1F/scripts.pory"

# 35 "data/maps/ArtisanCave_B1F/scripts.pory"
ArtisanCave_B1F_EventScript_ShowKoraidon::
# 36 "data/maps/ArtisanCave_B1F/scripts.pory"
	clearflag FLAG_HIDE_KORAIDON
# 37 "data/maps/ArtisanCave_B1F/scripts.pory"
	return
# 38 "data/maps/ArtisanCave_B1F/scripts.pory"

# 39 "data/maps/ArtisanCave_B1F/scripts.pory"
ArtisanCave_B1F_EventScript_Miraidon::
# 40 "data/maps/ArtisanCave_B1F/scripts.pory"
	lock
# 41 "data/maps/ArtisanCave_B1F/scripts.pory"
	faceplayer
# 42 "data/maps/ArtisanCave_B1F/scripts.pory"
	waitse
# 43 "data/maps/ArtisanCave_B1F/scripts.pory"
	playmoncry SPECIES_MIRAIDON, CRY_MODE_ENCOUNTER
# 44 "data/maps/ArtisanCave_B1F/scripts.pory"
	delay 40
# 45 "data/maps/ArtisanCave_B1F/scripts.pory"
	waitmoncry
# 46 "data/maps/ArtisanCave_B1F/scripts.pory"
	setwildbattle SPECIES_MIRAIDON, 70
# 47 "data/maps/ArtisanCave_B1F/scripts.pory"
	setflag FLAG_SYS_CTRL_OBJ_DELETE
# 48 "data/maps/ArtisanCave_B1F/scripts.pory"
	special BattleSetup_StartLegendaryBattle
# 49 "data/maps/ArtisanCave_B1F/scripts.pory"
	waitstate
# 50 "data/maps/ArtisanCave_B1F/scripts.pory"
	clearflag FLAG_SYS_CTRL_OBJ_DELETE
# 51 "data/maps/ArtisanCave_B1F/scripts.pory"
	specialvar VAR_RESULT, GetBattleOutcome
# 52 "data/maps/ArtisanCave_B1F/scripts.pory"
	goto_if_eq VAR_RESULT, B_OUTCOME_WON, ArtisanCave_B1F_EventScript_DefeatedMiraidon
# 53 "data/maps/ArtisanCave_B1F/scripts.pory"
	goto_if_eq VAR_RESULT, B_OUTCOME_RAN, ArtisanCave_B1F_EventScript_RanFromMiraidon
# 54 "data/maps/ArtisanCave_B1F/scripts.pory"
	goto_if_eq VAR_RESULT, B_OUTCOME_PLAYER_TELEPORTED, ArtisanCave_B1F_EventScript_RanFromMiraidon
# 55 "data/maps/ArtisanCave_B1F/scripts.pory"
	setflag FLAG_DEFEATED_MIRAIDON
# 56 "data/maps/ArtisanCave_B1F/scripts.pory"
	release
# 57 "data/maps/ArtisanCave_B1F/scripts.pory"
	end
# 58 "data/maps/ArtisanCave_B1F/scripts.pory"

# 59 "data/maps/ArtisanCave_B1F/scripts.pory"
ArtisanCave_B1F_EventScript_DefeatedMiraidon::
# 60 "data/maps/ArtisanCave_B1F/scripts.pory"
	setflag FLAG_DEFEATED_MIRAIDON
# 61 "data/maps/ArtisanCave_B1F/scripts.pory"
	goto Common_EventScript_RemoveStaticPokemon
# 62 "data/maps/ArtisanCave_B1F/scripts.pory"
	end
# 63 "data/maps/ArtisanCave_B1F/scripts.pory"

# 64 "data/maps/ArtisanCave_B1F/scripts.pory"
ArtisanCave_B1F_EventScript_RanFromMiraidon::
# 65 "data/maps/ArtisanCave_B1F/scripts.pory"
	setvar VAR_0x8004, SPECIES_MIRAIDON
# 66 "data/maps/ArtisanCave_B1F/scripts.pory"
	goto Common_EventScript_LegendaryFlewAway
# 67 "data/maps/ArtisanCave_B1F/scripts.pory"
	end
# 68 "data/maps/ArtisanCave_B1F/scripts.pory"

# 69 "data/maps/ArtisanCave_B1F/scripts.pory"

# 70 "data/maps/ArtisanCave_B1F/scripts.pory"
ArtisanCave_B1F_EventScript_Koraidon::
# 71 "data/maps/ArtisanCave_B1F/scripts.pory"
	lock
# 72 "data/maps/ArtisanCave_B1F/scripts.pory"
	faceplayer
# 73 "data/maps/ArtisanCave_B1F/scripts.pory"
	waitse
# 74 "data/maps/ArtisanCave_B1F/scripts.pory"
	playmoncry SPECIES_KORAIDON, CRY_MODE_ENCOUNTER
# 75 "data/maps/ArtisanCave_B1F/scripts.pory"
	delay 40
# 76 "data/maps/ArtisanCave_B1F/scripts.pory"
	waitmoncry
# 77 "data/maps/ArtisanCave_B1F/scripts.pory"
	setwildbattle SPECIES_KORAIDON, 70
# 78 "data/maps/ArtisanCave_B1F/scripts.pory"
	setflag FLAG_SYS_CTRL_OBJ_DELETE
# 79 "data/maps/ArtisanCave_B1F/scripts.pory"
	special BattleSetup_StartLegendaryBattle
# 80 "data/maps/ArtisanCave_B1F/scripts.pory"
	waitstate
# 81 "data/maps/ArtisanCave_B1F/scripts.pory"
	clearflag FLAG_SYS_CTRL_OBJ_DELETE
# 82 "data/maps/ArtisanCave_B1F/scripts.pory"
	specialvar VAR_RESULT, GetBattleOutcome
# 83 "data/maps/ArtisanCave_B1F/scripts.pory"
	goto_if_eq VAR_RESULT, B_OUTCOME_WON, ArtisanCave_B1F_EventScript_DefeatedKoraidon
# 84 "data/maps/ArtisanCave_B1F/scripts.pory"
	goto_if_eq VAR_RESULT, B_OUTCOME_RAN, ArtisanCave_B1F_EventScript_RanFromKoraidon
# 85 "data/maps/ArtisanCave_B1F/scripts.pory"
	goto_if_eq VAR_RESULT, B_OUTCOME_PLAYER_TELEPORTED, ArtisanCave_B1F_EventScript_RanFromKoraidon
# 86 "data/maps/ArtisanCave_B1F/scripts.pory"
	setflag FLAG_DEFEATED_KORAIDON
# 87 "data/maps/ArtisanCave_B1F/scripts.pory"
	release
# 88 "data/maps/ArtisanCave_B1F/scripts.pory"
	end
# 89 "data/maps/ArtisanCave_B1F/scripts.pory"

# 90 "data/maps/ArtisanCave_B1F/scripts.pory"
ArtisanCave_B1F_EventScript_DefeatedKoraidon::
# 91 "data/maps/ArtisanCave_B1F/scripts.pory"
	setflag FLAG_DEFEATED_KORAIDON
# 92 "data/maps/ArtisanCave_B1F/scripts.pory"
	goto Common_EventScript_RemoveStaticPokemon
# 93 "data/maps/ArtisanCave_B1F/scripts.pory"
	end
# 94 "data/maps/ArtisanCave_B1F/scripts.pory"

# 95 "data/maps/ArtisanCave_B1F/scripts.pory"
ArtisanCave_B1F_EventScript_RanFromKoraidon::
# 96 "data/maps/ArtisanCave_B1F/scripts.pory"
	setvar VAR_0x8004, SPECIES_KORAIDON
# 97 "data/maps/ArtisanCave_B1F/scripts.pory"
	goto Common_EventScript_LegendaryFlewAway
# 98 "data/maps/ArtisanCave_B1F/scripts.pory"
	end
