JubilifeCity_JubilifeTV_2F_MapScripts::
	.byte 0

JubilifeCity_JubilifeTV_2F_EventScript_Cameraman::
	msgbox JubilifeCity_JubilifeTV_2F_Text_CameramanQuiet, MSGBOX_NPC
	end

JubilifeCity_JubilifeTV_2F_EventScript_Reporter::
	msgbox JubilifeCity_JubilifeTV_2F_Text_ReporterLiveFromJubilife, MSGBOX_NPC
	end

JubilifeCity_JubilifeTV_2F_Text_CameramanQuiet:
	.string "Shh! We're rolling!$"

JubilifeCity_JubilifeTV_2F_Text_ReporterLiveFromJubilife:
	.string "This is JUBILIFE TV, coming to you\n"
	.string "live from the heart of SINNOH!$"
