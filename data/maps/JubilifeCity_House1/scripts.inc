JubilifeCity_House1_MapScripts::
	.byte 0

JubilifeCity_House1_EventScript_Trader::
	lock
	faceplayer
	goto_if_set FLAG_JUBILIFE_NPC_TRADE_COMPLETED, JubilifeCity_House1_EventScript_TradeCompleted
	setvar VAR_0x8008, INGAME_TRADE_SEEDOT
	copyvar VAR_0x8004, VAR_0x8008
	specialvar VAR_RESULT, GetInGameTradeSpeciesInfo
	copyvar VAR_0x8009, VAR_RESULT
	msgbox JubilifeCity_House1_Text_IllTradeIfYouWant, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, JubilifeCity_House1_EventScript_DeclineTrade
	special ChoosePartyMon
	copyvar VAR_0x800A, VAR_0x8004
	goto_if_eq VAR_0x8004, PARTY_NOTHING_CHOSEN, JubilifeCity_House1_EventScript_DeclineTrade
	copyvar VAR_0x8005, VAR_0x800A
	specialvar VAR_RESULT, GetTradeSpecies
	copyvar VAR_0x800B, VAR_RESULT
	goto_if_ne VAR_RESULT, VAR_0x8009, JubilifeCity_House1_EventScript_NotRequestedMon
	copyvar VAR_0x8004, VAR_0x8008
	copyvar VAR_0x8005, VAR_0x800A
	special CreateInGameTradePokemon
	special DoInGameTradeScene
	msgbox JubilifeCity_House1_Text_PleaseBeGoodToMyPokemon, MSGBOX_DEFAULT
	setflag FLAG_JUBILIFE_NPC_TRADE_COMPLETED
	release
	end

JubilifeCity_House1_EventScript_DeclineTrade::
	msgbox JubilifeCity_House1_Text_YouDontWantToThatsOkay, MSGBOX_DEFAULT
	release
	end

JubilifeCity_House1_EventScript_NotRequestedMon::
	bufferspeciesname STR_VAR_1, VAR_0x8009
	msgbox JubilifeCity_House1_Text_DoesntLookLikeMonToMe, MSGBOX_DEFAULT
	release
	end

JubilifeCity_House1_EventScript_TradeCompleted::
	msgbox JubilifeCity_House1_Text_AnyPokemonCanBeCute, MSGBOX_DEFAULT
	release
	end

JubilifeCity_House1_EventScript_Hiker::
	msgbox JubilifeCity_House1_Text_AllSortsOfPlaces, MSGBOX_NPC
	end

JubilifeCity_House1_Text_IllTradeIfYouWant:
	.string "Huh? My POKéMON is cute?\n"
	.string "Sure, I knew that.\p"
	.string "But if you really want, I'm willing\n"
	.string "to trade it to you.\p"
	.string "I'll trade you my {STR_VAR_2} for\n"
	.string "a {STR_VAR_1} if you want.$"

JubilifeCity_House1_Text_PleaseBeGoodToMyPokemon:
	.string "Eheheh…\n"
	.string "Please be good to my POKéMON.$"

JubilifeCity_House1_Text_DoesntLookLikeMonToMe:
	.string "Huh? That doesn't look anything like\n"
	.string "a {STR_VAR_1} to me.$"

JubilifeCity_House1_Text_YouDontWantToThatsOkay:
	.string "Oh, if you don't want to, that's okay.\n"
	.string "But my POKéMON is cute, you know…$"

JubilifeCity_House1_Text_AnyPokemonCanBeCute:
	.string "Any POKéMON can be cute if you raise\n"
	.string "it with care and kindness.$"

JubilifeCity_House1_Text_AllSortsOfPlaces:
	.string "In all sorts of places, there are all\n"
	.string "sorts of POKéMON and people.\p"
	.string "I find that fascinating, so I go to all\n"
	.string "sorts of places.$"

