@ Route 202, between Sandgem Town and Jubilife City. Drawn over Emerald's
@ Route 103.
Route202_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, Route202_OnTransition
	.byte 0

Route202_OnTransition:
	call Common_EventScript_SetupRivalGfxId
	call Common_EventScript_SetupAssistantGfxId
	call ProfBirch_EventScript_UpdateLocation
	call_if_eq VAR_ROUTE201_STATE, 6, Route202_EventScript_ShowAssistant
	end

@ After the POKéDEX, Prof. Birch's assistant waits in the first patch of grass
Route202_EventScript_ShowAssistant::
	clearflag FLAG_HIDE_ROUTE_202_ASSISTANT
	return

@ Stepping onto the route from Sandgem (x 26-29, y 23). The assistant walks
@ over from the grass at 21, 21, catches a Shinx to show how it's done, hands
@ over five POKé BALLS and heads back toward Sandgem and the lab.

Route202_EventScript_CatchTutorial26::
	lockall
	setvar VAR_0x8008, 26
	goto Route202_EventScript_CatchTutorial
	end

Route202_EventScript_CatchTutorial27::
	lockall
	setvar VAR_0x8008, 27
	goto Route202_EventScript_CatchTutorial
	end

Route202_EventScript_CatchTutorial28::
	lockall
	setvar VAR_0x8008, 28
	goto Route202_EventScript_CatchTutorial
	end

Route202_EventScript_CatchTutorial29::
	lockall
	setvar VAR_0x8008, 29
	goto Route202_EventScript_CatchTutorial
	end

Route202_EventScript_CatchTutorial::
	playse SE_PIN
	applywaitmovement LOCALID_ROUTE202_ASSISTANT, Common_Movement_ExclamationMark
	applywaitmovement LOCALID_ROUTE202_ASSISTANT, Common_Movement_Delay48
	msgbox Route202_Text_AssistantOverHere, MSGBOX_DEFAULT
	closemessage
	call_if_eq VAR_0x8008, 26, Route202_EventScript_AssistantApproach26
	call_if_eq VAR_0x8008, 27, Route202_EventScript_AssistantApproach27
	call_if_eq VAR_0x8008, 28, Route202_EventScript_AssistantApproach28
	call_if_eq VAR_0x8008, 29, Route202_EventScript_AssistantApproach29
	msgbox Route202_Text_AssistantWatchMeCatch, MSGBOX_DEFAULT
	closemessage
	special SavePlayerParty
	special LoadAssistantTutorialMon
	special StartAssistantCatchTutorialBattle
	special LoadPlayerParty
	msgbox Route202_Text_AssistantThatsHowYouDoIt, MSGBOX_DEFAULT
	giveitem ITEM_POKE_BALL, 5
	call_if_eq VAR_RESULT, FALSE, Route202_EventScript_BagFullForPokeBalls
	msgbox Route202_Text_AssistantHeadingBack, MSGBOX_DEFAULT
	closemessage
	call_if_eq VAR_0x8008, 26, Route202_EventScript_AssistantLeave26
	call_if_eq VAR_0x8008, 27, Route202_EventScript_AssistantLeave27
	call_if_eq VAR_0x8008, 28, Route202_EventScript_AssistantLeave28
	call_if_eq VAR_0x8008, 29, Route202_EventScript_AssistantLeave29
	removeobject LOCALID_ROUTE202_ASSISTANT
	@ She's back at the lab from now on
	clearflag FLAG_HIDE_BIRCHS_LAB_ASSISTANT
	setvar VAR_ROUTE201_STATE, 7
	releaseall
	end

Route202_EventScript_BagFullForPokeBalls::
	msgbox gText_TooBadBagIsFull, MSGBOX_DEFAULT
	return

Route202_EventScript_AssistantApproach26::
	applywaitmovement LOCALID_ROUTE202_ASSISTANT, Route202_Movement_AssistantApproach26
	return

