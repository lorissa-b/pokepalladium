@ Jubilife City, north of Route 202. Drawn over Emerald's Rustboro City.
JubilifeCity_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, JubilifeCity_OnTransition
	map_script MAP_SCRIPT_ON_FRAME_TABLE, JubilifeCity_OnFrame
	.byte 0

JubilifeCity_OnTransition:
	setflag FLAG_VISITED_JUBILIFE_CITY
	call Common_EventScript_SetupRivalGfxId
	call_if_eq VAR_JUBILIFE_CITY_STATE, 6, JubilifeCity_EventScript_HideMapNamePopup
	getplayerxy VAR_TEMP_0, VAR_TEMP_1
	goto_if_eq VAR_JUBILIFE_CITY_STATE, 6, JubilifeCity_EventScript_PositionScientistForExit
	end

JubilifeCity_EventScript_PositionScientistForExit::
	goto_if_eq VAR_TEMP_0, 11, JubilifeCity_EventScript_PositionScientistLeftExit
	setobjectxyperm LOCALID_JUBILIFE_SCIENTIST, 12, 15
	end

JubilifeCity_EventScript_PositionScientistLeftExit::
	setobjectxyperm LOCALID_JUBILIFE_SCIENTIST, 11, 15
	end

JubilifeCity_EventScript_HideMapNamePopup::
	setflag FLAG_HIDE_MAP_NAME_POPUP
	return

JubilifeCity_OnFrame:
	map_script_2 VAR_JUBILIFE_CITY_STATE, 6, JubilifeCity_EventScript_ScientistAddMatchCall
	.2byte 0

JubilifeCity_EventScript_ScientistAddMatchCall::
	lockall
	setvar VAR_ROUTE104_STATE, 1
	applywaitmovement LOCALID_PLAYER, JubilifeCity_Movement_PlayerWalkDown
	playse SE_EXIT
	delay 10
	addobject LOCALID_JUBILIFE_SCIENTIST
	applywaitmovement LOCALID_JUBILIFE_SCIENTIST, JubilifeCity_Movement_ScientistApproachPlayer
	playse SE_PIN
	applywaitmovement LOCALID_JUBILIFE_SCIENTIST, Common_Movement_ExclamationMark
	applywaitmovement LOCALID_JUBILIFE_SCIENTIST, Common_Movement_Delay48
	applywaitmovement LOCALID_PLAYER, Common_Movement_WalkInPlaceFasterUp
	applywaitmovement LOCALID_JUBILIFE_SCIENTIST, JubilifeCity_Movement_ScientistWalkInPlaceDown
	msgbox JubilifeCity_Text_DevelopedNewPokenavFeature, MSGBOX_DEFAULT
	closemessage
	setflag FLAG_HAS_MATCH_CALL
	applywaitmovement LOCALID_JUBILIFE_SCIENTIST, Common_Movement_WalkInPlaceFasterLeft
	playse SE_CLICK
	delay 10
	playse SE_CLICK
	delay 10
	playse SE_CLICK
	delay 10
	playse SE_CLICK
	delay 20
	applywaitmovement LOCALID_JUBILIFE_SCIENTIST, Common_Movement_WalkInPlaceFasterDown
	msgbox JubilifeCity_Text_AddedMatchCallPleaseCallMrStone, MSGBOX_DEFAULT
	closemessage
	delay 20
	goto JubilifeCity_EventScript_MatchCallTutorial

JubilifeCity_EventScript_PleaseSelectPokenav::
	msgbox JubilifeCity_Text_PleaseSelectPokenav, MSGBOX_DEFAULT
	closemessage
	delay 10
	goto JubilifeCity_EventScript_MatchCallTutorial

JubilifeCity_EventScript_MatchCallTutorial::
	setflag FLAG_ADDED_MATCH_CALL_TO_POKENAV
	special ScriptMenu_CreateStartMenuForPokenavTutorial
	switch VAR_RESULT
	case 0, JubilifeCity_EventScript_PleaseSelectPokenav
	case 1, JubilifeCity_EventScript_PleaseSelectPokenav
	case 2, JubilifeCity_EventScript_PleaseSelectPokenav
	case 4, JubilifeCity_EventScript_PleaseSelectPokenav
	case 5, JubilifeCity_EventScript_PleaseSelectPokenav
	case 6, JubilifeCity_EventScript_PleaseSelectPokenav
	case 7, JubilifeCity_EventScript_PleaseSelectPokenav
	case MULTI_B_PRESSED, JubilifeCity_EventScript_PleaseSelectPokenav
	special OpenPokenavForTutorial
	delay 20
	msgbox JubilifeCity_Text_IdBetterGetBackToWork, MSGBOX_DEFAULT
	closemessage
	applywaitmovement LOCALID_JUBILIFE_SCIENTIST, JubilifeCity_Movement_ScientistLeave
	playse SE_EXIT
	removeobject LOCALID_JUBILIFE_SCIENTIST
	setflag FLAG_HIDE_JUBILIFE_CITY_SCIENTIST
	setvar VAR_JUBILIFE_CITY_STATE, 7
	clearflag FLAG_HIDE_MAP_NAME_POPUP
	releaseall
	end

JubilifeCity_Movement_ScientistWalkInPlaceDown:
	walk_in_place_down
	step_end

JubilifeCity_Movement_PlayerWalkDown:
	walk_down
	step_end

JubilifeCity_Movement_ScientistApproachPlayer:
	walk_down
	delay_16
	step_end

@ Unused
JubilifeCity_Movement_ScientistWalkAroundPlayer:
	delay_16
	walk_left
	walk_down
	walk_down
	walk_right
	walk_in_place_faster_up
	delay_16
	step_end

JubilifeCity_Movement_ScientistLeave:
	walk_up
	step_end

JubilifeCity_EventScript_FatMan::
	lock
	faceplayer
	goto_if_set FLAG_DEVON_GOODS_STOLEN, JubilifeCity_EventScript_FatManSawGrunt
	msgbox JubilifeCity_Text_WeShortenItToDevon, MSGBOX_DEFAULT
	release
	end

JubilifeCity_EventScript_FatManSawGrunt::
	msgbox JubilifeCity_Text_SneakyLookingManWentAroundCorner, MSGBOX_DEFAULT
	release
	end

JubilifeCity_EventScript_DevonEmployee2::
	lock
	faceplayer
	msgbox JubilifeCity_Text_YoureNewAroundHere, MSGBOX_DEFAULT
	release
	end

JubilifeCity_EventScript_Woman::
	msgbox JubilifeCity_Text_GymLeaderIsntEasyWithFire, MSGBOX_NPC
	end

JubilifeCity_EventScript_Man1::
	lock
	faceplayer
	goto_if_set FLAG_BADGE01_GET, JubilifeCity_EventScript_Man1HaveBadge
	msgbox JubilifeCity_Text_HaveYouChallengedGym, MSGBOX_DEFAULT
	release
	end

