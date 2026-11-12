@ Team Galactic Eterna Building 3F, from Platinum's. Its four stairways
@ lead down to 2F's two and up to 4F's two. Everyone here is hidden once
@ Jupiter is beaten on 4F (FLAG_HIDE_ETERNA_CITY_GALACTIC_GRUNTS).
TeamGalacticEternaBuilding_3F_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, TeamGalacticEternaBuilding_3F_OnTransition
	.byte 0

TeamGalacticEternaBuilding_3F_OnTransition:
	call TeamGalacticEternaBuilding_1F_EventScript_TryPlayGalacticMusic
	end

TeamGalacticEternaBuilding_3F_EventScript_GruntM::
	msgbox TeamGalacticEternaBuilding_3F_Text_OrdinaryPeopleCantUnderstand, MSGBOX_NPC
	end

TeamGalacticEternaBuilding_3F_EventScript_Grunt::
	trainerbattle_single TRAINER_GRUNT_AQUA_HIDEOUT_7, TeamGalacticEternaBuilding_3F_Text_GruntIntro, TeamGalacticEternaBuilding_3F_Text_GruntDefeat
	msgbox TeamGalacticEternaBuilding_3F_Text_GruntPostBattle, MSGBOX_AUTOCLOSE
	end

TeamGalacticEternaBuilding_3F_EventScript_Travon::
	trainerbattle_single TRAINER_GRUNT_AQUA_HIDEOUT_8, TeamGalacticEternaBuilding_3F_Text_TravonIntro, TeamGalacticEternaBuilding_3F_Text_TravonDefeat
	msgbox TeamGalacticEternaBuilding_3F_Text_TravonPostBattle, MSGBOX_AUTOCLOSE
	end

TeamGalacticEternaBuilding_3F_EventScript_Sign::
	msgbox TeamGalacticEternaBuilding_3F_Text_FollowThisPath, MSGBOX_SIGN
	end

TeamGalacticEternaBuilding_3F_Text_OrdinaryPeopleCantUnderstand:
	.string "Ordinary people like you can't\n"
	.string "understand TEAM GALACTIC's grand\l"
	.string "designs!\p"
	.string "Come to think of it… I don't even\n"
	.string "know what's going on…$"

TeamGalacticEternaBuilding_3F_Text_FollowThisPath:
	.string "Question not and follow this path!\n"
	.string "The road to success!$"

TeamGalacticEternaBuilding_3F_Text_GruntIntro:
	.string "Why, thank you!\p"
	.string "We love it when people come to us\n"
	.string "to donate their POKéMON!$"

TeamGalacticEternaBuilding_3F_Text_GruntDefeat:
	.string "What do you think you're doing?!$"

TeamGalacticEternaBuilding_3F_Text_GruntPostBattle:
	.string "You can catch POKéMON anywhere.\n"
	.string "What's so special about them?\p"
	.string "TRAINERS that lose theirs should\n"
	.string "quit whining and catch some more!$"

TeamGalacticEternaBuilding_3F_Text_TravonIntro:
	.string "Our mission is to implement our\n"
	.string "operations without question.\p"
	.string "It is all for the new world order\n"
	.string "that our boss will lead!$"

TeamGalacticEternaBuilding_3F_Text_TravonDefeat:
	.string "This won't go unpunished…$"

TeamGalacticEternaBuilding_3F_Text_TravonPostBattle:
	.string "I'm willing to do anything to get ahead\n"
	.string "in TEAM GALACTIC.\p"
	.string "One day you'll see what we are all\n"
	.string "about.\p"
	.string "It's wonder, compassion, splendor,\n"
	.string "humor, power, awe, and tragedy!$"
