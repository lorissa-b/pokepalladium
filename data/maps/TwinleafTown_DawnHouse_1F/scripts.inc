@ The player's house, for both genders. The rival lives in the Lucas house (see TwinleafTown_LucasHouse_1F).
TwinleafTown_DawnHouse_1F_MapScripts::
	map_script MAP_SCRIPT_ON_LOAD, TwinleafTown_DawnHouse_1F_OnLoad
	map_script MAP_SCRIPT_ON_TRANSITION, TwinleafTown_DawnHouse_1F_OnTransition
	map_script MAP_SCRIPT_ON_FRAME_TABLE, TwinleafTown_DawnHouse_1F_OnFrame
	.byte 0

TwinleafTown_DawnHouse_1F_OnLoad:
	call_if_set FLAG_RECEIVED_RUNNING_SHOES, TwinleafTown_DawnHouse_1F_EventScript_ShowRunningShoesManual
	end

TwinleafTown_DawnHouse_1F_EventScript_ShowRunningShoesManual::
	setmetatile 6, 7, METATILE_BrendansMaysHouse_BookOnTable, TRUE
	return

TwinleafTown_DawnHouse_1F_OnTransition:
	call_if_eq VAR_TWINLEAF_TOWN_STATE, 3, TwinleafTown_DawnHouse_1F_EventScript_RivalMomVisiting
	call_if_eq VAR_TWINLEAF_INTRO_STATE, 8, TwinleafTown_DawnHouse_1F_EventScript_MoveMomNearStairs
	end

@ After getting the POKéDEX, the rival's mom is sitting at the table with Mom
TwinleafTown_DawnHouse_1F_EventScript_RivalMomVisiting::
	clearflag FLAG_HIDE_TWINLEAF_TOWN_DAWN_HOUSE_RIVAL_MOM
	setflag FLAG_HIDE_TWINLEAF_TOWN_LUCAS_HOUSE_RIVAL_MOM
	return

TwinleafTown_DawnHouse_1F_EventScript_MoveMomNearStairs::
	setobjectxyperm LOCALID_PLAYERS_HOUSE_1F_MOM, 2, 5
	setobjectmovementtype LOCALID_PLAYERS_HOUSE_1F_MOM, MOVEMENT_TYPE_FACE_UP
	return

TwinleafTown_DawnHouse_1F_OnFrame:
	map_script_2 VAR_TWINLEAF_TOWN_STATE, 3, TwinleafTown_DawnHouse_1F_EventScript_HomeWithPokedex
	map_script_2 VAR_TWINLEAF_INTRO_STATE, 8, TwinleafTown_DawnHouse_1F_EventScript_RivalWasLookingForYou
	map_script_2 VAR_TWINLEAF_HOUSES_STATE_MAY, 3, PlayersHouse_1F_EventScript_GetSSTicketAndSeeLatiTV
	.2byte 0

@ Coming home with the POKéDEX. Mom (8, 6) and the rival's mom (8, 7) are
@ sitting on the far side of the table; the player sits down across from them
@ at 5, 7. The rival's mom hands over a TOWN MAP and Mom gives RUNNING SHOES.
TwinleafTown_DawnHouse_1F_EventScript_HomeWithPokedex::
	lockall
	playse SE_PIN
	applywaitmovement LOCALID_RIVALS_HOUSE_1F_MOM, Common_Movement_ExclamationMark
	applywaitmovement LOCALID_RIVALS_HOUSE_1F_MOM, Common_Movement_Delay48
	applywaitmovement LOCALID_PLAYER, TwinleafTown_DawnHouse_1F_Movement_PlayerSitAtTable
	msgbox RivalsHouse_1F_Text_RivalRanAheadBringTownMap, MSGBOX_DEFAULT
	giveitem ITEM_TOWN_MAP
	msgbox RivalsHouse_1F_Text_KeepTheExtraTownMap, MSGBOX_DEFAULT
	closemessage
	applywaitmovement LOCALID_PLAYERS_HOUSE_1F_MOM, Common_Movement_ExclamationMark
	msgbox PlayersHouse_1F_Text_MomWearTheseRunningShoes, MSGBOX_DEFAULT
	playfanfare MUS_OBTAIN_ITEM
	message TwinleafTown_Text_SwitchShoesWithRunningShoes
	waitfanfare
	setflag FLAG_RECEIVED_RUNNING_SHOES
	setflag FLAG_SYS_B_DASH
	msgbox TwinleafTown_Text_ExplainRunningShoes, MSGBOX_DEFAULT
	msgbox TwinleafTown_Text_ComeHomeIfAnythingHappens, MSGBOX_DEFAULT
	closemessage
	@ She stays at the table for this visit and goes home afterwards
	setflag FLAG_HIDE_TWINLEAF_TOWN_DAWN_HOUSE_RIVAL_MOM
	clearflag FLAG_HIDE_TWINLEAF_TOWN_LUCAS_HOUSE_RIVAL_MOM
	setvar VAR_TWINLEAF_TOWN_STATE, 4
	releaseall
	end

