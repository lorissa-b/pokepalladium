@ Sandgem Town, drawn over Emerald's Oldale Town. Prof. Birch's lab is here now.
SandgemTown_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, SandgemTown_OnTransition
	.byte 0

SandgemTown_OnTransition:
	call Common_EventScript_SetupRivalGfxId
	call Common_EventScript_SetupAssistantGfxId
	setflag FLAG_VISITED_SANDGEM_TOWN
	call_if_set FLAG_ADVENTURE_STARTED, SandgemTown_EventScript_SetSandgemState
	end

@ This script seems pointless because nothing uses VAR_SANDGEM_TOWN_STATE
SandgemTown_EventScript_SetSandgemState::
	setvar VAR_SANDGEM_TOWN_STATE, 1
	return

SandgemTown_EventScript_TownSign::
	msgbox SandgemTown_Text_TownSign, MSGBOX_SIGN
	end

SandgemTown_EventScript_BirchsLabSign::
	msgbox SandgemTown_Text_BirchsLab, MSGBOX_SIGN
	end

SandgemTown_EventScript_Girl::
	msgbox SandgemTown_Text_SavingMyProgress, MSGBOX_NPC
	end

@ Stands by the MART door and hands out a promotional POTION
SandgemTown_EventScript_MartEmployee::
	lock
	faceplayer
	goto_if_set FLAG_RECEIVED_POTION_SANDGEM, SandgemTown_EventScript_ExplainPotion
	msgbox SandgemTown_Text_IWorkAtPokemonMart, MSGBOX_DEFAULT
	msgbox SandgemTown_Text_ThisIsAPokemonMart, MSGBOX_DEFAULT
	giveitem ITEM_POTION
	goto_if_eq VAR_RESULT, FALSE, SandgemTown_EventScript_BagIsFull
	msgbox SandgemTown_Text_PotionExplanation, MSGBOX_DEFAULT
	setflag FLAG_RECEIVED_POTION_SANDGEM
	release
	end

SandgemTown_EventScript_ExplainPotion::
	msgbox SandgemTown_Text_PotionExplanation, MSGBOX_DEFAULT
	release
	end

SandgemTown_EventScript_BagIsFull::
	msgbox gText_TooBadBagIsFull, MSGBOX_DEFAULT
	release
	end

@ Sketching footprints on the beach
SandgemTown_EventScript_FootprintsMan::
	lock
	faceplayer
	goto_if_set FLAG_ADVENTURE_STARTED, SandgemTown_EventScript_NotBlockingPath
	msgbox SandgemTown_Text_DiscoveredFootprints, MSGBOX_DEFAULT
	closemessage
	applywaitmovement LOCALID_FOOTPRINTS_MAN, Common_Movement_FaceOriginalDirection
	release
	end

SandgemTown_EventScript_NotBlockingPath::
	msgbox SandgemTown_Text_FinishedSketchingFootprints, MSGBOX_DEFAULT
	release
	end

@ After the Route 103 battle the rival waits on the lab mat (11, 7) and heads in
SandgemTown_EventScript_Rival::
	lockall
	applywaitmovement LOCALID_SANDGEM_RIVAL, Common_Movement_FacePlayer
	checkplayergender
	call_if_eq VAR_RESULT, MALE, SandgemTown_EventScript_ShowMayMessage
	call_if_eq VAR_RESULT, FEMALE, SandgemTown_EventScript_ShowBrendanMessage
	closemessage
	applywaitmovement LOCALID_SANDGEM_RIVAL, SandgemTown_Movement_RivalToLabDoor
	opendoor 11, 6
	waitdooranim
	applywaitmovement LOCALID_SANDGEM_RIVAL, SandgemTown_Movement_RivalEnterLab
	removeobject LOCALID_SANDGEM_RIVAL
	closedoor 11, 6
	waitdooranim
	setvar VAR_SANDGEM_RIVAL_STATE, 2
	setflag FLAG_HIDE_SANDGEM_TOWN_RIVAL
	releaseall
	end

SandgemTown_EventScript_ShowMayMessage::
	msgbox SandgemTown_Text_MayLetsGoBack, MSGBOX_DEFAULT
	return

SandgemTown_EventScript_ShowBrendanMessage::
	msgbox SandgemTown_Text_BrendanLetsGoBack, MSGBOX_DEFAULT
	return

SandgemTown_Movement_RivalToLabDoor:
	face_up
	step_end

SandgemTown_Movement_RivalEnterLab:
	walk_up
	set_invisible
	step_end

