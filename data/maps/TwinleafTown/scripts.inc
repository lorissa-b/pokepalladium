TwinleafTown_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, TwinleafTown_OnTransition
	map_script MAP_SCRIPT_ON_FRAME_TABLE, TwinleafTown_OnFrame
	map_script MAP_SCRIPT_ON_WARP_INTO_MAP_TABLE, TwinleafTown_OnWarp
	.byte 0

	@ State descriptions for VAR_TWINLEAF_TOWN_STATE
	@ 1: Met Rival
	@ 2: Saving Birch
	@ 3: Received Pokedex
	@ 4: Received Running Shoes

	@ State descriptions for VAR_TWINLEAF_INTRO_STATE
	@ 1: In truck male
	@ 2: In truck female
	@ 3: Entered house
	@ 4: Told to go set clock
	@ 5: Entered 2F to set clock
	@ 6: Set clock
	@ 7: Told to go meet rival
	@ 8: Woke up in bedroom (new game start, replaces 1-7)
	@ 9: Mom said rival was looking for you
	@ 10: Met rival (door bump or caught up at north exit), rival left for Route 201

	@ State descriptions for VAR_TWINLEAF_HOUSES_STATE_MAY
	@                    and VAR_TWINLEAF_HOUSES_STATE_BRENDAN
	@ 1: On Truck (and is corresponding gender)
	@ 2: Met Rival's Mom (and is corresponding gender)
	@ 3: Entered Hall of Fame
	@ 4: Completed Lati TV / SS Ticket event
	@ 5: Exited house after above event

	@ State descriptions for VAR_TWINLEAF_RIVAL_STATE
	@ 1: Never occurs
	@ 2: Entered Rival's bedroom
	@ 3: Met Rival
	@ 4: Received Pokedex


TwinleafTown_OnTransition:
	setflag FLAG_VISITED_TWINLEAF_TOWN
	call Common_EventScript_SetupRivalGfxId
	call_if_eq VAR_TWINLEAF_INTRO_STATE, 2, TwinleafTown_EventScript_MoveMomToMaysDoor
	call_if_unset FLAG_RESCUED_BIRCH, TwinleafTown_EventScript_SetTwinPos
	call_if_eq VAR_TWINLEAF_HOUSES_STATE_MAY, 4, TwinleafTown_EventScript_SetExitedHouseAfterLatiSSTicketEvent
	call_if_eq VAR_TWINLEAF_HOUSES_STATE_BRENDAN, 4, TwinleafTown_EventScript_SetExitedHouseAfterLatiSSTicketEvent
	call_if_eq VAR_SANDGEM_RIVAL_STATE, 1, TwinleafTown_EventScript_MoveRivalFromSandgem
	call_if_eq VAR_TWINLEAF_RIVAL_STATE, 3, TwinleafTown_EventScript_SetRivalLeftForRoute202
	call_if_eq VAR_DEX_UPGRADE_JOHTO_STARTER_STATE, 1, TwinleafTown_EventScript_HideMapNamePopup
	call_if_eq VAR_DEX_UPGRADE_JOHTO_STARTER_STATE, 2, TwinleafTown_EventScript_LeftLabAfterDexUpgrade
	end

TwinleafTown_EventScript_LeftLabAfterDexUpgrade::
	setvar VAR_DEX_UPGRADE_JOHTO_STARTER_STATE, 3
	return

TwinleafTown_EventScript_HideMapNamePopup::
	setflag FLAG_HIDE_MAP_NAME_POPUP
	return

TwinleafTown_EventScript_SetRivalLeftForRoute202::
	return

TwinleafTown_EventScript_MoveRivalFromSandgem::
	setvar VAR_SANDGEM_RIVAL_STATE, 2
	setflag FLAG_HIDE_SANDGEM_TOWN_RIVAL
	return

TwinleafTown_EventScript_SetExitedHouseAfterLatiSSTicketEvent::
	setvar VAR_TWINLEAF_HOUSES_STATE_MAY, 5
	setvar VAR_TWINLEAF_HOUSES_STATE_BRENDAN, 5
	return

TwinleafTown_EventScript_MoveMomToMaysDoor::
	setobjectxyperm LOCALID_TWINLEAF_MOM, 14, 8
	return

TwinleafTown_EventScript_SetTwinPos::
	goto_if_eq VAR_TWINLEAF_TOWN_STATE, 0, TwinleafTown_EventScript_SetTwinGuardingRoutePos
	setobjectxyperm LOCALID_TWINLEAF_TWIN, 11, 1
	setobjectmovementtype LOCALID_TWINLEAF_TWIN, MOVEMENT_TYPE_FACE_UP
	return

TwinleafTown_EventScript_SetTwinGuardingRoutePos::
	setobjectxyperm LOCALID_TWINLEAF_TWIN, 7, 2
	setobjectmovementtype LOCALID_TWINLEAF_TWIN, MOVEMENT_TYPE_FACE_DOWN
	return

