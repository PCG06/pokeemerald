# 1 "data/maps/DewfordTown/scripts.pory"

# 2 "data/maps/DewfordTown/scripts.pory"
@ Sailing to and from Dewford uses local IDs from different maps
# 3 "data/maps/DewfordTown/scripts.pory"
@ e.g. Route 104's sail to Dewford script references local IDs from Dewford's map
# 4 "data/maps/DewfordTown/scripts.pory"
@ All of these local IDs are labeled here
# 5 "data/maps/DewfordTown/scripts.pory"
.equ LOCALID_BRINEY_DEWFORD, 2
# 6 "data/maps/DewfordTown/scripts.pory"
.equ LOCALID_BOAT_DEWFORD, 3
# 7 "data/maps/DewfordTown/scripts.pory"

# 8 "data/maps/DewfordTown/scripts.pory"
.equ LOCALID_BOAT_R109, 1
# 9 "data/maps/DewfordTown/scripts.pory"
.equ LOCALID_BRINEY_R109, 2
# 10 "data/maps/DewfordTown/scripts.pory"

# 11 "data/maps/DewfordTown/scripts.pory"
.equ LOCALID_BOAT_R104, 7
# 12 "data/maps/DewfordTown/scripts.pory"
.equ LOCALID_BRINEY_R104, 8
# 13 "data/maps/DewfordTown/scripts.pory"

# 14 "data/maps/DewfordTown/scripts.pory"
DewfordTown_MapScripts::
# 15 "data/maps/DewfordTown/scripts.pory"
	map_script MAP_SCRIPT_ON_TRANSITION, DewfordTown_OnTransition
# 16 "data/maps/DewfordTown/scripts.pory"
	.byte 0
# 17 "data/maps/DewfordTown/scripts.pory"

# 18 "data/maps/DewfordTown/scripts.pory"
DewfordTown_OnTransition:
# 19 "data/maps/DewfordTown/scripts.pory"
	setflag FLAG_VISITED_DEWFORD_TOWN
# 20 "data/maps/DewfordTown/scripts.pory"
	end
# 21 "data/maps/DewfordTown/scripts.pory"

# 22 "data/maps/DewfordTown/scripts.pory"
DewfordTown_EventScript_Briney::
# 23 "data/maps/DewfordTown/scripts.pory"
	lock
# 24 "data/maps/DewfordTown/scripts.pory"
	faceplayer
# 25 "data/maps/DewfordTown/scripts.pory"
	goto_if_unset FLAG_DELIVERED_STEVEN_LETTER, DewfordTown_EventScript_ReturnToPetalburgPrompt
# 26 "data/maps/DewfordTown/scripts.pory"
	message DewfordTown_Text_WhereAreWeBound
# 27 "data/maps/DewfordTown/scripts.pory"
	waitmessage
# 28 "data/maps/DewfordTown/scripts.pory"
	multichoicedefault 21, 6, MULTI_BRINEY_ON_DEWFORD, 2, FALSE
# 29 "data/maps/DewfordTown/scripts.pory"
	switch VAR_RESULT
# 30 "data/maps/DewfordTown/scripts.pory"
	case 0, DewfordTown_EventScript_ChoosePetalburg
# 31 "data/maps/DewfordTown/scripts.pory"
	case 1, DewfordTown_EventScript_ChooseSlateport
# 32 "data/maps/DewfordTown/scripts.pory"
	case 2, DewfordTown_EventScript_CancelSailSelect
# 33 "data/maps/DewfordTown/scripts.pory"
	case MULTI_B_PRESSED, DewfordTown_EventScript_CancelSailSelect
# 34 "data/maps/DewfordTown/scripts.pory"
	end
# 35 "data/maps/DewfordTown/scripts.pory"

# 36 "data/maps/DewfordTown/scripts.pory"
DewfordTown_EventScript_ChoosePetalburg::
# 37 "data/maps/DewfordTown/scripts.pory"
	msgbox DewfordTown_Text_PetalburgWereSettingSail, MSGBOX_DEFAULT
# 38 "data/maps/DewfordTown/scripts.pory"
	closemessage
# 39 "data/maps/DewfordTown/scripts.pory"
	goto DewfordTown_EventScript_SailToPetalburg
# 40 "data/maps/DewfordTown/scripts.pory"
	release
# 41 "data/maps/DewfordTown/scripts.pory"
	end
# 42 "data/maps/DewfordTown/scripts.pory"

# 43 "data/maps/DewfordTown/scripts.pory"
DewfordTown_EventScript_ChooseSlateport::
# 44 "data/maps/DewfordTown/scripts.pory"
	msgbox DewfordTown_Text_SlateportWereSettingSail, MSGBOX_DEFAULT
# 45 "data/maps/DewfordTown/scripts.pory"
	closemessage
# 46 "data/maps/DewfordTown/scripts.pory"
	goto DewfordTown_EventScript_SailToSlateport
# 47 "data/maps/DewfordTown/scripts.pory"
	release
# 48 "data/maps/DewfordTown/scripts.pory"
	end
# 49 "data/maps/DewfordTown/scripts.pory"

# 50 "data/maps/DewfordTown/scripts.pory"
DewfordTown_EventScript_CancelSailSelect::
# 51 "data/maps/DewfordTown/scripts.pory"
	msgbox DewfordTown_Text_JustTellMeWhenYouNeedToSetSail, MSGBOX_DEFAULT
# 52 "data/maps/DewfordTown/scripts.pory"
	closemessage
# 53 "data/maps/DewfordTown/scripts.pory"
	release
# 54 "data/maps/DewfordTown/scripts.pory"
	end
# 55 "data/maps/DewfordTown/scripts.pory"

# 56 "data/maps/DewfordTown/scripts.pory"
DewfordTown_EventScript_ReturnToPetalburgPrompt::
# 57 "data/maps/DewfordTown/scripts.pory"
	msgbox DewfordTown_Text_SetSailBackToPetalburg, MSGBOX_YESNO
# 58 "data/maps/DewfordTown/scripts.pory"
	goto_if_eq VAR_RESULT, YES, DewfordTown_EventScript_SailBackToPetalburg
# 59 "data/maps/DewfordTown/scripts.pory"
	msgbox DewfordTown_Text_GoDeliverIllBeWaiting, MSGBOX_DEFAULT
# 60 "data/maps/DewfordTown/scripts.pory"
	release
# 61 "data/maps/DewfordTown/scripts.pory"
	end
# 62 "data/maps/DewfordTown/scripts.pory"

# 63 "data/maps/DewfordTown/scripts.pory"
DewfordTown_EventScript_SailBackToPetalburg::
# 64 "data/maps/DewfordTown/scripts.pory"
	msgbox DewfordTown_Text_PetalburgWereSettingSail2, MSGBOX_DEFAULT
# 65 "data/maps/DewfordTown/scripts.pory"
	closemessage
# 66 "data/maps/DewfordTown/scripts.pory"
	goto DewfordTown_EventScript_SailToPetalburg
# 67 "data/maps/DewfordTown/scripts.pory"
	end
# 68 "data/maps/DewfordTown/scripts.pory"

# 69 "data/maps/DewfordTown/scripts.pory"
DewfordTown_EventScript_Woman::
# 70 "data/maps/DewfordTown/scripts.pory"
	msgbox DewfordTown_Text_TinyIslandCommunity, MSGBOX_NPC
# 71 "data/maps/DewfordTown/scripts.pory"
	end
# 72 "data/maps/DewfordTown/scripts.pory"

# 73 "data/maps/DewfordTown/scripts.pory"
DewfordTown_EventScript_TownSign::
# 74 "data/maps/DewfordTown/scripts.pory"
	msgbox DewfordTown_Text_TownSign, MSGBOX_SIGN
# 75 "data/maps/DewfordTown/scripts.pory"
	end
# 76 "data/maps/DewfordTown/scripts.pory"

# 77 "data/maps/DewfordTown/scripts.pory"
DewfordTown_EventScript_GymSign::
# 78 "data/maps/DewfordTown/scripts.pory"
	msgbox DewfordTown_Text_GymSign, MSGBOX_SIGN
# 79 "data/maps/DewfordTown/scripts.pory"
	end
# 80 "data/maps/DewfordTown/scripts.pory"

# 81 "data/maps/DewfordTown/scripts.pory"
DewfordTown_EventScript_HallSign::
# 82 "data/maps/DewfordTown/scripts.pory"
	msgbox DewfordTown_Text_HallSign, MSGBOX_SIGN
# 83 "data/maps/DewfordTown/scripts.pory"
	end
# 84 "data/maps/DewfordTown/scripts.pory"

# 85 "data/maps/DewfordTown/scripts.pory"
DewfordTown_EventScript_SailToPetalburg::
# 86 "data/maps/DewfordTown/scripts.pory"
	call EventScript_BackupMrBrineyLocation
# 87 "data/maps/DewfordTown/scripts.pory"
	setobjectsubpriority LOCALID_BRINEY_DEWFORD, MAP_DEWFORD_TOWN, 0
# 88 "data/maps/DewfordTown/scripts.pory"
	setobjectsubpriority OBJ_EVENT_ID_PLAYER, MAP_DEWFORD_TOWN, 0
