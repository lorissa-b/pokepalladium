JubilifeCity_Mart_MapScripts::
	.byte 0

JubilifeCity_Mart_EventScript_Clerk::
	lock
	faceplayer
	message gText_HowMayIServeYou
	waitmessage
	goto_if_unset FLAG_MET_DEVON_EMPLOYEE, JubilifeCity_Mart_EventScript_PokemartBasic
	goto_if_set FLAG_MET_DEVON_EMPLOYEE, JubilifeCity_Mart_EventScript_PokemartExpanded
	end

JubilifeCity_Mart_EventScript_PokemartBasic::
	pokemart JubilifeCity_Mart_Pokemart_Basic
	msgbox gText_PleaseComeAgain, MSGBOX_DEFAULT
	release
	end

	.align 2
JubilifeCity_Mart_Pokemart_Basic:
	.2byte ITEM_POKE_BALL
	.2byte ITEM_POTION
	.2byte ITEM_SUPER_POTION
	.2byte ITEM_ANTIDOTE
	.2byte ITEM_PARALYZE_HEAL
	.2byte ITEM_ESCAPE_ROPE
	.2byte ITEM_REPEL
	.2byte ITEM_X_SPEED
	.2byte ITEM_X_ATTACK
	.2byte ITEM_X_DEFEND
	pokemartlistend

JubilifeCity_Mart_EventScript_PokemartExpanded::
	pokemart JubilifeCity_Mart_Pokemart_Expanded
	msgbox gText_PleaseComeAgain, MSGBOX_DEFAULT
	release
	end

	.align 2
JubilifeCity_Mart_Pokemart_Expanded:
	.2byte ITEM_POKE_BALL
	.2byte ITEM_TIMER_BALL
	.2byte ITEM_REPEAT_BALL
	.2byte ITEM_POTION
	.2byte ITEM_SUPER_POTION
	.2byte ITEM_ANTIDOTE
	.2byte ITEM_PARALYZE_HEAL
	.2byte ITEM_ESCAPE_ROPE
	.2byte ITEM_REPEL
	.2byte ITEM_X_SPEED
	.2byte ITEM_X_ATTACK
	.2byte ITEM_X_DEFEND
	pokemartlistend

JubilifeCity_Mart_EventScript_PokefanF::
	msgbox JubilifeCity_Mart_Text_BuyingHealsInCaseOfShroomish, MSGBOX_NPC
	end

JubilifeCity_Mart_EventScript_Boy::
	msgbox JubilifeCity_Mart_Text_ShouldBuySuperPotionsInstead, MSGBOX_NPC
	end

JubilifeCity_Mart_EventScript_BugCatcher::
	msgbox JubilifeCity_Mart_Text_GettingEscapeRopeJustInCase, MSGBOX_NPC
	end

JubilifeCity_Mart_Text_BuyingHealsInCaseOfShroomish:
	.string "I'm buying some PARLYZ HEALS and\n"
	.string "ANTIDOTES.\p"
	.string "Just in case I run into SHROOMISH\n"
	.string "in PETALBURG WOODS.$"

JubilifeCity_Mart_Text_ShouldBuySuperPotionsInstead:
	.string "My POKéMON evolved.\n"
	.string "It has a lot of HP now.\p"
	.string "I should buy SUPER POTIONS for it\n"
	.string "instead of ordinary POTIONS.$"

JubilifeCity_Mart_Text_GettingEscapeRopeJustInCase:
	.string "I'm getting an ESCAPE ROPE just in\n"
	.string "case I get lost in a cave.\p"
	.string "I just need to use it to get back to\n"
	.string "the entrance.$"

