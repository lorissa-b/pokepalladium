Route201_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, Route201_OnTransition
	.byte 0

	@ State descriptions for VAR_ROUTE201_STATE
	@ 0: Haven't been on Route 201 yet
	@ 1: Rival dared you to go to Lake Verity and is waiting by the north gap
	@ 2: Met Prof. Birch and his assistant; the rival went ahead into Lake Verity
	@ 3: (Lake Verity) Rival pointed out the bag Birch left behind
	@ 4: (Lake Verity) Chose a starter from the bag and beat the wild Starly;
	@    Birch, his assistant and the rival wait by the Twinleaf path
	@ 5: Returned the bag; Birch said to meet him in Sandgem Town
	@ 6: (Sandgem Town) Followed the assistant into the lab and received the POKéDEX
	@ 7: (Route 202) Watched the assistant's catching tutorial and got five POKé BALLS

Route201_OnTransition:
	call ProfBirch_EventScript_UpdateLocation
	call Common_EventScript_SetupRivalGfxId
	call Common_EventScript_SetupAssistantGfxId
	call_if_eq VAR_ROUTE201_STATE, 1, Route201_EventScript_SetRivalWaitingByLake
	call_if_eq VAR_ROUTE201_STATE, 4, Route201_EventScript_SetGroupByTwinleafPath
	end

@ Birch, his assistant and the rival wait at the west end of the path to Twinleaf,
@ where Platinum's Rowan stands
Route201_EventScript_SetGroupByTwinleafPath::
	setobjectxyperm LOCALID_ROUTE201_BIRCH, 28, 12
	setobjectmovementtype LOCALID_ROUTE201_BIRCH, MOVEMENT_TYPE_FACE_LEFT
	setobjectxyperm LOCALID_ROUTE201_ASSISTANT, 29, 12
	setobjectmovementtype LOCALID_ROUTE201_ASSISTANT, MOVEMENT_TYPE_FACE_LEFT
	setobjectxyperm LOCALID_ROUTE201_RIVAL, 28, 11
	setobjectmovementtype LOCALID_ROUTE201_RIVAL, MOVEMENT_TYPE_FACE_LEFT
	return

Route201_EventScript_SetRivalWaitingByLake::
	setobjectxyperm LOCALID_ROUTE201_RIVAL, 6, 4
	setobjectmovementtype LOCALID_ROUTE201_RIVAL, MOVEMENT_TYPE_FACE_LEFT
	return

@ Coming up the path from Twinleaf (x 33-36, y 19). The rival waits on the path at 35, 12.
Route201_EventScript_RivalDare::
	lockall
	applywaitmovement LOCALID_PLAYER, Route201_Movement_PlayerWalkUpPath
	playse SE_PIN
	applywaitmovement LOCALID_ROUTE201_RIVAL, Common_Movement_ExclamationMark
	applywaitmovement LOCALID_ROUTE201_RIVAL, Common_Movement_Delay48
	msgbox Route201_Text_RivalDare, MSGBOX_DEFAULT
	closemessage
	@ The rival runs west off screen, then jumps ahead to wait by the north gap
	applymovement LOCALID_ROUTE201_RIVAL, Route201_Movement_RivalRunTowardLake
	applymovement LOCALID_PLAYER, Route201_Movement_PlayerWatchRivalRunWest
	waitmovement LOCALID_ROUTE201_RIVAL
	waitmovement LOCALID_PLAYER
	setobjectxy LOCALID_ROUTE201_RIVAL, 6, 4
	call Route201_EventScript_SetRivalWaitingByLake
	setvar VAR_ROUTE201_STATE, 1
	releaseall
	end

Route201_EventScript_Rival::
	lock
	faceplayer
	goto_if_eq VAR_ROUTE201_STATE, 4, Route201_EventScript_RivalBagIsTheirs
	msgbox Route201_Text_RivalLakeIsUpThere, MSGBOX_DEFAULT
	applywaitmovement LOCALID_ROUTE201_RIVAL, Common_Movement_FaceLeft
	release
	end

@ Reaching the path into Lake Verity (x 3-4, y 3). Birch and his assistant come out of it.
Route201_EventScript_MeetBirchTrigger0::
	lockall
	setvar VAR_0x8008, 0
	goto Route201_EventScript_MeetBirch
	end

