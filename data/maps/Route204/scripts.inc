@ Route 204, between Jubilife City and Floaroma Town. Drawn over Emerald's
@ Route 116. The southern part (from Jubilife) and the northern part are only
@ joined through Ravaged Path, whose two cave mouths are drawn but not yet
@ linked to a cave map.
Route204_MapScripts::
	.byte 0

Route204_EventScript_RouteSignNorth::
	msgbox Route204_Text_RouteSign, MSGBOX_SIGN
	end

Route204_EventScript_RouteSignSouth::
	msgbox Route204_Text_RouteSign, MSGBOX_SIGN
	end

Route204_EventScript_RavagedPathSign::
	msgbox Route204_Text_RavagedPathSign, MSGBOX_SIGN
	end

@ At the end of the narrow path past the Cut trees. In Diamond/Pearl she gives
@ TM78 (Captivate), which this game doesn't have, so she gives Attract instead.
Route204_EventScript_AttractWoman::
	lock
	faceplayer
	goto_if_set FLAG_RECEIVED_ROUTE_204_TM_ATTRACT, Route204_EventScript_ReceivedAttract
	msgbox Route204_Text_YouFoundMyHidingPlace, MSGBOX_DEFAULT
	giveitem ITEM_TM_ATTRACT
	goto_if_eq VAR_RESULT, 0, Common_EventScript_ShowBagIsFull
	setflag FLAG_RECEIVED_ROUTE_204_TM_ATTRACT
	msgbox Route204_Text_AttractExplanation, MSGBOX_DEFAULT
	release
	end

Route204_EventScript_ReceivedAttract::
	msgbox Route204_Text_AttractExplanation, MSGBOX_DEFAULT
	release
	end

Route204_EventScript_Sarah::
	trainerbattle_single TRAINER_SARAH, Route204_Text_SarahIntro, Route204_Text_SarahDefeat
	msgbox Route204_Text_SarahPostBattle, MSGBOX_AUTOCLOSE
	end

Route204_EventScript_Tyler::
	trainerbattle_single TRAINER_JOEY, Route204_Text_TylerIntro, Route204_Text_TylerDefeat
	msgbox Route204_Text_TylerPostBattle, MSGBOX_AUTOCLOSE
	end

Route204_EventScript_Taylor::
	trainerbattle_single TRAINER_JANICE, Route204_Text_TaylorIntro, Route204_Text_TaylorDefeat
	msgbox Route204_Text_TaylorPostBattle, MSGBOX_AUTOCLOSE
	end

Route204_EventScript_Samantha::
	trainerbattle_single TRAINER_KAREN_1, Route204_Text_SamanthaIntro, Route204_Text_SamanthaDefeat, Route204_EventScript_SamanthaRegisterMatchCallAfterBattle
	specialvar VAR_RESULT, ShouldTryRematchBattle
	goto_if_eq VAR_RESULT, TRUE, Route204_EventScript_SamanthaRematch
	setvar VAR_0x8004, TRAINER_KAREN_1
	specialvar VAR_RESULT, IsTrainerRegistered
	goto_if_eq VAR_RESULT, FALSE, Route204_EventScript_SamanthaTryRegister
	msgbox Route204_Text_SamanthaPostBattle, MSGBOX_DEFAULT
	release
	end

Route204_EventScript_SamanthaRegisterMatchCallAfterBattle::
	special PlayerFaceTrainerAfterBattle
	waitmovement 0
	goto_if_set FLAG_HAS_MATCH_CALL, Route204_EventScript_SamanthaRegisterMatchCall
	@ No POKéNAV yet: register quietly so the rematch is ready once the player gets one
	setvar VAR_0x8004, TRAINER_KAREN_1
	special SetMatchCallRegisteredFlag
	release
	end

Route204_EventScript_SamanthaRegisterMatchCall::
	msgbox Route204_Text_SamanthaRegisterShort, MSGBOX_DEFAULT
	register_matchcall TRAINER_KAREN_1
	release
	end

Route204_EventScript_SamanthaTryRegister::
	goto_if_set FLAG_HAS_MATCH_CALL, Route204_EventScript_SamanthaRegister
	msgbox Route204_Text_SamanthaPostBattle, MSGBOX_DEFAULT
	release
	end