TwinleafTown_OnFrame:
	map_script_2 VAR_TWINLEAF_INTRO_STATE, 1, TwinleafTown_EventScript_StepOffTruckMale
	map_script_2 VAR_TWINLEAF_INTRO_STATE, 2, TwinleafTown_EventScript_StepOffTruckFemale
	map_script_2 VAR_DEX_UPGRADE_JOHTO_STARTER_STATE, 1, TwinleafTown_EventScript_BeginDexUpgradeScene
	.2byte 0

@ Brendan's house door is at 4, 8. The rival runs out of it and slams into the player.
TwinleafTown_EventScript_RivalBumpsIntoPlayer::
	lockall
	applywaitmovement LOCALID_PLAYER, Common_Movement_WalkInPlaceFasterUp
	setobjectxyperm LOCALID_TWINLEAF_RIVAL, 4, 8
	setobjectmovementtype LOCALID_TWINLEAF_RIVAL, MOVEMENT_TYPE_FACE_DOWN
	opendoor 4, 8
	waitdooranim
	addobject LOCALID_TWINLEAF_RIVAL
	playse SE_WALL_HIT
	applymovement LOCALID_TWINLEAF_RIVAL, TwinleafTown_Movement_RivalRushOutOfHouse
	applymovement LOCALID_PLAYER, TwinleafTown_Movement_PlayerKnockedBack
	waitmovement 0
	closedoor 4, 8
	waitdooranim
	playse SE_PIN
	applywaitmovement LOCALID_TWINLEAF_RIVAL, Common_Movement_ExclamationMark
	applywaitmovement LOCALID_TWINLEAF_RIVAL, Common_Movement_Delay48
	msgbox TwinleafTown_Text_RivalMeetMeOnRoute201, MSGBOX_DEFAULT
	closemessage
	applymovement LOCALID_TWINLEAF_RIVAL, TwinleafTown_Movement_RivalRunToRoute201
	applymovement LOCALID_PLAYER, TwinleafTown_Movement_PlayerWatchRivalLeave
	waitmovement LOCALID_TWINLEAF_RIVAL
	waitmovement LOCALID_PLAYER
	removeobject LOCALID_TWINLEAF_RIVAL
	call TwinleafTown_EventScript_SetMetRival
	releaseall
	end

TwinleafTown_EventScript_SetMetRival::
	setvar VAR_TWINLEAF_INTRO_STATE, 10
	@ Skips the vanilla "meet rival in their bedroom" scene
	setvar VAR_TWINLEAF_RIVAL_STATE, 3
	setvar VAR_TWINLEAF_TOWN_STATE, 1
	return

@ Leaving town at the north exit (x 8-13, y 0) before meeting the rival.
@ The rival runs up from behind and heads to Route 201 with the player.
TwinleafTown_EventScript_RivalCatchesUpTriggerWest::
	lockall
	setvar VAR_0x8009, 8
	goto TwinleafTown_EventScript_RivalCatchesUp
	end

TwinleafTown_EventScript_RivalCatchesUpTrigger0::
	lockall
	setvar VAR_0x8009, 9
	goto TwinleafTown_EventScript_RivalCatchesUp
	end

TwinleafTown_EventScript_RivalCatchesUpTrigger1::
	lockall
	setvar VAR_0x8009, 10
	goto TwinleafTown_EventScript_RivalCatchesUp
	end

TwinleafTown_EventScript_RivalCatchesUpTrigger2::
	lockall
	setvar VAR_0x8009, 11
	goto TwinleafTown_EventScript_RivalCatchesUp
	end

TwinleafTown_EventScript_RivalCatchesUpTrigger3::
	lockall
	setvar VAR_0x8009, 12
	goto TwinleafTown_EventScript_RivalCatchesUp
	end

TwinleafTown_EventScript_RivalCatchesUpTriggerEast::
	lockall
	setvar VAR_0x8009, 13
	goto TwinleafTown_EventScript_RivalCatchesUp
	end

TwinleafTown_EventScript_RivalCatchesUp::
	msgbox TwinleafTown_Text_RivalWaitUp, MSGBOX_DEFAULT
	closemessage
	applywaitmovement LOCALID_PLAYER, Common_Movement_WalkInPlaceFasterDown
	@ Start just off screen, below the player
	setobjectxyperm LOCALID_TWINLEAF_RIVAL, VAR_0x8009, 6
	setobjectmovementtype LOCALID_TWINLEAF_RIVAL, MOVEMENT_TYPE_FACE_UP
	addobject LOCALID_TWINLEAF_RIVAL
	applywaitmovement LOCALID_TWINLEAF_RIVAL, TwinleafTown_Movement_RivalRunUpBehindPlayer
	msgbox TwinleafTown_Text_RivalYouReadMyMind, MSGBOX_DEFAULT
	closemessage
	@ Pass the player on the side toward the middle of the exit
	call_if_lt VAR_0x8009, 12, TwinleafTown_EventScript_RivalRunAheadRight
	call_if_ge VAR_0x8009, 12, TwinleafTown_EventScript_RivalRunAheadLeft
	removeobject LOCALID_TWINLEAF_RIVAL
	call TwinleafTown_EventScript_SetMetRival
	releaseall
	end

