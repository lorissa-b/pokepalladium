@ Eterna City, east of Route 205 North, laid out as in Platinum. The west
@ edge joins Route 205 North; Route 211 (east) and the Cycling Road gate on
@ Route 206 (south door, 16-17, 57) don't exist yet.
@
@ The three Team Galactic Grunts stand around until
@ FLAG_HIDE_ETERNA_CITY_GALACTIC_GRUNTS is set, which in Platinum happens
@ when the player clears the Galactic Eterna Building. The townspeople who
@ talk about Team Galactic change their lines on the same flag.
@
@ Once Jupiter is beaten in the Galactic Eterna Building
@ (VAR_ETERNA_CITY_STATE 3), Cynthia walks up to the player crossing x=20 by
@ the Cycle Shop (rows 29-33) and offers a Togepi Egg. Turned down, or with
@ no room in the party, she waits at the spot (state 4), and won't let the
@ player past x=21 until they take it (state 5).
@
@ Not here yet: Barry and Cyrus at the statue, Cynthia giving HM01,
@ Gardenia outside the Gym and the exits blocked until the Bicycle.
EternaCity_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, EternaCity_OnTransition
	.byte 0

EternaCity_OnTransition:
	setflag FLAG_VISITED_ETERNA_CITY
	end

EternaCity_EventScript_GruntStatue::
	msgbox EternaCity_Text_StayAwayFromStatue, MSGBOX_NPC
	end

EternaCity_EventScript_GruntWest::
	msgbox EternaCity_Text_IllLetYouKeepPokemon, MSGBOX_NPC
	end

EternaCity_EventScript_GruntCenter::
	msgbox EternaCity_Text_HaveYouSeenOurBuilding, MSGBOX_NPC
	end

EternaCity_EventScript_PokefanM::
	msgbox EternaCity_Text_GetYourselfABicycle, MSGBOX_NPC
	end

EternaCity_EventScript_PokefanF::
	lock
	faceplayer
	goto_if_set FLAG_HIDE_ETERNA_CITY_GALACTIC_GRUNTS, EternaCity_EventScript_WhatWillHappenToGalacticBuilding
	msgbox EternaCity_Text_AtmosphereHasGottenCreepy, MSGBOX_DEFAULT
	release
	end

EternaCity_EventScript_WhatWillHappenToGalacticBuilding::
	msgbox EternaCity_Text_WhatWillHappenToGalacticBuilding, MSGBOX_DEFAULT
	release
	end

EternaCity_EventScript_BugCatcherCycleShop::
	lock
	faceplayer
	goto_if_set FLAG_HIDE_ETERNA_CITY_GALACTIC_GRUNTS, EternaCity_EventScript_DoYouUseKickstand
	msgbox EternaCity_Text_GalacticTookFavoritePokemon, MSGBOX_DEFAULT
	release
	end

EternaCity_EventScript_DoYouUseKickstand::
	msgbox EternaCity_Text_DoYouUseKickstand, MSGBOX_DEFAULT
	release
	end

EternaCity_EventScript_Beauty::
	msgbox EternaCity_Text_TheySayThisIsAnOldCity, MSGBOX_NPC
	end

EternaCity_EventScript_ExpertM::
	msgbox EternaCity_Text_EternaCitysNotLikeBefore, MSGBOX_NPC
	end

EternaCity_EventScript_NinjaBoy::
	msgbox EternaCity_Text_AwesomePokemonFromLongAgo, MSGBOX_NPC
	end

@ In Platinum she sends players without an Explorer Kit to the
@ Underground Man. The Underground isn't in, so she always says this.
EternaCity_EventScript_PokefanFSouth::
	msgbox EternaCity_Text_YouDontHaveExplorerKit, MSGBOX_NPC
	end

EternaCity_EventScript_BugCatcherWest::
	lock
	faceplayer
	goto_if_unset FLAG_RECEIVED_BIKE, EternaCity_EventScript_FeelsGreatRidingABike
	msgbox EternaCity_Text_YouGotABicycle, MSGBOX_DEFAULT
	release
	end

EternaCity_EventScript_FeelsGreatRidingABike::
	msgbox EternaCity_Text_FeelsGreatRidingABike, MSGBOX_DEFAULT
	release
	end

EternaCity_EventScript_Statue::
	msgbox EternaCity_Text_ItsEternaCitysPokemonStatue, MSGBOX_SIGN
	end

EternaCity_EventScript_CitySign::
	msgbox EternaCity_Text_CitySign, MSGBOX_SIGN
	end

EternaCity_EventScript_GymSign::
	msgbox EternaCity_Text_GymSign, MSGBOX_SIGN
	end

EternaCity_EventScript_CycleShopSign::
	msgbox EternaCity_Text_CycleShopSign, MSGBOX_SIGN
	end

