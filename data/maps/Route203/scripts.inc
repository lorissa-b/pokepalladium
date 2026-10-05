@ Route 203, east of Jubilife City toward Oreburgh City. Platinum's route
@ (rows 4-28 of its 64x32 chunk) drafted with mapkit on Jubilife's tilesets
@ (General and Rustboro), so the shared edge draws the same from both sides.
@ The entrance is widened to rows 15-20 to meet all six lanes of Jubilife's
@ east road. Oreburgh Gate is the cave outcrop in the east trees, its mouth
@ at (58,8) since Emerald has no west-facing cave mouth; it has no warp until
@ the gate exists. Platinum's trainers and the rival battle aren't here yet.
Route203_MapScripts::
	.byte 0

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