TwinleafTown_EventScript_RivalRunAheadRight::
	applymovement LOCALID_TWINLEAF_RIVAL, TwinleafTown_Movement_RivalRunAheadRight
	applymovement LOCALID_PLAYER, TwinleafTown_Movement_PlayerWatchRivalRunAheadRight
	waitmovement LOCALID_TWINLEAF_RIVAL
	waitmovement LOCALID_PLAYER
	return

TwinleafTown_EventScript_RivalRunAheadLeft::
	applymovement LOCALID_TWINLEAF_RIVAL, TwinleafTown_Movement_RivalRunAheadLeft
	applymovement LOCALID_PLAYER, TwinleafTown_Movement_PlayerWatchRivalRunAheadLeft
	waitmovement LOCALID_TWINLEAF_RIVAL
	waitmovement LOCALID_PLAYER
	return

TwinleafTown_Movement_RivalRunUpBehindPlayer:
	walk_fast_up
	walk_fast_up
	walk_fast_up
	walk_fast_up
	walk_fast_up
	step_end

TwinleafTown_Movement_RivalRunAheadRight:
	walk_fast_right
	walk_fast_up
	walk_fast_up
	walk_fast_up
	walk_fast_up
	walk_fast_up
	walk_fast_up
	walk_fast_up
	step_end

TwinleafTown_Movement_RivalRunAheadLeft:
	walk_fast_left
	walk_fast_up
	walk_fast_up
	walk_fast_up
	walk_fast_up
	walk_fast_up
	walk_fast_up
	walk_fast_up
	step_end

TwinleafTown_Movement_PlayerWatchRivalRunAheadRight:
	delay_4
	walk_in_place_faster_right
	delay_8
	walk_in_place_faster_up
	step_end

TwinleafTown_Movement_PlayerWatchRivalRunAheadLeft:
	delay_4
	walk_in_place_faster_left
	delay_8
	walk_in_place_faster_up
	step_end

TwinleafTown_Movement_RivalRushOutOfHouse:
	walk_faster_down
	step_end

TwinleafTown_Movement_PlayerKnockedBack:
	lock_facing_direction
	jump_down
	unlock_facing_direction
	step_end

TwinleafTown_Movement_RivalRunToRoute201:
	walk_fast_right
	walk_fast_right
	walk_fast_right
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
	step_end

TwinleafTown_Movement_PlayerWatchRivalLeave:
	delay_8
	walk_in_place_faster_right
	delay_16
	delay_16
	delay_16
	delay_16
	walk_in_place_faster_up
	step_end

TwinleafTown_EventScript_StepOffTruckMale::
	lockall
	setvar VAR_0x8004, 5
	setvar VAR_0x8005, 8
	call TwinleafTown_EventScript_GoInsideWithMom
	warpsilent MAP_TWINLEAF_TOWN_LUCAS_HOUSE_1F, 8, 8
	waitstate
	releaseall
	end

TwinleafTown_EventScript_StepOffTruckFemale::
	lockall
	setvar VAR_0x8004, 14
	setvar VAR_0x8005, 8
	call TwinleafTown_EventScript_GoInsideWithMom
	warpsilent MAP_TWINLEAF_TOWN_DAWN_HOUSE_1F, 2, 8
	waitstate
	releaseall
	end

TwinleafTown_EventScript_GoInsideWithMom::
	delay 15
	playse SE_LEDGE
	applywaitmovement LOCALID_PLAYER, TwinleafTown_Movement_PlayerStepOffTruck
	opendoor VAR_0x8004, VAR_0x8005
	waitdooranim
	addobject LOCALID_TWINLEAF_MOM
	applywaitmovement LOCALID_TWINLEAF_MOM, TwinleafTown_Movement_MomExitHouse
	closedoor VAR_0x8004, VAR_0x8005
	waitdooranim
	delay 10
	applywaitmovement LOCALID_TWINLEAF_MOM, TwinleafTown_Movement_MomApproachPlayerAtTruck
	msgbox TwinleafTown_Text_OurNewHomeLetsGoInside, MSGBOX_DEFAULT
	closemessage
	applymovement LOCALID_TWINLEAF_MOM, TwinleafTown_Movement_MomApproachDoor
	applymovement LOCALID_PLAYER, TwinleafTown_Movement_PlayerApproachDoor
	waitmovement 0
	opendoor VAR_0x8004, VAR_0x8005
	waitdooranim
	applymovement LOCALID_TWINLEAF_MOM, TwinleafTown_Movement_MomEnterHouse
	applymovement LOCALID_PLAYER, TwinleafTown_Movement_PlayerEnterHouse
	waitmovement 0
	setflag FLAG_HIDE_TWINLEAF_TOWN_MOM_OUTSIDE
	setvar VAR_TWINLEAF_INTRO_STATE, 3
	hideplayer
	closedoor VAR_0x8004, VAR_0x8005
	waitdooranim
	clearflag FLAG_HIDE_TWINLEAF_TOWN_FAT_MAN
	clearflag FLAG_HIDE_MAP_NAME_POPUP
	return

TwinleafTown_Movement_MomExitHouse:
	walk_down
	step_end

TwinleafTown_Movement_MomApproachPlayerAtTruck:
	walk_down
	walk_in_place_faster_left
	step_end