EternaCity_EventScript_GalacticBuildingSign::
	msgbox EternaCity_Text_GalacticBuildingSign, MSGBOX_SIGN
	end

EternaCity_EventScript_UndergroundMansHouseSign::
	msgbox EternaCity_Text_UndergroundMansHouseSign, MSGBOX_SIGN
	end

EternaCity_EventScript_CondominiumsSign::
	msgbox EternaCity_Text_CondominiumsSign, MSGBOX_SIGN
	end

@ Mauville's Wally call, kept because field_control_avatar.c still runs it.
EternaCity_EventScript_RegisterWallyCall::
	lockall
	pokenavcall EternaCity_Text_WallyPokenavCall
	waitmessage
	delay 30
	playfanfare MUS_REGISTER_MATCH_CALL
	msgbox EternaCity_Text_RegisteredWally, MSGBOX_DEFAULT
	waitfanfare
	closemessage
	delay 30
	setflag FLAG_ENABLE_WALLY_MATCH_CALL
	clearflag FLAG_ENABLE_FIRST_WALLY_POKENAV_CALL
	releaseall
	end


@ Crossing (20,29)-(20,33): Cynthia comes up column 16 from 8 rows below
@ and along the player's row to stand just west of them, at (19,y).
EternaCity_EventScript_CynthiaTryGiveEgg::
	lockall
	getplayerxy VAR_0x8004, VAR_0x8005
	copyvar VAR_0x8006, VAR_0x8005
	addvar VAR_0x8006, 8
	clearflag FLAG_HIDE_ETERNA_CITY_CYNTHIA
	setobjectxyperm LOCALID_ETERNA_CITY_CYNTHIA, 16, VAR_0x8006
	setobjectmovementtype LOCALID_ETERNA_CITY_CYNTHIA, MOVEMENT_TYPE_FACE_RIGHT
	addobject LOCALID_ETERNA_CITY_CYNTHIA
	applymovement LOCALID_ETERNA_CITY_CYNTHIA, EternaCity_Movement_CynthiaNoticeAndWalkUpToPlayer
	waitmovement 0
	applymovement LOCALID_PLAYER, Common_Movement_WalkInPlaceLeft
	waitmovement 0
	msgbox EternaCity_Text_WillYouAcceptEgg, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, EternaCity_EventScript_CynthiaEggRefused
	getpartysize
	goto_if_eq VAR_RESULT, PARTY_SIZE, EternaCity_EventScript_CynthiaNoRoomForEgg
	call EternaCity_EventScript_GiveTogepiEgg
	applymovement LOCALID_ETERNA_CITY_CYNTHIA, EternaCity_Movement_CynthiaLeaveWest
	waitmovement 0
	removeobject LOCALID_ETERNA_CITY_CYNTHIA
	releaseall
	end

EternaCity_EventScript_GiveTogepiEgg::
	playfanfare MUS_OBTAIN_ITEM
	message EternaCity_Text_ReceivedTheEgg
	waitfanfare
	giveegg SPECIES_TOGEPI
	setvar VAR_ETERNA_CITY_STATE, 5
	msgbox EternaCity_Text_ThatsWonderful, MSGBOX_DEFAULT
	msgbox EternaCity_Text_KeepEggInYourParty, MSGBOX_DEFAULT
	closemessage
	return

EternaCity_EventScript_CynthiaEggRefused::
	msgbox EternaCity_Text_DontFeelObligated, MSGBOX_DEFAULT
	closemessage
	setvar VAR_ETERNA_CITY_STATE, 4
	releaseall
	end

EternaCity_EventScript_CynthiaNoRoomForEgg::
	msgbox EternaCity_Text_NoRoomForEgg, MSGBOX_DEFAULT
	closemessage
	setvar VAR_ETERNA_CITY_STATE, 4
	releaseall
	end

@ Talking to her while she waits (state 4).
EternaCity_EventScript_Cynthia::
	lockall
	faceplayer
	msgbox EternaCity_Text_WillYouAcceptThisEgg, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, EternaCity_EventScript_CynthiaEggRefused
	getpartysize
	goto_if_eq VAR_RESULT, PARTY_SIZE, EternaCity_EventScript_CynthiaNoRoomForEgg
	call EternaCity_EventScript_GiveTogepiEgg
	goto_if_eq VAR_FACING, DIR_EAST, EternaCity_EventScript_CynthiaLeaveAroundPlayer
	goto_if_eq VAR_FACING, DIR_WEST, EternaCity_EventScript_CynthiaLeaveWest
	applymovement LOCALID_ETERNA_CITY_CYNTHIA, EternaCity_Movement_CynthiaLeaveWest
	applymovement LOCALID_PLAYER, EternaCity_Movement_PlayerWatchCynthiaLeave
	waitmovement 0
	goto EternaCity_EventScript_RemoveCynthia
	end

