@ Oreburgh Gym, from Platinum's: Roark on his stand at the back, Youngsters
@ Darius and Jonathon on the way up, the Gym Guide by the door and the two
@ statues listing certified Trainers. Roark is TRAINER_ROXANNE_1's slot,
@ Jonathon TRAINER_JOSH's and Darius TRAINER_TOMMY's (Common_EventScript_SetGymTrainers
@ case 1 sets all three Rustboro Gym trainer flags, as Platinum sets the
@ two Youngsters' when the Badge is won).

OreburghCity_Gym_MapScripts::
	.byte 0

OreburghCity_Gym_EventScript_Roark::
	trainerbattle_single TRAINER_ROXANNE_1, OreburghCity_Gym_Text_RoarkIntro, OreburghCity_Gym_Text_RoarkDefeat, OreburghCity_Gym_EventScript_RoarkDefeated, NO_MUSIC
	goto_if_unset FLAG_RECEIVED_TM_STEALTH_ROCK, OreburghCity_Gym_EventScript_GiveStealthRock
	msgbox OreburghCity_Gym_Text_RoarkPostBattle, MSGBOX_DEFAULT
	release
	end

OreburghCity_Gym_EventScript_RoarkDefeated::
	msgbox OreburghCity_Gym_Text_BeatRoark, MSGBOX_DEFAULT
	message OreburghCity_Gym_Text_ReceivedCoalBadge
	waitmessage
	call Common_EventScript_PlayGymBadgeFanfare
	setflag FLAG_BADGE01_GET
	setvar VAR_0x8008, 1
	call Common_EventScript_SetGymTrainers
	msgbox OreburghCity_Gym_Text_CoalBadgeInfoTakeThis, MSGBOX_DEFAULT
	goto OreburghCity_Gym_EventScript_GiveStealthRock
	end

OreburghCity_Gym_EventScript_GiveStealthRock::
	giveitem ITEM_TM_STEALTH_ROCK
	goto_if_eq VAR_RESULT, FALSE, Common_EventScript_ShowBagIsFull
	setflag FLAG_RECEIVED_TM_STEALTH_ROCK
	msgbox OreburghCity_Gym_Text_ExplainStealthRock, MSGBOX_DEFAULT
	release
	end

OreburghCity_Gym_EventScript_Jonathon::
	trainerbattle_single TRAINER_JOSH, OreburghCity_Gym_Text_JonathonIntro, OreburghCity_Gym_Text_JonathonDefeat
	msgbox OreburghCity_Gym_Text_JonathonPostBattle, MSGBOX_AUTOCLOSE
	end

OreburghCity_Gym_EventScript_Darius::
	trainerbattle_single TRAINER_TOMMY, OreburghCity_Gym_Text_DariusIntro, OreburghCity_Gym_Text_DariusDefeat
	msgbox OreburghCity_Gym_Text_DariusPostBattle, MSGBOX_AUTOCLOSE
	end

OreburghCity_Gym_EventScript_GymGuide::
	lock
	faceplayer
	goto_if_set FLAG_BADGE01_GET, OreburghCity_Gym_EventScript_GymGuidePostVictory
	msgbox OreburghCity_Gym_Text_GymGuideAdvice, MSGBOX_DEFAULT
	release
	end

OreburghCity_Gym_EventScript_GymGuidePostVictory::
	msgbox OreburghCity_Gym_Text_GymGuidePostVictory, MSGBOX_DEFAULT
	release
	end

OreburghCity_Gym_EventScript_GymStatue::
	lockall
	goto_if_set FLAG_BADGE01_GET, OreburghCity_Gym_EventScript_GymStatueCertified
	msgbox OreburghCity_Gym_Text_GymStatue, MSGBOX_DEFAULT
	releaseall
	end

OreburghCity_Gym_EventScript_GymStatueCertified::
	msgbox OreburghCity_Gym_Text_GymStatueCertified, MSGBOX_DEFAULT
	releaseall
	end

OreburghCity_Gym_Text_RoarkIntro:
	.string "Welcome!\n"
	.string "This is the OREBURGH POKéMON GYM!\p"
	.string "I'm ROARK, the GYM LEADER!\p"
	.string "I'm but one TRAINER who decided to\n"
	.string "walk proudly with ROCK-type POKéMON!\p"
	.string "As the GYM LEADER, I need to see\n"
	.string "your potential as a TRAINER.\p"
	.string "And, I'll need to see the toughness\n"
	.string "of the POKéMON that battle with you!$"

OreburghCity_Gym_Text_RoarkDefeat:
	.string "W-what? That can't be!\n"
	.string "My buffed-up POKéMON!$"

OreburghCity_Gym_Text_BeatRoark:
	.string "This is embarrassing…\p"
	.string "I went and lost to a TRAINER who\n"
	.string "didn't have a single GYM BADGE…\p"
	.string "But that's tough. You were strong,\n"
	.string "and I was weak. That's all there is.\p"
	.string "According to POKéMON LEAGUE rules,\n"
	.string "I have to give you our GYM BADGE\l"
	.string "since you've beaten me, the LEADER.\p"
	.string "Here's your official POKéMON LEAGUE\n"
	.string "COAL BADGE.$"

OreburghCity_Gym_Text_ReceivedCoalBadge:
	.string "{PLAYER} received the COAL BADGE\n"
	.string "from ROARK!$"

OreburghCity_Gym_Text_CoalBadgeInfoTakeThis:
	.string "Having that COAL BADGE means your\n"
	.string "POKéMON can now use the hidden move\l"
	.string "ROCK SMASH outside of battle.\p"
	.string "You should also take this, too.$"

OreburghCity_Gym_Text_ExplainStealthRock:
	.string "That TM76 contains the move\n"
	.string "STEALTH ROCK.\p"
	.string "It's a move that inflicts damage on\n"
	.string "foes that switch into battle.\p"
	.string "Incidentally, a TM teaches its move\n"
	.string "to a POKéMON instantly.\p"
	.string "But a TM will break when you use it,\n"
	.string "so make certain you want a POKéMON\l"
	.string "to learn that move.$"

OreburghCity_Gym_Text_RoarkPostBattle:
	.string "In this region of SINNOH, there are\n"
	.string "seven other GYM LEADERS.\p"
	.string "I should warn you--they're a lot\n"
	.string "tougher than I am!$"

OreburghCity_Gym_Text_JonathonIntro:
	.string "You avoided the long way around?\n"
	.string "Let's see why you'd do that!$"

OreburghCity_Gym_Text_JonathonDefeat:
	.string "Now I understand why you decided to\n"
	.string "battle with me.$"

OreburghCity_Gym_Text_JonathonPostBattle:
	.string "You should go whichever way you\n"
	.string "want, shortcut or the long way!$"

OreburghCity_Gym_Text_DariusIntro:
	.string "Only those who want to win can\n"
	.string "challenge a POKéMON GYM!$"

OreburghCity_Gym_Text_DariusDefeat:
	.string "Gggh…\n"
	.string "This is humiliating!$"

OreburghCity_Gym_Text_DariusPostBattle:
	.string "One day, I'm going to become a tough\n"
	.string "TRAINER like you.$"

OreburghCity_Gym_Text_GymGuideAdvice:
	.string "Howdy!\n"
	.string "How's it going, CHAMP-to-be?\p"
	.string "That's what I said to a really\n"
	.string "impatient boy earlier, too.\p"
	.string "The GYM LEADER is a user of\n"
	.string "ROCK-type POKéMON.\p"
	.string "Well, listen. ROCK-type POKéMON\n"
	.string "really hate water, all right?\p"
	.string "They're also weak to GRASS-type\n"
	.string "moves.\p"
	.string "Gee, they sure have weaknesses!\p"
	.string "But don't think it will be easy.\p"
	.string "You don't get to be a GYM LEADER\n"
	.string "without covering for weaknesses.\p"
	.string "Going after a ROCK-type POKéMON\n"
	.string "with a FIRE-type POKéMON won't be\l"
	.string "easy, either.\p"
	.string "That's all the advice I can give.\n"
	.string "Thanks for listening!$"

OreburghCity_Gym_Text_GymGuidePostVictory:
	.string "Oh! {PLAYER}!\n"
	.string "You've taken care of the GYM LEADER!\p"
	.string "So, did my advice come in handy\n"
	.string "or what?\p"
	.string "If my advice was useful, it'd be\n"
	.string "great if you'd become my fan!$"

OreburghCity_Gym_Text_GymStatue:
	.string "OREBURGH CITY POKéMON GYM\n"
	.string "LEADER: ROARK\p"
	.string "CERTIFIED TRAINERS:\n"
	.string "{RIVAL}$"

OreburghCity_Gym_Text_GymStatueCertified:
	.string "OREBURGH CITY POKéMON GYM\n"
	.string "LEADER: ROARK\p"
	.string "CERTIFIED TRAINERS:\n"
	.string "{RIVAL}\l"
	.string "{PLAYER}$"
