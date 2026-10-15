@ Eterna Forest, between Route 205 South and Route 205 North. Drawn over
@ Petalburg Woods from Platinum's map. Route 205 North and the Old Chateau
@ aren't here yet, so the east exit and the chateau door lead back to
@ themselves.

EternaForest_MapScripts::
	.byte 0

EternaForest_EventScript_BugCatcher::
	msgbox EternaForest_Text_SearchingForBugPokemon, MSGBOX_NPC
	end

EternaForest_EventScript_Sign::
	msgbox EternaForest_Text_Sign, MSGBOX_SIGN
	end

EternaForest_EventScript_TrainerTips::
	msgbox EternaForest_Text_TrainerTips, MSGBOX_SIGN
	end

EternaForest_Text_SearchingForBugPokemon:
	.string "I'm searching for bug POKéMON so I can\n"
	.string "win at the ETERNA GYM.\p"
	.string "Bug POKéMON are great, you know?\n"
	.string "They grow really fast!$"

EternaForest_Text_Sign:
	.string "ETERNA FOREST\n"
	.string "Where Time Flows Eternally$"

EternaForest_Text_TrainerTips:
	.string "TRAINER TIPS\p"
	.string "In forests and caves, there are many\n"
	.string "items on the ground that cannot be\l"
	.string "seen in the murky darkness.\p"
	.string "Take the time to search carefully!$"