TwinleafTown_Movement_MomApproachDoor:
	delay_16
	delay_8
	walk_up
	step_end

TwinleafTown_Movement_MomEnterHouse:
	walk_up
	set_invisible
	step_end

TwinleafTown_Movement_PlayerApproachDoor:
	delay_16
	delay_8
	walk_right
	walk_in_place_faster_up
	step_end

TwinleafTown_Movement_PlayerEnterHouse:
	walk_up
	walk_up
	step_end

TwinleafTown_Movement_PlayerStepOffTruck:
	jump_right
	delay_16
	delay_16
	delay_16
	step_end

TwinleafTown_EventScript_BeginDexUpgradeScene::
	lockall
	playse SE_PIN
	applywaitmovement LOCALID_TWINLEAF_BIRCH, Common_Movement_ExclamationMark
	delay 80
	msgbox TwinleafTown_Text_BirchSomethingToShowYouAtLab, MSGBOX_DEFAULT
	closemessage
	clearflag FLAG_HIDE_TWINLEAF_TOWN_RIVAL
	clearflag FLAG_HIDE_TWINLEAF_TOWN_BIRCH
	delay 20
	clearflag FLAG_HIDE_MAP_NAME_POPUP
	warp MAP_SANDGEM_TOWN_POKEMON_RESEARCH_LAB, 6, 5
	waitstate
	releaseall
	end

TwinleafTown_OnWarp:
	map_script_2 VAR_DEX_UPGRADE_JOHTO_STARTER_STATE, 1, TwinleafTown_EventScript_SetRivalBirchPosForDexUpgrade
	.2byte 0

TwinleafTown_EventScript_SetRivalBirchPosForDexUpgrade::
	addobject LOCALID_TWINLEAF_BIRCH
	addobject LOCALID_TWINLEAF_RIVAL
	@ Outside the player's house (May's house), for both genders
	setobjectxy LOCALID_TWINLEAF_RIVAL, 13, 20
	setobjectxy LOCALID_TWINLEAF_BIRCH, 14, 20
	end

TwinleafTown_EventScript_FatMan::
	msgbox TwinleafTown_Text_CanUsePCToStoreItems, MSGBOX_NPC
	end

TwinleafTown_EventScript_Boy::
	msgbox TwinleafTown_Text_BirchSpendsDaysInLab, MSGBOX_NPC
	end

TwinleafTown_EventScript_Twin::
	lock
	faceplayer
	goto_if_set FLAG_ADVENTURE_STARTED, TwinleafTown_EventScript_GoodLuck
	goto_if_set FLAG_RESCUED_BIRCH, TwinleafTown_EventScript_YouSavedBirch
	goto_if_ne VAR_TWINLEAF_TOWN_STATE, 0, TwinleafTown_EventScript_GoSaveBirch
	msgbox TwinleafTown_Text_IfYouGoInGrassPokemonWillJumpOut, MSGBOX_DEFAULT
	release
	end

TwinleafTown_EventScript_GoSaveBirch::
	special GetPlayerBigGuyGirlString
	msgbox TwinleafTown_Text_CanYouGoSeeWhatsHappening, MSGBOX_DEFAULT
	closemessage
	applywaitmovement LOCALID_TWINLEAF_TWIN, Common_Movement_FaceOriginalDirection
	setvar VAR_TWINLEAF_TOWN_STATE, 2
	release
	end

TwinleafTown_EventScript_YouSavedBirch::
	special GetPlayerBigGuyGirlString
	msgbox TwinleafTown_Text_YouSavedBirch, MSGBOX_DEFAULT
	release
	end

TwinleafTown_EventScript_GoodLuck::
	msgbox TwinleafTown_Text_GoodLuckCatchingPokemon, MSGBOX_DEFAULT
	release
	end

TwinleafTown_EventScript_NeedPokemonTriggerLeft::
	lockall
	applywaitmovement LOCALID_TWINLEAF_TWIN, TwinleafTown_Movement_TwinApproachPlayerLeft
	call TwinleafTown_EventScript_DangerousWithoutPokemon
	applywaitmovement LOCALID_TWINLEAF_TWIN, TwinleafTown_Movement_TwinReturnLeft
	releaseall
	end

TwinleafTown_EventScript_DangerousWithoutPokemon::
	msgbox TwinleafTown_Text_IfYouGoInGrassPokemonWillJumpOut, MSGBOX_DEFAULT
	closemessage
	applymovement LOCALID_TWINLEAF_TWIN, TwinleafTown_Movement_TwinPushPlayerFromRoute
	applymovement LOCALID_PLAYER, TwinleafTown_Movement_PushPlayerBackFromRoute
	waitmovement 0
	msgbox TwinleafTown_Text_DangerousIfYouDontHavePokemon, MSGBOX_DEFAULT
	closemessage
	return

TwinleafTown_Movement_TwinApproachPlayerLeft:
	face_right
	delay_8
	disable_jump_landing_ground_effect
	jump_in_place_right
	delay_8
	walk_fast_right
	walk_fast_right
	walk_fast_right
	walk_fast_right
	walk_fast_up
	walk_fast_up
	walk_fast_left
	face_down
	step_end