@ Stepping onto the lab mat (11, 7) after returning Birch's bag on Route 201.
@ The rival bursts out of the lab door (11, 6), knocks the player back and runs
@ west, then Birch's assistant comes out and leads the player inside.
SandgemTown_EventScript_ArriveAtLab::
	lockall
	applywaitmovement LOCALID_PLAYER, Common_Movement_WalkInPlaceFasterUp
	setobjectxyperm LOCALID_SANDGEM_RIVAL, 11, 6
	setobjectmovementtype LOCALID_SANDGEM_RIVAL, MOVEMENT_TYPE_FACE_DOWN
	opendoor 11, 6
	waitdooranim
	addobject LOCALID_SANDGEM_RIVAL
	playse SE_WALL_HIT
	applymovement LOCALID_SANDGEM_RIVAL, SandgemTown_Movement_RivalRushOutOfLab
	applymovement LOCALID_PLAYER, SandgemTown_Movement_PlayerKnockedBack
	waitmovement 0
	closedoor 11, 6
	waitdooranim
	playse SE_PIN
	applywaitmovement LOCALID_SANDGEM_RIVAL, Common_Movement_ExclamationMark
	applywaitmovement LOCALID_SANDGEM_RIVAL, Common_Movement_Delay48
	msgbox SandgemTown_Text_RivalProfessorIsNice, MSGBOX_DEFAULT
	closemessage
	applymovement LOCALID_SANDGEM_RIVAL, SandgemTown_Movement_RivalRunWest
	applymovement LOCALID_PLAYER, SandgemTown_Movement_PlayerWatchRivalRunWest
	waitmovement LOCALID_SANDGEM_RIVAL
	waitmovement LOCALID_PLAYER
	removeobject LOCALID_SANDGEM_RIVAL
	delay 20
	opendoor 11, 6
	waitdooranim
	addobject LOCALID_SANDGEM_ASSISTANT
	applywaitmovement LOCALID_SANDGEM_ASSISTANT, SandgemTown_Movement_AssistantStepOut
	closedoor 11, 6
	waitdooranim
	msgbox SandgemTown_Text_AssistantBeenWaiting, MSGBOX_DEFAULT
	closemessage
	opendoor 11, 6
	waitdooranim
	applymovement LOCALID_SANDGEM_ASSISTANT, SandgemTown_Movement_AssistantEnterLab
	applymovement LOCALID_PLAYER, SandgemTown_Movement_PlayerFollowIntoLab
	waitmovement 0
	hideplayer
	closedoor 11, 6
	waitdooranim
	removeobject LOCALID_SANDGEM_ASSISTANT
	setvar VAR_ROUTE201_STATE, 6
	setvar VAR_BIRCH_LAB_STATE, 1
	clearflag FLAG_HIDE_SANDGEM_TOWN_RESEARCH_LAB_BIRCH
	warp MAP_SANDGEM_TOWN_POKEMON_RESEARCH_LAB, 6, 11
	waitstate
	releaseall
	end

SandgemTown_Movement_RivalRushOutOfLab:
	walk_faster_down
	step_end

@ From the mat back onto the main street (11, 9)
SandgemTown_Movement_PlayerKnockedBack:
	lock_facing_direction
	jump_down
	unlock_facing_direction
	step_end

@ Down to the main street and west toward Route 201
SandgemTown_Movement_RivalRunWest:
	walk_fast_down
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

SandgemTown_Movement_PlayerWatchRivalRunWest:
	delay_8
	walk_in_place_faster_left
	step_end

SandgemTown_Movement_AssistantStepOut:
	walk_down
	step_end

SandgemTown_Movement_AssistantEnterLab:
	walk_up
	set_invisible
	step_end

SandgemTown_Movement_PlayerFollowIntoLab:
	walk_in_place_faster_up
	delay_16
	walk_up
	walk_up
	walk_up
	step_end

SandgemTown_Text_SavingMyProgress:
	.string "I want to take a rest, so I'm saving my\n"
	.string "progress.$"

SandgemTown_Text_IWorkAtPokemonMart:
	.string "Hi!\n"
	.string "I work at the POKéMON MART\l"
	.string "right here.$"

SandgemTown_Text_ThisIsAPokemonMart:
	.string "This is a POKéMON MART.\n"
	.string "Just look for our blue roof.\p"
	.string "We sell a variety of goods including\n"
	.string "POKé BALLS for catching POKéMON.\p"
	.string "Here, I'd like you to have this as\n"
	.string "a promotional item.$"

SandgemTown_Text_PotionExplanation:
	.string "A POTION can be used anytime, so it's\n"
	.string "even more useful than a POKéMON CENTER\l"
	.string "in certain situations.$"

SandgemTown_Text_DiscoveredFootprints:
	.string "I just discovered the footprints of\n"
	.string "a rare POKéMON!\p"
	.string "Wait until I finish sketching\n"
	.string "them, okay?$"

SandgemTown_Text_FinishedSketchingFootprints:
	.string "I finished sketching the footprints of\n"
	.string "a rare POKéMON.\p"
	.string "But it turns out they were only my\n"
	.string "own footprints…$"

SandgemTown_Text_MayLetsGoBack:
	.string "MAY: {PLAYER}{KUN}!\n"
	.string "Over here!\l"
	.string "Let's go to PROF. BIRCH's LAB!$"

SandgemTown_Text_BrendanLetsGoBack:
	.string "BRENDAN: I'm heading into\n"
	.string "PROF. BIRCH's LAB now.\l"
	.string "{PLAYER}, you should come in, too.$"

SandgemTown_Text_TownSign:
	.string "SANDGEM TOWN\n"
	.string "“The town of sand.”$"

SandgemTown_Text_BirchsLab:
	.string "PROF. BIRCH'S POKéMON LAB$"

SandgemTown_Text_RivalProfessorIsNice:
	.string "{RIVAL}: Whoa! Sorry, {PLAYER}!\n"
	.string "I didn't see you there!\p"
	.string "You're here to see PROF. BIRCH, too?\p"
	.string "Don't worry, he's not scary at all.\n"
	.string "He's actually really nice!\p"
	.string "Okay, I'm off! See ya!$"

SandgemTown_Text_AssistantBeenWaiting:
	.string "Oh! {PLAYER}!\n"
	.string "We've been waiting for you!\p"
	.string "The PROFESSOR is inside.\n"
	.string "Come on, follow me!$"
