JubilifeCity_Condominiums_1F_MapScripts::
	.byte 0

JubilifeCity_Condominiums_1F_EventScript_Man::
	msgbox JubilifeCity_Condominiums_1F_Text_EveryPokemonHasAbility, MSGBOX_NPC
	end

JubilifeCity_Condominiums_1F_EventScript_Woman::
	msgbox JubilifeCity_Condominiums_1F_Text_PokemonStrange, MSGBOX_NPC
	end

JubilifeCity_Condominiums_1F_EventScript_QuickClawGirl::
	lock
	faceplayer
	goto_if_set FLAG_RECEIVED_QUICK_CLAW, JubilifeCity_Condominiums_1F_EventScript_ReceivedQuickClaw
	msgbox JubilifeCity_Condominiums_1F_Text_YouCanHaveThis, MSGBOX_DEFAULT
	giveitem ITEM_QUICK_CLAW
	goto_if_eq VAR_RESULT, FALSE, Common_EventScript_ShowBagIsFull
	setflag FLAG_RECEIVED_QUICK_CLAW
	msgbox JubilifeCity_Condominiums_1F_Text_ExplainQuickClaw, MSGBOX_DEFAULT
	release
	end

JubilifeCity_Condominiums_1F_EventScript_ReceivedQuickClaw::
	msgbox JubilifeCity_Condominiums_1F_Text_ExplainQuickClaw, MSGBOX_DEFAULT
	release
	end

JubilifeCity_Condominiums_1F_Text_EveryPokemonHasAbility:
	.string "Every POKéMON has a special ability\n"
	.string "that it can use.$"

JubilifeCity_Condominiums_1F_Text_PokemonStrange:
	.string "POKéMON are such strange creatures.$"

JubilifeCity_Condominiums_1F_Text_YouCanHaveThis:
	.string "I found this claw, but I don't\n"
	.string "really need it.\p"
	.string "You're a TRAINER, right?\n"
	.string "You can have it!$"

JubilifeCity_Condominiums_1F_Text_ExplainQuickClaw:
	.string "If a POKéMON holds a QUICK CLAW,\n"
	.string "it can sometimes move first\l"
	.string "in battle.$"