JubilifeCity_EventScript_Man1HaveBadge::
	msgbox JubilifeCity_Text_HeyThatsJubilifesGymBadge, MSGBOX_DEFAULT
	release
	end

JubilifeCity_EventScript_Boy2::
	lock
	faceplayer
	goto_if_set FLAG_RECEIVED_POKENAV, JubilifeCity_EventScript_Boy2BrineyLeftTunnel
	msgbox JubilifeCity_Text_MrBrineyWalksInTheTunnel, MSGBOX_DEFAULT
	release
	end

JubilifeCity_EventScript_Boy2BrineyLeftTunnel::
	msgbox JubilifeCity_Text_MrBrineyLovesPeeko, MSGBOX_DEFAULT
	release
	end

JubilifeCity_EventScript_Twin::
	msgbox JubilifeCity_Text_WowYouHavePokemon, MSGBOX_NPC
	end

JubilifeCity_EventScript_NinjaBoy::
	msgbox JubilifeCity_Text_CatchRarePokemonIfIGoToSchool, MSGBOX_SIGN
	end

JubilifeCity_EventScript_TunnelSign::
	msgbox JubilifeCity_Text_TunnelNearingCompletion, MSGBOX_SIGN
	end

JubilifeCity_EventScript_PoketchCompanySign::
	msgbox JubilifeCity_Text_PoketchCompanySign, MSGBOX_SIGN
	end

JubilifeCity_EventScript_JubilifeTVSign::
	msgbox JubilifeCity_Text_JubilifeTVSign, MSGBOX_SIGN
	end

JubilifeCity_EventScript_GymSign::
	msgbox JubilifeCity_Text_GymSign, MSGBOX_SIGN
	end

@ Unused
JubilifeCity_EventScript_DevonCorpBranchOfficeSign::
	msgbox JubilifeCity_Text_DevonCorpBranchOfficeSign, MSGBOX_SIGN
	end

JubilifeCity_EventScript_CitySign::
	msgbox JubilifeCity_Text_CitySign, MSGBOX_SIGN
	end

JubilifeCity_EventScript_TrainersSchoolSign::
	msgbox JubilifeCity_Text_TrainersSchoolSign, MSGBOX_SIGN
	end

JubilifeCity_EventScript_CuttersHouseSign::
	msgbox JubilifeCity_Text_CuttersHouse, MSGBOX_SIGN
	end

JubilifeCity_EventScript_LittleBoy::
	lock
	faceplayer
	msgbox JubilifeCity_Text_PokemonCanChangeLookFromExp, MSGBOX_DEFAULT
	applywaitmovement LOCALID_JUBILIFE_LITTLE_BOY, Common_Movement_FaceOriginalDirection
	release
	end

JubilifeCity_EventScript_LittleGirl::
	lock
	faceplayer
	msgbox JubilifeCity_Text_PokemonChangeShape, MSGBOX_DEFAULT
	applywaitmovement LOCALID_JUBILIFE_LITTLE_GIRL, Common_Movement_FaceOriginalDirection
	release
	end

JubilifeCity_EventScript_Man2::
	lock
	faceplayer
	msgbox JubilifeCity_Text_TradePokemonGrowFast, MSGBOX_DEFAULT
	release
	end

JubilifeCity_EventScript_StolenGoodsTrigger0::
	lockall
	setobjectxyperm LOCALID_JUBILIFE_DEVON_EMPLOYEE, 14, 21
	setobjectmovementtype LOCALID_JUBILIFE_DEVON_EMPLOYEE, MOVEMENT_TYPE_FACE_RIGHT
	setvar VAR_0x8004, 0
	goto JubilifeCity_EventScript_StolenGoodsScene
	end

JubilifeCity_EventScript_StolenGoodsTrigger1::
	lockall
	setobjectxyperm LOCALID_JUBILIFE_DEVON_EMPLOYEE, 14, 21
	setobjectmovementtype LOCALID_JUBILIFE_DEVON_EMPLOYEE, MOVEMENT_TYPE_FACE_RIGHT
	setvar VAR_0x8004, 1
	goto JubilifeCity_EventScript_StolenGoodsScene
	end

JubilifeCity_EventScript_StolenGoodsTrigger2::
	lockall
	setobjectxyperm LOCALID_JUBILIFE_DEVON_EMPLOYEE, 14, 21
	setobjectmovementtype LOCALID_JUBILIFE_DEVON_EMPLOYEE, MOVEMENT_TYPE_FACE_RIGHT
	setvar VAR_0x8004, 2
	goto JubilifeCity_EventScript_StolenGoodsScene
	end

JubilifeCity_EventScript_StolenGoodsTrigger3::
	lockall
	setobjectxyperm LOCALID_JUBILIFE_DEVON_EMPLOYEE, 14, 21
	setobjectmovementtype LOCALID_JUBILIFE_DEVON_EMPLOYEE, MOVEMENT_TYPE_FACE_RIGHT
	setvar VAR_0x8004, 3
	goto JubilifeCity_EventScript_StolenGoodsScene
	end

JubilifeCity_EventScript_StolenGoodsTrigger4::
	lockall
	setobjectxyperm LOCALID_JUBILIFE_DEVON_EMPLOYEE, 14, 21
	setobjectmovementtype LOCALID_JUBILIFE_DEVON_EMPLOYEE, MOVEMENT_TYPE_FACE_RIGHT
	setvar VAR_0x8004, 4
	goto JubilifeCity_EventScript_StolenGoodsScene
	end

JubilifeCity_EventScript_StolenGoodsScene::
	msgbox JubilifeCity_Text_OutOfTheWay, MSGBOX_DEFAULT
	closemessage
	playbgm MUS_ENCOUNTER_AQUA, FALSE
	addobject LOCALID_JUBILIFE_GRUNT
	addobject LOCALID_JUBILIFE_DEVON_EMPLOYEE
	applywaitmovement LOCALID_JUBILIFE_GRUNT, JubilifeCity_Movement_GruntEscape
	removeobject LOCALID_JUBILIFE_GRUNT
	applywaitmovement LOCALID_JUBILIFE_DEVON_EMPLOYEE, JubilifeCity_Movement_EmployeeChaseGrunt1
	msgbox JubilifeCity_Text_WaitDontTakeMyGoods, MSGBOX_DEFAULT
	closemessage
	applywaitmovement LOCALID_JUBILIFE_DEVON_EMPLOYEE, JubilifeCity_Movement_EmployeeChaseGrunt2
	fadedefaultbgm
	removeobject LOCALID_JUBILIFE_DEVON_EMPLOYEE
	setobjectxyperm LOCALID_JUBILIFE_DEVON_EMPLOYEE, 30, 10
	clearflag FLAG_HIDE_JUBILIFE_CITY_DEVON_EMPLOYEE_1
	setflag FLAG_DEVON_GOODS_STOLEN
	setvar VAR_JUBILIFE_CITY_STATE, 2
	setvar VAR_RUSTURF_TUNNEL_STATE, 2
	setvar VAR_ROUTE116_STATE, 1
	clearflag FLAG_HIDE_ROUTE_116_MR_BRINEY
	clearflag FLAG_HIDE_RUSTURF_TUNNEL_PEEKO
	clearflag FLAG_HIDE_RUSTURF_TUNNEL_AQUA_GRUNT
	setflag FLAG_HIDE_BRINEYS_HOUSE_MR_BRINEY
	setflag FLAG_HIDE_BRINEYS_HOUSE_PEEKO
	releaseall
	end

