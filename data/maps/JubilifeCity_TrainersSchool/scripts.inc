@ Jubilife City's TRAINERS' SCHOOL, laid out after Platinum's with Emerald's
@ school tileset.
JubilifeCity_TrainersSchool_MapScripts::
	.byte 0

JubilifeCity_TrainersSchool_EventScript_Blackboard::
	lockall
	msgbox JubilifeCity_TrainersSchool_Text_BlackboardListsStatusChanges, MSGBOX_DEFAULT
	goto JubilifeCity_TrainersSchool_EventScript_ChooseBlackboardTopic
	end

JubilifeCity_TrainersSchool_EventScript_ChooseBlackboardTopic::
	message JubilifeCity_TrainersSchool_Text_ReadWhichTopic
	waitmessage
	multichoicegrid 8, 1, MULTI_STATUS_INFO, 3, FALSE
	switch VAR_RESULT
	case 0, JubilifeCity_TrainersSchool_EventScript_Poison
	case 1, JubilifeCity_TrainersSchool_EventScript_Paralysis
	case 2, JubilifeCity_TrainersSchool_EventScript_Sleep
	case 3, JubilifeCity_TrainersSchool_EventScript_Burn
	case 4, JubilifeCity_TrainersSchool_EventScript_Freeze
	case 5, JubilifeCity_TrainersSchool_EventScript_ExitTopicSelect
	case MULTI_B_PRESSED, JubilifeCity_TrainersSchool_EventScript_ExitTopicSelect
	end

JubilifeCity_TrainersSchool_EventScript_Poison::
	msgbox JubilifeCity_TrainersSchool_Text_ExplainPoison, MSGBOX_DEFAULT
	goto JubilifeCity_TrainersSchool_EventScript_ChooseBlackboardTopic
	end

JubilifeCity_TrainersSchool_EventScript_Paralysis::
	msgbox JubilifeCity_TrainersSchool_Text_ExplainParalysis, MSGBOX_DEFAULT
	goto JubilifeCity_TrainersSchool_EventScript_ChooseBlackboardTopic
	end

JubilifeCity_TrainersSchool_EventScript_Sleep::
	msgbox JubilifeCity_TrainersSchool_Text_ExplainSleep, MSGBOX_DEFAULT
	goto JubilifeCity_TrainersSchool_EventScript_ChooseBlackboardTopic
	end

JubilifeCity_TrainersSchool_EventScript_Burn::
	msgbox JubilifeCity_TrainersSchool_Text_ExplainBurn, MSGBOX_DEFAULT
	goto JubilifeCity_TrainersSchool_EventScript_ChooseBlackboardTopic
	end

JubilifeCity_TrainersSchool_EventScript_Freeze::
	msgbox JubilifeCity_TrainersSchool_Text_ExplainFreeze, MSGBOX_DEFAULT
	goto JubilifeCity_TrainersSchool_EventScript_ChooseBlackboardTopic
	end

JubilifeCity_TrainersSchool_EventScript_ExitTopicSelect::
	releaseall
	end

JubilifeCity_TrainersSchool_EventScript_Teacher::
	msgbox JubilifeCity_TrainersSchool_Text_Teacher, MSGBOX_NPC
	end

JubilifeCity_TrainersSchool_EventScript_StudentLeft::
	msgbox JubilifeCity_TrainersSchool_Text_StudentLeft, MSGBOX_NPC
	end

JubilifeCity_TrainersSchool_EventScript_StudentRight::
	msgbox JubilifeCity_TrainersSchool_Text_StudentRight, MSGBOX_NPC
	end

JubilifeCity_TrainersSchool_EventScript_ComputerStudent::
	msgbox JubilifeCity_TrainersSchool_Text_ComputerStudent, MSGBOX_NPC
	end

JubilifeCity_TrainersSchool_EventScript_StudentNotebook::
	msgbox JubilifeCity_TrainersSchool_Text_StudentsNotes, MSGBOX_SIGN
	end

