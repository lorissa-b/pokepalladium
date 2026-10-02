TwinleafTown_LucasHouse_1F_MapScripts::
	map_script MAP_SCRIPT_ON_LOAD, TwinleafTown_LucasHouse_1F_OnLoad
	map_script MAP_SCRIPT_ON_TRANSITION, TwinleafTown_LucasHouse_1F_OnTransition
	map_script MAP_SCRIPT_ON_FRAME_TABLE, TwinleafTown_LucasHouse_1F_OnFrame
	.byte 0

TwinleafTown_LucasHouse_1F_OnLoad:
	call_if_lt VAR_TWINLEAF_INTRO_STATE, 6, TwinleafTown_LucasHouse_1F_EventScript_SetMovingBoxes
	call_if_set FLAG_RECEIVED_RUNNING_SHOES, TwinleafTown_LucasHouse_1F_EventScript_CheckShowShoesManual
	end

TwinleafTown_LucasHouse_1F_EventScript_SetMovingBoxes::
	setmetatile 5, 4, METATILE_BrendansMaysHouse_MovingBox_Open, TRUE
	setmetatile 5, 2, METATILE_BrendansMaysHouse_MovingBox_Closed, TRUE
	return

TwinleafTown_LucasHouse_1F_EventScript_CheckShowShoesManual::
	checkplayergender
	goto_if_eq VAR_RESULT, MALE, TwinleafTown_LucasHouse_1F_EventScript_ShowRunningShoesManual
	return

TwinleafTown_LucasHouse_1F_EventScript_ShowRunningShoesManual::
	setmetatile 3, 7, METATILE_BrendansMaysHouse_BookOnTable, TRUE
	return

TwinleafTown_LucasHouse_1F_OnTransition:
	call_if_eq VAR_TWINLEAF_INTRO_STATE, 3, TwinleafTown_LucasHouse_1F_EventScript_MoveMomToDoor
	call_if_eq VAR_TWINLEAF_INTRO_STATE, 5, TwinleafTown_LucasHouse_1F_EventScript_MoveMomToStairs
	call_if_eq VAR_TWINLEAF_INTRO_STATE, 6, TwinleafTown_LucasHouse_1F_EventScript_MoveMomToTV
	end

TwinleafTown_LucasHouse_1F_EventScript_MoveMomToStairs::
	setobjectxyperm LOCALID_PLAYERS_HOUSE_1F_MOM, 8, 4
	setobjectmovementtype LOCALID_PLAYERS_HOUSE_1F_MOM, MOVEMENT_TYPE_FACE_UP
	return

TwinleafTown_LucasHouse_1F_EventScript_MoveMomToTV::
	setobjectxyperm LOCALID_PLAYERS_HOUSE_1F_MOM, 4, 5
	setobjectmovementtype LOCALID_PLAYERS_HOUSE_1F_MOM, MOVEMENT_TYPE_FACE_UP
	return

TwinleafTown_LucasHouse_1F_EventScript_MoveMomToDoor::
	setobjectxyperm LOCALID_PLAYERS_HOUSE_1F_MOM, 9, 8
	setobjectmovementtype LOCALID_PLAYERS_HOUSE_1F_MOM, MOVEMENT_TYPE_FACE_UP
	return

@ Many of the below scripts have no gender check because they assume youre in the correct house
@ The below SS Ticket script uses Mays house state by accident(?), but theyre both set identically after the intro
TwinleafTown_LucasHouse_1F_OnFrame:
	map_script_2 VAR_TWINLEAF_INTRO_STATE, 3, TwinleafTown_LucasHouse_1F_EventScript_EnterHouseMovingIn
	map_script_2 VAR_TWINLEAF_INTRO_STATE, 5, TwinleafTown_LucasHouse_1F_EventScript_GoUpstairsToSetClock
	map_script_2 VAR_TWINLEAF_INTRO_STATE, 6, TwinleafTown_LucasHouse_1F_EventScript_PetalburgGymReport
	map_script_2 VAR_TWINLEAF_HOUSES_STATE_MAY, 1, TwinleafTown_LucasHouse_1F_EventScript_YoureNewNeighbor
	map_script_2 VAR_TWINLEAF_HOUSES_STATE_MAY, 3, PlayersHouse_1F_EventScript_GetSSTicketAndSeeLatiTV
	.2byte 0

