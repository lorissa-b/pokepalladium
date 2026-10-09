@ Jubilife City, north of Route 202, laid out as in Platinum.
JubilifeCity_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, JubilifeCity_OnTransition
	.byte 0

JubilifeCity_OnTransition:
	setflag FLAG_VISITED_JUBILIFE_CITY
	call Common_EventScript_SetupAssistantGfxId
	call_if_eq VAR_JUBILIFE_LOOKER_STATE, 0, JubilifeCity_EventScript_SetLookerFirstArrivalPos
	call_if_set FLAG_BADGE01_GET, JubilifeCity_EventScript_TryStartTeamGalactic
	call_if_eq VAR_JUBILIFE_GALACTIC_STATE, 1, JubilifeCity_EventScript_ShowTeamGalactic
	call_if_ne VAR_JUBILIFE_GALACTIC_STATE, 1, JubilifeCity_EventScript_HideTeamGalactic
	end

@ Before the player first arrives, Looker is snooping around north of the
@ Pokémon Center; afterwards he keeps watch by the Route 203 exit at 58, 20
JubilifeCity_EventScript_SetLookerFirstArrivalPos::
	setobjectxyperm LOCALID_JUBILIFE_LOOKER, 49, 38
	setobjectmovementtype LOCALID_JUBILIFE_LOOKER, MOVEMENT_TYPE_FACE_DOWN
	return

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
	@ With the POKéTCH, Looker stops keeping watch by the Route 203 exit
	setvar VAR_JUBILIFE_LOOKER_STATE, 2
	removeobject LOCALID_JUBILIFE_LOOKER
	setflag FLAG_HIDE_JUBILIFE_CITY_LOOKER
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
	.string "the POKéTCH COMPANY.\p"
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

@ ---- First arrival (Platinum's scene, without Looker's Vs. Recorder) ----
@ Stepping north out of the Route 202 corridor (x 45-48, y 60). Birch's assistant
@ comes down the road, asks about the player's POKéMON and leads them north to
@ the Trainers' School. On the way they run into Looker, who introduces himself
@ and heads off to keep watch by the Route 203 exit.
JubilifeCity_EventScript_FirstArrival45::
	lockall
	setvar VAR_0x8008, 45
	setobjectxyperm LOCALID_JUBILIFE_ASSISTANT, 45, 52
	goto JubilifeCity_EventScript_FirstArrival
	end

JubilifeCity_EventScript_FirstArrival46::
	lockall
	setvar VAR_0x8008, 46
	setobjectxyperm LOCALID_JUBILIFE_ASSISTANT, 46, 52
	goto JubilifeCity_EventScript_FirstArrival
	end

JubilifeCity_EventScript_FirstArrival47::
	lockall
	setvar VAR_0x8008, 47
	setobjectxyperm LOCALID_JUBILIFE_ASSISTANT, 47, 52
	goto JubilifeCity_EventScript_FirstArrival
	end

JubilifeCity_EventScript_FirstArrival48::
	lockall
	setvar VAR_0x8008, 48
	setobjectxyperm LOCALID_JUBILIFE_ASSISTANT, 48, 52
	goto JubilifeCity_EventScript_FirstArrival
	end