Route201_EventScript_MeetBirchTrigger1::
	lockall
	setvar VAR_0x8008, 1
	goto Route201_EventScript_MeetBirch
	end

Route201_EventScript_MeetBirch::
	playse SE_PIN
	applywaitmovement LOCALID_ROUTE201_RIVAL, Common_Movement_ExclamationMark
	msgbox Route201_Text_RivalSomeonesComing, MSGBOX_DEFAULT
	closemessage
	@ Step out of the way: to 2, 4 or 5, 4
	call_if_eq VAR_0x8008, 0, Route201_EventScript_PlayerStepAsideLeft
	call_if_eq VAR_0x8008, 1, Route201_EventScript_PlayerStepAsideRight
	applywaitmovement LOCALID_ROUTE201_RIVAL, Common_Movement_FaceUp
	addobject LOCALID_ROUTE201_BIRCH
	addobject LOCALID_ROUTE201_ASSISTANT
	applymovement LOCALID_ROUTE201_BIRCH, Route201_Movement_WalkOutOfGap
	applymovement LOCALID_ROUTE201_ASSISTANT, Route201_Movement_WalkOutOfGap
	msgbox Route201_Text_BirchLakeWasCalm, MSGBOX_DEFAULT
	msgbox Route201_Text_AssistantNothingUnusual, MSGBOX_DEFAULT
	waitmovement LOCALID_ROUTE201_BIRCH
	waitmovement LOCALID_ROUTE201_ASSISTANT
	applymovement LOCALID_ROUTE201_BIRCH, Common_Movement_WalkInPlaceFasterRight
	applymovement LOCALID_ROUTE201_ASSISTANT, Common_Movement_WalkInPlaceFasterLeft
	waitmovement 0
	msgbox Route201_Text_BirchPokemonWereRestless, MSGBOX_DEFAULT
	msgbox Route201_Text_AssistantComeBackLater, MSGBOX_DEFAULT
	msgbox Route201_Text_BirchYesLetsDoThat, MSGBOX_DEFAULT
	closemessage
	playse SE_PIN
	applywaitmovement LOCALID_ROUTE201_BIRCH, Common_Movement_ExclamationMark
	applymovement LOCALID_ROUTE201_BIRCH, Common_Movement_WalkInPlaceFasterDown
	applymovement LOCALID_ROUTE201_ASSISTANT, Common_Movement_WalkInPlaceFasterDown
	waitmovement 0
	msgbox Route201_Text_BirchHeadBackHome, MSGBOX_DEFAULT
	msgbox Route201_Text_AssistantTakeCare, MSGBOX_DEFAULT
	closemessage
	@ They head home: south past the lakefront's ledges, then east along y 11
	applymovement LOCALID_ROUTE201_BIRCH, Route201_Movement_BirchLeave
	applymovement LOCALID_ROUTE201_ASSISTANT, Route201_Movement_AssistantLeave
	waitmovement LOCALID_ROUTE201_BIRCH
	waitmovement LOCALID_ROUTE201_ASSISTANT
	removeobject LOCALID_ROUTE201_BIRCH
	removeobject LOCALID_ROUTE201_ASSISTANT
	call_if_eq VAR_0x8008, 0, Route201_EventScript_RivalFacePlayerLeft
	call_if_eq VAR_0x8008, 1, Route201_EventScript_RivalFacePlayerRight
	msgbox Route201_Text_RivalWeGotCaught, MSGBOX_DEFAULT
	closemessage
	applywaitmovement LOCALID_ROUTE201_RIVAL, Route201_Movement_RivalRunToLake
	removeobject LOCALID_ROUTE201_RIVAL
	clearflag FLAG_HIDE_LAKE_VERITY_RIVAL
	setvar VAR_ROUTE201_STATE, 2
	releaseall
	end

Route201_EventScript_PlayerStepAsideLeft::
	applywaitmovement LOCALID_PLAYER, Route201_Movement_PlayerStepAsideLeft
	return

Route201_EventScript_PlayerStepAsideRight::
	applywaitmovement LOCALID_PLAYER, Route201_Movement_PlayerStepAsideRight
	return