@ Unused
JubilifeCity_EventScript_ShadyCharacterTookOff::
	msgbox JubilifeCity_Text_ShadyCharacterTookOffTowardsTunnel, MSGBOX_DEFAULT
	return

@ Unused
JubilifeCity_EventScript_YouGotItThankYou::
	msgbox JubilifeCity_Text_YouGotItThankYou, MSGBOX_DEFAULT
	return

@ Unknown, unused employee movements. Based on the differences in movement theyre for approaching an object (probably the player) in different positions
JubilifeCity_EventScript_EmployeeApproachUp::
	applywaitmovement LOCALID_JUBILIFE_DEVON_EMPLOYEE, JubilifeCity_Movement_EmployeeApproachUp
	return

@ Unused, see above
JubilifeCity_EventScript_EmployeeApproachLeft::
	applywaitmovement LOCALID_JUBILIFE_DEVON_EMPLOYEE, JubilifeCity_Movement_EmployeeApproachLeft
	return

@ Unused, see above
JubilifeCity_EventScript_EmployeeApproachRight::
	applywaitmovement LOCALID_JUBILIFE_DEVON_EMPLOYEE, JubilifeCity_Movement_EmployeeApproachRight
	return

@ Unused, see above
JubilifeCity_EventScript_EmployeeApproachDown::
	applywaitmovement LOCALID_JUBILIFE_DEVON_EMPLOYEE, JubilifeCity_Movement_EmployeeApproachDown
	return

@ Unused, similar movement to the above scripts
JubilifeCity_EventScript_EmployeeApproachPlayerFar::
	applywaitmovement LOCALID_JUBILIFE_DEVON_EMPLOYEE, JubilifeCity_Movement_EmployeeApproachPlayerFar
	applywaitmovement LOCALID_PLAYER, Common_Movement_WalkInPlaceFasterUp
	return

@ Unused
JubilifeCity_Movement_GruntEscapeExtended:
	walk_faster_right
	walk_faster_right
	walk_fast_right
	walk_fast_right
	walk_fast_right
	walk_fast_right
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
	walk_fast_up
	step_end

JubilifeCity_Movement_GruntEscape:
	walk_faster_right
	walk_faster_right
	walk_fast_right
	walk_fast_right
	walk_fast_right
	walk_fast_right
	walk_fast_right
	walk_fast_up
	walk_fast_up
	walk_fast_up
	walk_fast_up
	walk_fast_up
	walk_fast_up
	walk_fast_up
	walk_fast_up
	walk_fast_up
	step_end

JubilifeCity_Movement_EmployeeChaseGrunt1:
	walk_right
	walk_right
	walk_right
	walk_right
	walk_right
	walk_right
	walk_in_place_faster_up
	step_end

JubilifeCity_Movement_EmployeeChaseGrunt2:
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	step_end

@ Functionally unused
JubilifeCity_Movement_EmployeeApproachUp:
	walk_down
	walk_right
	walk_right
	step_end

@ Functionally unused
JubilifeCity_Movement_EmployeeApproachLeft:
	walk_down
	walk_down
	walk_right
	walk_right
	step_end

@ Functionally unused
JubilifeCity_Movement_EmployeeApproachRight:
	walk_down
	walk_down
	walk_right
	walk_right
	step_end

@ Functionally unused
JubilifeCity_Movement_EmployeeApproachDown:
	walk_down
	walk_down
	walk_down
	walk_right
	walk_right
	step_end

@ Functionally unused
JubilifeCity_Movement_EmployeeApproachPlayerFar:
	walk_down
	walk_down
	walk_down
	walk_right
	walk_right
	walk_right
	walk_in_place_faster_down
	step_end

JubilifeCity_EventScript_DevonEmployee1::
	lock
	faceplayer
	goto_if_set FLAG_RECOVERED_DEVON_GOODS, JubilifeCity_EventScript_ReturnGoodsSpokeToEmployee
	msgbox JubilifeCity_Text_ShadyCharacterTookOffTowardsTunnel, MSGBOX_DEFAULT
	release
	end

JubilifeCity_EventScript_ReturnGoodsSpokeToEmployee::
	waitse
	setvar VAR_TEMP_1, 4
	goto JubilifeCity_EventScript_ReturnGoods
	end

JubilifeCity_EventScript_HelpGetGoodsTrigger0::
	lockall
	setvar VAR_TEMP_1, 0
	goto JubilifeCity_EventScript_EmployeeAskToGetGoods
	end

JubilifeCity_EventScript_HelpGetGoodsTrigger1::
	lockall
	setvar VAR_TEMP_1, 1
	goto JubilifeCity_EventScript_EmployeeAskToGetGoods
	end

JubilifeCity_EventScript_HelpGetGoodsTrigger2::
	lockall
	setvar VAR_TEMP_1, 2
	goto JubilifeCity_EventScript_EmployeeAskToGetGoods
	end

JubilifeCity_EventScript_HelpGetGoodsTrigger3::
	lockall
	setvar VAR_TEMP_1, 3
	goto JubilifeCity_EventScript_EmployeeAskToGetGoods
	end

JubilifeCity_EventScript_EmployeeAskToGetGoods::
	call_if_eq VAR_TEMP_1, 0, JubilifeCity_EventScript_EmployeeFacePlayerUp1
	call_if_eq VAR_TEMP_1, 1, JubilifeCity_EventScript_EmployeeFacePlayerLeft1
	call_if_eq VAR_TEMP_1, 2, JubilifeCity_EventScript_EmployeeFacePlayerDown1
	call_if_eq VAR_TEMP_1, 3, JubilifeCity_EventScript_EmployeeApproachPlayerDown1
	setflag FLAG_INTERACTED_WITH_DEVON_EMPLOYEE_GOODS_STOLEN
	setvar VAR_JUBILIFE_CITY_STATE, 3
	copyobjectxytoperm LOCALID_JUBILIFE_DEVON_EMPLOYEE
	msgbox JubilifeCity_Text_HelpMeIWasRobbed, MSGBOX_DEFAULT
	releaseall
	end

@ The below movement scripts are either partially or fully duplicated by the movement scripts when the player returns the goods
JubilifeCity_EventScript_EmployeeFacePlayerUp1::
	applywaitmovement LOCALID_JUBILIFE_DEVON_EMPLOYEE, Common_Movement_WalkInPlaceFasterUp
	playse SE_PIN
	applywaitmovement LOCALID_JUBILIFE_DEVON_EMPLOYEE, Common_Movement_ExclamationMark
	applywaitmovement LOCALID_JUBILIFE_DEVON_EMPLOYEE, Common_Movement_Delay48
	applywaitmovement LOCALID_PLAYER, Common_Movement_WalkInPlaceFasterDown
	return