TwinleafTown_LucasHouse_1F_EventScript_GoUpstairsToSetClock::
	lockall
	msgbox PlayersHouse_1F_Text_GoSetTheClock, MSGBOX_DEFAULT
	closemessage
	applymovement LOCALID_PLAYER, TwinleafTown_LucasHouse_1F_Movement_PushTowardStairs
	applymovement LOCALID_PLAYERS_HOUSE_1F_MOM, TwinleafTown_LucasHouse_1F_Movement_PushTowardStairs
	waitmovement 0
	warp MAP_TWINLEAF_TOWN_LUCAS_HOUSE_2F, 7, 1
	waitstate
	releaseall
	end

TwinleafTown_LucasHouse_1F_Movement_PushTowardStairs:
	walk_up
	step_end

TwinleafTown_LucasHouse_1F_EventScript_EnterHouseMovingIn::
	lockall
	setvar VAR_0x8004, LOCALID_PLAYERS_HOUSE_1F_MOM
	setvar VAR_0x8005, MALE
	goto PlayersHouse_1F_EventScript_EnterHouseMovingIn
	end

TwinleafTown_LucasHouse_1F_EventScript_PetalburgGymReport::
	lockall
	setvar VAR_0x8004, MALE
	setvar VAR_0x8005, LOCALID_PLAYERS_HOUSE_1F_MOM
	goto PlayersHouse_1F_EventScript_PetalburgGymReportMale
	end

TwinleafTown_LucasHouse_1F_EventScript_YoureNewNeighbor::
	lockall
	playse SE_PIN
	applywaitmovement LOCALID_RIVALS_HOUSE_1F_MOM, Common_Movement_ExclamationMark
	applywaitmovement LOCALID_RIVALS_HOUSE_1F_MOM, Common_Movement_Delay48
	applymovement LOCALID_PLAYER, Common_Movement_WalkInPlaceFasterLeft
	applymovement LOCALID_RIVALS_HOUSE_1F_MOM, TwinleafTown_LucasHouse_1F_Movement_RivalMomApproach
	waitmovement 0
	special GetRivalSonDaughterString
	msgbox RivalsHouse_1F_Text_OhYoureTheNewNeighbor, MSGBOX_DEFAULT
	setflag FLAG_MET_RIVAL_MOM
	setvar VAR_TWINLEAF_HOUSES_STATE_MAY, 2
	releaseall
	end

TwinleafTown_LucasHouse_1F_Movement_RivalMomApproach:
	walk_down
	walk_right
	walk_right
	walk_right
	walk_right
	walk_right
	step_end

TwinleafTown_LucasHouse_1F_EventScript_GoSeeRoom::
	lockall
	setvar VAR_0x8004, LOCALID_PLAYERS_HOUSE_1F_MOM
	setvar VAR_0x8005, MALE
	applywaitmovement VAR_0x8004, Common_Movement_WalkInPlaceFasterLeft
	goto PlayersHouse_1F_EventScript_MomGoSeeRoom
	end

TwinleafTown_LucasHouse_1F_EventScript_MeetRival0::
	lockall
	setvar VAR_0x8008, 0
	goto TwinleafTown_LucasHouse_1F_EventScript_MeetRival
	end

TwinleafTown_LucasHouse_1F_EventScript_MeetRival1::
	lockall
	setvar VAR_0x8008, 1
	goto TwinleafTown_LucasHouse_1F_EventScript_MeetRival
	end

TwinleafTown_LucasHouse_1F_EventScript_MeetRival2::
	lockall
	setvar VAR_0x8008, 2
	goto TwinleafTown_LucasHouse_1F_EventScript_MeetRival
	end

