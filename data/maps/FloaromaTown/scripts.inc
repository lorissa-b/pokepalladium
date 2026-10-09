@ Floaroma Town, north of Route 204, laid out as in Platinum. The north
@ meadow entrance (20-21, 4) is reached from Route 205's Fuego Ironworks
@ side, not from the town; the west entrance (2-3, 33) is the way into
@ Floaroma Meadow from here. Two Team Galactic Grunts stand in the west
@ entrance until FLAG_HIDE_FLOAROMA_TOWN_GRUNTS is set; in Platinum the
@ little girl on Route 205 South sets it.
FloaromaTown_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, FloaromaTown_OnTransition
	.byte 0

FloaromaTown_OnTransition:
	setflag FLAG_VISITED_FLOAROMA_TOWN
	end

FloaromaTown_EventScript_Grunts::
	lockall
	applywaitmovement LOCALID_FLOAROMA_TOWN_GRUNT_WEST, Common_Movement_WalkInPlaceFasterRight
	msgbox FloaromaTown_Text_ThisDoesntSeemCool, MSGBOX_DEFAULT
	closemessage
	applywaitmovement LOCALID_FLOAROMA_TOWN_GRUNT_EAST, Common_Movement_WalkInPlaceFasterLeft
	msgbox FloaromaTown_Text_DontComplain, MSGBOX_DEFAULT
	releaseall
	end

FloaromaTown_EventScript_PokemonBreeder::
	msgbox FloaromaTown_Text_WelcomeToFloaroma, MSGBOX_NPC
	end

FloaromaTown_EventScript_LassWest::
	msgbox FloaromaTown_Text_ISimplyLoveItHere, MSGBOX_NPC
	end

@ In Platinum he talks about the Grunts in the meadow until Valley Windworks
FloaromaTown_EventScript_SchoolKid::
	msgbox FloaromaTown_Text_OddlyPleasantMood, MSGBOX_NPC
	end

@ In Platinum she gives Gracidea to a player with Shaymin
FloaromaTown_EventScript_Beauty::
	msgbox FloaromaTown_Text_DoYouKnowGracideaFlowers, MSGBOX_NPC
	end

FloaromaTown_EventScript_LassEast::
	msgbox FloaromaTown_Text_BerriesCanBeFoodForPokemon, MSGBOX_NPC
	end

FloaromaTown_EventScript_Camper::
	msgbox FloaromaTown_Text_DrawnHereByHoney, MSGBOX_NPC
	end

FloaromaTown_EventScript_TownSign::
	msgbox FloaromaTown_Text_TownSign, MSGBOX_SIGN
	end

FloaromaTown_EventScript_FlowerShopSign::
	msgbox FloaromaTown_Text_FlowerShopSign, MSGBOX_SIGN
	end

FloaromaTown_EventScript_MeadowSign::
	msgbox FloaromaTown_Text_MeadowSign, MSGBOX_SIGN
	end

FloaromaTown_Text_ThisDoesntSeemCool:
	.string "GRUNT: Aren't we TEAM GALACTIC?\n"
	.string "And we're assigned here?\p"
	.string "Standing around among flowers…\n"
	.string "This doesn't seem all that cool.$"

FloaromaTown_Text_DontComplain:
	.string "GRUNT: Hey now! Don't complain.\n"
	.string "This is a pretty decent assignment.\p"
	.string "But BUG- and GRASS-type POKéMON so\n"
	.string "creep me out…\p"
	.string "I don't like being here either.$"

FloaromaTown_Text_WelcomeToFloaroma:
	.string "Welcome to FLOAROMA, the town of\n"
	.string "flowers, flowers, and more flowers!\p"
	.string "You should plant some flowers of\n"
	.string "your own, too. Yay for flowers!$"

FloaromaTown_Text_ISimplyLoveItHere:
	.string "I simply love it here…\n"
	.string "The atmosphere is so uplifting.\p"
	.string "You came to get emotionally healed\n"
	.string "too, didn't you?$"

FloaromaTown_Text_OddlyPleasantMood:
	.string "Being surrounded by flowers puts me\n"
	.string "in an oddly pleasant mood.$"

FloaromaTown_Text_DoYouKnowGracideaFlowers:
	.string "Do you know about GRACIDEA flowers?\p"
	.string "They're given as bouquets to express\n"
	.string "feelings of gratitude.\p"
	.string "It's an enduring tradition around\n"
	.string "these parts.$"

FloaromaTown_Text_BerriesCanBeFoodForPokemon:
	.string "The BERRIES you pick can be used as\n"
	.string "food for POKéMON.\p"
	.string "You can also trade them in at the\n"
	.string "florist's.\p"
	.string "They'll exchange BERRIES for\n"
	.string "ACCESSORIES to dress up your\l"
	.string "POKéMON.$"

FloaromaTown_Text_DrawnHereByHoney:
	.string "I got drawn here by the sweet scent\n"
	.string "of HONEY…\p"
	.string "Seriously, what's that? What am I?\n"
	.string "A POKéMON or something?\l"
	.string "Mmm… HONEY.$"

FloaromaTown_Text_TownSign:
	.string "FLOAROMA TOWN\n"
	.string "“Vivid & Scented”$"

FloaromaTown_Text_FlowerShopSign:
	.string "PICK A PECK OF COLORS FLOWER SHOP\n"
	.string "Free BERRIES Available$"

FloaromaTown_Text_MeadowSign:
	.string "FLOAROMA MEADOW AHEAD\n"
	.string "HONEY Available$"