JubilifeCity_EventScript_FirstArrival::
	clearflag FLAG_HIDE_JUBILIFE_CITY_ASSISTANT
	addobject LOCALID_JUBILIFE_ASSISTANT
	applywaitmovement LOCALID_JUBILIFE_ASSISTANT, JubilifeCity_Movement_AssistantNoticePlayer
	applywaitmovement LOCALID_JUBILIFE_ASSISTANT, JubilifeCity_Movement_AssistantWalkToPlayer
	msgbox JubilifeCity_Text_AssistantAreYouCatchingPokemon, MSGBOX_DEFAULT
	getpartysize
	buffernumberstring STR_VAR_1, VAR_RESULT
	call_if_ge VAR_RESULT, 4, JubilifeCity_EventScript_AssistantAtLeastFourPokemon
	call_if_lt VAR_RESULT, 4, JubilifeCity_EventScript_AssistantLessThanFourPokemon
	msgbox JubilifeCity_Text_AssistantFollowMe, MSGBOX_DEFAULT
	closemessage
	call_if_eq VAR_0x8008, 45, JubilifeCity_EventScript_WalkNorth45
	call_if_eq VAR_0x8008, 46, JubilifeCity_EventScript_WalkNorth46
	call_if_eq VAR_0x8008, 47, JubilifeCity_EventScript_WalkNorth47
	call_if_eq VAR_0x8008, 48, JubilifeCity_EventScript_WalkNorth48
	msgbox JubilifeCity_Text_AssistantWhatIsHeDoing, MSGBOX_DEFAULT
	closemessage
	applywaitmovement LOCALID_JUBILIFE_LOOKER, JubilifeCity_Movement_LookerWalkAround
	applymovement LOCALID_JUBILIFE_LOOKER, JubilifeCity_Movement_LookerWalkEast
	applymovement LOCALID_JUBILIFE_ASSISTANT, JubilifeCity_Movement_AssistantWalkToLooker
	applymovement LOCALID_PLAYER, JubilifeCity_Movement_PlayerWalkToLooker
	waitmovement 0
	msgbox JubilifeCity_Text_AssistantUm, MSGBOX_DEFAULT
	applywaitmovement LOCALID_JUBILIFE_LOOKER, JubilifeCity_Movement_LookerNoticePlayer
	playbgm MUS_ENCOUNTER_SUSPICIOUS, FALSE
	msgbox JubilifeCity_Text_LookerWhaaat, MSGBOX_DEFAULT
	msgbox JubilifeCity_Text_LookerHowDidYouUnmaskMe, MSGBOX_DEFAULT
	closemessage
	applywaitmovement LOCALID_JUBILIFE_ASSISTANT, Common_Movement_WalkInPlaceFasterUp
	msgbox JubilifeCity_Text_AssistantJustMakingConversation, MSGBOX_DEFAULT
	msgbox JubilifeCity_Text_LookerIKnowBetter, MSGBOX_DEFAULT
	msgbox JubilifeCity_Text_LookerIsSayingFamiliar, MSGBOX_YESNO
	call_if_eq VAR_RESULT, YES, JubilifeCity_EventScript_LookerTakingFromOthersIsWrong
	call_if_eq VAR_RESULT, NO, JubilifeCity_EventScript_LookerYouClaimToNotKnowIt
	@ In Platinum he hands over the Vs. Recorder here; that comes later
	msgbox JubilifeCity_Text_LookerOnTheLookout, MSGBOX_DEFAULT
	closemessage
	applywaitmovement LOCALID_JUBILIFE_LOOKER, JubilifeCity_Movement_LookerLookAround
	msgbox JubilifeCity_Text_LookerInformMeOfAnyHappenings, MSGBOX_DEFAULT
	closemessage
	applymovement LOCALID_JUBILIFE_ASSISTANT, JubilifeCity_Movement_WatchLookerLeave
	applymovement LOCALID_PLAYER, JubilifeCity_Movement_WatchLookerLeave
	applymovement LOCALID_JUBILIFE_LOOKER, JubilifeCity_Movement_LookerLeave
	waitmovement 0
	@ Off screen now: he takes up his post by the Route 203 exit
	removeobject LOCALID_JUBILIFE_LOOKER
	setobjectxyperm LOCALID_JUBILIFE_LOOKER, 58, 20
	setobjectmovementtype LOCALID_JUBILIFE_LOOKER, MOVEMENT_TYPE_FACE_DOWN
	addobject LOCALID_JUBILIFE_LOOKER
	fadedefaultbgm
	msgbox JubilifeCity_Text_AssistantPoliceMustBeHard, MSGBOX_DEFAULT
	closemessage
	delay 30
	applymovement LOCALID_JUBILIFE_ASSISTANT, JubilifeCity_Movement_AssistantWalkToSchool
	applymovement LOCALID_PLAYER, JubilifeCity_Movement_PlayerWatchAssistantWalkToSchool
	waitmovement 0
	msgbox JubilifeCity_Text_AssistantHereIsTheTrainersSchool, MSGBOX_DEFAULT
	closemessage
	applywaitmovement LOCALID_JUBILIFE_ASSISTANT, JubilifeCity_Movement_AssistantWalkBackFromSchool
	msgbox JubilifeCity_Text_AssistantByeNow, MSGBOX_DEFAULT
	closemessage
	applymovement LOCALID_JUBILIFE_ASSISTANT, JubilifeCity_Movement_AssistantLeave
	applymovement LOCALID_PLAYER, JubilifeCity_Movement_PlayerWatchAssistantLeave
	waitmovement 0
	removeobject LOCALID_JUBILIFE_ASSISTANT
	setflag FLAG_HIDE_JUBILIFE_CITY_ASSISTANT
	setvar VAR_JUBILIFE_LOOKER_STATE, 1
	releaseall
	end

JubilifeCity_EventScript_AssistantAtLeastFourPokemon::
	msgbox JubilifeCity_Text_AssistantAlreadyGotXPokemon, MSGBOX_DEFAULT
	return

JubilifeCity_EventScript_AssistantLessThanFourPokemon::
	msgbox JubilifeCity_Text_AssistantSaferIfYouCatchMore, MSGBOX_DEFAULT
	return

JubilifeCity_EventScript_LookerTakingFromOthersIsWrong::
	msgbox JubilifeCity_Text_LookerTakingFromOthersIsWrong, MSGBOX_DEFAULT
	return

JubilifeCity_EventScript_LookerYouClaimToNotKnowIt::
	msgbox JubilifeCity_Text_LookerYouClaimToNotKnowIt, MSGBOX_DEFAULT
	return

@ The assistant ends at 46, 41 and the player at 46, 42, below Looker at 49, 38
JubilifeCity_EventScript_WalkNorth45::
	applymovement LOCALID_JUBILIFE_ASSISTANT, JubilifeCity_Movement_AssistantWalkNorth45
	applymovement LOCALID_PLAYER, JubilifeCity_Movement_PlayerWalkNorth45
	waitmovement 0
	return

JubilifeCity_EventScript_WalkNorth46::
	applymovement LOCALID_JUBILIFE_ASSISTANT, JubilifeCity_Movement_AssistantWalkNorth46
	applymovement LOCALID_PLAYER, JubilifeCity_Movement_PlayerWalkNorth46
	waitmovement 0
	return

JubilifeCity_EventScript_WalkNorth47::
	applymovement LOCALID_JUBILIFE_ASSISTANT, JubilifeCity_Movement_AssistantWalkNorth47
	applymovement LOCALID_PLAYER, JubilifeCity_Movement_PlayerWalkNorth47
	waitmovement 0
	return

JubilifeCity_EventScript_WalkNorth48::
	applymovement LOCALID_JUBILIFE_ASSISTANT, JubilifeCity_Movement_AssistantWalkNorth48
	applymovement LOCALID_PLAYER, JubilifeCity_Movement_PlayerWalkNorth48
	waitmovement 0
	return

JubilifeCity_Movement_AssistantWalkNorth45:
	walk_right
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	step_end

JubilifeCity_Movement_PlayerWalkNorth45:
	walk_up
	walk_right
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	step_end

JubilifeCity_Movement_AssistantWalkNorth46:
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	step_end

JubilifeCity_Movement_PlayerWalkNorth46:
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	step_end

JubilifeCity_Movement_AssistantWalkNorth47:
	walk_left
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	step_end

JubilifeCity_Movement_PlayerWalkNorth47:
	walk_up
	walk_left
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	step_end

