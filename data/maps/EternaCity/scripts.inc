@ Eterna City, east of Route 205 North, laid out as in Platinum. The west
@ edge joins Route 205 North; Route 211 (east) and the Cycling Road gate on
@ Route 206 (south door, 16-17, 57) don't exist yet.
@
@ The three Team Galactic Grunts stand around until
@ FLAG_HIDE_ETERNA_CITY_GALACTIC_GRUNTS is set, which in Platinum happens
@ when the player clears the Galactic Eterna Building. The townspeople who
@ talk about Team Galactic change their lines on the same flag.
@
@ Once Jupiter is beaten in the Galactic Eterna Building
@ (VAR_ETERNA_CITY_STATE 3), Cynthia walks up to the player crossing x=20 by
@ the Cycle Shop (rows 29-33) and offers a Togepi Egg. Turned down, or with
@ no room in the party, she waits at the spot (state 4), and won't let the
@ player past x=21 until they take it (state 5).
@
@ Earlier, as in Platinum: crossing x=15 on the way in has the rival run
@ into the player and take them to the statue to meet Cyrus (state 0 -> 1),
@ and heading back west across x=16 has Cynthia give HM01 (1 -> 2).
@
@ Not here yet: Gardenia outside the Gym and the exits blocked until the
@ Bicycle.
EternaCity_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, EternaCity_OnTransition
	.byte 0

EternaCity_OnTransition:
	setflag FLAG_VISITED_ETERNA_CITY
	end

EternaCity_EventScript_GruntStatue::
	msgbox EternaCity_Text_StayAwayFromStatue, MSGBOX_NPC
	end

EternaCity_EventScript_GruntWest::
	msgbox EternaCity_Text_IllLetYouKeepPokemon, MSGBOX_NPC
	end

EternaCity_EventScript_GruntCenter::
	msgbox EternaCity_Text_HaveYouSeenOurBuilding, MSGBOX_NPC
	end

EternaCity_EventScript_PokefanM::
	msgbox EternaCity_Text_GetYourselfABicycle, MSGBOX_NPC
	end

EternaCity_EventScript_PokefanF::
	lock
	faceplayer
	goto_if_set FLAG_HIDE_ETERNA_CITY_GALACTIC_GRUNTS, EternaCity_EventScript_WhatWillHappenToGalacticBuilding
	msgbox EternaCity_Text_AtmosphereHasGottenCreepy, MSGBOX_DEFAULT
	release
	end

EternaCity_EventScript_WhatWillHappenToGalacticBuilding::
	msgbox EternaCity_Text_WhatWillHappenToGalacticBuilding, MSGBOX_DEFAULT
	release
	end

EternaCity_EventScript_BugCatcherCycleShop::
	lock
	faceplayer
	goto_if_set FLAG_HIDE_ETERNA_CITY_GALACTIC_GRUNTS, EternaCity_EventScript_DoYouUseKickstand
	msgbox EternaCity_Text_GalacticTookFavoritePokemon, MSGBOX_DEFAULT
	release
	end

EternaCity_EventScript_DoYouUseKickstand::
	msgbox EternaCity_Text_DoYouUseKickstand, MSGBOX_DEFAULT
	release
	end

EternaCity_EventScript_Beauty::
	msgbox EternaCity_Text_TheySayThisIsAnOldCity, MSGBOX_NPC
	end

EternaCity_EventScript_ExpertM::
	msgbox EternaCity_Text_EternaCitysNotLikeBefore, MSGBOX_NPC
	end

EternaCity_EventScript_NinjaBoy::
	msgbox EternaCity_Text_AwesomePokemonFromLongAgo, MSGBOX_NPC
	end

@ In Platinum she sends players without an Explorer Kit to the
@ Underground Man. The Underground isn't in, so she always says this.
EternaCity_EventScript_PokefanFSouth::
	msgbox EternaCity_Text_YouDontHaveExplorerKit, MSGBOX_NPC
	end

EternaCity_EventScript_BugCatcherWest::
	lock
	faceplayer
	goto_if_unset FLAG_RECEIVED_BIKE, EternaCity_EventScript_FeelsGreatRidingABike
	msgbox EternaCity_Text_YouGotABicycle, MSGBOX_DEFAULT
	release
	end

EternaCity_EventScript_FeelsGreatRidingABike::
	msgbox EternaCity_Text_FeelsGreatRidingABike, MSGBOX_DEFAULT
	release
	end

EternaCity_EventScript_Statue::
	msgbox EternaCity_Text_ItsEternaCitysPokemonStatue, MSGBOX_SIGN
	end

EternaCity_EventScript_CitySign::
	msgbox EternaCity_Text_CitySign, MSGBOX_SIGN
	end

EternaCity_EventScript_GymSign::
	msgbox EternaCity_Text_GymSign, MSGBOX_SIGN
	end

EternaCity_EventScript_CycleShopSign::
	msgbox EternaCity_Text_CycleShopSign, MSGBOX_SIGN
	end

EternaCity_EventScript_GalacticBuildingSign::
	msgbox EternaCity_Text_GalacticBuildingSign, MSGBOX_SIGN
	end

EternaCity_EventScript_UndergroundMansHouseSign::
	msgbox EternaCity_Text_UndergroundMansHouseSign, MSGBOX_SIGN
	end

EternaCity_EventScript_CondominiumsSign::
	msgbox EternaCity_Text_CondominiumsSign, MSGBOX_SIGN
	end

