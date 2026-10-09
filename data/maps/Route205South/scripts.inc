@ Route 205 South, between Floaroma Town and Eterna Forest. Drawn over
@ Emerald's Route 115 from Platinum's map. The little girl on the bottom path
@ notices the player on the way east from Floaroma; talking to her clears the
@ Grunts out of Floaroma Meadow and sets VAR_VALLEY_WINDWORKS_STATE to 1. The
@ two Grunts on the bridge then push the player back until
@ FLAG_HIDE_ROUTE_205_SOUTH_GRUNTS is set, which in Platinum happens once
@ Valley Windworks is freed. Eterna Forest isn't here yet, so its cave mouths
@ (14-15, 5) lead back to themselves.
Route205South_MapScripts::
	.byte 0

Route205South_EventScript_LittleGirl::
	lock
	faceplayer
	goto_if_ge VAR_VALLEY_WINDWORKS_STATE, 1, Route205South_EventScript_IWantToSeeMyPapa
	goto Route205South_EventScript_HelpIWantToSeeMyPapa
	end

Route205South_EventScript_IWantToSeeMyPapa::
	msgbox Route205South_Text_IWantToSeeMyPapa, MSGBOX_DEFAULT
	release
	end

Route205South_EventScript_LittleGirlNoticeNorth2::
	lockall
	applymovement LOCALID_ROUTE205_SOUTH_LITTLE_GIRL, Route205South_Movement_LittleGirlNoticeNorthAndWalk
	waitmovement 0
	applymovement LOCALID_PLAYER, Common_Movement_WalkInPlaceFasterDown
	waitmovement 0
	goto Route205South_EventScript_HelpIWantToSeeMyPapa
	end

Route205South_EventScript_LittleGirlNoticeNorth1::
	lockall
	applymovement LOCALID_ROUTE205_SOUTH_LITTLE_GIRL, Route205South_Movement_LittleGirlNoticeNorth
	waitmovement 0
	applymovement LOCALID_PLAYER, Common_Movement_WalkInPlaceFasterDown
	waitmovement 0
	goto Route205South_EventScript_HelpIWantToSeeMyPapa
	end

Route205South_EventScript_LittleGirlNoticeSouth1::
	lockall
	applymovement LOCALID_ROUTE205_SOUTH_LITTLE_GIRL, Route205South_Movement_LittleGirlNoticeSouth
	waitmovement 0
	applymovement LOCALID_PLAYER, Common_Movement_WalkInPlaceFasterUp
	waitmovement 0
	goto Route205South_EventScript_HelpIWantToSeeMyPapa
	end

Route205South_EventScript_LittleGirlNoticeSouth2::
	lockall
	applymovement LOCALID_ROUTE205_SOUTH_LITTLE_GIRL, Route205South_Movement_LittleGirlNoticeSouthAndWalk
	waitmovement 0
	applymovement LOCALID_PLAYER, Common_Movement_WalkInPlaceFasterUp
	waitmovement 0
	goto Route205South_EventScript_HelpIWantToSeeMyPapa
	end

Route205South_EventScript_HelpIWantToSeeMyPapa::
	setflag FLAG_HIDE_FLOAROMA_TOWN_GRUNTS
	setvar VAR_VALLEY_WINDWORKS_STATE, 1
	msgbox Route205South_Text_HelpIWantToSeeMyPapa, MSGBOX_DEFAULT
	releaseall
	end

Route205South_Movement_LittleGirlNoticeNorthAndWalk:
	walk_in_place_faster_up
	emote_exclamation_mark
	delay_8
	walk_up
	step_end

Route205South_Movement_LittleGirlNoticeNorth:
	walk_in_place_faster_up
	emote_exclamation_mark
	delay_8
	step_end

Route205South_Movement_LittleGirlNoticeSouth:
	walk_in_place_faster_down
	emote_exclamation_mark
	delay_8
	step_end

Route205South_Movement_LittleGirlNoticeSouthAndWalk:
	walk_in_place_faster_down
	emote_exclamation_mark
	delay_8
	walk_down
	step_end

Route205South_EventScript_Grunts::
	lockall
	faceplayer
	applymovement LOCALID_ROUTE205_SOUTH_GRUNT_WEST, Common_Movement_WalkInPlaceFasterDown
	waitmovement 0
	msgbox Route205South_Text_TeamGalacticConductingResearch, MSGBOX_DEFAULT
	closemessage
	applymovement LOCALID_ROUTE205_SOUTH_GRUNT_EAST, Common_Movement_WalkInPlaceFasterDown
	waitmovement 0
	msgbox Route205South_Text_ResearchInvolvesCatchingPokemon, MSGBOX_DEFAULT
	releaseall
	end

@ Stepping between the Grunts on the bridge
Route205South_EventScript_GruntsPushBack::
	lockall
	applymovement LOCALID_ROUTE205_SOUTH_GRUNT_WEST, Common_Movement_WalkInPlaceFasterRight
	waitmovement 0
	msgbox Route205South_Text_TeamGalacticConductingResearch, MSGBOX_DEFAULT
	closemessage
	applymovement LOCALID_ROUTE205_SOUTH_GRUNT_EAST, Common_Movement_WalkInPlaceFasterLeft
	waitmovement 0
	msgbox Route205South_Text_ResearchInvolvesCatchingPokemon, MSGBOX_DEFAULT
	closemessage
	applymovement LOCALID_ROUTE205_SOUTH_GRUNT_WEST, Route205South_Movement_GruntPushBackPlayer
	applymovement LOCALID_PLAYER, Route205South_Movement_PlayerPushedBack
	waitmovement LOCALID_ROUTE205_SOUTH_GRUNT_WEST
	waitmovement LOCALID_PLAYER
	applymovement LOCALID_ROUTE205_SOUTH_GRUNT_EAST, Common_Movement_WalkInPlaceFasterDown
	waitmovement 0
	releaseall
	end

