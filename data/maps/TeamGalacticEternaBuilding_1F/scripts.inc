@ Team Galactic Eterna Building 1F, from Platinum's, over Mauville's old
@ Game Corner. The door from Eterna City (17,7) lands on the exit mat at
@ (11,14)-(12,14). The first time in (VAR_TEAM_GALACTIC_ETERNA_BUILDING_1F_STATE
@ 0 -> 1), a Grunt by the door turns out to be Looker in disguise. Both
@ stairways lead to 2F. Platinum's Rotom's Room door (2,1) is walled off.
@
@ Team Galactic's music plays on every floor until Jupiter is beaten on 4F,
@ which sets FLAG_HIDE_ETERNA_CITY_GALACTIC_GRUNTS and hides the Grunts
@ here, on the other floors and in Eterna City, and Looker too.
TeamGalacticEternaBuilding_1F_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, TeamGalacticEternaBuilding_1F_OnTransition
	map_script MAP_SCRIPT_ON_FRAME_TABLE, TeamGalacticEternaBuilding_1F_OnFrame
	.byte 0

TeamGalacticEternaBuilding_1F_OnTransition:
	call TeamGalacticEternaBuilding_1F_EventScript_TryPlayGalacticMusic
	end

@ Called by every floor.
TeamGalacticEternaBuilding_1F_EventScript_TryPlayGalacticMusic::
	goto_if_set FLAG_HIDE_ETERNA_CITY_GALACTIC_GRUNTS, Common_EventScript_NopReturn
	savebgm MUS_AQUA_MAGMA_HIDEOUT
	return

TeamGalacticEternaBuilding_1F_OnFrame:
	map_script_2 VAR_TEAM_GALACTIC_ETERNA_BUILDING_1F_STATE, 0, TeamGalacticEternaBuilding_1F_EventScript_LookerRevealsHimself
	.2byte 0

@ The player has just come in at (11,14); the disguised Looker at (14,14)
@ walks over to (12,14), spins round into himself, then back into a Grunt.
TeamGalacticEternaBuilding_1F_EventScript_LookerRevealsHimself::
	lockall
	applymovement LOCALID_TEAM_GALACTIC_ETERNA_BUILDING_1F_DISGUISED_LOOKER, TeamGalacticEternaBuilding_1F_Movement_LookerNoticeAndWalkToPlayer
	applymovement LOCALID_PLAYER, TeamGalacticEternaBuilding_1F_Movement_PlayerFaceLooker
	waitmovement 0
	msgbox TeamGalacticEternaBuilding_1F_Text_HelloItIsMe, MSGBOX_DEFAULT
	closemessage
	applymovement LOCALID_TEAM_GALACTIC_ETERNA_BUILDING_1F_DISGUISED_LOOKER, TeamGalacticEternaBuilding_1F_Movement_LookerSpin
	waitmovement 0
	clearflag FLAG_HIDE_TEAM_GALACTIC_ETERNA_BUILDING_1F_LOOKER
	setobjectxyperm LOCALID_TEAM_GALACTIC_ETERNA_BUILDING_1F_LOOKER, 12, 14
	setobjectmovementtype LOCALID_TEAM_GALACTIC_ETERNA_BUILDING_1F_LOOKER, MOVEMENT_TYPE_FACE_LEFT
	addobject LOCALID_TEAM_GALACTIC_ETERNA_BUILDING_1F_LOOKER
	removeobject LOCALID_TEAM_GALACTIC_ETERNA_BUILDING_1F_DISGUISED_LOOKER
	setvar VAR_TEAM_GALACTIC_ETERNA_BUILDING_1F_STATE, 1
	delay 30
	msgbox TeamGalacticEternaBuilding_1F_Text_FirstRateTrainer, MSGBOX_DEFAULT
	applymovement LOCALID_TEAM_GALACTIC_ETERNA_BUILDING_1F_LOOKER, TeamGalacticEternaBuilding_1F_Movement_LookerSpin
	waitmovement 0
	clearflag FLAG_HIDE_TEAM_GALACTIC_ETERNA_BUILDING_1F_DISGUISED_LOOKER
	setobjectxyperm LOCALID_TEAM_GALACTIC_ETERNA_BUILDING_1F_DISGUISED_LOOKER, 12, 14
	setobjectmovementtype LOCALID_TEAM_GALACTIC_ETERNA_BUILDING_1F_DISGUISED_LOOKER, MOVEMENT_TYPE_FACE_LEFT
	addobject LOCALID_TEAM_GALACTIC_ETERNA_BUILDING_1F_DISGUISED_LOOKER
	removeobject LOCALID_TEAM_GALACTIC_ETERNA_BUILDING_1F_LOOKER
	closemessage
	releaseall
	end

TeamGalacticEternaBuilding_1F_Movement_PlayerFaceLooker:
	delay_8
	delay_8
	delay_8
	delay_8
	delay_8
	delay_8
	delay_8
	delay_8
	delay_8
	walk_in_place_right
	step_end

TeamGalacticEternaBuilding_1F_Movement_LookerNoticeAndWalkToPlayer:
	walk_in_place_faster_left
	emote_exclamation_mark
	delay_8
	delay_8
	walk_left
	walk_left
	step_end

TeamGalacticEternaBuilding_1F_Movement_LookerSpin:
	face_down
	delay_2
	face_right
	delay_4
	face_up
	delay_4
	face_left
	delay_2
	step_end

