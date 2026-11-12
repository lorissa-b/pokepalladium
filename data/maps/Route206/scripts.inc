@ Route 206, Cycling Road south of Eterna City. Drawn over Emerald's Route
@ 110 from Platinum's map. The cycling road is a bridge at elevation 4,
@ entered only through the north gate (from Eterna City) and the south gate;
@ its deck is multi-level, so the ground below can be walked under it as in
@ Platinum. The south end joins Route 207.

Route206_MapScripts::
	.byte 0

Route206_EventScript_Axel::
	trainerbattle_single TRAINER_JACOB, Route206_Text_AxelIntro, Route206_Text_AxelDefeat
	msgbox Route206_Text_AxelPostBattle, MSGBOX_AUTOCLOSE
	end

Route206_EventScript_Megan::
	trainerbattle_single TRAINER_JASMINE, Route206_Text_MeganIntro, Route206_Text_MeganDefeat
	msgbox Route206_Text_MeganPostBattle, MSGBOX_AUTOCLOSE
	end

Route206_EventScript_James::
	trainerbattle_single TRAINER_ANTHONY, Route206_Text_JamesIntro, Route206_Text_JamesDefeat
	msgbox Route206_Text_JamesPostBattle, MSGBOX_AUTOCLOSE
	end

Route206_EventScript_Nicole::
	trainerbattle_single TRAINER_ALYSSA, Route206_Text_NicoleIntro, Route206_Text_NicoleDefeat
	msgbox Route206_Text_NicolePostBattle, MSGBOX_AUTOCLOSE
	end

Route206_EventScript_John::
	trainerbattle_single TRAINER_JOSEPH, Route206_Text_JohnIntro, Route206_Text_JohnDefeat
	msgbox Route206_Text_JohnPostBattle, MSGBOX_AUTOCLOSE
	end

Route206_EventScript_Kayla::
	trainerbattle_single TRAINER_JACLYN, Route206_Text_KaylaIntro, Route206_Text_KaylaDefeat
	msgbox Route206_Text_KaylaPostBattle, MSGBOX_AUTOCLOSE
	end

Route206_EventScript_Ryan::
	trainerbattle_single TRAINER_TIMMY, Route206_Text_RyanIntro, Route206_Text_RyanDefeat
	msgbox Route206_Text_RyanPostBattle, MSGBOX_AUTOCLOSE
	end

Route206_EventScript_Rachel::
	trainerbattle_single TRAINER_EDWARD, Route206_Text_RachelIntro, Route206_Text_RachelDefeat
	msgbox Route206_Text_RachelPostBattle, MSGBOX_AUTOCLOSE
	end

Route206_EventScript_Theodore::
	trainerbattle_single TRAINER_DALE, Route206_Text_TheodoreIntro, Route206_Text_TheodoreDefeat
	msgbox Route206_Text_TheodorePostBattle, MSGBOX_AUTOCLOSE
	end

Route206_EventScript_Hiker::
	msgbox Route206_Text_TwoCavesOnRoute206, MSGBOX_NPC
	end

Route206_EventScript_SignEternaCity::
	msgbox Route206_Text_SignEternaCity, MSGBOX_SIGN
	end

Route206_EventScript_SignOreburghCity::
	msgbox Route206_Text_SignOreburghCity, MSGBOX_SIGN
	end

Route206_Text_TwoCavesOnRoute206:
	.string "This is odd… I heard there were two\n"
	.string "caves on ROUTE 206.\p"
	.string "But I can only find one entrance.$"

Route206_Text_SignEternaCity:
	.string "ROUTE 206\n"
	.string "{UP_ARROW} ETERNA CITY$"

Route206_Text_SignOreburghCity:
	.string "ROUTE 206\n"
	.string "{DOWN_ARROW} OREBURGH CITY$"