Route202_EventScript_AssistantLeave26::
	applymovement LOCALID_ROUTE202_ASSISTANT, Route202_Movement_AssistantLeave26
	applymovement LOCALID_PLAYER, Route202_Movement_PlayerWatchAssistantLeave
	waitmovement LOCALID_ROUTE202_ASSISTANT
	waitmovement LOCALID_PLAYER
	return

Route202_EventScript_AssistantApproach27::
	applywaitmovement LOCALID_ROUTE202_ASSISTANT, Route202_Movement_AssistantApproach27
	return

Route202_EventScript_AssistantLeave27::
	applymovement LOCALID_ROUTE202_ASSISTANT, Route202_Movement_AssistantLeave27
	applymovement LOCALID_PLAYER, Route202_Movement_PlayerWatchAssistantLeave
	waitmovement LOCALID_ROUTE202_ASSISTANT
	waitmovement LOCALID_PLAYER
	return

Route202_EventScript_AssistantApproach28::
	applywaitmovement LOCALID_ROUTE202_ASSISTANT, Route202_Movement_AssistantApproach28
	return

Route202_EventScript_AssistantLeave28::
	applymovement LOCALID_ROUTE202_ASSISTANT, Route202_Movement_AssistantLeave28
	applymovement LOCALID_PLAYER, Route202_Movement_PlayerWatchAssistantLeave
	waitmovement LOCALID_ROUTE202_ASSISTANT
	waitmovement LOCALID_PLAYER
	return

Route202_EventScript_AssistantApproach29::
	applywaitmovement LOCALID_ROUTE202_ASSISTANT, Route202_Movement_AssistantApproach29
	return

Route202_EventScript_AssistantLeave29::
	applymovement LOCALID_ROUTE202_ASSISTANT, Route202_Movement_AssistantLeave29
	applymovement LOCALID_PLAYER, Route202_Movement_PlayerWatchAssistantLeave
	waitmovement LOCALID_ROUTE202_ASSISTANT
	waitmovement LOCALID_PLAYER
	return

Route202_Movement_AssistantApproach26:
	walk_right
	walk_right
	walk_right
	walk_right
	walk_right
	walk_down
	walk_in_place_faster_down
	step_end

Route202_Movement_AssistantApproach27:
	walk_right
	walk_right
	walk_right
	walk_right
	walk_right
	walk_right
	walk_down
	walk_in_place_faster_down
	step_end

Route202_Movement_AssistantApproach28:
	walk_right
	walk_right
	walk_right
	walk_right
	walk_right
	walk_right
	walk_right
	walk_down
	walk_in_place_faster_down
	step_end

Route202_Movement_AssistantApproach29:
	walk_right
	walk_right
	walk_right
	walk_right
	walk_right
	walk_right
	walk_right
	walk_right
	walk_down
	walk_in_place_faster_down
	step_end

Route202_Movement_AssistantLeave26:
	walk_right
	walk_down
	walk_down
	walk_down
	walk_down
	step_end

Route202_Movement_AssistantLeave27:
	walk_right
	walk_down
	walk_down
	walk_down
	walk_down
	step_end

Route202_Movement_AssistantLeave28:
	walk_left
	walk_down
	walk_down
	walk_down
	walk_down
	step_end

Route202_Movement_AssistantLeave29:
	walk_left
	walk_down
	walk_down
	walk_down
	walk_down
	step_end

Route202_Movement_PlayerWatchAssistantLeave:
	delay_16
	walk_in_place_faster_down
	step_end

Route202_EventScript_Natalie::
	trainerbattle_single TRAINER_DAISY, Route202_Text_NatalieIntro, Route202_Text_NatalieDefeated
	msgbox Route202_Text_NataliePostBattle, MSGBOX_AUTOCLOSE
	end

Route202_EventScript_Logan::
	trainerbattle_single TRAINER_RICK, Route202_Text_LoganIntro, Route202_Text_LoganDefeated
	msgbox Route202_Text_LoganPostBattle, MSGBOX_AUTOCLOSE
	end

