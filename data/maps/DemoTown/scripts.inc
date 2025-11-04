DemoTown_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, DemoTown_OnTransition
	.byte 0

DemoTown_OnTransition:
	setrespawn HEAL_LOCATION_DEMO_TOWN
	end

DemoTown_Sign_GoToTheLibrary::
	msgbox DemoTown_Text_Sign_GoToTheLibrary, MSGBOX_SIGN
	end

DemoTown_Text_Sign_GoToTheLibrary:
	.string "Expansion Tips!\p"
	.string "Go to the library to learn information\n"
	.string "about pokeemerald-expansion!$"

DemoTown_Sign_Library::
	msgbox DemoTown_Text_Sign_Library, MSGBOX_SIGN
	end

DemoTown_Text_Sign_Library:
	.string "Library$"