TwinleafTown_LucasHouse_1F_EventScript_MeetRival::
	playse SE_EXIT
	delay 10
	addobject LOCALID_RIVALS_HOUSE_1F_RIVAL
	delay 30
	playse SE_PIN
	applywaitmovement LOCALID_RIVALS_HOUSE_1F_RIVAL, Common_Movement_ExclamationMark
	applywaitmovement LOCALID_RIVALS_HOUSE_1F_RIVAL, Common_Movement_Delay48
	call_if_ne VAR_0x8008, 1, TwinleafTown_LucasHouse_1F_EventScript_PlayerFaceBrendan
	playbgm MUS_ENCOUNTER_BRENDAN, TRUE
	call_if_eq VAR_0x8008, 0, TwinleafTown_LucasHouse_1F_EventScript_BrendanApproachPlayer0
	call_if_eq VAR_0x8008, 1, TwinleafTown_LucasHouse_1F_EventScript_BrendanApproachPlayer1
	call_if_eq VAR_0x8008, 2, TwinleafTown_LucasHouse_1F_EventScript_BrendanApproachPlayer2
	msgbox RivalsHouse_1F_Text_BrendanWhoAreYou, MSGBOX_DEFAULT
	closemessage
	call_if_eq VAR_0x8008, 0, TwinleafTown_LucasHouse_1F_EventScript_BrendanGoUpstairs0
	call_if_eq VAR_0x8008, 1, TwinleafTown_LucasHouse_1F_EventScript_BrendanGoUpstairs1
	call_if_eq VAR_0x8008, 2, TwinleafTown_LucasHouse_1F_EventScript_BrendanGoUpstairs2
	playse SE_EXIT
	removeobject LOCALID_RIVALS_HOUSE_1F_RIVAL
	setflag FLAG_HIDE_TWINLEAF_TOWN_LUCAS_HOUSE_BRENDAN
	setflag FLAG_HIDE_TWINLEAF_TOWN_LUCAS_HOUSE_2F_POKE_BALL
	clearflag FLAG_HIDE_TWINLEAF_TOWN_LUCAS_HOUSE_RIVAL_BEDROOM
	delay 30
	setvar VAR_TWINLEAF_RIVAL_STATE, 3
	setvar VAR_TWINLEAF_TOWN_STATE, 1
	savebgm MUS_DUMMY
	fadedefaultbgm
	releaseall
	end

TwinleafTown_LucasHouse_1F_EventScript_PlayerFaceBrendan::
	applywaitmovement LOCALID_PLAYER, Common_Movement_WalkInPlaceFasterDown
	return

TwinleafTown_LucasHouse_1F_EventScript_BrendanApproachPlayer0::
	applywaitmovement LOCALID_RIVALS_HOUSE_1F_RIVAL, TwinleafTown_LucasHouse_1F_Movement_BrendanApproachPlayer0
	return

TwinleafTown_LucasHouse_1F_EventScript_BrendanApproachPlayer1::
	applywaitmovement LOCALID_RIVALS_HOUSE_1F_RIVAL, TwinleafTown_LucasHouse_1F_Movement_BrendanApproachPlayer1
	return

TwinleafTown_LucasHouse_1F_EventScript_BrendanApproachPlayer2::
	applywaitmovement LOCALID_RIVALS_HOUSE_1F_RIVAL, TwinleafTown_LucasHouse_1F_Movement_BrendanApproachPlayer2
	return

TwinleafTown_LucasHouse_1F_Movement_BrendanApproachPlayer0:
	walk_in_place_faster_left
	walk_left
	walk_in_place_faster_up
	walk_up
	walk_up
	walk_up
	walk_up
	step_end

TwinleafTown_LucasHouse_1F_Movement_BrendanApproachPlayer1:
	walk_up
	walk_up
	walk_up
	step_end

TwinleafTown_LucasHouse_1F_Movement_BrendanApproachPlayer2:
	walk_in_place_faster_right
	walk_right
	walk_in_place_faster_up
	walk_up
	walk_up
	walk_up
	walk_up
	step_end

TwinleafTown_LucasHouse_1F_EventScript_BrendanGoUpstairs0::
	applymovement LOCALID_PLAYER, TwinleafTown_LucasHouse_1F_Movement_PlayerWatchBrendanExit0
	applymovement LOCALID_RIVALS_HOUSE_1F_RIVAL, TwinleafTown_LucasHouse_1F_Movement_BrendanGoUpstairs0
	waitmovement 0
	return

TwinleafTown_LucasHouse_1F_EventScript_BrendanGoUpstairs1::
	applymovement LOCALID_PLAYER, TwinleafTown_LucasHouse_1F_Movement_PlayerWatchBrendanExit1
	applymovement LOCALID_RIVALS_HOUSE_1F_RIVAL, TwinleafTown_LucasHouse_1F_Movement_BrendanGoUpstairs1
	waitmovement 0
	return

TwinleafTown_LucasHouse_1F_EventScript_BrendanGoUpstairs2::
	applymovement LOCALID_PLAYER, TwinleafTown_LucasHouse_1F_Movement_PlayerWatchBrendanExit2
	applymovement LOCALID_RIVALS_HOUSE_1F_RIVAL, TwinleafTown_LucasHouse_1F_Movement_BrendanGoUpstairs2
	waitmovement 0
	return

