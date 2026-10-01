@ Route 219, the beach and sea south of Sandgem Town. Drawn over Emerald's
@ Route 102.
Route219_MapScripts::
	.byte 0

Route219_EventScript_LittleBoy::
	msgbox Route219_Text_ImNotVeryTall, MSGBOX_NPC
	end

Route219_EventScript_RouteSignSandgem::
	msgbox Route219_Text_RouteSignSandgem, MSGBOX_SIGN
	end

Route219_EventScript_RouteSignPetalburg::
	msgbox Route219_Text_RouteSignPetalburg, MSGBOX_SIGN
	end

Route219_EventScript_Boy::
	msgbox Route219_Text_CatchWholeBunchOfPokemon, MSGBOX_NPC
	end

Route219_EventScript_Tiana::
	trainerbattle_single TRAINER_TIANA, Route219_Text_TianaIntro, Route219_Text_TianaDefeated
	msgbox Route219_Text_TianaPostBattle, MSGBOX_AUTOCLOSE
	end

Route219_EventScript_Allen::
	trainerbattle_single TRAINER_ALLEN, Route219_Text_AllenIntro, Route219_Text_AllenDefeated
	msgbox Route219_Text_AllenPostBattle, MSGBOX_AUTOCLOSE
	end

Route219_Text_WatchMeCatchPokemon:
	.string "WALLY: {PLAYER}…\n"
	.string "POKéMON hide in tall grass like this,\l"
	.string "don't they?\p"
	.string "Please watch me and see if I can\n"
	.string "catch one properly.\p"
	.string "…Whoa!$"

Route219_Text_WallyIDidIt:
	.string "WALLY: I did it… It's my…\n"
	.string "My POKéMON!$"

Route219_Text_LetsGoBack:
	.string "{PLAYER}, thank you!\n"
	.string "Let's go back to the GYM!$"

Route219_Text_ImNotVeryTall:
	.string "I'm…not very tall, so I sink right\n"
	.string "into tall grass.\p"
	.string "The grass goes up my nose and…\n"
	.string "Fwafwafwafwafwa…\p"
	.string "Fwatchoo!$"

Route219_Text_CatchWholeBunchOfPokemon:
	.string "I'm going to catch a whole bunch of\n"
	.string "POKéMON!$"

Route219_Text_RouteSignSandgem:
	.string "ROUTE 219\n"
	.string "{UP_ARROW} SANDGEM TOWN$"

Route219_Text_RouteSignPetalburg:
	.string "ROUTE 102\n"
	.string "{LEFT_ARROW} PETALBURG CITY$"
