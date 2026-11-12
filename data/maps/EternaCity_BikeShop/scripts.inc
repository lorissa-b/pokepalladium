@ Rad Rickshaw's Cycle Shop, drawn over Rydel's Cycles from Platinum's map.
@ Its owner is held in the Galactic Eterna Building until Jupiter is beaten
@ on 4F, which clears FLAG_HIDE_CYCLE_SHOP_OWNER and FLAG_HIDE_CYCLE_SHOP_CLEFAIRY;
@ back here, he gives the player the BICYCLE (FLAG_RECEIVED_BIKE), and
@ Eterna City's exits are blocked until they also have the EXPLORER KIT
@ (VAR_ETERNA_CITY_BLOCK_EXITS_STATE).
EternaCity_BikeShop_MapScripts::
	.byte 0

EternaCity_BikeShop_EventScript_Owner::
	lock
	faceplayer
	goto_if_set FLAG_RECEIVED_BIKE, EternaCity_BikeShop_EventScript_ReadOperatingManual
	msgbox EternaCity_BikeShop_Text_ThanksForRescuingMe, MSGBOX_DEFAULT
	giveitem ITEM_BICYCLE
	setflag FLAG_RECEIVED_BIKE
	setvar VAR_ETERNA_CITY_BLOCK_EXITS_STATE, 1
	goto EternaCity_BikeShop_EventScript_ReadOperatingManual
	end

EternaCity_BikeShop_EventScript_ReadOperatingManual::
	msgbox EternaCity_BikeShop_Text_IllReadOperatingManual, MSGBOX_DEFAULT
	release
	end

EternaCity_BikeShop_EventScript_Clefairy::
	lock
	faceplayer
	waitse
	playmoncry SPECIES_CLEFAIRY, CRY_MODE_NORMAL
	msgbox EternaCity_BikeShop_Text_ClefairyPipPiih, MSGBOX_DEFAULT
	waitmoncry
	release
	end

EternaCity_BikeShop_EventScript_Youngster::
	lock
	faceplayer
	goto_if_set FLAG_HIDE_ETERNA_CITY_GALACTIC_GRUNTS, EternaCity_BikeShop_EventScript_WhatWasTeamGalacticTrying
	msgbox EternaCity_BikeShop_Text_ManagersGoneOff, MSGBOX_DEFAULT
	release
	end

EternaCity_BikeShop_EventScript_WhatWasTeamGalacticTrying::
	msgbox EternaCity_BikeShop_Text_WhatWasTeamGalacticTrying, MSGBOX_DEFAULT
	release
	end

EternaCity_BikeShop_EventScript_Manual::
	msgbox EternaCity_BikeShop_Text_ManualOnLatestBicycle, MSGBOX_SIGN
	end

EternaCity_BikeShop_EventScript_Catalog::
	msgbox EternaCity_BikeShop_Text_CatalogFromHoenn, MSGBOX_SIGN
	end

EternaCity_BikeShop_Text_ThanksForRescuingMe:
	.string "Thanks for rescuing me!\n"
	.string "This is something to show my thanks!\p"
	.string "It's the latest model BICYCLE!\n"
	.string "You must take it. I insist!$"

EternaCity_BikeShop_Text_IllReadOperatingManual:
	.string "It's the latest model, so I'll read\n"
	.string "the operating manual to you.\p"
	.string "“Press the B Button to shift up\n"
	.string "or down.”\p"
	.string "In third gear, you can't go all that\n"
	.string "fast, but it's easier to control.\p"
	.string "In fourth gear, it gets harder to\n"
	.string "steer and stop, but it's fast! Really!$"

EternaCity_BikeShop_Text_ClefairyPipPiih:
	.string "CLEFAIRY: Pip piih!$"

EternaCity_BikeShop_Text_ManagersGoneOff:
	.string "The manager's gone off to the TEAM\n"
	.string "GALACTIC building and hasn't returned.\p"
	.string "I wonder what's holding him up?$"

EternaCity_BikeShop_Text_WhatWasTeamGalacticTrying:
	.string "What was TEAM GALACTIC trying to do\n"
	.string "with other people's POKéMON?$"

EternaCity_BikeShop_Text_ManualOnLatestBicycle:
	.string "There is a manual on the latest\n"
	.string "model BICYCLE.\p"
	.string "Third gear for steering.\n"
	.string "Fourth gear for all-out speed!\p"
	.string "Change gears with the B Button for\n"
	.string "exhilarating cycling!$"

EternaCity_BikeShop_Text_CatalogFromHoenn:
	.string "There's a catalog from the HOENN\n"
	.string "region on its popular BIKES…\p"
	.string "MACH BIKE--a folding BIKE built solely\n"
	.string "for blazing speed!\p"
	.string "ACRO BIKE--a folding BIKE designed for\n"
	.string "stunts like jumps and wheelies!$"