Route202_EventScript_Tristan::
	trainerbattle_single TRAINER_CALVIN_1, Route202_Text_TristanIntro, Route202_Text_TristanDefeated, Route202_EventScript_TristanRegisterMatchCallAfterBattle
	specialvar VAR_RESULT, ShouldTryRematchBattle
	goto_if_eq VAR_RESULT, TRUE, Route202_EventScript_TristanRematch
	setvar VAR_0x8004, TRAINER_CALVIN_1
	specialvar VAR_RESULT, IsTrainerRegistered
	goto_if_eq VAR_RESULT, FALSE, Route202_EventScript_TristanTryRegister
	msgbox Route202_Text_TristanPostBattle, MSGBOX_DEFAULT
	release
	end

Route202_EventScript_TristanRegisterMatchCallAfterBattle::
	special PlayerFaceTrainerAfterBattle
	waitmovement 0
	goto_if_set FLAG_HAS_MATCH_CALL, Route202_EventScript_TristanRegisterMatchCall
	@ No POKéNAV yet: register him quietly so he's already in Match Call once
	@ the player gets one
	setvar VAR_0x8004, TRAINER_CALVIN_1
	special SetMatchCallRegisteredFlag
	release
	end

Route202_EventScript_TristanRegisterMatchCall::
	msgbox Route202_Text_TristanRegisterShort, MSGBOX_DEFAULT
	register_matchcall TRAINER_CALVIN_1
	release
	end

Route202_EventScript_TristanTryRegister::
	goto_if_set FLAG_HAS_MATCH_CALL, Route202_EventScript_TristanRegister
	msgbox Route202_Text_TristanPostBattle, MSGBOX_DEFAULT
	release
	end

Route202_EventScript_TristanRegister::
	msgbox Route202_Text_TristanRegister, MSGBOX_DEFAULT
	register_matchcall TRAINER_CALVIN_1
	release
	end

Route202_EventScript_TristanRematch::
	trainerbattle_rematch TRAINER_CALVIN_1, Route202_Text_TristanRematchIntro, Route202_Text_TristanRematchDefeated
	msgbox Route202_Text_TristanRematchPostBattle, MSGBOX_AUTOCLOSE
	end

Route202_EventScript_TrainerTips::
	msgbox Route202_Text_TrainerTips, MSGBOX_SIGN
	end

Route202_EventScript_Boy::
	msgbox Route202_Text_ShouldHaveBroughtPotion, MSGBOX_NPC
	end

Route202_EventScript_Man::
	msgbox Route202_Text_ShortcutToSandgem, MSGBOX_NPC
	end

Route202_EventScript_RouteSign::
	msgbox Route202_Text_RouteSign, MSGBOX_SIGN
	end

Route202_EventScript_Amy::
	trainerbattle_double TRAINER_AMY_AND_LIV_1, Route202_Text_AmyIntro, Route202_Text_AmyDefeated, Route202_Text_AmyNotEnoughPokemon, Route219_EventScript_AmyRegisterMatchCallAfterBattle
	specialvar VAR_RESULT, ShouldTryRematchBattle
	goto_if_eq VAR_RESULT, TRUE, Route219_EventScript_AmyRematch
	msgbox Route202_Text_AmyPostBattle, MSGBOX_AUTOCLOSE
	end

Route219_EventScript_AmyRegisterMatchCallAfterBattle::
	msgbox Route202_Text_AmyLivRegister, MSGBOX_DEFAULT
	register_matchcall TRAINER_AMY_AND_LIV_1
	release
	end

Route219_EventScript_AmyRematch::
	trainerbattle_rematch_double TRAINER_AMY_AND_LIV_1, Route202_Text_AmyRematchIntro, Route202_Text_AmyRematchDefeated, Route202_Text_AmyRematchNotEnoughPokemon
	msgbox Route202_Text_AmyRematchPostBattle, MSGBOX_AUTOCLOSE
	end

