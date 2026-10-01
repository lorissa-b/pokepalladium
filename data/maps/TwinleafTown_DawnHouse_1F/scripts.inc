TwinleafTown_DawnHouse_1F_MapScripts::
	map_script MAP_SCRIPT_ON_LOAD, TwinleafTown_DawnHouse_1F_OnLoad
	map_script MAP_SCRIPT_ON_TRANSITION, TwinleafTown_DawnHouse_1F_OnTransition
	map_script MAP_SCRIPT_ON_FRAME_TABLE, TwinleafTown_DawnHouse_1F_OnFrame
	.byte 0

TwinleafTown_DawnHouse_1F_OnLoad:
	call_if_lt VAR_TWINLEAF_INTRO_STATE, 6, TwinleafTown_DawnHouse_1F_EventScript_SetMovingBoxes
	call_if_set FLAG_RECEIVED_RUNNING_SHOES, TwinleafTown_DawnHouse_1F_EventScript_CheckShowShoesManual
	end

TwinleafTown_DawnHouse_1F_EventScript_SetMovingBoxes::
	setmetatile 5, 4, METATILE_BrendansMaysHouse_MovingBox_Open, TRUE
	setmetatile 5, 2, METATILE_BrendansMaysHouse_MovingBox_Closed, TRUE
	return

TwinleafTown_DawnHouse_1F_EventScript_CheckShowShoesManual::
	checkplayergender
	goto_if_eq VAR_RESULT, FEMALE, TwinleafTown_DawnHouse_1F_EventScript_ShowRunningShoesManual
	return

TwinleafTown_DawnHouse_1F_EventScript_ShowRunningShoesManual::
	setmetatile 6, 7, METATILE_BrendansMaysHouse_BookOnTable, TRUE
	return

TwinleafTown_DawnHouse_1F_OnTransition:
	call_if_eq VAR_TWINLEAF_TOWN_STATE, 3, TwinleafTown_DawnHouse_1F_EventScript_RivalMomVisiting
	call_if_eq VAR_TWINLEAF_INTRO_STATE, 8, TwinleafTown_DawnHouse_1F_EventScript_MoveMomNearStairs
	call_if_eq VAR_TWINLEAF_INTRO_STATE, 3, TwinleafTown_DawnHouse_1F_EventScript_MoveMomToDoor
	call_if_eq VAR_TWINLEAF_INTRO_STATE, 5, TwinleafTown_DawnHouse_1F_EventScript_MoveMomToStairs
	call_if_eq VAR_TWINLEAF_INTRO_STATE, 6, TwinleafTown_DawnHouse_1F_EventScript_MoveMomToTV
	end

@ After getting the POKéDEX, the rival's mom is sitting at the table with Mom
TwinleafTown_DawnHouse_1F_EventScript_RivalMomVisiting::
	clearflag FLAG_HIDE_TWINLEAF_TOWN_DAWN_HOUSE_RIVAL_MOM
	return

TwinleafTown_DawnHouse_1F_EventScript_MoveMomToStairs::
	setobjectxyperm LOCALID_PLAYERS_HOUSE_1F_MOM, 2, 4
	setobjectmovementtype LOCALID_PLAYERS_HOUSE_1F_MOM, MOVEMENT_TYPE_FACE_UP
	return

TwinleafTown_DawnHouse_1F_EventScript_MoveMomToTV::
	setobjectxyperm LOCALID_PLAYERS_HOUSE_1F_MOM, 6, 5
	setobjectmovementtype LOCALID_PLAYERS_HOUSE_1F_MOM, MOVEMENT_TYPE_FACE_UP
	return

TwinleafTown_DawnHouse_1F_EventScript_MoveMomNearStairs::
	setobjectxyperm LOCALID_PLAYERS_HOUSE_1F_MOM, 2, 5
	setobjectmovementtype LOCALID_PLAYERS_HOUSE_1F_MOM, MOVEMENT_TYPE_FACE_UP
	return

