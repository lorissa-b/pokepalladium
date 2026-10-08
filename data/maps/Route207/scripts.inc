Route207_MapScripts::
	.byte 0

Route207_EventScript_Cyclist::
	lock
	faceplayer
	goto_if_set FLAG_RECEIVED_BIKE, Route207_EventScript_CyclistTakeARun
	msgbox Route207_Text_ThatSlopesTooSlippery, MSGBOX_DEFAULT
	release
	end

Route207_EventScript_CyclistTakeARun::
	msgbox Route207_Text_TakeARunUpTheSlope, MSGBOX_DEFAULT
	release
	end

Route207_EventScript_SignMtCoronet::
	msgbox Route207_Text_SignMtCoronet, MSGBOX_SIGN
	end

Route207_EventScript_SignOreburghCity::
	msgbox Route207_Text_SignOreburghCity, MSGBOX_SIGN
	end

Route207_EventScript_TrainerTips::
	msgbox Route207_Text_TrainerTips, MSGBOX_SIGN
	end

Route207_Text_ThatSlopesTooSlippery:
	.string "That slope's too slippery.\n"
	.string "You'll need a BIKE to get up.\p"
	.string "You don't have a BIKE?\n"
	.string "There's a CYCLE SHOP in ETERNA CITY.$"

Route207_Text_TakeARunUpTheSlope:
	.string "Get some speed up on your BIKE and\n"
	.string "take a run.\p"
	.string "That'll shoot you right up that slope.$"

Route207_Text_SignMtCoronet:
	.string "ROUTE 207\n"
	.string "{RIGHT_ARROW} MT. CORONET$"

Route207_Text_SignOreburghCity:
	.string "ROUTE 207\n"
	.string "{DOWN_ARROW} OREBURGH CITY$"

Route207_Text_TrainerTips:
	.string "TRAINER TIPS\p"
	.string "POKéMON may become immobilized if\n"
	.string "they are asleep or paralyzed.\p"
	.string "If it happens to your POKéMON,\n"
	.string "it could be disastrous.\p"
	.string "If your opponent's POKéMON can't\n"
	.string "move, it's a big opportunity!$"