TwinleafTown_Movement_TwinPushPlayerFromRoute:
	walk_down
	step_end

TwinleafTown_Movement_TwinReturnLeft:
	walk_right
	walk_down
	walk_down
	walk_left
	walk_left
	walk_left
	walk_left
	walk_up
	walk_in_place_faster_down
	step_end

TwinleafTown_Movement_PushPlayerBackFromRoute:
	lock_facing_direction
	walk_down
	unlock_facing_direction
	step_end

TwinleafTown_EventScript_NeedPokemonTriggerRight::
	lockall
	applywaitmovement LOCALID_TWINLEAF_TWIN, TwinleafTown_Movement_TwinApproachPlayerRight
	call TwinleafTown_EventScript_DangerousWithoutPokemon
	applywaitmovement LOCALID_TWINLEAF_TWIN, TwinleafTown_Movement_TwinReturnRight
	releaseall
	end

TwinleafTown_Movement_TwinApproachPlayerRight:
	face_right
	delay_8
	disable_jump_landing_ground_effect
	jump_in_place_right
	delay_8
	walk_fast_right
	walk_fast_right
	walk_fast_right
	walk_fast_up
	walk_fast_up
	walk_fast_right
	face_down
	step_end

TwinleafTown_Movement_TwinReturnRight:
	walk_left
	walk_down
	walk_left
	walk_left
	walk_left
	walk_in_place_faster_down
	step_end

TwinleafTown_EventScript_GoSaveBirchTrigger::
	lockall
	applywaitmovement LOCALID_TWINLEAF_TWIN, Common_Movement_WalkInPlaceFasterRight
	applywaitmovement LOCALID_PLAYER, Common_Movement_WalkInPlaceFasterLeft
	special GetPlayerBigGuyGirlString
	msgbox TwinleafTown_Text_CanYouGoSeeWhatsHappening, MSGBOX_DEFAULT
	closemessage
	applywaitmovement LOCALID_TWINLEAF_TWIN, Common_Movement_FaceOriginalDirection
	setvar VAR_TWINLEAF_TOWN_STATE, 2
	releaseall
	end

TwinleafTown_EventScript_TownSign::
	msgbox TwinleafTown_Text_TownSign, MSGBOX_SIGN
	end

TwinleafTown_EventScript_BirchsLabSign::
	msgbox TwinleafTown_Text_ProfBirchsLab, MSGBOX_SIGN
	end

@ Brendan's house is always the rival's house
TwinleafTown_EventScript_BrendansHouseSign::
	msgbox TwinleafTown_Text_RivalsHouse, MSGBOX_SIGN
	end

@ May's house is always the player's house
TwinleafTown_EventScript_MaysHouseSign::
	msgbox TwinleafTown_Text_PlayersHouse, MSGBOX_SIGN
	end

TwinleafTown_EventScript_GiveRunningShoesTrigger0::
	lockall
	setvar VAR_0x8008, 0
	setobjectxy LOCALID_TWINLEAF_MOM, 10, 9
	goto TwinleafTown_EventScript_GiveRunningShoesTrigger
	end

TwinleafTown_EventScript_GiveRunningShoesTrigger1::
	lockall
	setvar VAR_0x8008, 1
	setobjectxy LOCALID_TWINLEAF_MOM, 11, 9
	goto TwinleafTown_EventScript_GiveRunningShoesTrigger
	end

TwinleafTown_EventScript_GiveRunningShoesTrigger2::
	lockall
	setvar VAR_0x8008, 2
	goto TwinleafTown_EventScript_GiveRunningShoesTrigger
	end

TwinleafTown_EventScript_GiveRunningShoesTrigger3::
	lockall
	setvar VAR_0x8008, 3
	goto TwinleafTown_EventScript_GiveRunningShoesTrigger
	end

TwinleafTown_EventScript_GiveRunningShoesTrigger4::
	lockall
	setvar VAR_0x8008, 4
	goto TwinleafTown_EventScript_GiveRunningShoesTrigger
	end

TwinleafTown_EventScript_GiveRunningShoesTrigger5::
	lockall
	setvar VAR_0x8008, 5
	goto TwinleafTown_EventScript_GiveRunningShoesTrigger
	end

@ The player's house is always May's house, for both genders
TwinleafTown_EventScript_GiveRunningShoesTrigger::
	call TwinleafTown_EventScript_MomNoticePlayerFemale
	call TwinleafTown_EventScript_SetHomeDoorCoords
	msgbox TwinleafTown_Text_WaitPlayer, MSGBOX_DEFAULT
	closemessage
	call TwinleafTown_EventScript_MomApproachPlayerFemale
	call TwinleafTown_EventScript_GiveRunningShoes
	call TwinleafTown_EventScript_MomReturnHomeFemale
	goto TwinleafTown_EventScript_SetReceivedRunningShoes
	end

@ Door of the player's house (May's house)
TwinleafTown_EventScript_SetHomeDoorCoords::
	setvar VAR_0x8009, 14
	setvar VAR_0x800A, 18
	return

