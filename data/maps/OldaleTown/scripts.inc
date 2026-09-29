@ This map is Sandgem Town. It was Oldale Town and keeps its internal name
@ (MAP_OLDALE_TOWN). Prof. Birch's lab is here now.
OldaleTown_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, OldaleTown_OnTransition
	.byte 0

OldaleTown_OnTransition:
	call Common_EventScript_SetupRivalGfxId
	call Common_EventScript_SetupAssistantGfxId
	setflag FLAG_VISITED_OLDALE_TOWN
	call_if_set FLAG_ADVENTURE_STARTED, OldaleTown_EventScript_SetOldaleState
	end

@ This script seems pointless because nothing uses VAR_OLDALE_TOWN_STATE
OldaleTown_EventScript_SetOldaleState::
	setvar VAR_OLDALE_TOWN_STATE, 1
	return

OldaleTown_EventScript_TownSign::
	msgbox OldaleTown_Text_TownSign, MSGBOX_SIGN
	end

OldaleTown_EventScript_BirchsLabSign::
	msgbox OldaleTown_Text_BirchsLab, MSGBOX_SIGN
	end

OldaleTown_EventScript_Girl::
	msgbox OldaleTown_Text_SavingMyProgress, MSGBOX_NPC
	end

@ Stands by the MART door and hands out a promotional POTION
OldaleTown_EventScript_MartEmployee::
	lock
	faceplayer
	goto_if_set FLAG_RECEIVED_POTION_OLDALE, OldaleTown_EventScript_ExplainPotion
	msgbox OldaleTown_Text_IWorkAtPokemonMart, MSGBOX_DEFAULT
	msgbox OldaleTown_Text_ThisIsAPokemonMart, MSGBOX_DEFAULT
	giveitem ITEM_POTION
	goto_if_eq VAR_RESULT, FALSE, OldaleTown_EventScript_BagIsFull
	msgbox OldaleTown_Text_PotionExplanation, MSGBOX_DEFAULT
	setflag FLAG_RECEIVED_POTION_OLDALE
	release
	end

OldaleTown_EventScript_ExplainPotion::
	msgbox OldaleTown_Text_PotionExplanation, MSGBOX_DEFAULT
	release
	end

OldaleTown_EventScript_BagIsFull::
	msgbox gText_TooBadBagIsFull, MSGBOX_DEFAULT
	release
	end

@ Sketching footprints on the beach
OldaleTown_EventScript_FootprintsMan::
	lock
	faceplayer
	goto_if_set FLAG_ADVENTURE_STARTED, OldaleTown_EventScript_NotBlockingPath
	msgbox OldaleTown_Text_DiscoveredFootprints, MSGBOX_DEFAULT
	closemessage
	applywaitmovement LOCALID_FOOTPRINTS_MAN, Common_Movement_FaceOriginalDirection
	release
	end

OldaleTown_EventScript_NotBlockingPath::
	msgbox OldaleTown_Text_FinishedSketchingFootprints, MSGBOX_DEFAULT
	release
	end

@ After the Route 103 battle the rival waits on the lab mat (11, 7) and heads in
OldaleTown_EventScript_Rival::
	lockall
	applywaitmovement LOCALID_OLDALE_RIVAL, Common_Movement_FacePlayer
	checkplayergender
	call_if_eq VAR_RESULT, MALE, OldaleTown_EventScript_ShowMayMessage
	call_if_eq VAR_RESULT, FEMALE, OldaleTown_EventScript_ShowBrendanMessage
	closemessage
	applywaitmovement LOCALID_OLDALE_RIVAL, OldaleTown_Movement_RivalToLabDoor
	opendoor 11, 6
	waitdooranim
	applywaitmovement LOCALID_OLDALE_RIVAL, OldaleTown_Movement_RivalEnterLab
	removeobject LOCALID_OLDALE_RIVAL
	closedoor 11, 6
	waitdooranim
	setvar VAR_OLDALE_RIVAL_STATE, 2
	setflag FLAG_HIDE_OLDALE_TOWN_RIVAL
	releaseall
	end

OldaleTown_EventScript_ShowMayMessage::
	msgbox OldaleTown_Text_MayLetsGoBack, MSGBOX_DEFAULT
	return

OldaleTown_EventScript_ShowBrendanMessage::
	msgbox OldaleTown_Text_BrendanLetsGoBack, MSGBOX_DEFAULT
	return

OldaleTown_Movement_RivalToLabDoor:
	face_up
	step_end

OldaleTown_Movement_RivalEnterLab:
	walk_up
	set_invisible
	step_end

