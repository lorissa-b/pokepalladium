@ Valley Windworks building, drawn over Emerald's Lanette's House from
@ Platinum's map. The Grunt who locked the door is waiting just inside and
@ runs off to warn Mars the first time the player comes in
@ (VAR_VALLEY_WINDWORKS_TEAM_GALACTIC_STATE 0 -> 1). Two Grunts are trainers on
@ the way, and Mars stops the player crossing x=19. Beating her clears Team
@ Galactic out of here and off Route 205 South's bridge, sets
@ VAR_VALLEY_WINDWORKS_STATE to 2, reunites the little girl from Route 205 South
@ with her papa, and sets VAR_VALLEY_WINDWORKS_LOOKER_STATE to 1 so Looker
@ meets the player outside.
ValleyWindworks_Building_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, ValleyWindworks_Building_OnTransition
	map_script MAP_SCRIPT_ON_FRAME_TABLE, ValleyWindworks_Building_OnFrame
	.byte 0

ValleyWindworks_Building_OnTransition:
	call_if_lt VAR_VALLEY_WINDWORKS_TEAM_GALACTIC_STATE, 2, ValleyWindworks_Building_EventScript_PlayGalacticMusic
	call_if_ge VAR_VALLEY_WINDWORKS_STATE, 2, ValleyWindworks_Building_EventScript_SetGirlAndPapaPositions
	end

ValleyWindworks_Building_EventScript_PlayGalacticMusic::
	savebgm MUS_AQUA_MAGMA_HIDEOUT
	return

ValleyWindworks_Building_EventScript_SetGirlAndPapaPositions::
	setobjectxyperm LOCALID_VALLEY_WINDWORKS_BUILDING_LITTLE_GIRL, 21, 3
	setobjectmovementtype LOCALID_VALLEY_WINDWORKS_BUILDING_LITTLE_GIRL, MOVEMENT_TYPE_FACE_LEFT
	setobjectxyperm LOCALID_VALLEY_WINDWORKS_BUILDING_PAPA, 18, 2
	setobjectmovementtype LOCALID_VALLEY_WINDWORKS_BUILDING_PAPA, MOVEMENT_TYPE_FACE_DOWN
	return

ValleyWindworks_Building_OnFrame:
	map_script_2 VAR_VALLEY_WINDWORKS_TEAM_GALACTIC_STATE, 0, ValleyWindworks_Building_EventScript_DoorGruntRunsOff
	.2byte 0

@ The player has just come in on the mat at (12,14).
ValleyWindworks_Building_EventScript_DoorGruntRunsOff::
	lockall
	applymovement LOCALID_VALLEY_WINDWORKS_BUILDING_DOOR_GRUNT, Common_Movement_ExclamationMark
	waitmovement 0
	msgbox ValleyWindworks_Building_Text_GotToAlertCommander, MSGBOX_DEFAULT
	closemessage
	applymovement LOCALID_VALLEY_WINDWORKS_BUILDING_DOOR_GRUNT, ValleyWindworks_Building_Movement_DoorGruntLeave
	applymovement LOCALID_PLAYER, ValleyWindworks_Building_Movement_PlayerWatchDoorGruntLeave
	waitmovement 0
	removeobject LOCALID_VALLEY_WINDWORKS_BUILDING_DOOR_GRUNT
	setvar VAR_VALLEY_WINDWORKS_TEAM_GALACTIC_STATE, 1
	releaseall
	end

ValleyWindworks_Building_Movement_DoorGruntLeave:
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

ValleyWindworks_Building_Movement_PlayerWatchDoorGruntLeave:
	delay_8
	walk_in_place_faster_left
	step_end

ValleyWindworks_Building_EventScript_Grunt::
	msgbox ValleyWindworks_Building_Text_CommanderWillSmooshYou, MSGBOX_NPC
	end

ValleyWindworks_Building_EventScript_Grunt2::
	trainerbattle_single TRAINER_NOLAN, ValleyWindworks_Building_Text_Grunt2Intro, ValleyWindworks_Building_Text_Grunt2Defeat
	msgbox ValleyWindworks_Building_Text_Grunt2PostBattle, MSGBOX_AUTOCLOSE
	end

