@ Jubilife City, north of Route 202. Drawn over Emerald's Rustboro City.
JubilifeCity_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, JubilifeCity_OnTransition
	.byte 0

JubilifeCity_OnTransition:
	setflag FLAG_VISITED_JUBILIFE_CITY
	end

JubilifeCity_EventScript_FatMan::
	lock
	faceplayer
	goto_if_set FLAG_DEVON_GOODS_STOLEN, JubilifeCity_EventScript_FatManSawGrunt
	msgbox JubilifeCity_Text_WeShortenItToDevon, MSGBOX_DEFAULT
	release
	end

JubilifeCity_EventScript_FatManSawGrunt::
	msgbox JubilifeCity_Text_SneakyLookingManWentAroundCorner, MSGBOX_DEFAULT
	release
	end

JubilifeCity_EventScript_DevonEmployee2::
	lock
	faceplayer
	msgbox JubilifeCity_Text_YoureNewAroundHere, MSGBOX_DEFAULT
	release
	end

JubilifeCity_EventScript_Woman::
	msgbox JubilifeCity_Text_GymLeaderIsntEasyWithFire, MSGBOX_NPC
	end

JubilifeCity_EventScript_Man1::
	lock
	faceplayer
	goto_if_set FLAG_BADGE01_GET, JubilifeCity_EventScript_Man1HaveBadge
	msgbox JubilifeCity_Text_HaveYouChallengedGym, MSGBOX_DEFAULT
	release
	end

JubilifeCity_EventScript_Man1HaveBadge::
	msgbox JubilifeCity_Text_HeyThatsJubilifesGymBadge, MSGBOX_DEFAULT
	release
	end

JubilifeCity_EventScript_Boy2::
	lock
	faceplayer
	goto_if_set FLAG_RECEIVED_POKENAV, JubilifeCity_EventScript_Boy2BrineyLeftTunnel
	msgbox JubilifeCity_Text_MrBrineyWalksInTheTunnel, MSGBOX_DEFAULT
	release
	end

JubilifeCity_EventScript_Boy2BrineyLeftTunnel::
	msgbox JubilifeCity_Text_MrBrineyLovesPeeko, MSGBOX_DEFAULT
	release
	end

JubilifeCity_EventScript_Twin::
	msgbox JubilifeCity_Text_WowYouHavePokemon, MSGBOX_NPC
	end

JubilifeCity_EventScript_NinjaBoy::
	msgbox JubilifeCity_Text_CatchRarePokemonIfIGoToSchool, MSGBOX_SIGN
	end

JubilifeCity_EventScript_PoketchCompanySign::
	msgbox JubilifeCity_Text_PoketchCompanySign, MSGBOX_SIGN
	end

JubilifeCity_EventScript_JubilifeTVSign::
	msgbox JubilifeCity_Text_JubilifeTVSign, MSGBOX_SIGN
	end

JubilifeCity_EventScript_CitySign::
	msgbox JubilifeCity_Text_CitySign, MSGBOX_SIGN
	end

JubilifeCity_EventScript_TrainersSchoolSign::
	msgbox JubilifeCity_Text_TrainersSchoolSign, MSGBOX_SIGN
	end

JubilifeCity_EventScript_LittleBoy::
	lock
	faceplayer
	msgbox JubilifeCity_Text_PokemonCanChangeLookFromExp, MSGBOX_DEFAULT
	applywaitmovement LOCALID_JUBILIFE_LITTLE_BOY, Common_Movement_FaceOriginalDirection
	release
	end

JubilifeCity_EventScript_LittleGirl::
	lock
	faceplayer
	msgbox JubilifeCity_Text_PokemonChangeShape, MSGBOX_DEFAULT
	applywaitmovement LOCALID_JUBILIFE_LITTLE_GIRL, Common_Movement_FaceOriginalDirection
	release
	end

JubilifeCity_EventScript_Man2::
	lock
	faceplayer
	msgbox JubilifeCity_Text_TradePokemonGrowFast, MSGBOX_DEFAULT
	release
	end

JubilifeCity_EventScript_Boy1::
	msgbox JubilifeCity_Text_YouCanHave2On2Battle, MSGBOX_NPC
	end

