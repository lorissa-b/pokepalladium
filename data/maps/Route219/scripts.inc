@ Route 219 (Sandgem Beach), south of Sandgem Town. Drawn over Emerald's
@ Route 102.
Route219_MapScripts::
	.byte 0

Route219_EventScript_RouteSign::
	msgbox Route219_Text_RouteSign, MSGBOX_SIGN
	end

Route219_EventScript_Trenton::
	trainerbattle_single TRAINER_ALLEN, Route219_Text_TrentonIntro, Route219_Text_TrentonDefeated
	msgbox Route219_Text_TrentonPostBattle, MSGBOX_AUTOCLOSE
	end

Route219_EventScript_Mariel::
	trainerbattle_single TRAINER_TIANA, Route219_Text_MarielIntro, Route219_Text_MarielDefeated
	msgbox Route219_Text_MarielPostBattle, MSGBOX_AUTOCLOSE
	end

Route219_Text_RouteSign:
	.string "ROUTE 219\n"
	.string "SANDGEM BEACH$"
