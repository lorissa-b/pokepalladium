@ Route 211 West, between Eterna City's east exit and Mt. Coronet. Drawn
@ over Emerald's Route 118 from Platinum's map. The west edge joins Eterna
@ City; the cave mouth at (28,20) is Mt. Coronet's entrance, which leads to
@ Fiery Path as a stand-in until Mt. Coronet exists.

Route211West_MapScripts::
	.byte 0

Route211West_EventScript_Alexandra::
	trainerbattle_single TRAINER_PERRY, Route211West_Text_AlexandraIntro, Route211West_Text_AlexandraDefeat
	msgbox Route211West_Text_AlexandraPostBattle, MSGBOX_AUTOCLOSE
	end

Route211West_EventScript_Louis::
	trainerbattle_single TRAINER_CHESTER, Route211West_Text_LouisIntro, Route211West_Text_LouisDefeat
	msgbox Route211West_Text_LouisPostBattle, MSGBOX_AUTOCLOSE
	end

Route211West_EventScript_Zach::
	trainerbattle_single TRAINER_DEANDRE, Route211West_Text_ZachIntro, Route211West_Text_ZachDefeat
	msgbox Route211West_Text_ZachPostBattle, MSGBOX_AUTOCLOSE
	end

Route211West_EventScript_SignMtCoronet::
	msgbox Route211West_Text_SignMtCoronet, MSGBOX_SIGN
	end

Route211West_EventScript_SignEternaCity::
	msgbox Route211West_Text_SignEternaCity, MSGBOX_SIGN
	end

Route211West_Text_SignMtCoronet:
	.string "ROUTE 211\n"
	.string "{RIGHT_ARROW} MT. CORONET$"

Route211West_Text_SignEternaCity:
	.string "ROUTE 211\n"
	.string "{LEFT_ARROW} ETERNA CITY$"
