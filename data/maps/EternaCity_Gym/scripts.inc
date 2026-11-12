@ Eterna Gym, from Platinum's: a garden whose floor is a floral clock.
@ Gardenia waits at the back; Lass Caroline, Aroma Lady Jenna and Aroma
@ Lady Angela are spoken to, not spotted, and beating each in turn moves
@ the clock (src/eterna_gym.c), whose hands are the only paths across it:
@ Caroline's opens the way to Jenna, Jenna's to Angela (and drains the
@ east fountain), Angela's to Gardenia (and drains the west fountain).
@ Beating Gardenia moves it once more, into a straight path from the door.
@ The clock's state is VAR_ETERNA_GYM_CLOCK_STATE (0-4).
@
@ Gardenia is TRAINER_BRAWLY_1's slot (the second Badge's leader, as Roark
@ is Roxanne's); Caroline is TRAINER_VIVIAN's, Jenna TRAINER_KIRK's and
@ Angela TRAINER_SHAWN's (Common_EventScript_SetGymTrainers case 3).

EternaCity_Gym_MapScripts::
	map_script MAP_SCRIPT_ON_LOAD, EternaCity_Gym_OnLoad
	.byte 0

EternaCity_Gym_OnLoad:
	special EternaGym_DrawClock
	end

@ VAR_0x8004 is the clock state to move to. Platinum pans the camera to the
@ clock while it turns, then to a fountain when one stops.
EternaCity_Gym_EventScript_TurnClock::
	special EternaGym_CameraToClock
	waitstate
	special EternaGym_TurnClock
	waitstate
	call_if_eq VAR_ETERNA_GYM_CLOCK_STATE, 2, EternaCity_Gym_EventScript_DrainFountain
	call_if_eq VAR_ETERNA_GYM_CLOCK_STATE, 3, EternaCity_Gym_EventScript_DrainFountain
	special EternaGym_CameraToPlayer
	waitstate
	return

EternaCity_Gym_EventScript_DrainFountain::
	special EternaGym_CameraToFountain
	waitstate
	msgbox EternaCity_Gym_Text_FountainWaterLevelDropped, MSGBOX_DEFAULT
	closemessage
	return

EternaCity_Gym_EventScript_Gardenia::
	trainerbattle_single TRAINER_BRAWLY_1, EternaCity_Gym_Text_GardeniaIntro, EternaCity_Gym_Text_GardeniaDefeat, EternaCity_Gym_EventScript_GardeniaDefeated, NO_MUSIC
	goto_if_unset FLAG_RECEIVED_TM_GRASS_KNOT, EternaCity_Gym_EventScript_GiveGrassKnotAgain
	msgbox EternaCity_Gym_Text_GardeniaPostBattle, MSGBOX_DEFAULT
	release
	end

EternaCity_Gym_EventScript_GardeniaDefeated::
	msgbox EternaCity_Gym_Text_BeatGardenia, MSGBOX_DEFAULT
	message EternaCity_Gym_Text_ReceivedForestBadge
	waitmessage
	call Common_EventScript_PlayGymBadgeFanfare
	setflag FLAG_BADGE02_GET
	setvar VAR_0x8008, 3
	call Common_EventScript_SetGymTrainers
	clearflag FLAG_HIDE_ETERNA_FOREST_GARDENIA
	msgbox EternaCity_Gym_Text_ForestBadgeInfoTakeThis, MSGBOX_DEFAULT
	call EternaCity_Gym_EventScript_GiveGrassKnot
	closemessage
	setvar VAR_0x8004, 4
	call EternaCity_Gym_EventScript_TurnClock
	release
	end

EternaCity_Gym_EventScript_GiveGrassKnotAgain::
	call EternaCity_Gym_EventScript_GiveGrassKnot
	release
	end

EternaCity_Gym_EventScript_GiveGrassKnot::
	giveitem ITEM_TM_GRASS_KNOT
	goto_if_eq VAR_RESULT, FALSE, Common_EventScript_BagIsFull
	setflag FLAG_RECEIVED_TM_GRASS_KNOT
	msgbox EternaCity_Gym_Text_ExplainGrassKnot, MSGBOX_DEFAULT
	return