JubilifeCity_EventScript_EmployeeFacePlayerLeft1::
	applywaitmovement LOCALID_JUBILIFE_DEVON_EMPLOYEE, Common_Movement_WalkInPlaceFasterLeft
	playse SE_PIN
	applywaitmovement LOCALID_JUBILIFE_DEVON_EMPLOYEE, Common_Movement_ExclamationMark
	applywaitmovement LOCALID_JUBILIFE_DEVON_EMPLOYEE, Common_Movement_Delay48
	applywaitmovement LOCALID_PLAYER, Common_Movement_FaceRight
	return

JubilifeCity_EventScript_EmployeeFacePlayerDown1::
	applywaitmovement LOCALID_JUBILIFE_DEVON_EMPLOYEE, Common_Movement_WalkInPlaceFasterDown
	playse SE_PIN
	applywaitmovement LOCALID_JUBILIFE_DEVON_EMPLOYEE, Common_Movement_ExclamationMark
	applywaitmovement LOCALID_JUBILIFE_DEVON_EMPLOYEE, Common_Movement_Delay48
	applywaitmovement LOCALID_PLAYER, Common_Movement_WalkInPlaceFasterUp
	return

JubilifeCity_EventScript_EmployeeApproachPlayerDown1::
	applywaitmovement LOCALID_JUBILIFE_DEVON_EMPLOYEE, Common_Movement_WalkInPlaceFasterDown
	playse SE_PIN
	applywaitmovement LOCALID_JUBILIFE_DEVON_EMPLOYEE, Common_Movement_ExclamationMark
	applywaitmovement LOCALID_JUBILIFE_DEVON_EMPLOYEE, Common_Movement_Delay48
	applywaitmovement LOCALID_JUBILIFE_DEVON_EMPLOYEE, JubilifeCity_Movement_EmployeeApproachPlayerDown
	applywaitmovement LOCALID_PLAYER, Common_Movement_WalkInPlaceFasterUp
	return

JubilifeCity_Movement_EmployeeApproachPlayerDown:
	walk_down
	step_end

JubilifeCity_EventScript_ReturnGoodsTrigger0::
	lockall
	setvar VAR_TEMP_1, 0
	goto JubilifeCity_EventScript_ReturnGoods
	end

JubilifeCity_EventScript_ReturnGoodsTrigger1::
	lockall
	setvar VAR_TEMP_1, 1
	goto JubilifeCity_EventScript_ReturnGoods
	end

JubilifeCity_EventScript_ReturnGoodsTrigger2::
	lockall
	setvar VAR_TEMP_1, 2
	goto JubilifeCity_EventScript_ReturnGoods
	end

JubilifeCity_EventScript_ReturnGoodsTrigger3::
	lockall
	setvar VAR_TEMP_1, 3
	goto JubilifeCity_EventScript_ReturnGoods
	end

JubilifeCity_EventScript_ReturnGoods::
	call_if_eq VAR_TEMP_1, 0, JubilifeCity_EventScript_EmployeeFacePlayerUp2
	call_if_eq VAR_TEMP_1, 1, JubilifeCity_EventScript_EmployeeFacePlayerLeft2
	call_if_eq VAR_TEMP_1, 2, JubilifeCity_EventScript_EmployeeFacePlayerDown2
	call_if_eq VAR_TEMP_1, 3, JubilifeCity_EventScript_EmployeeApproachPlayerDown2
	call_if_eq VAR_TEMP_1, 4, JubilifeCity_EventScript_EmployeeFacePlayerRight
	msgbox JubilifeCity_Text_YouGotItThankYou, MSGBOX_DEFAULT
	giveitem ITEM_GREAT_BALL
	call_if_eq VAR_RESULT, FALSE, JubilifeCity_EventScript_BagFull
	msgbox JubilifeCity_Text_PleaseComeWithMe, MSGBOX_DEFAULT
	closemessage
	setflag FLAG_RETURNED_DEVON_GOODS
	setflag FLAG_HIDE_JUBILIFE_CITY_DEVON_EMPLOYEE_1
	setvar VAR_JUBILIFE_CITY_STATE, 5
	delay 30
	warp MAP_JUBILIFE_CITY_POKETCH_COMPANY_3F, 2, 2
	waitstate
	releaseall
	end

JubilifeCity_EventScript_BagFull::
	msgbox JubilifeCity_Text_YoureLoadedWithItems, MSGBOX_DEFAULT
	return

JubilifeCity_EventScript_EmployeeFacePlayerUp2::
	applywaitmovement LOCALID_JUBILIFE_DEVON_EMPLOYEE, Common_Movement_WalkInPlaceFasterUp
	playse SE_PIN
	applywaitmovement LOCALID_JUBILIFE_DEVON_EMPLOYEE, Common_Movement_ExclamationMark
	applywaitmovement LOCALID_JUBILIFE_DEVON_EMPLOYEE, Common_Movement_Delay48
	applywaitmovement LOCALID_PLAYER, Common_Movement_WalkInPlaceFasterDown
	return

JubilifeCity_EventScript_EmployeeFacePlayerLeft2::
	applywaitmovement LOCALID_JUBILIFE_DEVON_EMPLOYEE, Common_Movement_WalkInPlaceFasterRight
	playse SE_PIN
	applywaitmovement LOCALID_JUBILIFE_DEVON_EMPLOYEE, Common_Movement_ExclamationMark
	applywaitmovement LOCALID_JUBILIFE_DEVON_EMPLOYEE, Common_Movement_Delay48
	applywaitmovement LOCALID_PLAYER, Common_Movement_FaceLeft
	return

JubilifeCity_EventScript_EmployeeFacePlayerDown2::
	applywaitmovement LOCALID_JUBILIFE_DEVON_EMPLOYEE, Common_Movement_WalkInPlaceFasterDown
	playse SE_PIN
	applywaitmovement LOCALID_JUBILIFE_DEVON_EMPLOYEE, Common_Movement_ExclamationMark
	applywaitmovement LOCALID_JUBILIFE_DEVON_EMPLOYEE, Common_Movement_Delay48
	applywaitmovement LOCALID_PLAYER, Common_Movement_WalkInPlaceFasterUp
	return

JubilifeCity_EventScript_EmployeeApproachPlayerDown2::
	applywaitmovement LOCALID_JUBILIFE_DEVON_EMPLOYEE, Common_Movement_WalkInPlaceFasterDown
	playse SE_PIN
	applywaitmovement LOCALID_JUBILIFE_DEVON_EMPLOYEE, Common_Movement_ExclamationMark
	applywaitmovement LOCALID_JUBILIFE_DEVON_EMPLOYEE, Common_Movement_Delay48
	applywaitmovement LOCALID_JUBILIFE_DEVON_EMPLOYEE, JubilifeCity_Movement_EmployeeApproachPlayerDown
	applywaitmovement LOCALID_PLAYER, Common_Movement_WalkInPlaceFasterUp
	return

