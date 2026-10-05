@ Route 203, east of Jubilife City toward Oreburgh City. Platinum's route
@ (rows 4-28 of its 64x32 chunk) drafted with mapkit on Jubilife's tilesets
@ (General and Rustboro), so the shared edge draws the same from both sides.
@ The entrance is widened to rows 15-20 to meet all six lanes of Jubilife's
@ east road. Oreburgh Gate is the cave outcrop in the east trees, its mouth
@ at (58,8) since Emerald has no west-facing cave mouth. Platinum's five trainers stand at its spots with its
@ teams, and the rival waits by the entrance until he's beaten.
Route203_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, Route203_OnTransition
	.byte 0

	@ VAR_ROUTE203_STATE: 0 until the rival battle by the entrance is won, then 1.
	@ The rival is hidden by FLAG_DEFEATED_RIVAL_ROUTE_203, set when he leaves.

Route203_OnTransition:
	call Common_EventScript_SetupRivalGfxId
	end

Route203_EventScript_Lass::
	msgbox Route203_Text_FavoritePokemonAtTheTop, MSGBOX_NPC
	end

Route203_EventScript_SignJubilifeCity::
	msgbox Route203_Text_SignJubilifeCity, MSGBOX_SIGN
	end

Route203_EventScript_SignOreburghCity::
	msgbox Route203_Text_SignOreburghCity, MSGBOX_SIGN
	end

Route203_EventScript_TrainerTipsWest::
	msgbox Route203_Text_TrainerTipsMovesUsePP, MSGBOX_SIGN
	end

Route203_EventScript_TrainerTipsEast::
	msgbox Route203_Text_TrainerTipsEvolveCanBeStopped, MSGBOX_SIGN
	end

Route203_EventScript_Sebastian::
	trainerbattle_single TRAINER_BILLY, Route203_Text_SebastianIntro, Route203_Text_SebastianDefeated
	msgbox Route203_Text_SebastianPostBattle, MSGBOX_AUTOCLOSE
	end

Route203_EventScript_Michael::
	trainerbattle_single TRAINER_JOHNSON, Route203_Text_MichaelIntro, Route203_Text_MichaelDefeated
	msgbox Route203_Text_MichaelPostBattle, MSGBOX_AUTOCLOSE
	end

Route203_EventScript_Kaitlin::
	trainerbattle_single TRAINER_IVAN, Route203_Text_KaitlinIntro, Route203_Text_KaitlinDefeated
	msgbox Route203_Text_KaitlinPostBattle, MSGBOX_AUTOCLOSE
	end

Route203_EventScript_Dallas::
	trainerbattle_single TRAINER_JOSE, Route203_Text_DallasIntro, Route203_Text_DallasDefeated
	msgbox Route203_Text_DallasPostBattle, MSGBOX_AUTOCLOSE
	end

Route203_EventScript_Madeline::
	trainerbattle_single TRAINER_DARIAN, Route203_Text_MadelineIntro, Route203_Text_MadelineDefeated
	msgbox Route203_Text_MadelinePostBattle, MSGBOX_AUTOCLOSE
	end

@ Stepping onto the route from Jubilife (x 4, y 15-20). The rival, waiting at
@ (9,17), comes over to the player and battles. Losing whites out as usual.
Route203_EventScript_RivalTrigger::
	lockall
	applywaitmovement LOCALID_ROUTE203_RIVAL, Route203_Movement_RivalNoticePlayer
	getplayerxy VAR_0x8004, VAR_0x8005
	switch VAR_0x8005
	case 15, Route203_EventScript_RivalApproachY15
	case 16, Route203_EventScript_RivalApproachY16
	case 17, Route203_EventScript_RivalApproachY17
	case 18, Route203_EventScript_RivalApproachY18
	case 19, Route203_EventScript_RivalApproachY19
	case 20, Route203_EventScript_RivalApproachY20
	releaseall
	end

Route203_EventScript_RivalApproachY15::
	applywaitmovement LOCALID_ROUTE203_RIVAL, Route203_Movement_RivalApproachY15
	goto Route203_EventScript_RivalBattle

Route203_EventScript_RivalApproachY16::
	applywaitmovement LOCALID_ROUTE203_RIVAL, Route203_Movement_RivalApproachY16
	goto Route203_EventScript_RivalBattle

Route203_EventScript_RivalApproachY17::
	applywaitmovement LOCALID_ROUTE203_RIVAL, Route203_Movement_RivalApproachY17
	goto Route203_EventScript_RivalBattle

Route203_EventScript_RivalApproachY18::
	applywaitmovement LOCALID_ROUTE203_RIVAL, Route203_Movement_RivalApproachY18
	goto Route203_EventScript_RivalBattle

Route203_EventScript_RivalApproachY19::
	applywaitmovement LOCALID_ROUTE203_RIVAL, Route203_Movement_RivalApproachY19
	goto Route203_EventScript_RivalBattle