TwinleafTown_DawnHouse_1F_EventScript_MoveMomToDoor::
	setobjectxyperm LOCALID_PLAYERS_HOUSE_1F_MOM, 1, 8
	setobjectmovementtype LOCALID_PLAYERS_HOUSE_1F_MOM, MOVEMENT_TYPE_FACE_UP
	return

@ Many of the below scripts have no gender check because they assume youre in the correct house
TwinleafTown_DawnHouse_1F_OnFrame:
	map_script_2 VAR_TWINLEAF_TOWN_STATE, 3, TwinleafTown_DawnHouse_1F_EventScript_HomeWithPokedex
	map_script_2 VAR_TWINLEAF_INTRO_STATE, 8, TwinleafTown_DawnHouse_1F_EventScript_RivalWasLookingForYou
	map_script_2 VAR_TWINLEAF_INTRO_STATE, 3, TwinleafTown_DawnHouse_1F_EventScript_EnterHouseMovingIn
	map_script_2 VAR_TWINLEAF_INTRO_STATE, 5, TwinleafTown_DawnHouse_1F_EventScript_GoUpstairsToSetClock
	map_script_2 VAR_TWINLEAF_INTRO_STATE, 6, TwinleafTown_DawnHouse_1F_EventScript_PetalburgGymReport
	map_script_2 VAR_TWINLEAF_HOUSES_STATE_BRENDAN, 1, TwinleafTown_DawnHouse_1F_EventScript_YoureNewNeighbor
	map_script_2 VAR_TWINLEAF_HOUSES_STATE_MAY, 3, PlayersHouse_1F_EventScript_GetSSTicketAndSeeLatiTV
	.2byte 0

@ Coming home with the POKéDEX. Mom (8, 6) and the rival's mom (8, 7) are
@ sitting on the far side of the table; the player sits down across from them
@ at 5, 7. The rival's mom hands over a TOWN MAP and Mom gives RUNNING SHOES.
TwinleafTown_DawnHouse_1F_EventScript_HomeWithPokedex::
	lockall
	playse SE_PIN
	applywaitmovement LOCALID_RIVALS_HOUSE_1F_MOM, Common_Movement_ExclamationMark
	applywaitmovement LOCALID_RIVALS_HOUSE_1F_MOM, Common_Movement_Delay48
	applywaitmovement LOCALID_PLAYER, TwinleafTown_DawnHouse_1F_Movement_PlayerSitAtTable
	msgbox RivalsHouse_1F_Text_RivalRanAheadBringTownMap, MSGBOX_DEFAULT
	giveitem ITEM_TOWN_MAP
	msgbox RivalsHouse_1F_Text_KeepTheExtraTownMap, MSGBOX_DEFAULT
	closemessage
	applywaitmovement LOCALID_PLAYERS_HOUSE_1F_MOM, Common_Movement_ExclamationMark
	msgbox PlayersHouse_1F_Text_MomWearTheseRunningShoes, MSGBOX_DEFAULT
	playfanfare MUS_OBTAIN_ITEM
	message TwinleafTown_Text_SwitchShoesWithRunningShoes
	waitfanfare
	setflag FLAG_RECEIVED_RUNNING_SHOES
	setflag FLAG_SYS_B_DASH
	msgbox TwinleafTown_Text_ExplainRunningShoes, MSGBOX_DEFAULT
	msgbox TwinleafTown_Text_ComeHomeIfAnythingHappens, MSGBOX_DEFAULT
	closemessage
	@ She stays at the table for this visit and goes home afterwards
	setflag FLAG_HIDE_TWINLEAF_TOWN_DAWN_HOUSE_RIVAL_MOM
	setvar VAR_TWINLEAF_TOWN_STATE, 4
	releaseall
	end

