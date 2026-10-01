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

JubilifeCity_TrainersSchool_EventScript_GameboyKid1::
	msgbox JubilifeCity_TrainersSchool_Text_TradingRightNow, MSGBOX_NPC
	end

JubilifeCity_TrainersSchool_EventScript_GameboyKid2::
	msgbox JubilifeCity_TrainersSchool_Text_AlwaysWantedSeedot, MSGBOX_NPC
	end

JubilifeCity_TrainersSchool_EventScript_RichBoy::
	msgbox JubilifeCity_TrainersSchool_Text_PokemontCantUseManMadeItems, MSGBOX_NPC
	end

JubilifeCity_TrainersSchool_EventScript_Lass::
	msgbox JubilifeCity_TrainersSchool_Text_ConfusedPokemonAttacksItself, MSGBOX_NPC
	end

JubilifeCity_TrainersSchool_EventScript_SchoolKidM::
	msgbox JubilifeCity_TrainersSchool_Text_PokemonHealItselfWithBerry, MSGBOX_NPC
	end

JubilifeCity_TrainersSchool_EventScript_StudentNotebook::
	msgbox JubilifeCity_TrainersSchool_Text_StudentsNotes, MSGBOX_SIGN
	end

JubilifeCity_TrainersSchool_EventScript_Teacher::
	lock
	faceplayer
	goto_if_set FLAG_RECEIVED_QUICK_CLAW, JubilifeCity_TrainersSchool_EventScript_GaveQuickClaw
	call_if_eq VAR_FACING, DIR_EAST, JubilifeCity_TrainersSchool_EventScript_TeacherCheckOnStudentsEast
	call_if_eq VAR_FACING, DIR_WEST, JubilifeCity_TrainersSchool_EventScript_TeacherCheckOnStudentsWest
	msgbox JubilifeCity_TrainersSchool_Text_StudentsWhoDontStudyGetQuickClaw, MSGBOX_DEFAULT
	giveitem ITEM_QUICK_CLAW
	goto_if_eq VAR_RESULT, 0, Common_EventScript_ShowBagIsFull
	closemessage
	applywaitmovement VAR_LAST_TALKED, Common_Movement_WalkInPlaceFasterDown
	setflag FLAG_RECEIVED_QUICK_CLAW
	release
	end

JubilifeCity_TrainersSchool_EventScript_TeacherCheckOnStudentsEast::
	applywaitmovement VAR_LAST_TALKED, JubilifeCity_TrainersSchool_Movement_TeacherCheckOnStudentsEast
	return

JubilifeCity_TrainersSchool_EventScript_TeacherCheckOnStudentsWest::
	applywaitmovement VAR_LAST_TALKED, JubilifeCity_TrainersSchool_Movement_TeacherCheckOnStudentsWest
	return

JubilifeCity_TrainersSchool_EventScript_GaveQuickClaw::
	msgbox JubilifeCity_TrainersSchool_Text_ExplainQuickClaw, MSGBOX_DEFAULT
	closemessage
	applywaitmovement VAR_LAST_TALKED, Common_Movement_WalkInPlaceFasterDown
	release
	end

JubilifeCity_TrainersSchool_Movement_TeacherCheckOnStudentsWest:
	walk_left
	walk_down
	walk_down
	walk_right
	walk_in_place_faster_down
	delay_16
	delay_16
	delay_16
	walk_down
	walk_in_place_faster_left
	delay_16
	delay_16
	walk_right
	delay_16
	delay_16
	delay_8
	walk_up
	walk_left
	walk_left
	walk_up
	walk_up
	walk_right
	step_end

JubilifeCity_TrainersSchool_Movement_TeacherCheckOnStudentsEast:
	walk_right
	walk_right
	walk_down
	walk_down
	walk_left
	walk_left
	walk_in_place_faster_down
	delay_16
	delay_16
	delay_16
	walk_down
	walk_in_place_faster_left
	delay_16
	delay_16
	walk_right
	delay_16
	delay_16
	delay_8
	walk_up
	walk_right
	walk_up
	walk_up
	walk_left
	walk_left
	step_end

JubilifeCity_TrainersSchool_EventScript_Scott::
	lock
	faceplayer
	goto_if_set FLAG_MET_SCOTT_AFTER_OBTAINING_STONE_BADGE, JubilifeCity_TrainersSchool_EventScript_ScottWatchStudents
	goto_if_set FLAG_MET_SCOTT_JUBILIFE, JubilifeCity_TrainersSchool_EventScript_ScottSpokeAlready
	goto_if_set FLAG_BADGE01_GET, JubilifeCity_TrainersSchool_EventScript_ScottGreetHasBadge
	msgbox JubilifeCity_TrainersSchool_Text_ScottMetAlreadyCut, MSGBOX_DEFAULT
	addvar VAR_SCOTT_STATE, 1
	setflag FLAG_MET_SCOTT_JUBILIFE
	release
	end

JubilifeCity_TrainersSchool_EventScript_ScottSpokeAlready::
	goto_if_set FLAG_BADGE01_GET, JubilifeCity_TrainersSchool_EventScript_ScottNoticeBadge
	msgbox JubilifeCity_TrainersSchool_Text_StudentTalentLevelUnknown, MSGBOX_DEFAULT
	release
	end