Route203_EventScript_RivalApproachY20::
	applywaitmovement LOCALID_ROUTE203_RIVAL, Route203_Movement_RivalApproachY20
	goto Route203_EventScript_RivalBattle

@ The ROUTE_203 trainers are named after the player's starter, like Route 202's:
@ the rival has the starter that's strong against it.
Route203_EventScript_RivalBattle::
	checkplayergender
	goto_if_eq VAR_RESULT, FEMALE, Route203_EventScript_BrendanBattle
	playbgm MUS_ENCOUNTER_MAY, TRUE
	msgbox Route203_Text_RivalGotTougher, MSGBOX_DEFAULT
	closemessage
	switch VAR_STARTER_MON
	case 0, Route203_EventScript_MayBattleTurtwig
	case 1, Route203_EventScript_MayBattleChimchar
	case 2, Route203_EventScript_MayBattlePiplup
	end

Route203_EventScript_BrendanBattle::
	playbgm MUS_ENCOUNTER_BRENDAN, TRUE
	msgbox Route203_Text_RivalGotTougher, MSGBOX_DEFAULT
	closemessage
	switch VAR_STARTER_MON
	case 0, Route203_EventScript_BrendanBattleTurtwig
	case 1, Route203_EventScript_BrendanBattleChimchar
	case 2, Route203_EventScript_BrendanBattlePiplup
	end

Route203_EventScript_MayBattleTurtwig::
	trainerbattle_no_intro TRAINER_MAY_ROUTE_203_TREECKO, Route203_Text_RivalDefeated
	goto Route203_EventScript_RivalAfterBattle

Route203_EventScript_MayBattleChimchar::
	trainerbattle_no_intro TRAINER_MAY_ROUTE_203_TORCHIC, Route203_Text_RivalDefeated
	goto Route203_EventScript_RivalAfterBattle

Route203_EventScript_MayBattlePiplup::
	trainerbattle_no_intro TRAINER_MAY_ROUTE_203_MUDKIP, Route203_Text_RivalDefeatedVsPiplup
	goto Route203_EventScript_RivalAfterBattle

Route203_EventScript_BrendanBattleTurtwig::
	trainerbattle_no_intro TRAINER_BRENDAN_ROUTE_203_TREECKO, Route203_Text_RivalDefeated
	goto Route203_EventScript_RivalAfterBattle

Route203_EventScript_BrendanBattleChimchar::
	trainerbattle_no_intro TRAINER_BRENDAN_ROUTE_203_TORCHIC, Route203_Text_RivalDefeated
	goto Route203_EventScript_RivalAfterBattle

Route203_EventScript_BrendanBattlePiplup::
	trainerbattle_no_intro TRAINER_BRENDAN_ROUTE_203_MUDKIP, Route203_Text_RivalDefeatedVsPiplup
	goto Route203_EventScript_RivalAfterBattle

@ He runs east off screen. From row 16 he goes up a row first, around the sign.
Route203_EventScript_RivalAfterBattle::
	msgbox Route203_Text_RivalWhatDoYouMeanILost, MSGBOX_DEFAULT
	closemessage
	getplayerxy VAR_0x8004, VAR_0x8005
	call_if_eq VAR_0x8005, 16, Route203_EventScript_RivalLeaveAroundSign
	call_if_ne VAR_0x8005, 16, Route203_EventScript_RivalLeave
	removeobject LOCALID_ROUTE203_RIVAL
	setvar VAR_ROUTE203_STATE, 1
	releaseall
	end

Route203_EventScript_RivalLeave::
	applywaitmovement LOCALID_ROUTE203_RIVAL, Route203_Movement_RivalLeave
	return

Route203_EventScript_RivalLeaveAroundSign::
	applywaitmovement LOCALID_ROUTE203_RIVAL, Route203_Movement_RivalLeaveAroundSign
	return

Route203_Movement_RivalNoticePlayer:
	delay_8
	emote_exclamation_mark
	delay_8
	step_end

@ From (9,17) to beside the player at (5,y), facing them
Route203_Movement_RivalApproachY15:
	walk_fast_left
	walk_fast_left
	walk_fast_up
	walk_fast_up
	walk_fast_left
	walk_fast_left
	step_end

Route203_Movement_RivalApproachY16:
	walk_fast_left
	walk_fast_left
	walk_fast_up
	walk_fast_left
	walk_fast_left
	step_end

Route203_Movement_RivalApproachY17:
	walk_fast_left
	walk_fast_left
	walk_fast_left
	walk_fast_left
	step_end

Route203_Movement_RivalApproachY18:
	walk_fast_left
	walk_fast_left
	walk_fast_down
	walk_fast_left
	walk_fast_left
	step_end

Route203_Movement_RivalApproachY19:
	walk_fast_left
	walk_fast_left
	walk_fast_down
	walk_fast_down
	walk_fast_left
	walk_fast_left
	step_end

Route203_Movement_RivalApproachY20:
	walk_fast_left
	walk_fast_left
	walk_fast_down
	walk_fast_down
	walk_fast_down
	walk_fast_left
	walk_fast_left
	step_end