TwinleafTown_LucasHouse_1F_Movement_PlayerWatchBrendanExit0:
	delay_16
	delay_8
	walk_in_place_faster_right
	step_end

TwinleafTown_LucasHouse_1F_Movement_PlayerWatchBrendanExit1:
	delay_16
	delay_8
	walk_in_place_faster_right
	delay_16
	delay_16
	walk_in_place_faster_up
	step_end

TwinleafTown_LucasHouse_1F_Movement_PlayerWatchBrendanExit2:
	delay_16
	delay_8
	walk_in_place_faster_left
	step_end

TwinleafTown_LucasHouse_1F_Movement_BrendanGoUpstairs0:
	walk_in_place_faster_right
	walk_right
	walk_in_place_faster_up
	walk_up
	walk_up
	step_end

TwinleafTown_LucasHouse_1F_Movement_BrendanGoUpstairs1:
	walk_in_place_faster_right
	walk_right
	walk_in_place_faster_up
	walk_up
	walk_up
	walk_in_place_faster_left
	walk_left
	walk_in_place_faster_up
	walk_up
	step_end

TwinleafTown_LucasHouse_1F_Movement_BrendanGoUpstairs2:
	walk_in_place_faster_left
	walk_left
	walk_in_place_faster_up
	walk_up
	walk_up
	step_end

PlayersHouse_1F_Text_IsntItNiceInHere:
	.string "MOM: See, {PLAYER}?\n"
	.string "Isn't it nice in here, too?$"

PlayersHouse_1F_Text_RivalWasLookingForYou:
	.string "MOM: Oh, {PLAYER}! You're finally up!\p"
	.string "{RIVAL} came by looking for you\n"
	.string "a little while ago.\p"
	.string "{RIVAL} seemed awfully excited\n"
	.string "about something!\p"
	.string "Why don't you go over to {RIVAL}'s\n"
	.string "house and see what's going on?$"

PlayersHouse_1F_Text_GoToRivalsHouse:
	.string "MOM: {RIVAL} was looking for you,\n"
	.string "{PLAYER}.\p"
	.string "{RIVAL}'s house is just up the road.\n"
	.string "Don't keep your friend waiting!$"

PlayersHouse_1F_Text_DidYouFindRival:
	.string "MOM: Did you find {RIVAL}?\p"
	.string "…Off to ROUTE 201, you say?\n"
	.string "Well, be careful out there!$"

PlayersHouse_1F_Text_MoversPokemonGoSetClock:
	.string "The mover's POKéMON do all the work\n"
	.string "of moving us in and cleaning up after.\l"
	.string "This is so convenient!\p"
	.string "{PLAYER}, your room is upstairs.\n"
	.string "Go check it out, dear!\p"
	.string "DAD bought you a new clock to mark\n"
	.string "our move here.\l"
	.string "Don't forget to set it!$"

PlayersHouse_1F_Text_ArentYouInterestedInRoom:
	.string "MOM: Well, {PLAYER}?\p"
	.string "Aren't you interested in seeing your\n"
	.string "very own room?$"

PlayersHouse_1F_Text_GoSetTheClock:
	.string "MOM: {PLAYER}.\p"
	.string "Go set the clock in your room, honey.$"

PlayersHouse_1F_Text_OhComeQuickly:
	.string "MOM: Oh! {PLAYER}, {PLAYER}!\n"
	.string "Quick! Come quickly!$"

PlayersHouse_1F_Text_MaybeDadWillBeOn:
	.string "MOM: Look! It's PETALBURG GYM!\n"
	.string "Maybe DAD will be on!$"

PlayersHouse_1F_Text_ItsOverWeMissedHim:
	.string "MOM: Oh… It's over.\p"
	.string "I think DAD was on, but we missed him.\n"
	.string "Too bad.$"

PlayersHouse_1F_Text_GoIntroduceYourselfNextDoor:
	.string "Oh, yes.\n"
	.string "One of DAD's friends lives in town.\p"
	.string "PROF. BIRCH is his name.\p"
	.string "He lives right next door, so you should\n"
	.string "go over and introduce yourself.$"

PlayersHouse_1F_Text_SeeYouHoney:
	.string "MOM: See you, honey!$"

