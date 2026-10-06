TwinleafTown_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, TwinleafTown_OnTransition
	map_script MAP_SCRIPT_ON_FRAME_TABLE, TwinleafTown_OnFrame
	map_script MAP_SCRIPT_ON_WARP_INTO_MAP_TABLE, TwinleafTown_OnWarp
	.byte 0

	@ State descriptions for VAR_TWINLEAF_TOWN_STATE
	@ 1: Met Rival
	@ 3: Received Pokedex (Mom and the rival's mom are waiting at home)
	@ 4: Received Town Map and Running Shoes at home

	@ State descriptions for VAR_TWINLEAF_INTRO_STATE
	@ 8: Woke up in bedroom (set when a new game starts)
	@ 9: Mom said rival was looking for you
	@ 10: Met rival (door bump or caught up at north exit), rival left for Route 201

	@ State descriptions for VAR_TWINLEAF_HOUSES_STATE_MAY
	@                    and VAR_TWINLEAF_HOUSES_STATE_BRENDAN
	@ 3: Entered Hall of Fame
	@ 4: Completed Lati TV / SS Ticket event
	@ 5: Exited house after above event

	@ State descriptions for VAR_TWINLEAF_RIVAL_STATE
	@ 3: Met Rival
	@ 4: Received Pokedex


TwinleafTown_OnTransition:
	setflag FLAG_VISITED_TWINLEAF_TOWN
	call Common_EventScript_SetupRivalGfxId
	call_if_eq VAR_TWINLEAF_HOUSES_STATE_MAY, 4, TwinleafTown_EventScript_SetExitedHouseAfterLatiSSTicketEvent
	call_if_eq VAR_TWINLEAF_HOUSES_STATE_BRENDAN, 4, TwinleafTown_EventScript_SetExitedHouseAfterLatiSSTicketEvent
	call_if_eq VAR_DEX_UPGRADE_JOHTO_STARTER_STATE, 1, TwinleafTown_EventScript_HideMapNamePopup
	call_if_eq VAR_DEX_UPGRADE_JOHTO_STARTER_STATE, 2, TwinleafTown_EventScript_LeftLabAfterDexUpgrade
	end

TwinleafTown_EventScript_LeftLabAfterDexUpgrade::
	setvar VAR_DEX_UPGRADE_JOHTO_STARTER_STATE, 3
	return

TwinleafTown_EventScript_HideMapNamePopup::
	setflag FLAG_HIDE_MAP_NAME_POPUP
	return

TwinleafTown_EventScript_SetExitedHouseAfterLatiSSTicketEvent::
	setvar VAR_TWINLEAF_HOUSES_STATE_MAY, 5
	setvar VAR_TWINLEAF_HOUSES_STATE_BRENDAN, 5
	return

TwinleafTown_OnFrame:
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
	setvar VAR_TWINLEAF_RIVAL_STATE, 3
	setvar VAR_TWINLEAF_TOWN_STATE, 1
	return

@ Leaving town at the north exit (x 9-12, y 0) before meeting the rival.
@ The rival runs up from behind and heads to Route 201 with the player.
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
	call_if_lt VAR_0x8009, 11, TwinleafTown_EventScript_RivalRunAheadRight
	call_if_ge VAR_0x8009, 11, TwinleafTown_EventScript_RivalRunAheadLeft
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
	msgbox TwinleafTown_Text_ProfessorInSandgem, MSGBOX_NPC
	end

TwinleafTown_EventScript_Twin::
	lock
	faceplayer
	goto_if_set FLAG_ADVENTURE_STARTED, TwinleafTown_EventScript_GoodLuck
	goto_if_set FLAG_SYS_POKEMON_GET, TwinleafTown_EventScript_YouGotAPokemon
	msgbox TwinleafTown_Text_IfYouGoInGrassPokemonWillJumpOut, MSGBOX_DEFAULT
	release
	end

TwinleafTown_EventScript_YouGotAPokemon::
	msgbox TwinleafTown_Text_YouGotAPokemon, MSGBOX_DEFAULT
	release
	end

TwinleafTown_EventScript_GoodLuck::
	msgbox TwinleafTown_Text_GoodLuckCatchingPokemon, MSGBOX_DEFAULT
	release
	end

TwinleafTown_EventScript_TownSign::
	msgbox TwinleafTown_Text_TownSign, MSGBOX_SIGN
	end

@ Brendan's house is always the rival's house
TwinleafTown_EventScript_BrendansHouseSign::
	msgbox TwinleafTown_Text_RivalsHouse, MSGBOX_SIGN
	end

@ May's house is always the player's house
TwinleafTown_EventScript_MaysHouseSign::
	msgbox TwinleafTown_Text_PlayersHouse, MSGBOX_SIGN
	end

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

TwinleafTown_Text_ProfessorInSandgem:
	.string "There's a POKéMON PROFESSOR with\n"
	.string "a LAB over in SANDGEM TOWN.\p"
	.string "I hear he's hardly ever there, though.\n"
	.string "He's always out in the wild doing\l"
	.string "fieldwork!$"

TwinleafTown_Text_IfYouGoInGrassPokemonWillJumpOut:
	.string "Um, um, um!\p"
	.string "If you go outside and go in the grass,\n"
	.string "wild POKéMON will jump out!\p"
	.string "It's dangerous if you don't have\n"
	.string "your own POKéMON.$"

TwinleafTown_Text_YouGotAPokemon:
	.string "Wow! Is that your very own POKéMON?\p"
	.string "No fair! I want one too!$"

TwinleafTown_Text_GoodLuckCatchingPokemon:
	.string "Are you going to catch POKéMON?\n"
	.string "Good luck!$"

TwinleafTown_Text_TownSign:
	.string "TWINLEAF TOWN\n"
	.string "“Fresh and free!”\p"
	.string "A quiet little town where the\n"
	.string "breeze smells of new leaves.$"

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