Route202_EventScript_Liv::
	trainerbattle_double TRAINER_AMY_AND_LIV_1, Route202_Text_LivIntro, Route202_Text_LivDefeated, Route202_Text_LivNotEnoughPokemon, Route219_EventScript_LivRegisterMatchCallAfterBattle
	specialvar VAR_RESULT, ShouldTryRematchBattle
	goto_if_eq VAR_RESULT, TRUE, Route219_EventScript_LivRematch
	msgbox Route202_Text_LivPostBattle, MSGBOX_AUTOCLOSE
	end

Route219_EventScript_LivRegisterMatchCallAfterBattle::
	msgbox Route202_Text_AmyLivRegister, MSGBOX_DEFAULT
	register_matchcall TRAINER_AMY_AND_LIV_1
	release
	end

Route219_EventScript_LivRematch::
	trainerbattle_rematch_double TRAINER_AMY_AND_LIV_1, Route202_Text_LivRematchIntro, Route202_Text_LivRematchDefeated, Route202_Text_LivRematchNotEnoughPokemon
	msgbox Route202_Text_LivRematchPostBattle, MSGBOX_AUTOCLOSE
	end

Route202_EventScript_Andrew::
	trainerbattle_single TRAINER_ANDREW, Route202_Text_AndrewIntro, Route202_Text_AndrewDefeated
	msgbox Route202_Text_AndrewPostBattle, MSGBOX_AUTOCLOSE
	end

Route202_EventScript_Miguel::
	trainerbattle_single TRAINER_MIGUEL_1, Route202_Text_MiguelIntro, Route202_Text_MiguelDefeated, Route219_EventScript_MiguelRegisterMatchCallAfterBattle
	specialvar VAR_RESULT, ShouldTryRematchBattle
	goto_if_eq VAR_RESULT, TRUE, Route202_EventScript_MiguelRematch
	msgbox Route202_Text_MiguelPostBattle, MSGBOX_DEFAULT
	release
	end

Route219_EventScript_MiguelRegisterMatchCallAfterBattle::
	special PlayerFaceTrainerAfterBattle
	waitmovement 0
	msgbox Route202_Text_MiguelRegister, MSGBOX_DEFAULT
	register_matchcall TRAINER_MIGUEL_1
	release
	end

Route202_EventScript_MiguelRematch::
	trainerbattle_rematch TRAINER_MIGUEL_1, Route202_Text_MiguelRematchIntro, Route202_Text_MiguelRematchDefeated
	msgbox Route202_Text_MiguelRematchPostBattle, MSGBOX_AUTOCLOSE
	end

Route202_EventScript_Marcos::
	trainerbattle_single TRAINER_MARCOS, Route202_Text_MarcosIntro, Route202_Text_MarcosDefeated
	msgbox Route202_Text_MarcosPostBattle, MSGBOX_AUTOCLOSE
	end

Route202_EventScript_Rhett::
	trainerbattle_single TRAINER_RHETT, Route202_Text_RhettIntro, Route202_Text_RhettDefeated
	msgbox Route202_Text_RhettPostBattle, MSGBOX_AUTOCLOSE
	end

Route202_EventScript_Pete::
	trainerbattle_single TRAINER_PETE, Route202_Text_PeteIntro, Route202_Text_PeteDefeated
	msgbox Route202_Text_PetePostBattle, MSGBOX_AUTOCLOSE
	end

Route202_EventScript_Isabelle::
	trainerbattle_single TRAINER_ISABELLE, Route202_Text_IsabelleIntro, Route202_Text_IsabelleDefeated
	msgbox Route202_Text_IsabellePostBattle, MSGBOX_AUTOCLOSE
	end

Route202_Text_MayRoute202Pokemon:
	.string "MAY: Let's see… The POKéMON found\n"
	.string "on ROUTE 103 include…$"

Route202_Text_MayLetsBattle:
	.string "Oh, hi, {PLAYER}{KUN}!\p"
	.string "…Oh, I see, my dad gave you\n"
	.string "a POKéMON as a gift.\p"
	.string "Since we're here, let's have a quick\n"
	.string "battle!\p"
	.string "I'll give you a taste of what being\n"
	.string "a TRAINER is like.$"