TwinleafTown_EventScript_MomNoticePlayerFemale::
	applywaitmovement LOCALID_TWINLEAF_MOM, Common_Movement_WalkInPlaceFasterLeft
	return

TwinleafTown_EventScript_MomApproachPlayerFemale::
	call_if_eq VAR_0x8008, 0, TwinleafTown_EventScript_MomApproachPlayer0
	call_if_eq VAR_0x8008, 1, TwinleafTown_EventScript_MomApproachPlayer1
	call_if_eq VAR_0x8008, 2, TwinleafTown_EventScript_MomApproachPlayerFemale2
	call_if_eq VAR_0x8008, 3, TwinleafTown_EventScript_MomApproachPlayerFemale3
	call_if_eq VAR_0x8008, 4, TwinleafTown_EventScript_MomApproachPlayerFemale4
	call_if_eq VAR_0x8008, 5, TwinleafTown_EventScript_MomApproachPlayerFemale5
	return

TwinleafTown_EventScript_MomApproachPlayer0::
	applywaitmovement LOCALID_PLAYER, Common_Movement_WalkInPlaceFasterDown
	applywaitmovement LOCALID_TWINLEAF_MOM, TwinleafTown_Movement_MomApproachPlayer0
	return

TwinleafTown_EventScript_MomApproachPlayer1::
	applywaitmovement LOCALID_PLAYER, Common_Movement_WalkInPlaceFasterDown
	applywaitmovement LOCALID_TWINLEAF_MOM, TwinleafTown_Movement_MomApproachPlayer1
	return

TwinleafTown_EventScript_MomApproachPlayerFemale2::
	applywaitmovement LOCALID_PLAYER, Common_Movement_WalkInPlaceFasterRight
	applywaitmovement LOCALID_TWINLEAF_MOM, TwinleafTown_Movement_MomApproachPlayerFemale2
	return

TwinleafTown_EventScript_MomApproachPlayerFemale3::
	applywaitmovement LOCALID_PLAYER, Common_Movement_WalkInPlaceFasterRight
	applywaitmovement LOCALID_TWINLEAF_MOM, TwinleafTown_Movement_MomApproachPlayerFemale3
	return

TwinleafTown_EventScript_MomApproachPlayerFemale4::
	applywaitmovement LOCALID_PLAYER, Common_Movement_WalkInPlaceFasterRight
	applywaitmovement LOCALID_TWINLEAF_MOM, TwinleafTown_Movement_MomApproachPlayerFemale4
	return

TwinleafTown_EventScript_MomApproachPlayerFemale5::
	applywaitmovement LOCALID_PLAYER, Common_Movement_WalkInPlaceFasterRight
	applywaitmovement LOCALID_TWINLEAF_MOM, TwinleafTown_Movement_MomApproachPlayerFemale5
	return

TwinleafTown_EventScript_MomReturnHomeFemale::
	call_if_eq VAR_0x8008, 0, TwinleafTown_EventScript_MomReturnHome0
	call_if_eq VAR_0x8008, 1, TwinleafTown_EventScript_MomReturnHome1
	call_if_eq VAR_0x8008, 2, TwinleafTown_EventScript_MomReturnHomeFemale2
	call_if_eq VAR_0x8008, 3, TwinleafTown_EventScript_MomReturnHomeFemale3
	call_if_eq VAR_0x8008, 4, TwinleafTown_EventScript_MomReturnHomeFemale4
	call_if_eq VAR_0x8008, 5, TwinleafTown_EventScript_MomReturnHomeFemale5
	return

TwinleafTown_EventScript_MomReturnHome0::
	applywaitmovement LOCALID_TWINLEAF_MOM, TwinleafTown_Movement_MomReturnHome0
	return

TwinleafTown_EventScript_MomReturnHome1::
	applywaitmovement LOCALID_TWINLEAF_MOM, TwinleafTown_Movement_MomReturnHome1
	return

TwinleafTown_EventScript_MomReturnHomeFemale2::
	applywaitmovement LOCALID_TWINLEAF_MOM, TwinleafTown_Movement_MomReturnHomeFemale2
	opendoor VAR_0x8009, VAR_0x800A
	waitdooranim
	applywaitmovement LOCALID_TWINLEAF_MOM, TwinleafTown_Movement_MomExitThroughDoor
	hideobjectat LOCALID_TWINLEAF_MOM, MAP_TWINLEAF_TOWN
	closedoor VAR_0x8009, VAR_0x800A
	waitdooranim
	return

TwinleafTown_EventScript_MomReturnHomeFemale3::
	applywaitmovement LOCALID_TWINLEAF_MOM, TwinleafTown_Movement_MomReturnHomeFemale3
	opendoor VAR_0x8009, VAR_0x800A
	waitdooranim
	applywaitmovement LOCALID_TWINLEAF_MOM, TwinleafTown_Movement_MomExitThroughDoor
	hideobjectat LOCALID_TWINLEAF_MOM, MAP_TWINLEAF_TOWN
	closedoor VAR_0x8009, VAR_0x800A
	waitdooranim
	return