JubilifeCity_EventScript_PoketchMan::
	lock
	faceplayer
	goto_if_set FLAG_RECEIVED_POKENAV, JubilifeCity_EventScript_PoketchManAfter
	goto_if_unset FLAG_RECEIVED_TICKET_JUBILIFE_CENTER_CLOWN, JubilifeCity_EventScript_PoketchManNeedTickets
	goto_if_unset FLAG_RECEIVED_TICKET_JUBILIFE_TV_CLOWN, JubilifeCity_EventScript_PoketchManNeedTickets
	goto_if_unset FLAG_RECEIVED_TICKET_JUBILIFE_FOUNTAIN_CLOWN, JubilifeCity_EventScript_PoketchManNeedTickets
	msgbox JubilifeCity_Text_YouHaveAllThreeTickets, MSGBOX_DEFAULT
	playfanfare MUS_OBTAIN_ITEM
	message JubilifeCity_Text_ReceivedPoketch
	waitfanfare
	waitmessage
	setflag FLAG_SYS_POKENAV_GET
	setflag FLAG_RECEIVED_POKENAV
	msgbox JubilifeCity_Text_ExplainPoketch, MSGBOX_DEFAULT
	release
	end

JubilifeCity_EventScript_PoketchManNeedTickets::
	msgbox JubilifeCity_Text_FindTheClowns, MSGBOX_DEFAULT
	release
	end

JubilifeCity_EventScript_PoketchManAfter::
	msgbox JubilifeCity_Text_EnjoyThePoketch, MSGBOX_DEFAULT
	release
	end

JubilifeCity_EventScript_CenterClown::
	lock
	faceplayer
	goto_if_set FLAG_RECEIVED_TICKET_JUBILIFE_CENTER_CLOWN, JubilifeCity_EventScript_ClownAlreadyGaveTicket
	msgbox JubilifeCity_Text_CenterClownQuiz, MSGBOX_DEFAULT
	multichoice 21, 6, MULTI_JUBILIFE_QUIZ_CENTER_ROOF, FALSE
	goto_if_eq VAR_RESULT, MULTI_B_PRESSED, JubilifeCity_EventScript_ClownQuizDeclined
	goto_if_ne VAR_RESULT, 2, JubilifeCity_EventScript_ClownWrongAnswer
	call JubilifeCity_EventScript_ClownGiveTicket
	setflag FLAG_RECEIVED_TICKET_JUBILIFE_CENTER_CLOWN
	goto JubilifeCity_EventScript_ClownAfterTicket

JubilifeCity_EventScript_TVClown::
	lock
	faceplayer
	goto_if_set FLAG_RECEIVED_TICKET_JUBILIFE_TV_CLOWN, JubilifeCity_EventScript_ClownAlreadyGaveTicket
	msgbox JubilifeCity_Text_TVClownQuiz, MSGBOX_DEFAULT
	multichoice 21, 6, MULTI_JUBILIFE_QUIZ_CITY, FALSE
	goto_if_eq VAR_RESULT, MULTI_B_PRESSED, JubilifeCity_EventScript_ClownQuizDeclined
	goto_if_ne VAR_RESULT, 1, JubilifeCity_EventScript_ClownWrongAnswer
	call JubilifeCity_EventScript_ClownGiveTicket
	setflag FLAG_RECEIVED_TICKET_JUBILIFE_TV_CLOWN
	goto JubilifeCity_EventScript_ClownAfterTicket

JubilifeCity_EventScript_FountainClown::
	lock
	faceplayer
	goto_if_set FLAG_RECEIVED_TICKET_JUBILIFE_FOUNTAIN_CLOWN, JubilifeCity_EventScript_ClownAlreadyGaveTicket
	msgbox JubilifeCity_Text_FountainClownQuiz, MSGBOX_DEFAULT
	multichoice 21, 6, MULTI_JUBILIFE_QUIZ_TYPE, FALSE
	goto_if_eq VAR_RESULT, MULTI_B_PRESSED, JubilifeCity_EventScript_ClownQuizDeclined
	goto_if_ne VAR_RESULT, 0, JubilifeCity_EventScript_ClownWrongAnswer
	call JubilifeCity_EventScript_ClownGiveTicket
	setflag FLAG_RECEIVED_TICKET_JUBILIFE_FOUNTAIN_CLOWN
	goto JubilifeCity_EventScript_ClownAfterTicket