JubilifeCity_Movement_AssistantWalkNorth48:
	walk_left
	walk_left
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	step_end

JubilifeCity_Movement_PlayerWalkNorth48:
	walk_up
	walk_left
	walk_left
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	step_end

JubilifeCity_Movement_AssistantNoticePlayer:
	walk_down
	walk_down
	walk_down
	walk_down
	emote_exclamation_mark
	delay_16
	step_end

JubilifeCity_Movement_AssistantWalkToPlayer:
	walk_down
	walk_down
	walk_down
	step_end

@ Looker looks around, slinks five steps west, then comes back east to 46, 38
JubilifeCity_Movement_LookerWalkAround:
	walk_in_place_faster_left
	delay_8
	delay_8
	walk_in_place_faster_down
	delay_8
	walk_slow_left
	walk_fast_left
	walk_fast_left
	walk_fast_left
	walk_fast_left
	walk_fast_left
	walk_in_place_faster_right
	delay_8
	walk_in_place_faster_up
	delay_8
	step_end

JubilifeCity_Movement_LookerWalkEast:
	walk_slow_right
	walk_fast_right
	walk_fast_right
	step_end

JubilifeCity_Movement_AssistantWalkToLooker:
	walk_up
	walk_up
	step_end

@ To 47, 39, beside the assistant and facing Looker
JubilifeCity_Movement_PlayerWalkToLooker:
	walk_up
	walk_up
	walk_right
	walk_up
	step_end

JubilifeCity_Movement_LookerNoticePlayer:
	walk_in_place_faster_down
	emote_exclamation_mark
	step_end

JubilifeCity_Movement_LookerLookAround:
	walk_in_place_faster_left
	delay_8
	walk_in_place_faster_right
	delay_8
	delay_8
	walk_in_place_faster_down
	step_end

JubilifeCity_Movement_WatchLookerLeave:
	walk_in_place_faster_right
	delay_8
	delay_8
	delay_8
	delay_8
	walk_in_place_faster_up
	step_end

@ East to 49, 38, then north out of sight to 49, 30
JubilifeCity_Movement_LookerLeave:
	walk_fast_right
	walk_fast_right
	walk_fast_right
	walk_in_place_faster_left
	delay_8
	walk_slow_up
	walk_fast_up
	walk_fast_up
	walk_fast_up
	walk_fast_up
	walk_fast_up
	walk_fast_up
	walk_fast_up
	step_end

@ To 44, 40, at the Trainers' School door's east side
JubilifeCity_Movement_AssistantWalkToSchool:
	walk_down
	walk_left
	walk_left
	step_end

JubilifeCity_Movement_PlayerWatchAssistantWalkToSchool:
	delay_4
	walk_in_place_faster_left
	step_end

JubilifeCity_Movement_AssistantWalkBackFromSchool:
	walk_up
	walk_right
	walk_right
	walk_in_place_faster_right
	step_end

JubilifeCity_Movement_AssistantLeave:
	walk_down
	walk_down
	walk_down
	walk_down
	walk_down
	walk_down
	walk_down
	walk_down
	walk_down
	step_end

JubilifeCity_Movement_PlayerWatchAssistantLeave:
	delay_8
	walk_in_place_faster_down
	step_end

@ ---- Looker by the Route 203 exit, until the player has a POKéTCH ----
@ Crossing x 60 (rows 21-24): Looker comes down from 58, 20, steps in beside
@ the player, asks about the POKéTCH and walks them one step back west.
JubilifeCity_EventScript_LookerBlock21::
	lockall
	setvar VAR_0x8008, 21
	applymovement LOCALID_PLAYER, JubilifeCity_Movement_PlayerFaceLooker
	applymovement LOCALID_JUBILIFE_LOOKER, JubilifeCity_Movement_LookerWalkToPlayer21
	waitmovement 0
	goto JubilifeCity_EventScript_LookerBlockPlayer
	end

JubilifeCity_EventScript_LookerBlock22::
	lockall
	setvar VAR_0x8008, 22
	applymovement LOCALID_PLAYER, JubilifeCity_Movement_PlayerFaceLooker
	applymovement LOCALID_JUBILIFE_LOOKER, JubilifeCity_Movement_LookerWalkToPlayer22
	waitmovement 0
	goto JubilifeCity_EventScript_LookerBlockPlayer
	end

JubilifeCity_EventScript_LookerBlock23::
	lockall
	setvar VAR_0x8008, 23
	applymovement LOCALID_PLAYER, JubilifeCity_Movement_PlayerFaceLooker
	applymovement LOCALID_JUBILIFE_LOOKER, JubilifeCity_Movement_LookerWalkToPlayer23
	waitmovement 0
	goto JubilifeCity_EventScript_LookerBlockPlayer
	end

JubilifeCity_EventScript_LookerBlock24::
	lockall
	setvar VAR_0x8008, 24
	applymovement LOCALID_PLAYER, JubilifeCity_Movement_PlayerFaceLooker
	applymovement LOCALID_JUBILIFE_LOOKER, JubilifeCity_Movement_LookerWalkToPlayer24
	waitmovement 0
	goto JubilifeCity_EventScript_LookerBlockPlayer
	end

JubilifeCity_EventScript_LookerBlockPlayer::
	msgbox JubilifeCity_Text_LookerTellMeNoPoketch, MSGBOX_DEFAULT
	closemessage
	applymovement LOCALID_JUBILIFE_LOOKER, JubilifeCity_Movement_LookerStepBackWest
	applymovement LOCALID_PLAYER, JubilifeCity_Movement_PlayerStepWest
	waitmovement 0
	call_if_eq VAR_0x8008, 21, JubilifeCity_EventScript_LookerWalkBack21
	call_if_eq VAR_0x8008, 22, JubilifeCity_EventScript_LookerWalkBack22
	call_if_eq VAR_0x8008, 23, JubilifeCity_EventScript_LookerWalkBack23
	call_if_eq VAR_0x8008, 24, JubilifeCity_EventScript_LookerWalkBack24
	releaseall
	end