@ Stepping onto the lab mat (11, 7) after returning Birch's bag on Route 201.
@ The rival bursts out of the lab door (11, 6), knocks the player back and runs
@ west, then Birch's assistant comes out and leads the player inside.
OldaleTown_EventScript_ArriveAtLab::
	lockall
	applywaitmovement LOCALID_PLAYER, Common_Movement_WalkInPlaceFasterUp
	setobjectxyperm LOCALID_OLDALE_RIVAL, 11, 6
	setobjectmovementtype LOCALID_OLDALE_RIVAL, MOVEMENT_TYPE_FACE_DOWN
	opendoor 11, 6
	waitdooranim
	addobject LOCALID_OLDALE_RIVAL
	playse SE_WALL_HIT
	applymovement LOCALID_OLDALE_RIVAL, OldaleTown_Movement_RivalRushOutOfLab
	applymovement LOCALID_PLAYER, OldaleTown_Movement_PlayerKnockedBack
	waitmovement 0
	closedoor 11, 6
	waitdooranim
	playse SE_PIN
	applywaitmovement LOCALID_OLDALE_RIVAL, Common_Movement_ExclamationMark
	applywaitmovement LOCALID_OLDALE_RIVAL, Common_Movement_Delay48
	msgbox OldaleTown_Text_RivalProfessorIsNice, MSGBOX_DEFAULT
	closemessage
	applymovement LOCALID_OLDALE_RIVAL, OldaleTown_Movement_RivalRunWest
	applymovement LOCALID_PLAYER, OldaleTown_Movement_PlayerWatchRivalRunWest
	waitmovement LOCALID_OLDALE_RIVAL
	waitmovement LOCALID_PLAYER
	removeobject LOCALID_OLDALE_RIVAL
	delay 20
	opendoor 11, 6
	waitdooranim
	addobject LOCALID_SANDGEM_ASSISTANT
	applywaitmovement LOCALID_SANDGEM_ASSISTANT, OldaleTown_Movement_AssistantStepOut
	closedoor 11, 6
	waitdooranim
	msgbox OldaleTown_Text_AssistantBeenWaiting, MSGBOX_DEFAULT
	closemessage
	opendoor 11, 6
	waitdooranim
	applymovement LOCALID_SANDGEM_ASSISTANT, OldaleTown_Movement_AssistantEnterLab
	applymovement LOCALID_PLAYER, OldaleTown_Movement_PlayerFollowIntoLab
	waitmovement 0
	hideplayer
	closedoor 11, 6
	waitdooranim
	removeobject LOCALID_SANDGEM_ASSISTANT
	setvar VAR_ROUTE101_STATE, 6
	setvar VAR_BIRCH_LAB_STATE, 1
	clearflag FLAG_HIDE_LITTLEROOT_TOWN_BIRCHS_LAB_BIRCH
	warp MAP_LITTLEROOT_TOWN_PROFESSOR_BIRCHS_LAB, 6, 11
	waitstate
	releaseall
	end

OldaleTown_Movement_RivalRushOutOfLab:
	walk_faster_down
	step_end

@ From the mat back onto the main street (11, 9)
OldaleTown_Movement_PlayerKnockedBack:
	lock_facing_direction
	jump_down
	unlock_facing_direction
	step_end

@ Down to the main street and west toward Route 201
OldaleTown_Movement_RivalRunWest:
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

OldaleTown_Movement_PlayerWatchRivalRunWest:
	delay_8
	walk_in_place_faster_left
	step_end

OldaleTown_Movement_AssistantStepOut:
	walk_down
	step_end

OldaleTown_Movement_AssistantEnterLab:
	walk_up
	set_invisible
	step_end

OldaleTown_Movement_PlayerFollowIntoLab:
	walk_in_place_faster_up
	delay_16
	walk_up
	walk_up
	walk_up
	step_end

OldaleTown_Text_SavingMyProgress:
	.string "I want to take a rest, so I'm saving my\n"
	.string "progress.$"

OldaleTown_Text_IWorkAtPokemonMart:
	.string "Hi!\n"
	.string "I work at the POKéMON MART\l"
	.string "right here.$"

OldaleTown_Text_ThisIsAPokemonMart:
	.string "This is a POKéMON MART.\n"
	.string "Just look for our blue roof.\p"
	.string "We sell a variety of goods including\n"
	.string "POKé BALLS for catching POKéMON.\p"
	.string "Here, I'd like you to have this as\n"
	.string "a promotional item.$"

OldaleTown_Text_PotionExplanation:
	.string "A POTION can be used anytime, so it's\n"
	.string "even more useful than a POKéMON CENTER\l"
	.string "in certain situations.$"

OldaleTown_Text_DiscoveredFootprints:
	.string "I just discovered the footprints of\n"
	.string "a rare POKéMON!\p"
	.string "Wait until I finish sketching\n"
	.string "them, okay?$"

OldaleTown_Text_FinishedSketchingFootprints:
	.string "I finished sketching the footprints of\n"
	.string "a rare POKéMON.\p"
	.string "But it turns out they were only my\n"
	.string "own footprints…$"

OldaleTown_Text_MayLetsGoBack:
	.string "MAY: {PLAYER}{KUN}!\n"
	.string "Over here!\l"
	.string "Let's go to PROF. BIRCH's LAB!$"

OldaleTown_Text_BrendanLetsGoBack:
	.string "BRENDAN: I'm heading into\n"
	.string "PROF. BIRCH's LAB now.\l"
	.string "{PLAYER}, you should come in, too.$"

OldaleTown_Text_TownSign:
	.string "SANDGEM TOWN\n"
	.string "“The town of sand.”$"

OldaleTown_Text_BirchsLab:
	.string "PROF. BIRCH'S POKéMON LAB$"

OldaleTown_Text_RivalProfessorIsNice:
	.string "{RIVAL}: Whoa! Sorry, {PLAYER}!\n"
	.string "I didn't see you there!\p"
	.string "You're here to see PROF. BIRCH, too?\p"
	.string "Don't worry, he's not scary at all.\n"
	.string "He's actually really nice!\p"
	.string "Okay, I'm off! See ya!$"

OldaleTown_Text_AssistantBeenWaiting:
	.string "Oh! {PLAYER}!\n"
	.string "We've been waiting for you!\p"
	.string "The PROFESSOR is inside.\n"
	.string "Come on, follow me!$"
