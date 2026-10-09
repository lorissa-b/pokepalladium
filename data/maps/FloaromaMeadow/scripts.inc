@ Floaroma Meadow, reached by warp from Floaroma Town, laid out as in
@ Platinum. Its flowers have wild encounters. Platinum's Team Galactic
@ Grunts, the honey man, items and hidden items aren't here yet. The
@ scripts below are Route 117's, kept for the Day Care scripts that use
@ their text.
FloaromaMeadow_MapScripts::
	.byte 0

FloaromaMeadow_EventScript_Woman::
	msgbox FloaromaMeadow_Text_ArentTheseFlowersPretty, MSGBOX_NPC
	end

FloaromaMeadow_EventScript_LittleBoy::
	msgbox FloaromaMeadow_Text_AirIsTastyHere, MSGBOX_NPC
	end

FloaromaMeadow_EventScript_Girl::
	msgbox FloaromaMeadow_Text_DayCarePokemonHadNewMove, MSGBOX_NPC
	end

FloaromaMeadow_EventScript_RouteSignVerdanturf::
	msgbox FloaromaMeadow_Text_RouteSignVerdanturf, MSGBOX_SIGN
	end

FloaromaMeadow_EventScript_RouteSignMauville::
	msgbox FloaromaMeadow_Text_RouteSignMauville, MSGBOX_SIGN
	end

FloaromaMeadow_EventScript_DayCareSign::
	msgbox FloaromaMeadow_Text_DayCareSign, MSGBOX_SIGN
	end

FloaromaMeadow_EventScript_Isaac::
	trainerbattle_single TRAINER_ISAAC_1, FloaromaMeadow_Text_IsaacIntro, FloaromaMeadow_Text_IsaacDefeat, FloaromaMeadow_EventScript_RegisterIsaac
	specialvar VAR_RESULT, ShouldTryRematchBattle
	goto_if_eq VAR_RESULT, TRUE, FloaromaMeadow_EventScript_RematchIsaac
	msgbox FloaromaMeadow_Text_IsaacPostBattle, MSGBOX_DEFAULT
	release
	end

FloaromaMeadow_EventScript_RegisterIsaac::
	special PlayerFaceTrainerAfterBattle
	waitmovement 0
	msgbox FloaromaMeadow_Text_IsaacRegister, MSGBOX_DEFAULT
	register_matchcall TRAINER_ISAAC_1
	release
	end

FloaromaMeadow_EventScript_RematchIsaac::
	trainerbattle_rematch TRAINER_ISAAC_1, FloaromaMeadow_Text_IsaacRematchIntro, FloaromaMeadow_Text_IsaacRematchDefeat
	msgbox FloaromaMeadow_Text_IsaacPostRematch, MSGBOX_AUTOCLOSE
	end

FloaromaMeadow_EventScript_Lydia::
	trainerbattle_single TRAINER_LYDIA_1, FloaromaMeadow_Text_LydiaIntro, FloaromaMeadow_Text_LydiaDefeat, FloaromaMeadow_EventScript_RegisterLydia
	specialvar VAR_RESULT, ShouldTryRematchBattle
	goto_if_eq VAR_RESULT, TRUE, FloaromaMeadow_EventScript_RematchLydia
	msgbox FloaromaMeadow_Text_LydiaPostBattle, MSGBOX_DEFAULT
	release
	end

FloaromaMeadow_EventScript_RegisterLydia::
	special PlayerFaceTrainerAfterBattle
	waitmovement 0
	msgbox FloaromaMeadow_Text_LydiaRegister, MSGBOX_DEFAULT
	register_matchcall TRAINER_LYDIA_1
	release
	end

FloaromaMeadow_EventScript_RematchLydia::
	trainerbattle_rematch TRAINER_LYDIA_1, FloaromaMeadow_Text_LydiaRematchIntro, FloaromaMeadow_Text_LydiaRematchDefeat
	msgbox FloaromaMeadow_Text_LydiaPostRematch, MSGBOX_AUTOCLOSE
	end

FloaromaMeadow_EventScript_Dylan::
	trainerbattle_single TRAINER_DYLAN_1, FloaromaMeadow_Text_DylanIntro, FloaromaMeadow_Text_DylanDefeat, FloaromaMeadow_EventScript_RegisterDylan
	specialvar VAR_RESULT, ShouldTryRematchBattle
	goto_if_eq VAR_RESULT, TRUE, FloaromaMeadow_EventScript_RematchDylan
	msgbox FloaromaMeadow_Text_DylanPostBattle, MSGBOX_DEFAULT
	release
	end

FloaromaMeadow_EventScript_RegisterDylan::
	special PlayerFaceTrainerAfterBattle
	waitmovement 0
	msgbox FloaromaMeadow_Text_DylanRegister, MSGBOX_DEFAULT
	register_matchcall TRAINER_DYLAN_1
	release
	end

FloaromaMeadow_EventScript_RematchDylan::
	trainerbattle_rematch TRAINER_DYLAN_1, FloaromaMeadow_Text_DylanRematchIntro, FloaromaMeadow_Text_DylanRematchDefeat
	msgbox FloaromaMeadow_Text_DylanPostRematch, MSGBOX_AUTOCLOSE
	end

FloaromaMeadow_EventScript_Maria::
	trainerbattle_single TRAINER_MARIA_1, FloaromaMeadow_Text_MariaIntro, FloaromaMeadow_Text_MariaDefeat, FloaromaMeadow_EventScript_RegisterMaria
	specialvar VAR_RESULT, ShouldTryRematchBattle
	goto_if_eq VAR_RESULT, TRUE, FloaromaMeadow_EventScript_RematchMaria
	msgbox FloaromaMeadow_Text_MariaPostBattle, MSGBOX_DEFAULT
	release
	end