JubilifeCity_TrainersSchool_EventScript_ScottGreetHasBadge::
	msgbox JubilifeCity_TrainersSchool_Text_ScottMetAlreadyStoneBadge, MSGBOX_DEFAULT
	goto JubilifeCity_TrainersSchool_EventScript_MetScottAfterBadge
	end

JubilifeCity_TrainersSchool_EventScript_ScottNoticeBadge::
	msgbox JubilifeCity_TrainersSchool_Text_ScottStoneBadge, MSGBOX_DEFAULT
	goto JubilifeCity_TrainersSchool_EventScript_MetScottAfterBadge
	end

JubilifeCity_TrainersSchool_EventScript_MetScottAfterBadge::
	addvar VAR_SCOTT_STATE, 1
	setflag FLAG_MET_SCOTT_AFTER_OBTAINING_STONE_BADGE
	release
	end

JubilifeCity_TrainersSchool_EventScript_ScottWatchStudents::
	msgbox JubilifeCity_TrainersSchool_Text_StudentTalentLevelUnknown, MSGBOX_DEFAULT
	release
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

JubilifeCity_TrainersSchool_Text_StudentsWhoDontStudyGetQuickClaw:
	.string "Students who don't study get a little\n"
	.string "taste of my QUICK CLAW.\p"
	.string "Whether or not you are a good student \n"
	.string "will be evident from the way you use\l"
	.string "this item.$"

JubilifeCity_TrainersSchool_Text_ExplainQuickClaw:
	.string "A POKéMON holding the QUICK CLAW will\n"
	.string "occasionally speed up and get to move\l"
	.string "before its opponent.\p"
	.string "There are many other items that are\n"
	.string "meant to be held by POKéMON.\p"
	.string "Just those alone will give you many\n"
	.string "topics to study!$"

JubilifeCity_TrainersSchool_Text_TradingRightNow:
	.string "I'm trading POKéMON with my friend\n"
	.string "right now.$"

JubilifeCity_TrainersSchool_Text_AlwaysWantedSeedot:
	.string "I always wanted a SEEDOT, and\n"
	.string "I'm finally getting one!$"

JubilifeCity_TrainersSchool_Text_PokemontCantUseManMadeItems:
	.string "POKéMON can hold items, but they\n"
	.string "don't know what to do with man-made\l"
	.string "items like POTION and ANTIDOTE.$"

JubilifeCity_TrainersSchool_Text_ConfusedPokemonAttacksItself:
	.string "You know how some POKéMON moves can\n"
	.string "confuse a POKéMON?\p"
	.string "A confused POKéMON will sometimes\n"
	.string "attack itself without meaning to.\p"
	.string "But once it leaves battle, it will\n"
	.string "return to normal.$"

JubilifeCity_TrainersSchool_Text_PokemonHealItselfWithBerry:
	.string "A POKéMON holding a BERRY will heal\n"
	.string "itself…\p"
	.string "There are many kinds of items that\n"
	.string "POKéMON can hold…\p"
	.string "Boy, it sure is hard taking notes\n"
	.string "down…$"

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

JubilifeCity_TrainersSchool_Text_ScottMetAlreadyCut:
	.string "Hello? Didn't we meet before?\n"
	.string "I think back in PETALBURG CITY.\p"
	.string "Let me introduce myself.\n"
	.string "My name's SCOTT.\p"
	.string "I've been traveling everywhere in\n"
	.string "search of outstanding TRAINERS.\p"
	.string "More specifically, I'm looking for\n"
	.string "POKéMON battle experts.\p"
	.string "So, what brings you to this SCHOOL?\n"
	.string "Are you a TRAINER, too?\p"
	.string "The first thing you should do is to\n"
	.string "have a POKéMON learn the move CUT.\p"
	.string "If I remember correctly, someone in\n"
	.string "this town has CUT.$"

JubilifeCity_TrainersSchool_Text_StudentTalentLevelUnknown:
	.string "SCOTT: Hmm…\p"
	.string "The talent levels of the students here\n"
	.string "are unknown. The potential's there.$"

JubilifeCity_TrainersSchool_Text_ScottStoneBadge:
	.string "SCOTT: Oh, what's that?\p"
	.string "It's a STONE BADGE, isn't it?\n"
	.string "That's pretty impressive, I'd say.\p"
	.string "But, you know, I would've loved to see\n"
	.string "you in battle.\p"
	.string "It's hard to tell what you're like as\n"
	.string "a TRAINER from a LEAGUE BADGE.$"

JubilifeCity_TrainersSchool_Text_ScottMetAlreadyStoneBadge:
	.string "Hello? Didn't we meet before?\n"
	.string "I think back in PETALBURG CITY.\p"
	.string "Let me introduce myself.\n"
	.string "My name's SCOTT.\p"
	.string "I've been traveling everywhere in\n"
	.string "search of outstanding TRAINERS.\p"
	.string "More specifically, I'm looking for\n"
	.string "POKéMON battle experts.\p"
	.string "Oh, what's that?\n"
	.string "It's a STONE BADGE, isn't it?\p"
	.string "That's pretty impressive, I'd say, for\n"
	.string "a starting TRAINER like yourself.\p"
	.string "But, you know, I would've loved to see\n"
	.string "you in battle.\p"
	.string "It's hard to tell what you're like as\n"
	.string "a TRAINER from a LEAGUE BADGE.$"