# 89 "data/maps/DewfordTown/scripts.pory"
	applymovement LOCALID_BRINEY_DEWFORD, DewfordTown_Movement_BrineyBoardBoat
# 90 "data/maps/DewfordTown/scripts.pory"
	waitmovement 0
# 91 "data/maps/DewfordTown/scripts.pory"
	removeobject LOCALID_BRINEY_DEWFORD
# 92 "data/maps/DewfordTown/scripts.pory"
	applymovement OBJ_EVENT_ID_PLAYER, DewfordTown_Movement_PlayerBoardBoat
# 93 "data/maps/DewfordTown/scripts.pory"
	waitmovement 0
# 94 "data/maps/DewfordTown/scripts.pory"
	hideobjectat OBJ_EVENT_ID_PLAYER, MAP_DEWFORD_TOWN
# 95 "data/maps/DewfordTown/scripts.pory"
	call Common_EventScript_PlayBrineysBoatMusic
# 96 "data/maps/DewfordTown/scripts.pory"
	applymovement LOCALID_BOAT_DEWFORD, DewfordTown_Movement_SailToPetalburg
# 97 "data/maps/DewfordTown/scripts.pory"
	applymovement OBJ_EVENT_ID_PLAYER, DewfordTown_Movement_SailToPetalburg
# 98 "data/maps/DewfordTown/scripts.pory"
	waitmovement 0
# 99 "data/maps/DewfordTown/scripts.pory"
	showobjectat OBJ_EVENT_ID_PLAYER, MAP_ROUTE104
# 100 "data/maps/DewfordTown/scripts.pory"
	call Common_EventScript_StopBrineysBoatMusic
# 101 "data/maps/DewfordTown/scripts.pory"
	applymovement OBJ_EVENT_ID_PLAYER, DewfordTown_Movement_ExitBoatPetalburg
# 102 "data/maps/DewfordTown/scripts.pory"
	waitmovement 0
# 103 "data/maps/DewfordTown/scripts.pory"
	showobjectat OBJ_EVENT_ID_PLAYER, MAP_ROUTE104
# 104 "data/maps/DewfordTown/scripts.pory"
	clearflag FLAG_HIDE_BRINEYS_HOUSE_MR_BRINEY
# 105 "data/maps/DewfordTown/scripts.pory"
	clearflag FLAG_HIDE_BRINEYS_HOUSE_PEEKO
# 106 "data/maps/DewfordTown/scripts.pory"
	clearflag FLAG_HIDE_ROUTE_104_MR_BRINEY_BOAT
# 107 "data/maps/DewfordTown/scripts.pory"
	setflag FLAG_HIDE_MR_BRINEY_BOAT_DEWFORD_TOWN
# 108 "data/maps/DewfordTown/scripts.pory"
	hideobjectat LOCALID_BOAT_DEWFORD, MAP_DEWFORD_TOWN
# 109 "data/maps/DewfordTown/scripts.pory"
	setvar VAR_BOARD_BRINEY_BOAT_STATE, 2
# 110 "data/maps/DewfordTown/scripts.pory"
	resetobjectsubpriority OBJ_EVENT_ID_PLAYER, MAP_DEWFORD_TOWN
# 111 "data/maps/DewfordTown/scripts.pory"
	warp MAP_ROUTE104_MR_BRINEYS_HOUSE, 5, 4
# 112 "data/maps/DewfordTown/scripts.pory"
	copyvar VAR_BRINEY_LOCATION, VAR_0x8008
# 113 "data/maps/DewfordTown/scripts.pory"
	waitstate
# 114 "data/maps/DewfordTown/scripts.pory"
	release
# 115 "data/maps/DewfordTown/scripts.pory"
	end
# 116 "data/maps/DewfordTown/scripts.pory"

# 117 "data/maps/DewfordTown/scripts.pory"
DewfordTown_EventScript_SailToSlateport::
# 118 "data/maps/DewfordTown/scripts.pory"
	call EventScript_BackupMrBrineyLocation
# 119 "data/maps/DewfordTown/scripts.pory"
	setobjectsubpriority LOCALID_BRINEY_DEWFORD, MAP_DEWFORD_TOWN, 0
# 120 "data/maps/DewfordTown/scripts.pory"
	setobjectsubpriority OBJ_EVENT_ID_PLAYER, MAP_DEWFORD_TOWN, 1
# 121 "data/maps/DewfordTown/scripts.pory"
	applymovement LOCALID_BRINEY_DEWFORD, DewfordTown_Movement_BrineyBoardBoat
# 122 "data/maps/DewfordTown/scripts.pory"
	waitmovement 0
# 123 "data/maps/DewfordTown/scripts.pory"
	removeobject LOCALID_BRINEY_DEWFORD
# 124 "data/maps/DewfordTown/scripts.pory"
	applymovement OBJ_EVENT_ID_PLAYER, DewfordTown_Movement_PlayerBoardBoat
# 125 "data/maps/DewfordTown/scripts.pory"
	waitmovement 0
# 126 "data/maps/DewfordTown/scripts.pory"
	hideobjectat OBJ_EVENT_ID_PLAYER, MAP_DEWFORD_TOWN
# 127 "data/maps/DewfordTown/scripts.pory"
	call Common_EventScript_PlayBrineysBoatMusic
# 128 "data/maps/DewfordTown/scripts.pory"
	applymovement LOCALID_BOAT_DEWFORD, DewfordTown_Movement_SailToSlateport
# 129 "data/maps/DewfordTown/scripts.pory"
	applymovement OBJ_EVENT_ID_PLAYER, DewfordTown_Movement_SailToSlateport
# 130 "data/maps/DewfordTown/scripts.pory"
	waitmovement 0
# 131 "data/maps/DewfordTown/scripts.pory"
	call Common_EventScript_StopBrineysBoatMusic
# 132 "data/maps/DewfordTown/scripts.pory"
	showobjectat OBJ_EVENT_ID_PLAYER, MAP_ROUTE109
# 133 "data/maps/DewfordTown/scripts.pory"
	applymovement OBJ_EVENT_ID_PLAYER, DewfordTown_Movement_ExitBoatSlateport
# 134 "data/maps/DewfordTown/scripts.pory"
	waitmovement 0
# 135 "data/maps/DewfordTown/scripts.pory"
	setobjectxyperm LOCALID_BRINEY_R109, 21, 26
# 136 "data/maps/DewfordTown/scripts.pory"
	addobject LOCALID_BRINEY_R109
# 137 "data/maps/DewfordTown/scripts.pory"
	setobjectsubpriority LOCALID_BRINEY_R109, MAP_ROUTE109, 0
# 138 "data/maps/DewfordTown/scripts.pory"
	applymovement LOCALID_BRINEY_R109, DewfordTown_Movement_BrineyExitBoat
# 139 "data/maps/DewfordTown/scripts.pory"
	waitmovement 0
# 140 "data/maps/DewfordTown/scripts.pory"
	clearflag FLAG_HIDE_ROUTE_109_MR_BRINEY
# 141 "data/maps/DewfordTown/scripts.pory"
	addobject LOCALID_BOAT_R109
# 142 "data/maps/DewfordTown/scripts.pory"
	clearflag FLAG_HIDE_ROUTE_109_MR_BRINEY_BOAT
# 143 "data/maps/DewfordTown/scripts.pory"
	setflag FLAG_HIDE_MR_BRINEY_BOAT_DEWFORD_TOWN
# 144 "data/maps/DewfordTown/scripts.pory"
	hideobjectat LOCALID_BOAT_DEWFORD, MAP_DEWFORD_TOWN
# 145 "data/maps/DewfordTown/scripts.pory"
	call_if_unset FLAG_DELIVERED_DEVON_GOODS, DewfordTown_EventScript_LandedSlateportDeliverGoods
# 146 "data/maps/DewfordTown/scripts.pory"
	call_if_set FLAG_DELIVERED_DEVON_GOODS, DewfordTown_EventScript_LandedSlateport
# 147 "data/maps/DewfordTown/scripts.pory"
	closemessage
# 148 "data/maps/DewfordTown/scripts.pory"
	copyvar VAR_BRINEY_LOCATION, VAR_0x8008
# 149 "data/maps/DewfordTown/scripts.pory"
	resetobjectsubpriority OBJ_EVENT_ID_PLAYER, MAP_DEWFORD_TOWN
# 150 "data/maps/DewfordTown/scripts.pory"
	resetobjectsubpriority LOCALID_BRINEY_R109, MAP_ROUTE109
# 151 "data/maps/DewfordTown/scripts.pory"
	copyobjectxytoperm LOCALID_BRINEY_R109
# 152 "data/maps/DewfordTown/scripts.pory"
	release
# 153 "data/maps/DewfordTown/scripts.pory"
	end
# 154 "data/maps/DewfordTown/scripts.pory"

# 155 "data/maps/DewfordTown/scripts.pory"
DewfordTown_EventScript_LandedSlateportDeliverGoods::
# 156 "data/maps/DewfordTown/scripts.pory"
	msgbox DewfordTown_Text_BrineyLandedInSlateportDeliverGoods, MSGBOX_DEFAULT
# 157 "data/maps/DewfordTown/scripts.pory"
	return
