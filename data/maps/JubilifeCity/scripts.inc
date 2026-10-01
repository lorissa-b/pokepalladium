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
