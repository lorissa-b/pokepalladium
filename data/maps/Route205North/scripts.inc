@ Route 205 North, between Eterna Forest's east exit and Eterna City. Drawn
@ over Emerald's Route 113 from Platinum's map. The arrow warps on the west
@ edge (3, 12-13) lead into Eterna Forest's east exit, and the east edge
@ joins Eterna City.
@
@ Platinum's three Fishermen trainers stand on the bridges with sight 0, as
@ in Platinum: they only battle when spoken to.

Route205North_MapScripts::
	.byte 0

Route205North_EventScript_Andrew::
	trainerbattle_single TRAINER_JAYLEN, Route205North_Text_AndrewIntro, Route205North_Text_AndrewDefeat
	msgbox Route205North_Text_AndrewPostBattle, MSGBOX_AUTOCLOSE
	end

Route205North_EventScript_Joseph::
	trainerbattle_single TRAINER_DILLON, Route205North_Text_JosephIntro, Route205North_Text_JosephDefeat
	msgbox Route205North_Text_JosephPostBattle, MSGBOX_AUTOCLOSE
	end

Route205North_EventScript_Zachary::
	trainerbattle_single TRAINER_LUNG, Route205North_Text_ZacharyIntro, Route205North_Text_ZacharyDefeat
	msgbox Route205North_Text_ZacharyPostBattle, MSGBOX_AUTOCLOSE
	end

Route205North_EventScript_Fisherman::
	msgbox Route205North_Text_FishCasually, MSGBOX_NPC
	end

Route205North_EventScript_SignEternaForest::
	msgbox Route205North_Text_SignEternaForest, MSGBOX_SIGN
	end

Route205North_EventScript_SignEternaCity::
	msgbox Route205North_Text_SignEternaCity, MSGBOX_SIGN
	end

Route205North_Text_FishCasually:
	.string "To sit back and fish casually…\n"
	.string "It's a grown-up's hobby.$"

Route205North_Text_SignEternaForest:
	.string "ROUTE 205\n"
	.string "{LEFT_ARROW} ETERNA FOREST$"

Route205North_Text_SignEternaCity:
	.string "ROUTE 205\n"
	.string "{RIGHT_ARROW} ETERNA CITY$"
