@ Oreburgh City, east of Oreburgh Gate, laid out as in Platinum. Platinum's
@ arrival scene (the Youngster leading the player to the Gym, and Barry) and
@ its Machops and vents aren't here yet.
OreburghCity_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, OreburghCity_OnTransition
	.byte 0

OreburghCity_OnTransition:
	setflag FLAG_VISITED_OREBURGH_CITY
	end

OreburghCity_EventScript_Hiker::
	msgbox OreburghCity_Text_UpAheadTheresAHugeMountain, MSGBOX_NPC
	end

OreburghCity_EventScript_Worker1::
	msgbox OreburghCity_Text_TheseVentsExchangeAirDownBelow, MSGBOX_NPC
	end

OreburghCity_EventScript_Worker2::
	lock
	faceplayer
	goto_if_set FLAG_RECEIVED_OREBURGH_CITY_SUPER_POTION, OreburghCity_EventScript_PutsASmileOnMyFace
	msgbox OreburghCity_Text_HereTakeOneOfThese, MSGBOX_DEFAULT
	giveitem ITEM_SUPER_POTION
	goto_if_eq VAR_RESULT, FALSE, Common_EventScript_ShowBagIsFull
	setflag FLAG_RECEIVED_OREBURGH_CITY_SUPER_POTION
	goto OreburghCity_EventScript_PutsASmileOnMyFace

OreburghCity_EventScript_PutsASmileOnMyFace::
	msgbox OreburghCity_Text_PutsASmileOnMyFace, MSGBOX_DEFAULT
	release
	end

OreburghCity_EventScript_Worker3::
	msgbox OreburghCity_Text_ThisPileIsCalledASlagHeap, MSGBOX_NPC
	end

OreburghCity_EventScript_Worker4::
	msgbox OreburghCity_Text_ChopChopAwayOnRocks, MSGBOX_NPC
	end

OreburghCity_EventScript_BattleGirl1::
	msgbox OreburghCity_Text_TMsSingleUseHMsOverAndOver, MSGBOX_NPC
	end

OreburghCity_EventScript_BattleGirl2::
	msgbox OreburghCity_Text_ThatsWhatFossilsAre, MSGBOX_NPC
	end

OreburghCity_EventScript_SchoolKidF::
	msgbox OreburghCity_Text_GymLeaderWentOffToWork, MSGBOX_NPC
	end

OreburghCity_EventScript_Guitarist::
	msgbox OreburghCity_Text_HaveYouSeenTheVents, MSGBOX_NPC
	end

OreburghCity_EventScript_PokefanF::
	msgbox OreburghCity_Text_EveryonesProudOfTheMine, MSGBOX_NPC
	end

OreburghCity_EventScript_Camper::
	msgbox OreburghCity_Text_ImTheCenterOfAttention, MSGBOX_NPC
	end

OreburghCity_EventScript_CitySign::
	msgbox OreburghCity_Text_CitySign, MSGBOX_SIGN
	end

OreburghCity_EventScript_GymSign::
	msgbox OreburghCity_Text_GymSign, MSGBOX_SIGN
	end

OreburghCity_EventScript_MiningMuseumSign::
	msgbox OreburghCity_Text_MiningMuseumSign, MSGBOX_SIGN
	end

OreburghCity_EventScript_MineSign::
	msgbox OreburghCity_Text_MineSign, MSGBOX_SIGN
	end

OreburghCity_Text_UpAheadTheresAHugeMountain:
	.string "Up ahead there's a huge mountain\n"
	.string "that towers above everything.\p"
	.string "It divides SINNOH into two distinct\n"
	.string "halves.$"

OreburghCity_Text_TheseVentsExchangeAirDownBelow:
	.string "These vents exchange the hot air\n"
	.string "down below with the fresh air outside.\p"
	.string "Looking down them gives me the urge\n"
	.string "to start yelling into 'em! Hello!$"

OreburghCity_Text_HereTakeOneOfThese:
	.string "There's no question. This is hard,\n"
	.string "hard work.\p"
	.string "But working alongside POKéMON…\n"
	.string "It gives me a boost somehow.\p"
	.string "Here, take one of these.$"

OreburghCity_Text_PutsASmileOnMyFace:
	.string "Feeling hale and hearty puts a smile\n"
	.string "on my face. And that makes me happy.$"

OreburghCity_Text_ThisPileIsCalledASlagHeap:
	.string "This pile of dirty sand is called\n"
	.string "a slag heap.\p"
	.string "It's the waste from sorting coal.$"

OreburghCity_Text_ChopChopAwayOnRocks:
	.string "Chop, chop, MACHOP!\n"
	.string "Chop, chop away on rocks!$"

OreburghCity_Text_TMsSingleUseHMsOverAndOver:
	.string "TMs are single use only, but HMs\n"
	.string "can be used over and over.\p"
	.string "It's tricky trying to decide which\n"
	.string "POKéMON should learn what moves.$"

OreburghCity_Text_ThatsWhatFossilsAre:
	.string "Natural time capsules buried many,\n"
	.string "many years ago…\p"
	.string "That's what FOSSILS are.$"

OreburghCity_Text_GymLeaderWentOffToWork:
	.string "The GYM LEADER went off to work in\n"
	.string "the mine.$"

OreburghCity_Text_HaveYouSeenTheVents:
	.string "Have you seen the vents?\n"
	.string "They release steam from down below.$"

OreburghCity_Text_EveryonesProudOfTheMine:
	.string "Everyone's proud of the mine.\p"
	.string "They're digging for coal under the\n"
	.string "sea as we speak…\p"
	.string "It's amazing how they do it!$"

OreburghCity_Text_ImTheCenterOfAttention:
	.string "When I run around with my RUNNING\n"
	.string "SHOES on, I'm the center of attention!\p"
	.string "…At least that's what it feels like.\n"
	.string "There's always someone looking at me.$"

OreburghCity_Text_CitySign:
	.string "OREBURGH CITY\n"
	.string "“The City of Energy”$"

OreburghCity_Text_GymSign:
	.string "OREBURGH CITY POKéMON GYM\n"
	.string "LEADER: ROARK\l"
	.string "“Call me ROARK the Rock!”$"

OreburghCity_Text_MiningMuseumSign:
	.string "OREBURGH MINING MUSEUM\n"
	.string "“Coal Mining and You!”$"

OreburghCity_Text_MineSign:
	.string "OREBURGH MINE\n"
	.string "Beware of Busy POKéMON!$"