# 158 "data/maps/DewfordTown/scripts.pory"

# 159 "data/maps/DewfordTown/scripts.pory"
DewfordTown_EventScript_LandedSlateport::
# 160 "data/maps/DewfordTown/scripts.pory"
	msgbox DewfordTown_Text_BrineyLandedInSlateport, MSGBOX_DEFAULT
# 161 "data/maps/DewfordTown/scripts.pory"
	return
# 162 "data/maps/DewfordTown/scripts.pory"

# 163 "data/maps/DewfordTown/scripts.pory"

# 164 "data/maps/DewfordTown/scripts.pory"
DewfordTown_MiniMarket_EventScript_Saleswoman2::
# 165 "data/maps/DewfordTown/scripts.pory"
	lock
# 166 "data/maps/DewfordTown/scripts.pory"
	faceplayer
# 167 "data/maps/DewfordTown/scripts.pory"
	message gText_HowMayIServeYou
# 168 "data/maps/DewfordTown/scripts.pory"
	waitmessage
# 169 "data/maps/DewfordTown/scripts.pory"
	goto_if_ge VAR_GAME_SETTING_DIFFICULTY_MODE, GAME_SETTING_DIFFICULTY_HARD_MODE, DewfordTown_MiniMarket_EventScript_Saleswoman2_HardMode
# 170 "data/maps/DewfordTown/scripts.pory"
	tmshop DewfordTown_MiniMart2
# 171 "data/maps/DewfordTown/scripts.pory"
	msgbox gText_PleaseComeAgain, MSGBOX_DEFAULT
# 172 "data/maps/DewfordTown/scripts.pory"
	release
# 173 "data/maps/DewfordTown/scripts.pory"
	end
# 174 "data/maps/DewfordTown/scripts.pory"

# 175 "data/maps/DewfordTown/scripts.pory"
DewfordTown_MiniMarket_EventScript_Saleswoman2_HardMode::
# 176 "data/maps/DewfordTown/scripts.pory"
	tmshop DewfordTown_MiniMart2HardMode
# 177 "data/maps/DewfordTown/scripts.pory"
	msgbox gText_PleaseComeAgain, MSGBOX_DEFAULT
# 178 "data/maps/DewfordTown/scripts.pory"
	release
# 179 "data/maps/DewfordTown/scripts.pory"
	end
# 180 "data/maps/DewfordTown/scripts.pory"

# 181 "data/maps/DewfordTown/scripts.pory"
	.align 2
# 182 "data/maps/DewfordTown/scripts.pory"
DewfordTown_MiniMart2:
# 183 "data/maps/DewfordTown/scripts.pory"
	.2byte ITEM_TM_ELECTROWEB
# 184 "data/maps/DewfordTown/scripts.pory"
	.2byte ITEM_TM_ROCK_TOMB
# 185 "data/maps/DewfordTown/scripts.pory"
	.2byte ITEM_TM_AERIAL_ACE
# 186 "data/maps/DewfordTown/scripts.pory"
	.2byte ITEM_TM_LOW_KICK
# 187 "data/maps/DewfordTown/scripts.pory"
	.2byte ITEM_TM_PLUCK
# 188 "data/maps/DewfordTown/scripts.pory"
	.2byte ITEM_TM_SNARL
# 189 "data/maps/DewfordTown/scripts.pory"
	.2byte ITEM_TM_SMACK_DOWN
# 190 "data/maps/DewfordTown/scripts.pory"
	.2byte ITEM_NONE
# 191 "data/maps/DewfordTown/scripts.pory"
	release
# 192 "data/maps/DewfordTown/scripts.pory"
	end
# 193 "data/maps/DewfordTown/scripts.pory"

# 194 "data/maps/DewfordTown/scripts.pory"
	.align 2
# 195 "data/maps/DewfordTown/scripts.pory"
DewfordTown_MiniMart2HardMode:
# 196 "data/maps/DewfordTown/scripts.pory"
	.2byte ITEM_TM_ELECTROWEB
# 197 "data/maps/DewfordTown/scripts.pory"
	.2byte ITEM_TM_ICY_WIND
# 198 "data/maps/DewfordTown/scripts.pory"
	.2byte ITEM_TM_ROCK_TOMB
# 199 "data/maps/DewfordTown/scripts.pory"
	.2byte ITEM_TM_AERIAL_ACE
# 200 "data/maps/DewfordTown/scripts.pory"
	.2byte ITEM_TM_LOW_KICK
# 201 "data/maps/DewfordTown/scripts.pory"
	.2byte ITEM_TM_PLUCK
# 202 "data/maps/DewfordTown/scripts.pory"
	.2byte ITEM_TM_SNARL
# 203 "data/maps/DewfordTown/scripts.pory"
	.2byte ITEM_TM_SMACK_DOWN
# 204 "data/maps/DewfordTown/scripts.pory"
	.2byte ITEM_NONE
# 205 "data/maps/DewfordTown/scripts.pory"
	release
# 206 "data/maps/DewfordTown/scripts.pory"
	end
# 207 "data/maps/DewfordTown/scripts.pory"

# 208 "data/maps/DewfordTown/scripts.pory"
DewfordTown_Movement_SailToPetalburg:
# 209 "data/maps/DewfordTown/scripts.pory"
	walk_up
# 210 "data/maps/DewfordTown/scripts.pory"
	walk_up
# 211 "data/maps/DewfordTown/scripts.pory"
	walk_fast_up
# 212 "data/maps/DewfordTown/scripts.pory"
	walk_fast_up
# 213 "data/maps/DewfordTown/scripts.pory"
	walk_fast_up
# 214 "data/maps/DewfordTown/scripts.pory"
	walk_fast_up
# 215 "data/maps/DewfordTown/scripts.pory"
	walk_fast_up
# 216 "data/maps/DewfordTown/scripts.pory"
	walk_fast_up
# 217 "data/maps/DewfordTown/scripts.pory"
	walk_fast_up
# 218 "data/maps/DewfordTown/scripts.pory"
	walk_fast_up
# 219 "data/maps/DewfordTown/scripts.pory"
	walk_fast_up
# 220 "data/maps/DewfordTown/scripts.pory"
	walk_fast_up
# 221 "data/maps/DewfordTown/scripts.pory"
	walk_fast_up
# 222 "data/maps/DewfordTown/scripts.pory"
	walk_fast_up
# 223 "data/maps/DewfordTown/scripts.pory"
	walk_fast_up
# 224 "data/maps/DewfordTown/scripts.pory"
	walk_fast_up
# 225 "data/maps/DewfordTown/scripts.pory"
	walk_fast_up
# 226 "data/maps/DewfordTown/scripts.pory"
	walk_up
# 227 "data/maps/DewfordTown/scripts.pory"
	walk_up
# 228 "data/maps/DewfordTown/scripts.pory"
	walk_left
# 229 "data/maps/DewfordTown/scripts.pory"
	walk_left
# 230 "data/maps/DewfordTown/scripts.pory"
	walk_fast_left
# 231 "data/maps/DewfordTown/scripts.pory"
	walk_fast_left
# 232 "data/maps/DewfordTown/scripts.pory"
	walk_fast_left
# 233 "data/maps/DewfordTown/scripts.pory"
	walk_fast_left
# 234 "data/maps/DewfordTown/scripts.pory"
	walk_faster_left
# 235 "data/maps/DewfordTown/scripts.pory"
	walk_faster_left
# 236 "data/maps/DewfordTown/scripts.pory"
	walk_faster_left
# 237 "data/maps/DewfordTown/scripts.pory"
	walk_faster_left
# 238 "data/maps/DewfordTown/scripts.pory"
	walk_faster_left
# 239 "data/maps/DewfordTown/scripts.pory"
	walk_faster_left
# 240 "data/maps/DewfordTown/scripts.pory"
	walk_faster_left
# 241 "data/maps/DewfordTown/scripts.pory"
	walk_faster_left
# 242 "data/maps/DewfordTown/scripts.pory"
	walk_faster_left
# 243 "data/maps/DewfordTown/scripts.pory"
	walk_faster_left
# 244 "data/maps/DewfordTown/scripts.pory"
	walk_faster_left
# 245 "data/maps/DewfordTown/scripts.pory"
	walk_faster_left
# 246 "data/maps/DewfordTown/scripts.pory"
	walk_faster_left
# 247 "data/maps/DewfordTown/scripts.pory"
	walk_faster_left
# 248 "data/maps/DewfordTown/scripts.pory"
	walk_faster_left
# 249 "data/maps/DewfordTown/scripts.pory"
	walk_faster_left
# 250 "data/maps/DewfordTown/scripts.pory"
	walk_faster_left
# 251 "data/maps/DewfordTown/scripts.pory"
	walk_faster_left
# 252 "data/maps/DewfordTown/scripts.pory"
	walk_faster_left
# 253 "data/maps/DewfordTown/scripts.pory"
	walk_faster_left
# 254 "data/maps/DewfordTown/scripts.pory"
	walk_faster_left
# 255 "data/maps/DewfordTown/scripts.pory"
	walk_faster_left
# 256 "data/maps/DewfordTown/scripts.pory"
	walk_faster_left
# 257 "data/maps/DewfordTown/scripts.pory"
	walk_faster_left
