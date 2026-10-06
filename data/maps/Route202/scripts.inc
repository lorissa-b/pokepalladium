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
@ over from the grass at 21, 21. As in Platinum, the player has to have gone
@ home to see Mom after getting the POKéDEX (VAR_TWINLEAF_TOWN_STATE 4, the
@ Town Map and Running Shoes scene); otherwise the assistant sends them back.
@ Once they have, the assistant catches a Shinx to show how it's done, hands
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
	goto_if_lt VAR_TWINLEAF_TOWN_STATE, 4, Route202_EventScript_TellYourFamily
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

Route202_EventScript_TellYourFamily::
	goto_if_set FLAG_TALKED_TO_ROUTE_202_ASSISTANT, Route202_EventScript_GoTellYourFamily
	msgbox Route202_Text_AssistantDidYouTellYourFamily, MSGBOX_DEFAULT
	goto Route202_EventScript_SendPlayerBack
	end

Route202_EventScript_GoTellYourFamily::
	msgbox Route202_Text_AssistantGoTellYourFamily, MSGBOX_DEFAULT
	goto Route202_EventScript_SendPlayerBack
	end

@ The assistant steps down onto the trigger, nudging the player back to y 24,
@ then returns to the grass at 21, 21
Route202_EventScript_SendPlayerBack::
	closemessage
	applymovement LOCALID_ROUTE202_ASSISTANT, Route202_Movement_AssistantNudgePlayer
	applymovement LOCALID_PLAYER, Route202_Movement_PlayerPushedBack
	waitmovement 0
	call_if_eq VAR_0x8008, 26, Route202_EventScript_AssistantReturn26
	call_if_eq VAR_0x8008, 27, Route202_EventScript_AssistantReturn27
	call_if_eq VAR_0x8008, 28, Route202_EventScript_AssistantReturn28
	call_if_eq VAR_0x8008, 29, Route202_EventScript_AssistantReturn29
	setflag FLAG_TALKED_TO_ROUTE_202_ASSISTANT
	releaseall
	end

Route202_EventScript_AssistantReturn26::
	applywaitmovement LOCALID_ROUTE202_ASSISTANT, Route202_Movement_AssistantReturn26
	return

Route202_EventScript_AssistantReturn27::
	applywaitmovement LOCALID_ROUTE202_ASSISTANT, Route202_Movement_AssistantReturn27
	return

Route202_EventScript_AssistantReturn28::
	applywaitmovement LOCALID_ROUTE202_ASSISTANT, Route202_Movement_AssistantReturn28
	return

Route202_EventScript_AssistantReturn29::
	applywaitmovement LOCALID_ROUTE202_ASSISTANT, Route202_Movement_AssistantReturn29
	return

Route202_Movement_AssistantNudgePlayer:
	walk_down
	step_end

Route202_Movement_PlayerPushedBack:
	lock_facing_direction
	walk_down
	unlock_facing_direction
	step_end

Route202_Movement_AssistantReturn26:
	walk_up
	walk_up
	walk_left
	walk_left
	walk_left
	walk_left
	walk_left
	face_right
	step_end

Route202_Movement_AssistantReturn27:
	walk_up
	walk_up
	walk_left
	walk_left
	walk_left
	walk_left
	walk_left
	walk_left
	face_right
	step_end

Route202_Movement_AssistantReturn28:
	walk_up
	walk_up
	walk_left
	walk_left
	walk_left
	walk_left
	walk_left
	walk_left
	walk_left
	face_right
	step_end

Route202_Movement_AssistantReturn29:
	walk_up
	walk_up
	walk_left
	walk_left
	walk_left
	walk_left
	walk_left
	walk_left
	walk_left
	walk_left
	face_right
	step_end

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
	@ No POKéTCH yet: register him quietly so he's already in Match Call once
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

Route202_EventScript_RouteSignSandgem::
	msgbox Route202_Text_RouteSignSandgem, MSGBOX_SIGN
	end

Route202_EventScript_RouteSignJubilife::
	msgbox Route202_Text_RouteSignJubilife, MSGBOX_SIGN
	end

Route202_EventScript_Youngster::
	msgbox Route202_Text_SomePokemonOnlyInTheMorning, MSGBOX_NPC
	end

Route202_Text_RouteSignSandgem:
	.string "ROUTE 202\n"
	.string "{DOWN_ARROW} SANDGEM TOWN$"

Route202_Text_RouteSignJubilife:
	.string "ROUTE 202\n"
	.string "{UP_ARROW} JUBILIFE CITY$"

Route202_Text_SomePokemonOnlyInTheMorning:
	.string "Some POKéMON only appear in the\n"
	.string "morning.\p"
	.string "Some only come out at night.\p"
	.string "I'll keep looking till I've seen them\n"
	.string "all!$"

Route202_Text_AssistantOverHere:
	.string "ASSISTANT: Oh, {PLAYER}!\n"
	.string "Over here!$"

Route202_Text_AssistantDidYouTellYourFamily:
	.string "ASSISTANT: {PLAYER}?!\p"
	.string "Did you tell your family that you're\n"
	.string "helping the PROFESSOR?\p"
	.string "…You didn't, did you?\p"
	.string "I know that you're really eager to\n"
	.string "see all kinds of POKéMON.\p"
	.string "But you shouldn't make your mom\n"
	.string "worry that you've disappeared.$"

Route202_Text_AssistantGoTellYourFamily:
	.string "ASSISTANT: You should go tell your\n"
	.string "family that you're helping the\l"
	.string "PROFESSOR.\p"
	.string "The faster the better, too.$"

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
	.string "All POKéMON that are involved in\n"
	.string "a battle earn EXP. POINTS!\p"
	.string "The more your POKéMON battle,\n"
	.string "the stronger they will become.$"

Route202_Text_NatalieIntro:
	.string "I spy, with my little eye…\n"
	.string "A TRAINER!\p"
	.string "A battle, please!$"

Route202_Text_NatalieDefeated:
	.string "Oh, boo!\n"
	.string "I can't win!$"

Route202_Text_NataliePostBattle:
	.string "My POKéMON are all tuckered out!\n"
	.string "I need to go to a POKéMON CENTER!$"

Route202_Text_LoganIntro:
	.string "Let's have a POKéMON battle!\n"
	.string "Humph! But I'm gonna win!$"

Route202_Text_LoganDefeated:
	.string "You beat me?!\n"
	.string "You've got talent!$"

Route202_Text_LoganPostBattle:
	.string "It was fun battling!\n"
	.string "Yup! I'm going to battle everyone!$"