JubilifeCity_EventScript_LookerWalkBack21::
	applywaitmovement LOCALID_JUBILIFE_LOOKER, JubilifeCity_Movement_LookerWalkBack21
	return

JubilifeCity_EventScript_LookerWalkBack22::
	applywaitmovement LOCALID_JUBILIFE_LOOKER, JubilifeCity_Movement_LookerWalkBack22
	return

JubilifeCity_EventScript_LookerWalkBack23::
	applywaitmovement LOCALID_JUBILIFE_LOOKER, JubilifeCity_Movement_LookerWalkBack23
	return

JubilifeCity_EventScript_LookerWalkBack24::
	applywaitmovement LOCALID_JUBILIFE_LOOKER, JubilifeCity_Movement_LookerWalkBack24
	return

JubilifeCity_Movement_LookerWalkToPlayer21:
	face_down
	emote_exclamation_mark
	delay_16
	walk_down
	walk_right
	step_end

JubilifeCity_Movement_LookerWalkBack21:
	walk_up
	walk_in_place_faster_down
	step_end

JubilifeCity_Movement_LookerWalkToPlayer22:
	face_down
	emote_exclamation_mark
	delay_16
	walk_down
	walk_down
	walk_right
	step_end

JubilifeCity_Movement_LookerWalkBack22:
	walk_up
	walk_up
	walk_in_place_faster_down
	step_end

JubilifeCity_Movement_LookerWalkToPlayer23:
	face_down
	emote_exclamation_mark
	delay_16
	walk_down
	walk_down
	walk_down
	walk_right
	step_end

JubilifeCity_Movement_LookerWalkBack23:
	walk_up
	walk_up
	walk_up
	walk_in_place_faster_down
	step_end

JubilifeCity_Movement_LookerWalkToPlayer24:
	face_down
	emote_exclamation_mark
	delay_16
	walk_down
	walk_down
	walk_down
	walk_down
	walk_right
	step_end

JubilifeCity_Movement_LookerWalkBack24:
	walk_up
	walk_up
	walk_up
	walk_up
	walk_in_place_faster_down
	step_end

JubilifeCity_Movement_PlayerFaceLooker:
	delay_8
	delay_8
	delay_8
	delay_8
	delay_8
	walk_in_place_faster_left
	step_end

JubilifeCity_Movement_LookerStepBackWest:
	lock_facing_direction
	walk_left
	unlock_facing_direction
	step_end

JubilifeCity_Movement_PlayerStepWest:
	walk_left
	step_end

JubilifeCity_EventScript_Looker::
	lock
	faceplayer
	msgbox JubilifeCity_Text_LookerItIsYouNoPoketch, MSGBOX_DEFAULT
	closemessage
	applywaitmovement LOCALID_JUBILIFE_LOOKER, Common_Movement_FaceDown
	release
	end

JubilifeCity_Text_AssistantAreYouCatchingPokemon:
	.string "ASSISTANT: Hi, {PLAYER}!\n"
	.string "Are you catching POKéMON?$"

JubilifeCity_Text_AssistantAlreadyGotXPokemon:
	.string "Wow! You've already got {STR_VAR_1}\n"
	.string "POKéMON with you!\p"
	.string "At that pace, your POKéDEX should\n"
	.string "be full in no time at all!$"

JubilifeCity_Text_AssistantSaferIfYouCatchMore:
	.string "…Um…\p"
	.string "I think you'd feel a lot safer if you\n"
	.string "were to catch some more.\p"
	.string "You know, like when you need to\n"
	.string "travel to faraway places.$"

JubilifeCity_Text_AssistantFollowMe:
	.string "Oh, I know! Don't you think it's\n"
	.string "better to know more about POKéMON?\p"
	.string "Sure it is!\n"
	.string "I know just the place.\p"
	.string "Come on!\n"
	.string "Don't be shy. Follow me!$"

JubilifeCity_Text_AssistantWhatIsHeDoing:
	.string "ASSISTANT: That man…\n"
	.string "What is he doing?$"

JubilifeCity_Text_AssistantUm:
	.string "ASSISTANT: Um…$"

JubilifeCity_Text_LookerWhaaat:
	.string "WHAAAT?!$"

JubilifeCity_Text_LookerHowDidYouUnmaskMe:
	.string "MYSTERIOUS MAN: …How did you know?\p"
	.string "How did you unmask me as a member\n"
	.string "of the INTERNATIONAL POLICE?!$"

JubilifeCity_Text_AssistantJustMakingConversation:
	.string "ASSISTANT: Huh? What?\n"
	.string "I beg your pardon?\p"
	.string "I was just making conversation…$"

JubilifeCity_Text_LookerIKnowBetter:
	.string "SHADY MAN: …Heh. You claim you were\n"
	.string "only making conversation, do you?\p"
	.string "But I know better than to believe\n"
	.string "that. No, no, no.\p"
	.string "You saw right away that I was\n"
	.string "someone extraordinary.\p"
	.string "That is why you spoke to me, is it\n"
	.string "not? Your power of observation is\l"
	.string "fearsome!\p"
	.string "Quite admirable, you are!$"

JubilifeCity_Text_LookerIsSayingFamiliar:
	.string "Now that my cover has been blown,\n"
	.string "let me introduce myself.\p"
	.string "I am a globe-trotting elite of the\n"
	.string "INTERNATIONAL POLICE.\p"
	.string "My name… Ah, no, I shall inform you\n"
	.string "only of my code name.\p"
	.string "My code name, it is LOOKER.\n"
	.string "It is what they all call me.\p"
	.string "Incidentally, is the saying, “Don't\n"
	.string "be a thief!” familiar to you?$"

