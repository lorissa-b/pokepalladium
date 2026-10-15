@ Valley Windworks, east of Route 205 South. Drawn over Emerald's Route 114
@ from Platinum's map. A Team Galactic Grunt guards the door; beaten, he locks
@ himself in. The door stays locked (the doorless METATILE_Rustboro_Door_Tan_Locked
@ in map.bin) until the player uses the WORKS KEY from Floaroma Meadow, which
@ sets FLAG_UNLOCKED_VALLEY_WINDWORKS_DOOR. Once VAR_VALLEY_WINDWORKS_LOOKER_STATE
@ is 1 (set in ValleyWindworks_Building after Mars is beaten), Looker meets the
@ player at the door on the way out, looks inside and leaves for Eterna City.
ValleyWindworks_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, ValleyWindworks_OnTransition
	map_script MAP_SCRIPT_ON_LOAD, ValleyWindworks_OnLoad
	map_script MAP_SCRIPT_ON_FRAME_TABLE, ValleyWindworks_OnFrame
	.byte 0

ValleyWindworks_OnTransition:
	call_if_eq VAR_VALLEY_WINDWORKS_LOOKER_STATE, 1, ValleyWindworks_EventScript_ShowLooker
	call_if_ne VAR_VALLEY_WINDWORKS_LOOKER_STATE, 1, ValleyWindworks_EventScript_HideLooker
	end

ValleyWindworks_EventScript_ShowLooker::
	clearflag FLAG_HIDE_VALLEY_WINDWORKS_LOOKER
	return

ValleyWindworks_EventScript_HideLooker::
	setflag FLAG_HIDE_VALLEY_WINDWORKS_LOOKER
	return

ValleyWindworks_OnLoad:
	call_if_set FLAG_UNLOCKED_VALLEY_WINDWORKS_DOOR, ValleyWindworks_EventScript_SetDoorOpen
	end

ValleyWindworks_EventScript_SetDoorOpen::
	setmetatile 19, 14, METATILE_Rustboro_Door_Tan, TRUE
	return

ValleyWindworks_OnFrame:
	map_script_2 VAR_VALLEY_WINDWORKS_LOOKER_STATE, 1, ValleyWindworks_EventScript_Looker
	.2byte 0

ValleyWindworks_EventScript_Grunt::
	lock
	faceplayer
	msgbox ValleyWindworks_Text_BattleMeForIt, MSGBOX_DEFAULT
	closemessage
	trainerbattle_no_intro TRAINER_LENNY, ValleyWindworks_Text_GruntDefeat
	msgbox ValleyWindworks_Text_GettingOwnedBySomeKid, MSGBOX_DEFAULT
	closemessage
	applywaitmovement LOCALID_VALLEY_WINDWORKS_GRUNT, Common_Movement_WalkInPlaceFasterUp
	@ Open the door just long enough for him to get in
	call ValleyWindworks_EventScript_SetDoorOpen
	special DrawWholeMapView
	opendoor 19, 14
	waitdooranim
	applywaitmovement LOCALID_VALLEY_WINDWORKS_GRUNT, ValleyWindworks_Movement_EnterBuilding
	closedoor 19, 14
	waitdooranim
	setmetatile 19, 14, METATILE_Rustboro_Door_Tan_Locked, TRUE
	special DrawWholeMapView
	removeobject LOCALID_VALLEY_WINDWORKS_GRUNT
	setflag FLAG_HIDE_VALLEY_WINDWORKS_GRUNT
	msgbox ValleyWindworks_Text_Kerchunk, MSGBOX_DEFAULT
	release
	end

ValleyWindworks_Movement_EnterBuilding:
	walk_up
	set_invisible
	step_end

ValleyWindworks_EventScript_Door::
	lockall
	goto_if_set FLAG_UNLOCKED_VALLEY_WINDWORKS_DOOR, ValleyWindworks_EventScript_DoorEnd
	checkitem ITEM_WORKS_KEY
	goto_if_eq VAR_RESULT, TRUE, ValleyWindworks_EventScript_AskUseWorksKey
	msgbox ValleyWindworks_Text_ItsLockedFromInside, MSGBOX_DEFAULT
	releaseall
	end

ValleyWindworks_EventScript_AskUseWorksKey::
	msgbox ValleyWindworks_Text_UseTheWorksKey, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, ValleyWindworks_EventScript_DoorEnd
	setflag FLAG_UNLOCKED_VALLEY_WINDWORKS_DOOR
	call ValleyWindworks_EventScript_SetDoorOpen
	special DrawWholeMapView
	msgbox ValleyWindworks_Text_KerchunkTheDoorOpened, MSGBOX_DEFAULT
	releaseall
	end

ValleyWindworks_EventScript_DoorEnd::
	releaseall
	end

ValleyWindworks_EventScript_Sign::
	msgbox ValleyWindworks_Text_SignValleyWindworks, MSGBOX_SIGN
	end

