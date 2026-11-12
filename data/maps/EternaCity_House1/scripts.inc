@ The Underground Man's house, from Platinum's, over Mauville's Rock Smash
@ house (Oreburgh Gate gives HM06, as in Platinum). The Underground Man
@ gives the EXPLORER KIT (FLAG_RECEIVED_EXPLORER_KIT). Palladium has no
@ Underground: the Kit raises a rock wall to mine outdoors instead (see
@ src/mining.c), so his and his visitors' lines about the tunnels, Spheres
@ and the touch screen are reworded for that. His Underground missions,
@ the Underground Radar and his PC's Underground manual are left out.
EternaCity_House1_MapScripts::
	.byte 0

EternaCity_House1_EventScript_UndergroundMan::
	lock
	faceplayer
	goto_if_set FLAG_RECEIVED_EXPLORER_KIT, EternaCity_House1_EventScript_UseExplorerKit
	msgbox EternaCity_House1_Text_CallMeUndergroundMan, MSGBOX_DEFAULT
	giveitem ITEM_EXPLORER_KIT
	setflag FLAG_RECEIVED_EXPLORER_KIT
	msgbox EternaCity_House1_Text_MyGiftExplorerKit, MSGBOX_DEFAULT
	release
	end

EternaCity_House1_EventScript_UseExplorerKit::
	msgbox EternaCity_House1_Text_UseExplorerKit, MSGBOX_DEFAULT
	release
	end

EternaCity_House1_EventScript_Youngster::
	msgbox EternaCity_House1_Text_TunnelsGoEverywhere, MSGBOX_NPC
	end

EternaCity_House1_EventScript_Scientist::
	lock
	faceplayer
	goto_if_set FLAG_TALKED_TO_UNDERGROUND_MAN_HOUSE_SCIENTIST, EternaCity_House1_EventScript_ScientistMoreAdvice
	setflag FLAG_TALKED_TO_UNDERGROUND_MAN_HOUSE_SCIENTIST
	msgbox EternaCity_House1_Text_LetsDigUpSomeTreasure, MSGBOX_DEFAULT
	release
	end

EternaCity_House1_EventScript_ScientistMoreAdvice::
	msgbox EternaCity_House1_Text_YoudLikeMoreAdvice, MSGBOX_DEFAULT
	release
	end

EternaCity_House1_EventScript_BugCatcher::
	msgbox EternaCity_House1_Text_AllIUnearthAreArmorFossils, MSGBOX_NPC
	end

EternaCity_House1_EventScript_Poster::
	msgbox EternaCity_House1_Text_OnSaleUndergroundRadar, MSGBOX_SIGN
	end

EternaCity_House1_Text_CallMeUndergroundMan:
	.string "I am! I am the first to have gone\n"
	.string "underground and dug some tunnels.\p"
	.string "You can call me the UNDERGROUND MAN.\n"
	.string "Everyone else does.\p"
	.string "I'll make a gift of this to you.\n"
	.string "You hold up your end of the deal\l"
	.string "and put it to good use!$"

EternaCity_House1_Text_MyGiftExplorerKit:
	.string "My gift to you, the EXPLORER KIT,\n"
	.string "holds a pickax and a sledgehammer.\p"
	.string "Use it, and a rock wall will rise up\n"
	.string "out of the ground before you!\p"
	.string "There are many hidden things in\n"
	.string "those walls. Dig them out!\p"
	.string "But you need to be outside. It can't\n"
	.string "be used indoors or in a cave.$"

EternaCity_House1_Text_UseExplorerKit:
	.string "To dig for Treasure, you need only\n"
	.string "to use your EXPLORER KIT.\p"
	.string "But you need to be outside. It can't\n"
	.string "be used indoors or in a cave.\p"
	.string "Now, off you go!$"

EternaCity_House1_Text_TunnelsGoEverywhere:
	.string "The underground tunnels go\n"
	.string "everywhere!$"

EternaCity_House1_Text_LetsDigUpSomeTreasure:
	.string "Let's dig up some Treasure!\p"
	.string "I'm from OREBURGH.\n"
	.string "I work at the mining museum there.\p"
	.string "I got advice from the UNDERGROUND\n"
	.string "MAN on digging up Treasure.\p"
	.string "Before I get digging, let me share\n"
	.string "his advice with you.\p"
	.string "While you're digging, you may uncover\n"
	.string "dark rocks.\p"
	.string "They are very hard! If you keep\n"
	.string "hitting them, the wall will collapse.\p"
	.string "Don't forget that, and you'll be\n"
	.string "digging up Treasure in no time!$"

EternaCity_House1_Text_YoudLikeMoreAdvice:
	.string "Oh, you'd like more advice on digging\n"
	.string "up Treasure?\p"
	.string "Well, your EXPLORER KIT contains\n"
	.string "a sledgehammer and a pickax.\p"
	.string "Choose either of these tools and\n"
	.string "press the A Button to dig.\p"
	.string "You get to keep the stuff you fully\n"
	.string "uncover before the wall collapses.\p"
	.string "Don't forget, the dark rocks are\n"
	.string "impossible to break.\p"
	.string "If you hit them too often, the wall\n"
	.string "will collapse.\p"
	.string "Don't forget that, and you'll be\n"
	.string "digging up Treasure in no time!$"

EternaCity_House1_Text_AllIUnearthAreArmorFossils:
	.string "All I ever unearth are ARMOR FOSSILS.\n"
	.string "My buddy only digs up SKULL FOSSILS.\l"
	.string "I wonder why that happens?\p"
	.string "It doesn't matter though. I can just\n"
	.string "swap them with my buddy.\p"
	.string "I simply let a POKéMON hold an ARMOR\n"
	.string "FOSSIL and trade it to him.$"

EternaCity_House1_Text_OnSaleUndergroundRadar:
	.string "New and on sale!\n"
	.string "Underground Radar!$"