JubilifeCity_Text_LookerTakingFromOthersIsWrong:
	.string "LOOKER: Yes, that is correct.\n"
	.string "Taking what belongs to others\l"
	.string "is wrong.$"

JubilifeCity_Text_LookerYouClaimToNotKnowIt:
	.string "LOOKER: No? You claim to not know\n"
	.string "it? How could that be?\p"
	.string "Perhaps your mama has said it, but\n"
	.string "you have forgotten it, like usual?\p"
	.string "Now listen, my friends. It is wrong\n"
	.string "to take what belongs to others.$"

JubilifeCity_Text_LookerOnTheLookout:
	.string "Unfortunately, there are apparently\n"
	.string "those who do not heed those words.\p"
	.string "In SINNOH, in fact, there are\n"
	.string "criminals stealing the POKéMON\l"
	.string "of others.\p"
	.string "I have, therefore, been on the\n"
	.string "lookout for characters arousing\l"
	.string "my suspicion.$"

JubilifeCity_Text_LookerInformMeOfAnyHappenings:
	.string "LOOKER: Also, I have a request.\p"
	.string "If you were to see me again, I ask\n"
	.string "that you not talk to me, for I am\l"
	.string "on duty.\p"
	.string "…Actually, yes, yes, you may speak\n"
	.string "to me. You must.\p"
	.string "Not because I am lonely, no, no!\n"
	.string "You must inform me of bad guys!\p"
	.string "You must inform me of any\n"
	.string "happenings!$"

JubilifeCity_Text_AssistantPoliceMustBeHard:
	.string "ASSISTANT: …Working for the\n"
	.string "INTERNATIONAL POLICE must be hard.$"

JubilifeCity_Text_AssistantHereIsTheTrainersSchool:
	.string "{PLAYER}, here it is.\n"
	.string "It's the TRAINERS' SCHOOL.\p"
	.string "The name says it all!$"

JubilifeCity_Text_AssistantByeNow:
	.string "You can learn all kinds of things\n"
	.string "about POKéMON in there.\p"
	.string "OK, bye now!$"

JubilifeCity_Text_LookerTellMeNoPoketch:
	.string "LOOKER: Tell me, have you not\n"
	.string "obtained a POKéTCH?\p"
	.string "I believe a POKéTCH is now free in\n"
	.string "exchange for some TICKETS?$"

JubilifeCity_Text_LookerItIsYouNoPoketch:
	.string "Ah, it is you! Have no fear.\n"
	.string "There are no shady characters about.\p"
	.string "Incidentally, have you not obtained\n"
	.string "a POKéTCH?\p"
	.string "I believe a POKéTCH is now free in\n"
	.string "exchange for some TICKETS?$"

@ ---- Team Galactic at the Route 204 exit, after the Coal Badge ----
@ Once the player has Oreburgh's Badge, two Team Galactic Grunts corner
@ Prof. Birch and his assistant at the north exit. Stepping onto row 7
@ (x 44-49) walks the player up to Birch, and the player and the assistant
@ take on the Grunts in a multi battle, as in Platinum. Platinum's road is
@ only 3 tiles wide here, so its trigger covers less of the row.
JubilifeCity_EventScript_TryStartTeamGalactic::
	call_if_eq VAR_JUBILIFE_GALACTIC_STATE, 0, JubilifeCity_EventScript_StartTeamGalactic
	return

JubilifeCity_EventScript_StartTeamGalactic::
	setvar VAR_JUBILIFE_GALACTIC_STATE, 1
	return

JubilifeCity_EventScript_ShowTeamGalactic::
	clearflag FLAG_HIDE_JUBILIFE_CITY_TEAM_GALACTIC
	clearflag FLAG_HIDE_JUBILIFE_CITY_ASSISTANT
	setobjectxyperm LOCALID_JUBILIFE_ASSISTANT, 48, 3
	setobjectmovementtype LOCALID_JUBILIFE_ASSISTANT, MOVEMENT_TYPE_FACE_LEFT
	return

JubilifeCity_EventScript_HideTeamGalactic::
	setflag FLAG_HIDE_JUBILIFE_CITY_TEAM_GALACTIC
	return

JubilifeCity_EventScript_TeamGalactic44::
	lockall
	setvar VAR_0x8008, 44
	goto JubilifeCity_EventScript_TeamGalactic
	end

JubilifeCity_EventScript_TeamGalactic45::
	lockall
	setvar VAR_0x8008, 45
	goto JubilifeCity_EventScript_TeamGalactic
	end

JubilifeCity_EventScript_TeamGalactic46::
	lockall
	setvar VAR_0x8008, 46
	goto JubilifeCity_EventScript_TeamGalactic
	end

JubilifeCity_EventScript_TeamGalactic47::
	lockall
	setvar VAR_0x8008, 47
	goto JubilifeCity_EventScript_TeamGalactic
	end

JubilifeCity_EventScript_TeamGalactic48::
	lockall
	setvar VAR_0x8008, 48
	goto JubilifeCity_EventScript_TeamGalactic
	end

JubilifeCity_EventScript_TeamGalactic49::
	lockall
	setvar VAR_0x8008, 49
	goto JubilifeCity_EventScript_TeamGalactic
	end