EternaCity_EventScript_CynthiaLeaveWest::
	applymovement LOCALID_ETERNA_CITY_CYNTHIA, EternaCity_Movement_CynthiaLeaveWest
	waitmovement 0
	goto EternaCity_EventScript_RemoveCynthia
	end

@ The player is west of her, in her way: she steps down first.
EternaCity_EventScript_CynthiaLeaveAroundPlayer::
	applymovement LOCALID_ETERNA_CITY_CYNTHIA, EternaCity_Movement_CynthiaLeaveAroundPlayer
	applymovement LOCALID_PLAYER, EternaCity_Movement_PlayerWatchCynthiaLeaveAround
	waitmovement 0
	goto EternaCity_EventScript_RemoveCynthia
	end

EternaCity_EventScript_RemoveCynthia::
	removeobject LOCALID_ETERNA_CITY_CYNTHIA
	releaseall
	end

@ Crossing (21,28)-(21,33) while she still has the Egg.
EternaCity_EventScript_CynthiaBlockCycleShop::
	lockall
	applymovement LOCALID_ETERNA_CITY_CYNTHIA, Common_Movement_WalkInPlaceRight
	waitmovement 0
	msgbox EternaCity_Text_DontLeaveWithoutEgg, MSGBOX_DEFAULT
	closemessage
	applymovement LOCALID_PLAYER, EternaCity_Movement_PlayerStepBackWest
	waitmovement 0
	releaseall
	end

EternaCity_Movement_CynthiaNoticeAndWalkUpToPlayer:
	walk_up
	walk_up
	walk_up
	emote_exclamation_mark
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	walk_right
	walk_right
	walk_right
	step_end

EternaCity_Movement_CynthiaLeaveWest:
	walk_left
	walk_left
	walk_left
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

EternaCity_Movement_CynthiaLeaveAroundPlayer:
	walk_down
	walk_left
	walk_left
	walk_left
	walk_down
	walk_down
	walk_down
	walk_down
	walk_down
	walk_down
	walk_down
	walk_down
	step_end

EternaCity_Movement_PlayerStepBackWest:
	walk_left
	step_end

EternaCity_Movement_PlayerWatchCynthiaLeave:
	delay_8
	walk_in_place_left
	step_end

EternaCity_Movement_PlayerWatchCynthiaLeaveAround:
	delay_8
	delay_8
	delay_4
	walk_in_place_left
	step_end

EternaCity_Text_WallyPokenavCall:
	.string "… … … … … …\n"
	.string "… … … … … Beep!\p"
	.string "WALLY: Oh, hello, {PLAYER}!\p"
	.string "I just wanted to tell you that\n"
	.string "my uncle bought me a POKéTCH!\p"
	.string "Now I can get in touch with you\n"
	.string "anytime, {PLAYER}!\p"
	.string "… … … … … …\n"
	.string "… … … … … Click!$"

EternaCity_Text_RegisteredWally:
	.string "Registered WALLY\n"
	.string "in the POKéTCH.$"

EternaCity_Text_StayAwayFromStatue:
	.string "Hey! You there, kid!\n"
	.string "Stay away from that POKéMON statue!\p"
	.string "TEAM GALACTIC is conducting an\n"
	.string "official investigation here!$"

EternaCity_Text_IllLetYouKeepPokemon:
	.string "Hey, you! Yeah, you, TRAINER!\p"
	.string "Errr…\n"
	.string "It's OK. Never mind. You look tough.\p"
	.string "I won't take… Um, I'll just let you\n"
	.string "keep your POKéMON. Carry on!$"

EternaCity_Text_HaveYouSeenOurBuilding:
	.string "Have you seen our building?\p"
	.string "TEAM GALACTIC is so stinking rich\n"
	.string "that we can have one built instantly!$"

EternaCity_Text_GetYourselfABicycle:
	.string "If you're visiting ETERNA, you need to\n"
	.string "get yourself a BICYCLE and take a\l"
	.string "ride down the CYCLING ROAD.$"

EternaCity_Text_AtmosphereHasGottenCreepy:
	.string "Since that TEAM GALACTIC building went\n"
	.string "up, the city's atmosphere has gotten\l"
	.string "all creepy and tense.\p"
	.string "I don't know what those people are\n"
	.string "doing up in that place…$"

EternaCity_Text_WhatWillHappenToGalacticBuilding:
	.string "With TEAM GALACTIC gone, what's going\n"
	.string "to happen to their building?$"

EternaCity_Text_GalacticTookFavoritePokemon:
	.string "My friend said TEAM GALACTIC took his\n"
	.string "favorite POKéMON away.\p"
	.string "I don't think he's telling the truth.\n"
	.string "TEAM GALACTIC looks so cool.\l"
	.string "They don't look like common thugs.$"

