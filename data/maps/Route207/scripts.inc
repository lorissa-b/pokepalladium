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

Route207_EventScript_Anthony::
	trainerbattle_single TRAINER_ANDREW, Route207_Text_AnthonyIntro, Route207_Text_AnthonyDefeat
	msgbox Route207_Text_AnthonyPostBattle, MSGBOX_AUTOCLOSE
	end

Route207_EventScript_Lauren::
	trainerbattle_single TRAINER_SHAYLA, Route207_Text_LaurenIntro, Route207_Text_LaurenDefeat
	msgbox Route207_Text_LaurenPostBattle, MSGBOX_AUTOCLOSE
	end

Route207_EventScript_Austin::
	trainerbattle_single TRAINER_MARCOS, Route207_Text_AustinIntro, Route207_Text_AustinDefeat
	msgbox Route207_Text_AustinPostBattle, MSGBOX_AUTOCLOSE
	end

Route207_EventScript_Justin::
	trainerbattle_single TRAINER_BRYANT, Route207_Text_JustinIntro, Route207_Text_JustinDefeat
	msgbox Route207_Text_JustinPostBattle, MSGBOX_AUTOCLOSE
	end

Route207_EventScript_Kevin::
	trainerbattle_single TRAINER_RHETT, Route207_Text_KevinIntro, Route207_Text_KevinDefeat
	msgbox Route207_Text_KevinPostBattle, MSGBOX_AUTOCLOSE
	end

Route207_EventScript_Helen::
	trainerbattle_single TRAINER_ISABELLE, Route207_Text_HelenIntro, Route207_Text_HelenDefeat
	msgbox Route207_Text_HelenPostBattle, MSGBOX_AUTOCLOSE
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

Route207_Text_AnthonyIntro:
	.string "Go for it, my POKéMON!\n"
	.string "I'll do my best, too!$"

Route207_Text_AnthonyDefeat:
	.string "Oh…\n"
	.string "We lost…$"

Route207_Text_AnthonyPostBattle:
	.string "Dig for fossils in the UNDERGROUND.\p"
	.string "The MINING MUSEUM might be able to\n"
	.string "bring back the ancient POKéMON!$"

Route207_Text_LaurenIntro:
	.string "Go do it, my POKéMON!\n"
	.string "I'll work hard, too!$"

Route207_Text_LaurenDefeat:
	.string "Oh…\n"
	.string "We lost…$"

Route207_Text_LaurenPostBattle:
	.string "I think we'll visit the TV station in\n"
	.string "JUBILIFE CITY next.\p"
	.string "Or maybe we should go to the GLOBAL\n"
	.string "TERMINAL?$"

Route207_Text_AustinIntro:
	.string "I wear shorts the whole year!\n"
	.string "I've got energy to burn!$"

Route207_Text_AustinDefeat:
	.string "I couldn't win with just energy…\n"
	.string "Nothing's really that easy!$"

Route207_Text_AustinPostBattle:
	.string "POKéMON don't wear clothes.\n"
	.string "I worry about them getting cold.$"

Route207_Text_JustinIntro:
	.string "I just got through MT. CORONET, but\n"
	.string "I've still got energy to burn!$"

Route207_Text_JustinDefeat:
	.string "Whoops!\n"
	.string "Taken down easily!$"

Route207_Text_JustinPostBattle:
	.string "We need to rest up in OREBURGH CITY\n"
	.string "before heading to MT. CORONET again.$"

Route207_Text_KevinIntro:
	.string "Wahaha!\n"
	.string "Laughter brings good fortune!$"

Route207_Text_KevinDefeat:
	.string "Wahahaha! There's nothing that\n"
	.string "I can do but laugh!$"

Route207_Text_KevinPostBattle:
	.string "Wahahahaha!\n"
	.string "…Go ahead, laugh at me--the loser!$"

Route207_Text_HelenIntro:
	.string "OK, OK!\n"
	.string "Let's be quick about this!$"

Route207_Text_HelenDefeat:
	.string "Oh, yeah? Let's do this again!\n"
	.string "…Oops. I'm out of POKéMON!$"

Route207_Text_HelenPostBattle:
	.string "Yup! Good going!\n"
	.string "I like the way you battle!$"

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