JubilifeCity_EventScript_TeamGalactic::
	applywaitmovement LOCALID_JUBILIFE_GRUNT_2, Common_Movement_WalkInPlaceFasterRight
	msgbox JubilifeCity_Text_HandOverResearchFindings, MSGBOX_DEFAULT
	closemessage
	applymovement LOCALID_JUBILIFE_BIRCH, JubilifeCity_Movement_BirchNoticePlayer
	call_if_eq VAR_0x8008, 44, JubilifeCity_EventScript_PlayerWalkToBirch44
	call_if_eq VAR_0x8008, 45, JubilifeCity_EventScript_PlayerWalkToBirch45
	call_if_eq VAR_0x8008, 46, JubilifeCity_EventScript_PlayerWalkToBirch46
	call_if_eq VAR_0x8008, 47, JubilifeCity_EventScript_PlayerWalkToBirch47
	call_if_eq VAR_0x8008, 48, JubilifeCity_EventScript_PlayerWalkToBirch48
	call_if_eq VAR_0x8008, 49, JubilifeCity_EventScript_PlayerWalkToBirch49
	waitmovement 0
	msgbox JubilifeCity_Text_ThatsOreburghsGymBadge, MSGBOX_DEFAULT
	applywaitmovement LOCALID_JUBILIFE_GRUNT_2, Common_Movement_WalkInPlaceFasterRight
	msgbox JubilifeCity_Text_MustYouBeSoDifficult, MSGBOX_DEFAULT
	applywaitmovement LOCALID_JUBILIFE_BIRCH, Common_Movement_WalkInPlaceFasterLeft
	msgbox JubilifeCity_Text_WhyMustYouBeSuchANuisance, MSGBOX_DEFAULT
	applywaitmovement LOCALID_JUBILIFE_BIRCH, Common_Movement_WalkInPlaceFasterDown
	msgbox JubilifeCity_Text_YouCallYourselvesAdults, MSGBOX_DEFAULT
	applymovement LOCALID_JUBILIFE_GRUNT_1, JubilifeCity_Movement_GruntStomp
	applymovement LOCALID_JUBILIFE_GRUNT_2, JubilifeCity_Movement_GruntStomp
	waitmovement 0
	msgbox JubilifeCity_Text_RegretInsultingTeamGalactic, MSGBOX_DEFAULT
	msgbox JubilifeCity_Text_GiveThugsALessonInCivility, MSGBOX_DEFAULT
	applywaitmovement LOCALID_JUBILIFE_ASSISTANT, Common_Movement_WalkInPlaceFasterDown
	checkplayergender
	call_if_eq VAR_RESULT, MALE, JubilifeCity_EventScript_DawnLetsBattleTogether
	call_if_eq VAR_RESULT, FEMALE, JubilifeCity_EventScript_LucasJoinMe
	closemessage
	applywaitmovement LOCALID_JUBILIFE_BIRCH, Common_Movement_WalkInPlaceFasterLeft
	applymovement LOCALID_JUBILIFE_ASSISTANT, JubilifeCity_Movement_AssistantWalkToGrunt
	applymovement LOCALID_PLAYER, JubilifeCity_Movement_PlayerWalkToGrunt
	waitmovement 0
	@ Emerald's multi battles give the player half the party, so the
	@ first three Pokémon that can battle go in beside the assistant's
	special SavePlayerParty
	special ChooseFirstHalfPartyForBattle
	special ReducePlayerPartyToSelectedMons
	frontier_set FRONTIER_DATA_SELECTED_MON_ORDER
	setvar VAR_0x8004, SPECIAL_BATTLE_JUBILIFE_GALACTIC
	setvar VAR_0x8005, 0
	special DoSpecialTrainerBattle
	frontier_saveparty
	special LoadPlayerParty
	goto_if_ne VAR_RESULT, B_OUTCOME_WON, JubilifeCity_EventScript_TeamGalacticWhiteOut
	applywaitmovement LOCALID_JUBILIFE_GRUNT_2, Common_Movement_WalkInPlaceFasterRight
	msgbox JubilifeCity_Text_WeWillRetreatForNow, MSGBOX_DEFAULT
	closemessage
	applymovement LOCALID_JUBILIFE_GRUNT_1, JubilifeCity_Movement_Grunt1Leave
	applymovement LOCALID_JUBILIFE_GRUNT_2, JubilifeCity_Movement_Grunt2Leave
	waitmovement 0
	removeobject LOCALID_JUBILIFE_GRUNT_1
	removeobject LOCALID_JUBILIFE_GRUNT_2
	applymovement LOCALID_JUBILIFE_BIRCH, Common_Movement_WalkInPlaceFasterDown
	applymovement LOCALID_JUBILIFE_ASSISTANT, JubilifeCity_Movement_AssistantWalkToPlayer2
	waitmovement 0
	msgbox JubilifeCity_Text_CalledThemselvesTeamGalactic, MSGBOX_DEFAULT
	applywaitmovement LOCALID_JUBILIFE_ASSISTANT, Common_Movement_WalkInPlaceFasterDown
	checkplayergender
	call_if_eq VAR_RESULT, MALE, JubilifeCity_EventScript_DawnProfessorStudiesEvolution
	call_if_eq VAR_RESULT, FEMALE, JubilifeCity_EventScript_LucasProfessorStudiesEvolution
	msgbox JubilifeCity_Text_CollectAllGymBadges, MSGBOX_DEFAULT
	closemessage
	applymovement LOCALID_JUBILIFE_ASSISTANT, JubilifeCity_Movement_AssistantLeaveSouth
	applymovement LOCALID_JUBILIFE_BIRCH, JubilifeCity_Movement_BirchLeave
	applymovement LOCALID_PLAYER, JubilifeCity_Movement_PlayerWatchBirchLeave
	waitmovement 0
	removeobject LOCALID_JUBILIFE_ASSISTANT
	removeobject LOCALID_JUBILIFE_BIRCH
	setflag FLAG_HIDE_JUBILIFE_CITY_ASSISTANT
	setflag FLAG_HIDE_JUBILIFE_CITY_TEAM_GALACTIC
	setvar VAR_JUBILIFE_GALACTIC_STATE, 2
	releaseall
	end

@ Losing leaves the Grunts where they are, to be battled again
JubilifeCity_EventScript_TeamGalacticWhiteOut::
	fadescreen FADE_TO_BLACK
	special SetCB2WhiteOut
	waitstate
	releaseall
	end