JubilifeCity_TrainersSchool_EventScript_Computer::
	msgbox JubilifeCity_TrainersSchool_Text_Computer, MSGBOX_SIGN
	end

@ Harrison and Christine use the TRAINER_TED and TRAINER_GEORGIA slots.
@ Beating both earns Harrison's prize: TM10 (Diamond/Pearl) and a POTION
@ (Platinum).
JubilifeCity_TrainersSchool_EventScript_Harrison::
	trainerbattle_single TRAINER_TED, JubilifeCity_TrainersSchool_Text_HarrisonIntro, JubilifeCity_TrainersSchool_Text_HarrisonDefeat, JubilifeCity_TrainersSchool_EventScript_HarrisonPrize
	goto JubilifeCity_TrainersSchool_EventScript_HarrisonPrize

JubilifeCity_TrainersSchool_EventScript_HarrisonPrize::
	lock
	faceplayer
	goto_if_set FLAG_RECEIVED_TRAINERS_SCHOOL_PRIZE, JubilifeCity_TrainersSchool_EventScript_HarrisonAfterPrize
	checktrainerflag TRAINER_GEORGIA
	goto_if_eq VAR_RESULT, FALSE, JubilifeCity_TrainersSchool_EventScript_HarrisonBeatChristine
	msgbox JubilifeCity_TrainersSchool_Text_YouBeatUsBoth, MSGBOX_DEFAULT
	giveitem ITEM_TM10
	goto_if_eq VAR_RESULT, FALSE, Common_EventScript_ShowBagIsFull
	giveitem ITEM_POTION
	goto_if_eq VAR_RESULT, FALSE, Common_EventScript_ShowBagIsFull
	setflag FLAG_RECEIVED_TRAINERS_SCHOOL_PRIZE
	msgbox JubilifeCity_TrainersSchool_Text_ExplainHiddenPower, MSGBOX_DEFAULT
	release
	end

JubilifeCity_TrainersSchool_EventScript_HarrisonBeatChristine::
	msgbox JubilifeCity_TrainersSchool_Text_BattleChristineToo, MSGBOX_DEFAULT
	release
	end

JubilifeCity_TrainersSchool_EventScript_HarrisonAfterPrize::
	msgbox JubilifeCity_TrainersSchool_Text_HarrisonPostBattle, MSGBOX_DEFAULT
	release
	end

JubilifeCity_TrainersSchool_EventScript_Christine::
	trainerbattle_single TRAINER_GEORGIA, JubilifeCity_TrainersSchool_Text_ChristineIntro, JubilifeCity_TrainersSchool_Text_ChristineDefeat
	msgbox JubilifeCity_TrainersSchool_Text_ChristinePostBattle, MSGBOX_AUTOCLOSE
	end

JubilifeCity_TrainersSchool_Text_BlackboardListsStatusChanges:
	.string "The blackboard lists status changes\n"
	.string "that may affect POKéMON in battle.$"

JubilifeCity_TrainersSchool_Text_ReadWhichTopic:
	.string "Which topic do you want to read?$"

JubilifeCity_TrainersSchool_Text_ExplainPoison:
	.string "If a POKéMON is poisoned, it will\n"
	.string "steadily lose HP.\p"
	.string "The effects of poison remain after\n"
	.string "a battle.\p"
	.string "A poisoned POKéMON's HP will drop\n"
	.string "while it is traveling.\p"
	.string "Heal a poisoning using an ANTIDOTE.$"

JubilifeCity_TrainersSchool_Text_ExplainParalysis:
	.string "If a POKéMON becomes paralyzed,\n"
	.string "its SPEED drops.\p"
	.string "It may also not be able to move while\n"
	.string "it is in battle.\p"
	.string "Paralysis remains after battle.\n"
	.string "Cure it using a PARLYZ HEAL.$"