ValleyWindworks_Building_EventScript_Grunt3::
	trainerbattle_single TRAINER_CLAUDE, ValleyWindworks_Building_Text_Grunt3Intro, ValleyWindworks_Building_Text_Grunt3Defeat
	msgbox ValleyWindworks_Building_Text_Grunt3PostBattle, MSGBOX_AUTOCLOSE
	end

@ Triggered at (19,4) or (19,5) once the door Grunt has run off.
ValleyWindworks_Building_EventScript_Mars::
	lockall
	getplayerxy VAR_0x8004, VAR_0x8005
	goto_if_eq VAR_0x8005, 4, ValleyWindworks_Building_EventScript_MarsNoticeAndWalkToPlayer
	applymovement LOCALID_VALLEY_WINDWORKS_BUILDING_MARS, ValleyWindworks_Building_Movement_MarsNoticePlayer
	waitmovement 0
	goto ValleyWindworks_Building_EventScript_BattleMars
	end

ValleyWindworks_Building_EventScript_MarsNoticeAndWalkToPlayer::
	applymovement LOCALID_VALLEY_WINDWORKS_BUILDING_MARS, ValleyWindworks_Building_Movement_MarsNoticeAndWalkToPlayer
	waitmovement 0
	goto ValleyWindworks_Building_EventScript_BattleMars
	end

ValleyWindworks_Building_EventScript_BattleMars::
	applymovement LOCALID_PLAYER, Common_Movement_WalkInPlaceFasterRight
	waitmovement 0
	msgbox ValleyWindworks_Building_Text_LetsHaveABattle, MSGBOX_DEFAULT
	closemessage
	trainerbattle_no_intro TRAINER_SHANE, ValleyWindworks_Building_Text_MarsDefeat
	msgbox ValleyWindworks_Building_Text_IMessedThatOneUp, MSGBOX_DEFAULT
	closemessage
	applymovement LOCALID_VALLEY_WINDWORKS_BUILDING_CHARON, Common_Movement_WalkInPlaceFasterDown
	waitmovement 0
	delay 15
	msgbox ValleyWindworks_Building_Text_LoseToAChild, MSGBOX_DEFAULT
	applymovement LOCALID_VALLEY_WINDWORKS_BUILDING_MARS, Common_Movement_WalkInPlaceFasterUp
	waitmovement 0
	msgbox ValleyWindworks_Building_Text_WillYouShutIt, MSGBOX_DEFAULT
	applymovement LOCALID_VALLEY_WINDWORKS_BUILDING_MARS, Common_Movement_WalkInPlaceFasterLeft
	waitmovement 0
	msgbox ValleyWindworks_Building_Text_LeaveForTheTimeBeing, MSGBOX_DEFAULT
	closemessage
	fadescreen FADE_TO_BLACK
	removeobject LOCALID_VALLEY_WINDWORKS_BUILDING_MARS
	removeobject LOCALID_VALLEY_WINDWORKS_BUILDING_CHARON
	removeobject LOCALID_VALLEY_WINDWORKS_BUILDING_GRUNT
	removeobject LOCALID_VALLEY_WINDWORKS_BUILDING_GRUNT_2
	removeobject LOCALID_VALLEY_WINDWORKS_BUILDING_GRUNT_3
	setflag FLAG_HIDE_ROUTE_205_SOUTH_GRUNTS
	setvar VAR_VALLEY_WINDWORKS_STATE, 2
	fadedefaultbgm
	fadescreen FADE_FROM_BLACK
	getplayerxy VAR_0x8004, VAR_0x8005
	goto_if_eq VAR_0x8005, 4, ValleyWindworks_Building_EventScript_ReunionNorth
	goto ValleyWindworks_Building_EventScript_ReunionSouth
	end