@ From the door mat (1, 8) to the chair across the table (5, 7)
TwinleafTown_DawnHouse_1F_Movement_PlayerSitAtTable:
	walk_up
	walk_right
	walk_right
	walk_right
	walk_right
	walk_in_place_faster_right
	step_end

@ First time coming downstairs after the game starts
TwinleafTown_DawnHouse_1F_EventScript_RivalWasLookingForYou::
	lockall
	playse SE_PIN
	applywaitmovement LOCALID_PLAYERS_HOUSE_1F_MOM, Common_Movement_ExclamationMark
	applywaitmovement LOCALID_PLAYERS_HOUSE_1F_MOM, Common_Movement_Delay48
	applywaitmovement LOCALID_PLAYER, Common_Movement_WalkInPlaceFasterDown
	applywaitmovement LOCALID_PLAYERS_HOUSE_1F_MOM, TwinleafTown_DawnHouse_1F_Movement_MomApproachStairs
	msgbox PlayersHouse_1F_Text_RivalWasLookingForYou, MSGBOX_DEFAULT
	closemessage
	applywaitmovement LOCALID_PLAYERS_HOUSE_1F_MOM, TwinleafTown_DawnHouse_1F_Movement_MomReturnFromStairs
	setvar VAR_TWINLEAF_INTRO_STATE, 9
	releaseall
	end

@ The player steps off the stairs onto 2, 3, so Mom stops just below them
TwinleafTown_DawnHouse_1F_Movement_MomApproachStairs:
	walk_up
	step_end

TwinleafTown_DawnHouse_1F_Movement_MomReturnFromStairs:
	walk_down
	walk_in_place_faster_up
	step_end

TwinleafTown_DawnHouse_1F_EventScript_GoUpstairsToSetClock::
	lockall
	msgbox PlayersHouse_1F_Text_GoSetTheClock, MSGBOX_DEFAULT
	closemessage
	applymovement LOCALID_PLAYER, TwinleafTown_DawnHouse_1F_Movement_PushTowardStairs
	applymovement LOCALID_PLAYERS_HOUSE_1F_MOM, TwinleafTown_DawnHouse_1F_Movement_PushTowardStairs
	waitmovement 0
	warp MAP_TWINLEAF_TOWN_DAWN_HOUSE_2F, 1, 1
	waitstate
	releaseall
	end

TwinleafTown_DawnHouse_1F_Movement_PushTowardStairs:
	walk_up
	step_end

TwinleafTown_DawnHouse_1F_EventScript_EnterHouseMovingIn::
	lockall
	setvar VAR_0x8004, LOCALID_PLAYERS_HOUSE_1F_MOM
	setvar VAR_0x8005, FEMALE
	goto PlayersHouse_1F_EventScript_EnterHouseMovingIn
	end

TwinleafTown_DawnHouse_1F_EventScript_PetalburgGymReport::
	lockall
	setvar VAR_0x8004, FEMALE
	setvar VAR_0x8005, LOCALID_PLAYERS_HOUSE_1F_MOM
	goto PlayersHouse_1F_EventScript_PetalburgGymReportFemale
	end

TwinleafTown_DawnHouse_1F_EventScript_YoureNewNeighbor::
	lockall
	playse SE_PIN
	applywaitmovement LOCALID_RIVALS_HOUSE_1F_MOM, Common_Movement_ExclamationMark
	applywaitmovement LOCALID_RIVALS_HOUSE_1F_MOM, Common_Movement_Delay48
	applymovement LOCALID_PLAYER, Common_Movement_WalkInPlaceFasterRight
	applymovement LOCALID_RIVALS_HOUSE_1F_MOM, TwinleafTown_DawnHouse_1F_Movement_RivalMomApproach
	waitmovement 0
	special GetRivalSonDaughterString
	msgbox RivalsHouse_1F_Text_OhYoureTheNewNeighbor, MSGBOX_DEFAULT
	setflag FLAG_MET_RIVAL_MOM
	setvar VAR_TWINLEAF_HOUSES_STATE_BRENDAN, 2
	releaseall
	end

