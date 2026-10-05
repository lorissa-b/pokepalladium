@ Oreburgh Gate 1F, after Platinum's (x - 2, y - 3), on Emerald's cave tileset
@ like Ravaged Path. The exits to Route 203 (west) and Oreburgh City (east) are
@ south-facing cave mouths at the ends of Platinum's corridor, since the cave
@ tileset has no side exits; the east one has no warp until Oreburgh City exists.
@ The stairs down are a ladder to B1F.
OreburghGate_1F_MapScripts::
	.byte 0

	@ VAR_OREBURGH_GATE_1F_STATE: 0 until the Hiker has given HM06 Rock Smash, then 1.

@ Passing him on the way east (x 5, y 19-20), the Hiker comes over with HM06
OreburghGate_1F_EventScript_HikerTriggerY19::
	lockall
	applymovement LOCALID_OREBURGH_GATE_1F_HIKER, OreburghGate_1F_Movement_HikerWalkDown1
	applymovement LOCALID_PLAYER, OreburghGate_1F_Movement_PlayerFaceHiker
	waitmovement 0
	goto OreburghGate_1F_EventScript_HikerGiveRockSmash

OreburghGate_1F_EventScript_HikerTriggerY20::
	lockall
	applymovement LOCALID_OREBURGH_GATE_1F_HIKER, OreburghGate_1F_Movement_HikerWalkDown2
	applymovement LOCALID_PLAYER, OreburghGate_1F_Movement_PlayerFaceHiker
	waitmovement 0
	goto OreburghGate_1F_EventScript_HikerGiveRockSmash

OreburghGate_1F_EventScript_HikerGiveRockSmash::
	msgbox OreburghGate_1F_Text_MakeAGiftOfThisHM, MSGBOX_DEFAULT
	giveitem ITEM_HM_ROCK_SMASH
	setflag FLAG_RECEIVED_HM_ROCK_SMASH
	setvar VAR_OREBURGH_GATE_1F_STATE, 1
	msgbox OreburghGate_1F_Text_ThatHMContainsRockSmash, MSGBOX_DEFAULT
	releaseall
	end

OreburghGate_1F_EventScript_Hiker::
	lock
	faceplayer
	goto_if_set FLAG_BADGE01_GET, OreburghGate_1F_EventScript_HikerHaveBadge
	goto_if_set FLAG_RECEIVED_HM_ROCK_SMASH, OreburghGate_1F_EventScript_HikerExplainRockSmash
	msgbox OreburghGate_1F_Text_MakeAGiftOfThisHM, MSGBOX_DEFAULT
	giveitem ITEM_HM_ROCK_SMASH
	setflag FLAG_RECEIVED_HM_ROCK_SMASH
	setvar VAR_OREBURGH_GATE_1F_STATE, 1
	goto OreburghGate_1F_EventScript_HikerExplainRockSmash

OreburghGate_1F_EventScript_HikerExplainRockSmash::
	msgbox OreburghGate_1F_Text_ThatHMContainsRockSmash, MSGBOX_DEFAULT
	release
	end

OreburghGate_1F_EventScript_HikerHaveBadge::
	msgbox OreburghGate_1F_Text_NowYouCanUseRockSmash, MSGBOX_DEFAULT
	release
	end

OreburghGate_1F_Movement_HikerWalkDown1:
	face_down
	emote_exclamation_mark
	walk_down
	step_end

OreburghGate_1F_Movement_HikerWalkDown2:
	face_down
	emote_exclamation_mark
	walk_down
	walk_down
	step_end

OreburghGate_1F_Movement_PlayerFaceHiker:
	delay_8
	walk_in_place_faster_up
	step_end

OreburghGate_1F_Text_MakeAGiftOfThisHM:
	.string "That sparkling new POKéTCH!\n"
	.string "Your awkwardness as a traveler!\p"
	.string "Hmm… You're still a pretty new\n"
	.string "TRAINER, aren't you?\p"
	.string "But it's all good! You're a new\n"
	.string "fellow friend of POKéMON!\p"
	.string "So, let me make a gift of this\n"
	.string "HIDDEN MACHINE to you!$"

OreburghGate_1F_Text_ThatHMContainsRockSmash:
	.string "That HIDDEN MACHINE, or HM for short,\n"
	.string "contains the hidden move ROCK SMASH.\p"
	.string "A POKéMON learning ROCK SMASH can\n"
	.string "smash small boulders in the field.\p"
	.string "But the TRAINER has to have\n"
	.string "the GYM BADGE from OREBURGH CITY.\p"
	.string "If you don't have that BADGE, you\n"
	.string "can't make a POKéMON use the hidden\l"
	.string "move ROCK SMASH outside of battle.$"

OreburghGate_1F_Text_NowYouCanUseRockSmash:
	.string "Oh! Good golly!\n"
	.string "That's OREBURGH CITY's GYM BADGE!\p"
	.string "All right! Now you can use the hidden\n"
	.string "move ROCK SMASH on small boulders!$"