Route201_EventScript_RivalFacePlayerLeft::
	applywaitmovement LOCALID_ROUTE201_RIVAL, Common_Movement_FaceLeft
	applywaitmovement LOCALID_PLAYER, Common_Movement_WalkInPlaceFasterRight
	return

@ The rival waits at 6, 4, east of the player either way
Route201_EventScript_RivalFacePlayerRight::
	applywaitmovement LOCALID_ROUTE201_RIVAL, Common_Movement_FaceLeft
	applywaitmovement LOCALID_PLAYER, Common_Movement_WalkInPlaceFasterRight
	return

Route201_EventScript_RivalBagIsTheirs::
	msgbox Route201_Text_RivalGiveThemTheBag, MSGBOX_DEFAULT
	release
	end

@ Coming back from Lake Verity with the bag. The group waits at 28-29, 11-12, and
@ every way east from the lake crosses x 24 (y 8-9 above the ledges, 12-14 below),
@ so the player is always west of them and they can leave east along y 12.
Route201_EventScript_ReturnBag::
	lockall
	playse SE_PIN
	applywaitmovement LOCALID_ROUTE201_ASSISTANT, Common_Movement_ExclamationMark
	applymovement LOCALID_ROUTE201_ASSISTANT, Common_Movement_FacePlayer
	applymovement LOCALID_ROUTE201_BIRCH, Common_Movement_FacePlayer
	applymovement LOCALID_ROUTE201_RIVAL, Common_Movement_FacePlayer
	waitmovement 0
	msgbox Route201_Text_AssistantThatsOurBag, MSGBOX_DEFAULT
	msgbox Route201_Text_HandedOverBag, MSGBOX_DEFAULT
	msgbox Route201_Text_AssistantDidYouUseThem, MSGBOX_DEFAULT
	closemessage
	applywaitmovement LOCALID_ROUTE201_ASSISTANT, Common_Movement_WalkInPlaceFasterLeft
	msgbox Route201_Text_AssistantWhatShouldWeDo, MSGBOX_DEFAULT
	closemessage
	applywaitmovement LOCALID_ROUTE201_BIRCH, Common_Movement_FacePlayer
	msgbox Route201_Text_BirchLetMeAskYou, MSGBOX_DEFAULT
Route201_EventScript_AskLovePokemon::
	msgbox Route201_Text_BirchDoYouLovePokemon, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, YES, Route201_EventScript_LovesPokemon
	msgbox Route201_Text_BirchAskAgain, MSGBOX_DEFAULT
	goto Route201_EventScript_AskLovePokemon
	end

Route201_EventScript_LovesPokemon::
	msgbox Route201_Text_BirchMeetInSandgem, MSGBOX_DEFAULT
	applywaitmovement LOCALID_ROUTE201_ASSISTANT, Common_Movement_FacePlayer
	msgbox Route201_Text_AssistantSeeYouThere, MSGBOX_DEFAULT
	closemessage
	@ Off the right side of the screen, then the rival runs after them
	call Route201_EventScript_GroupLeaveAlongRow12
	removeobject LOCALID_ROUTE201_BIRCH
	removeobject LOCALID_ROUTE201_ASSISTANT
	applywaitmovement LOCALID_ROUTE201_RIVAL, Common_Movement_FacePlayer
	@ Win or lose, the story carries on: no white out, and the party is healed
	setflag FLAG_NO_WHITEOUT_ON_LOSS
	call Route201_EventScript_RivalBattle
	clearflag FLAG_NO_WHITEOUT_ON_LOSS
	specialvar VAR_RESULT, GetBattleOutcome
	call_if_eq VAR_RESULT, B_OUTCOME_LOST, Route201_EventScript_RivalWon
	special HealPlayerParty
	msgbox Route201_Text_RivalWaitForMe, MSGBOX_DEFAULT
	closemessage
	call Route201_EventScript_RivalFollowAlongRow12
	removeobject LOCALID_ROUTE201_RIVAL
	setvar VAR_ROUTE201_STATE, 5
	releaseall
	end