TwinleafTown_DawnHouse_1F_Movement_RivalMomApproach:
	walk_down
	walk_left
	walk_left
	walk_left
	walk_left
	walk_left
	step_end

RivalsHouse_1F_EventScript_RivalMom::
	lock
	faceplayer
	goto_if_ge VAR_TWINLEAF_TOWN_STATE, 4, RivalsHouse_1F_EventScript_PleaseDeliverTownMap
	goto_if_set FLAG_DEFEATED_RIVAL_ROUTE202, RivalsHouse_1F_EventScript_GoHomeEverySoOften
	goto_if_set FLAG_SYS_POKEMON_GET, RivalsHouse_1F_EventScript_RivalIsOnRoute202
	goto_if_eq VAR_TWINLEAF_INTRO_STATE, 10, RivalsHouse_1F_EventScript_RivalRanOffToRoute201
	goto_if_eq VAR_TWINLEAF_RIVAL_STATE, 3, RivalsHouse_1F_EventScript_RivalTooBusy
	special GetRivalSonDaughterString
	msgbox RivalsHouse_1F_Text_LikeChildLikeFather, MSGBOX_DEFAULT
	release
	end

RivalsHouse_1F_EventScript_PleaseDeliverTownMap::
	msgbox RivalsHouse_1F_Text_PleaseDeliverTownMap, MSGBOX_DEFAULT
	release
	end

RivalsHouse_1F_EventScript_RivalTooBusy::
	msgbox RivalsHouse_1F_Text_TooBusyToNoticeVisit, MSGBOX_DEFAULT
	release
	end

RivalsHouse_1F_EventScript_RivalRanOffToRoute201::
	msgbox RivalsHouse_1F_Text_RivalRanOffToRoute201, MSGBOX_DEFAULT
	release
	end

RivalsHouse_1F_EventScript_RivalIsOnRoute202::
	msgbox RivalsHouse_1F_Text_WentOutToRoute202, MSGBOX_DEFAULT
	release
	end

RivalsHouse_1F_EventScript_GoHomeEverySoOften::
	msgbox RivalsHouse_1F_Text_ShouldGoHomeEverySoOften, MSGBOX_DEFAULT
	release
	end

RivalsHouse_1F_EventScript_RivalSibling::
	lock
	faceplayer
	special GetPlayerBigGuyGirlString
	msgbox RivalsHouse_1F_Text_DoYouHavePokemon, MSGBOX_DEFAULT
	release
	end

TwinleafTown_DawnHouse_1F_EventScript_GoSeeRoom::
	lockall
	setvar VAR_0x8004, LOCALID_PLAYERS_HOUSE_1F_MOM
	setvar VAR_0x8005, FEMALE
	applywaitmovement VAR_0x8004, Common_Movement_WalkInPlaceFasterRight
	goto PlayersHouse_1F_EventScript_MomGoSeeRoom
	end

TwinleafTown_DawnHouse_1F_EventScript_MeetRival0::
	lockall
	setvar VAR_0x8008, 0
	goto TwinleafTown_DawnHouse_1F_EventScript_MeetRival
	end

TwinleafTown_DawnHouse_1F_EventScript_MeetRival1::
	lockall
	setvar VAR_0x8008, 1
	goto TwinleafTown_DawnHouse_1F_EventScript_MeetRival
	end

TwinleafTown_DawnHouse_1F_EventScript_MeetRival2::
	lockall
	setvar VAR_0x8008, 2
	goto TwinleafTown_DawnHouse_1F_EventScript_MeetRival
	end