# 258 "data/maps/DewfordTown/scripts.pory"
	walk_faster_left
# 259 "data/maps/DewfordTown/scripts.pory"
	walk_faster_left
# 260 "data/maps/DewfordTown/scripts.pory"
	walk_faster_left
# 261 "data/maps/DewfordTown/scripts.pory"
	walk_faster_left
# 262 "data/maps/DewfordTown/scripts.pory"
	walk_faster_left
# 263 "data/maps/DewfordTown/scripts.pory"
	walk_faster_left
# 264 "data/maps/DewfordTown/scripts.pory"
	walk_faster_left
# 265 "data/maps/DewfordTown/scripts.pory"
	walk_faster_left
# 266 "data/maps/DewfordTown/scripts.pory"
	walk_faster_left
# 267 "data/maps/DewfordTown/scripts.pory"
	walk_faster_left
# 268 "data/maps/DewfordTown/scripts.pory"
	walk_faster_left
# 269 "data/maps/DewfordTown/scripts.pory"
	walk_faster_left
# 270 "data/maps/DewfordTown/scripts.pory"
	walk_faster_left
# 271 "data/maps/DewfordTown/scripts.pory"
	walk_faster_left
# 272 "data/maps/DewfordTown/scripts.pory"
	walk_faster_left
# 273 "data/maps/DewfordTown/scripts.pory"
	walk_faster_left
# 274 "data/maps/DewfordTown/scripts.pory"
	walk_fast_left
# 275 "data/maps/DewfordTown/scripts.pory"
	walk_fast_left
# 276 "data/maps/DewfordTown/scripts.pory"
	walk_fast_left
# 277 "data/maps/DewfordTown/scripts.pory"
	walk_fast_left
# 278 "data/maps/DewfordTown/scripts.pory"
	walk_fast_up
# 279 "data/maps/DewfordTown/scripts.pory"
	walk_fast_up
# 280 "data/maps/DewfordTown/scripts.pory"
	walk_fast_up
# 281 "data/maps/DewfordTown/scripts.pory"
	walk_fast_up
# 282 "data/maps/DewfordTown/scripts.pory"
	walk_faster_up
# 283 "data/maps/DewfordTown/scripts.pory"
	walk_faster_up
# 284 "data/maps/DewfordTown/scripts.pory"
	walk_faster_up
# 285 "data/maps/DewfordTown/scripts.pory"
	walk_faster_up
# 286 "data/maps/DewfordTown/scripts.pory"
	walk_faster_up
# 287 "data/maps/DewfordTown/scripts.pory"
	walk_faster_up
# 288 "data/maps/DewfordTown/scripts.pory"
	walk_faster_up
# 289 "data/maps/DewfordTown/scripts.pory"
	walk_faster_up
# 290 "data/maps/DewfordTown/scripts.pory"
	walk_faster_up
# 291 "data/maps/DewfordTown/scripts.pory"
	walk_faster_up
# 292 "data/maps/DewfordTown/scripts.pory"
	walk_faster_up
# 293 "data/maps/DewfordTown/scripts.pory"
	walk_faster_up
# 294 "data/maps/DewfordTown/scripts.pory"
	walk_faster_up
# 295 "data/maps/DewfordTown/scripts.pory"
	walk_faster_up
# 296 "data/maps/DewfordTown/scripts.pory"
	walk_faster_up
# 297 "data/maps/DewfordTown/scripts.pory"
	walk_faster_up
# 298 "data/maps/DewfordTown/scripts.pory"
	walk_faster_up
# 299 "data/maps/DewfordTown/scripts.pory"
	walk_faster_up
# 300 "data/maps/DewfordTown/scripts.pory"
	walk_faster_up
# 301 "data/maps/DewfordTown/scripts.pory"
	walk_faster_up
# 302 "data/maps/DewfordTown/scripts.pory"
	walk_faster_up
# 303 "data/maps/DewfordTown/scripts.pory"
	walk_faster_up
# 304 "data/maps/DewfordTown/scripts.pory"
	walk_faster_up
# 305 "data/maps/DewfordTown/scripts.pory"
	walk_faster_up
# 306 "data/maps/DewfordTown/scripts.pory"
	walk_faster_up
# 307 "data/maps/DewfordTown/scripts.pory"
	walk_faster_up
# 308 "data/maps/DewfordTown/scripts.pory"
	walk_faster_up
# 309 "data/maps/DewfordTown/scripts.pory"
	walk_faster_up
# 310 "data/maps/DewfordTown/scripts.pory"
	walk_faster_up
# 311 "data/maps/DewfordTown/scripts.pory"
	walk_faster_up
# 312 "data/maps/DewfordTown/scripts.pory"
	walk_faster_up
# 313 "data/maps/DewfordTown/scripts.pory"
	walk_faster_up
# 314 "data/maps/DewfordTown/scripts.pory"
	walk_faster_up
# 315 "data/maps/DewfordTown/scripts.pory"
	walk_faster_up
# 316 "data/maps/DewfordTown/scripts.pory"
	walk_faster_up
# 317 "data/maps/DewfordTown/scripts.pory"
	walk_faster_up
# 318 "data/maps/DewfordTown/scripts.pory"
	walk_faster_up
# 319 "data/maps/DewfordTown/scripts.pory"
	walk_faster_up
# 320 "data/maps/DewfordTown/scripts.pory"
	walk_faster_up
# 321 "data/maps/DewfordTown/scripts.pory"
	walk_faster_up
# 322 "data/maps/DewfordTown/scripts.pory"
	walk_faster_up
# 323 "data/maps/DewfordTown/scripts.pory"
	walk_faster_up
# 324 "data/maps/DewfordTown/scripts.pory"
	walk_faster_up
# 325 "data/maps/DewfordTown/scripts.pory"
	walk_faster_up
# 326 "data/maps/DewfordTown/scripts.pory"
	walk_faster_up
# 327 "data/maps/DewfordTown/scripts.pory"
	walk_faster_up
# 328 "data/maps/DewfordTown/scripts.pory"
	walk_faster_up
# 329 "data/maps/DewfordTown/scripts.pory"
	walk_faster_up
# 330 "data/maps/DewfordTown/scripts.pory"
	walk_faster_up
# 331 "data/maps/DewfordTown/scripts.pory"
	walk_faster_up
# 332 "data/maps/DewfordTown/scripts.pory"
	walk_faster_up
# 333 "data/maps/DewfordTown/scripts.pory"
	walk_faster_up
# 334 "data/maps/DewfordTown/scripts.pory"
	walk_faster_up
# 335 "data/maps/DewfordTown/scripts.pory"
	walk_faster_up
# 336 "data/maps/DewfordTown/scripts.pory"
	walk_faster_up
# 337 "data/maps/DewfordTown/scripts.pory"
	walk_faster_up
# 338 "data/maps/DewfordTown/scripts.pory"
	walk_faster_up
# 339 "data/maps/DewfordTown/scripts.pory"
	walk_faster_up
# 340 "data/maps/DewfordTown/scripts.pory"
	walk_faster_up
# 341 "data/maps/DewfordTown/scripts.pory"
	walk_faster_up
# 342 "data/maps/DewfordTown/scripts.pory"
	walk_faster_up
# 343 "data/maps/DewfordTown/scripts.pory"
	walk_faster_up
# 344 "data/maps/DewfordTown/scripts.pory"
	walk_faster_up
# 345 "data/maps/DewfordTown/scripts.pory"
	walk_faster_up
# 346 "data/maps/DewfordTown/scripts.pory"
	walk_faster_up
# 347 "data/maps/DewfordTown/scripts.pory"
	walk_faster_up
# 348 "data/maps/DewfordTown/scripts.pory"
	walk_faster_up
# 349 "data/maps/DewfordTown/scripts.pory"
	walk_faster_up
# 350 "data/maps/DewfordTown/scripts.pory"
	walk_faster_up
# 351 "data/maps/DewfordTown/scripts.pory"
	walk_faster_up
# 352 "data/maps/DewfordTown/scripts.pory"
	walk_faster_up
# 353 "data/maps/DewfordTown/scripts.pory"
	walk_faster_up
# 354 "data/maps/DewfordTown/scripts.pory"
	walk_faster_up
# 355 "data/maps/DewfordTown/scripts.pory"
	walk_faster_up
# 356 "data/maps/DewfordTown/scripts.pory"
	walk_faster_up
# 357 "data/maps/DewfordTown/scripts.pory"
	walk_faster_up
# 358 "data/maps/DewfordTown/scripts.pory"
	walk_faster_up
# 359 "data/maps/DewfordTown/scripts.pory"
	walk_faster_up
# 360 "data/maps/DewfordTown/scripts.pory"
	walk_faster_up
# 361 "data/maps/DewfordTown/scripts.pory"
	walk_faster_up
# 362 "data/maps/DewfordTown/scripts.pory"
	walk_faster_up
# 363 "data/maps/DewfordTown/scripts.pory"
	walk_faster_up
# 364 "data/maps/DewfordTown/scripts.pory"
	walk_faster_up
# 365 "data/maps/DewfordTown/scripts.pory"
	walk_fast_up
# 366 "data/maps/DewfordTown/scripts.pory"
	walk_fast_up