EternaCity_Gym_EventScript_Caroline::
	trainerbattle_single TRAINER_VIVIAN, EternaCity_Gym_Text_CarolineIntro, EternaCity_Gym_Text_CarolineDefeat, EternaCity_Gym_EventScript_CarolineDefeated
	msgbox EternaCity_Gym_Text_CarolinePostBattle, MSGBOX_AUTOCLOSE
	end

EternaCity_Gym_EventScript_CarolineDefeated::
	msgbox EternaCity_Gym_Text_CarolinePostBattle, MSGBOX_DEFAULT
	closemessage
	setvar VAR_0x8004, 1
	call EternaCity_Gym_EventScript_TurnClock
	release
	end

EternaCity_Gym_EventScript_Jenna::
	trainerbattle_single TRAINER_KIRK, EternaCity_Gym_Text_JennaIntro, EternaCity_Gym_Text_JennaDefeat, EternaCity_Gym_EventScript_JennaDefeated
	msgbox EternaCity_Gym_Text_JennaPostBattle, MSGBOX_AUTOCLOSE
	end

EternaCity_Gym_EventScript_JennaDefeated::
	msgbox EternaCity_Gym_Text_JennaPostBattle, MSGBOX_DEFAULT
	closemessage
	setvar VAR_0x8004, 2
	call EternaCity_Gym_EventScript_TurnClock
	release
	end

EternaCity_Gym_EventScript_Angela::
	trainerbattle_single TRAINER_SHAWN, EternaCity_Gym_Text_AngelaIntro, EternaCity_Gym_Text_AngelaDefeat, EternaCity_Gym_EventScript_AngelaDefeated
	msgbox EternaCity_Gym_Text_AngelaPostBattle, MSGBOX_AUTOCLOSE
	end

EternaCity_Gym_EventScript_AngelaDefeated::
	msgbox EternaCity_Gym_Text_AngelaPostBattle, MSGBOX_DEFAULT
	closemessage
	setvar VAR_0x8004, 3
	call EternaCity_Gym_EventScript_TurnClock
	release
	end

EternaCity_Gym_EventScript_GymGuide::
	lock
	faceplayer
	goto_if_set FLAG_BADGE02_GET, EternaCity_Gym_EventScript_GymGuidePostVictory
	msgbox EternaCity_Gym_Text_GymGuideAdvice, MSGBOX_DEFAULT
	release
	end

EternaCity_Gym_EventScript_GymGuidePostVictory::
	msgbox EternaCity_Gym_Text_GymGuidePostVictory, MSGBOX_DEFAULT
	release
	end

EternaCity_Gym_EventScript_GymStatue::
	lockall
	goto_if_set FLAG_BADGE02_GET, EternaCity_Gym_EventScript_GymStatueCertified
	msgbox EternaCity_Gym_Text_GymStatue, MSGBOX_DEFAULT
	releaseall
	end

EternaCity_Gym_EventScript_GymStatueCertified::
	msgbox EternaCity_Gym_Text_GymStatueCertified, MSGBOX_DEFAULT
	releaseall
	end

EternaCity_Gym_Text_GardeniaIntro:
	.string "You kept me waiting!\n"
	.string "I'm ETERNA's GYM LEADER, GARDENIA,\l"
	.string "the GRASS-type master!\p"
	.string "When I first saw you, I was convinced\n"
	.string "you'd find your way to me.\p"
	.string "My hunch was right on the money.\n"
	.string "You have a winning aura about you.\p"
	.string "So, anyway, this will be fun.\n"
	.string "Let's have our battle!$"

EternaCity_Gym_Text_GardeniaDefeat:
	.string "Amazing!\n"
	.string "You're very good, aren't you?$"

EternaCity_Gym_Text_BeatGardenia:
	.string "I might've said it before, but you're\n"
	.string "really tough!\p"
	.string "Wasn't it hard for you to raise your\n"
	.string "POKéMON to be so good?\p"
	.string "I guess that's a measure of how much\n"
	.string "you love your POKéMON.\p"
	.string "In recognition of that, I proudly\n"
	.string "grant you this!$"

EternaCity_Gym_Text_ReceivedForestBadge:
	.string "{PLAYER} received the FOREST BADGE\n"
	.string "from GARDENIA!$"

