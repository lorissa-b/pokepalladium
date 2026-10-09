Route205South_MapScripts::
	map_script MAP_SCRIPT_ON_LOAD, Route205South_OnLoad
	map_script MAP_SCRIPT_ON_TRANSITION, Route205South_OnTransition
	map_script MAP_SCRIPT_ON_FRAME_TABLE, Route205South_OnFrame
	.byte 0

Route205South_OnLoad:
	call_if_eq VAR_ABNORMAL_WEATHER_LOCATION, ABNORMAL_WEATHER_ROUTE_205_SOUTH_WEST, AbnormalWeather_EventScript_PlaceTilesRoute205SouthWest
	call_if_eq VAR_ABNORMAL_WEATHER_LOCATION, ABNORMAL_WEATHER_ROUTE_205_SOUTH_EAST, AbnormalWeather_EventScript_PlaceTilesRoute205SouthEast
	end

Route205South_OnTransition:
	call_if_eq VAR_SHOULD_END_ABNORMAL_WEATHER, 1, AbnormalWeather_EventScript_HideMapNamePopup
	call_if_eq VAR_ABNORMAL_WEATHER_LOCATION, ABNORMAL_WEATHER_ROUTE_205_SOUTH_WEST, AbnormalWeather_StartGroudonWeather
	call_if_eq VAR_ABNORMAL_WEATHER_LOCATION, ABNORMAL_WEATHER_ROUTE_205_SOUTH_EAST, AbnormalWeather_StartGroudonWeather
	end

Route205South_OnFrame:
	map_script_2 VAR_SHOULD_END_ABNORMAL_WEATHER, 1, AbnormalWeather_EventScript_EndEventAndCleanup_1
	.2byte 0

Route205South_EventScript_Woman::
	msgbox Route205South_Text_NeverKnowWhenCavePokemonWillAppear, MSGBOX_NPC
	end

Route205South_EventScript_RouteSignJubilife::
	msgbox Route205South_Text_RouteSignJubilife, MSGBOX_SIGN
	end

Route205South_EventScript_MeteorFallsSign::
	msgbox Route205South_Text_MeteorFallsSign, MSGBOX_SIGN
	end

Route205South_EventScript_Timothy::
	trainerbattle_single TRAINER_TIMOTHY_1, Route205South_Text_TimothyIntro, Route205South_Text_TimothyDefeat, Route205South_EventScript_RegisterTimothy
	specialvar VAR_RESULT, ShouldTryRematchBattle
	goto_if_eq VAR_RESULT, TRUE, Route205South_EventScript_RematchTimothy
	msgbox Route205South_Text_TimothyPostBattle, MSGBOX_DEFAULT
	release
	end

Route205South_EventScript_RegisterTimothy::
	special PlayerFaceTrainerAfterBattle
	waitmovement 0
	msgbox Route205South_Text_TimothyRegister, MSGBOX_DEFAULT
	register_matchcall TRAINER_TIMOTHY_1
	release
	end

Route205South_EventScript_RematchTimothy::
	trainerbattle_rematch TRAINER_TIMOTHY_1, Route205South_Text_TimothyRematchIntro, Route205South_Text_TimothyRematchDefeat
	msgbox Route205South_Text_TimothyPostRematch, MSGBOX_AUTOCLOSE
	end

Route205South_EventScript_Koichi::
	trainerbattle_single TRAINER_KOICHI, Route205South_Text_KoichiIntro, Route205South_Text_KoichiDefeat
	msgbox Route205South_Text_KoichiPostBattle, MSGBOX_AUTOCLOSE
	end

Route205South_EventScript_Nob::
	trainerbattle_single TRAINER_NOB_1, Route205South_Text_NobIntro, Route205South_Text_NobDefeat, Route205South_EventScript_RegisterNob
	specialvar VAR_RESULT, ShouldTryRematchBattle
	goto_if_eq VAR_RESULT, TRUE, Route205South_EventScript_RematchNob
	msgbox Route205South_Text_NobPostBattle, MSGBOX_DEFAULT
	release
	end