# 367 "data/maps/DewfordTown/scripts.pory"
	walk_fast_up
# 368 "data/maps/DewfordTown/scripts.pory"
	walk_fast_up
# 369 "data/maps/DewfordTown/scripts.pory"
	walk_fast_left
# 370 "data/maps/DewfordTown/scripts.pory"
	walk_fast_left
# 371 "data/maps/DewfordTown/scripts.pory"
	walk_faster_left
# 372 "data/maps/DewfordTown/scripts.pory"
	walk_faster_left
# 373 "data/maps/DewfordTown/scripts.pory"
	walk_faster_left
# 374 "data/maps/DewfordTown/scripts.pory"
	walk_faster_left
# 375 "data/maps/DewfordTown/scripts.pory"
	walk_faster_left
# 376 "data/maps/DewfordTown/scripts.pory"
	walk_faster_left
# 377 "data/maps/DewfordTown/scripts.pory"
	walk_fast_left
# 378 "data/maps/DewfordTown/scripts.pory"
	walk_fast_left
# 379 "data/maps/DewfordTown/scripts.pory"
	walk_fast_up
# 380 "data/maps/DewfordTown/scripts.pory"
	walk_fast_up
# 381 "data/maps/DewfordTown/scripts.pory"
	walk_faster_up
# 382 "data/maps/DewfordTown/scripts.pory"
	walk_faster_up
# 383 "data/maps/DewfordTown/scripts.pory"
	walk_faster_up
# 384 "data/maps/DewfordTown/scripts.pory"
	walk_faster_up
# 385 "data/maps/DewfordTown/scripts.pory"
	walk_faster_up
# 386 "data/maps/DewfordTown/scripts.pory"
	walk_faster_up
# 387 "data/maps/DewfordTown/scripts.pory"
	walk_faster_up
# 388 "data/maps/DewfordTown/scripts.pory"
	walk_faster_up
# 389 "data/maps/DewfordTown/scripts.pory"
	walk_faster_up
# 390 "data/maps/DewfordTown/scripts.pory"
	walk_faster_up
# 391 "data/maps/DewfordTown/scripts.pory"
	walk_faster_up
# 392 "data/maps/DewfordTown/scripts.pory"
	walk_faster_up
# 393 "data/maps/DewfordTown/scripts.pory"
	walk_faster_up
# 394 "data/maps/DewfordTown/scripts.pory"
	walk_faster_up
# 395 "data/maps/DewfordTown/scripts.pory"
	walk_fast_up
# 396 "data/maps/DewfordTown/scripts.pory"
	walk_fast_up
# 397 "data/maps/DewfordTown/scripts.pory"
	walk_fast_up
# 398 "data/maps/DewfordTown/scripts.pory"
	walk_fast_up
# 399 "data/maps/DewfordTown/scripts.pory"
	walk_up
# 400 "data/maps/DewfordTown/scripts.pory"
	walk_up
# 401 "data/maps/DewfordTown/scripts.pory"
	walk_up
# 402 "data/maps/DewfordTown/scripts.pory"
	walk_up
# 403 "data/maps/DewfordTown/scripts.pory"
	step_end
# 404 "data/maps/DewfordTown/scripts.pory"

# 405 "data/maps/DewfordTown/scripts.pory"
DewfordTown_Movement_SailToSlateport:
# 406 "data/maps/DewfordTown/scripts.pory"
	walk_right
# 407 "data/maps/DewfordTown/scripts.pory"
	walk_fast_right
# 408 "data/maps/DewfordTown/scripts.pory"
	walk_fast_right
# 409 "data/maps/DewfordTown/scripts.pory"
	walk_faster_right
# 410 "data/maps/DewfordTown/scripts.pory"
	walk_faster_right
# 411 "data/maps/DewfordTown/scripts.pory"
	walk_faster_right
# 412 "data/maps/DewfordTown/scripts.pory"
	walk_faster_right
# 413 "data/maps/DewfordTown/scripts.pory"
	walk_faster_right
# 414 "data/maps/DewfordTown/scripts.pory"
	walk_faster_right
# 415 "data/maps/DewfordTown/scripts.pory"
	walk_faster_right
# 416 "data/maps/DewfordTown/scripts.pory"
	walk_faster_right
# 417 "data/maps/DewfordTown/scripts.pory"
	walk_faster_right
# 418 "data/maps/DewfordTown/scripts.pory"
	walk_faster_right
# 419 "data/maps/DewfordTown/scripts.pory"
	walk_faster_right
# 420 "data/maps/DewfordTown/scripts.pory"
	walk_faster_right
# 421 "data/maps/DewfordTown/scripts.pory"
	walk_faster_right
# 422 "data/maps/DewfordTown/scripts.pory"
	walk_faster_right
# 423 "data/maps/DewfordTown/scripts.pory"
	walk_faster_right
# 424 "data/maps/DewfordTown/scripts.pory"
	walk_faster_right
# 425 "data/maps/DewfordTown/scripts.pory"
	walk_faster_right
# 426 "data/maps/DewfordTown/scripts.pory"
	walk_faster_right
# 427 "data/maps/DewfordTown/scripts.pory"
	walk_faster_right
# 428 "data/maps/DewfordTown/scripts.pory"
	walk_faster_right
# 429 "data/maps/DewfordTown/scripts.pory"
	walk_faster_right
# 430 "data/maps/DewfordTown/scripts.pory"
	walk_faster_right
# 431 "data/maps/DewfordTown/scripts.pory"
	walk_faster_right
# 432 "data/maps/DewfordTown/scripts.pory"
	walk_faster_right
# 433 "data/maps/DewfordTown/scripts.pory"
	walk_faster_right
# 434 "data/maps/DewfordTown/scripts.pory"
	walk_faster_right
# 435 "data/maps/DewfordTown/scripts.pory"
	walk_faster_right
# 436 "data/maps/DewfordTown/scripts.pory"
	walk_faster_right
# 437 "data/maps/DewfordTown/scripts.pory"
	walk_faster_right
# 438 "data/maps/DewfordTown/scripts.pory"
	walk_faster_right
# 439 "data/maps/DewfordTown/scripts.pory"
	walk_faster_right
# 440 "data/maps/DewfordTown/scripts.pory"
	walk_faster_right
# 441 "data/maps/DewfordTown/scripts.pory"
	walk_faster_right
# 442 "data/maps/DewfordTown/scripts.pory"
	walk_faster_right
# 443 "data/maps/DewfordTown/scripts.pory"
	walk_faster_right
# 444 "data/maps/DewfordTown/scripts.pory"
	walk_faster_right
# 445 "data/maps/DewfordTown/scripts.pory"
	walk_faster_right
# 446 "data/maps/DewfordTown/scripts.pory"
	walk_faster_right
# 447 "data/maps/DewfordTown/scripts.pory"
	walk_faster_right
# 448 "data/maps/DewfordTown/scripts.pory"
	walk_faster_right
# 449 "data/maps/DewfordTown/scripts.pory"
	walk_faster_right
# 450 "data/maps/DewfordTown/scripts.pory"
	walk_faster_right
# 451 "data/maps/DewfordTown/scripts.pory"
	walk_faster_right
# 452 "data/maps/DewfordTown/scripts.pory"
	walk_faster_right
# 453 "data/maps/DewfordTown/scripts.pory"
	walk_faster_right
# 454 "data/maps/DewfordTown/scripts.pory"
	walk_faster_right
# 455 "data/maps/DewfordTown/scripts.pory"
	walk_faster_right
# 456 "data/maps/DewfordTown/scripts.pory"
	walk_faster_right
# 457 "data/maps/DewfordTown/scripts.pory"
	walk_faster_right
# 458 "data/maps/DewfordTown/scripts.pory"
	walk_faster_right
# 459 "data/maps/DewfordTown/scripts.pory"
	walk_faster_right
# 460 "data/maps/DewfordTown/scripts.pory"
	walk_faster_right
# 461 "data/maps/DewfordTown/scripts.pory"
	walk_faster_right
# 462 "data/maps/DewfordTown/scripts.pory"
	walk_faster_right
# 463 "data/maps/DewfordTown/scripts.pory"
	walk_faster_right
# 464 "data/maps/DewfordTown/scripts.pory"
	walk_faster_right
# 465 "data/maps/DewfordTown/scripts.pory"
	walk_faster_right
# 466 "data/maps/DewfordTown/scripts.pory"
	walk_faster_right
# 467 "data/maps/DewfordTown/scripts.pory"
	walk_faster_right
# 468 "data/maps/DewfordTown/scripts.pory"
	walk_faster_right
# 469 "data/maps/DewfordTown/scripts.pory"
	walk_faster_right
# 470 "data/maps/DewfordTown/scripts.pory"
	walk_faster_right
# 471 "data/maps/DewfordTown/scripts.pory"
	walk_faster_right
# 472 "data/maps/DewfordTown/scripts.pory"
	walk_faster_right
# 473 "data/maps/DewfordTown/scripts.pory"
	walk_faster_right
# 474 "data/maps/DewfordTown/scripts.pory"
	walk_faster_right
# 475 "data/maps/DewfordTown/scripts.pory"
	walk_faster_right
# 476 "data/maps/DewfordTown/scripts.pory"
	walk_faster_right