TwinleafTown_DawnHouse_1F_EventScript_MeetRival::
	playse SE_EXIT
	delay 10
	addobject LOCALID_RIVALS_HOUSE_1F_RIVAL
	delay 30
	playse SE_PIN
	applywaitmovement LOCALID_RIVALS_HOUSE_1F_RIVAL, Common_Movement_ExclamationMark
	applywaitmovement LOCALID_RIVALS_HOUSE_1F_RIVAL, Common_Movement_Delay48
	call_if_ne VAR_0x8008, 1, TwinleafTown_DawnHouse_1F_EventScript_PlayerFaceMay
	playbgm MUS_ENCOUNTER_MAY, TRUE
	call_if_eq VAR_0x8008, 0, TwinleafTown_DawnHouse_1F_EventScript_MayApproachPlayer0
	call_if_eq VAR_0x8008, 1, TwinleafTown_DawnHouse_1F_EventScript_MayApproachPlayer1
	call_if_eq VAR_0x8008, 2, TwinleafTown_DawnHouse_1F_EventScript_MayApproachPlayer2
	msgbox RivalsHouse_1F_Text_MayWhoAreYou, MSGBOX_DEFAULT
	closemessage
	call_if_eq VAR_0x8008, 0, TwinleafTown_DawnHouse_1F_EventScript_MayGoUpstairs0
	call_if_eq VAR_0x8008, 1, TwinleafTown_DawnHouse_1F_EventScript_MayGoUpstairs1
	call_if_eq VAR_0x8008, 2, TwinleafTown_DawnHouse_1F_EventScript_MayGoUpstairs2
	playse SE_EXIT
	removeobject LOCALID_RIVALS_HOUSE_1F_RIVAL
	setflag FLAG_HIDE_TWINLEAF_TOWN_DAWN_HOUSE_MAY
	setflag FLAG_HIDE_TWINLEAF_TOWN_DAWN_HOUSE_2F_POKE_BALL
	clearflag FLAG_HIDE_TWINLEAF_TOWN_DAWN_HOUSE_RIVAL_BEDROOM
	delay 30
	setvar VAR_TWINLEAF_RIVAL_STATE, 3
	setvar VAR_TWINLEAF_TOWN_STATE, 1
	savebgm MUS_DUMMY
	fadedefaultbgm
	releaseall
	end

TwinleafTown_DawnHouse_1F_EventScript_PlayerFaceMay::
	applywaitmovement LOCALID_PLAYER, Common_Movement_WalkInPlaceFasterDown
	return

TwinleafTown_DawnHouse_1F_EventScript_MayApproachPlayer0::
	applywaitmovement LOCALID_RIVALS_HOUSE_1F_RIVAL, TwinleafTown_DawnHouse_1F_Movement_MayApproachPlayer0
	return

TwinleafTown_DawnHouse_1F_EventScript_MayApproachPlayer1::
	applywaitmovement LOCALID_RIVALS_HOUSE_1F_RIVAL, TwinleafTown_DawnHouse_1F_Movement_MayApproachPlayer1
	return

TwinleafTown_DawnHouse_1F_EventScript_MayApproachPlayer2::
	applywaitmovement LOCALID_RIVALS_HOUSE_1F_RIVAL, TwinleafTown_DawnHouse_1F_Movement_MayApproachPlayer2
	return

TwinleafTown_DawnHouse_1F_Movement_MayApproachPlayer0:
	walk_in_place_faster_left
	walk_left
	walk_in_place_faster_up
	walk_up
	walk_up
	walk_up
	walk_up
	step_end

TwinleafTown_DawnHouse_1F_Movement_MayApproachPlayer1:
	walk_up
	walk_up
	walk_up
	step_end

TwinleafTown_DawnHouse_1F_Movement_MayApproachPlayer2:
	walk_in_place_faster_right
	walk_right
	walk_in_place_faster_up
	walk_up
	walk_up
	walk_up
	walk_up
	step_end

