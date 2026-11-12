@ Team Galactic Eterna Building 4F, from Platinum's. Jupiter stands guard
@ over the Cycle Shop owner's Clefairy and a stolen Buneary. Talking to her
@ starts her battle; beating her sends Team Galactic out of the building
@ and Eterna City (FLAG_HIDE_ETERNA_CITY_GALACTIC_GRUNTS, which also hides
@ Looker on 1F) and the owner thanks the player before going home with
@ the Pokemon (FLAG_HIDE_TEAM_GALACTIC_ETERNA_BUILDING_4F_HOSTAGES).
@ VAR_ETERNA_CITY_STATE becomes 3, so Cynthia brings the Togepi Egg in
@ Eterna City, and the owner and his Clefairy are back in the Cycle Shop.
TeamGalacticEternaBuilding_4F_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, TeamGalacticEternaBuilding_4F_OnTransition
	.byte 0

TeamGalacticEternaBuilding_4F_OnTransition:
	call TeamGalacticEternaBuilding_1F_EventScript_TryPlayGalacticMusic
	end

TeamGalacticEternaBuilding_4F_EventScript_Jupiter::
	lockall
	faceplayer
	applymovement LOCALID_TEAM_GALACTIC_ETERNA_BUILDING_4F_POKEFAN_M, Common_Movement_WalkInPlaceFasterUp
	waitmovement 0
	msgbox TeamGalacticEternaBuilding_4F_Text_IWillDealWithYou, MSGBOX_DEFAULT
	closemessage
	trainerbattle_no_intro TRAINER_MATT, TeamGalacticEternaBuilding_4F_Text_JupiterDefeat
	msgbox TeamGalacticEternaBuilding_4F_Text_WellArentYouTough, MSGBOX_DEFAULT
	closemessage
	fadescreen FADE_TO_BLACK
	removeobject LOCALID_TEAM_GALACTIC_ETERNA_BUILDING_4F_JUPITER
	fadescreen FADE_FROM_BLACK
	delay 15
	goto_if_eq VAR_FACING, DIR_NORTH, TeamGalacticEternaBuilding_4F_EventScript_PokefanMWalkToPlayerNorth
	goto_if_eq VAR_FACING, DIR_WEST, TeamGalacticEternaBuilding_4F_EventScript_PokefanMWalkToPlayerWest
	goto TeamGalacticEternaBuilding_4F_EventScript_PokefanMWalkToPlayerEast
	end

@ The player is below where Jupiter stood: the owner comes up behind them.
TeamGalacticEternaBuilding_4F_EventScript_PokefanMWalkToPlayerNorth::
	applymovement LOCALID_TEAM_GALACTIC_ETERNA_BUILDING_4F_POKEFAN_M, TeamGalacticEternaBuilding_4F_Movement_PokefanMWalkToPlayerNorth
	applymovement LOCALID_PLAYER, TeamGalacticEternaBuilding_4F_Movement_PlayerFacePokefanM
	waitmovement 0
	goto TeamGalacticEternaBuilding_4F_EventScript_TeamGalacticLeaves
	end

TeamGalacticEternaBuilding_4F_EventScript_PokefanMWalkToPlayerWest::
	applymovement LOCALID_TEAM_GALACTIC_ETERNA_BUILDING_4F_POKEFAN_M, TeamGalacticEternaBuilding_4F_Movement_PokefanMWalkToPlayerWest
	waitmovement 0
	goto TeamGalacticEternaBuilding_4F_EventScript_TeamGalacticLeaves
	end

TeamGalacticEternaBuilding_4F_EventScript_PokefanMWalkToPlayerEast::
	applymovement LOCALID_TEAM_GALACTIC_ETERNA_BUILDING_4F_POKEFAN_M, TeamGalacticEternaBuilding_4F_Movement_PokefanMWalkToPlayerEast
	waitmovement 0
	goto TeamGalacticEternaBuilding_4F_EventScript_TeamGalacticLeaves
	end

TeamGalacticEternaBuilding_4F_EventScript_TeamGalacticLeaves::
	msgbox TeamGalacticEternaBuilding_4F_Text_ThankYouCruiseByCycleShop, MSGBOX_DEFAULT
	closemessage
	fadescreen FADE_TO_BLACK
	setflag FLAG_HIDE_ETERNA_CITY_GALACTIC_GRUNTS
	setflag FLAG_HIDE_TEAM_GALACTIC_ETERNA_BUILDING_1F_LOOKER
	setflag FLAG_HIDE_TEAM_GALACTIC_ETERNA_BUILDING_1F_DISGUISED_LOOKER
	removeobject LOCALID_TEAM_GALACTIC_ETERNA_BUILDING_4F_POKEFAN_M
	removeobject LOCALID_TEAM_GALACTIC_ETERNA_BUILDING_4F_CLEFAIRY
	removeobject LOCALID_TEAM_GALACTIC_ETERNA_BUILDING_4F_BUNEARY
	setvar VAR_ETERNA_CITY_STATE, 3
	clearflag FLAG_HIDE_CYCLE_SHOP_OWNER
	clearflag FLAG_HIDE_CYCLE_SHOP_CLEFAIRY
	fadedefaultbgm
	fadescreen FADE_FROM_BLACK
	releaseall
	end

