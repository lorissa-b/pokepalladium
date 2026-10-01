@ The gate between Jubilife City's west end and Route 218, using Emerald's
@ Seaside Cycling Road gate interior. The east door leads back to Jubilife;
@ the west door has no warp until Route 218 exists.
JubilifeCity_Route218Gate_MapScripts::
	.byte 0

JubilifeCity_Route218Gate_EventScript_Clerk::
	msgbox JubilifeCity_Route218Gate_Text_Route218IsClosed, MSGBOX_NPC
	end

JubilifeCity_Route218Gate_EventScript_Fisherman::
	lock
	faceplayer
	goto_if_set FLAG_RECEIVED_OLD_ROD, JubilifeCity_Route218Gate_EventScript_ReceivedOldRod
	msgbox JubilifeCity_Route218Gate_Text_YouShouldTryFishing, MSGBOX_DEFAULT
	giveitem ITEM_OLD_ROD
	goto_if_eq VAR_RESULT, FALSE, Common_EventScript_ShowBagIsFull
	setflag FLAG_RECEIVED_OLD_ROD
	msgbox JubilifeCity_Route218Gate_Text_ExplainOldRod, MSGBOX_DEFAULT
	release
	end

JubilifeCity_Route218Gate_EventScript_ReceivedOldRod::
	msgbox JubilifeCity_Route218Gate_Text_HowsFishing, MSGBOX_DEFAULT
	release
	end

JubilifeCity_Route218Gate_Text_Route218IsClosed:
	.string "Through the west door is ROUTE 218.\p"
	.string "I'm sorry, but it's closed for now.\n"
	.string "Please come back another time.$"

JubilifeCity_Route218Gate_Text_YouShouldTryFishing:
	.string "Fishing is the best!\n"
	.string "You never know what you'll reel in!\p"
	.string "You should give it a try, too.\n"
	.string "Here, take this!$"

JubilifeCity_Route218Gate_Text_ExplainOldRod:
	.string "Use the OLD ROD while facing water,\n"
	.string "and see what bites!$"

JubilifeCity_Route218Gate_Text_HowsFishing:
	.string "How's the fishing going?\n"
	.string "Catch anything good?$"