@ Mauville's Wally call, kept because field_control_avatar.c still runs it.
EternaCity_EventScript_RegisterWallyCall::
	lockall
	pokenavcall EternaCity_Text_WallyPokenavCall
	waitmessage
	delay 30
	playfanfare MUS_REGISTER_MATCH_CALL
	msgbox EternaCity_Text_RegisteredWally, MSGBOX_DEFAULT
	waitfanfare
	closemessage
	delay 30
	setflag FLAG_ENABLE_WALLY_MATCH_CALL
	clearflag FLAG_ENABLE_FIRST_WALLY_POKENAV_CALL
	releaseall
	end


@ Crossing (15,11)-(15,13) the first time (VAR_ETERNA_CITY_STATE 0): the
@ rival runs up from the south and bumps the player one step east, then
@ takes them to the statue, where Cyrus (Steven's sprite as a stand-in)
@ is musing about Sinnoh's myths. Both end at the statue: the rival at
@ (35,12), the player at (35,13).
EternaCity_EventScript_RivalRunsIntoPlayer::
	lockall
	setobjectxyperm LOCALID_ETERNA_CITY_RIVAL, 11, 21
	clearflag FLAG_HIDE_ETERNA_CITY_RIVAL
	addobject LOCALID_ETERNA_CITY_RIVAL
	getplayerxy VAR_0x8004, VAR_0x8005
	goto_if_eq VAR_0x8005, 12, EternaCity_EventScript_RivalRunIntoPlayerY12
	goto_if_eq VAR_0x8005, 13, EternaCity_EventScript_RivalRunIntoPlayerY13
	applymovement LOCALID_ETERNA_CITY_RIVAL, EternaCity_Movement_RivalRunIntoPlayerY11
	applymovement LOCALID_PLAYER, EternaCity_Movement_PlayerGetPushedByRivalY11
	waitmovement 0
	call EternaCity_EventScript_RivalTakePlayerToStatue
	applymovement LOCALID_ETERNA_CITY_RIVAL, EternaCity_Movement_RivalWalkInFrontOfPlayerY11
	applymovement LOCALID_PLAYER, EternaCity_Movement_PlayerFaceRivalY11
	waitmovement 0
	applymovement LOCALID_ETERNA_CITY_RIVAL, EternaCity_Movement_RivalWalkToStatueY11Or13
	applymovement LOCALID_PLAYER, EternaCity_Movement_PlayerWalkToStatueY11
	waitmovement 0
	goto EternaCity_EventScript_Cyrus
	end

EternaCity_EventScript_RivalRunIntoPlayerY12::
	applymovement LOCALID_ETERNA_CITY_RIVAL, EternaCity_Movement_RivalRunIntoPlayerY12
	applymovement LOCALID_PLAYER, EternaCity_Movement_PlayerGetPushedByRivalY12
	waitmovement 0
	call EternaCity_EventScript_RivalTakePlayerToStatue
	applymovement LOCALID_ETERNA_CITY_RIVAL, EternaCity_Movement_RivalWalkInFrontOfPlayerY12
	applymovement LOCALID_PLAYER, EternaCity_Movement_PlayerFaceRivalY12
	waitmovement 0
	applymovement LOCALID_ETERNA_CITY_RIVAL, EternaCity_Movement_RivalWalkToStatueY12
	applymovement LOCALID_PLAYER, EternaCity_Movement_PlayerWalkToStatueY12
	waitmovement 0
	goto EternaCity_EventScript_Cyrus
	end

EternaCity_EventScript_RivalRunIntoPlayerY13::
	applymovement LOCALID_ETERNA_CITY_RIVAL, EternaCity_Movement_RivalRunIntoPlayerY13
	applymovement LOCALID_PLAYER, EternaCity_Movement_PlayerGetPushedByRivalY13
	waitmovement 0
	call EternaCity_EventScript_RivalTakePlayerToStatue
	applymovement LOCALID_ETERNA_CITY_RIVAL, EternaCity_Movement_RivalWalkInFrontOfPlayerY13
	applymovement LOCALID_PLAYER, EternaCity_Movement_PlayerFaceRivalY13
	waitmovement 0
	applymovement LOCALID_ETERNA_CITY_RIVAL, EternaCity_Movement_RivalWalkToStatueY11Or13
	applymovement LOCALID_PLAYER, EternaCity_Movement_PlayerWalkToStatueY13
	waitmovement 0
	goto EternaCity_EventScript_Cyrus
	end

EternaCity_EventScript_RivalTakePlayerToStatue::
	playse SE_WALL_HIT
	msgbox EternaCity_Text_BigThud, MSGBOX_DEFAULT
	closemessage
	checkplayergender
	call_if_eq VAR_RESULT, MALE, EternaCity_EventScript_PlayRivalMusicMay
	call_if_eq VAR_RESULT, FEMALE, EternaCity_EventScript_PlayRivalMusicBrendan
	msgbox EternaCity_Text_YouCameToSeeStatue, MSGBOX_DEFAULT
	closemessage
	return

@ The rival's music, as on Route 201.
EternaCity_EventScript_PlayRivalMusicMay::
	playbgm MUS_ENCOUNTER_MAY, TRUE
	return

EternaCity_EventScript_PlayRivalMusicBrendan::
	playbgm MUS_ENCOUNTER_BRENDAN, TRUE
	return

EternaCity_EventScript_Cyrus::
	msgbox EternaCity_Text_WhatThe, MSGBOX_DEFAULT
	closemessage
	fadedefaultbgm
	special SpawnCameraObject
	applymovement LOCALID_CAMERA, EternaCity_Movement_CameraPanRight
	waitmovement 0
	msgbox EternaCity_Text_SoThisIsEternasStatue, MSGBOX_DEFAULT
	closemessage
	delay 15
	applymovement LOCALID_ETERNA_CITY_CYRUS, EternaCity_Movement_CyrusWalkToPlayer
	applymovement LOCALID_CAMERA, EternaCity_Movement_CameraPanLeft
	waitmovement 0
	special RemoveCameraObject
	msgbox EternaCity_Text_PardonMeStandAside, MSGBOX_DEFAULT
	closemessage
	applymovement LOCALID_ETERNA_CITY_CYRUS, EternaCity_Movement_CyrusLeave
	applymovement LOCALID_ETERNA_CITY_RIVAL, EternaCity_Movement_RivalWatchCyrusLeave
	applymovement LOCALID_PLAYER, EternaCity_Movement_PlayerMoveAsideAndWatchCyrusLeave
	waitmovement 0
	removeobject LOCALID_ETERNA_CITY_CYRUS
	delay 35
	applymovement LOCALID_ETERNA_CITY_RIVAL, EternaCity_Movement_RivalFacePlayer
	applymovement LOCALID_PLAYER, EternaCity_Movement_PlayerFaceRival
	waitmovement 0
	msgbox EternaCity_Text_GoCheckOutStatueSeeYou, MSGBOX_DEFAULT
	closemessage
	applymovement LOCALID_ETERNA_CITY_RIVAL, EternaCity_Movement_RivalLeave
	applymovement LOCALID_PLAYER, EternaCity_Movement_PlayerWatchRivalLeave
	waitmovement 0
	removeobject LOCALID_ETERNA_CITY_RIVAL
	setvar VAR_ETERNA_CITY_STATE, 1
	releaseall
	end

@ The rival arrives on the player's tile just as they're knocked east.
EternaCity_Movement_RivalRunIntoPlayerY11:
	walk_fast_up
	walk_fast_up
	walk_fast_up
	walk_fast_up
	walk_fast_up
	walk_fast_up
	walk_fast_up
	walk_fast_up
	walk_fast_up
	walk_fast_up
	walk_fast_right
	walk_fast_right
	walk_fast_right
	walk_fast_right
	step_end

EternaCity_Movement_RivalRunIntoPlayerY12:
	walk_fast_up
	walk_fast_up
	walk_fast_up
	walk_fast_up
	walk_fast_up
	walk_fast_up
	walk_fast_up
	walk_fast_up
	walk_fast_up
	walk_fast_right
	walk_fast_right
	walk_fast_right
	walk_fast_right
	step_end

EternaCity_Movement_RivalRunIntoPlayerY13:
	walk_fast_up
	walk_fast_up
	walk_fast_up
	walk_fast_up
	walk_fast_up
	walk_fast_up
	walk_fast_up
	walk_fast_up
	walk_fast_right
	walk_fast_right
	walk_fast_right
	walk_fast_right
	step_end

EternaCity_Movement_PlayerGetPushedByRivalY11:
	delay_16
	delay_16
	delay_16
	delay_16
	delay_16
	delay_16
	delay_8
	lock_facing_direction
	walk_fast_right
	unlock_facing_direction
	face_left
	step_end

EternaCity_Movement_PlayerGetPushedByRivalY12:
	delay_16
	delay_16
	delay_16
	delay_16
	delay_16
	delay_16
	lock_facing_direction
	walk_fast_right
	unlock_facing_direction
	face_left
	step_end

EternaCity_Movement_PlayerGetPushedByRivalY13:
	delay_16
	delay_16
	delay_16
	delay_16
	delay_16
	delay_8
	lock_facing_direction
	walk_fast_right
	unlock_facing_direction
	face_left
	step_end

EternaCity_Movement_RivalWalkInFrontOfPlayerY11:
	walk_down
	walk_right
	walk_in_place_up
	step_end

EternaCity_Movement_RivalWalkInFrontOfPlayerY12:
	walk_down
	walk_right
	walk_right
	walk_up
	walk_in_place_left
	step_end

EternaCity_Movement_RivalWalkInFrontOfPlayerY13:
	walk_up
	walk_right
	walk_in_place_down
	step_end

EternaCity_Movement_PlayerFaceRivalY11:
	delay_16
	delay_8
	walk_in_place_down
	step_end

EternaCity_Movement_PlayerFaceRivalY12:
	delay_16
	delay_8
	walk_in_place_down
	delay_8
	walk_in_place_right
	step_end

EternaCity_Movement_PlayerFaceRivalY13:
	delay_16
	delay_8
	walk_in_place_up
	step_end

EternaCity_Movement_RivalWalkToStatueY11Or13:
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
	walk_right
	walk_right
	walk_right
	walk_right
	walk_right
	walk_right
	step_end

EternaCity_Movement_RivalWalkToStatueY12:
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
	walk_right
	walk_right
	walk_right
	walk_right
	walk_right
	step_end

EternaCity_Movement_PlayerWalkToStatueY11:
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
	walk_right
	walk_right
	walk_right
	walk_right
	walk_right
	walk_right
	walk_right
	walk_right
	walk_down
	walk_right
	step_end

EternaCity_Movement_PlayerWalkToStatueY12:
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
	walk_right
	walk_right
	walk_right
	walk_right
	walk_right
	walk_down
	walk_right
	step_end

EternaCity_Movement_PlayerWalkToStatueY13:
	walk_up
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
	walk_right
	walk_right
	walk_right
	walk_right
	walk_right
	walk_down
	walk_right
	step_end

EternaCity_Movement_CameraPanRight:
	walk_right
	walk_right
	walk_right
	walk_right
	step_end

EternaCity_Movement_CameraPanLeft:
	walk_left
	walk_left
	walk_left
	walk_left
	step_end

@ From (39,14) to (36,13), beside the player at (35,13).
EternaCity_Movement_CyrusWalkToPlayer:
	walk_left
	walk_left
	walk_left
	walk_up
	walk_in_place_left
	step_end

EternaCity_Movement_CyrusLeave:
	delay_16
	delay_16
	walk_left
	walk_left
	walk_left
	walk_left
	walk_left
	walk_left
	walk_left
	walk_left
	walk_left
	walk_left
	walk_left
	walk_left
	walk_left
	walk_left
	step_end

EternaCity_Movement_RivalWatchCyrusLeave:
	delay_16
	delay_16
	delay_16
	walk_in_place_faster_left
	step_end

EternaCity_Movement_PlayerMoveAsideAndWatchCyrusLeave:
	walk_down
	walk_in_place_right
	delay_16
	delay_16
	walk_in_place_left
	step_end

EternaCity_Movement_RivalFacePlayer:
	walk_fast_down
	step_end

EternaCity_Movement_PlayerFaceRival:
	walk_in_place_up
	step_end

EternaCity_Movement_RivalLeave:
	walk_fast_left
	walk_fast_left
	walk_fast_left
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

EternaCity_Movement_PlayerWatchRivalLeave:
	delay_16
	walk_in_place_left
	step_end

@ After the statue (VAR_ETERNA_CITY_STATE 1), heading back west across
@ (16-18,10) or (16,11)-(16,13) brings Cynthia (Glacia's sprite) from the
@ east with HM01. From row 10 she comes up below the player; otherwise she
@ stops just east of them. She leaves west and down column 11.
EternaCity_EventScript_CynthiaGiveCut::
	lockall
	getplayerxy VAR_0x8004, VAR_0x8005
	clearflag FLAG_HIDE_ETERNA_CITY_CYNTHIA
	setobjectmovementtype LOCALID_ETERNA_CITY_CYNTHIA, MOVEMENT_TYPE_FACE_LEFT
	goto_if_eq VAR_0x8005, 10, EternaCity_EventScript_CynthiaGiveCutFromBelow
	setobjectxyperm LOCALID_ETERNA_CITY_CYNTHIA, 25, VAR_0x8005
	addobject LOCALID_ETERNA_CITY_CYNTHIA
	applymovement LOCALID_ETERNA_CITY_CYNTHIA, EternaCity_Movement_CynthiaNoticeAndWalkUpToPlayerEast
	waitmovement 0
	applymovement LOCALID_PLAYER, Common_Movement_WalkInPlaceRight
	waitmovement 0
	msgbox EternaCity_Text_OhYouHaveAPokedex, MSGBOX_DEFAULT
	closemessage
	applymovement LOCALID_ETERNA_CITY_CYNTHIA, EternaCity_Movement_CynthiaLookAroundThenWest
	waitmovement 0
	call EternaCity_EventScript_CynthiaGiveHM01
	goto_if_eq VAR_0x8005, 13, EternaCity_EventScript_CynthiaLeaveAfterCutY13
	applymovement LOCALID_ETERNA_CITY_CYNTHIA, EternaCity_Movement_CynthiaLeaveAfterCutY11Or12
	applymovement LOCALID_PLAYER, EternaCity_Movement_PlayerWatchCynthiaLeaveAfterCutEast
	waitmovement 0
	goto EternaCity_EventScript_CynthiaGiveCutEnd
	end

EternaCity_EventScript_CynthiaLeaveAfterCutY13::
	applymovement LOCALID_ETERNA_CITY_CYNTHIA, EternaCity_Movement_CynthiaLeaveAfterCutY13
	applymovement LOCALID_PLAYER, EternaCity_Movement_PlayerWatchCynthiaLeaveAfterCutEast
	waitmovement 0
	goto EternaCity_EventScript_CynthiaGiveCutEnd
	end

@ Player at (16-18,10): she starts 9 tiles east on row 12 and ends below them.
EternaCity_EventScript_CynthiaGiveCutFromBelow::
	copyvar VAR_0x8006, VAR_0x8004
	addvar VAR_0x8006, 9
	setobjectxyperm LOCALID_ETERNA_CITY_CYNTHIA, VAR_0x8006, 12
	addobject LOCALID_ETERNA_CITY_CYNTHIA
	applymovement LOCALID_ETERNA_CITY_CYNTHIA, EternaCity_Movement_CynthiaNoticeAndWalkUpToPlayerBelow
	waitmovement 0
	applymovement LOCALID_PLAYER, Common_Movement_WalkInPlaceDown
	waitmovement 0
	msgbox EternaCity_Text_OhYouHaveAPokedex, MSGBOX_DEFAULT
	closemessage
	applymovement LOCALID_ETERNA_CITY_CYNTHIA, EternaCity_Movement_CynthiaLookAroundThenNorth
	waitmovement 0
	call EternaCity_EventScript_CynthiaGiveHM01
	goto_if_eq VAR_0x8004, 17, EternaCity_EventScript_CynthiaLeaveAfterCutX17
	goto_if_eq VAR_0x8004, 18, EternaCity_EventScript_CynthiaLeaveAfterCutX18
	applymovement LOCALID_ETERNA_CITY_CYNTHIA, EternaCity_Movement_CynthiaLeaveAfterCutX16
	applymovement LOCALID_PLAYER, EternaCity_Movement_PlayerWatchCynthiaLeaveAfterCutBelow
	waitmovement 0
	goto EternaCity_EventScript_CynthiaGiveCutEnd
	end

EternaCity_EventScript_CynthiaLeaveAfterCutX17::
	applymovement LOCALID_ETERNA_CITY_CYNTHIA, EternaCity_Movement_CynthiaLeaveAfterCutX17
	applymovement LOCALID_PLAYER, EternaCity_Movement_PlayerWatchCynthiaLeaveAfterCutBelow
	waitmovement 0
	goto EternaCity_EventScript_CynthiaGiveCutEnd
	end

EternaCity_EventScript_CynthiaLeaveAfterCutX18::
	applymovement LOCALID_ETERNA_CITY_CYNTHIA, EternaCity_Movement_CynthiaLeaveAfterCutX18
	applymovement LOCALID_PLAYER, EternaCity_Movement_PlayerWatchCynthiaLeaveAfterCutBelow
	waitmovement 0
	goto EternaCity_EventScript_CynthiaGiveCutEnd
	end

EternaCity_EventScript_CynthiaGiveHM01::
	msgbox EternaCity_Text_TryUsingThis, MSGBOX_DEFAULT
	giveitem ITEM_HM01
	msgbox EternaCity_Text_CutCanBeUsedInField, MSGBOX_DEFAULT
	closemessage
	return

EternaCity_EventScript_CynthiaGiveCutEnd::
	removeobject LOCALID_ETERNA_CITY_CYNTHIA
	setvar VAR_ETERNA_CITY_STATE, 2
	releaseall
	end

EternaCity_Movement_CynthiaNoticeAndWalkUpToPlayerBelow:
	walk_left
	walk_left
	walk_left
	walk_left
	emote_exclamation_mark
	walk_left
	walk_left
	walk_left
	walk_left
	walk_left
	walk_up
	step_end

EternaCity_Movement_CynthiaNoticeAndWalkUpToPlayerEast:
	walk_left
	walk_left
	walk_left
	walk_left
	emote_exclamation_mark
	walk_left
	walk_left
	walk_left
	walk_left
	step_end

EternaCity_Movement_CynthiaLookAroundThenNorth:
	walk_in_place_right
	delay_16
	delay_16
	walk_in_place_up
	step_end

EternaCity_Movement_CynthiaLookAroundThenWest:
	walk_in_place_right
	delay_16
	delay_16
	walk_in_place_left
	step_end

EternaCity_Movement_CynthiaLeaveAfterCutX16:
	walk_left
	walk_left
	walk_left
	walk_left
	walk_left
	walk_down
	walk_down
	walk_down
	walk_down
	walk_down
	walk_down
	walk_down
	walk_down
	walk_down
	walk_down
	step_end

EternaCity_Movement_CynthiaLeaveAfterCutX17:
	walk_left
	walk_left
	walk_left
	walk_left
	walk_left
	walk_left
	walk_down
	walk_down
	walk_down
	walk_down
	walk_down
	walk_down
	walk_down
	walk_down
	walk_down
	walk_down
	step_end

EternaCity_Movement_CynthiaLeaveAfterCutX18:
	walk_left
	walk_left
	walk_left
	walk_left
	walk_left
	walk_left
	walk_left
	walk_down
	walk_down
	walk_down
	walk_down
	walk_down
	walk_down
	walk_down
	walk_down
	walk_down
	walk_down
	step_end

EternaCity_Movement_CynthiaLeaveAfterCutY11Or12:
	walk_down
	walk_left
	walk_left
	walk_left
	walk_left
	walk_left
	walk_left
	walk_down
	walk_down
	walk_down
	walk_down
	walk_down
	walk_down
	walk_down
	walk_down
	walk_down
	walk_down
	step_end

EternaCity_Movement_CynthiaLeaveAfterCutY13:
	walk_up
	walk_left
	walk_left
	walk_left
	walk_left
	walk_left
	walk_left
	walk_down
	walk_down
	walk_down
	walk_down
	walk_down
	walk_down
	walk_down
	walk_down
	walk_down
	walk_down
	walk_down
	step_end

EternaCity_Movement_PlayerWatchCynthiaLeaveAfterCutBelow:
	delay_16
	walk_in_place_left
	step_end

EternaCity_Movement_PlayerWatchCynthiaLeaveAfterCutEast:
	delay_16
	delay_16
	walk_in_place_left
	step_end

@ Crossing (20,29)-(20,33): Cynthia comes up column 16 from 8 rows below
@ and along the player's row to stand just west of them, at (19,y).
EternaCity_EventScript_CynthiaTryGiveEgg::
	lockall
	getplayerxy VAR_0x8004, VAR_0x8005
	copyvar VAR_0x8006, VAR_0x8005
	addvar VAR_0x8006, 8
	clearflag FLAG_HIDE_ETERNA_CITY_CYNTHIA
	setobjectxyperm LOCALID_ETERNA_CITY_CYNTHIA, 16, VAR_0x8006
	setobjectmovementtype LOCALID_ETERNA_CITY_CYNTHIA, MOVEMENT_TYPE_FACE_RIGHT
	addobject LOCALID_ETERNA_CITY_CYNTHIA
	applymovement LOCALID_ETERNA_CITY_CYNTHIA, EternaCity_Movement_CynthiaNoticeAndWalkUpToPlayer
	waitmovement 0
	applymovement LOCALID_PLAYER, Common_Movement_WalkInPlaceLeft
	waitmovement 0
	msgbox EternaCity_Text_WillYouAcceptEgg, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, EternaCity_EventScript_CynthiaEggRefused
	getpartysize
	goto_if_eq VAR_RESULT, PARTY_SIZE, EternaCity_EventScript_CynthiaNoRoomForEgg
	call EternaCity_EventScript_GiveTogepiEgg
	applymovement LOCALID_ETERNA_CITY_CYNTHIA, EternaCity_Movement_CynthiaLeaveWest
	waitmovement 0
	removeobject LOCALID_ETERNA_CITY_CYNTHIA
	releaseall
	end

EternaCity_EventScript_GiveTogepiEgg::
	playfanfare MUS_OBTAIN_ITEM
	message EternaCity_Text_ReceivedTheEgg
	waitfanfare
	giveegg SPECIES_TOGEPI
	setvar VAR_ETERNA_CITY_STATE, 5
	msgbox EternaCity_Text_ThatsWonderful, MSGBOX_DEFAULT
	msgbox EternaCity_Text_KeepEggInYourParty, MSGBOX_DEFAULT
	closemessage
	return

EternaCity_EventScript_CynthiaEggRefused::
	msgbox EternaCity_Text_DontFeelObligated, MSGBOX_DEFAULT
	closemessage
	setvar VAR_ETERNA_CITY_STATE, 4
	releaseall
	end

EternaCity_EventScript_CynthiaNoRoomForEgg::
	msgbox EternaCity_Text_NoRoomForEgg, MSGBOX_DEFAULT
	closemessage
	setvar VAR_ETERNA_CITY_STATE, 4
	releaseall
	end

@ Talking to her while she waits (state 4).
EternaCity_EventScript_Cynthia::
	lockall
	faceplayer
	msgbox EternaCity_Text_WillYouAcceptThisEgg, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, EternaCity_EventScript_CynthiaEggRefused
	getpartysize
	goto_if_eq VAR_RESULT, PARTY_SIZE, EternaCity_EventScript_CynthiaNoRoomForEgg
	call EternaCity_EventScript_GiveTogepiEgg
	goto_if_eq VAR_FACING, DIR_EAST, EternaCity_EventScript_CynthiaLeaveAroundPlayer
	goto_if_eq VAR_FACING, DIR_WEST, EternaCity_EventScript_CynthiaLeaveWest
	applymovement LOCALID_ETERNA_CITY_CYNTHIA, EternaCity_Movement_CynthiaLeaveWest
	applymovement LOCALID_PLAYER, EternaCity_Movement_PlayerWatchCynthiaLeave
	waitmovement 0
	goto EternaCity_EventScript_RemoveCynthia
	end

EternaCity_EventScript_CynthiaLeaveWest::
	applymovement LOCALID_ETERNA_CITY_CYNTHIA, EternaCity_Movement_CynthiaLeaveWest
	waitmovement 0
	goto EternaCity_EventScript_RemoveCynthia
	end

@ The player is west of her, in her way: she steps down first.
EternaCity_EventScript_CynthiaLeaveAroundPlayer::
	applymovement LOCALID_ETERNA_CITY_CYNTHIA, EternaCity_Movement_CynthiaLeaveAroundPlayer
	applymovement LOCALID_PLAYER, EternaCity_Movement_PlayerWatchCynthiaLeaveAround
	waitmovement 0
	goto EternaCity_EventScript_RemoveCynthia
	end

EternaCity_EventScript_RemoveCynthia::
	removeobject LOCALID_ETERNA_CITY_CYNTHIA
	releaseall
	end

@ Crossing (21,28)-(21,33) while she still has the Egg.
EternaCity_EventScript_CynthiaBlockCycleShop::
	lockall
	applymovement LOCALID_ETERNA_CITY_CYNTHIA, Common_Movement_WalkInPlaceRight
	waitmovement 0
	msgbox EternaCity_Text_DontLeaveWithoutEgg, MSGBOX_DEFAULT
	closemessage
	applymovement LOCALID_PLAYER, EternaCity_Movement_PlayerStepBackWest
	waitmovement 0
	releaseall
	end

EternaCity_Movement_CynthiaNoticeAndWalkUpToPlayer:
	walk_up
	walk_up
	walk_up
	emote_exclamation_mark
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	walk_right
	walk_right
	walk_right
	step_end

EternaCity_Movement_CynthiaLeaveWest:
	walk_left
	walk_left
	walk_left
	walk_down
	walk_down
	walk_down
	walk_down
	walk_down
	walk_down
	walk_down
	walk_down
	walk_down
	step_end

EternaCity_Movement_CynthiaLeaveAroundPlayer:
	walk_down
	walk_left
	walk_left
	walk_left
	walk_down
	walk_down
	walk_down
	walk_down
	walk_down
	walk_down
	walk_down
	walk_down
	step_end

EternaCity_Movement_PlayerStepBackWest:
	walk_left
	step_end

EternaCity_Movement_PlayerWatchCynthiaLeave:
	delay_8
	walk_in_place_left
	step_end

EternaCity_Movement_PlayerWatchCynthiaLeaveAround:
	delay_8
	delay_8
	delay_4
	walk_in_place_left
	step_end

EternaCity_Text_WallyPokenavCall:
	.string "… … … … … …\n"
	.string "… … … … … Beep!\p"
	.string "WALLY: Oh, hello, {PLAYER}!\p"
	.string "I just wanted to tell you that\n"
	.string "my uncle bought me a POKéTCH!\p"
	.string "Now I can get in touch with you\n"
	.string "anytime, {PLAYER}!\p"
	.string "… … … … … …\n"
	.string "… … … … … Click!$"

EternaCity_Text_RegisteredWally:
	.string "Registered WALLY\n"
	.string "in the POKéTCH.$"

EternaCity_Text_StayAwayFromStatue:
	.string "Hey! You there, kid!\n"
	.string "Stay away from that POKéMON statue!\p"
	.string "TEAM GALACTIC is conducting an\n"
	.string "official investigation here!$"

EternaCity_Text_IllLetYouKeepPokemon:
	.string "Hey, you! Yeah, you, TRAINER!\p"
	.string "Errr…\n"
	.string "It's OK. Never mind. You look tough.\p"
	.string "I won't take… Um, I'll just let you\n"
	.string "keep your POKéMON. Carry on!$"

EternaCity_Text_HaveYouSeenOurBuilding:
	.string "Have you seen our building?\p"
	.string "TEAM GALACTIC is so stinking rich\n"
	.string "that we can have one built instantly!$"

EternaCity_Text_GetYourselfABicycle:
	.string "If you're visiting ETERNA, you need to\n"
	.string "get yourself a BICYCLE and take a\l"
	.string "ride down the CYCLING ROAD.$"

EternaCity_Text_AtmosphereHasGottenCreepy:
	.string "Since that TEAM GALACTIC building went\n"
	.string "up, the city's atmosphere has gotten\l"
	.string "all creepy and tense.\p"
	.string "I don't know what those people are\n"
	.string "doing up in that place…$"

EternaCity_Text_WhatWillHappenToGalacticBuilding:
	.string "With TEAM GALACTIC gone, what's going\n"
	.string "to happen to their building?$"

EternaCity_Text_GalacticTookFavoritePokemon:
	.string "My friend said TEAM GALACTIC took his\n"
	.string "favorite POKéMON away.\p"
	.string "I don't think he's telling the truth.\n"
	.string "TEAM GALACTIC looks so cool.\l"
	.string "They don't look like common thugs.$"

EternaCity_Text_DoYouUseKickstand:
	.string "Do you use your BIKE's kickstand?\p"
	.string "It looks cool to hop off your BIKE and\n"
	.string "then snap it into place in one motion!$"

EternaCity_Text_TheySayThisIsAnOldCity:
	.string "They say this is an old city.\p"
	.string "It doesn't look it, though.\n"
	.string "Not with all these tall buildings.$"

EternaCity_Text_EternaCitysNotLikeBefore:
	.string "ETERNA CITY's not anything like it\n"
	.string "was before, way back in the day.$"

EternaCity_Text_AwesomePokemonFromLongAgo:
	.string "They say this is an awesome POKéMON\n"
	.string "that people saw long ago.$"

EternaCity_Text_YouDontHaveExplorerKit:
	.string "Oh, don't tell me you don't have\n"
	.string "an EXPLORER KIT?\p"
	.string "You should have a word with the man\n"
	.string "next door to the POKéMON CENTER.$"

EternaCity_Text_YouGotABicycle:
	.string "Oh, wow!\n"
	.string "You got a BICYCLE!\p"
	.string "You can ride down CYCLING ROAD\n"
	.string "and end up in HEARTHOME CITY.\p"
	.string "It's a fast and fun ride.\n"
	.string "You should check it out!$"

EternaCity_Text_FeelsGreatRidingABike:
	.string "It feels great riding a BIKE with the\n"
	.string "wind in your face, doesn't it?$"

EternaCity_Text_ItsEternaCitysPokemonStatue:
	.string "It's ETERNA CITY's POKéMON statue.\p"
	.string "There seems to have been a descriptive\n"
	.string "plate, but it appears it was torn off.$"

EternaCity_Text_CitySign:
	.string "ETERNA CITY\n"
	.string "“History Living”$"

EternaCity_Text_GymSign:
	.string "ETERNA CITY POKéMON GYM\n"
	.string "LEADER: GARDENIA\p"
	.string "“Master of Vivid Plant POKéMON!”$"

EternaCity_Text_CycleShopSign:
	.string "RAD RICKSHAW'S CYCLE SHOP\n"
	.string "Get On and Ride!$"

EternaCity_Text_GalacticBuildingSign:
	.string "TEAM GALACTIC ETERNA BUILDING\n"
	.string "We Want Your POKéMON!$"

EternaCity_Text_UndergroundMansHouseSign:
	.string "UNDERGROUND MAN'S HOUSE\n"
	.string "Goin' Down for Adventure!$"

EternaCity_Text_CondominiumsSign:
	.string "ETERNA CONDOMINIUMS\n"
	.string "Name-Rating Services Available$"

EternaCity_Text_WillYouAcceptEgg:
	.string "CYNTHIA: Oh, there you are!\p"
	.string "I was looking for you.\n"
	.string "I've got something nice.\p"
	.string "I wanted you to have this POKéMON EGG.\n"
	.string "Will you accept it?$"

EternaCity_Text_ReceivedTheEgg:
	.string "{PLAYER} received the EGG.$"

EternaCity_Text_ThatsWonderful:
	.string "CYNTHIA: That's wonderful!\p"
	.string "The POKéMON inside the EGG is\n"
	.string "happy, too. I'm sure of it!$"

EternaCity_Text_KeepEggInYourParty:
	.string "CYNTHIA: Keep that EGG with you in your\n"
	.string "party of POKéMON.\p"
	.string "A POKéMON will hatch from it while you\n"
	.string "are traveling.\p"
	.string "I would be happy to know that it will\n"
	.string "help fill another page of your POKéDEX.\p"
	.string "See you again!$"

EternaCity_Text_NoRoomForEgg:
	.string "CYNTHIA: Oh?\n"
	.string "You have six POKéMON with you already.\l"
	.string "You don't have room for this EGG.\p"
	.string "You'll have to store a POKéMON in a\n"
	.string "PC BOX at the POKéMON CENTER first.\p"
	.string "I'll give you this EGG when you've made\n"
	.string "room for it.\p"
	.string "I'll be waiting right here.$"

EternaCity_Text_DontFeelObligated:
	.string "CYNTHIA: Oh…?\n"
	.string "You don't need to feel obligated.\p"
	.string "It's hard filling the pages of the\n"
	.string "POKéDEX, isn't it?\p"
	.string "I only want to lend you a little hand,\n"
	.string "that's all.$"

EternaCity_Text_WillYouAcceptThisEgg:
	.string "CYNTHIA: Well?\n"
	.string "Will you accept this POKéMON EGG?$"

EternaCity_Text_DontLeaveWithoutEgg:
	.string "CYNTHIA: Oh, wait!\p"
	.string "Don't leave without this POKéMON EGG.\n"
	.string "Please? I want you to have it.$"

EternaCity_Text_BigThud:
	.string "Thud!!$"

EternaCity_Text_YouCameToSeeStatue:
	.string "{RIVAL}: Hey, {PLAYER}!\p"
	.string "You came to see the POKéMON statue,\n"
	.string "right? I'll take you there!$"

EternaCity_Text_WhatThe:
	.string "{RIVAL}: What the…$"

EternaCity_Text_SoThisIsEternasStatue:
	.string "CYRUS: …So, this is ETERNA's POKéMON\n"
	.string "statue…\p"
	.string "Shaping our world are time and space\n"
	.string "in an intertwining spiral.\p"
	.string "Revered in SINNOH are the POKéMON of\n"
	.string "time and space…\p"
	.string "SINNOH's myth…\n"
	.string "Its truth needs investigation…$"

EternaCity_Text_PardonMeStandAside:
	.string "…Pardon me.\n"
	.string "Stand aside.$"

EternaCity_Text_GoCheckOutStatueSeeYou:
	.string "{RIVAL}: Oh, hey!\p"
	.string "When we smacked together, a brilliant\n"
	.string "flash of inspiration came to me!\p"
	.string "An easy way to become a great TRAINER!\n"
	.string "Yeah, that!\p"
	.string "All right.\n"
	.string "Listen close…\p"
	.string "Make certain all your attacks hit!\n"
	.string "Avoid enemy attacks for sure!\p"
	.string "You do that, you'd never lose!\n"
	.string "You'd be the invincible TRAINER!\p"
	.string "Anyway, go check out the POKéMON\n"
	.string "statue. See you around!$"

EternaCity_Text_OhYouHaveAPokedex:
	.string "Oh, that device you have…\p"
	.string "It is a POKéDEX?\n"
	.string "Wow, it brings back some memories.\p"
	.string "Pardon me, but what's your name?\p"
	.string "…\p"
	.string "OK, hello, {PLAYER}.\n"
	.string "I'll be sure to remember that name!\p"
	.string "My name is CYNTHIA.\n"
	.string "I'm a TRAINER just like you.\p"
	.string "I've been studying POKéMON mythology\n"
	.string "lately, just out of curiosity.$"

EternaCity_Text_TryUsingThis:
	.string "Here in ETERNA CITY, there is a statue\n"
	.string "of an ancient POKéMON.\p"
	.string "Have you seen it already?\p"
	.string "According to myth, it was an extremely\n"
	.string "powerful POKéMON.\p"
	.string "Who knows? You may encounter\n"
	.string "something like it while you're\l"
	.string "traveling with your POKéDEX.\p"
	.string "Oh, I know! Try using this.\n"
	.string "It should be helpful.$"

EternaCity_Text_CutCanBeUsedInField:
	.string "CYNTHIA: Remember, the hidden move\n"
	.string "CUT can be used in the field.\p"
	.string "It will let you go places that were\n"
	.string "previously inaccessible to you.\p"
	.string "That's important for filling up your\n"
	.string "POKéDEX, right?\p"
	.string "Then luck be with you, young TRAINER!\p"
	.string "Oh, there's one last thing!\n"
	.string "Please give my regards to PROF. BIRCH.$"