Route205South_EventScript_RegisterNob::
	special PlayerFaceTrainerAfterBattle
	waitmovement 0
	msgbox Route205South_Text_NobRegister, MSGBOX_DEFAULT
	register_matchcall TRAINER_NOB_1
	release
	end

Route205South_EventScript_RematchNob::
	trainerbattle_rematch TRAINER_NOB_1, Route205South_Text_NobRematchIntro, Route205South_Text_NobRematchDefeat
	msgbox Route205South_Text_NobPostRematch, MSGBOX_AUTOCLOSE
	end

Route205South_EventScript_Cyndy::
	trainerbattle_single TRAINER_CYNDY_1, Route205South_Text_CyndyIntro, Route205South_Text_CyndyDefeat, Route205South_EventScript_RegisterCyndy
	specialvar VAR_RESULT, ShouldTryRematchBattle
	goto_if_eq VAR_RESULT, TRUE, Route205South_EventScript_RematchCyndy
	msgbox Route205South_Text_CyndyPostBattle, MSGBOX_DEFAULT
	release
	end

Route205South_EventScript_RegisterCyndy::
	special PlayerFaceTrainerAfterBattle
	waitmovement 0
	msgbox Route205South_Text_CyndyRegister, MSGBOX_DEFAULT
	register_matchcall TRAINER_CYNDY_1
	release
	end

Route205South_EventScript_RematchCyndy::
	trainerbattle_rematch TRAINER_CYNDY_1, Route205South_Text_CyndyRematchIntro, Route205South_Text_CyndyRematchDefeat
	msgbox Route205South_Text_CyndyPostRematch, MSGBOX_AUTOCLOSE
	end

Route205South_EventScript_Hector::
	trainerbattle_single TRAINER_HECTOR, Route205South_Text_HectorIntro, Route205South_Text_HectorDefeat
	msgbox Route205South_Text_HectorPostBattle, MSGBOX_AUTOCLOSE
	end

Route205South_EventScript_Kyra::
	trainerbattle_single TRAINER_KYRA, Route205South_Text_KyraIntro, Route205South_Text_KyraDefeat
	msgbox Route205South_Text_KyraPostBattle, MSGBOX_AUTOCLOSE
	end

Route205South_EventScript_Jaiden::
	trainerbattle_single TRAINER_JAIDEN, Route205South_Text_JaidenIntro, Route205South_Text_JaidenDefeat
	msgbox Route205South_Text_JaidenPostBattle, MSGBOX_AUTOCLOSE
	end

Route205South_EventScript_Alix::
	trainerbattle_single TRAINER_ALIX, Route205South_Text_AlixIntro, Route205South_Text_AlixDefeat
	msgbox Route205South_Text_AlixPostBattle, MSGBOX_AUTOCLOSE
	end

Route205South_EventScript_Helene::
	trainerbattle_single TRAINER_HELENE, Route205South_Text_HeleneIntro, Route205South_Text_HeleneDefeat
	msgbox Route205South_Text_HelenePostBattle, MSGBOX_AUTOCLOSE
	end

Route205South_EventScript_Marlene::
	trainerbattle_single TRAINER_MARLENE, Route205South_Text_MarleneIntro, Route205South_Text_MarleneDefeat
	msgbox Route205South_Text_MarlenePostBattle, MSGBOX_AUTOCLOSE
	end

Route205South_Text_NeverKnowWhenCavePokemonWillAppear:
	.string "Exploring a cave isn't like walking\n"
	.string "on a road.\p"
	.string "You never know when wild POKéMON will\n"
	.string "appear. It's full of suspense.$"

Route205South_Text_RouteSignJubilife:
	.string "ROUTE 205\n"
	.string "{DOWN_ARROW} RUSTBORO CITY$"

Route205South_Text_MeteorFallsSign:
	.string "METEOR FALLS\n"
	.string "FALLARBOR TOWN THROUGH HERE$"

