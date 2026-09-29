@ The rival is Barry whichever gender the player is
Common_EventScript_SetupRivalGfxId::
	setvar VAR_OBJ_GFX_ID_0, OBJ_EVENT_GFX_BARRY
	return

@ Prof. Birch's assistant is the player character of the other gender
Common_EventScript_SetupAssistantGfxId::
	checkplayergender
	goto_if_eq VAR_RESULT, MALE, EventScript_SetupAssistantGfxIdFemale
	goto_if_eq VAR_RESULT, FEMALE, EventScript_SetupAssistantGfxIdMale
	end

EventScript_SetupAssistantGfxIdFemale::
	setvar VAR_OBJ_GFX_ID_1, OBJ_EVENT_GFX_RIVAL_MAY_NORMAL
	return

EventScript_SetupAssistantGfxIdMale::
	setvar VAR_OBJ_GFX_ID_1, OBJ_EVENT_GFX_RIVAL_BRENDAN_NORMAL
	return

Common_EventScript_SetupRivalOnBikeGfxId::
	checkplayergender
	goto_if_eq VAR_RESULT, MALE, EventScript_SetupRivalOnBikeGfxIdFemale
	goto_if_eq VAR_RESULT, FEMALE, EventScript_SetupRivalOnBikeGfxIdMale
	end

EventScript_SetupRivalOnBikeGfxIdFemale::
	setvar VAR_OBJ_GFX_ID_3, OBJ_EVENT_GFX_RIVAL_MAY_MACH_BIKE
	return

EventScript_SetupRivalOnBikeGfxIdMale::
	setvar VAR_OBJ_GFX_ID_3, OBJ_EVENT_GFX_RIVAL_BRENDAN_MACH_BIKE
	return

@ Unused
Common_EventScript_SetupRivalGfxIdSameGender::
	checkplayergender
	goto_if_eq VAR_RESULT, MALE, EventScript_SetupRivalGfxIdMale2
	goto_if_eq VAR_RESULT, FEMALE, EventScript_SetupRivalGfxIdFemale2
	end

EventScript_SetupRivalGfxIdMale2::
	setvar VAR_OBJ_GFX_ID_0, OBJ_EVENT_GFX_RIVAL_BRENDAN_NORMAL
	return

EventScript_SetupRivalGfxIdFemale2::
	setvar VAR_OBJ_GFX_ID_0, OBJ_EVENT_GFX_RIVAL_MAY_NORMAL
	return
