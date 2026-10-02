@ The gate between Jubilife City's west end and Route 218, using Emerald's
@ Seaside Cycling Road gate interior. From Jubilife it is entered through an
@ invisible door in the building's east side (west-arrow warps on the road
@ next to it); its east door leads back there and its west door leads to
@ the matching side door of the gate building on Route 218.
JubilifeCity_Route218Gate_MapScripts::
	.byte 0

JubilifeCity_Route218Gate_EventScript_Clerk::
	msgbox JubilifeCity_Route218Gate_Text_Route218IsWest, MSGBOX_NPC
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

JubilifeCity_Route218Gate_Text_Route218IsWest:
	.string "Through the west door is ROUTE 218.\p"
	.string "You'll need a POKéMON that can SURF\n"
	.string "to get across it.$"

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