@ The rival's first battle, with the starter that's strong against the player's.
@ The ROUTE_202 trainers are named after the player's starter (see trainer_parties.h).
Route201_EventScript_RivalBattle::
	checkplayergender
	goto_if_eq VAR_RESULT, FEMALE, Route201_EventScript_BrendanBattle
	playbgm MUS_ENCOUNTER_MAY, TRUE
	msgbox Route201_Text_RivalLetsBattle, MSGBOX_DEFAULT
	switch VAR_STARTER_MON
	case 0, Route201_EventScript_MayBattleTurtwig
	case 1, Route201_EventScript_MayBattleChimchar
	case 2, Route201_EventScript_MayBattlePiplup
	return

Route201_EventScript_BrendanBattle::
	playbgm MUS_ENCOUNTER_BRENDAN, TRUE
	msgbox Route201_Text_RivalLetsBattle, MSGBOX_DEFAULT
	switch VAR_STARTER_MON
	case 0, Route201_EventScript_BrendanBattleTurtwig
	case 1, Route201_EventScript_BrendanBattleChimchar
	case 2, Route201_EventScript_BrendanBattlePiplup
	return

Route201_EventScript_RivalWon::
	msgbox Route201_Text_RivalWon, MSGBOX_DEFAULT
	return

Route201_EventScript_MayBattleTurtwig::
	trainerbattle_no_intro TRAINER_MAY_ROUTE_202_TREECKO, Route201_Text_RivalDefeated
	return

Route201_EventScript_MayBattleChimchar::
	trainerbattle_no_intro TRAINER_MAY_ROUTE_202_TORCHIC, Route201_Text_RivalDefeated
	return

Route201_EventScript_MayBattlePiplup::
	trainerbattle_no_intro TRAINER_MAY_ROUTE_202_MUDKIP, Route201_Text_RivalDefeated
	return

Route201_EventScript_BrendanBattleTurtwig::
	trainerbattle_no_intro TRAINER_BRENDAN_ROUTE_202_TREECKO, Route201_Text_RivalDefeated
	return

Route201_EventScript_BrendanBattleChimchar::
	trainerbattle_no_intro TRAINER_BRENDAN_ROUTE_202_TORCHIC, Route201_Text_RivalDefeated
	return

Route201_EventScript_BrendanBattlePiplup::
	trainerbattle_no_intro TRAINER_BRENDAN_ROUTE_202_MUDKIP, Route201_Text_RivalDefeated
	return

Route201_EventScript_GroupLeaveAlongRow12::
	applymovement LOCALID_ROUTE201_BIRCH, Route201_Movement_LeaveRight
	applymovement LOCALID_ROUTE201_ASSISTANT, Route201_Movement_LeaveRight
	waitmovement LOCALID_ROUTE201_BIRCH
	waitmovement LOCALID_ROUTE201_ASSISTANT
	return

Route201_EventScript_RivalFollowAlongRow12::
	applywaitmovement LOCALID_ROUTE201_RIVAL, Route201_Movement_RivalFollowViaRow12
	return

Route201_Movement_LeaveRight:
	walk_right
	walk_right
	walk_right
	walk_right
	walk_right
	walk_right
	walk_right
	walk_right
	walk_right
	walk_right
	walk_right
	walk_right
	walk_right
	step_end

Route201_Movement_RivalFollowViaRow12:
	walk_fast_down
	walk_fast_right
	walk_fast_right
	walk_fast_right
	walk_fast_right
	walk_fast_right
	walk_fast_right
	walk_fast_right
	walk_fast_right
	walk_fast_right
	walk_fast_right
	walk_fast_right
	walk_fast_right
	walk_fast_right
	walk_fast_right
	step_end

Route201_Movement_PlayerWalkUpPath:
	walk_up
	walk_up
	walk_up
	walk_up
	step_end

Route201_Movement_RivalRunTowardLake:
	walk_fast_up
	walk_fast_left
	walk_fast_left
	walk_fast_left
	walk_fast_left
	walk_fast_left
	walk_fast_left
	walk_fast_left
	walk_fast_left
	walk_fast_left
	step_end

Route201_Movement_PlayerWatchRivalRunWest:
	delay_16
	walk_in_place_faster_left
	step_end

