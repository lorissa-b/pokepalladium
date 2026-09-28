@ This map is Lake Verity. It was the Safari Zone's south-west area and keeps its
@ internal name (MAP_SAFARI_ZONE_SOUTHWEST). Its story progress is tracked with
@ VAR_ROUTE101_STATE, continuing on from Route 201 (see Route101/scripts.inc).
SafariZone_Southwest_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, LakeVerity_OnTransition
	.byte 0

LakeVerity_OnTransition:
	call Common_EventScript_SetupRivalGfxId
	end

@ Walking up from the entrance (x 38-43, y 30): the rival is standing by Birch's bag
LakeVerity_EventScript_RivalFoundBag::
	lockall
	playse SE_PIN
	applywaitmovement LOCALID_LAKE_VERITY_RIVAL, Common_Movement_ExclamationMark
	applywaitmovement LOCALID_LAKE_VERITY_RIVAL, Common_Movement_WalkInPlaceFasterDown
	msgbox LakeVerity_Text_RivalFoundBag, MSGBOX_DEFAULT
	closemessage
	applywaitmovement LOCALID_LAKE_VERITY_RIVAL, Common_Movement_WalkInPlaceFasterUp
	setvar VAR_ROUTE101_STATE, 3
	releaseall
	end

LakeVerity_EventScript_Rival::
	lock
	faceplayer
	msgbox LakeVerity_Text_RivalBringItBack, MSGBOX_DEFAULT
	applywaitmovement LOCALID_LAKE_VERITY_RIVAL, Common_Movement_FaceUp
	release
	end

@ Trying to pick up the bag: a wild Starly attacks and the player grabs a starter
LakeVerity_EventScript_BirchsBag::
	lockall
	msgbox LakeVerity_Text_ReachForBag, MSGBOX_DEFAULT
	closemessage
	playmoncry SPECIES_STARLY, CRY_MODE_ENCOUNTER
	waitmoncry
	playse SE_PIN
	applywaitmovement LOCALID_LAKE_VERITY_RIVAL, Common_Movement_ExclamationMark
	msgbox LakeVerity_Text_RivalWildStarly, MSGBOX_DEFAULT
	closemessage
	special ChooseStarter
	waitstate
	special HealPlayerParty
	setflag FLAG_SYS_POKEMON_GET
	setflag FLAG_RESCUED_BIRCH
	applywaitmovement LOCALID_LAKE_VERITY_RIVAL, Common_Movement_FacePlayer
	msgbox LakeVerity_Text_RivalWeShouldReturnIt, MSGBOX_DEFAULT
	closemessage
	removeobject LOCALID_LAKE_VERITY_BAG
	applywaitmovement LOCALID_LAKE_VERITY_RIVAL, LakeVerity_Movement_RivalLeaveWithBag
	removeobject LOCALID_LAKE_VERITY_RIVAL
	setvar VAR_ROUTE101_STATE, 4
	releaseall
	end

@ From 42, 28 back down the exit path toward Route 201
LakeVerity_Movement_RivalLeaveWithBag:
	walk_fast_down
	walk_fast_down
	walk_fast_down
	walk_fast_left
	walk_fast_down
	walk_fast_down
	walk_fast_down
	walk_fast_down
	walk_fast_down
	walk_fast_down
	walk_fast_down
	walk_fast_down
	step_end

LakeVerity_Text_RivalFoundBag:
	.string "{RIVAL}: {PLAYER}! Over here!\p"
	.string "Look, somebody left a bag in the grass.\p"
	.string "…It must belong to the PROFESSOR and\n"
	.string "that kid who just left!$"

LakeVerity_Text_RivalBringItBack:
	.string "{RIVAL}: That's the PROFESSOR's bag,\n"
	.string "for sure. We should bring it back!$"

LakeVerity_Text_ReachForBag:
	.string "{PLAYER} reached for the bag…$"

LakeVerity_Text_RivalWildStarly:
	.string "{RIVAL}: Wh-what was that?!\p"
	.string "A wild STARLY! It's diving at us!\p"
	.string "There are POKé BALLS in the bag!\n"
	.string "Quick, {PLAYER}, use one!$"

LakeVerity_Text_RivalWeShouldReturnIt:
	.string "{RIVAL}: Wow… You did it, {PLAYER}!\p"
	.string "…But that was the PROFESSOR's POKéMON.\n"
	.string "We'd better bring the bag back to him\l"
	.string "and explain what happened.\p"
	.string "I'll carry it! Come on!$"