Route202_Text_MayDefeated:
	.string "Wow! That's great!\n"
	.string "{PLAYER}{KUN}, you're pretty good!$"

Route202_Text_MayTimeToHeadBack:
	.string "MAY: I think I know why my dad has\n"
	.string "an eye out for you now.\p"
	.string "I mean, you just got that POKéMON,\n"
	.string "but it already likes you.\p"
	.string "You might be able to befriend any\n"
	.string "kind of POKéMON easily.\p"
	.string "Well, it's time to head back to\n"
	.string "the LAB.$"

Route202_Text_BrendanRoute202Pokemon:
	.string "BRENDAN: Okay, so it's this one and\n"
	.string "that one that live on ROUTE 103…$"

Route202_Text_BrendanLetsBattle:
	.string "Hey, it's {PLAYER}!\p"
	.string "…Oh, yeah, Dad gave you a POKéMON.\p"
	.string "Since we're here, how about a little\n"
	.string "battle?\p"
	.string "I'll teach you what being a TRAINER's\n"
	.string "about!$"

Route202_Text_BrendanDefeated:
	.string "Huh, {PLAYER}, you're not too shabby.$"

Route202_Text_BrendanTimeToHeadBack:
	.string "BRENDAN: I think I get it.\n"
	.string "I think I know why my dad has his eye\l"
	.string "out for you now.\p"
	.string "Look, your POKéMON already likes you,\n"
	.string "even though you just got it.\p"
	.string "{PLAYER}, I get the feeling that you\n"
	.string "could befriend any POKéMON with ease.\p"
	.string "We should head back to the LAB.$"

Route202_Text_ShouldHaveBroughtPotion:
	.string "My POKéMON is staggeringly tired…\n"
	.string "I should have brought a POTION…$"

Route202_Text_ShortcutToSandgem:
	.string "If you cross the sea from here,\n"
	.string "it'll be a shortcut to SANDGEM TOWN.\p"
	.string "Fufufu, that's useful, isn't it?$"

Route202_Text_RouteSign:
	.string "ROUTE 202\n"
	.string "{UP_ARROW} JUBILIFE CITY  {DOWN_ARROW} SANDGEM TOWN$"

Route202_Text_AssistantOverHere:
	.string "ASSISTANT: Oh, {PLAYER}!\n"
	.string "Over here!$"

Route202_Text_AssistantWatchMeCatch:
	.string "The PROFESSOR asked me to show you\n"
	.string "how to catch POKéMON.\p"
	.string "Watch closely, okay?$"

Route202_Text_AssistantThatsHowYouDoIt:
	.string "ASSISTANT: See? Weaken it first,\n"
	.string "then throw a POKé BALL!\p"
	.string "Now it's your turn.\n"
	.string "Here, take these!$"

Route202_Text_AssistantHeadingBack:
	.string "ASSISTANT: Good luck, {PLAYER}!\n"
	.string "I'm heading back to the LAB now.$"

Route202_Text_TrainerTips:
	.string "TRAINER TIPS\p"
	.string "Weaken a wild POKéMON before you\n"
	.string "throw a POKé BALL at it.\p"
	.string "It's easier to catch a POKéMON\n"
	.string "that's low on HP!$"

Route202_Text_NatalieIntro:
	.string "My BIDOOF are twins!\n"
	.string "Can you handle both of them?$"

Route202_Text_NatalieDefeated:
	.string "Oh no!\n"
	.string "My poor BIDOOF…$"

Route202_Text_NataliePostBattle:
	.string "Two of the same POKéMON wasn't\n"
	.string "enough. I'll catch different ones!$"

Route202_Text_LoganIntro:
	.string "My SHINX and I just caught each\n"
	.string "other's eye. Let's battle!$"

Route202_Text_LoganDefeated:
	.string "Aww… My SHINX\n"
	.string "gave it everything!$"

Route202_Text_LoganPostBattle:
	.string "The tall grass around here is full of\n"
	.string "POKéMON. I'm going to catch more!$"