TeamGalacticEternaBuilding_4F_Movement_PokefanMWalkToPlayerNorth:
	walk_up
	step_end

TeamGalacticEternaBuilding_4F_Movement_PokefanMWalkToPlayerWest:
	walk_up
	walk_up
	walk_up
	walk_in_place_right
	step_end

TeamGalacticEternaBuilding_4F_Movement_PokefanMWalkToPlayerEast:
	walk_up
	walk_up
	walk_up
	walk_in_place_left
	step_end

TeamGalacticEternaBuilding_4F_Movement_PlayerFacePokefanM:
	delay_8
	walk_in_place_down
	step_end

TeamGalacticEternaBuilding_4F_EventScript_PokefanM::
	lock
	faceplayer
	goto_if_set FLAG_HIDE_ETERNA_CITY_GALACTIC_GRUNTS, TeamGalacticEternaBuilding_4F_EventScript_ThankYouCruiseByCycleShop
	msgbox TeamGalacticEternaBuilding_4F_Text_WhatsWithTeamGalactic, MSGBOX_DEFAULT
	release
	end

TeamGalacticEternaBuilding_4F_EventScript_ThankYouCruiseByCycleShop::
	msgbox TeamGalacticEternaBuilding_4F_Text_ThankYouCruiseByCycleShop, MSGBOX_DEFAULT
	release
	end

TeamGalacticEternaBuilding_4F_EventScript_Clefairy::
	lock
	faceplayer
	waitse
	playmoncry SPECIES_CLEFAIRY, CRY_MODE_NORMAL
	msgbox TeamGalacticEternaBuilding_4F_Text_ClefairyCry, MSGBOX_DEFAULT
	waitmoncry
	release
	end

TeamGalacticEternaBuilding_4F_EventScript_Buneary::
	lock
	faceplayer
	waitse
	playmoncry SPECIES_BUNEARY, CRY_MODE_NORMAL
	msgbox TeamGalacticEternaBuilding_4F_Text_BunearyCry, MSGBOX_DEFAULT
	waitmoncry
	release
	end

TeamGalacticEternaBuilding_4F_EventScript_Sign::
	msgbox TeamGalacticEternaBuilding_4F_Text_WorkingForWorldPeace, MSGBOX_SIGN
	end

TeamGalacticEternaBuilding_4F_Text_IWillDealWithYou:
	.string "Eh? Did you want something?\p"
	.string "How silly of me to even ask.\n"
	.string "You want to free the POKéMON.\p"
	.string "Fine!\n"
	.string "I, JUPITER, will deal with you.$"

TeamGalacticEternaBuilding_4F_Text_JupiterDefeat:
	.string "Losing to some child…\n"
	.string "Being careless cost me too much.$"

TeamGalacticEternaBuilding_4F_Text_WellArentYouTough:
	.string "Well, aren't you tough…?\p"
	.string "It's OK, though. Our official POKéMON-\n"
	.string "statue investigation is finished.\p"
	.string "MARS has collected energy from\n"
	.string "the VALLEY WINDWORKS.\p"
	.string "We're pretty much finished here.\n"
	.string "I'll let you in on one little thing.\p"
	.string "Our boss is researching the myths\n"
	.string "of ancient POKéMON.\p"
	.string "With the power of mythical POKéMON,\n"
	.string "he will become the ruler of SINNOH…\p"
	.string "I suggest you keep out of TEAM\n"
	.string "GALACTIC's affairs from now on.\p"
	.string "This is your last warning!$"

TeamGalacticEternaBuilding_4F_Text_ClefairyCry:
	.string "CLEFAIRY: Pi…$"

TeamGalacticEternaBuilding_4F_Text_BunearyCry:
	.string "BUNEARY: Myun…$"

TeamGalacticEternaBuilding_4F_Text_WhatsWithTeamGalactic:
	.string "What is it with this TEAM GALACTIC?\p"
	.string "Why did they take our POKéMON?\n"
	.string "What are they trying to accomplish?$"

TeamGalacticEternaBuilding_4F_Text_ThankYouCruiseByCycleShop:
	.string "I got my CLEFAIRY back, and it's all\n"
	.string "thanks to you!\p"
	.string "But that TEAM GALACTIC…\p"
	.string "They said, “CLEFAIRY came from space.\n"
	.string "Hand it over!”\p"
	.string "Their logic baffles me still.\n"
	.string "It's like they are from space!\p"
	.string "Anyway, they're gone now.\n"
	.string "Thank you very much!\p"
	.string "I can't thank you enough right now,\n"
	.string "but cruise by my CYCLE SHOP, OK?$"

TeamGalacticEternaBuilding_4F_Text_WorkingForWorldPeace:
	.string "Working for world peace.\n"
	.string "              TEAM GALACTIC$"