JubilifeCity_EventScript_ClownGiveTicket::
	msgbox JubilifeCity_Text_ClownCorrect, MSGBOX_DEFAULT
	playfanfare MUS_OBTAIN_ITEM
	message JubilifeCity_Text_ReceivedTicket
	waitfanfare
	waitmessage
	return

JubilifeCity_EventScript_ClownAfterTicket::
	msgbox JubilifeCity_Text_TakeTicketToSchool, MSGBOX_DEFAULT
	release
	end

JubilifeCity_EventScript_ClownWrongAnswer::
	msgbox JubilifeCity_Text_ClownWrong, MSGBOX_DEFAULT
	release
	end

JubilifeCity_EventScript_ClownQuizDeclined::
	msgbox JubilifeCity_Text_ClownComeBack, MSGBOX_DEFAULT
	release
	end

JubilifeCity_EventScript_ClownAlreadyGaveTicket::
	msgbox JubilifeCity_Text_ClownAlreadyGaveTicket, MSGBOX_DEFAULT
	release
	end

JubilifeCity_Text_WeShortenItToDevon:
	.string "The DEVON CORPORATION…\n"
	.string "We all just shorten it to DEVON.\p"
	.string "That company makes all sorts of\n"
	.string "convenient products.$"

JubilifeCity_Text_SneakyLookingManWentAroundCorner:
	.string "Hm? A sneaky-looking man?\p"
	.string "Come to think of it, yes, a shady-\n"
	.string "looking guy went around the corner.$"

JubilifeCity_Text_HaveYouChallengedGym:
	.string "Have you taken the POKéMON GYM\n"
	.string "challenge?\p"
	.string "When you get that shiny GYM BADGE\n"
	.string "in hand, I guess TRAINERS begin to\l"
	.string "realize what is required of them.$"

JubilifeCity_Text_HeyThatsJubilifesGymBadge:
	.string "Hey, that's RUSTBORO's GYM BADGE!\p"
	.string "Out of all the POKéMON GYM BADGES,\n"
	.string "RUSTBORO's is the coolest, I'd say.$"

JubilifeCity_Text_YoureNewAroundHere:
	.string "Oh? Who might you be?\n"
	.string "You're a new face around these parts.\p"
	.string "Have you just transferred into the\n"
	.string "POKéMON TRAINER'S SCHOOL?$"

JubilifeCity_Text_GymLeaderIsntEasyWithFire:
	.string "I challenged the GYM LEADER, but…\p"
	.string "It's not going to be easy winning with\n"
	.string "my FIRE-type POKéMON…\p"
	.string "FIRE-type POKéMON don't match up\n"
	.string "well against ROCK-type POKéMON…$"

JubilifeCity_Text_MrBrineyWalksInTheTunnel:
	.string "The old sailor MR. BRINEY lives in\n"
	.string "a cottage by the sea.\p"
	.string "He goes for walks in the tunnel every\n"
	.string "so often.$"

JubilifeCity_Text_MrBrineyLovesPeeko:
	.string "The old sailor MR. BRINEY lives in\n"
	.string "a cottage by the sea.\p"
	.string "He said he was going shopping in\n"
	.string "SLATEPORT for his pet, PEEKO.\p"
	.string "That old sea dog, he must really love\n"
	.string "that PEEKO.$"

JubilifeCity_Text_WowYouHavePokemon:
	.string "Wow, you have POKéMON with you, too.\p"
	.string "When I get bigger, I'm going to go\n"
	.string "places with POKéMON, too.$"

JubilifeCity_Text_CatchRarePokemonIfIGoToSchool:
	.string "POKéMON TRAINER'S SCHOOL!\p"
	.string "If I go to this school, will I be able\n"
	.string "to catch rare POKéMON easily?$"

JubilifeCity_Text_PokemonCanChangeLookFromExp:
	.string "If a POKéMON gains experience in\n"
	.string "battles, it can sometimes change in\l"
	.string "the way it looks.$"

JubilifeCity_Text_PokemonChangeShape:
	.string "A POKéMON changes shape?\n"
	.string "If one did that, I would be shocked!$"

JubilifeCity_Text_TradePokemonGrowFast:
	.string "A POKéMON you get in a trade from\n"
	.string "someone grows fast.\p"
	.string "But if you don't have certain GYM\n"
	.string "BADGES, it may not obey you…$"