@ From the door mat (1, 8) to the chair across the table (5, 7)
TwinleafTown_DawnHouse_1F_Movement_PlayerSitAtTable:
	walk_up
	walk_right
	walk_right
	walk_right
	walk_right
	walk_in_place_faster_right
	step_end

@ First time coming downstairs after the game starts
TwinleafTown_DawnHouse_1F_EventScript_RivalWasLookingForYou::
	lockall
	playse SE_PIN
	applywaitmovement LOCALID_PLAYERS_HOUSE_1F_MOM, Common_Movement_ExclamationMark
	applywaitmovement LOCALID_PLAYERS_HOUSE_1F_MOM, Common_Movement_Delay48
	applywaitmovement LOCALID_PLAYER, Common_Movement_WalkInPlaceFasterDown
	applywaitmovement LOCALID_PLAYERS_HOUSE_1F_MOM, TwinleafTown_DawnHouse_1F_Movement_MomApproachStairs
	msgbox PlayersHouse_1F_Text_RivalWasLookingForYou, MSGBOX_DEFAULT
	closemessage
	applywaitmovement LOCALID_PLAYERS_HOUSE_1F_MOM, TwinleafTown_DawnHouse_1F_Movement_MomReturnFromStairs
	setvar VAR_TWINLEAF_INTRO_STATE, 9
	releaseall
	end

@ The player steps off the stairs onto 2, 3, so Mom stops just below them
TwinleafTown_DawnHouse_1F_Movement_MomApproachStairs:
	walk_up
	step_end

TwinleafTown_DawnHouse_1F_Movement_MomReturnFromStairs:
	walk_down
	walk_in_place_faster_up
	step_end

RivalsHouse_1F_Text_RivalRanAheadBringTownMap:
	.string "Oh, {PLAYER}{KUN}! Welcome home!\p"
	.string "{RIVAL} was just here, but ran on\n"
	.string "ahead again. As usual!\p"
	.string "And guess what got left behind?\n"
	.string "This TOWN MAP.\p"
	.string "Could you please bring it to {RIVAL}\n"
	.string "when you catch up?$"

RivalsHouse_1F_Text_KeepTheExtraTownMap:
	.string "There was an extra one here, too.\n"
	.string "That one's yours, so keep it!$"

PlayersHouse_1F_Text_MomWearTheseRunningShoes:
	.string "MOM: {PLAYER}! You got a POKéDEX\n"
	.string "from PROF. BIRCH? How wonderful!\p"
	.string "If you're going out on an adventure,\n"
	.string "you'll need these, honey.\p"
	.string "Here, wear these RUNNING SHOES.\n"
	.string "They'll put a zip in your step!$"

PlayersHouse_1F_Text_RivalWasLookingForYou:
	.string "MOM: Oh, {PLAYER}! You're finally up!\p"
	.string "{RIVAL} came by looking for you\n"
	.string "a little while ago.\p"
	.string "{RIVAL} seemed awfully excited\n"
	.string "about something!\p"
	.string "Why don't you go over to {RIVAL}'s\n"
	.string "house and see what's going on?$"

PlayersHouse_1F_Text_GoToRivalsHouse:
	.string "MOM: {RIVAL} was looking for you,\n"
	.string "{PLAYER}.\p"
	.string "{RIVAL}'s house is just up the road.\n"
	.string "Don't keep your friend waiting!$"