JubilifeCity_EventScript_EmployeeFacePlayerRight::
	applywaitmovement LOCALID_JUBILIFE_DEVON_EMPLOYEE, Common_Movement_WalkInPlaceFasterLeft
	playse SE_PIN
	applywaitmovement LOCALID_JUBILIFE_DEVON_EMPLOYEE, Common_Movement_ExclamationMark
	applywaitmovement LOCALID_JUBILIFE_DEVON_EMPLOYEE, Common_Movement_Delay48
	return

JubilifeCity_EventScript_Rival::
	lockall
	call_if_unset FLAG_MET_RIVAL_JUBILIFE, JubilifeCity_EventScript_PlayRivalMusic
	applywaitmovement LOCALID_JUBILIFE_RIVAL, Common_Movement_FacePlayer
	goto JubilifeCity_EventScript_RivalEncounter

JubilifeCity_EventScript_PlayRivalMusic::
	checkplayergender
	goto_if_eq VAR_RESULT, MALE, JubilifeCity_EventScript_PlayMayMusic
	goto_if_eq VAR_RESULT, FEMALE, JubilifeCity_EventScript_PlayBrendanMusic
	return

JubilifeCity_EventScript_PlayMayMusic::
	playbgm MUS_ENCOUNTER_MAY, TRUE
	return

JubilifeCity_EventScript_PlayBrendanMusic::
	playbgm MUS_ENCOUNTER_BRENDAN, TRUE
	return

JubilifeCity_EventScript_RivalTrigger0::
	lockall
	call JubilifeCity_EventScript_PlayRivalMusic
	applywaitmovement LOCALID_JUBILIFE_RIVAL, Common_Movement_WalkInPlaceFasterDown
	playse SE_PIN
	applywaitmovement LOCALID_JUBILIFE_RIVAL, Common_Movement_ExclamationMark
	applywaitmovement LOCALID_JUBILIFE_RIVAL, Common_Movement_Delay48
	applywaitmovement LOCALID_JUBILIFE_RIVAL, JubilifeCity_Movement_RivalApproachPlayer0
	applywaitmovement LOCALID_PLAYER, Common_Movement_WalkInPlaceFasterUp
	goto JubilifeCity_EventScript_RivalEncounter

JubilifeCity_EventScript_RivalTrigger1::
	lockall
	call JubilifeCity_EventScript_PlayRivalMusic
	applywaitmovement LOCALID_JUBILIFE_RIVAL, Common_Movement_WalkInPlaceFasterDown
	playse SE_PIN
	applywaitmovement LOCALID_JUBILIFE_RIVAL, Common_Movement_ExclamationMark
	applywaitmovement LOCALID_JUBILIFE_RIVAL, Common_Movement_Delay48
	applywaitmovement LOCALID_JUBILIFE_RIVAL, JubilifeCity_Movement_RivalApproachPlayer1
	applywaitmovement LOCALID_PLAYER, Common_Movement_WalkInPlaceFasterUp
	goto JubilifeCity_EventScript_RivalEncounter

JubilifeCity_EventScript_RivalTrigger2::
	lockall
	call JubilifeCity_EventScript_PlayRivalMusic
	applywaitmovement LOCALID_JUBILIFE_RIVAL, Common_Movement_WalkInPlaceFasterDown
	playse SE_PIN
	applywaitmovement LOCALID_JUBILIFE_RIVAL, Common_Movement_ExclamationMark
	applywaitmovement LOCALID_JUBILIFE_RIVAL, Common_Movement_Delay48
	applywaitmovement LOCALID_JUBILIFE_RIVAL, JubilifeCity_Movement_RivalApproachPlayer2
	applywaitmovement LOCALID_PLAYER, Common_Movement_WalkInPlaceFasterUp
	goto JubilifeCity_EventScript_RivalEncounter

JubilifeCity_EventScript_RivalTrigger3::
	lockall
	call JubilifeCity_EventScript_PlayRivalMusic
	applywaitmovement LOCALID_JUBILIFE_RIVAL, Common_Movement_WalkInPlaceFasterDown
	playse SE_PIN
	applywaitmovement LOCALID_JUBILIFE_RIVAL, Common_Movement_ExclamationMark
	applywaitmovement LOCALID_JUBILIFE_RIVAL, Common_Movement_Delay48
	applywaitmovement LOCALID_JUBILIFE_RIVAL, JubilifeCity_Movement_RivalApproachPlayer3
	applywaitmovement LOCALID_PLAYER, Common_Movement_WalkInPlaceFasterUp
	goto JubilifeCity_EventScript_RivalEncounter

JubilifeCity_EventScript_RivalTrigger4::
	lockall
	call JubilifeCity_EventScript_PlayRivalMusic
	applywaitmovement LOCALID_JUBILIFE_RIVAL, Common_Movement_WalkInPlaceFasterDown
	playse SE_PIN
	applywaitmovement LOCALID_JUBILIFE_RIVAL, Common_Movement_ExclamationMark
	applywaitmovement LOCALID_JUBILIFE_RIVAL, Common_Movement_Delay48
	applywaitmovement LOCALID_JUBILIFE_RIVAL, JubilifeCity_Movement_RivalApproachPlayer4
	applywaitmovement LOCALID_PLAYER, Common_Movement_WalkInPlaceFasterUp
	goto JubilifeCity_EventScript_RivalEncounter

JubilifeCity_EventScript_RivalTrigger5::
	lockall
	call JubilifeCity_EventScript_PlayRivalMusic
	applywaitmovement LOCALID_JUBILIFE_RIVAL, Common_Movement_WalkInPlaceFasterDown
	playse SE_PIN
	applywaitmovement LOCALID_JUBILIFE_RIVAL, Common_Movement_ExclamationMark
	applywaitmovement LOCALID_JUBILIFE_RIVAL, Common_Movement_Delay48
	applywaitmovement LOCALID_JUBILIFE_RIVAL, JubilifeCity_Movement_RivalApproachPlayer5
	applywaitmovement LOCALID_PLAYER, Common_Movement_WalkInPlaceFasterUp
	goto JubilifeCity_EventScript_RivalEncounter

JubilifeCity_EventScript_RivalTrigger6::
	lockall
	call JubilifeCity_EventScript_PlayRivalMusic
	applywaitmovement LOCALID_JUBILIFE_RIVAL, Common_Movement_WalkInPlaceFasterDown
	playse SE_PIN
	applywaitmovement LOCALID_JUBILIFE_RIVAL, Common_Movement_ExclamationMark
	applywaitmovement LOCALID_JUBILIFE_RIVAL, Common_Movement_Delay48
	applywaitmovement LOCALID_JUBILIFE_RIVAL, JubilifeCity_Movement_RivalApproachPlayer6
	applywaitmovement LOCALID_PLAYER, Common_Movement_WalkInPlaceFasterUp
	goto JubilifeCity_EventScript_RivalEncounter