EternaCity_Text_DoYouUseKickstand:
	.string "Do you use your BIKE's kickstand?\p"
	.string "It looks cool to hop off your BIKE and\n"
	.string "then snap it into place in one motion!$"

EternaCity_Text_TheySayThisIsAnOldCity:
	.string "They say this is an old city.\p"
	.string "It doesn't look it, though.\n"
	.string "Not with all these tall buildings.$"

EternaCity_Text_EternaCitysNotLikeBefore:
	.string "ETERNA CITY's not anything like it\n"
	.string "was before, way back in the day.$"

EternaCity_Text_AwesomePokemonFromLongAgo:
	.string "They say this is an awesome POKéMON\n"
	.string "that people saw long ago.$"

EternaCity_Text_YouDontHaveExplorerKit:
	.string "Oh, don't tell me you don't have\n"
	.string "an EXPLORER KIT?\p"
	.string "You should have a word with the man\n"
	.string "next door to the POKéMON CENTER.$"

EternaCity_Text_YouGotABicycle:
	.string "Oh, wow!\n"
	.string "You got a BICYCLE!\p"
	.string "You can ride down CYCLING ROAD\n"
	.string "and end up in HEARTHOME CITY.\p"
	.string "It's a fast and fun ride.\n"
	.string "You should check it out!$"

EternaCity_Text_FeelsGreatRidingABike:
	.string "It feels great riding a BIKE with the\n"
	.string "wind in your face, doesn't it?$"

EternaCity_Text_ItsEternaCitysPokemonStatue:
	.string "It's ETERNA CITY's POKéMON statue.\p"
	.string "There seems to have been a descriptive\n"
	.string "plate, but it appears it was torn off.$"

EternaCity_Text_CitySign:
	.string "ETERNA CITY\n"
	.string "“History Living”$"

EternaCity_Text_GymSign:
	.string "ETERNA CITY POKéMON GYM\n"
	.string "LEADER: GARDENIA\p"
	.string "“Master of Vivid Plant POKéMON!”$"

EternaCity_Text_CycleShopSign:
	.string "RAD RICKSHAW'S CYCLE SHOP\n"
	.string "Get On and Ride!$"

EternaCity_Text_GalacticBuildingSign:
	.string "TEAM GALACTIC ETERNA BUILDING\n"
	.string "We Want Your POKéMON!$"

EternaCity_Text_UndergroundMansHouseSign:
	.string "UNDERGROUND MAN'S HOUSE\n"
	.string "Goin' Down for Adventure!$"

EternaCity_Text_CondominiumsSign:
	.string "ETERNA CONDOMINIUMS\n"
	.string "Name-Rating Services Available$"

EternaCity_Text_WillYouAcceptEgg:
	.string "CYNTHIA: Oh, there you are!\p"
	.string "I was looking for you.\n"
	.string "I've got something nice.\p"
	.string "I wanted you to have this POKéMON EGG.\n"
	.string "Will you accept it?$"

EternaCity_Text_ReceivedTheEgg:
	.string "{PLAYER} received the EGG.$"

EternaCity_Text_ThatsWonderful:
	.string "CYNTHIA: That's wonderful!\p"
	.string "The POKéMON inside the EGG is\n"
	.string "happy, too. I'm sure of it!$"

EternaCity_Text_KeepEggInYourParty:
	.string "CYNTHIA: Keep that EGG with you in your\n"
	.string "party of POKéMON.\p"
	.string "A POKéMON will hatch from it while you\n"
	.string "are traveling.\p"
	.string "I would be happy to know that it will\n"
	.string "help fill another page of your POKéDEX.\p"
	.string "See you again!$"

EternaCity_Text_NoRoomForEgg:
	.string "CYNTHIA: Oh?\n"
	.string "You have six POKéMON with you already.\l"
	.string "You don't have room for this EGG.\p"
	.string "You'll have to store a POKéMON in a\n"
	.string "PC BOX at the POKéMON CENTER first.\p"
	.string "I'll give you this EGG when you've made\n"
	.string "room for it.\p"
	.string "I'll be waiting right here.$"

EternaCity_Text_DontFeelObligated:
	.string "CYNTHIA: Oh…?\n"
	.string "You don't need to feel obligated.\p"
	.string "It's hard filling the pages of the\n"
	.string "POKéDEX, isn't it?\p"
	.string "I only want to lend you a little hand,\n"
	.string "that's all.$"

EternaCity_Text_WillYouAcceptThisEgg:
	.string "CYNTHIA: Well?\n"
	.string "Will you accept this POKéMON EGG?$"

EternaCity_Text_DontLeaveWithoutEgg:
	.string "CYNTHIA: Oh, wait!\p"
	.string "Don't leave without this POKéMON EGG.\n"
	.string "Please? I want you to have it.$"
