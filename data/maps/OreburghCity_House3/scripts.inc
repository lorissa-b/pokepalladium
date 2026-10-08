OreburghCity_House3_MapScripts::
	map_script MAP_SCRIPT_ON_FRAME_TABLE, OreburghCity_House3_OnFrame
	map_script MAP_SCRIPT_ON_WARP_INTO_MAP_TABLE, OreburghCity_House3_OnWarp
	.byte 0

OreburghCity_House3_OnWarp:
	map_script_2 VAR_OREBURGH_CITY_STATE, 4, OreburghCity_House3_EventScript_PlayerWallysDadFaceEachOther
	.2byte 0

OreburghCity_House3_EventScript_PlayerWallysDadFaceEachOther::
	turnobject LOCALID_PLAYER, DIR_EAST
	turnobject LOCALID_WALLYS_HOUSE_WALLYS_DAD, DIR_WEST
	end

OreburghCity_House3_OnFrame:
	map_script_2 VAR_OREBURGH_CITY_STATE, 4, OreburghCity_House3_EventScript_GiveHMSurf
	.2byte 0

OreburghCity_House3_EventScript_GiveHMSurf::
	lockall
	msgbox OreburghCity_House3_Text_PleaseExcuseUs, MSGBOX_DEFAULT
	giveitem ITEM_HM_SURF
	setflag FLAG_RECEIVED_HM_SURF
	msgbox OreburghCity_House3_Text_SurfGoAllSortsOfPlaces, MSGBOX_DEFAULT
	setvar VAR_OREBURGH_CITY_STATE, 5
	releaseall
	end

OreburghCity_House3_EventScript_WallysDad::
	lock
	faceplayer
	goto_if_set FLAG_DEFEATED_WALLY_VICTORY_ROAD, OreburghCity_House3_EventScript_DefeatedWallyInVictoryRoad
	goto_if_set FLAG_RECEIVED_HM_SURF, OreburghCity_House3_EventScript_ReceievedHMSurf
	goto_if_set FLAG_THANKED_FOR_PLAYING_WITH_WALLY, OreburghCity_House3_EventScript_PlayedWithWally
	msgbox OreburghCity_House3_Text_ThanksForPlayingWithWally, MSGBOX_DEFAULT
	setflag FLAG_THANKED_FOR_PLAYING_WITH_WALLY
	release
	end

OreburghCity_House3_EventScript_ReceievedHMSurf::
	msgbox OreburghCity_House3_Text_WallyIsComingHomeSoon, MSGBOX_DEFAULT
	release
	end

OreburghCity_House3_EventScript_DefeatedWallyInVictoryRoad::
	msgbox OreburghCity_House3_Text_YouMetWallyInEverGrandeCity, MSGBOX_DEFAULT
	release
	end

OreburghCity_House3_EventScript_PlayedWithWally::
	msgbox OreburghCity_House3_Text_WonderHowWallyIsDoing, MSGBOX_DEFAULT
	release
	end

OreburghCity_House3_EventScript_WallysMom::
	lock
	faceplayer
	goto_if_set FLAG_RECEIVED_HM_SURF, OreburghCity_House3_EventScript_ReceivedHMSurf
	msgbox OreburghCity_House3_Text_WallyWasReallyHappy, MSGBOX_DEFAULT
	release
	end

OreburghCity_House3_EventScript_ReceivedHMSurf::
	msgbox OreburghCity_House3_Text_WallyLeftWithoutTelling, MSGBOX_DEFAULT
	release
	end

OreburghCity_House3_Text_ThanksForPlayingWithWally:
	.string "You're…\n"
	.string "Ah, you must be {PLAYER}{KUN}, right?\p"
	.string "Thank you for playing with WALLY a\n"
	.string "little while ago.\p"
	.string "He's been frail and sickly ever\n"
	.string "since he was a baby.\p"
	.string "We've sent him to stay with my relatives\n"
	.string "in VERDANTURF TOWN for a while.\p"
	.string "The air is a lot cleaner there\n"
	.string "than it is here.\p"
	.string "What's that? Where's WALLY?\n"
	.string "He's already left, our WALLY.\p"
	.string "I wonder where he could have\n"
	.string "gotten by now?$"

OreburghCity_House3_Text_WonderHowWallyIsDoing:
	.string "I wonder how our WALLY is doing?$"

OreburghCity_House3_Text_PleaseExcuseUs:
	.string "{PLAYER}{KUN}! Please excuse us for\n"
	.string "dragging you here this way.\p"
	.string "But our WALLY's become very healthy\n"
	.string "since he went to VERDANTURF TOWN.\p"
	.string "We owe it all to you!\p"
	.string "When WALLY left town, you helped\n"
	.string "him catch a POKéMON, right?\p"
	.string "I think that made WALLY really\n"
	.string "happy.\p"
	.string "Actually, not just WALLY.\n"
	.string "It made me, his father, happy too.\p"
	.string "Happy that he's gained such a great\n"
	.string "friend as you.\p"
	.string "This isn't a bribe or anything, but\n"
	.string "I'd really like you to have this.$"

OreburghCity_House3_Text_SurfGoAllSortsOfPlaces:
	.string "If your POKéMON can SURF, you'll be\n"
	.string "able to go to all sorts of places.$"

OreburghCity_House3_Text_WallyIsComingHomeSoon:
	.string "WALLY's coming home soon.\n"
	.string "I'm looking forward to that.$"

OreburghCity_House3_Text_YouMetWallyInEverGrandeCity:
	.string "Oh? You met WALLY in\n"
	.string "EVER GRANDE CITY?\p"
	.string "Oh, {PLAYER}{KUN}, don't be silly.\p"
	.string "He may have gotten healthy, but he\n"
	.string "can't go somewhere far away like\l"
	.string "that all by himself.$"

OreburghCity_House3_Text_WallyWasReallyHappy:
	.string "WALLY was really happy when he told\n"
	.string "us that he caught a POKéMON.\p"
	.string "It's been ages since I've seen him\n"
	.string "smile like that.$"

OreburghCity_House3_Text_WallyLeftWithoutTelling:
	.string "I want you to keep this a secret\n"
	.string "from my husband…\p"
	.string "But our WALLY left VERDANTURF TOWN\n"
	.string "without telling anyone.\p"
	.string "You know, WALLY is frail, but\n"
	.string "he's surprisingly strong-willed.\p"
	.string "I'm sure that he'll come back safe\n"
	.string "and sound one day!$"