@ Player at (19,4): Papa comes to (20,4), the girl runs to (20,5) below him.
ValleyWindworks_Building_EventScript_ReunionNorth::
	applymovement LOCALID_VALLEY_WINDWORKS_BUILDING_PAPA, ValleyWindworks_Building_Movement_PapaWalkToPlayerNorth
	waitmovement 0
	msgbox ValleyWindworks_Building_Text_FinallySeeMyDaughterAgain, MSGBOX_DEFAULT
	closemessage
	call ValleyWindworks_Building_EventScript_AddLittleGirl
	applymovement LOCALID_VALLEY_WINDWORKS_BUILDING_LITTLE_GIRL, ValleyWindworks_Building_Movement_LittleGirlRunToPapaNorth
	applymovement LOCALID_VALLEY_WINDWORKS_BUILDING_PAPA, ValleyWindworks_Building_Movement_PapaFaceDaughterBelow
	waitmovement 0
	msgbox ValleyWindworks_Building_Text_YuckPapaYoureStinky, MSGBOX_DEFAULT
	closemessage
	applymovement LOCALID_VALLEY_WINDWORKS_BUILDING_PAPA, ValleyWindworks_Building_Movement_PapaLookAroundFaceDown
	waitmovement 0
	goto ValleyWindworks_Building_EventScript_BalloonPokemonWillVisitAgain
	end

@ Player at (19,5): Papa comes to (20,5), the girl runs to (20,4) above him.
ValleyWindworks_Building_EventScript_ReunionSouth::
	applymovement LOCALID_VALLEY_WINDWORKS_BUILDING_PAPA, ValleyWindworks_Building_Movement_PapaWalkToPlayerSouth
	waitmovement 0
	msgbox ValleyWindworks_Building_Text_FinallySeeMyDaughterAgain, MSGBOX_DEFAULT
	closemessage
	call ValleyWindworks_Building_EventScript_AddLittleGirl
	applymovement LOCALID_VALLEY_WINDWORKS_BUILDING_LITTLE_GIRL, ValleyWindworks_Building_Movement_LittleGirlRunToPapaSouth
	applymovement LOCALID_VALLEY_WINDWORKS_BUILDING_PAPA, ValleyWindworks_Building_Movement_PapaFaceDaughterAbove
	waitmovement 0
	msgbox ValleyWindworks_Building_Text_YuckPapaYoureStinky, MSGBOX_DEFAULT
	closemessage
	applymovement LOCALID_VALLEY_WINDWORKS_BUILDING_PAPA, ValleyWindworks_Building_Movement_PapaLookAroundFaceUp
	waitmovement 0
	goto ValleyWindworks_Building_EventScript_BalloonPokemonWillVisitAgain
	end

ValleyWindworks_Building_EventScript_AddLittleGirl::
	setflag FLAG_HIDE_ROUTE_205_SOUTH_LITTLE_GIRL
	clearflag FLAG_HIDE_VALLEY_WINDWORKS_BUILDING_LITTLE_GIRL
	addobject LOCALID_VALLEY_WINDWORKS_BUILDING_LITTLE_GIRL
	return

ValleyWindworks_Building_EventScript_BalloonPokemonWillVisitAgain::
	msgbox ValleyWindworks_Building_Text_SorryWorkingNonstop, MSGBOX_DEFAULT
	msgbox ValleyWindworks_Building_Text_BalloonPokemonWillVisitAgain, MSGBOX_DEFAULT
	closemessage
	setvar VAR_VALLEY_WINDWORKS_LOOKER_STATE, 1
	setvar VAR_VALLEY_WINDWORKS_TEAM_GALACTIC_STATE, 2
	releaseall
	end

ValleyWindworks_Building_Movement_MarsNoticeAndWalkToPlayer:
	emote_exclamation_mark
	walk_up
	walk_in_place_faster_left
	step_end

ValleyWindworks_Building_Movement_MarsNoticePlayer:
	emote_exclamation_mark
	walk_in_place_faster_left
	step_end

ValleyWindworks_Building_Movement_PapaWalkToPlayerNorth:
	walk_down
	walk_down
	walk_down
	walk_left
	step_end

ValleyWindworks_Building_Movement_PapaWalkToPlayerSouth:
	walk_down
	walk_down
	walk_down
	walk_down
	walk_left
	step_end

