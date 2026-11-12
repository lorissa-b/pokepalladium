@ Team Galactic Eterna Building 2F, from Platinum's. Its four stairways
@ lead down to 1F's two and up to 3F's two. Everyone here is hidden once
@ Jupiter is beaten on 4F (FLAG_HIDE_ETERNA_CITY_GALACTIC_GRUNTS).
TeamGalacticEternaBuilding_2F_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, TeamGalacticEternaBuilding_2F_OnTransition
	.byte 0

TeamGalacticEternaBuilding_2F_OnTransition:
	call TeamGalacticEternaBuilding_1F_EventScript_TryPlayGalacticMusic
	end

TeamGalacticEternaBuilding_2F_EventScript_GruntM::
	msgbox TeamGalacticEternaBuilding_2F_Text_WorkingToFindEnergySources, MSGBOX_NPC
	end

TeamGalacticEternaBuilding_2F_EventScript_GruntF::
	msgbox TeamGalacticEternaBuilding_2F_Text_ResearchOnFormsOfEnergy, MSGBOX_NPC
	end

TeamGalacticEternaBuilding_2F_EventScript_Grunt1::
	trainerbattle_single TRAINER_GRUNT_AQUA_HIDEOUT_2, TeamGalacticEternaBuilding_2F_Text_Grunt1Intro, TeamGalacticEternaBuilding_2F_Text_Grunt1Defeat
	msgbox TeamGalacticEternaBuilding_2F_Text_Grunt1PostBattle, MSGBOX_AUTOCLOSE
	end

TeamGalacticEternaBuilding_2F_EventScript_Grunt2::
	trainerbattle_single TRAINER_GRUNT_AQUA_HIDEOUT_6, TeamGalacticEternaBuilding_2F_Text_Grunt2Intro, TeamGalacticEternaBuilding_2F_Text_Grunt2Defeat
	msgbox TeamGalacticEternaBuilding_2F_Text_Grunt2PostBattle, MSGBOX_AUTOCLOSE
	end

TeamGalacticEternaBuilding_2F_EventScript_Sign::
	msgbox TeamGalacticEternaBuilding_2F_Text_MakeProgressTogether, MSGBOX_SIGN
	end

TeamGalacticEternaBuilding_2F_Text_WorkingToFindEnergySources:
	.string "TEAM GALACTIC is working hard to find\n"
	.string "new energy sources for the future!$"

TeamGalacticEternaBuilding_2F_Text_ResearchOnFormsOfEnergy:
	.string "We are conducting research on new\n"
	.string "forms of energy!\p"
	.string "We want to harness the mysterious\n"
	.string "power of POKéMON and use it as energy!$"

TeamGalacticEternaBuilding_2F_Text_MakeProgressTogether:
	.string "Let us make progress together!\n"
	.string "             TEAM GALACTIC$"

TeamGalacticEternaBuilding_2F_Text_Grunt1Intro:
	.string "Our objectives are incredible!\n"
	.string "Too incredible for me to understand!\p"
	.string "I'll fight blindly to defend them,\n"
	.string "and your intrusion won't be forgiven!$"

TeamGalacticEternaBuilding_2F_Text_Grunt1Defeat:
	.string "What was I fighting for, again?$"

TeamGalacticEternaBuilding_2F_Text_Grunt1PostBattle:
	.string "What do you have against taking\n"
	.string "POKéMON from others?\p"
	.string "Look in any field or forest!\n"
	.string "Those things aren't exactly rare!$"

TeamGalacticEternaBuilding_2F_Text_Grunt2Intro:
	.string "Let's see how good this POKéMON is!\n"
	.string "I stole it from some weak TRAINER.$"

TeamGalacticEternaBuilding_2F_Text_Grunt2Defeat:
	.string "What is this?!$"

TeamGalacticEternaBuilding_2F_Text_Grunt2PostBattle:
	.string "Wimpy little POKéMON…\p"
	.string "When I get promoted, I'll demand\n"
	.string "better stolen POKéMON.$"
