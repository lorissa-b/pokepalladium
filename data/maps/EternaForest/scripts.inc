@ Eterna Forest, between Route 205 South and Route 205 North. Drawn over
@ Petalburg Woods from Platinum's map. Route 205 North and the Old Chateau
@ aren't here yet, so the east exit and the chateau door lead back to
@ themselves.
@
@ Cheryl and the eight trainers are Platinum's. The trainers stand in facing
@ pairs that watch the same tiles, so both spot the player at once; with
@ Cheryl following, that's a multi battle beside her.

EternaForest_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, EternaForest_OnTransition
	.byte 0

@ Cheryl waits at the south entrance and goes through the forest with the
@ player as in Platinum: she follows them, battles two trainers at a time
@ beside them and heals their Pokémon after every battle (VAR_FOLLOWER_PARTNER).
@ If the player whited out while she followed, she's back at the entrance.
EternaForest_OnTransition::
	call_if_eq VAR_ETERNA_FOREST_CHERYL_STATE, 1, EternaForest_EventScript_TryResetCheryl
	end

EternaForest_EventScript_TryResetCheryl::
	checkfollower
	goto_if_eq VAR_RESULT, TRUE, EternaForest_EventScript_CherylStillFollowing
	setvar VAR_ETERNA_FOREST_CHERYL_STATE, 3
	setvar VAR_FOLLOWER_PARTNER, 0
	clearflag FLAG_HIDE_ETERNA_FOREST_CHERYL
EternaForest_EventScript_CherylStillFollowing::
	return

@ Stepping north from the entrance (x 19-20, y 80)
EternaForest_EventScript_CherylJoin19::
	lockall
	playse SE_PIN
	applymovement LOCALID_ETERNA_FOREST_CHERYL, EternaForest_Movement_CherylNoticeAndWalkToPlayer19
	waitmovement 0
	goto EternaForest_EventScript_CherylJoin
	end

EternaForest_EventScript_CherylJoin20::
	lockall
	playse SE_PIN
	applymovement LOCALID_ETERNA_FOREST_CHERYL, EternaForest_Movement_CherylNoticeAndWalkToPlayer20
	waitmovement 0
	goto EternaForest_EventScript_CherylJoin
	end

EternaForest_EventScript_Cheryl::
	lockall
	faceplayer
	goto EternaForest_EventScript_CherylJoin
	end

EternaForest_EventScript_CherylJoin::
	call_if_eq VAR_ETERNA_FOREST_CHERYL_STATE, 0, EternaForest_EventScript_CherylMayIGoWithYou
	call_if_eq VAR_ETERNA_FOREST_CHERYL_STATE, 3, EternaForest_EventScript_CherylLetsGetThroughTogether
	playfanfare MUS_OBTAIN_ITEM
	message EternaForest_Text_PlayerDecidedToGoWithCheryl
	waitmessage
	waitfanfare
	msgbox EternaForest_Text_IllKeepPokemonHealthy, MSGBOX_DEFAULT
	closemessage
	setvar VAR_ETERNA_FOREST_CHERYL_STATE, 1
	setvar VAR_FOLLOWER_PARTNER, TRAINER_CHERYL_PARTNER
	loadword 0, EternaForest_EventScript_FollowingCheryl
	setfollower LOCALID_ETERNA_FOREST_CHERYL, FOLLOWER_FLAG_CLEAR_ON_WHITE_OUT | FOLLOWER_FLAG_CUSTOM_FOLLOW_SCRIPT
	@ The follower is respawned on every map load, so the map's own copy stays hidden
	setflag FLAG_HIDE_ETERNA_FOREST_CHERYL
	releaseall
	end

EternaForest_EventScript_CherylMayIGoWithYou::
	msgbox EternaForest_Text_MayIGoWithYou, MSGBOX_DEFAULT
	return

EternaForest_EventScript_CherylLetsGetThroughTogether::
	msgbox EternaForest_Text_LetsGetThroughForestTogether, MSGBOX_DEFAULT
	return

