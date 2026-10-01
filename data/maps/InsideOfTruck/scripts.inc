InsideOfTruck_MapScripts::
	map_script MAP_SCRIPT_ON_LOAD, InsideOfTruck_OnLoad
	map_script MAP_SCRIPT_ON_RESUME, InsideOfTruck_OnResume
	.byte 0

InsideOfTruck_OnLoad:
	setmetatile 4, 1, METATILE_InsideOfTruck_ExitLight_Top, FALSE
	setmetatile 4, 2, METATILE_InsideOfTruck_ExitLight_Mid, FALSE
	setmetatile 4, 3, METATILE_InsideOfTruck_ExitLight_Bottom, FALSE
	end

InsideOfTruck_OnResume:
	setstepcallback STEP_CB_TRUCK
	end

InsideOfTruck_EventScript_SetIntroFlags::
	lockall
	setflag FLAG_HIDE_MAP_NAME_POPUP
	checkplayergender
	goto_if_eq VAR_RESULT, MALE, InsideOfTruck_EventScript_SetIntroFlagsMale
	goto_if_eq VAR_RESULT, FEMALE, InsideOfTruck_EventScript_SetIntroFlagsFemale
	end

InsideOfTruck_EventScript_SetIntroFlagsMale::
	setrespawn HEAL_LOCATION_TWINLEAF_TOWN_LUCAS_HOUSE_2F
	setvar VAR_TWINLEAF_INTRO_STATE, 1
	setflag FLAG_HIDE_TWINLEAF_TOWN_DAWN_HOUSE_MOM
	setflag FLAG_HIDE_TWINLEAF_TOWN_LUCAS_HOUSE_RIVAL_MOM
	setflag FLAG_HIDE_TWINLEAF_TOWN_LUCAS_HOUSE_RIVAL_SIBLING
	setflag FLAG_HIDE_TWINLEAF_TOWN_LUCAS_HOUSE_2F_POKE_BALL
	setvar VAR_TWINLEAF_HOUSES_STATE_BRENDAN, 1
	setdynamicwarp MAP_TWINLEAF_TOWN, 3, 10
	releaseall
	end

InsideOfTruck_EventScript_SetIntroFlagsFemale::
	setrespawn HEAL_LOCATION_TWINLEAF_TOWN_DAWN_HOUSE_2F
	setvar VAR_TWINLEAF_INTRO_STATE, 2
	setflag FLAG_HIDE_TWINLEAF_TOWN_LUCAS_HOUSE_MOM
	setflag FLAG_HIDE_TWINLEAF_TOWN_DAWN_HOUSE_RIVAL_MOM
	setflag FLAG_HIDE_TWINLEAF_TOWN_DAWN_HOUSE_RIVAL_SIBLING
	setflag FLAG_HIDE_TWINLEAF_TOWN_DAWN_HOUSE_2F_POKE_BALL
	setvar VAR_TWINLEAF_HOUSES_STATE_MAY, 1
	setdynamicwarp MAP_TWINLEAF_TOWN, 12, 10
	releaseall
	end

InsideOfTruck_EventScript_MovingBox::
	msgbox InsideOfTruck_Text_BoxPrintedWithMonLogo, MSGBOX_SIGN
	end

InsideOfTruck_Text_BoxPrintedWithMonLogo:
	.string "The box is printed with a POKéMON logo.\p"
	.string "It's a POKéMON brand moving and\n"
	.string "delivery service.$"