EternaCity_Gym_Text_ForestBadgeInfoTakeThis:
	.string "With the FOREST BADGE, your POKéMON\n"
	.string "can use the hidden move CUT outside\l"
	.string "of battle.\p"
	.string "Now that you have two GYM BADGES,\n"
	.string "all POKéMON you received in trades\l"
	.string "will obey you up to Lv. 30.\p"
	.string "I've also got something else.\n"
	.string "It's a gift from me!$"

EternaCity_Gym_Text_ExplainGrassKnot:
	.string "That TM86 contains the move\n"
	.string "GRASS KNOT.\p"
	.string "It's one of my favorite moves.\p"
	.string "The heavier the foe, the more\n"
	.string "damage this move will cause.$"

EternaCity_Gym_Text_GardeniaPostBattle:
	.string "Yup! If there's one thing I can tell\n"
	.string "about you, it's this!\p"
	.string "You and your POKéMON are gonna get\n"
	.string "even tougher, and I mean seriously!$"

EternaCity_Gym_Text_CarolineIntro:
	.string "Welcome!\p"
	.string "Without further ado, let's go!$"

EternaCity_Gym_Text_CarolineDefeat:
	.string "Giggle…\n"
	.string "The clock will turn now.$"

EternaCity_Gym_Text_CarolinePostBattle:
	.string "This GYM's floral clock…\p"
	.string "You can walk on its hands.\p"
	.string "That's how you get to the next\n"
	.string "TRAINER.$"

EternaCity_Gym_Text_JennaIntro:
	.string "My GRASS-type POKéMON can dole out\n"
	.string "some serious special attacks…\p"
	.string "I won't let you win…easily!$"

EternaCity_Gym_Text_JennaDefeat:
	.string "The more beautiful a flower,\n"
	.string "the faster it withers…$"

EternaCity_Gym_Text_JennaPostBattle:
	.string "Every time you beat a TRAINER, the\n"
	.string "floral clock moves.\p"
	.string "The fountain also stops if I lose.\p"
	.string "It's so you can go to the POKéMON\n"
	.string "CENTER during your challenge.$"

EternaCity_Gym_Text_AngelaIntro:
	.string "You can't win when you're distracted\n"
	.string "by the lovely aromas of POKéMON!$"

EternaCity_Gym_Text_AngelaDefeat:
	.string "Just like your inability to conceal\n"
	.string "your abundant skill.$"

EternaCity_Gym_Text_AngelaPostBattle:
	.string "The floral clock moves, and time\n"
	.string "flows…\p"
	.string "There's only our GYM LEADER left.\n"
	.string "Go on, don't be afraid!$"

EternaCity_Gym_Text_FountainWaterLevelDropped:
	.string "The fountain's water level dropped!\p"
	.string "It's possible to walk across the\n"
	.string "fountain now!$"

EternaCity_Gym_Text_GymGuideAdvice:
	.string "Hey, there!\n"
	.string "How's it going, CHAMP-to-be?\p"
	.string "Actually, that's what I said to some\n"
	.string "boy who zipped in here earlier, too!\p"
	.string "The GYM LEADER, GARDENIA, is a user\n"
	.string "of GRASS-type POKéMON.\p"
	.string "It should be obvious, but GRASS-type\n"
	.string "POKéMON really hate fire.\p"
	.string "They're also at a huge disadvantage\n"
	.string "against the FLYING type, too.\p"
	.string "Oh, wait a second. I should tell you\n"
	.string "about the GYM itself first.\p"
	.string "You can't challenge the GYM LEADER\n"
	.string "until you beat all the GYM's TRAINERS.\p"
	.string "It's a bit challenging, but just think\n"
	.string "of it as good training and go for it!$"

EternaCity_Gym_Text_GymGuidePostVictory:
	.string "Oh! {PLAYER}!\n"
	.string "You've beaten the GYM LEADER!\p"
	.string "Your POKéMON look much tougher than\n"
	.string "when they went in, too!\p"
	.string "At least that's how I see it!$"

EternaCity_Gym_Text_GymStatue:
	.string "ETERNA CITY POKéMON GYM\n"
	.string "LEADER: GARDENIA\p"
	.string "CERTIFIED TRAINERS:\n"
	.string "{RIVAL}$"

EternaCity_Gym_Text_GymStatueCertified:
	.string "ETERNA CITY POKéMON GYM\n"
	.string "LEADER: GARDENIA\p"
	.string "CERTIFIED TRAINERS:\n"
	.string "{RIVAL}\l"
	.string "{PLAYER}$"