@ Talking to Cheryl while she follows
EternaForest_EventScript_FollowingCheryl::
	lock
	facefollower
	msgbox EternaForest_Text_IllKeepPokemonHealthy, MSGBOX_DEFAULT
	release
	end

@ Stepping back onto the south exit (x 19-20, y 81) with Cheryl behind
EternaForest_EventScript_CherylWait19::
	lockall
	setvar VAR_0x8008, 19
	goto EternaForest_EventScript_CherylWait
	end

EternaForest_EventScript_CherylWait20::
	lockall
	setvar VAR_0x8008, 20
	goto EternaForest_EventScript_CherylWait
	end

EternaForest_EventScript_CherylWait::
	special StopFollowerInPlace
	setvar VAR_FOLLOWER_PARTNER, 0
	applywaitmovement LOCALID_ETERNA_FOREST_CHERYL, Common_Movement_FaceDown
	msgbox EternaForest_Text_IllWaitHereForYouThen, MSGBOX_DEFAULT
	closemessage
	call_if_eq VAR_0x8008, 19, EternaForest_EventScript_CherylWalkBack19
	call_if_eq VAR_0x8008, 20, EternaForest_EventScript_CherylWalkBack20
	setobjectxyperm LOCALID_ETERNA_FOREST_CHERYL, 19, 78
	setobjectmovementtype LOCALID_ETERNA_FOREST_CHERYL, MOVEMENT_TYPE_FACE_DOWN
	clearflag FLAG_HIDE_ETERNA_FOREST_CHERYL
	setvar VAR_ETERNA_FOREST_CHERYL_STATE, 3
	releaseall
	end

EternaForest_EventScript_CherylWalkBack19::
	applywaitmovement LOCALID_ETERNA_FOREST_CHERYL, EternaForest_Movement_CherylWalkBack19
	return

EternaForest_EventScript_CherylWalkBack20::
	applywaitmovement LOCALID_ETERNA_FOREST_CHERYL, EternaForest_Movement_CherylWalkBack20
	return

@ Reaching the east exit (x 73, y 29-34). Cheryl is behind the player, at
@ x 72, and walks out along row 31 or 32 past them.
EternaForest_EventScript_CherylLeave::
	lockall
	special StopFollowerInPlace
	setvar VAR_FOLLOWER_PARTNER, 0
	applymovement LOCALID_ETERNA_FOREST_CHERYL, EternaForest_Movement_CherylExclamationMark
	applymovement LOCALID_PLAYER, EternaForest_Movement_PlayerFaceCheryl
	waitmovement 0
	msgbox EternaForest_Text_ThankYouSoMuch, MSGBOX_DEFAULT
	msgbox EternaForest_Text_TokenOfAppreciation, MSGBOX_DEFAULT
	giveitem ITEM_SOOTHE_BELL
	closemessage
	getplayerxy VAR_0x8004, VAR_0x8005
	call_if_eq VAR_0x8005, 29, EternaForest_EventScript_CherylWalkToExit29
	call_if_eq VAR_0x8005, 30, EternaForest_EventScript_CherylWalkToExit30
	call_if_eq VAR_0x8005, 31, EternaForest_EventScript_CherylWalkToExit31
	call_if_eq VAR_0x8005, 32, EternaForest_EventScript_CherylWalkToExit32
	call_if_eq VAR_0x8005, 33, EternaForest_EventScript_CherylWalkToExit33
	call_if_eq VAR_0x8005, 34, EternaForest_EventScript_CherylWalkToExit34
	applywaitmovement LOCALID_ETERNA_FOREST_CHERYL, Common_Movement_WalkInPlaceFasterLeft
	msgbox EternaForest_Text_ImSureWellMeetAgain, MSGBOX_DEFAULT
	closemessage
	applywaitmovement LOCALID_ETERNA_FOREST_CHERYL, EternaForest_Movement_CherylLeave
	playse SE_EXIT
	removeobject LOCALID_ETERNA_FOREST_CHERYL
	waitse
	setflag FLAG_HIDE_ETERNA_FOREST_CHERYL
	setvar VAR_ETERNA_FOREST_CHERYL_STATE, 2
	releaseall
	end