JubilifeCity_EventScript_DawnLetsBattleTogether::
	msgbox JubilifeCity_Text_DawnLetsBattleTogether, MSGBOX_DEFAULT
	return

JubilifeCity_EventScript_LucasJoinMe::
	msgbox JubilifeCity_Text_LucasJoinMe, MSGBOX_DEFAULT
	return

JubilifeCity_EventScript_DawnProfessorStudiesEvolution::
	msgbox JubilifeCity_Text_DawnProfessorStudiesEvolution, MSGBOX_DEFAULT
	return

JubilifeCity_EventScript_LucasProfessorStudiesEvolution::
	msgbox JubilifeCity_Text_LucasProfessorStudiesEvolution, MSGBOX_DEFAULT
	return

@ The Grunts' battle setups, read by DoSpecialTrainerBattle
JubilifeCity_EventScript_Grunt1Trainer::
	trainerbattle TRAINER_BATTLE_SET_TRAINER_A, TRAINER_GRUNT_UNUSED, LOCALID_NONE, JubilifeCity_Text_Grunt1Defeat, JubilifeCity_Text_Grunt1Defeat
	end

JubilifeCity_EventScript_Grunt2Trainer::
	trainerbattle TRAINER_BATTLE_SET_TRAINER_B, TRAINER_TERRY, LOCALID_NONE, JubilifeCity_Text_Grunt2Defeat, JubilifeCity_Text_Grunt2Defeat
	end

JubilifeCity_EventScript_PlayerWalkToBirch44::
	applymovement LOCALID_PLAYER, JubilifeCity_Movement_PlayerWalkToBirch44
	return

JubilifeCity_EventScript_PlayerWalkToBirch45::
	applymovement LOCALID_PLAYER, JubilifeCity_Movement_PlayerWalkToBirch45
	return

JubilifeCity_EventScript_PlayerWalkToBirch46::
	applymovement LOCALID_PLAYER, JubilifeCity_Movement_PlayerWalkToBirch46
	return

JubilifeCity_EventScript_PlayerWalkToBirch47::
	applymovement LOCALID_PLAYER, JubilifeCity_Movement_PlayerWalkToBirch47
	return

JubilifeCity_EventScript_PlayerWalkToBirch48::
	applymovement LOCALID_PLAYER, JubilifeCity_Movement_PlayerWalkToBirch48
	return

JubilifeCity_EventScript_PlayerWalkToBirch49::
	applymovement LOCALID_PLAYER, JubilifeCity_Movement_PlayerWalkToBirch49
	return

@ Up to 47, 5, below Birch and facing him

JubilifeCity_Movement_PlayerWalkToBirch44:
	delay_8
	delay_8
	walk_up
	walk_right
	walk_right
	walk_right
	walk_up
	step_end

JubilifeCity_Movement_PlayerWalkToBirch45:
	delay_8
	delay_8
	walk_up
	walk_right
	walk_right
	walk_up
	step_end

JubilifeCity_Movement_PlayerWalkToBirch46:
	delay_8
	delay_8
	walk_up
	walk_right
	walk_up
	step_end

JubilifeCity_Movement_PlayerWalkToBirch47:
	delay_8
	delay_8
	walk_up
	walk_up
	step_end

JubilifeCity_Movement_PlayerWalkToBirch48:
	delay_8
	delay_8
	walk_up
	walk_left
	walk_up
	step_end

JubilifeCity_Movement_PlayerWalkToBirch49:
	delay_8
	delay_8
	walk_up
	walk_left
	walk_left
	walk_up
	step_end

JubilifeCity_Movement_BirchNoticePlayer:
	walk_in_place_faster_down
	emote_exclamation_mark
	step_end

JubilifeCity_Movement_GruntStomp:
	walk_in_place_fast_right
	walk_in_place_fast_right
	step_end

@ To 47, 3, facing Grunt 1
JubilifeCity_Movement_AssistantWalkToGrunt:
	walk_left
	step_end

@ To 46, 5, facing Grunt 2
JubilifeCity_Movement_PlayerWalkToGrunt:
	walk_left
	walk_in_place_faster_up
	step_end

JubilifeCity_Movement_Grunt1Leave:
	walk_fast_up
	walk_fast_up
	walk_fast_up
	walk_fast_up
	walk_fast_up
	walk_fast_up
	walk_fast_up
	walk_fast_up
	walk_fast_up
	step_end

JubilifeCity_Movement_Grunt2Leave:
	walk_fast_up
	walk_fast_up
	walk_fast_up
	walk_fast_up
	walk_fast_up
	walk_fast_up
	walk_fast_up
	walk_fast_up
	walk_fast_up
	walk_fast_up
	step_end

@ To 46, 4, in front of the player
JubilifeCity_Movement_AssistantWalkToPlayer2:
	walk_left
	walk_down
	step_end

JubilifeCity_Movement_AssistantLeaveSouth:
	walk_right
	walk_down
	walk_down
	walk_down
	walk_down
	walk_down
	walk_down
	walk_down
	walk_down
	walk_down
	step_end

JubilifeCity_Movement_BirchLeave:
	walk_down
	walk_down
	walk_down
	walk_down
	walk_down
	walk_down
	walk_down
	walk_down
	walk_down
	step_end

JubilifeCity_Movement_PlayerWatchBirchLeave:
	delay_8
	walk_in_place_faster_right
	delay_8
	walk_in_place_faster_down
	step_end

JubilifeCity_Text_HandOverResearchFindings:
	.string "MYSTERY MAN: Now, now, now, now,\n"
	.string "now!\p"
	.string "PROF. BIRCH, you must comply.\n"
	.string "Hand over all your research\l"
	.string "findings. For free, naturally.\p"
	.string "Failure to comply will result in\n"
	.string "a painful time for your assistant.$"