@ The player has just come out of the door to (19,15). Looker runs up from
@ (18,19), checks the building, then leaves down the path to Route 205.
ValleyWindworks_EventScript_Looker::
	lockall
	applymovement LOCALID_VALLEY_WINDWORKS_LOOKER, ValleyWindworks_Movement_LookerNoticeAndWalkToPlayer
	applymovement LOCALID_PLAYER, ValleyWindworks_Movement_PlayerFaceLooker
	waitmovement 0
	msgbox ValleyWindworks_Text_IShallInvestigateInside, MSGBOX_DEFAULT
	closemessage
	applymovement LOCALID_PLAYER, ValleyWindworks_Movement_PlayerMoveAwayFromDoor
	applymovement LOCALID_VALLEY_WINDWORKS_LOOKER, ValleyWindworks_Movement_LookerWalkToDoor
	waitmovement 0
	opendoor 19, 14
	waitdooranim
	applywaitmovement LOCALID_VALLEY_WINDWORKS_LOOKER, ValleyWindworks_Movement_LookerEnterBuilding
	closedoor 19, 14
	waitdooranim
	delay 120
	opendoor 19, 14
	waitdooranim
	applywaitmovement LOCALID_VALLEY_WINDWORKS_LOOKER, ValleyWindworks_Movement_LookerExitBuilding
	closedoor 19, 14
	waitdooranim
	msgbox ValleyWindworks_Text_HideoutIsInEternaCity, MSGBOX_DEFAULT
	closemessage
	applymovement LOCALID_PLAYER, ValleyWindworks_Movement_PlayerWatchLookerLeave
	applymovement LOCALID_VALLEY_WINDWORKS_LOOKER, ValleyWindworks_Movement_LookerLeave
	waitmovement 0
	removeobject LOCALID_VALLEY_WINDWORKS_LOOKER
	setflag FLAG_HIDE_VALLEY_WINDWORKS_LOOKER
	setvar VAR_VALLEY_WINDWORKS_LOOKER_STATE, 2
	releaseall
	end

ValleyWindworks_Movement_PlayerFaceLooker:
	delay_8
	delay_8
	delay_8
	delay_8
	delay_8
	delay_4
	walk_in_place_faster_left
	step_end

ValleyWindworks_Movement_PlayerMoveAwayFromDoor:
	walk_down
	walk_in_place_faster_up
	step_end

ValleyWindworks_Movement_PlayerWatchLookerLeave:
	delay_8
	walk_in_place_faster_left
	walk_in_place_faster_down
	step_end

ValleyWindworks_Movement_LookerNoticeAndWalkToPlayer:
	emote_exclamation_mark
	delay_8
	walk_fast_up
	walk_fast_up
	walk_fast_up
	walk_fast_up
	walk_in_place_faster_right
	step_end

ValleyWindworks_Movement_LookerWalkToDoor:
	delay_8
	delay_4
	walk_right
	walk_in_place_faster_up
	step_end

ValleyWindworks_Movement_LookerEnterBuilding:
	walk_up
	set_invisible
	face_down
	step_end

ValleyWindworks_Movement_LookerExitBuilding:
	set_visible
	walk_fast_down
	walk_in_place_faster_down
	step_end

ValleyWindworks_Movement_LookerLeave:
	walk_fast_left
	walk_fast_down
	walk_fast_down
	walk_fast_down
	walk_fast_down
	walk_fast_down
	walk_fast_down
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
	step_end

ValleyWindworks_Text_BattleMeForIt:
	.string "Don't you dare go into the VALLEY\n"
	.string "WINDWORKS.\p"
	.string "I got ordered to keep everyone out\n"
	.string "if they're not part of TEAM GALACTIC.\p"
	.string "You're giving me a look that makes\n"
	.string "me think you're gonna try to get in.\p"
	.string "OK, then.\n"
	.string "You'll have to battle me for it!$"

ValleyWindworks_Text_GruntDefeat:
	.string "Yeah, like I'm really supposed to\n"
	.string "win with a POKéMON like this…$"

ValleyWindworks_Text_GettingOwnedBySomeKid:
	.string "Isn't that just great.\n"
	.string "Getting owned by some kid.\p"
	.string "But you can't do a thing if I lock\n"
	.string "myself in with my WORKS KEY!\p"
	.string "That's right, the other WORKS KEY's\n"
	.string "not here!\p"
	.string "My cohorts in the flower patch have\n"
	.string "the only other one!\p"
	.string "Hahaha! In that sense, I'm no longer\n"
	.string "the loser!$"

ValleyWindworks_Text_Kerchunk:
	.string "Ker-chunk!$"

ValleyWindworks_Text_ItsLockedFromInside:
	.string "It's locked from inside!$"

ValleyWindworks_Text_UseTheWorksKey:
	.string "It's locked from inside!\p"
	.string "Use the WORKS KEY?$"

ValleyWindworks_Text_KerchunkTheDoorOpened:
	.string "Ker-chunk!\p"
	.string "The door to the VALLEY WINDWORKS\n"
	.string "opened!$"

ValleyWindworks_Text_SignValleyWindworks:
	.string "VALLEY WINDWORKS\n"
	.string "Ecological, Wind-Driven Energy$"

ValleyWindworks_Text_IShallInvestigateInside:
	.string "LOOKER: Ah, yes.\n"
	.string "It is you, yes.\p"
	.string "I have heard that the TEAM GALACTIC\n"
	.string "appeared at this power plant.\p"
	.string "I have, therefore, come running.\n"
	.string "So, the TEAM GALACTIC. Where are they?\p"
	.string "…\p"
	.string "You have run them off, you say?\n"
	.string "You are a TRAINER, but…?\p"
	.string "Very well!\n"
	.string "I shall go investigate inside!$"

ValleyWindworks_Text_HideoutIsInEternaCity:
	.string "LOOKER: Magnificent! Yes, it is!\n"
	.string "What you have said, it is all true!\p"
	.string "Superb! Though you are young, you are\n"
	.string "a TRAINER through and through!\p"
	.string "Very well!\n"
	.string "I shall pursue the culprits!\p"
	.string "I have received tips that the TEAM\n"
	.string "GALACTIC hideout is in ETERNA CITY.\p"
	.string "To there, I must be off!$"
