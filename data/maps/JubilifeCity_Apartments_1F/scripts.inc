JubilifeCity_Apartments_1F_MapScripts::
	.byte 0

JubilifeCity_Apartments_1F_EventScript_OldWoman::
	msgbox JubilifeCity_Apartments_1F_Text_DevonWorkersLiveHere, MSGBOX_NPC
	end

JubilifeCity_Apartments_1F_EventScript_Skitty::
	lock
	faceplayer
	waitse
	playmoncry SPECIES_SKITTY, CRY_MODE_NORMAL
	msgbox JubilifeCity_Apartments_1F_Text_Skitty, MSGBOX_DEFAULT
	waitmoncry
	release
	end

JubilifeCity_Apartments_1F_Text_DevonWorkersLiveHere:
	.string "DEVON CORPORATION's workers live in\n"
	.string "this building.$"

JubilifeCity_Apartments_1F_Text_Skitty:
	.string "SKITTY: Gyaaaah!$"