ValleyWindworks_Building_Movement_LittleGirlRunToPapaNorth:
	walk_fast_right
	walk_fast_right
	walk_fast_right
	walk_fast_right
	walk_fast_up
	walk_fast_right
	walk_fast_right
	walk_fast_right
	walk_fast_right
	walk_fast_right
	walk_fast_right
	walk_in_place_faster_up
	step_end

ValleyWindworks_Building_Movement_LittleGirlRunToPapaSouth:
	walk_fast_right
	walk_fast_right
	walk_fast_right
	walk_fast_right
	walk_fast_up
	walk_fast_up
	walk_fast_right
	walk_fast_right
	walk_fast_right
	walk_fast_right
	walk_fast_right
	walk_fast_right
	walk_in_place_faster_down
	step_end

ValleyWindworks_Building_Movement_PapaFaceDaughterBelow:
	delay_8
	delay_8
	delay_8
	delay_8
	delay_8
	delay_8
	walk_in_place_down
	step_end

ValleyWindworks_Building_Movement_PapaFaceDaughterAbove:
	delay_8
	delay_8
	delay_8
	delay_8
	delay_8
	delay_8
	walk_in_place_up
	step_end

ValleyWindworks_Building_Movement_PapaLookAroundFaceDown:
	face_right
	delay_8
	delay_8
	face_left
	delay_8
	delay_8
	face_down
	step_end

ValleyWindworks_Building_Movement_PapaLookAroundFaceUp:
	face_right
	delay_8
	delay_8
	face_left
	delay_8
	delay_8
	face_up
	step_end

ValleyWindworks_Building_EventScript_Papa::
	msgbox ValleyWindworks_Building_Text_BalloonPokemonOnDayOfWeek, MSGBOX_NPC
	end

ValleyWindworks_Building_EventScript_LittleGirl::
	msgbox ValleyWindworks_Building_Text_BalloonPokemonWillVisitAgain, MSGBOX_NPC
	end

ValleyWindworks_Building_EventScript_PC::
	msgbox ValleyWindworks_Building_Text_BringersAndTargetsTeamGalactic, MSGBOX_SIGN
	end

ValleyWindworks_Building_Text_GotToAlertCommander:
	.string "What?! No way!\n"
	.string "You have the key?\p"
	.string "Locking the door was meaningless!\n"
	.string "I am the loser in this after all!\p"
	.string "This is no time for self-pity!\n"
	.string "I've got to alert the COMMANDER!$"

ValleyWindworks_Building_Text_CommanderWillSmooshYou:
	.string "My POKéMON can't battle anymore.\p"
	.string "They're gonna take away my POKéMON\n"
	.string "for messing this one up…\p"
	.string "Humph! Just you wait!\n"
	.string "Our COMMANDER will smoosh you!$"

ValleyWindworks_Building_Text_Grunt2Intro:
	.string "Oi! This is no fun and games!\n"
	.string "This is proper work, I tell you!$"

ValleyWindworks_Building_Text_Grunt2Defeat:
	.string "What…?!\n"
	.string "Some brat messed up my job…$"

ValleyWindworks_Building_Text_Grunt2PostBattle:
	.string "TEAM GALACTIC needs electricity for\n"
	.string "doing something big!$"

ValleyWindworks_Building_Text_Grunt3Intro:
	.string "Collecting electricity from the VALLEY\n"
	.string "WINDWORKS… That's our mission.$"

ValleyWindworks_Building_Text_Grunt3Defeat:
	.string "I really don't like kids who also\n"
	.string "happen to be tough TRAINERS…$"

ValleyWindworks_Building_Text_Grunt3PostBattle:
	.string "You don't have an inkling of what\n"
	.string "TEAM GALACTIC is trying to achieve!\p"
	.string "Of course, they don't tell a grunt\n"
	.string "like me anything, either… But still!$"