Route201_Movement_PlayerStepAsideLeft:
	lock_facing_direction
	walk_down
	unlock_facing_direction
	walk_left
	walk_in_place_faster_right
	step_end

Route201_Movement_PlayerStepAsideRight:
	lock_facing_direction
	walk_down
	unlock_facing_direction
	walk_right
	walk_in_place_faster_left
	step_end

Route201_Movement_WalkOutOfGap:
	walk_down
	walk_down
	walk_down
	walk_down
	step_end

Route201_Movement_BirchLeave:
	walk_down
	walk_down
	walk_down
	walk_down
	walk_down
	walk_down
	walk_down
	walk_right
	walk_right
	walk_right
	walk_right
	walk_right
	walk_right
	walk_right
	walk_right
	walk_right
	step_end

@ Follows one step behind Birch
Route201_Movement_AssistantLeave:
	walk_left
	walk_down
	walk_down
	walk_down
	walk_down
	walk_down
	walk_down
	walk_down
	walk_right
	walk_right
	walk_right
	walk_right
	walk_right
	walk_right
	walk_right
	walk_right
	step_end

@ From 6, 4 round below the player and up the path into Lake Verity
Route201_Movement_RivalRunToLake:
	walk_fast_down
	walk_fast_left
	walk_fast_left
	walk_fast_up
	walk_fast_up
	walk_fast_up
	walk_fast_up
	walk_fast_up
	step_end

Route201_EventScript_Youngster::
	msgbox Route201_Text_TakeTiredPokemonToPokeCenter, MSGBOX_NPC
	end

Route201_EventScript_Boy::
	msgbox Route201_Text_WildPokemonInTallGrass, MSGBOX_NPC
	end

Route201_EventScript_RouteSign::
	msgbox Route201_Text_RouteSign, MSGBOX_SIGN
	end

Route201_EventScript_RouteSignMiddle::
	msgbox Route201_Text_RouteSignMiddle, MSGBOX_SIGN
	end

Route201_EventScript_LakeVeritySign::
	msgbox Route201_Text_LakeVeritySign, MSGBOX_SIGN
	end

Route201_EventScript_TrainerTips::
	msgbox Route201_Text_TrainerTips, MSGBOX_SIGN
	end

Route201_Text_RivalDare:
	.string "{RIVAL}: There you are, {PLAYER}!\n"
	.string "What took you so long?\p"
	.string "Hey, did you see that TV show last\n"
	.string "night? About the red GYARADOS?\p"
	.string "I bet a POKéMON like that lives in\n"
	.string "LAKE VERITY, at the west end of\l"
	.string "this route!\p"
	.string "Let's go find it!\n"
	.string "…Unless you're too scared?\p"
	.string "Ha! I knew you'd say yes!\n"
	.string "I dare you to keep up with me!$"

Route201_Text_RivalLakeIsUpThere:
	.string "{RIVAL}: LAKE VERITY is just up\n"
	.string "this path! Come on, {PLAYER}!$"

Route201_Text_RivalSomeonesComing:
	.string "{RIVAL}: Shh! {PLAYER}!\n"
	.string "Someone's coming out of there!$"

Route201_Text_BirchLakeWasCalm:
	.string "PROF. BIRCH: …So the lake was as calm\n"
	.string "as ever.$"

Route201_Text_AssistantNothingUnusual:
	.string "ASSISTANT: Yes, Professor.\n"
	.string "There was nothing unusual at all.$"

Route201_Text_BirchPokemonWereRestless:
	.string "PROF. BIRCH: Hmm… And yet the POKéMON\n"
	.string "around here were so restless last night.\p"
	.string "Something must have startled them.$"

Route201_Text_AssistantComeBackLater:
	.string "ASSISTANT: Should we come back with\n"
	.string "better equipment?$"

Route201_Text_BirchYesLetsDoThat:
	.string "PROF. BIRCH: Yes, let's do that.\n"
	.string "…Hm?$"

Route201_Text_BirchHeadBackHome:
	.string "PROF. BIRCH: Oh! {PLAYER}{KUN}!\n"
	.string "And {RIVAL}, too!\p"
	.string "Were you two heading to LAKE VERITY?\p"
	.string "The tall grass is full of wild POKéMON.\n"
	.string "It's dangerous without POKéMON of\l"
	.string "your own!\p"
	.string "Head back home for now, all right?$"