TeamGalacticEternaBuilding_1F_EventScript_Looker::
	msgbox TeamGalacticEternaBuilding_1F_Text_InvestigationIntoTeamGalactic, MSGBOX_NPC
	end

TeamGalacticEternaBuilding_1F_EventScript_GruntM1::
	msgbox TeamGalacticEternaBuilding_1F_Text_WelcomeToTeamGalactic, MSGBOX_NPC
	end

TeamGalacticEternaBuilding_1F_EventScript_GruntM2::
	msgbox TeamGalacticEternaBuilding_1F_Text_SightsSetOnTheStars, MSGBOX_NPC
	end

TeamGalacticEternaBuilding_1F_EventScript_Grunt1::
	trainerbattle_single TRAINER_GRUNT_AQUA_HIDEOUT_1, TeamGalacticEternaBuilding_1F_Text_Grunt1Intro, TeamGalacticEternaBuilding_1F_Text_Grunt1Defeat
	msgbox TeamGalacticEternaBuilding_1F_Text_Grunt1PostBattle, MSGBOX_AUTOCLOSE
	end

TeamGalacticEternaBuilding_1F_EventScript_Grunt2::
	trainerbattle_single TRAINER_GRUNT_AQUA_HIDEOUT_5, TeamGalacticEternaBuilding_1F_Text_Grunt2Intro, TeamGalacticEternaBuilding_1F_Text_Grunt2Defeat
	msgbox TeamGalacticEternaBuilding_1F_Text_Grunt2PostBattle, MSGBOX_AUTOCLOSE
	end

TeamGalacticEternaBuilding_1F_EventScript_Sign::
	msgbox TeamGalacticEternaBuilding_1F_Text_PathLeadsToGlory, MSGBOX_SIGN
	end

TeamGalacticEternaBuilding_1F_Text_HelloItIsMe:
	.string "LOOKER: Hello! It is me! Me!\p"
	.string "Hahaha! I have startled you, yes?\p"
	.string "I am, after all, an officer of the\n"
	.string "International Police.\p"
	.string "It is only natural that I be a master of\n"
	.string "disguise.$"

TeamGalacticEternaBuilding_1F_Text_FirstRateTrainer:
	.string "…Incidentally, you are a first-rate\n"
	.string "TRAINER. Of that, I am very aware.\p"
	.string "I therefore think you will be OK, but\n"
	.string "I must warn you to be careful anyway.\p"
	.string "Also, a word of advice.\p"
	.string "This building, it has two stairs.\n"
	.string "One of them, a trap it must be.\p"
	.string "But the crooks of TEAM GALACTIC, they\n"
	.string "do not appear, how shall I say, smart.\p"
	.string "There should be an easy way to tell\n"
	.string "the stairs apart!$"

TeamGalacticEternaBuilding_1F_Text_InvestigationIntoTeamGalactic:
	.string "LOOKER: Now, I have been conducting\n"
	.string "my investigation into TEAM GALACTIC.\p"
	.string "But I have no clue as to who they are\n"
	.string "or what it is they wish to do.\p"
	.string "The minions, they appear ignorant of\n"
	.string "the intentions of their boss.\p"
	.string "Also, a word of advice.\p"
	.string "This building, it has two stairs.\n"
	.string "One of them, a trap it must be.\p"
	.string "But the crooks of TEAM GALACTIC, they\n"
	.string "do not appear, how shall I say, smart.\p"
	.string "There should be an easy way to tell\n"
	.string "the stairs apart!$"

TeamGalacticEternaBuilding_1F_Text_WelcomeToTeamGalactic:
	.string "Welcome to TEAM GALACTIC. Are you\n"
	.string "here to donate your POKéMON?\p"
	.string "What? You're not?\n"
	.string "Then kindly get off our property!\p"
	.string "Do we have to bring the pain like we\n"
	.string "did with the CYCLE SHOP manager?$"

TeamGalacticEternaBuilding_1F_Text_SightsSetOnTheStars:
	.string "As a group, TEAM GALACTIC has its\n"
	.string "sights set on the stars, not on this\l"
	.string "world!$"

TeamGalacticEternaBuilding_1F_Text_PathLeadsToGlory:
	.string "The path you chose leads to glory.$"

TeamGalacticEternaBuilding_1F_Text_Grunt1Intro:
	.string "We are acquiring POKéMON for a most\n"
	.string "astonishing undertaking!\p"
	.string "Your meddling won't be tolerated!$"

TeamGalacticEternaBuilding_1F_Text_Grunt1Defeat:
	.string "I feel like I got meddled with…$"

TeamGalacticEternaBuilding_1F_Text_Grunt1PostBattle:
	.string "Our astonishing undertaking is\n"
	.string "most certainly breathtaking.\p"
	.string "If you ever hope to see its glorious\n"
	.string "unveiling, then don't meddle with us!$"

TeamGalacticEternaBuilding_1F_Text_Grunt2Intro:
	.string "So sorry, sweetie.\n"
	.string "TEAM GALACTIC has no room for kiddies!$"

TeamGalacticEternaBuilding_1F_Text_Grunt2Defeat:
	.string "B-but, deary, you're just a kid!$"

TeamGalacticEternaBuilding_1F_Text_Grunt2PostBattle:
	.string "You wanted to go up?\n"
	.string "Well, why didn't you?$"