PlayersHouse_1F_Text_DidYouMeetProfBirch:
	.string "MOM: Did you introduce yourself to\n"
	.string "PROF. BIRCH?$"

PlayersHouse_1F_Text_YouShouldRestABit:
	.string "MOM: How are you doing, {PLAYER}?\n"
	.string "You look a little tired.\p"
	.string "I think you should rest a bit.$"

PlayersHouse_1F_Text_TakeCareHoney:
	.string "MOM: Take care, honey!$"

PlayersHouse_1F_Text_GotDadsBadgeHeresSomethingFromMom:
	.string "MOM: Oh? Did DAD give you that BADGE?\p"
	.string "Then here's something from your MOM!$"

PlayersHouse_1F_Text_DontPushYourselfTooHard:
	.string "Don't push yourself too hard, dear.\n"
	.string "You can always come home.\p"
	.string "Go for it, honey!$"

PlayersHouse_1F_Text_IsThatAPokenav:
	.string "MOM: What is that, honey? A POKéTCH?\n"
	.string "Someone from DEVON gave it to you?\p"
	.string "Well, honey, how about registering\n"
	.string "your mom?\p"
	.string "… … …$"

PlayersHouse_1F_Text_RegisteredMom:
	.string "Registered MOM\n"
	.string "in the POKéTCH.$"

PlayersHouse_1F_Text_Vigoroth1:
	.string "Fugiiiiih!$"

PlayersHouse_1F_Text_Vigoroth2:
	.string "Huggoh, uggo uggo…$"

PlayersHouse_1F_Text_ReportFromPetalburgGym:
	.string "INTERVIEWER: …We brought you this\n"
	.string "report from in front of PETALBURG GYM.$"

PlayersHouse_1F_Text_TheresAMovieOnTV:
	.string "There is a movie on TV.\p"
	.string "Two men are dancing on a big piano\n"
	.string "keyboard.\p"
	.string "Better get going!$"

PlayersHouse_1F_Text_RunningShoesManual:
	.string "It's the instruction booklet for the\n"
	.string "RUNNING SHOES.\p"
	.string "“Press the B Button to run while\n"
	.string "wearing your RUNNING SHOES.\p"
	.string "“Lace up your RUNNING SHOES and hit\n"
	.string "the road running!”$"

PlayersHouse_1F_Text_TicketFromBrineyCameForYou:
	.string "DAD: Hm?\p"
	.string "Hey, it's {PLAYER}!\p"
	.string "It's been a while since I saw you,\n"
	.string "but you look…stronger, somehow.\p"
	.string "That's the impression I get.\n"
	.string "But your old man hasn't given up yet!\p"
	.string "Oh, yes, I have something for you.\n"
	.string "This came to you from someone named\l"
	.string "MR. BRINEY.$"

PlayersHouse_1F_Text_PortsInSlateportLilycove:
	.string "DAD: Hm, a TICKET for a ferry?\p"
	.string "If I recall, there are ferry ports in\n"
	.string "SLATEPORT and LILYCOVE.$"

PlayersHouse_1F_Text_BetterGetBackToGym:
	.string "I'd better get back to PETALBURG GYM.\p"
	.string "MOM, thanks for looking after the house\n"
	.string "while I'm away.$"

PlayersHouse_1F_Text_DadShouldStayLonger:
	.string "MOM: That DAD of yours…\p"
	.string "He comes home for the first time in a\n"
	.string "while, but all he talks about is POKéMON.\p"
	.string "He should relax and stay a little longer.$"

PlayersHouse_1F_Text_IsThatABreakingStory:
	.string "MOM: Is that a breaking news story?$"

PlayersHouse_1F_Text_LatiEmergencyNewsFlash:
	.string "We bring you this emergency\n"
	.string "news flash!\p"
	.string "In various HOENN locales, there have\n"
	.string "been reports of a BZZT…colored\l"
	.string "POKéMON in flight.\p"
	.string "The identity of this POKéMON is\n"
	.string "currently unknown.\p"
	.string "We now return you to the regular\n"
	.string "movie program.$"

PlayersHouse_1F_Text_WhatColorDidTheySay:
	.string "MOM: {PLAYER}, did you catch that?\p"
	.string "What color did the announcer say\n"
	.string "that POKéMON was?$"

PlayersHouse_1F_Text_StillUnknownPokemon:
	.string "MOM: Well, isn't that something!\n"
	.string "There are still unknown POKéMON.$"