EternaForest_EventScript_CherylWalkToExit29::
	applymovement LOCALID_ETERNA_FOREST_CHERYL, EternaForest_Movement_CherylWalkToExit29
	applymovement LOCALID_PLAYER, EternaForest_Movement_PlayerWatchCherylSouth2
	waitmovement 0
	return

EternaForest_EventScript_CherylWalkToExit30::
	applymovement LOCALID_ETERNA_FOREST_CHERYL, EternaForest_Movement_CherylWalkToExit30
	applymovement LOCALID_PLAYER, EternaForest_Movement_PlayerWatchCherylSouth
	waitmovement 0
	return

EternaForest_EventScript_CherylWalkToExit31::
	applymovement LOCALID_ETERNA_FOREST_CHERYL, EternaForest_Movement_CherylWalkToExit31
	applymovement LOCALID_PLAYER, EternaForest_Movement_PlayerWatchCherylSouth
	waitmovement 0
	return

EternaForest_EventScript_CherylWalkToExit32::
	applymovement LOCALID_ETERNA_FOREST_CHERYL, EternaForest_Movement_CherylWalkToExit32
	applymovement LOCALID_PLAYER, EternaForest_Movement_PlayerWatchCherylNorth
	waitmovement 0
	return

EternaForest_EventScript_CherylWalkToExit33::
	applymovement LOCALID_ETERNA_FOREST_CHERYL, EternaForest_Movement_CherylWalkToExit33
	applymovement LOCALID_PLAYER, EternaForest_Movement_PlayerWatchCherylNorth
	waitmovement 0
	return

EternaForest_EventScript_CherylWalkToExit34::
	applymovement LOCALID_ETERNA_FOREST_CHERYL, EternaForest_Movement_CherylWalkToExit34
	applymovement LOCALID_PLAYER, EternaForest_Movement_PlayerWatchCherylNorth2
	waitmovement 0
	return

EternaForest_Movement_CherylNoticeAndWalkToPlayer19:
	emote_exclamation_mark
	walk_down
	step_end

EternaForest_Movement_CherylNoticeAndWalkToPlayer20:
	emote_exclamation_mark
	walk_down
	walk_right
	face_down
	step_end

EternaForest_Movement_CherylWalkBack19:
	walk_up
	walk_up
	face_down
	step_end

EternaForest_Movement_CherylWalkBack20:
	walk_up
	walk_up
	walk_left
	face_down
	step_end

EternaForest_Movement_CherylExclamationMark:
	walk_in_place_faster_right
	emote_exclamation_mark
	step_end

EternaForest_Movement_PlayerFaceCheryl:
	delay_8
	delay_8
	delay_8
	walk_in_place_faster_left
	step_end

EternaForest_Movement_CherylWalkToExit29:
	walk_down
	walk_down
	walk_right
	walk_right
	walk_right
	walk_right
	walk_right
	step_end

EternaForest_Movement_CherylWalkToExit30:
	walk_down
	walk_right
	walk_right
	walk_right
	walk_right
	walk_right
	step_end

EternaForest_Movement_CherylWalkToExit31:
	walk_down
	walk_right
	walk_right
	walk_right
	walk_right
	walk_right
	step_end

EternaForest_Movement_CherylWalkToExit32:
	walk_up
	walk_right
	walk_right
	walk_right
	walk_right
	walk_right
	step_end

EternaForest_Movement_CherylWalkToExit33:
	walk_up
	walk_right
	walk_right
	walk_right
	walk_right
	walk_right
	step_end

EternaForest_Movement_CherylWalkToExit34:
	walk_up
	walk_up
	walk_right
	walk_right
	walk_right
	walk_right
	walk_right
	step_end

EternaForest_Movement_CherylLeave:
	delay_8
	walk_in_place_faster_right
	step_end

EternaForest_Movement_PlayerWatchCherylSouth:
	delay_8
	walk_in_place_faster_down
	delay_8
	walk_in_place_faster_right
	step_end

EternaForest_Movement_PlayerWatchCherylSouth2:
	delay_8
	delay_8
	walk_in_place_faster_down
	delay_8
	walk_in_place_faster_right
	step_end