TwinleafTown_DawnHouse_1F_EventScript_MayGoUpstairs0::
	applymovement LOCALID_PLAYER, TwinleafTown_DawnHouse_1F_Movement_PlayerWatchMayExit0
	applymovement LOCALID_RIVALS_HOUSE_1F_RIVAL, TwinleafTown_DawnHouse_1F_Movement_MayGoUpstairs0
	waitmovement 0
	return

TwinleafTown_DawnHouse_1F_EventScript_MayGoUpstairs1::
	applymovement LOCALID_PLAYER, TwinleafTown_DawnHouse_1F_Movement_PlayerWatchMayExit1
	applymovement LOCALID_RIVALS_HOUSE_1F_RIVAL, TwinleafTown_DawnHouse_1F_Movement_MayGoUpstairs1
	waitmovement 0
	return

TwinleafTown_DawnHouse_1F_EventScript_MayGoUpstairs2::
	applymovement LOCALID_PLAYER, TwinleafTown_DawnHouse_1F_Movement_PlayerWatchMayExit2
	applymovement LOCALID_RIVALS_HOUSE_1F_RIVAL, TwinleafTown_DawnHouse_1F_Movement_MayGoUpstairs2
	waitmovement 0
	return

TwinleafTown_DawnHouse_1F_Movement_PlayerWatchMayExit0:
	delay_16
	delay_8
	walk_in_place_faster_right
	step_end

TwinleafTown_DawnHouse_1F_Movement_PlayerWatchMayExit1:
	delay_16
	delay_8
	walk_in_place_faster_right
	delay_16
	delay_16
	walk_in_place_faster_up
	step_end

TwinleafTown_DawnHouse_1F_Movement_PlayerWatchMayExit2:
	delay_16
	delay_8
	walk_in_place_faster_left
	step_end

TwinleafTown_DawnHouse_1F_Movement_MayGoUpstairs0:
	walk_in_place_faster_right
	walk_right
	walk_in_place_faster_up
	walk_up
	walk_up
	step_end

TwinleafTown_DawnHouse_1F_Movement_MayGoUpstairs1:
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

TwinleafTown_DawnHouse_1F_Movement_MayGoUpstairs2:
	walk_in_place_faster_left
	walk_left
	walk_in_place_faster_up
	walk_up
	walk_up
	step_end

RivalsHouse_1F_Text_OhYoureTheNewNeighbor:
	.string "Oh, hello. And you are?\p"
	.string "… … … … … … … … …\n"
	.string "… … … … … … … … …\p"
	.string "Oh, you're {PLAYER}{KUN}, our new next-door\n"
	.string "neighbor! Hi!\p"
	.string "We have a {STR_VAR_1} about the same\n"
	.string "age as you.\p"
	.string "Our {STR_VAR_1} was excited about making\n"
	.string "a new friend.\p"
	.string "Our {STR_VAR_1} is upstairs, I think.$"

RivalsHouse_1F_Text_LikeChildLikeFather:
	.string "Like child, like father.\p"
	.string "My husband is as wild about POKéMON\n"
	.string "as our child.\p"
	.string "If he's not at his LAB, he's likely\n"
	.string "scrabbling about in grassy places.$"

RivalsHouse_1F_Text_RivalRanOffToRoute201:
	.string "Oh, {PLAYER}{KUN}!\n"
	.string "You just missed {RIVAL}.\p"
	.string "{RIVAL} went charging out the door\n"
	.string "shouting about ROUTE 201.\p"
	.string "Honestly, that child never stops\n"
	.string "to walk anywhere!$"

RivalsHouse_1F_Text_TooBusyToNoticeVisit:
	.string "That {RIVAL}!\p"
	.string "I guess our child is too busy with\n"
	.string "POKéMON to notice that you came\l"
	.string "to visit, {PLAYER}{KUN}.$"

RivalsHouse_1F_Text_WentOutToRoute202:
	.string "Oh, {RIVAL} went out to ROUTE 103\n"
	.string "just a little while ago.\p"
	.string "Like father, like child.\n"
	.string "{RIVAL} can't stay quietly at home.$"

