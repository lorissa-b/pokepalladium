@ The rival's house. The player always lives in the Dawn house (see TwinleafTown_DawnHouse_1F).
TwinleafTown_LucasHouse_1F_MapScripts::
	.byte 0

@ The player can't get in here before the rival runs out of the door (TwinleafTown_EventScript_RivalBumpsIntoPlayer)
RivalsHouse_1F_EventScript_RivalMom::
	lock
	faceplayer
	goto_if_ge VAR_TWINLEAF_TOWN_STATE, 4, RivalsHouse_1F_EventScript_PleaseDeliverTownMap
	goto_if_set FLAG_SYS_POKEMON_GET, RivalsHouse_1F_EventScript_RivalWentToSandgem
	msgbox RivalsHouse_1F_Text_RivalRanOffToRoute201, MSGBOX_DEFAULT
	release
	end

RivalsHouse_1F_EventScript_PleaseDeliverTownMap::
	msgbox RivalsHouse_1F_Text_PleaseDeliverTownMap, MSGBOX_DEFAULT
	release
	end

RivalsHouse_1F_EventScript_RivalWentToSandgem::
	msgbox RivalsHouse_1F_Text_RivalWentToSandgem, MSGBOX_DEFAULT
	release
	end

RivalsHouse_1F_EventScript_RivalSibling::
	msgbox RivalsHouse_1F_Text_DoYouHavePokemon, MSGBOX_NPC
	end

RivalsHouse_1F_Text_RivalRanOffToRoute201:
	.string "Oh, {PLAYER}{KUN}!\n"
	.string "You just missed {RIVAL}.\p"
	.string "{RIVAL} went charging out the door\n"
	.string "shouting about ROUTE 201.\p"
	.string "Honestly, that child never stops\n"
	.string "to walk anywhere!$"

RivalsHouse_1F_Text_RivalWentToSandgem:
	.string "Oh, {PLAYER}{KUN}! {RIVAL} came flying\n"
	.string "home just now, shouting something\l"
	.string "about SANDGEM TOWN…\p"
	.string "…and then flew right back out again.\p"
	.string "Like father, like child.\n"
	.string "{RIVAL} can't stay quietly at home.$"

RivalsHouse_1F_Text_DoYouHavePokemon:
	.string "Hi, neighbor!\p"
	.string "Do you already have your\n"
	.string "own POKéMON?$"

RivalsHouse_1F_Text_PleaseDeliverTownMap:
	.string "Please give that TOWN MAP to {RIVAL}\n"
	.string "when you catch up. Thank you!$"