TwinleafTown_EventScript_MomReturnHomeFemale4::
	applywaitmovement LOCALID_TWINLEAF_MOM, TwinleafTown_Movement_MomReturnHomeFemale4
	opendoor VAR_0x8009, VAR_0x800A
	waitdooranim
	applywaitmovement LOCALID_TWINLEAF_MOM, TwinleafTown_Movement_MomExitThroughDoor
	hideobjectat LOCALID_TWINLEAF_MOM, MAP_TWINLEAF_TOWN
	closedoor VAR_0x8009, VAR_0x800A
	waitdooranim
	return

TwinleafTown_EventScript_MomReturnHomeFemale5::
	applywaitmovement LOCALID_TWINLEAF_MOM, TwinleafTown_Movement_MomReturnHomeFemale5
	opendoor VAR_0x8009, VAR_0x800A
	waitdooranim
	applywaitmovement LOCALID_TWINLEAF_MOM, TwinleafTown_Movement_MomExitThroughDoor
	hideobjectat LOCALID_TWINLEAF_MOM, MAP_TWINLEAF_TOWN
	closedoor VAR_0x8009, VAR_0x800A
	waitdooranim
	return

TwinleafTown_Movement_MomApproachPlayer0:
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	step_end

TwinleafTown_Movement_MomApproachPlayer1:
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	step_end

TwinleafTown_Movement_MomApproachPlayerFemale2:
	walk_left
	walk_left
	walk_left
	step_end

TwinleafTown_Movement_MomApproachPlayerFemale3:
	walk_left
	walk_left
	step_end

TwinleafTown_Movement_MomApproachPlayerFemale4:
	walk_left
	walk_left
	walk_left
	walk_left
	walk_left
	step_end

TwinleafTown_Movement_MomApproachPlayerFemale5:
	walk_left
	walk_left
	walk_left
	walk_left
	step_end

TwinleafTown_Movement_MomReturnHome0:
	walk_down
	walk_down
	walk_down
	walk_down
	walk_down
	step_end

TwinleafTown_Movement_MomReturnHome1:
	walk_down
	walk_down
	walk_down
	walk_down
	walk_down
	step_end

TwinleafTown_Movement_MomReturnHomeFemale2:
	walk_right
	walk_right
	walk_right
	walk_in_place_faster_up
	step_end

TwinleafTown_Movement_MomReturnHomeFemale3:
	walk_right
	walk_right
	walk_in_place_faster_up
	step_end

TwinleafTown_Movement_MomReturnHomeFemale4:
	walk_right
	walk_right
	walk_right
	walk_right
	walk_right
	walk_in_place_faster_up
	step_end

TwinleafTown_Movement_MomReturnHomeFemale5:
	walk_right
	walk_right
	walk_right
	walk_right
	walk_in_place_faster_up
	step_end

TwinleafTown_Movement_MomExitThroughDoor:
	walk_up
	step_end

TwinleafTown_EventScript_Mom::
	lock
	faceplayer
	call TwinleafTown_EventScript_SetHomeDoorCoords
	call TwinleafTown_EventScript_GiveRunningShoes
	applywaitmovement LOCALID_TWINLEAF_MOM, Common_Movement_WalkInPlaceFasterUp
	opendoor VAR_0x8009, VAR_0x800A
	waitdooranim
	applywaitmovement LOCALID_TWINLEAF_MOM, TwinleafTown_Movement_MomExitThroughDoor
	hideobjectat LOCALID_TWINLEAF_MOM, MAP_TWINLEAF_TOWN
	closedoor VAR_0x8009, VAR_0x800A
	waitdooranim
	goto TwinleafTown_EventScript_SetReceivedRunningShoes
	end

TwinleafTown_EventScript_SetReceivedRunningShoes::
	removeobject LOCALID_TWINLEAF_MOM
	setflag FLAG_SYS_B_DASH
	setvar VAR_TWINLEAF_TOWN_STATE, 4
	release
	end

TwinleafTown_EventScript_GiveRunningShoes::
	msgbox TwinleafTown_Text_WearTheseRunningShoes, MSGBOX_DEFAULT
	playfanfare MUS_OBTAIN_ITEM
	message TwinleafTown_Text_SwitchShoesWithRunningShoes
	waitfanfare
	setflag FLAG_RECEIVED_RUNNING_SHOES
	msgbox TwinleafTown_Text_ExplainRunningShoes, MSGBOX_DEFAULT
	msgbox TwinleafTown_Text_ComeHomeIfAnythingHappens, MSGBOX_DEFAULT
	closemessage
	delay 30
	return

TwinleafTown_Text_OurNewHomeLetsGoInside:
	.string "MOM: {PLAYER}, we're here, honey!\p"
	.string "It must be tiring riding with our things\n"
	.string "in the moving truck.\p"
	.string "Well, this is TWINLEAF TOWN.\p"
	.string "How do you like it?\n"
	.string "This is our new home!\p"
	.string "It has a quaint feel, but it seems to be\n"
	.string "an easy place to live, don't you think?\p"
	.string "And, you get your own room, {PLAYER}!\n"
	.string "Let's go inside.$"

TwinleafTown_Text_WaitPlayer:
	.string "MOM: Wait, {PLAYER}!$"