RivalsHouse_1F_Text_ShouldGoHomeEverySoOften:
	.string "I think it's wonderful for people to\n"
	.string "travel with POKéMON.\p"
	.string "But you should go home every so often\n"
	.string "to let your mother know you're okay.\p"
	.string "She might not say it, but I'm sure she\n"
	.string "worries about you, {PLAYER}{KUN}.$"

RivalsHouse_1F_Text_MayWhoAreYou:
	.string "Huh?\n"
	.string "Who… Who are you?\p"
	.string "… … … … … … … …\n"
	.string "… … … … … … … …\p"
	.string "Oh, you're {PLAYER}{KUN}.\n"
	.string "So your move was today.\p"
	.string "Um… I'm MAY.\n"
	.string "Glad to meet you!\p"
	.string "I…\n"
	.string "I have this dream of becoming friends\l"
	.string "with POKéMON all over the world.\p"
	.string "I… I heard about you, {PLAYER}{KUN}, from\n"
	.string "my dad, PROF. BIRCH.\p"
	.string "I was hoping that you would be nice,\n"
	.string "{PLAYER}{KUN}, and that we could be friends.\p"
	.string "Oh, this is silly, isn't it?\n"
	.string "I… I've just met you, {PLAYER}{KUN}.\p"
	.string "Eheheh…\p"
	.string "Oh, no! I forgot!\p"
	.string "I was supposed to go help Dad catch\n"
	.string "some wild POKéMON!\p"
	.string "{PLAYER}{KUN}, I'll catch you later!$"

RivalsHouse_1F_Text_BrendanWhoAreYou:
	.string "Hey!\n"
	.string "You…\p"
	.string "Who are you?\p"
	.string "Oh, you're {PLAYER}, aren't you?\n"
	.string "Moved in next door, right?\p"
	.string "I didn't know that you're a girl.\p"
	.string "Dad, PROF. BIRCH, said that our new\n"
	.string "next-door neighbor is a GYM LEADER's\l"
	.string "kid, so I assumed you'd be a guy.\p"
	.string "My name's BRENDAN.\n"
	.string "So, hi, neighbor!\p"
	.string "Huh? Hey, {PLAYER}, don't you have\n"
	.string "a POKéMON?\p"
	.string "Do you want me to go catch you one?\p"
	.string "Aw, darn, I forgot…\p"
	.string "I'm supposed to go help my dad catch\n"
	.string "some wild POKéMON.\p"
	.string "Some other time, okay?$"

RivalsHouse_1F_Text_DoYouHavePokemon:
	.string "Hi, neighbor!\p"
	.string "Do you already have your\n"
	.string "own POKéMON?$"

RivalsHouse_1F_Text_RivalRanAheadBringTownMap:
	.string "Oh, {PLAYER}{KUN}! Welcome home!\p"
	.string "{RIVAL} was just here, but ran on\n"
	.string "ahead again. As usual!\p"
	.string "And guess what got left behind?\n"
	.string "This TOWN MAP.\p"
	.string "Could you please bring it to {RIVAL}\n"
	.string "when you catch up?$"

RivalsHouse_1F_Text_KeepTheExtraTownMap:
	.string "There was an extra one here, too.\n"
	.string "That one's yours, so keep it!$"

RivalsHouse_1F_Text_PleaseDeliverTownMap:
	.string "Please give that TOWN MAP to {RIVAL}\n"
	.string "when you catch up. Thank you!$"

PlayersHouse_1F_Text_MomWearTheseRunningShoes:
	.string "MOM: {PLAYER}! You got a POKéDEX\n"
	.string "from PROF. BIRCH? How wonderful!\p"
	.string "If you're going out on an adventure,\n"
	.string "you'll need these, honey.\p"
	.string "Here, wear these RUNNING SHOES.\n"
	.string "They'll put a zip in your step!$"