Route201_Text_AssistantTakeCare:
	.string "ASSISTANT: Take care, you two!$"

Route201_Text_RivalWeGotCaught:
	.string "{RIVAL}: …Aww. We got caught.\p"
	.string "But they just came back from the lake,\n"
	.string "so it can't be THAT dangerous, right?\p"
	.string "Come on, {PLAYER}! Let's take a peek\n"
	.string "before anyone else shows up!$"

Route201_Text_RivalGiveThemTheBag:
	.string "{RIVAL}: Go on, {PLAYER}!\n"
	.string "Give them back their bag!$"

Route201_Text_AssistantThatsOurBag:
	.string "ASSISTANT: Oh! That's our bag!\n"
	.string "We left it by the lake!$"

Route201_Text_HandedOverBag:
	.string "{PLAYER} handed the bag to the\n"
	.string "ASSISTANT.$"

Route201_Text_AssistantDidYouUseThem:
	.string "ASSISTANT: …Wait a second.\p"
	.string "One of the POKé BALLS is empty.\n"
	.string "Did you use a POKéMON from in here?$"

Route201_Text_AssistantWhatShouldWeDo:
	.string "ASSISTANT: Professor, what should\n"
	.string "we do?$"

Route201_Text_BirchLetMeAskYou:
	.string "PROF. BIRCH: Hmm…\p"
	.string "{PLAYER}{KUN}, let me ask you something.$"

Route201_Text_BirchDoYouLovePokemon:
	.string "PROF. BIRCH: Do you truly love\n"
	.string "POKéMON?$"

Route201_Text_BirchAskAgain:
	.string "PROF. BIRCH: …Is that so?\n"
	.string "Think about it carefully.$"

Route201_Text_BirchMeetInSandgem:
	.string "PROF. BIRCH: …I see.\p"
	.string "And that POKéMON seems quite taken\n"
	.string "with you, too.\p"
	.string "Very well. Come see me at my LAB in\n"
	.string "SANDGEM TOWN. We have much to discuss.$"

Route201_Text_AssistantSeeYouThere:
	.string "ASSISTANT: See you in SANDGEM TOWN!$"

Route201_Text_RivalWaitForMe:
	.string "{RIVAL}: Hey, wait for me!\n"
	.string "I'm coming too!$"

Route201_Text_TakeTiredPokemonToPokeCenter:
	.string "If POKéMON get tired, take them to\n"
	.string "a POKéMON CENTER.\p"
	.string "There's a POKéMON CENTER in SANDGEM\n"
	.string "TOWN right close by.$"

Route201_Text_WildPokemonInTallGrass:
	.string "Wild POKéMON will jump out at you in\n"
	.string "tall grass.\p"
	.string "If you want to catch POKéMON, you have\n"
	.string "to go into the tall grass and search.$"

Route201_Text_RouteSign:
	.string "ROUTE 201\n"
	.string "{DOWN_ARROW} TWINLEAF TOWN$"

Route201_Text_RouteSignMiddle:
	.string "ROUTE 201\n"
	.string "{LEFT_ARROW} LAKE VERITY  {RIGHT_ARROW} SANDGEM TOWN$"

Route201_Text_LakeVeritySign:
	.string "{UP_ARROW} LAKE VERITY$"

Route201_Text_TrainerTips:
	.string "TRAINER TIPS\p"
	.string "Jump down ledges for a shortcut!\n"
	.string "You can't climb back up, though.$"

Route201_Text_RivalLetsBattle:
	.string "{RIVAL}: Hey, {PLAYER}!\n"
	.string "Before I go…\p"
	.string "Our POKéMON are raring to go, right?\n"
	.string "Let's battle!$"

Route201_Text_RivalDefeated:
	.string "Whoa! You're pretty good,\n"
	.string "{PLAYER}!$"

Route201_Text_RivalWon:
	.string "{RIVAL}: Yes! I won!\p"
	.string "Don't worry, {PLAYER}. Our POKéMON\n"
	.string "will both get stronger from here!$"