JubilifeCity_Text_ThatsOreburghsGymBadge:
	.string "PROF. BIRCH: Ah, {PLAYER}.\n"
	.string "Well? How is the POKéDEX\l"
	.string "progressing?\p"
	.string "Hm! That's OREBURGH's\n"
	.string "GYM BADGE, I see.\p"
	.string "But hadn't I given you your first\n"
	.string "POKéMON only recently…?\p"
	.string "Perhaps being a TRAINER is like\n"
	.string "second nature to you.$"

JubilifeCity_Text_MustYouBeSoDifficult:
	.string "MYSTERY MAN: Oh, professor of\n"
	.string "POKéMON, must you be so difficult?\p"
	.string "We are speaking to you on\n"
	.string "business. Because this is work\l"
	.string "for us.\p"
	.string "What we're saying is--we demand\n"
	.string "you comply with our demands.$"

JubilifeCity_Text_WhyMustYouBeSuchANuisance:
	.string "PROF. BIRCH: Quiet, you lot!\n"
	.string "Why must you be such a nuisance?\p"
	.string "Let me list some lessons you still\n"
	.string "need to learn.\p"
	.string "No. 1: Don't loiter about for no\n"
	.string "good reason.\p"
	.string "No. 2: Don't interrupt others while\n"
	.string "they are attempting to converse.\p"
	.string "No. 3: If you don't get your way,\n"
	.string "don't raise your voice to be\l"
	.string "intimidating.\p"
	.string "No. 4: Don't think you've grown\n"
	.string "strong just because you're in\l"
	.string "a group.\p"
	.string "No. 5: What is with those\n"
	.string "outlandish outfits you have on?$"

JubilifeCity_Text_YouCallYourselvesAdults:
	.string "My goodness…\n"
	.string "You call yourselves adults?\p"
	.string "You kids, don't grow up to be like\n"
	.string "these sorry specimens.$"

JubilifeCity_Text_RegretInsultingTeamGalactic:
	.string "MYSTERY MAN: Eeeeeh!\n"
	.string "You had to make this personal!\p"
	.string "You have forced our hand into\n"
	.string "making a show of force!\p"
	.string "We will make you regret insulting\n"
	.string "TEAM GALACTIC!$"

JubilifeCity_Text_GiveThugsALessonInCivility:
	.string "PROF. BIRCH: You kids, give these\n"
	.string "thugs a lesson in civility, please.$"

JubilifeCity_Text_DawnLetsBattleTogether:
	.string "ASSISTANT: {PLAYER}!\n"
	.string "Let's battle together!$"

JubilifeCity_Text_LucasJoinMe:
	.string "ASSISTANT: {PLAYER}!\n"
	.string "Join me and battle these guys!$"

JubilifeCity_Text_Grunt1Defeat:
	.string "How is this madness possible?\p"
	.string "The two of us, losing\n"
	.string "to children?$"

JubilifeCity_Text_Grunt2Defeat:
	.string "This won't do…\n"
	.string "Time to retreat.\p"
	.string "This mission is a failure.$"

JubilifeCity_Text_WeWillRetreatForNow:
	.string "GRUNT: You leave us no option.\n"
	.string "We will retreat for now.\p"
	.string "We shall do so because TEAM\n"
	.string "GALACTIC is benevolent to all.$"

JubilifeCity_Text_CalledThemselvesTeamGalactic:
	.string "PROF. BIRCH: That lot…\n"
	.string "They called themselves\l"
	.string "TEAM GALACTIC.\p"
	.string "When POKéMON evolve, they seem to\n"
	.string "release some type of energy…\p"
	.string "However, I believe that it's a\n"
	.string "mystic power far beyond our\l"
	.string "control.\p"
	.string "But TEAM GALACTIC seems to be\n"
	.string "studying that power's potential.\p"
	.string "They want to know if it can be\n"
	.string "used as energy for something…$"

JubilifeCity_Text_DawnProfessorStudiesEvolution:
	.string "ASSISTANT: {PLAYER}, did you know?\p"
	.string "The professor studies the\n"
	.string "evolution of POKéMON, too.\p"
	.string "According to his research, 90% of\n"
	.string "all POKéMON are somehow tied to\l"
	.string "evolution!\p"
	.string "Well, maybe that's the reason\n"
	.string "those people tried to take the\l"
	.string "professor's research data by\l"
	.string "force.\p"
	.string "That's really unforgivable!$"

JubilifeCity_Text_LucasProfessorStudiesEvolution:
	.string "ASSISTANT: {PLAYER}, did you know\n"
	.string "about this?\p"
	.string "The professor studies the\n"
	.string "evolution of POKéMON, too.\p"
	.string "According to his research, 90% of\n"
	.string "all POKéMON are somehow tied to\l"
	.string "evolution!\p"
	.string "Maybe that's the reason those\n"
	.string "goons tried to take the\l"
	.string "professor's research data.\p"
	.string "They're not allowed to do that!$"

JubilifeCity_Text_CollectAllGymBadges:
	.string "PROF. BIRCH: It's all over, so you\n"
	.string "can relax now.\p"
	.string "Still, thanks to you two, nothing\n"
	.string "came of that situation.\l"
	.string "I appreciate that.\p"
	.string "Incidentally, {PLAYER}.\n"
	.string "Kudos to you for your battling\l"
	.string "skill.\p"
	.string "It got me thinking. Why don't you\n"
	.string "collect all the GYM BADGES\l"
	.string "of SINNOH?\p"
	.string "Doing so, you will be sure to\n"
	.string "encounter lots of POKéMON.\p"
	.string "That, of course, means your\n"
	.string "POKéDEX pages will continuously\l"
	.string "fill up.\p"
	.string "In other words, it will be of\n"
	.string "great help to my research.\p"
	.string "That said, I've given you your\n"
	.string "first POKéMON and POKéDEX as\l"
	.string "yours to keep.\p"
	.string "You're free to do with them as you\n"
	.string "wish. Take care now.$"
