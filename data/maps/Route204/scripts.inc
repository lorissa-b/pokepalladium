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

Route204_Text_RouteSign:
	.string "ROUTE 204\n"
	.string "{UP_ARROW} FLOAROMA TOWN  {DOWN_ARROW} JUBILIFE CITY$"

Route204_Text_RavagedPathSign:
	.string "RAVAGED PATH$"