Route204_EventScript_SamanthaRegister::
	msgbox Route204_Text_SamanthaRegister, MSGBOX_DEFAULT
	register_matchcall TRAINER_KAREN_1
	release
	end

Route204_EventScript_SamanthaRematch::
	trainerbattle_rematch TRAINER_KAREN_1, Route204_Text_SamanthaRematchIntro, Route204_Text_SamanthaRematchDefeat
	msgbox Route204_Text_SamanthaRematchPostBattle, MSGBOX_AUTOCLOSE
	end

Route204_EventScript_Brandon::
	trainerbattle_single TRAINER_JERRY_1, Route204_Text_BrandonIntro, Route204_Text_BrandonDefeat, Route204_EventScript_BrandonRegisterMatchCallAfterBattle
	specialvar VAR_RESULT, ShouldTryRematchBattle
	goto_if_eq VAR_RESULT, TRUE, Route204_EventScript_BrandonRematch
	setvar VAR_0x8004, TRAINER_JERRY_1
	specialvar VAR_RESULT, IsTrainerRegistered
	goto_if_eq VAR_RESULT, FALSE, Route204_EventScript_BrandonTryRegister
	msgbox Route204_Text_BrandonPostBattle, MSGBOX_DEFAULT
	release
	end

Route204_EventScript_BrandonRegisterMatchCallAfterBattle::
	special PlayerFaceTrainerAfterBattle
	waitmovement 0
	goto_if_set FLAG_HAS_MATCH_CALL, Route204_EventScript_BrandonRegisterMatchCall
	@ No POKéNAV yet: register quietly so the rematch is ready once the player gets one
	setvar VAR_0x8004, TRAINER_JERRY_1
	special SetMatchCallRegisteredFlag
	release
	end

Route204_EventScript_BrandonRegisterMatchCall::
	msgbox Route204_Text_BrandonRegisterShort, MSGBOX_DEFAULT
	register_matchcall TRAINER_JERRY_1
	release
	end

Route204_EventScript_BrandonTryRegister::
	goto_if_set FLAG_HAS_MATCH_CALL, Route204_EventScript_BrandonRegister
	msgbox Route204_Text_BrandonPostBattle, MSGBOX_DEFAULT
	release
	end

Route204_EventScript_BrandonRegister::
	msgbox Route204_Text_BrandonRegister, MSGBOX_DEFAULT
	register_matchcall TRAINER_JERRY_1
	release
	end

Route204_EventScript_BrandonRematch::
	trainerbattle_rematch TRAINER_JERRY_1, Route204_Text_BrandonRematchIntro, Route204_Text_BrandonRematchDefeat
	msgbox Route204_Text_BrandonRematchPostBattle, MSGBOX_AUTOCLOSE
	end

Route204_EventScript_Liv::
	trainerbattle_double TRAINER_GINA_AND_MIA_1, Route204_Text_LivIntro, Route204_Text_LivDefeat, Route204_Text_LivNotEnoughMons
	msgbox Route204_Text_LivPostBattle, MSGBOX_AUTOCLOSE
	end

Route204_EventScript_Liz::
	trainerbattle_double TRAINER_GINA_AND_MIA_1, Route204_Text_LizIntro, Route204_Text_LizDefeat, Route204_Text_LizNotEnoughMons
	msgbox Route204_Text_LizPostBattle, MSGBOX_AUTOCLOSE
	end

Route204_Text_RouteSign:
	.string "ROUTE 204\n"
	.string "{UP_ARROW} FLOAROMA TOWN  {DOWN_ARROW} JUBILIFE CITY$"

Route204_Text_RavagedPathSign:
	.string "RAVAGED PATH$"

Route204_Text_YouFoundMyHidingPlace:
	.string "Oh! You cut your way all the way\n"
	.string "back here just to see me?\p"
	.string "That's so sweet! You deserve\n"
	.string "a present!$"

Route204_Text_AttractExplanation:
	.string "That TM is ATTRACT.\p"
	.string "If the target is the opposite gender\n"
	.string "of your POKéMON, it falls in love\l"
	.string "and may not attack.$"