JubilifeCity_Text_PoketchCompanySign:
	.string "POKéTCH COMPANY$"

JubilifeCity_Text_JubilifeTVSign:
	.string "JUBILIFE TV$"

JubilifeCity_Text_CitySign:
	.string "JUBILIFE CITY\p"
	.string "“The most modern city in SINNOH.”$"

JubilifeCity_Text_TrainersSchoolSign:
	.string "POKéMON TRAINER'S SCHOOL\p"
	.string "“We'll teach you anything about\n"
	.string "POKéMON!”$"

JubilifeCity_Text_YouCanHave2On2Battle:
	.string "Did you know this?\p"
	.string "You can have a 2-on-2 battle even\n"
	.string "if you're not with another TRAINER.\p"
	.string "If you catch the eyes of two TRAINERS\n"
	.string "when you have two or more POKéMON,\l"
	.string "they'll both challenge you.\p"
	.string "Don't you think it'd be cool if you\n"
	.string "could beat two TRAINERS by yourself?$"

JubilifeCity_Text_FindTheClowns:
	.string "Hello there! I'm handing out the\n"
	.string "POKéTCH, the POKéMON WATCH!\p"
	.string "But first, a little game!\n"
	.string "Three clowns are hiding in JUBILIFE.\p"
	.string "One is by the POKéMON CENTER, one\n"
	.string "is by JUBILIFE TV, and one is by\l"
	.string "the fountains to the southwest.\p"
	.string "Answer each clown's quiz to get\n"
	.string "a TICKET from them.\p"
	.string "Bring me all three TICKETS, and the\n"
	.string "POKéTCH is yours!$"

JubilifeCity_Text_YouHaveAllThreeTickets:
	.string "Oh! Those are the clowns' TICKETS!\n"
	.string "You found all three of them!\p"
	.string "As promised, I'll trade you\n"
	.string "a POKéTCH for them.$"

JubilifeCity_Text_ReceivedPoketch:
	.string "{PLAYER} handed over the three\n"
	.string "TICKETS and received a POKéTCH!$"

JubilifeCity_Text_ExplainPoketch:
	.string "The POKéTCH is a POKéMON WATCH made\n"
	.string "right here in JUBILIFE by the\l"
	.string "POKéTCH COMPANY.\p"
	.string "It's a must-have for any TRAINER\n"
	.string "on a journey through SINNOH!$"

JubilifeCity_Text_EnjoyThePoketch:
	.string "How do you like the POKéTCH?\n"
	.string "I hope it helps you on your journey!$"

JubilifeCity_Text_CenterClownQuiz:
	.string "Ta-da! You found me!\n"
	.string "Here's my POKéTCH quiz!\p"
	.string "What color is the roof of\n"
	.string "a POKéMON CENTER?$"

JubilifeCity_Text_TVClownQuiz:
	.string "Ta-da! You found me!\n"
	.string "Here's my POKéTCH quiz!\p"
	.string "What is the name of\n"
	.string "this city?$"

JubilifeCity_Text_FountainClownQuiz:
	.string "Ta-da! You found me!\n"
	.string "Here's my POKéTCH quiz!\p"
	.string "Which type of move is super\n"
	.string "effective on WATER-type POKéMON?$"

JubilifeCity_Text_ClownCorrect:
	.string "Ding-ding-ding! That's right!\n"
	.string "Here's your prize!$"

JubilifeCity_Text_ReceivedTicket:
	.string "{PLAYER} received a TICKET!$"

JubilifeCity_Text_TakeTicketToSchool:
	.string "Take the TICKET to the man by the\n"
	.string "TRAINERS' SCHOOL.\p"
	.string "Collect all three TICKETS and he'll\n"
	.string "give you a POKéTCH!$"

JubilifeCity_Text_ClownWrong:
	.string "Bzzzt! Sorry, that's wrong!\n"
	.string "Have a think and try me again!$"

JubilifeCity_Text_ClownComeBack:
	.string "Aww, no answer?\n"
	.string "Come back when you want to play!$"

JubilifeCity_Text_ClownAlreadyGaveTicket:
	.string "You already got my TICKET!\n"
	.string "Have you found the other clowns?$"
