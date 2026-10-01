@ Route 204, between Jubilife City and Floaroma Town. Drawn over Emerald's
@ Route 116. The southern part (from Jubilife) and the northern part are only
@ joined through Ravaged Path, whose two cave mouths are drawn but not yet
@ linked to a cave map.
Route204_MapScripts::
	.byte 0

Route204_EventScript_RouteSignNorth::
	msgbox Route204_Text_RouteSign, MSGBOX_SIGN
	end

Route204_EventScript_RouteSignSouth::
	msgbox Route204_Text_RouteSign, MSGBOX_SIGN
	end

Route204_EventScript_RavagedPathSign::
	msgbox Route204_Text_RavagedPathSign, MSGBOX_SIGN
	end

@ At the end of the narrow path past the Cut trees. In Diamond/Pearl she gives
@ TM78 (Captivate), which this game doesn't have, so she gives Attract instead.
Route204_EventScript_AttractWoman::
	lock
	faceplayer
	goto_if_set FLAG_RECEIVED_ROUTE_204_TM_ATTRACT, Route204_EventScript_ReceivedAttract
	msgbox Route204_Text_YouFoundMyHidingPlace, MSGBOX_DEFAULT
	giveitem ITEM_TM_ATTRACT
	goto_if_eq VAR_RESULT, 0, Common_EventScript_ShowBagIsFull
	setflag FLAG_RECEIVED_ROUTE_204_TM_ATTRACT
	msgbox Route204_Text_AttractExplanation, MSGBOX_DEFAULT
	release
	end

Route204_EventScript_ReceivedAttract::
	msgbox Route204_Text_AttractExplanation, MSGBOX_DEFAULT
	release
	end

Route204_Text_RouteSign:
	.string "ROUTE 204\n"
	.string "{UP_ARROW} FLOAROMA TOWN  {DOWN_ARROW} JUBILIFE CITY$"

Route204_Text_RavagedPathSign:
	.string "RAVAGED PATH$"

Route204_Text_YouFoundMyHidingPlace:
	.string "Oh! You cut your way all the way\n"
	.string "back here just to see me?\p"
	.string "That's so sweet! You deserve\n"
	.string "a present!$"

Route204_Text_AttractExplanation:
	.string "That TM is ATTRACT.\p"
	.string "If the target is the opposite gender\n"
	.string "of your POKéMON, it falls in love\l"
	.string "and may not attack.$"