EternaForest_Movement_PlayerWatchCherylNorth:
	delay_8
	walk_in_place_faster_up
	delay_8
	walk_in_place_faster_right
	step_end

EternaForest_Movement_PlayerWatchCherylNorth2:
	delay_8
	delay_8
	walk_in_place_faster_up
	delay_8
	walk_in_place_faster_right
	step_end

EternaForest_EventScript_Jack::
	trainerbattle_single TRAINER_LYLE, EternaForest_Text_JackIntro, EternaForest_Text_JackDefeat
	msgbox EternaForest_Text_JackPostBattle, MSGBOX_AUTOCLOSE
	end

EternaForest_EventScript_Briana::
	trainerbattle_single TRAINER_KAYLEE, EternaForest_Text_BrianaIntro, EternaForest_Text_BrianaDefeat
	msgbox EternaForest_Text_BrianaPostBattle, MSGBOX_AUTOCLOSE
	end

EternaForest_EventScript_Lindsey::
	trainerbattle_single TRAINER_NANCY, EternaForest_Text_LindseyIntro, EternaForest_Text_LindseyDefeat
	msgbox EternaForest_Text_LindseyPostBattle, MSGBOX_AUTOCLOSE
	end

EternaForest_EventScript_Elijah::
	trainerbattle_single TRAINER_LUCAS_1, EternaForest_Text_ElijahIntro, EternaForest_Text_ElijahDefeat
	msgbox EternaForest_Text_ElijahPostBattle, MSGBOX_AUTOCLOSE
	end

EternaForest_EventScript_Kody::
	trainerbattle_single TRAINER_KAI, EternaForest_Text_KodyIntro, EternaForest_Text_KodyDefeat
	msgbox EternaForest_Text_KodyPostBattle, MSGBOX_AUTOCLOSE
	end

EternaForest_EventScript_Rachael::
	trainerbattle_single TRAINER_ANGELINA, EternaForest_Text_RachaelIntro, EternaForest_Text_RachaelDefeat
	msgbox EternaForest_Text_RachaelPostBattle, MSGBOX_AUTOCLOSE
	end

EternaForest_EventScript_Donald::
	trainerbattle_single TRAINER_DAWSON, EternaForest_Text_DonaldIntro, EternaForest_Text_DonaldDefeat
	msgbox EternaForest_Text_DonaldPostBattle, MSGBOX_AUTOCLOSE
	end

EternaForest_EventScript_Phillip::
	trainerbattle_single TRAINER_MIKE_1, EternaForest_Text_PhillipIntro, EternaForest_Text_PhillipDefeat
	msgbox EternaForest_Text_PhillipPostBattle, MSGBOX_AUTOCLOSE
	end

EternaForest_EventScript_BugCatcher::
	msgbox EternaForest_Text_SearchingForBugPokemon, MSGBOX_NPC
	end

EternaForest_EventScript_Sign::
	msgbox EternaForest_Text_Sign, MSGBOX_SIGN
	end

EternaForest_EventScript_TrainerTips::
	msgbox EternaForest_Text_TrainerTips, MSGBOX_SIGN
	end

EternaForest_Text_MayIGoWithYou:
	.string "Hello, my name's CHERYL.\n"
	.string "And you are...?\p"
	.string "OK, so your name is {PLAYER}.\n"
	.string "I'm sincerely glad to meet you.\p"
	.string "{PLAYER}, may I ask a big favor\n"
	.string "of you?\p"
	.string "I want to get through this forest,\n"
	.string "but I'm afraid of doing it alone.\p"
	.string "I've heard that there is a sinister\n"
	.string "group of people called TEAM GALACTIC\l"
	.string "about.\p"
	.string "I think there'd be safety in numbers.\n"
	.string "Please, may I go through with you?$"

EternaForest_Text_LetsGetThroughForestTogether:
	.string "CHERYL: Oh, hi, {PLAYER}!\n"
	.string "I've been waiting like I said.\p"
	.string "Let's get through this forest\n"
	.string "together.$"

EternaForest_Text_PlayerDecidedToGoWithCheryl:
	.string "{PLAYER} decided to go with\n"
	.string "CHERYL!$"

