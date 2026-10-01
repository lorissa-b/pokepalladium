JubilifeCity_Apartments_2F_MapScripts::
	.byte 0

JubilifeCity_Apartments_2F_EventScript_OldMan::
	msgbox JubilifeCity_Apartments_2F_Text_DevonWasTinyInOldDays, MSGBOX_NPC
	end

JubilifeCity_Apartments_2F_EventScript_NinjaBoy::
	lock
	faceplayer
	goto_if_set FLAG_RECEIVED_PREMIER_BALL_JUBILIFE, JubilifeCity_Apartments_2F_EventScript_GavePremierBall
	msgbox JubilifeCity_Apartments_2F_Text_MyDaddyMadeThisYouCanHaveIt, MSGBOX_DEFAULT
	giveitem ITEM_PREMIER_BALL
	goto_if_eq VAR_RESULT, FALSE, Common_EventScript_ShowBagIsFull
	setflag FLAG_RECEIVED_PREMIER_BALL_JUBILIFE
	release
	end

JubilifeCity_Apartments_2F_EventScript_GavePremierBall::
	msgbox JubilifeCity_Apartments_2F_Text_GoingToWorkAtDevonToo, MSGBOX_DEFAULT
	release
	end

JubilifeCity_Apartments_2F_Text_DevonWasTinyInOldDays:
	.string "Way back in the old days, DEVON was just\n"
	.string "a teeny, tiny company.$"

JubilifeCity_Apartments_2F_Text_MyDaddyMadeThisYouCanHaveIt:
	.string "My daddy's working at the CORPORATION.\p"
	.string "My daddy made this!\n"
	.string "But I can't use it, so you can have it.$"

JubilifeCity_Apartments_2F_Text_GoingToWorkAtDevonToo:
	.string "My daddy's working at the CORPORATION.\p"
	.string "When I grow up, I'm going to work for\n"
	.string "DEVON, too.$"