Route205South_Movement_GruntPushBackPlayer:
	delay_8
	walk_fast_right
	walk_in_place_faster_down
	delay_16
	walk_left
	walk_in_place_faster_down
	step_end

Route205South_Movement_PlayerPushedBack:
	walk_down
	walk_in_place_faster_up
	step_end

Route205South_EventScript_BugCatcher::
	msgbox Route205South_Text_ManyPoisonTypePokemon, MSGBOX_NPC
	end

Route205South_EventScript_Youngster::
	msgbox Route205South_Text_SlatherHoneyOnATree, MSGBOX_NPC
	end

Route205South_EventScript_Daniel::
	trainerbattle_single TRAINER_DEVAN, Route205South_Text_DanielIntro, Route205South_Text_DanielDefeat
	msgbox Route205South_Text_DanielPostBattle, MSGBOX_AUTOCLOSE
	end

Route205South_EventScript_Elizabeth::
	trainerbattle_single TRAINER_KYRA, Route205South_Text_ElizabethIntro, Route205South_Text_ElizabethDefeat
	msgbox Route205South_Text_ElizabethPostBattle, MSGBOX_AUTOCLOSE
	end

Route205South_EventScript_Jacob::
	trainerbattle_single TRAINER_KOICHI, Route205South_Text_JacobIntro, Route205South_Text_JacobDefeat
	msgbox Route205South_Text_JacobPostBattle, MSGBOX_AUTOCLOSE
	end

Route205South_EventScript_Siena::
	trainerbattle_single TRAINER_HECTOR, Route205South_Text_SienaIntro, Route205South_Text_SienaDefeat
	msgbox Route205South_Text_SienaPostBattle, MSGBOX_AUTOCLOSE
	end

Route205South_EventScript_Nicholas::
	trainerbattle_single TRAINER_JAIDEN, Route205South_Text_NicholasIntro, Route205South_Text_NicholasDefeat
	msgbox Route205South_Text_NicholasPostBattle, MSGBOX_AUTOCLOSE
	end

Route205South_EventScript_Kelsey::
	trainerbattle_single TRAINER_HELENE, Route205South_Text_KelseyIntro, Route205South_Text_KelseyDefeat
	msgbox Route205South_Text_KelseyPostBattle, MSGBOX_AUTOCLOSE
	end

Route205South_EventScript_Karina::
	trainerbattle_single TRAINER_ALIX, Route205South_Text_KarinaIntro, Route205South_Text_KarinaDefeat
	msgbox Route205South_Text_KarinaPostBattle, MSGBOX_AUTOCLOSE
	end

Route205South_EventScript_Zackary::
	trainerbattle_single TRAINER_MARLENE, Route205South_Text_ZackaryIntro, Route205South_Text_ZackaryDefeat
	msgbox Route205South_Text_ZackaryPostBattle, MSGBOX_AUTOCLOSE
	end

Route205South_EventScript_SignFloaromaTown::
	msgbox Route205South_Text_SignFloaromaTown, MSGBOX_SIGN
	end

Route205South_EventScript_SignEternaForest::
	msgbox Route205South_Text_SignEternaForest, MSGBOX_SIGN
	end

Route205South_EventScript_TrainerTips::
	msgbox Route205South_Text_TrainerTips, MSGBOX_SIGN
	end

Route205South_Text_HelpIWantToSeeMyPapa:
	.string "Help, help, TRAINER!\n"
	.string "Please, I want to see my papa.\p"
	.string "My papa and I lived at the VALLEY\n"
	.string "WINDWORKS.\p"
	.string "But then a whole bunch of people\n"
	.string "dressed like spacemen came…\p"
	.string "They kicked me out, and they're making\n"
	.string "my papa do something.\p"
	.string "Please, TRAINER!\n"
	.string "I miss my papa!$"

Route205South_Text_IWantToSeeMyPapa:
	.string "Please, TRAINER! I want to see my papa.\n"
	.string "He's at the VALLEY WINDWORKS…$"

Route205South_Text_TeamGalacticConductingResearch:
	.string "We, TEAM GALACTIC, are conducting\n"
	.string "research in the ETERNA FOREST!\p"
	.string "This is as far as you will go!$"

Route205South_Text_ResearchInvolvesCatchingPokemon:
	.string "Our research involves catching POKéMON\n"
	.string "in the flowery meadows of FLOAROMA.\p"
	.string "We are also covertly stealing the\n"
	.string "energy of the VALLEY WINDWORKS.\p"
	.string "But that is of no concern to you!$"

Route205South_Text_ManyPoisonTypePokemon:
	.string "Many POISON-type POKéMON make the\n"
	.string "ETERNA FOREST their home.\p"
	.string "But I didn't buy any ANTIDOTES.\n"
	.string "What should I do?\p"
	.string "Do I press on, or do I go back?$"

Route205South_Text_SlatherHoneyOnATree:
	.string "If you slather sweet HONEY on\n"
	.string "a sweet-smelling tree…\p"
	.string "POKéMON may be attracted by that\n"
	.string "sweet scent.$"

Route205South_Text_SignFloaromaTown:
	.string "ROUTE 205\n"
	.string "{LEFT_ARROW} FLOAROMA TOWN$"

Route205South_Text_SignEternaForest:
	.string "ROUTE 205\n"
	.string "{UP_ARROW} ETERNA FOREST$"

Route205South_Text_TrainerTips:
	.string "TRAINER TIPS\p"
	.string "The items placed in your BAG are\n"
	.string "automatically sorted to the\l"
	.string "appropriate POCKETS.\p"
	.string "It's very convenient!\n"
	.string "It's a TRAINER's best friend!$"