JubilifeCity_TrainersSchool_Text_ExplainSleep:
	.string "If a POKéMON falls asleep, it will be\n"
	.string "unable to attack.\p"
	.string "A POKéMON may wake up on its own,\n"
	.string "but if a battle ends while it is\l"
	.string "sleeping, it will stay asleep.\p"
	.string "Wake it up using an AWAKENING.$"

JubilifeCity_TrainersSchool_Text_ExplainBurn:
	.string "A burn reduces ATTACK power, and it\n"
	.string "steadily reduces the victim's HP.\p"
	.string "A burn lingers after battle.\n"
	.string "Cure a burn using a BURN HEAL.$"

JubilifeCity_TrainersSchool_Text_ExplainFreeze:
	.string "If a POKéMON is frozen, it becomes\n"
	.string "completely helpless.\p"
	.string "It will remain frozen after battle.\n"
	.string "Thaw it out using an ICE HEAL.$"

JubilifeCity_TrainersSchool_Text_StudentsNotes:
	.string "It's this student's notebook…\p"
	.string "POKéMON are to be caught using\n"
	.string "POKé BALLS.\p"
	.string "Up to six POKéMON can accompany\n"
	.string "a TRAINER.\p"
	.string "A TRAINER is someone who catches\n"
	.string "POKéMON, raises them, and battles\l"
	.string "with them.\p"
	.string "A TRAINER's mission is to defeat\n"
	.string "the strong TRAINERS who await\l"
	.string "challengers in POKéMON GYMS.$"

JubilifeCity_TrainersSchool_Text_Teacher:
	.string "Welcome to the TRAINERS' SCHOOL!\p"
	.string "The blackboard explains status\n"
	.string "conditions. Do have a look.\p"
	.string "And if you want to test yourself,\n"
	.string "my two top students are waiting in\l"
	.string "the next room!$"

JubilifeCity_TrainersSchool_Text_StudentLeft:
	.string "A POKéMON that's asleep or frozen\n"
	.string "can't attack.\p"
	.string "I'm writing that down so I don't\n"
	.string "forget it…$"

JubilifeCity_TrainersSchool_Text_StudentRight:
	.string "POKéMON can hold items, but they\n"
	.string "don't know what to do with man-made\l"
	.string "items like POTION and ANTIDOTE.$"

JubilifeCity_TrainersSchool_Text_ComputerStudent:
	.string "I'm studying POKéMON types on the\n"
	.string "computer.\p"
	.string "WATER beats FIRE, FIRE beats GRASS,\n"
	.string "and GRASS beats WATER!$"

JubilifeCity_TrainersSchool_Text_Computer:
	.string "There's a type matchup chart on\n"
	.string "the screen.$"

JubilifeCity_TrainersSchool_Text_HarrisonIntro:
	.string "You want to battle us?\n"
	.string "Let's see what you've learned!$"

JubilifeCity_TrainersSchool_Text_HarrisonDefeat:
	.string "Whoa!\n"
	.string "You're really good!$"

JubilifeCity_TrainersSchool_Text_BattleChristineToo:
	.string "If you can beat CHRISTINE too,\n"
	.string "I'll give you something nice!$"

JubilifeCity_TrainersSchool_Text_YouBeatUsBoth:
	.string "You beat both of us!\n"
	.string "Here, you've earned these!$"

JubilifeCity_TrainersSchool_Text_ExplainHiddenPower:
	.string "TM10 is HIDDEN POWER.\p"
	.string "Its type and power depend on the\n"
	.string "POKéMON that uses it.$"

JubilifeCity_TrainersSchool_Text_HarrisonPostBattle:
	.string "I'm going to keep studying so\n"
	.string "I can beat you next time!$"

JubilifeCity_TrainersSchool_Text_ChristineIntro:
	.string "I've been studying really hard!\n"
	.string "Battle with me!$"

JubilifeCity_TrainersSchool_Text_ChristineDefeat:
	.string "Oh no…\n"
	.string "I studied so hard, too…$"

JubilifeCity_TrainersSchool_Text_ChristinePostBattle:
	.string "I guess studying isn't the same\n"
	.string "as battling for real.$"
