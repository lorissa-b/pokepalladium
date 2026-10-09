@ The house in Floaroma Meadow, with Platinum's Hiker and Ninja Boy
FloaromaMeadow_House_MapScripts::
	.byte 0

FloaromaMeadow_House_EventScript_Hiker::
	msgbox FloaromaMeadow_House_Text_FlowersInTheFieldsAreSpecial, MSGBOX_NPC
	end

FloaromaMeadow_House_EventScript_NinjaBoy::
	msgbox FloaromaMeadow_House_Text_RustlingTreeOftenRarePokemon, MSGBOX_NPC
	end

FloaromaMeadow_House_Text_FlowersInTheFieldsAreSpecial:
	.string "The flowers that grow in the fields\n"
	.string "here are special.\p"
	.string "Do you see how they're more vivid\n"
	.string "than flowers elsewhere in SINNOH?\p"
	.string "You can thank POKéMON for that.$"

FloaromaMeadow_House_Text_RustlingTreeOftenRarePokemon:
	.string "If you put HONEY on a tree, check\n"
	.string "back later to see if it's rustling.\p"
	.string "If it is, it's quite often a rare\n"
	.string "POKéMON.\p"
	.string "It gets my hopes up if I see that\n"
	.string "happening.$"