FloaromaMeadow_EventScript_RegisterMaria::
	special PlayerFaceTrainerAfterBattle
	waitmovement 0
	msgbox FloaromaMeadow_Text_MariaRegister, MSGBOX_DEFAULT
	register_matchcall TRAINER_MARIA_1
	release
	end

FloaromaMeadow_EventScript_RematchMaria::
	trainerbattle_rematch TRAINER_MARIA_1, FloaromaMeadow_Text_MariaRematchIntro, FloaromaMeadow_Text_MariaRematchDefeat
	msgbox FloaromaMeadow_Text_MariaPostRematch, MSGBOX_AUTOCLOSE
	end

FloaromaMeadow_EventScript_Derek::
	trainerbattle_single TRAINER_DEREK, FloaromaMeadow_Text_DerekIntro, FloaromaMeadow_Text_DerekDefeat
	msgbox FloaromaMeadow_Text_DerekPostBattle, MSGBOX_AUTOCLOSE
	end

FloaromaMeadow_EventScript_Anna::
	trainerbattle_double TRAINER_ANNA_AND_MEG_1, FloaromaMeadow_Text_AnnaIntro, FloaromaMeadow_Text_AnnaDefeat, FloaromaMeadow_Text_AnnaNotEnoughMons, FloaromaMeadow_EventScript_RegisterAnna
	specialvar VAR_RESULT, ShouldTryRematchBattle
	goto_if_eq VAR_RESULT, TRUE, FloaromaMeadow_EventScript_RematchAnna
	msgbox FloaromaMeadow_Text_AnnaPostBattle, MSGBOX_DEFAULT
	release
	end

FloaromaMeadow_EventScript_RegisterAnna::
	msgbox FloaromaMeadow_Text_AnnaAndMegRegister, MSGBOX_DEFAULT
	register_matchcall TRAINER_ANNA_AND_MEG_1
	release
	end

FloaromaMeadow_EventScript_RematchAnna::
	trainerbattle_rematch_double TRAINER_ANNA_AND_MEG_1, FloaromaMeadow_Text_AnnaRematchIntro, FloaromaMeadow_Text_AnnaRematchDefeat, FloaromaMeadow_Text_AnnaRematchNotEnoughMons
	msgbox FloaromaMeadow_Text_AnnaPostRematch, MSGBOX_AUTOCLOSE
	end

FloaromaMeadow_EventScript_Meg::
	trainerbattle_double TRAINER_ANNA_AND_MEG_1, FloaromaMeadow_Text_MegIntro, FloaromaMeadow_Text_MegDefeat, FloaromaMeadow_Text_MegNotEnoughMons, FloaromaMeadow_EventScript_RegisterMeg
	specialvar VAR_RESULT, ShouldTryRematchBattle
	goto_if_eq VAR_RESULT, TRUE, FloaromaMeadow_EventScript_RematchMeg
	msgbox FloaromaMeadow_Text_MegPostBattle, MSGBOX_DEFAULT
	release
	end

FloaromaMeadow_EventScript_RegisterMeg::
	msgbox FloaromaMeadow_Text_AnnaAndMegRegister, MSGBOX_DEFAULT
	register_matchcall TRAINER_ANNA_AND_MEG_1
	release
	end

FloaromaMeadow_EventScript_RematchMeg::
	trainerbattle_rematch_double TRAINER_ANNA_AND_MEG_1, FloaromaMeadow_Text_MegRematchIntro, FloaromaMeadow_Text_MegRematchDefeat, FloaromaMeadow_Text_MegRematchNotEnoughMons
	msgbox FloaromaMeadow_Text_MegPostRematch, MSGBOX_AUTOCLOSE
	end

FloaromaMeadow_EventScript_Melina::
	trainerbattle_single TRAINER_MELINA, FloaromaMeadow_Text_MelinaIntro, FloaromaMeadow_Text_MelinaDefeat
	msgbox FloaromaMeadow_Text_MelinaPostBattle, MSGBOX_AUTOCLOSE
	end

FloaromaMeadow_EventScript_Brandi::
	trainerbattle_single TRAINER_BRANDI, FloaromaMeadow_Text_BrandiIntro, FloaromaMeadow_Text_BrandiDefeat
	msgbox FloaromaMeadow_Text_BrandiPostBattle, MSGBOX_AUTOCLOSE
	end

FloaromaMeadow_EventScript_Aisha::
	trainerbattle_single TRAINER_AISHA, FloaromaMeadow_Text_AishaIntro, FloaromaMeadow_Text_AishaDefeat
	msgbox FloaromaMeadow_Text_AishaPostBattle, MSGBOX_AUTOCLOSE
	end

FloaromaMeadow_Text_DayCarePokemonHadNewMove:
	.string "I left my POKéMON at the DAY CARE.\p"
	.string "When I got it back, it had a new move\n"
	.string "that I didn't teach it.\l"
	.string "I was really, really surprised.$"

FloaromaMeadow_Text_ArentTheseFlowersPretty:
	.string "What do you think?\n"
	.string "Aren't these flowers pretty?\p"
	.string "I planted them all!$"

FloaromaMeadow_Text_AirIsTastyHere:
	.string "The air is tasty here!$"

FloaromaMeadow_Text_RouteSignVerdanturf:
	.string "ROUTE 117\n"
	.string "{LEFT_ARROW} VERDANTURF TOWN$"

FloaromaMeadow_Text_RouteSignMauville:
	.string "ROUTE 117\n"
	.string "{RIGHT_ARROW} MAUVILLE CITY$"

FloaromaMeadow_Text_DayCareSign:
	.string "POKéMON DAY CARE\n"
	.string "“Let us raise your POKéMON.”$"