# 477 "data/maps/DewfordTown/scripts.pory"
	walk_faster_right
# 478 "data/maps/DewfordTown/scripts.pory"
	walk_faster_right
# 479 "data/maps/DewfordTown/scripts.pory"
	walk_faster_right
# 480 "data/maps/DewfordTown/scripts.pory"
	walk_faster_right
# 481 "data/maps/DewfordTown/scripts.pory"
	walk_faster_right
# 482 "data/maps/DewfordTown/scripts.pory"
	walk_faster_right
# 483 "data/maps/DewfordTown/scripts.pory"
	walk_faster_right
# 484 "data/maps/DewfordTown/scripts.pory"
	walk_faster_right
# 485 "data/maps/DewfordTown/scripts.pory"
	walk_faster_right
# 486 "data/maps/DewfordTown/scripts.pory"
	walk_faster_right
# 487 "data/maps/DewfordTown/scripts.pory"
	walk_faster_right
# 488 "data/maps/DewfordTown/scripts.pory"
	walk_faster_right
# 489 "data/maps/DewfordTown/scripts.pory"
	walk_faster_right
# 490 "data/maps/DewfordTown/scripts.pory"
	walk_faster_right
# 491 "data/maps/DewfordTown/scripts.pory"
	walk_faster_right
# 492 "data/maps/DewfordTown/scripts.pory"
	walk_faster_right
# 493 "data/maps/DewfordTown/scripts.pory"
	walk_faster_right
# 494 "data/maps/DewfordTown/scripts.pory"
	walk_faster_right
# 495 "data/maps/DewfordTown/scripts.pory"
	walk_faster_right
# 496 "data/maps/DewfordTown/scripts.pory"
	walk_faster_right
# 497 "data/maps/DewfordTown/scripts.pory"
	walk_faster_right
# 498 "data/maps/DewfordTown/scripts.pory"
	walk_faster_right
# 499 "data/maps/DewfordTown/scripts.pory"
	walk_faster_right
# 500 "data/maps/DewfordTown/scripts.pory"
	walk_faster_right
# 501 "data/maps/DewfordTown/scripts.pory"
	walk_faster_right
# 502 "data/maps/DewfordTown/scripts.pory"
	walk_faster_right
# 503 "data/maps/DewfordTown/scripts.pory"
	walk_faster_right
# 504 "data/maps/DewfordTown/scripts.pory"
	walk_faster_right
# 505 "data/maps/DewfordTown/scripts.pory"
	walk_faster_right
# 506 "data/maps/DewfordTown/scripts.pory"
	walk_faster_right
# 507 "data/maps/DewfordTown/scripts.pory"
	walk_faster_right
# 508 "data/maps/DewfordTown/scripts.pory"
	walk_faster_right
# 509 "data/maps/DewfordTown/scripts.pory"
	walk_faster_right
# 510 "data/maps/DewfordTown/scripts.pory"
	walk_faster_right
# 511 "data/maps/DewfordTown/scripts.pory"
	walk_faster_right
# 512 "data/maps/DewfordTown/scripts.pory"
	walk_faster_right
# 513 "data/maps/DewfordTown/scripts.pory"
	walk_faster_right
# 514 "data/maps/DewfordTown/scripts.pory"
	walk_faster_right
# 515 "data/maps/DewfordTown/scripts.pory"
	walk_faster_right
# 516 "data/maps/DewfordTown/scripts.pory"
	walk_faster_right
# 517 "data/maps/DewfordTown/scripts.pory"
	walk_faster_right
# 518 "data/maps/DewfordTown/scripts.pory"
	walk_faster_right
# 519 "data/maps/DewfordTown/scripts.pory"
	walk_faster_right
# 520 "data/maps/DewfordTown/scripts.pory"
	walk_faster_right
# 521 "data/maps/DewfordTown/scripts.pory"
	walk_faster_right
# 522 "data/maps/DewfordTown/scripts.pory"
	walk_faster_right
# 523 "data/maps/DewfordTown/scripts.pory"
	walk_faster_right
# 524 "data/maps/DewfordTown/scripts.pory"
	walk_faster_right
# 525 "data/maps/DewfordTown/scripts.pory"
	walk_faster_right
# 526 "data/maps/DewfordTown/scripts.pory"
	walk_faster_right
# 527 "data/maps/DewfordTown/scripts.pory"
	walk_faster_right
# 528 "data/maps/DewfordTown/scripts.pory"
	walk_faster_right
# 529 "data/maps/DewfordTown/scripts.pory"
	walk_faster_right
# 530 "data/maps/DewfordTown/scripts.pory"
	walk_faster_right
# 531 "data/maps/DewfordTown/scripts.pory"
	walk_faster_right
# 532 "data/maps/DewfordTown/scripts.pory"
	walk_faster_right
# 533 "data/maps/DewfordTown/scripts.pory"
	walk_faster_right
# 534 "data/maps/DewfordTown/scripts.pory"
	walk_faster_right
# 535 "data/maps/DewfordTown/scripts.pory"
	walk_faster_right
# 536 "data/maps/DewfordTown/scripts.pory"
	walk_faster_right
# 537 "data/maps/DewfordTown/scripts.pory"
	walk_faster_right
# 538 "data/maps/DewfordTown/scripts.pory"
	walk_faster_right
# 539 "data/maps/DewfordTown/scripts.pory"
	walk_faster_right
# 540 "data/maps/DewfordTown/scripts.pory"
	walk_faster_right
# 541 "data/maps/DewfordTown/scripts.pory"
	walk_faster_right
# 542 "data/maps/DewfordTown/scripts.pory"
	walk_faster_right
# 543 "data/maps/DewfordTown/scripts.pory"
	walk_faster_right
# 544 "data/maps/DewfordTown/scripts.pory"
	walk_faster_right
# 545 "data/maps/DewfordTown/scripts.pory"
	walk_faster_right
# 546 "data/maps/DewfordTown/scripts.pory"
	walk_faster_right
# 547 "data/maps/DewfordTown/scripts.pory"
	walk_faster_right
# 548 "data/maps/DewfordTown/scripts.pory"
	walk_faster_right
# 549 "data/maps/DewfordTown/scripts.pory"
	walk_faster_right
# 550 "data/maps/DewfordTown/scripts.pory"
	walk_faster_right
# 551 "data/maps/DewfordTown/scripts.pory"
	walk_faster_right
# 552 "data/maps/DewfordTown/scripts.pory"
	walk_faster_right
# 553 "data/maps/DewfordTown/scripts.pory"
	walk_fast_right
# 554 "data/maps/DewfordTown/scripts.pory"
	walk_fast_right
# 555 "data/maps/DewfordTown/scripts.pory"
	walk_fast_up
# 556 "data/maps/DewfordTown/scripts.pory"
	walk_fast_up
# 557 "data/maps/DewfordTown/scripts.pory"
	walk_faster_up
# 558 "data/maps/DewfordTown/scripts.pory"
	walk_faster_up
# 559 "data/maps/DewfordTown/scripts.pory"
	walk_faster_up
# 560 "data/maps/DewfordTown/scripts.pory"
	walk_faster_up
# 561 "data/maps/DewfordTown/scripts.pory"
	walk_faster_up
# 562 "data/maps/DewfordTown/scripts.pory"
	walk_faster_up
# 563 "data/maps/DewfordTown/scripts.pory"
	walk_faster_up
# 564 "data/maps/DewfordTown/scripts.pory"
	walk_faster_up
# 565 "data/maps/DewfordTown/scripts.pory"
	walk_faster_up
# 566 "data/maps/DewfordTown/scripts.pory"
	walk_faster_up
# 567 "data/maps/DewfordTown/scripts.pory"
	walk_fast_up
# 568 "data/maps/DewfordTown/scripts.pory"
	walk_fast_up
# 569 "data/maps/DewfordTown/scripts.pory"
	walk_fast_up
# 570 "data/maps/DewfordTown/scripts.pory"
	walk_fast_up
# 571 "data/maps/DewfordTown/scripts.pory"
	walk_fast_up
# 572 "data/maps/DewfordTown/scripts.pory"
	walk_fast_up
# 573 "data/maps/DewfordTown/scripts.pory"
	walk_fast_up
# 574 "data/maps/DewfordTown/scripts.pory"
	walk_fast_up
# 575 "data/maps/DewfordTown/scripts.pory"
	walk_fast_up
# 576 "data/maps/DewfordTown/scripts.pory"
	walk_fast_up
# 577 "data/maps/DewfordTown/scripts.pory"
	step_end
# 578 "data/maps/DewfordTown/scripts.pory"

# 579 "data/maps/DewfordTown/scripts.pory"
DewfordTown_Movement_PlayerBoardBoat:
# 580 "data/maps/DewfordTown/scripts.pory"
	walk_right
# 581 "data/maps/DewfordTown/scripts.pory"
	walk_up
# 582 "data/maps/DewfordTown/scripts.pory"
	step_end
# 583 "data/maps/DewfordTown/scripts.pory"

# 584 "data/maps/DewfordTown/scripts.pory"
DewfordTown_Movement_ExitBoatPetalburg:
# 585 "data/maps/DewfordTown/scripts.pory"
	walk_up
