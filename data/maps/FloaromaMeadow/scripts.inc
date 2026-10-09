@ Floaroma Meadow, reached by warp from Floaroma Town, laid out as in
@ Platinum. Its flowers have wild encounters. Two Team Galactic Grunts are
@ after the Pokéfan's HONEY; stepping onto row 48 at x 12-13 starts back to
@ back battles with them, as in Platinum, after which the Pokéfan hands over
@ the WORKS KEY they dropped and 10 HONEY, and from then on sells HONEY.
@ The scripts after these are Route 117's, kept for the Day Care scripts
@ that use their text.
FloaromaMeadow_MapScripts::
	.byte 0

FloaromaMeadow_EventScript_Grunts12::
	lockall
	setvar VAR_0x8008, 12
	goto FloaromaMeadow_EventScript_Grunts
	end

FloaromaMeadow_EventScript_Grunts13::
	lockall
	setvar VAR_0x8008, 13
	goto FloaromaMeadow_EventScript_Grunts
	end

FloaromaMeadow_EventScript_Grunts::
	msgbox FloaromaMeadow_Text_HandOverThatHoney, MSGBOX_DEFAULT
	msgbox FloaromaMeadow_Text_DoAsWeSay, MSGBOX_DEFAULT
	applymovement LOCALID_FLOAROMA_MEADOW_GRUNT_1, FloaromaMeadow_Movement_GruntNoticePlayer
	applymovement LOCALID_FLOAROMA_MEADOW_GRUNT_2, FloaromaMeadow_Movement_GruntNoticePlayer
	waitmovement 0
	msgbox FloaromaMeadow_Text_WhatllWeDo, MSGBOX_DEFAULT
	msgbox FloaromaMeadow_Text_EnsureTheBratStaysQuiet, MSGBOX_DEFAULT
	closemessage
	trainerbattle_no_intro TRAINER_DUDLEY, FloaromaMeadow_Text_Grunt1Defeat
	applywaitmovement LOCALID_FLOAROMA_MEADOW_GRUNT_1, Common_Movement_WalkInPlaceFasterRight
	msgbox FloaromaMeadow_Text_ThisIsSoLame, MSGBOX_DEFAULT
	closemessage
	applywaitmovement LOCALID_FLOAROMA_MEADOW_GRUNT_1, Common_Movement_WalkInPlaceFasterDown
	msgbox FloaromaMeadow_Text_ImUpNextHeheheh, MSGBOX_DEFAULT
	closemessage
	trainerbattle_no_intro TRAINER_PETE, FloaromaMeadow_Text_Grunt2Defeat
	msgbox FloaromaMeadow_Text_LetsGetBackToWindworks, MSGBOX_DEFAULT
	closemessage
	fadescreen FADE_TO_BLACK
	removeobject LOCALID_FLOAROMA_MEADOW_GRUNT_1
	removeobject LOCALID_FLOAROMA_MEADOW_GRUNT_2
	setvar VAR_FLOAROMA_MEADOW_STATE, 1
	fadescreen FADE_FROM_BLACK
	call_if_eq VAR_0x8008, 12, FloaromaMeadow_EventScript_PokefanWalkToPlayer12
	call_if_eq VAR_0x8008, 13, FloaromaMeadow_EventScript_PokefanWalkToPlayer13
	msgbox FloaromaMeadow_Text_KeyMustBeForWindworks, MSGBOX_DEFAULT
	giveitem ITEM_WORKS_KEY
	msgbox FloaromaMeadow_Text_ThankYouTakeALot, MSGBOX_DEFAULT
	checkitemspace ITEM_HONEY, 10
	goto_if_eq VAR_RESULT, FALSE, FloaromaMeadow_EventScript_NoRoomForThanksHoney
	giveitem ITEM_HONEY, 10
	msgbox FloaromaMeadow_Text_SlatherHoneyOnATree, MSGBOX_DEFAULT
	releaseall
	end

FloaromaMeadow_EventScript_NoRoomForThanksHoney::
	msgbox FloaromaMeadow_Text_YourBagsFull, MSGBOX_DEFAULT
	releaseall
	end

FloaromaMeadow_EventScript_PokefanWalkToPlayer12::
	applywaitmovement LOCALID_FLOAROMA_MEADOW_POKEFAN, FloaromaMeadow_Movement_PokefanWalkToPlayer12
	return

FloaromaMeadow_EventScript_PokefanWalkToPlayer13::
	applywaitmovement LOCALID_FLOAROMA_MEADOW_POKEFAN, FloaromaMeadow_Movement_PokefanWalkToPlayer13
	return

FloaromaMeadow_Movement_GruntNoticePlayer:
	walk_in_place_faster_down
	emote_exclamation_mark
	step_end

@ Down to where the Grunts stood, looks at the WORKS KEY, then turns to the player
FloaromaMeadow_Movement_PokefanWalkToPlayer12:
	walk_down
	walk_in_place_faster_right
	delay_8
	delay_8
	delay_8
	walk_in_place_faster_down
	step_end

FloaromaMeadow_Movement_PokefanWalkToPlayer13:
	walk_down
	walk_in_place_faster_right
	delay_8
	delay_8
	delay_8
	walk_right
	walk_in_place_faster_down
	step_end

@ Sells HONEY at ¥100 each, one or ten at a time
FloaromaMeadow_EventScript_HoneySeller::
	lock
	faceplayer
	message FloaromaMeadow_Text_BuySomeHoney
	waitmessage
	showmoneybox 0, 0
	multichoice 19, 6, MULTI_FLOAROMA_HONEY, FALSE
	switch VAR_RESULT
	case 0, FloaromaMeadow_EventScript_TryBuy1Honey
	case 1, FloaromaMeadow_EventScript_TryBuy10Honey
	goto FloaromaMeadow_EventScript_OopsyWellComeAgain
	end