TwinleafTown_Text_WearTheseRunningShoes:
	.string "MOM: {PLAYER}! {PLAYER}! Did you\n"
	.string "introduce yourself to PROF. BIRCH?\p"
	.string "Oh! What an adorable POKéMON!\n"
	.string "You got it from PROF. BIRCH. How nice!\p"
	.string "You're your father's child, all right.\n"
	.string "You look good together with POKéMON!\p"
	.string "Here, honey! If you're going out on an\n"
	.string "adventure, wear these RUNNING SHOES.\p"
	.string "They'll put a zip in your step!$"

TwinleafTown_Text_SwitchShoesWithRunningShoes:
	.string "{PLAYER} switched shoes with the\n"
	.string "RUNNING SHOES.$"

TwinleafTown_Text_ExplainRunningShoes:
	.string "MOM: {PLAYER}, those shoes came with\n"
	.string "instructions.\p"
	.string "“Press the B Button while wearing these\n"
	.string "RUNNING SHOES to run extra-fast!\p"
	.string "“Slip on these RUNNING SHOES and race\n"
	.string "in the great outdoors!”$"

TwinleafTown_Text_ComeHomeIfAnythingHappens:
	.string "… … … … … … … …\n"
	.string "… … … … … … … …\p"
	.string "To think that you have your very own\n"
	.string "POKéMON now…\l"
	.string "Your father will be overjoyed.\p"
	.string "…But please be careful.\n"
	.string "If anything happens, you can come home.\p"
	.string "Go on, go get them, honey!$"

TwinleafTown_Text_CanUsePCToStoreItems:
	.string "If you use a PC, you can store items\n"
	.string "and POKéMON.\p"
	.string "The power of science is staggering!$"

TwinleafTown_Text_BirchSpendsDaysInLab:
	.string "PROF. BIRCH spends days in his LAB\n"
	.string "studying, then he'll suddenly go out in\l"
	.string "the wild to do more research…\p"
	.string "When does PROF. BIRCH spend time\n"
	.string "at home?$"

TwinleafTown_Text_IfYouGoInGrassPokemonWillJumpOut:
	.string "Um, um, um!\p"
	.string "If you go outside and go in the grass,\n"
	.string "wild POKéMON will jump out!$"

TwinleafTown_Text_DangerousIfYouDontHavePokemon:
	.string "It's dangerous if you don't have\n"
	.string "your own POKéMON.$"

TwinleafTown_Text_CanYouGoSeeWhatsHappening:
	.string "Um, hi!\p"
	.string "There are scary POKéMON outside!\n"
	.string "I can hear their cries!\p"
	.string "I want to go see what's going on,\n"
	.string "but I don't have any POKéMON…\p"
	.string "Can you go see what's happening\n"
	.string "for me?$"

TwinleafTown_Text_YouSavedBirch:
	.string "You saved PROF. BIRCH!\n"
	.string "I'm so glad!$"

TwinleafTown_Text_GoodLuckCatchingPokemon:
	.string "Are you going to catch POKéMON?\n"
	.string "Good luck!$"

TwinleafTown_Text_TownSign:
	.string "TWINLEAF TOWN\n"
	.string "“Fresh and free!”\p"
	.string "A quiet little town where the\n"
	.string "breeze smells of new leaves.$"

TwinleafTown_Text_ProfBirchsLab:
	.string "PROF. BIRCH'S POKéMON LAB$"

TwinleafTown_Text_PlayersHouse:
	.string "{PLAYER}'s HOUSE$"

TwinleafTown_Text_RivalsHouse:
	.string "{RIVAL}'s HOUSE$"

TwinleafTown_Text_BirchSomethingToShowYouAtLab:
	.string "PROF. BIRCH: Well, well, {PLAYER}{KUN}!\n"
	.string "That was good work out there!\p"
	.string "I knew there was something special\n"
	.string "about you when I first saw you,\l"
	.string "but I never expected this.\p"
	.string "Oh, yes. Do you still have the POKéDEX\n"
	.string "I gave you?\p"
	.string "I have something to show you.\n"
	.string "Let's go to my LAB.$"

TwinleafTown_Text_RivalMeetMeOnRoute201:
	.string "{RIVAL}: Oof! Hey, watch where\n"
	.string "you're standing!\p"
	.string "…Oh! {PLAYER}! Perfect timing!\n"
	.string "I was just coming to get you!\p"
	.string "I've got the best idea ever!\n"
	.string "Meet me on ROUTE 201!\p"
	.string "It's just north of town.\n"
	.string "Hurry up, or you're buying lunch!$"

TwinleafTown_Text_RivalWaitUp:
	.string "{PLAYER}! Hey, wait up!$"

TwinleafTown_Text_RivalYouReadMyMind:
	.string "{RIVAL}: Huff… huff…\n"
	.string "There you are, {PLAYER}!\p"
	.string "I've been looking all over for you!\p"
	.string "…Wait, you're off to ROUTE 201?\n"
	.string "You read my mind!\p"
	.string "That's where I was headed too!\n"
	.string "Last one there buys lunch!$"
