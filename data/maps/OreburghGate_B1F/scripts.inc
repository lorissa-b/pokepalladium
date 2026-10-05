@ Oreburgh Gate B1F, after Platinum's (x - 5, y - 2), on Emerald's cave tileset.
@ Platinum's Rock Smash rocks, Strength boulders and items are at its spots;
@ its south lake ends in a straight edge like Ravaged Path's.
OreburghGate_B1F_MapScripts::
	.byte 0

OreburghGate_B1F_EventScript_Cyclist::
	msgbox OreburghGate_B1F_Text_DoYouSeeThoseBumps, MSGBOX_NPC
	end

OreburghGate_B1F_EventScript_Grant::
	trainerbattle_single TRAINER_FREDRICK, OreburghGate_B1F_Text_GrantIntro, OreburghGate_B1F_Text_GrantDefeated
	msgbox OreburghGate_B1F_Text_GrantPostBattle, MSGBOX_AUTOCLOSE
	end

OreburghGate_B1F_Text_DoYouSeeThoseBumps:
	.string "Do you see those bumps on the\n"
	.string "ground?\p"
	.string "It feels awesome taking those fast\n"
	.string "on a BIKE.$"

OreburghGate_B1F_Text_GrantIntro:
	.string "I'm a VETERAN, yes, but don't you\n"
	.string "dare treat me like a senior citizen!$"

OreburghGate_B1F_Text_GrantDefeated:
	.string "Grrr…\n"
	.string "So young, and yet so talented!$"

OreburghGate_B1F_Text_GrantPostBattle:
	.string "I'll always remain young at heart.\n"
	.string "Never let them say you're too old!$"