JubilifeCity_EventScript_RivalTrigger7::
	lockall
	call JubilifeCity_EventScript_PlayRivalMusic
	applywaitmovement LOCALID_JUBILIFE_RIVAL, Common_Movement_WalkInPlaceFasterDown
	playse SE_PIN
	applywaitmovement LOCALID_JUBILIFE_RIVAL, Common_Movement_ExclamationMark
	applywaitmovement LOCALID_JUBILIFE_RIVAL, Common_Movement_Delay48
	applywaitmovement LOCALID_JUBILIFE_RIVAL, JubilifeCity_Movement_RivalApproachPlayer7
	applywaitmovement LOCALID_PLAYER, Common_Movement_WalkInPlaceFasterUp
	goto JubilifeCity_EventScript_RivalEncounter

JubilifeCity_EventScript_RivalEncounter::
	checkplayergender
	goto_if_eq VAR_RESULT, MALE, JubilifeCity_EventScript_MayEncounter
	goto_if_eq VAR_RESULT, FEMALE, JubilifeCity_EventScript_BrendanEncounter
	end

JubilifeCity_EventScript_MayEncounter::
	goto_if_set FLAG_DEFEATED_RIVAL_JUBILIFE, JubilifeCity_EventScript_MayBrineyHint
	goto_if_set FLAG_MET_RIVAL_JUBILIFE, JubilifeCity_EventScript_MayAskToBattle
	setflag FLAG_MET_RIVAL_JUBILIFE
	msgbox JubilifeCity_Text_MayHiLetsRegister, MSGBOX_DEFAULT
	closemessage
	delay 30
	playfanfare MUS_REGISTER_MATCH_CALL
	msgbox JubilifeCity_Text_RegisteredMay, MSGBOX_DEFAULT
	waitfanfare
	closemessage
	delay 30
	setflag FLAG_ENABLE_RIVAL_MATCH_CALL
	setvar VAR_JUBILIFE_CITY_STATE, 8
	setvar VAR_ROUTE104_STATE, 2
	setvar VAR_0x8008, 0
	msgbox JubilifeCity_Text_MayPassedBrineyWantToBattle, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, YES, JubilifeCity_EventScript_BattleMay
	msgbox JubilifeCity_Text_MayOhHaventRaisedPokemonEnough, MSGBOX_DEFAULT
	call JubilifeCity_EventScript_RestoreBgm
	releaseall
	end

JubilifeCity_EventScript_MayAskToBattle::
	setvar VAR_0x8008, 1
	msgbox JubilifeCity_Text_MayWantToBattle, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, YES, JubilifeCity_EventScript_BattleMay
	msgbox JubilifeCity_Text_MayOhHaventRaisedPokemonEnough, MSGBOX_DEFAULT
	releaseall
	end

JubilifeCity_EventScript_BattleMay::
	msgbox JubilifeCity_Text_MayImNotGoingToLose, MSGBOX_DEFAULT
	switch VAR_STARTER_MON
	case 0, JubilifeCity_EventScript_BattleMayTreecko
	case 1, JubilifeCity_EventScript_BattleMayTorchic
	case 2, JubilifeCity_EventScript_BattleMayMudkip
	end

JubilifeCity_EventScript_MayBrineyHint::
	msgbox JubilifeCity_Text_MayMrBrineyHint, MSGBOX_DEFAULT
	call_if_eq VAR_0x8008, 0, JubilifeCity_EventScript_RestoreBgm
	releaseall
	end

JubilifeCity_EventScript_RestoreBgm::
	savebgm MUS_DUMMY
	fadedefaultbgm
	return

JubilifeCity_EventScript_BattleMayTreecko::
	trainerbattle_no_intro TRAINER_MAY_JUBILIFE_TREECKO, JubilifeCity_Text_MayDefeat
	setflag FLAG_DEFEATED_RIVAL_JUBILIFE
	goto JubilifeCity_EventScript_MayBrineyHint
	end

JubilifeCity_EventScript_BattleMayTorchic::
	trainerbattle_no_intro TRAINER_MAY_JUBILIFE_TORCHIC, JubilifeCity_Text_MayDefeat
	setflag FLAG_DEFEATED_RIVAL_JUBILIFE
	goto JubilifeCity_EventScript_MayBrineyHint
	end

JubilifeCity_EventScript_BattleMayMudkip::
	trainerbattle_no_intro TRAINER_MAY_JUBILIFE_MUDKIP, JubilifeCity_Text_MayDefeat
	setflag FLAG_DEFEATED_RIVAL_JUBILIFE
	goto JubilifeCity_EventScript_MayBrineyHint
	end

JubilifeCity_EventScript_BrendanEncounter::
	goto_if_set FLAG_DEFEATED_RIVAL_JUBILIFE, JubilifeCity_EventScript_BrendanBrineyHint
	goto_if_set FLAG_MET_RIVAL_JUBILIFE, JubilifeCity_EventScript_BrendanAskToBattle
	setflag FLAG_MET_RIVAL_JUBILIFE
	msgbox JubilifeCity_Text_BrendanHiLetsRegister, MSGBOX_DEFAULT
	closemessage
	delay 30
	playfanfare MUS_REGISTER_MATCH_CALL
	msgbox JubilifeCity_Text_RegisteredBrendan, MSGBOX_DEFAULT
	waitfanfare
	closemessage
	delay 30
	setflag FLAG_ENABLE_RIVAL_MATCH_CALL
	setvar VAR_JUBILIFE_CITY_STATE, 8
	setvar VAR_ROUTE104_STATE, 2
	msgbox JubilifeCity_Text_BrendanPassedBrineyWantToBattle, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, YES, JubilifeCity_EventScript_BattleBrendan
	msgbox JubilifeCity_Text_BrendanNoConfidenceInPokemon, MSGBOX_DEFAULT
	call JubilifeCity_EventScript_RestoreBgm
	releaseall
	end

JubilifeCity_EventScript_BrendanAskToBattle::
	msgbox JubilifeCity_Text_BrendanWantToBattle, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, YES, JubilifeCity_EventScript_BattleBrendan
	msgbox JubilifeCity_Text_BrendanNoConfidenceInPokemon, MSGBOX_DEFAULT
	releaseall
	end

JubilifeCity_EventScript_BattleBrendan::
	msgbox JubilifeCity_Text_BrendanIWontGoEasy, MSGBOX_DEFAULT
	switch VAR_STARTER_MON
	case 0, JubilifeCity_EventScript_BattleBrendanTreecko
	case 1, JubilifeCity_EventScript_BattleBrendanTorchic
	case 2, JubilifeCity_EventScript_BattleBrendanMudkip
	end

JubilifeCity_EventScript_BrendanBrineyHint::
	msgbox JubilifeCity_Text_BrendanMrBrineyHint, MSGBOX_DEFAULT
	call_if_eq VAR_0x8008, 0, JubilifeCity_EventScript_RestoreBgm
	releaseall
	end

