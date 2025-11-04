DemoTown_Library_MapScripts::
	.byte 0

/* *************************** */

DemoTown_Library_Bookshelf1_Text_Category:
	.string "It's a book about debug commands.\n"
	.string "What part do you want to read?$"

DemoTown_Library_Bookshelf1_Text_OverworldDebugMenu:
	.string "Overworld Debug Menu$"

DemoTown_Library_Bookshelf1_Text_PokemonSpriteVisualizer:
	.string "{PKMN} Sprite Visualizer$"

DemoTown_Library_Bookshelf1_Text_BattleDebugMenu:
	.string "Battle Debug Menu$"

DemoTown_Library_Bookshelf1_Text_Close:
	.string "Close$"

DemoTown_Library_Bookshelf1::
	lockall
	msgbox DemoTown_Library_Bookshelf1_Text_Category, MSGBOX_DEFAULT
	dynmultipush DemoTown_Library_Bookshelf1_Text_OverworldDebugMenu, 0
	dynmultipush DemoTown_Library_Bookshelf1_Text_PokemonSpriteVisualizer, 1
	dynmultipush DemoTown_Library_Bookshelf1_Text_BattleDebugMenu, 2
	dynmultipush DemoTown_Library_Bookshelf1_Text_Close, 3
	dynmultistack 0, 0, FALSE, 4, FALSE, 0, NULL
	switch VAR_RESULT
	case 0, DemoTown_Library_Bookshelf1_1
	case 1, DemoTown_Library_Bookshelf1_2
	case 2, DemoTown_Library_Bookshelf1_3
	case 3, DemoTown_Library_Bookshelf1_Close
DemoTown_Library_Bookshelf1_Close:
	releaseall
	end

DemoTown_Library_Bookshelf1_1::
	msgbox DemoTown_Library_Bookshelf1_Text_1, MSGBOX_DEFAULT
	goto DemoTown_Library_Bookshelf1
	end

DemoTown_Library_Bookshelf1_Text_1:
	.string "Press the {START_BUTTON} while holding {R_BUTTON} to open\n"
	.string "the Expansion's overworld debug menu!$"

DemoTown_Library_Bookshelf1_2::
	msgbox DemoTown_Library_Bookshelf1_Text_2, MSGBOX_DEFAULT
	goto DemoTown_Library_Bookshelf1
	end

DemoTown_Library_Bookshelf1_Text_2:
	.string "Pressing {SELECT_BUTTON} in a Pokémon's summary will\n"
	.string "open the Pokémon Sprite Visualizer.\p"
	.string "You can test your sprite and\n"
	.string "animation changes there.$"

DemoTown_Library_Bookshelf1_3::
	msgbox DemoTown_Library_Bookshelf1_Text_3, MSGBOX_DEFAULT
	goto DemoTown_Library_Bookshelf1
	end

DemoTown_Library_Bookshelf1_Text_3:
	.string "TEST 3.$"