# 586 "data/maps/DewfordTown/scripts.pory"
	walk_up
# 587 "data/maps/DewfordTown/scripts.pory"
	walk_up
# 588 "data/maps/DewfordTown/scripts.pory"
	step_end
# 589 "data/maps/DewfordTown/scripts.pory"

# 590 "data/maps/DewfordTown/scripts.pory"
DewfordTown_Movement_ExitBoatSlateport:
# 591 "data/maps/DewfordTown/scripts.pory"
	walk_up
# 592 "data/maps/DewfordTown/scripts.pory"
	walk_up
# 593 "data/maps/DewfordTown/scripts.pory"
	walk_up
# 594 "data/maps/DewfordTown/scripts.pory"
	walk_in_place_faster_down
# 595 "data/maps/DewfordTown/scripts.pory"
	step_end
# 596 "data/maps/DewfordTown/scripts.pory"

# 597 "data/maps/DewfordTown/scripts.pory"
DewfordTown_Movement_BrineyBoardBoat:
# 598 "data/maps/DewfordTown/scripts.pory"
	walk_up
# 599 "data/maps/DewfordTown/scripts.pory"
	step_end
# 600 "data/maps/DewfordTown/scripts.pory"

# 601 "data/maps/DewfordTown/scripts.pory"
DewfordTown_Movement_BrineyExitBoat:
# 602 "data/maps/DewfordTown/scripts.pory"
	walk_up
# 603 "data/maps/DewfordTown/scripts.pory"
	walk_up
# 604 "data/maps/DewfordTown/scripts.pory"
	step_end
# 605 "data/maps/DewfordTown/scripts.pory"

# 606 "data/maps/DewfordTown/scripts.pory"
DewfordTown_EventScript_TrendyPhraseBoy::
# 607 "data/maps/DewfordTown/scripts.pory"
	lock
# 608 "data/maps/DewfordTown/scripts.pory"
	faceplayer
# 609 "data/maps/DewfordTown/scripts.pory"
	call Common_EventScript_BufferTrendyPhrase
# 610 "data/maps/DewfordTown/scripts.pory"
	msgbox DewfordTown_Text_XIsTheBiggestHappeningThingRight, MSGBOX_YESNO
# 611 "data/maps/DewfordTown/scripts.pory"
	goto_if_eq VAR_RESULT, YES, DewfordTown_EventScript_ConfirmTrendyPhrase
# 612 "data/maps/DewfordTown/scripts.pory"
	goto_if_eq VAR_RESULT, NO, DewfordTown_EventScript_RejectTrendyPhrase
# 613 "data/maps/DewfordTown/scripts.pory"
	end
# 614 "data/maps/DewfordTown/scripts.pory"

# 615 "data/maps/DewfordTown/scripts.pory"
DewfordTown_EventScript_ConfirmTrendyPhrase::
# 616 "data/maps/DewfordTown/scripts.pory"
	msgbox DewfordTown_Text_YeahDefinitionOfInRightNow, MSGBOX_DEFAULT
# 617 "data/maps/DewfordTown/scripts.pory"
	release
# 618 "data/maps/DewfordTown/scripts.pory"
	end
# 619 "data/maps/DewfordTown/scripts.pory"

# 620 "data/maps/DewfordTown/scripts.pory"
DewfordTown_EventScript_RejectTrendyPhrase::
# 621 "data/maps/DewfordTown/scripts.pory"
	msgbox DewfordTown_Text_TellMeWhatsNewAndIn, MSGBOX_DEFAULT
# 622 "data/maps/DewfordTown/scripts.pory"
	setvar VAR_0x8004, EASY_CHAT_TYPE_TRENDY_PHRASE
# 623 "data/maps/DewfordTown/scripts.pory"
	call Common_ShowEasyChatScreen
# 624 "data/maps/DewfordTown/scripts.pory"
	lock
# 625 "data/maps/DewfordTown/scripts.pory"
	faceplayer
# 626 "data/maps/DewfordTown/scripts.pory"
	goto_if_eq VAR_RESULT, TRUE, DewfordTown_EventScript_GiveNewTrendyPhrase
# 627 "data/maps/DewfordTown/scripts.pory"
	goto_if_eq VAR_RESULT, FALSE, DewfordTown_EventScript_CancelNewTrendyPhrase
# 628 "data/maps/DewfordTown/scripts.pory"
	end
# 629 "data/maps/DewfordTown/scripts.pory"

# 630 "data/maps/DewfordTown/scripts.pory"
DewfordTown_EventScript_GiveNewTrendyPhrase::
# 631 "data/maps/DewfordTown/scripts.pory"
	incrementgamestat GAME_STAT_STARTED_TRENDS
# 632 "data/maps/DewfordTown/scripts.pory"
	goto_if_eq VAR_0x8004, FALSE, DewfordTown_EventScript_PhraseNotTrendyEnough
# 633 "data/maps/DewfordTown/scripts.pory"
	msgbox DewfordTown_Text_OfCourseIKnowAboutThat, MSGBOX_DEFAULT
# 634 "data/maps/DewfordTown/scripts.pory"
	release
# 635 "data/maps/DewfordTown/scripts.pory"
	end
# 636 "data/maps/DewfordTown/scripts.pory"

# 637 "data/maps/DewfordTown/scripts.pory"
DewfordTown_EventScript_CancelNewTrendyPhrase::
# 638 "data/maps/DewfordTown/scripts.pory"
	msgbox DewfordTown_Text_HearOfAnyTrendsComeShareWithMe, MSGBOX_DEFAULT
# 639 "data/maps/DewfordTown/scripts.pory"
	release
# 640 "data/maps/DewfordTown/scripts.pory"
	end
# 641 "data/maps/DewfordTown/scripts.pory"

# 642 "data/maps/DewfordTown/scripts.pory"
DewfordTown_EventScript_PhraseNotTrendyEnough::
# 643 "data/maps/DewfordTown/scripts.pory"
	msgbox DewfordTown_Text_XHuhIThinkYIsCool, MSGBOX_DEFAULT
# 644 "data/maps/DewfordTown/scripts.pory"
	release
# 645 "data/maps/DewfordTown/scripts.pory"
	end
# 646 "data/maps/DewfordTown/scripts.pory"

# 647 "data/maps/DewfordTown/scripts.pory"
DewfordTown_Text_TinyIslandCommunity:
# 648 "data/maps/DewfordTown/scripts.pory"
	.string "Dewford is a tiny island community.\n"
# 649 "data/maps/DewfordTown/scripts.pory"
	.string "If something gets trendy here,\l"
# 650 "data/maps/DewfordTown/scripts.pory"
	.string "everyone picks up on it right away.$"
# 651 "data/maps/DewfordTown/scripts.pory"

# 652 "data/maps/DewfordTown/scripts.pory"
DewfordTown_Text_TownSign:
# 653 "data/maps/DewfordTown/scripts.pory"
	.string "Dewford Town\n"
# 654 "data/maps/DewfordTown/scripts.pory"
	.string "“A tiny island in the blue sea.”$"
# 655 "data/maps/DewfordTown/scripts.pory"

# 656 "data/maps/DewfordTown/scripts.pory"
DewfordTown_Text_GymSign:
# 657 "data/maps/DewfordTown/scripts.pory"
	.string "Dewford Town Pokémon Gym\n"
# 658 "data/maps/DewfordTown/scripts.pory"
	.string "Leader: Brawly\l"
# 659 "data/maps/DewfordTown/scripts.pory"
	.string "“A big wave in fighting!”$"
# 660 "data/maps/DewfordTown/scripts.pory"

# 661 "data/maps/DewfordTown/scripts.pory"
DewfordTown_Text_HallSign:
# 662 "data/maps/DewfordTown/scripts.pory"
	.string "Dewford Hall\n"
# 663 "data/maps/DewfordTown/scripts.pory"
	.string "“Everyone's information exchange!”$"
# 664 "data/maps/DewfordTown/scripts.pory"

# 665 "data/maps/DewfordTown/scripts.pory"
Route104_Text_LandedInDewfordDeliverLetter:
# 666 "data/maps/DewfordTown/scripts.pory"
	.string "Mr. Briney: Ahoy!\n"
# 667 "data/maps/DewfordTown/scripts.pory"
	.string "We've hit land in Dewford.\p"
# 668 "data/maps/DewfordTown/scripts.pory"
	.string "I suppose you're off to deliver that\n"
# 669 "data/maps/DewfordTown/scripts.pory"
	.string "Letter to, who was it now, Steven!$"
# 670 "data/maps/DewfordTown/scripts.pory"

# 671 "data/maps/DewfordTown/scripts.pory"
DewfordTown_Text_SetSailBackToPetalburg:
# 672 "data/maps/DewfordTown/scripts.pory"
	.string "Mr. Briney: Have you delivered your\n"
# 673 "data/maps/DewfordTown/scripts.pory"
	.string "Letter?\p"
# 674 "data/maps/DewfordTown/scripts.pory"
	.string "Or were you meaning to sail back to\n"
# 675 "data/maps/DewfordTown/scripts.pory"
	.string "Petalburg?$"
# 676 "data/maps/DewfordTown/scripts.pory"