JubilifeCity_EventScript_BattleBrendanTreecko::
	trainerbattle_no_intro TRAINER_BRENDAN_JUBILIFE_TREECKO, JubilifeCity_Text_BrendanDefeat
	setflag FLAG_DEFEATED_RIVAL_JUBILIFE
	goto JubilifeCity_EventScript_BrendanBrineyHint
	end

JubilifeCity_EventScript_BattleBrendanTorchic::
	trainerbattle_no_intro TRAINER_BRENDAN_JUBILIFE_TORCHIC, JubilifeCity_Text_BrendanDefeat
	setflag FLAG_DEFEATED_RIVAL_JUBILIFE
	goto JubilifeCity_EventScript_BrendanBrineyHint
	end

JubilifeCity_EventScript_BattleBrendanMudkip::
	trainerbattle_no_intro TRAINER_BRENDAN_JUBILIFE_MUDKIP, JubilifeCity_Text_BrendanDefeat
	setflag FLAG_DEFEATED_RIVAL_JUBILIFE
	goto JubilifeCity_EventScript_BrendanBrineyHint
	end

JubilifeCity_Movement_RivalApproachPlayer0:
	walk_down
	walk_left
	walk_left
	walk_left
	walk_left
	walk_down
	step_end

JubilifeCity_Movement_RivalApproachPlayer1:
	walk_down
	walk_left
	walk_left
	walk_left
	walk_down
	step_end

JubilifeCity_Movement_RivalApproachPlayer2:
	walk_down
	walk_left
	walk_left
	walk_down
	step_end

JubilifeCity_Movement_RivalApproachPlayer3:
	walk_down
	walk_left
	walk_down
	step_end

JubilifeCity_Movement_RivalApproachPlayer4:
	walk_down
	walk_down
	step_end

JubilifeCity_Movement_RivalApproachPlayer5:
	walk_down
	walk_right
	walk_down
	step_end

JubilifeCity_Movement_RivalApproachPlayer6:
	walk_down
	walk_right
	walk_right
	walk_down
	step_end

JubilifeCity_Movement_RivalApproachPlayer7:
	walk_down
	walk_right
	walk_right
	walk_right
	walk_down
	step_end

JubilifeCity_EventScript_Boy1::
	msgbox JubilifeCity_Text_YouCanHave2On2Battle, MSGBOX_NPC
	end

JubilifeCity_Text_WeShortenItToDevon:
	.string "The DEVON CORPORATION…\n"
	.string "We all just shorten it to DEVON.\p"
	.string "That company makes all sorts of\n"
	.string "convenient products.$"

JubilifeCity_Text_SneakyLookingManWentAroundCorner:
	.string "Hm? A sneaky-looking man?\p"
	.string "Come to think of it, yes, a shady-\n"
	.string "looking guy went around the corner.$"

JubilifeCity_Text_HaveYouChallengedGym:
	.string "Have you taken the POKéMON GYM\n"
	.string "challenge?\p"
	.string "When you get that shiny GYM BADGE\n"
	.string "in hand, I guess TRAINERS begin to\l"
	.string "realize what is required of them.$"

JubilifeCity_Text_HeyThatsJubilifesGymBadge:
	.string "Hey, that's RUSTBORO's GYM BADGE!\p"
	.string "Out of all the POKéMON GYM BADGES,\n"
	.string "RUSTBORO's is the coolest, I'd say.$"

JubilifeCity_Text_YoureNewAroundHere:
	.string "Oh? Who might you be?\n"
	.string "You're a new face around these parts.\p"
	.string "Have you just transferred into the\n"
	.string "POKéMON TRAINER'S SCHOOL?$"

JubilifeCity_Text_GymLeaderIsntEasyWithFire:
	.string "I challenged the GYM LEADER, but…\p"
	.string "It's not going to be easy winning with\n"
	.string "my FIRE-type POKéMON…\p"
	.string "FIRE-type POKéMON don't match up\n"
	.string "well against ROCK-type POKéMON…$"

JubilifeCity_Text_MrBrineyWalksInTheTunnel:
	.string "The old sailor MR. BRINEY lives in\n"
	.string "a cottage by the sea.\p"
	.string "He goes for walks in the tunnel every\n"
	.string "so often.$"

JubilifeCity_Text_MrBrineyLovesPeeko:
	.string "The old sailor MR. BRINEY lives in\n"
	.string "a cottage by the sea.\p"
	.string "He said he was going shopping in\n"
	.string "SLATEPORT for his pet, PEEKO.\p"
	.string "That old sea dog, he must really love\n"
	.string "that PEEKO.$"

JubilifeCity_Text_WowYouHavePokemon:
	.string "Wow, you have POKéMON with you, too.\p"
	.string "When I get bigger, I'm going to go\n"
	.string "places with POKéMON, too.$"

JubilifeCity_Text_CatchRarePokemonIfIGoToSchool:
	.string "POKéMON TRAINER'S SCHOOL!\p"
	.string "If I go to this school, will I be able\n"
	.string "to catch rare POKéMON easily?$"

JubilifeCity_Text_PokemonCanChangeLookFromExp:
	.string "If a POKéMON gains experience in\n"
	.string "battles, it can sometimes change in\l"
	.string "the way it looks.$"

JubilifeCity_Text_PokemonChangeShape:
	.string "A POKéMON changes shape?\n"
	.string "If one did that, I would be shocked!$"

JubilifeCity_Text_TradePokemonGrowFast:
	.string "A POKéMON you get in a trade from\n"
	.string "someone grows fast.\p"
	.string "But if you don't have certain GYM\n"
	.string "BADGES, it may not obey you…$"

JubilifeCity_Text_OutOfTheWay:
	.string "Get out!\n"
	.string "Out of the way!$"

JubilifeCity_Text_WaitDontTakeMyGoods:
	.string "Wait! Pleeeaaase!\p"
	.string "Don't take my GOODS!$"

JubilifeCity_Text_HelpMeIWasRobbed:
	.string "Oh, it's you!\p"
	.string "You're that fantastic TRAINER who\n"
	.string "helped me in PETALBURG WOODS!\p"
	.string "Help me! I was robbed by TEAM AQUA!\n"
	.string "I have to get the DEVON GOODS back!\p"
	.string "If I don't…\n"
	.string "I'm going to be in serious trouble.$"

JubilifeCity_Text_ShadyCharacterTookOffTowardsTunnel:
	.string "That shady character, I think he took\n"
	.string "off towards the tunnel over there.$"

JubilifeCity_Text_YouGotItThankYou:
	.string "Oh! How did it go?\n"
	.string "The DEVON GOODS?\p"
	.string "You did!\n"
	.string "You got them back!\p"
	.string "You really are a great TRAINER!\p"
	.string "I know! As my thanks, I'll give you\n"
	.string "another GREAT BALL!$"

JubilifeCity_Text_YoureLoadedWithItems:
	.string "You're loaded with items.\n"
	.string "I can't give you this GREAT BALL.$"

JubilifeCity_Text_PleaseComeWithMe:
	.string "Excuse me, please!\n"
	.string "Please come with me!$"

