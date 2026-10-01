JubilifeCity_PoketchCompany_1F_MapScripts::
	.byte 0

JubilifeCity_PoketchCompany_1F_EventScript_Receptionist::
	msgbox JubilifeCity_PoketchCompany_1F_Text_WelcomeToPoketchCompany, MSGBOX_NPC
	end

JubilifeCity_PoketchCompany_1F_EventScript_Man::
	msgbox JubilifeCity_PoketchCompany_1F_Text_WatchingTheTV, MSGBOX_NPC
	end

JubilifeCity_PoketchCompany_1F_EventScript_Woman::
	msgbox JubilifeCity_PoketchCompany_1F_Text_NewAppsEveryMonth, MSGBOX_NPC
	end

JubilifeCity_PoketchCompany_1F_EventScript_Boy::
	msgbox JubilifeCity_PoketchCompany_1F_Text_WantAPoketch, MSGBOX_NPC
	end

JubilifeCity_PoketchCompany_1F_Text_WelcomeToPoketchCompany:
	.string "Welcome to the POKéTCH COMPANY!\p"
	.string "We make the POKéTCH, the watch\n"
	.string "every TRAINER in SINNOH wants!$"

JubilifeCity_PoketchCompany_1F_Text_WatchingTheTV:
	.string "They're showing an ad for the new\n"
	.string "POKéTCH apps on TV right now.$"

JubilifeCity_PoketchCompany_1F_Text_NewAppsEveryMonth:
	.string "The POKéTCH gets new apps all the\n"
	.string "time. I can't keep up!$"

JubilifeCity_PoketchCompany_1F_Text_WantAPoketch:
	.string "I want a POKéTCH of my own!\n"
	.string "I wonder how I could get one…$"
