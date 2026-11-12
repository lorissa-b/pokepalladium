@ Cycling Road's south gate, between Route 206's bridge and its ground. North
@ of the counters (row 6) the player must be on the BICYCLE, which sets
@ FLAG_SYS_CYCLING_ROAD (no getting off) until they come back past row 8.

Route206_CyclingRoadSouthGate_MapScripts::
	.byte 0

Route206_CyclingRoadSouthGate_EventScript_Clerk::
	msgbox Route206_CyclingRoadSouthGate_Text_LearnHowToShiftGears, MSGBOX_NPC
	end

Route206_CyclingRoadSouthGate_EventScript_BikeCheck::
	lockall
	specialvar VAR_RESULT, GetPlayerAvatarBike
	goto_if_eq VAR_RESULT, 0, Route206_CyclingRoadSouthGate_EventScript_NoBike
	setflag FLAG_SYS_CYCLING_ROAD
	setvar VAR_TEMP_1, 1
	releaseall
	end

Route206_CyclingRoadSouthGate_EventScript_NoBike::
	applymovement LOCALID_ROUTE206_SOUTH_GATE_CLERK_WEST, Common_Movement_ExclamationMark
	waitmovement 0
	msgbox Route206_CyclingRoadSouthGate_Text_OpenOnlyToCyclists, MSGBOX_DEFAULT
	closemessage
	applymovement LOCALID_PLAYER, Route206_CyclingRoadSouthGate_Movement_PlayerWalkSouth
	waitmovement 0
	releaseall
	end

Route206_CyclingRoadSouthGate_Movement_PlayerWalkSouth:
	walk_down
	step_end

Route206_CyclingRoadSouthGate_EventScript_ClearCyclingRoad::
	lockall
	clearflag FLAG_SYS_CYCLING_ROAD
	setvar VAR_TEMP_1, 0
	releaseall
	end

Route206_CyclingRoadSouthGate_EventScript_BattleGirl::
	msgbox Route206_CyclingRoadSouthGate_Text_GoThroughMtCoronet, MSGBOX_NPC
	end

Route206_CyclingRoadSouthGate_Text_OpenOnlyToCyclists:
	.string "CYCLING ROAD is open only to cyclists.\n"
	.string "You must be on a BICYCLE.$"

Route206_CyclingRoadSouthGate_Text_LearnHowToShiftGears:
	.string "Learn how to shift gears, and you'll\n"
	.string "be able to ride anywhere.$"

Route206_CyclingRoadSouthGate_Text_GoThroughMtCoronet:
	.string "Go through MT. CORONET from here,\n"
	.string "and you can get to HEARTHOME CITY.\p"
	.string "HEARTHOME is where they hold POKéMON\n"
	.string "CONTESTS.$"