JubilifeCity_Text_MayHiLetsRegister:
	.string "MAY: Oh, hi, {PLAYER}{KUN}!\p"
	.string "You had a MATCH CALL feature put\n"
	.string "on your POKéNAV!\p"
	.string "Let's register each other so we can\n"
	.string "contact one another anytime!\p"
	.string "… … … … … …$"

JubilifeCity_Text_RegisteredMay:
	.string "Registered MAY\n"
	.string "in the POKéNAV.$"

JubilifeCity_Text_MayPassedBrineyWantToBattle:
	.string "MAY: Oh, by the way, I passed\n"
	.string "MR. BRINEY in PETALBURG WOODS.\p"
	.string "I guess he's on his way home to his\n"
	.string "cottage by the sea.\p"
	.string "How's your POKéDEX coming along,\n"
	.string "{PLAYER}{KUN}? Mine's looking pretty decent!\p"
	.string "So…\n"
	.string "How about a little battle?$"

JubilifeCity_Text_MayOhHaventRaisedPokemonEnough:
	.string "MAY: Oh, what's the matter?\p"
	.string "Haven't you caught or raised your\n"
	.string "POKéMON very much?\p"
	.string "That's not very good for a TRAINER!$"

JubilifeCity_Text_MayWantToBattle:
	.string "MAY: So, what do you think?\n"
	.string "How about a little battle here?$"

JubilifeCity_Text_MayImNotGoingToLose:
	.string "MAY: You just became a TRAINER,\n"
	.string "{PLAYER}{KUN}. I'm not going to lose!$"

JubilifeCity_Text_MayDefeat:
	.string "Yikes!\n"
	.string "You're better than I expected!$"

JubilifeCity_Text_MayMrBrineyHint:
	.string "MAY: Oh, by the way, MR. BRINEY, who\n"
	.string "I just passed…\p"
	.string "{PLAYER}{KUN}, you just moved here so you\n"
	.string "might not know this, but MR. BRINEY\l"
	.string "was once a revered seafarer.$"

JubilifeCity_Text_BrendanHiLetsRegister:
	.string "BRENDAN: Oh, hey, {PLAYER}!\p"
	.string "You had a MATCH CALL feature put\n"
	.string "on your POKéNAV! Cool!\p"
	.string "Let's register each other so we can\n"
	.string "get in touch anytime!\p"
	.string "… … … … … …$"

JubilifeCity_Text_RegisteredBrendan:
	.string "Registered BRENDAN\n"
	.string "in the POKéNAV.$"

JubilifeCity_Text_BrendanPassedBrineyWantToBattle:
	.string "BRENDAN: By the way, {PLAYER}, I walked\n"
	.string "by MR. BRINEY in PETALBURG WOODS.\p"
	.string "I bet he was on his way home to his\n"
	.string "cottage by the sea.\p"
	.string "{PLAYER}, how's your POKéDEX?\n"
	.string "Have you filled in any pages yet?\p"
	.string "Mine rules.\p"
	.string "Want to have a battle to test how\n"
	.string "far you've progressed?$"

JubilifeCity_Text_BrendanNoConfidenceInPokemon:
	.string "BRENDAN: What's the matter? Don't have\n"
	.string "any confidence in your POKéMON?$"

JubilifeCity_Text_BrendanWantToBattle:
	.string "BRENDAN: What's up?\n"
	.string "Want to have a battle with me?$"

JubilifeCity_Text_BrendanIWontGoEasy:
	.string "BRENDAN: I know you just became\n"
	.string "a TRAINER, but I won't go easy!$"

JubilifeCity_Text_BrendanDefeat:
	.string "Hmm…\n"
	.string "You're pretty good.$"

JubilifeCity_Text_BrendanMrBrineyHint:
	.string "BRENDAN: By the way, you know\n"
	.string "MR. BRINEY? The guy I just passed?\p"
	.string "I bet you didn't know this, since you\n"
	.string "just moved here, {PLAYER}, but\l"
	.string "MR. BRINEY was once a great sailor.$"

JubilifeCity_Text_TunnelNearingCompletion:
	.string "“Timesaving tunnel nearing\n"
	.string "completion!”\p"
	.string "…Is what it says on the sign, but\n"
	.string "there's also a big “X” splashed\l"
	.string "across it in red paint…$"

JubilifeCity_Text_PoketchCompanySign:
	.string "POKéTCH COMPANY$"

JubilifeCity_Text_JubilifeTVSign:
	.string "JUBILIFE TV$"

JubilifeCity_Text_GymSign:
	.string "RUSTBORO CITY POKéMON GYM\n"
	.string "LEADER: ROXANNE\p"
	.string "“The ROCK-loving honors student!”$"

JubilifeCity_Text_DevonCorpBranchOfficeSign:
	.string "DEVON CORP. BRANCH OFFICE\p"
	.string "“Access limited to DEVON employees\n"
	.string "and authorized personnel.”$"

JubilifeCity_Text_CitySign:
	.string "JUBILIFE CITY\p"
	.string "“The most modern city in SINNOH.”$"

JubilifeCity_Text_TrainersSchoolSign:
	.string "POKéMON TRAINER'S SCHOOL\p"
	.string "“We'll teach you anything about\n"
	.string "POKéMON!”$"

JubilifeCity_Text_CuttersHouse:
	.string "CUTTER'S HOUSE$"

JubilifeCity_Text_DevelopedNewPokenavFeature:
	.string "I've been developing an added feature\n"
	.string "for the POKéNAV…\p"
	.string "And it turned out great!\p"
	.string "{PLAYER}, may I see that POKéNAV?\n"
	.string "The one our PRESIDENT gave you?$"

JubilifeCity_Text_AddedMatchCallPleaseCallMrStone:
	.string "There you go, {PLAYER}!\p"
	.string "I added a new feature named\n"
	.string "MATCH CALL to your POKéNAV.\p"
	.string "Using the MATCH CALL feature,\n"
	.string "you can chat with people who have\l"
	.string "been registered in your POKéNAV.\p"
	.string "{PLAYER}, our PRESIDENT STONE should\n"
	.string "be registered in your POKéNAV.\p"
	.string "Test it out.\n"
	.string "Please give our PRESIDENT a call.$"

JubilifeCity_Text_PleaseSelectPokenav:
	.string "Please select the POKéNAV.$"

JubilifeCity_Text_IdBetterGetBackToWork:
	.string "Okay, I'd better get back to work.\n"
	.string "Please take care, {PLAYER}!$"

JubilifeCity_Text_YouCanHave2On2Battle:
	.string "Did you know this?\p"
	.string "You can have a 2-on-2 battle even\n"
	.string "if you're not with another TRAINER.\p"
	.string "If you catch the eyes of two TRAINERS\n"
	.string "when you have two or more POKéMON,\l"
	.string "they'll both challenge you.\p"
	.string "Don't you think it'd be cool if you\n"
	.string "could beat two TRAINERS by yourself?$"