@ Seven tiles east is off screen, short of the Lass who wanders around (14,17)
Route203_Movement_RivalLeave:
	walk_fast_right
	walk_fast_right
	walk_fast_right
	walk_fast_right
	walk_fast_right
	walk_fast_right
	walk_fast_right
	step_end

Route203_Movement_RivalLeaveAroundSign:
	walk_fast_up
	walk_fast_right
	walk_fast_right
	walk_fast_right
	walk_fast_right
	walk_fast_right
	walk_fast_right
	walk_fast_right
	step_end

Route203_Text_FavoritePokemonAtTheTop:
	.string "Your favorite POKéMON should be placed\n"
	.string "at the top of your party list.\p"
	.string "That way, it will be the first to go\n"
	.string "out in a battle.$"

Route203_Text_SignJubilifeCity:
	.string "ROUTE 203\n"
	.string "{LEFT_ARROW} JUBILIFE CITY$"

Route203_Text_SignOreburghCity:
	.string "ROUTE 203\n"
	.string "{RIGHT_ARROW} OREBURGH CITY$"

Route203_Text_TrainerTipsMovesUsePP:
	.string "TRAINER TIPS\p"
	.string "POKéMON moves use energy called\n"
	.string "POWER POINTS, or PP for short.\p"
	.string "If the PP of moves runs low, head for\n"
	.string "any POKéMON CENTER.$"

Route203_Text_TrainerTipsEvolveCanBeStopped:
	.string "TRAINER TIPS\p"
	.string "If a POKéMON is about to evolve,\n"
	.string "it can be instantly stopped.\p"
	.string "Press the B Button during evolution\n"
	.string "to startle the POKéMON into stopping.\p"
	.string "That is how evolution can be\n"
	.string "canceled.$"

Route203_Text_SebastianIntro:
	.string "Before I challenge the POKéMON\n"
	.string "GYM LEADER, I'll test my strength\l"
	.string "on you!$"

Route203_Text_SebastianDefeated:
	.string "I guess it means I'm not ready for\n"
	.string "the GYM challenge yet…$"

Route203_Text_SebastianPostBattle:
	.string "I guess if you don't have a single\n"
	.string "GYM BADGE, you're a total nobody…$"

Route203_Text_MichaelIntro:
	.string "I get my get-up-and-go from my\n"
	.string "POKéMON!\p"
	.string "Here we go!\n"
	.string "KRICKETOT! ZUBAT!$"

Route203_Text_MichaelDefeated:
	.string "Ack! My POKéMON…$"

Route203_Text_MichaelPostBattle:
	.string "Keep out of the grass or dodge the\n"
	.string "eyes of TRAINERS…?\p"
	.string "I always have a tough time\n"
	.string "choosing.$"

Route203_Text_KaitlinIntro:
	.string "I found a TRAINER while looking for\n"
	.string "some POKéMON!$"

Route203_Text_KaitlinDefeated:
	.string "Hunh? You're finished so soon?$"

Route203_Text_KaitlinPostBattle:
	.string "I walk through tall grass until I\n"
	.string "find the POKéMON I want.$"

Route203_Text_DallasIntro:
	.string "I'm never apart from my SHINX.\n"
	.string "Our bond is strong!$"

Route203_Text_DallasDefeated:
	.string "So sad…\n"
	.string "And so crushed…$"

Route203_Text_DallasPostBattle:
	.string "I don't bear grudges over losing.\n"
	.string "But I promise I'll get tougher!$"

Route203_Text_MadelineIntro:
	.string "I got a nifty-keen GYM BADGE from\n"
	.string "the GYM LEADER in OREBURGH CITY!$"

Route203_Text_MadelineDefeated:
	.string "Oh, I lost! Lalala!$"

Route203_Text_MadelinePostBattle:
	.string "I'm collecting lots of GYM BADGES.\n"
	.string "They're so sparkly and pretty!$"

Route203_Text_RivalGotTougher:
	.string "{RIVAL}: Hey! {PLAYER}!\n"
	.string "Tell me you got a little tougher!\p"
	.string "Me? Do you even need to ask?\n"
	.string "Of course I got tougher!\p"
	.string "Come on!\n"
	.string "I'll demonstrate it to you!$"

Route203_Text_RivalDefeated:
	.string "What just happened?\n"
	.string "I lost?!$"

Route203_Text_RivalDefeatedVsPiplup:
	.string "What's going on here?\n"
	.string "What, I lost?!$"

Route203_Text_RivalWhatDoYouMeanILost:
	.string "{RIVAL}: Waah!\n"
	.string "What do you mean I lost?!\p"
	.string "Well, that's it!\n"
	.string "That's the last time I'll ever lose!\p"
	.string "I'm going to be the world's toughest\n"
	.string "TRAINER, and you know it!\p"
	.string "The first thing to do is take on\n"
	.string "the OREBURGH CITY POKéMON GYM!\p"
	.string "I'm gonna toughen up for that,\n"
	.string "totally!$"