FloaromaMeadow_EventScript_TryBuy1Honey::
	checkmoney 100
	goto_if_eq VAR_RESULT, FALSE, FloaromaMeadow_EventScript_NotEnoughMoney
	setvar VAR_TEMP_0, 1
	checkitemspace ITEM_HONEY, 1
	goto_if_eq VAR_RESULT, FALSE, FloaromaMeadow_EventScript_NoRoomForHoney
	removemoney 100
	goto FloaromaMeadow_EventScript_GiveHoney
	end

FloaromaMeadow_EventScript_TryBuy10Honey::
	checkmoney 1000
	goto_if_eq VAR_RESULT, FALSE, FloaromaMeadow_EventScript_NotEnoughMoney
	setvar VAR_TEMP_0, 10
	checkitemspace ITEM_HONEY, 10
	goto_if_eq VAR_RESULT, FALSE, FloaromaMeadow_EventScript_NoRoomForHoney
	removemoney 1000
	goto FloaromaMeadow_EventScript_GiveHoney
	end

FloaromaMeadow_EventScript_GiveHoney::
	updatemoneybox
	playse SE_SHOP
	waitse
	giveitem ITEM_HONEY, VAR_TEMP_0
	hidemoneybox
	release
	end

FloaromaMeadow_EventScript_OopsyWellComeAgain::
	msgbox FloaromaMeadow_Text_OopsyWellComeAgain, MSGBOX_DEFAULT
	hidemoneybox
	release
	end

FloaromaMeadow_EventScript_NotEnoughMoney::
	msgbox FloaromaMeadow_Text_NotEnoughMoney, MSGBOX_DEFAULT
	hidemoneybox
	release
	end

FloaromaMeadow_EventScript_NoRoomForHoney::
	msgbox FloaromaMeadow_Text_YourBagsFull, MSGBOX_DEFAULT
	hidemoneybox
	release
	end

FloaromaMeadow_Text_HandOverThatHoney:
	.string "GRUNT: Enough arguing!\n"
	.string "Hand over that sweet HONEY!$"

FloaromaMeadow_Text_DoAsWeSay:
	.string "GRUNT: Do as we say!\n"
	.string "TEAM GALACTIC will have that HONEY!\p"
	.string "We must have it to attract POKéMON\n"
	.string "in great numbers!$"

FloaromaMeadow_Text_WhatllWeDo:
	.string "GRUNT: Hey! This kid is a witness!\n"
	.string "What'll we do?$"

FloaromaMeadow_Text_EnsureTheBratStaysQuiet:
	.string "GRUNT: We don't need the brat\n"
	.string "running off to get help…\p"
	.string "The solution's simple!\n"
	.string "We ensure the brat stays quiet!$"

FloaromaMeadow_Text_Grunt1Defeat:
	.string "…This is why I'm gonna be\n"
	.string "stuck as a grunt forever…$"

FloaromaMeadow_Text_ThisIsSoLame:
	.string "GRUNT: This is so lame…\n"
	.string "They're going to dock my pay!$"

FloaromaMeadow_Text_ImUpNextHeheheh:
	.string "GRUNT: I'm up next!\n"
	.string "Heheheh…\p"
	.string "Battling you right after my cohort\n"
	.string "weakened you gives me the edge!$"

FloaromaMeadow_Text_Grunt2Defeat:
	.string "…I want a promotion so I can\n"
	.string "get some tougher POKéMON…$"

FloaromaMeadow_Text_LetsGetBackToWindworks:
	.string "GRUNT: This brat's tough!\n"
	.string "Like, really, really tough!\p"
	.string "Tougher than I can put into words,\n"
	.string "and I know a lot of words!\p"
	.string "We're done for here… Let's get back\n"
	.string "to the VALLEY WINDWORKS.$"

FloaromaMeadow_Text_KeyMustBeForWindworks:
	.string "Oh, what's this thing?\n"
	.string "They seem to have forgotten it…\p"
	.string "It's a WORKS KEY…\n"
	.string "It must be for the VALLEY WINDWORKS!\p"
	.string "It's of no use to me.\n"
	.string "You may as well have it.$"

FloaromaMeadow_Text_ThankYouTakeALot:
	.string "Thank you!\n"
	.string "You surely saved me.\p"
	.string "Those goons were trying to rob me\n"
	.string "of my sweet, sweet HONEY.\p"
	.string "They sure were weird, though.\n"
	.string "You saw how they were dressed.\p"
	.string "What they were saying didn't make\n"
	.string "a whole lot of sense, either.\p"
	.string "Oh! I need to thank you!\n"
	.string "This sweet, sweet HONEY!\p"
	.string "Go on, take a whole lot!$"

FloaromaMeadow_Text_SlatherHoneyOnATree:
	.string "Slather that HONEY on a\n"
	.string "sweet-smelling tree.\l"
	.string "It might attract POKéMON.$"

FloaromaMeadow_Text_BuySomeHoney:
	.string "Would you like to buy some HONEY\n"
	.string "for attracting POKéMON?\l"
	.string "Just ¥100 each!$"

FloaromaMeadow_Text_YourBagsFull:
	.string "Oh, too bad!\n"
	.string "Seems to me your BAG's full.\p"
	.string "You can't hold any more HONEY,\n"
	.string "a shame though it is.$"

FloaromaMeadow_Text_NotEnoughMoney:
	.string "Oopsy!\n"
	.string "You don't have enough money.$"

FloaromaMeadow_Text_OopsyWellComeAgain:
	.string "Oopsy!\n"
	.string "Well, come again.$"

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