PlayersHouse_1F_Text_DidYouFindRival:
	.string "MOM: Did you find {RIVAL}?\p"
	.string "…Off to ROUTE 201, you say?\n"
	.string "Well, be careful out there!$"

PlayersHouse_1F_Text_YouShouldRestABit:
	.string "MOM: How are you doing, {PLAYER}?\n"
	.string "You look a little tired.\p"
	.string "I think you should rest a bit.$"

PlayersHouse_1F_Text_TakeCareHoney:
	.string "MOM: Take care, honey!$"

PlayersHouse_1F_Text_GotFiveBadgesHeresSomethingFromMom:
	.string "MOM: Oh my! You've collected five\n"
	.string "GYM BADGES already?\p"
	.string "Then here's something from your MOM!$"

PlayersHouse_1F_Text_DontPushYourselfTooHard:
	.string "Don't push yourself too hard, dear.\n"
	.string "You can always come home.\p"
	.string "Go for it, honey!$"

PlayersHouse_1F_Text_IsThatAPokenav:
	.string "MOM: What is that, honey? A POKéTCH?\n"
	.string "My, that looks handy!\p"
	.string "Well, honey, how about registering\n"
	.string "your mom?\p"
	.string "… … …$"

PlayersHouse_1F_Text_RegisteredMom:
	.string "Registered MOM\n"
	.string "in the POKéTCH.$"

PlayersHouse_1F_Text_TheresAMovieOnTV:
	.string "There is a movie on TV.\p"
	.string "Two men are dancing on a big piano\n"
	.string "keyboard.\p"
	.string "Better get going!$"

PlayersHouse_1F_Text_RunningShoesManual:
	.string "It's the instruction booklet for the\n"
	.string "RUNNING SHOES.\p"
	.string "“Press the B Button to run while\n"
	.string "wearing your RUNNING SHOES.\p"
	.string "“Lace up your RUNNING SHOES and hit\n"
	.string "the road running!”$"

PlayersHouse_1F_Text_TicketFromBrineyCameForYou:
	.string "DAD: Hm?\p"
	.string "Hey, it's {PLAYER}!\p"
	.string "It's been a while since I saw you,\n"
	.string "but you look…stronger, somehow.\p"
	.string "That's the impression I get.\n"
	.string "But your old man hasn't given up yet!\p"
	.string "Oh, yes, I have something for you.\n"
	.string "This came to you from someone named\l"
	.string "MR. BRINEY.$"

PlayersHouse_1F_Text_PortsInSlateportLilycove:
	.string "DAD: Hm, a TICKET for a ferry?\p"
	.string "If I recall, there are ferry ports in\n"
	.string "SLATEPORT and LILYCOVE.$"

PlayersHouse_1F_Text_BetterGetBackToGym:
	.string "I'd better get back to PETALBURG GYM.\p"
	.string "MOM, thanks for looking after the house\n"
	.string "while I'm away.$"

PlayersHouse_1F_Text_DadShouldStayLonger:
	.string "MOM: That DAD of yours…\p"
	.string "He comes home for the first time in a\n"
	.string "while, but all he talks about is POKéMON.\p"
	.string "He should relax and stay a little longer.$"

PlayersHouse_1F_Text_IsThatABreakingStory:
	.string "MOM: Is that a breaking news story?$"

PlayersHouse_1F_Text_LatiEmergencyNewsFlash:
	.string "We bring you this emergency\n"
	.string "news flash!\p"
	.string "In various HOENN locales, there have\n"
	.string "been reports of a BZZT…colored\l"
	.string "POKéMON in flight.\p"
	.string "The identity of this POKéMON is\n"
	.string "currently unknown.\p"
	.string "We now return you to the regular\n"
	.string "movie program.$"

PlayersHouse_1F_Text_WhatColorDidTheySay:
	.string "MOM: {PLAYER}, did you catch that?\p"
	.string "What color did the announcer say\n"
	.string "that POKéMON was?$"

PlayersHouse_1F_Text_StillUnknownPokemon:
	.string "MOM: Well, isn't that something!\n"
	.string "There are still unknown POKéMON.$"