EternaForest_Text_IllKeepPokemonHealthy:
	.string "CHERYL: I'll keep your POKéMON\n"
	.string "in perfect health.$"

EternaForest_Text_IllWaitHereForYouThen:
	.string "CHERYL: Oh? Are you going back?\n"
	.string "I'll wait here for you, then.$"

EternaForest_Text_ThankYouSoMuch:
	.string "CHERYL: Oh! There's the exit!\n"
	.string "I'm so relieved... We finally got here.\p"
	.string "I would've never been able to get\n"
	.string "through this by myself.\p"
	.string "Thank you so much, {PLAYER}!$"

EternaForest_Text_TokenOfAppreciation:
	.string "This is my token of appreciation.\n"
	.string "Please accept it!$"

EternaForest_Text_ImSureWellMeetAgain:
	.string "I'm sure we'll meet again somewhere!\n"
	.string "Bye for now!$"

EternaForest_Text_SearchingForBugPokemon:
	.string "I'm searching for bug POKéMON so I can\n"
	.string "win at the ETERNA GYM.\p"
	.string "Bug POKéMON are great, you know?\n"
	.string "They grow really fast!$"

EternaForest_Text_Sign:
	.string "ETERNA FOREST\n"
	.string "Where Time Flows Eternally$"

EternaForest_Text_TrainerTips:
	.string "TRAINER TIPS\p"
	.string "In forests and caves, there are many\n"
	.string "items on the ground that cannot be\l"
	.string "seen in the murky darkness.\p"
	.string "Take the time to search carefully!$"

EternaForest_EventScript_Gardenia::
	lockall
	faceplayer
	msgbox EternaForest_Text_GardeniaIntriguedByRumors, MSGBOX_DEFAULT
	closemessage
	applymovement LOCALID_ETERNA_FOREST_GARDENIA, Common_Movement_FaceUp
	waitmovement 0
	msgbox EternaForest_Text_GardeniaGhostPokemon, MSGBOX_DEFAULT
	applymovement LOCALID_ETERNA_FOREST_GARDENIA, Common_Movement_FacePlayer
	waitmovement 0
	msgbox EternaForest_Text_GardeniaGoodLuck, MSGBOX_DEFAULT
	closemessage
	goto_if_eq VAR_FACING, DIR_NORTH, EternaForest_EventScript_GardeniaLeaveNorth
	applymovement LOCALID_ETERNA_FOREST_GARDENIA, EternaForest_Movement_GardeniaLeave
	waitmovement 0
	goto EternaForest_EventScript_RemoveGardenia
	end

EternaForest_EventScript_GardeniaLeaveNorth::
	applymovement LOCALID_ETERNA_FOREST_GARDENIA, EternaForest_Movement_GardeniaLeaveNorth
	waitmovement 0
EternaForest_EventScript_RemoveGardenia::
	removeobject LOCALID_ETERNA_FOREST_GARDENIA
	setflag FLAG_HIDE_ETERNA_FOREST_GARDENIA
	releaseall
	end

EternaForest_Movement_GardeniaLeave:
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

EternaForest_Movement_GardeniaLeaveNorth:
	walk_left
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
	walk_right
	step_end

EternaForest_Text_GardeniaIntriguedByRumors:
	.string "GARDENIA: Hiya! Were you intrigued\n"
	.string "by the rumors going around, too?$"

EternaForest_Text_GardeniaGhostPokemon:
	.string "You know, people've been talking about\n"
	.string "the ghost POKéMON of the OLD CHATEAU.\p"
	.string "I've been hearing rumors about\n"
	.string "sinister shadows, too.$"

EternaForest_Text_GardeniaGoodLuck:
	.string "I should check it out myself, but\n"
	.string "going in there is…\p"
	.string "…Well, you know, being that I'm\n"
	.string "a GYM LEADER, I'm always busy.\p"
	.string "So, since I'm so busy, I can't go in.\n"
	.string "It's not because I'm s-scared!\p"
	.string "S-so, good luck with your\n"
	.string "investigation!$"
