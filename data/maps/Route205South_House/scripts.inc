@ The rest house on Route 205 South, as in Platinum. Was Route 119's house.
Route205South_House_MapScripts::
	.byte 0

Route205South_House_EventScript_PokefanF::
	lock
	faceplayer
	msgbox Route205South_House_Text_RestAWhile, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, Route205South_House_EventScript_DontPushYourself
	closemessage
	call Common_EventScript_OutOfCenterPartyHeal
	msgbox Route205South_House_Text_PokemonLookRefreshed, MSGBOX_DEFAULT
	release
	end

Route205South_House_EventScript_DontPushYourself::
	msgbox Route205South_House_Text_DontPushYourself, MSGBOX_DEFAULT
	release
	end

Route205South_House_EventScript_BugCatcher::
	msgbox Route205South_House_Text_RepelsAreUseful, MSGBOX_NPC
	end

Route205South_House_Text_RestAWhile:
	.string "You seem to be tired…\n"
	.string "Rest a while.$"

Route205South_House_Text_PokemonLookRefreshed:
	.string "You and your POKéMON look\n"
	.string "completely refreshed!$"

Route205South_House_Text_DontPushYourself:
	.string "Just because you're young doesn't\n"
	.string "mean you should push yourself.$"

Route205South_House_Text_RepelsAreUseful:
	.string "REPELS are useful, don't you think?\p"
	.string "When you use a REPEL, weak POKéMON\n"
	.string "won't come out.\p"
	.string "…But that also means there won't be\n"
	.string "any POKéMON to capture.$"
