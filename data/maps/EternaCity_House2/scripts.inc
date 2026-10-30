EternaCity_House2_MapScripts::
	.byte 0

EternaCity_House2_EventScript_Woman::
	lock
	faceplayer
	goto_if_set FLAG_RECEIVED_COIN_CASE, EternaCity_House2_EventScript_ReceivedCoinCase
	msgbox EternaCity_House2_Text_BuyHarborMailAtSlateport, MSGBOX_DEFAULT
	checkitem ITEM_HARBOR_MAIL
	goto_if_eq VAR_RESULT, TRUE, EternaCity_House2_EventScript_AskToTradeForHarborMail
	release
	end

EternaCity_House2_EventScript_AskToTradeForHarborMail::
	playse SE_PIN
	applywaitmovement VAR_LAST_TALKED, Common_Movement_ExclamationMark
	applywaitmovement VAR_LAST_TALKED, Common_Movement_Delay48
	msgbox EternaCity_House2_Text_TradeHarborMailForCoinCase, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, YES, EternaCity_House2_EventScript_AcceptTrade
	goto_if_eq VAR_RESULT, NO, EternaCity_House2_EventScript_DeclineTrade
	end

EternaCity_House2_EventScript_AcceptTrade::
	msgbox EternaCity_House2_Text_IllTradeYouCoinCase, MSGBOX_DEFAULT
	removeitem ITEM_HARBOR_MAIL
	giveitem ITEM_COIN_CASE
	setflag FLAG_RECEIVED_COIN_CASE
	goto EternaCity_House2_EventScript_ReceivedCoinCase
	end

EternaCity_House2_EventScript_ReceivedCoinCase::
	msgbox EternaCity_House2_Text_UseCoinCaseAtGameCorner, MSGBOX_DEFAULT
	release
	end

EternaCity_House2_EventScript_DeclineTrade::
	msgbox EternaCity_House2_Text_ThatsDisappointing, MSGBOX_DEFAULT
	release
	end

EternaCity_House2_Text_BuyHarborMailAtSlateport:
	.string "If I had a BIKE, it'd be easy to cycle to\n"
	.string "SLATEPORT for some shopping.\p"
	.string "I'd be able to buy HARBOR MAIL at the\n"
	.string "POKéMON MART in SLATEPORT…$"

EternaCity_House2_Text_TradeHarborMailForCoinCase:
	.string "Oh! You have HARBOR MAIL?\n"
	.string "Will you trade it for a COIN CASE?$"

EternaCity_House2_Text_IllTradeYouCoinCase:
	.string "Oh, I'm so happy!\n"
	.string "Okay, I'll trade you a COIN CASE!$"

EternaCity_House2_Text_UseCoinCaseAtGameCorner:
	.string "That COIN CASE can be used\n"
	.string "at the GAME CORNER.$"

EternaCity_House2_Text_ThatsDisappointing:
	.string "Oh, that's disappointing.\p"
	.string "A COIN CASE is needed for the\n"
	.string "GAME CORNER.$"

