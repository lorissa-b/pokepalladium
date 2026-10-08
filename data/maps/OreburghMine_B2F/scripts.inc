@ Oreburgh Mine B2F, after Platinum's, on Emerald's cave tileset.
@ A ladder leads up to B1F. Roark waits at the far end between two Rock
@ Smash rocks until the player has the first Badge; Platinum's Machop
@ aren't added, as Emerald has no Machop overworld sprite.
OreburghMine_B2F_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, OreburghMine_B2F_OnTransition
	.byte 0

OreburghMine_B2F_OnTransition:
	call_if_set FLAG_BADGE01_GET, OreburghMine_B2F_EventScript_HideRoark
	end

OreburghMine_B2F_EventScript_HideRoark::
	setflag FLAG_HIDE_OREBURGH_MINE_B2F_ROARK
	return

@ Roark smashes the rock east of him to show Rock Smash, then walks off
@ east towards the Gym.
OreburghMine_B2F_EventScript_Roark::
	lockall
	faceplayer
	msgbox OreburghMine_B2F_Text_StandBackAndWatch, MSGBOX_DEFAULT
	closemessage
	applymovement LOCALID_OREBURGH_MINE_B2F_ROARK, OreburghMine_B2F_Movement_RoarkFaceEast
	waitmovement 0
	playse SE_M_ROCK_THROW
	applymovement LOCALID_OREBURGH_MINE_B2F_ROCK_EAST, OreburghMine_B2F_Movement_SmashRock
	waitmovement 0
	removeobject LOCALID_OREBURGH_MINE_B2F_ROCK_EAST
	faceplayer
	msgbox OreburghMine_B2F_Text_BouldersNeedToBeSmashed, MSGBOX_DEFAULT
	closemessage
	applymovement LOCALID_OREBURGH_MINE_B2F_ROARK, OreburghMine_B2F_Movement_RoarkLeave
	waitmovement 0
	removeobject LOCALID_OREBURGH_MINE_B2F_ROARK
	setflag FLAG_HIDE_OREBURGH_MINE_B2F_ROARK
	releaseall
	end

OreburghMine_B2F_Movement_RoarkFaceEast:
	delay_16
	face_right
	delay_16
	step_end

OreburghMine_B2F_Movement_SmashRock:
	rock_smash_break
	step_end

OreburghMine_B2F_Movement_RoarkLeave:
	walk_right
	walk_right
	walk_right
	walk_right
	walk_right
	walk_right
	walk_right
	walk_right
	walk_right
	step_end

OreburghMine_B2F_EventScript_Worker::
	msgbox OreburghMine_B2F_Text_IMetMachopOnRoute207, MSGBOX_NPC
	end

OreburghMine_B2F_EventScript_Colin::
	trainerbattle_single TRAINER_CLARK, OreburghMine_B2F_Text_ColinIntro, OreburghMine_B2F_Text_ColinDefeated
	msgbox OreburghMine_B2F_Text_ColinPostBattle, MSGBOX_AUTOCLOSE
	end

OreburghMine_B2F_EventScript_Mason::
	trainerbattle_single TRAINER_BRICE, OreburghMine_B2F_Text_MasonIntro, OreburghMine_B2F_Text_MasonDefeated
	msgbox OreburghMine_B2F_Text_MasonPostBattle, MSGBOX_AUTOCLOSE
	end

OreburghMine_B2F_Text_StandBackAndWatch:
	.string "OK, stand back and watch this.\p"
	.string "Using the hidden move ROCK SMASH,\n"
	.string "a boulder blocking your way…$"

OreburghMine_B2F_Text_BouldersNeedToBeSmashed:
	.string "Fallen boulders need to be smashed\n"
	.string "so they're out of the way.\p"
	.string "If you could get the BADGE from\n"
	.string "the GYM in town, you'd be able\l"
	.string "to do this too.\p"
	.string "Of course, you'd have to beat the\n"
	.string "GYM LEADER first. That'd be me!$"

OreburghMine_B2F_Text_IMetMachopOnRoute207:
	.string "I met my MACHOP on ROUTE 207.\p"
	.string "They've now become my most trusted\n"
	.string "partners!$"

OreburghMine_B2F_Text_ColinIntro:
	.string "Fine, all righty then.\n"
	.string "Just a quick battle on my break!$"

OreburghMine_B2F_Text_ColinDefeated:
	.string "Wahaha!\n"
	.string "Mercy, I cry mercy!$"

OreburghMine_B2F_Text_ColinPostBattle:
	.string "I'd better get on back to work…\p"
	.string "You know, I'd rather be traveling\n"
	.string "around with POKéMON the way you do.$"

OreburghMine_B2F_Text_MasonIntro:
	.string "I'm gonna let you in on a secret…\n"
	.string "I snuck my POKéMON to work!$"

OreburghMine_B2F_Text_MasonDefeated:
	.string "Well, now aren't you something!$"

OreburghMine_B2F_Text_MasonPostBattle:
	.string "OK, OK, time for work! I'll work up\n"
	.string "a thirst with my POKéMON!$"