ValleyWindworks_Building_Text_LetsHaveABattle:
	.string "I'm one of TEAM GALACTIC's three\n"
	.string "COMMANDERS.\p"
	.string "…No, wait.\n"
	.string "That's one of four COMMANDERS.\p"
	.string "My name is MARS!\p"
	.string "We've been trying to create a new\n"
	.string "world that's better than this one…\p"
	.string "But people have shown little\n"
	.string "understanding about what we do.\p"
	.string "You don't understand either, do you?\n"
	.string "It's a little saddening…\p"
	.string "So, let's have a battle to decide\n"
	.string "what we should do next.\p"
	.string "If I win, you leave.\p"
	.string "If you win, we, TEAM GALACTIC, will\n"
	.string "leave!$"

ValleyWindworks_Building_Text_MarsDefeat:
	.string "This can't be?! I lost?!\n"
	.string "You… you uppity brat!$"

ValleyWindworks_Building_Text_IMessedThatOneUp:
	.string "MARS: Oops!\n"
	.string "I messed that one up!\p"
	.string "That's all right, though.\n"
	.string "I quite enjoyed our battle.$"

ValleyWindworks_Building_Text_LoseToAChild:
	.string "CHARON: My, my.\n"
	.string "Lose to a child, will you?\p"
	.string "But, no matter.\n"
	.string "We've collected plenty of electricity.\p"
	.string "With the power we obtained, we can\n"
	.string "do something quite spectacular.\p"
	.string "It seems quite obvious to me, CHARON,\n"
	.string "the genius even the boss recognizes.\p"
	.string "Now, MARS, we should be going.$"

ValleyWindworks_Building_Text_WillYouShutIt:
	.string "MARS: Will you shut it!\p"
	.string "The boss is the only person in the\n"
	.string "world who's allowed to order me around!\p"
	.string "You can keep quiet around me!\n"
	.string "You only joined us recently.\p"
	.string "Don't think you're important!$"

ValleyWindworks_Building_Text_LeaveForTheTimeBeing:
	.string "I guess it's time to say good-bye\n"
	.string "and leave for the time being!$"

ValleyWindworks_Building_Text_FinallySeeMyDaughterAgain:
	.string "TEAM GALACTIC…\p"
	.string "They were saying they had to gather\n"
	.string "POKéMON and energy no matter what.\p"
	.string "It was for creating a new universe,\n"
	.string "or so they claimed.\p"
	.string "Nothing they said made any sort of\n"
	.string "sense to me.\p"
	.string "I can't tell you how grateful I am for\n"
	.string "you saving me.\p"
	.string "I can finally see my little daughter\n"
	.string "again!$"

ValleyWindworks_Building_Text_YuckPapaYoureStinky:
	.string "LITTLE GIRL: Papa!\p"
	.string "Yuck! Papa, you're stinky!\n"
	.string "Go shower now, stinky!$"

ValleyWindworks_Building_Text_SorryWorkingNonstop:
	.string "PAPA: Oh, sorry! Ahaha!\n"
	.string "They had me working nonstop!$"

ValleyWindworks_Building_Text_BalloonPokemonWillVisitAgain:
	.string "LITTLE GIRL: Thank you, TRAINER!\n"
	.string "You made those bad people go away!\p"
	.string "I think the balloon POKéMON will\n"
	.string "come visiting again!$"

ValleyWindworks_Building_Text_BalloonPokemonOnDayOfWeek:
	.string "PAPA: Oh, yes, a balloon POKéMON\n"
	.string "appears in front of the windworks.\p"
	.string "It's always a certain day of the week,\n"
	.string "but I'm not sure which day.\p"
	.string "Could its appearance have something\n"
	.string "to do with this windworks using\l"
	.string "wind-generated energy?$"

ValleyWindworks_Building_Text_BringersAndTargetsTeamGalactic:
	.string "The bringer of wars…\p"
	.string "Your target is the energy of the power\n"
	.string "plant.\p"
	.string "The bringer of contentment…\p"
	.string "Your target is the POKéMON statue of\n"
	.string "ETERNA CITY.\p"
	.string "The bringer of aging…\p"
	.string "Your target is the POKéMON that sleeps\n"
	.string "on today.\p"
	.string "The dream energy.\n"
	.string "           TEAM GALACTIC$"
