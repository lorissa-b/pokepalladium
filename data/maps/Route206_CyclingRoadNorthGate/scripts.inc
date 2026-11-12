@ Cycling Road's north gate, between Eterna City and Route 206. South of
@ the counters (row 8) the player must be on the BICYCLE, which sets
@ FLAG_SYS_CYCLING_ROAD (no getting off) until they come back past row 6.

Route206_CyclingRoadNorthGate_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, Route206_CyclingRoadNorthGate_OnTransition
	.byte 0

Route206_CyclingRoadNorthGate_OnTransition:
	call_if_set FLAG_RECEIVED_EXP_SHARE, Route206_CyclingRoadNorthGate_EventScript_HideScientist
	end

Route206_CyclingRoadNorthGate_EventScript_HideScientist::
	setflag FLAG_HIDE_ROUTE_206_CYCLING_ROAD_NORTH_GATE_SCIENTIST
	return

Route206_CyclingRoadNorthGate_EventScript_Clerk::
	msgbox Route206_CyclingRoadNorthGate_Text_LearnHowToShiftGears, MSGBOX_NPC
	end

Route206_CyclingRoadNorthGate_EventScript_BikeCheck::
	lockall
	specialvar VAR_RESULT, GetPlayerAvatarBike
	goto_if_eq VAR_RESULT, 0, Route206_CyclingRoadNorthGate_EventScript_NoBike
	setflag FLAG_SYS_CYCLING_ROAD
	setvar VAR_TEMP_1, 1
	releaseall
	end

Route206_CyclingRoadNorthGate_EventScript_NoBike::
	applymovement LOCALID_ROUTE206_NORTH_GATE_CLERK_WEST, Common_Movement_ExclamationMark
	waitmovement 0
	msgbox Route206_CyclingRoadNorthGate_Text_OnlyForBicycles, MSGBOX_DEFAULT
	closemessage
	applymovement LOCALID_PLAYER, Route206_CyclingRoadNorthGate_Movement_PlayerWalkNorth
	waitmovement 0
	releaseall
	end

Route206_CyclingRoadNorthGate_Movement_PlayerWalkNorth:
	walk_up
	step_end

Route206_CyclingRoadNorthGate_EventScript_ClearCyclingRoad::
	lockall
	clearflag FLAG_SYS_CYCLING_ROAD
	setvar VAR_TEMP_1, 0
	releaseall
	end

Route206_CyclingRoadNorthGate_EventScript_Scientist::
	lock
	faceplayer
	goto_if_set FLAG_RECEIVED_EXP_SHARE, Route206_CyclingRoadNorthGate_EventScript_ExplainExpShare
	msgbox Route206_CyclingRoadNorthGate_Text_MetHowManyPokemon, MSGBOX_DEFAULT
	setvar VAR_0x8004, 1
	specialvar VAR_RESULT, ScriptGetPokedexInfo
	buffernumberstring STR_VAR_1, VAR_0x8005
	goto_if_lt VAR_0x8005, 35, Route206_CyclingRoadNorthGate_EventScript_FindAtLeast35
	msgbox Route206_CyclingRoadNorthGate_Text_SomethingForYou, MSGBOX_DEFAULT
	giveitem ITEM_EXP_SHARE
	goto_if_eq VAR_RESULT, FALSE, Common_EventScript_ShowBagIsFull
	setflag FLAG_RECEIVED_EXP_SHARE
	release
	end

Route206_CyclingRoadNorthGate_EventScript_ExplainExpShare::
	msgbox Route206_CyclingRoadNorthGate_Text_HoldingExpShare, MSGBOX_DEFAULT
	release
	end

Route206_CyclingRoadNorthGate_EventScript_FindAtLeast35::
	msgbox Route206_CyclingRoadNorthGate_Text_FindAtLeast35, MSGBOX_DEFAULT
	release
	end

Route206_CyclingRoadNorthGate_Text_OnlyForBicycles:
	.string "CYCLING ROAD is only for BICYCLES!\p"
	.string "Please get on your BICYCLE!$"

Route206_CyclingRoadNorthGate_Text_LearnHowToShiftGears:
	.string "Learn how to shift gears, and you'll\n"
	.string "be able to ride anywhere.$"

Route206_CyclingRoadNorthGate_Text_MetHowManyPokemon:
	.string "{PLAYER}, long time, no see!\p"
	.string "Oh? You look puzzled.\n"
	.string "You're wondering who I am.\p"
	.string "I'm PROF. BIRCH's assistant and\n"
	.string "{RIVAL}'s father.\p"
	.string "I've come a long way to help you on\n"
	.string "your adventure at the PROFESSOR's\l"
	.string "behest.\p"
	.string "{PLAYER}, let me ask you,\n"
	.string "how many POKéMON have you met?$"

Route206_CyclingRoadNorthGate_Text_SomethingForYou:
	.string "Oh, my!\n"
	.string "You've met {STR_VAR_1} POKéMON!\p"
	.string "Wonderful! Yes, bravo!\n"
	.string "PROF. BIRCH should be delighted!\p"
	.string "Here is something for you!$"

Route206_CyclingRoadNorthGate_Text_HoldingExpShare:
	.string "A POKéMON holding an EXP. SHARE\n"
	.string "will get some of the EXP. POINTS\l"
	.string "from battle.\p"
	.string "It will get EXP. POINTS even if it\n"
	.string "didn't actually battle.\p"
	.string "Therefore, it should be held by a\n"
	.string "POKéMON you want to raise quickly.\p"
	.string "Also, a POKéMON you get in a trade may\n"
	.string "disobey you if it is too high in level.\p"
	.string "Don't worry, though. It will obey if\n"
	.string "you have enough GYM BADGES.$"

Route206_CyclingRoadNorthGate_Text_FindAtLeast35:
	.string "You've met only {STR_VAR_1} POKéMON?\p"
	.string "Hmm…\n"
	.string "You've got to find at least 35.\p"
	.string "If you can't, it will be tough to\n"
	.string "complete the SINNOH POKéDEX.$"