# 677 "data/maps/DewfordTown/scripts.pory"
DewfordTown_Text_PetalburgWereSettingSail2:
# 678 "data/maps/DewfordTown/scripts.pory"
	.string "Mr. Briney: Petalburg it is, then!\p"
# 679 "data/maps/DewfordTown/scripts.pory"
	.string "Anchors aweigh!\n"
# 680 "data/maps/DewfordTown/scripts.pory"
	.string "Peeko, we're setting sail, my darling!$"
# 681 "data/maps/DewfordTown/scripts.pory"

# 682 "data/maps/DewfordTown/scripts.pory"
DewfordTown_Text_GoDeliverIllBeWaiting:
# 683 "data/maps/DewfordTown/scripts.pory"
	.string "Mr. Briney: Then you go on and deliver\n"
# 684 "data/maps/DewfordTown/scripts.pory"
	.string "the Letter. I'll be waiting.$"
# 685 "data/maps/DewfordTown/scripts.pory"

# 686 "data/maps/DewfordTown/scripts.pory"
DewfordTown_Text_BrineyLandedInDewford:
# 687 "data/maps/DewfordTown/scripts.pory"
	.string "Mr. Briney: Ahoy!\n"
# 688 "data/maps/DewfordTown/scripts.pory"
	.string "We've hit land in Dewford!\p"
# 689 "data/maps/DewfordTown/scripts.pory"
	.string "You just go on and tell me whenever\n"
# 690 "data/maps/DewfordTown/scripts.pory"
	.string "you want to set sail again!$"
# 691 "data/maps/DewfordTown/scripts.pory"

# 692 "data/maps/DewfordTown/scripts.pory"
DewfordTown_Text_WhereAreWeBound:
# 693 "data/maps/DewfordTown/scripts.pory"
	.string "Mr. Briney: Ahoy!\n"
# 694 "data/maps/DewfordTown/scripts.pory"
	.string "For you, I'll go out to sea anytime!\p"
# 695 "data/maps/DewfordTown/scripts.pory"
	.string "Now, my friend, where are we bound?$"
# 696 "data/maps/DewfordTown/scripts.pory"

# 697 "data/maps/DewfordTown/scripts.pory"
DewfordTown_Text_PetalburgWereSettingSail:
# 698 "data/maps/DewfordTown/scripts.pory"
	.string "Mr. Briney: Petalburg, is it?\p"
# 699 "data/maps/DewfordTown/scripts.pory"
	.string "Anchors aweigh!\n"
# 700 "data/maps/DewfordTown/scripts.pory"
	.string "Peeko, we're setting sail, my darling!$"
# 701 "data/maps/DewfordTown/scripts.pory"

# 702 "data/maps/DewfordTown/scripts.pory"
DewfordTown_Text_SlateportWereSettingSail:
# 703 "data/maps/DewfordTown/scripts.pory"
	.string "Mr. Briney: Slateport, is it?\p"
# 704 "data/maps/DewfordTown/scripts.pory"
	.string "Anchors aweigh!\n"
# 705 "data/maps/DewfordTown/scripts.pory"
	.string "Peeko, we're setting sail, my darling!$"
# 706 "data/maps/DewfordTown/scripts.pory"

# 707 "data/maps/DewfordTown/scripts.pory"
DewfordTown_Text_JustTellMeWhenYouNeedToSetSail:
# 708 "data/maps/DewfordTown/scripts.pory"
	.string "Mr. Briney: You just tell me whenever\n"
# 709 "data/maps/DewfordTown/scripts.pory"
	.string "you need to set sail again!$"
# 710 "data/maps/DewfordTown/scripts.pory"

# 711 "data/maps/DewfordTown/scripts.pory"
DewfordTown_Text_XIsTheBiggestHappeningThingRight:
# 712 "data/maps/DewfordTown/scripts.pory"
	.string "I like what's hip, happening, and trendy.\n"
# 713 "data/maps/DewfordTown/scripts.pory"
	.string "I'm always checking it out.\p"
# 714 "data/maps/DewfordTown/scripts.pory"
	.string "Listen, have you heard about this new\n"
# 715 "data/maps/DewfordTown/scripts.pory"
	.string "“{STR_VAR_1}”?\p"
# 716 "data/maps/DewfordTown/scripts.pory"
	.string "That's right!\n"
# 717 "data/maps/DewfordTown/scripts.pory"
	.string "Of course you know!\p"
# 718 "data/maps/DewfordTown/scripts.pory"
	.string "I mean, sheesh,\n"
# 719 "data/maps/DewfordTown/scripts.pory"
	.string "“{STR_VAR_1}”…\l"
# 720 "data/maps/DewfordTown/scripts.pory"
	.string "It's the hottest thing in cool!\p"
# 721 "data/maps/DewfordTown/scripts.pory"
	.string "Wherever you're from,\n"
# 722 "data/maps/DewfordTown/scripts.pory"
	.string "“{STR_VAR_1}”\l"
# 723 "data/maps/DewfordTown/scripts.pory"
	.string "is the biggest happening thing, right?$"
# 724 "data/maps/DewfordTown/scripts.pory"

# 725 "data/maps/DewfordTown/scripts.pory"
DewfordTown_Text_TellMeWhatsNewAndIn:
# 726 "data/maps/DewfordTown/scripts.pory"
	.string "Hunh?\n"
# 727 "data/maps/DewfordTown/scripts.pory"
	.string "It's not the hip and happening thing?\p"
# 728 "data/maps/DewfordTown/scripts.pory"
	.string "Well, hey, you have to tell me,\n"
# 729 "data/maps/DewfordTown/scripts.pory"
	.string "what's new and what's “in”?$"
# 730 "data/maps/DewfordTown/scripts.pory"

# 731 "data/maps/DewfordTown/scripts.pory"
DewfordTown_Text_OfCourseIKnowAboutThat:
# 732 "data/maps/DewfordTown/scripts.pory"
	.string "Hunh?\n"
# 733 "data/maps/DewfordTown/scripts.pory"
	.string "“{STR_VAR_2}”?\p"
# 734 "data/maps/DewfordTown/scripts.pory"
	.string "… …\p"
# 735 "data/maps/DewfordTown/scripts.pory"
	.string "…Uh… Yeah! That's right!\n"
# 736 "data/maps/DewfordTown/scripts.pory"
	.string "Yeah, I knew that! Knew it all along!\p"
# 737 "data/maps/DewfordTown/scripts.pory"
	.string "Of course I know about that!\n"
# 738 "data/maps/DewfordTown/scripts.pory"
	.string "“{STR_VAR_2},” right?\p"
# 739 "data/maps/DewfordTown/scripts.pory"
	.string "Yeah, that's it, it's there!\n"
# 740 "data/maps/DewfordTown/scripts.pory"
	.string "Isn't “{STR_VAR_2}”\l"
# 741 "data/maps/DewfordTown/scripts.pory"
	.string "the coolest, or what?\p"
# 742 "data/maps/DewfordTown/scripts.pory"
	.string "It's the hippest thing in hip.\n"
# 743 "data/maps/DewfordTown/scripts.pory"
	.string "You think I'd not know about it?\p"
# 744 "data/maps/DewfordTown/scripts.pory"
	.string "“{STR_VAR_1}”…\n"
# 745 "data/maps/DewfordTown/scripts.pory"
	.string "It's, like, so five minutes ago.\p"
# 746 "data/maps/DewfordTown/scripts.pory"
	.string "Now, “{STR_VAR_2}” is\n"
# 747 "data/maps/DewfordTown/scripts.pory"
	.string "what's vital and in tune with the times!$"
# 748 "data/maps/DewfordTown/scripts.pory"

# 749 "data/maps/DewfordTown/scripts.pory"
DewfordTown_Text_XHuhIThinkYIsCool:
# 750 "data/maps/DewfordTown/scripts.pory"
	.string "Hmm…\n"
# 751 "data/maps/DewfordTown/scripts.pory"
	.string "“{STR_VAR_2},” huh?\p"
# 752 "data/maps/DewfordTown/scripts.pory"
	.string "But personally, I think\n"
# 753 "data/maps/DewfordTown/scripts.pory"
	.string "“{STR_VAR_1}”\l"
# 754 "data/maps/DewfordTown/scripts.pory"
	.string "is what's real in cool.$"
# 755 "data/maps/DewfordTown/scripts.pory"

# 756 "data/maps/DewfordTown/scripts.pory"
DewfordTown_Text_HearOfAnyTrendsComeShareWithMe:
# 757 "data/maps/DewfordTown/scripts.pory"
	.string "Well, if you hear of any happening new\n"
# 758 "data/maps/DewfordTown/scripts.pory"
	.string "trends, come share them with me, okay?$"
# 759 "data/maps/DewfordTown/scripts.pory"

# 760 "data/maps/DewfordTown/scripts.pory"
DewfordTown_Text_YeahDefinitionOfInRightNow:
# 761 "data/maps/DewfordTown/scripts.pory"
	.string "Yeah, absolutely right!\p"
# 762 "data/maps/DewfordTown/scripts.pory"
	.string "“{STR_VAR_1}” is the\n"
# 763 "data/maps/DewfordTown/scripts.pory"
	.string "definition of “in” right now.$"
