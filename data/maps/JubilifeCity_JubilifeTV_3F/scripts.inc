JubilifeCity_JubilifeTV_3F_MapScripts::
	.byte 0

JubilifeCity_JubilifeTV_3F_EventScript_DevonEmployee::
	msgbox JubilifeCity_JubilifeTV_3F_Text_PresidentCollectsRareStones, MSGBOX_NPC
	end

JubilifeCity_JubilifeTV_3F_EventScript_Woman::
	msgbox JubilifeCity_JubilifeTV_3F_Text_PresidentsSonAlsoCollectsRareStones, MSGBOX_NPC
	end

JubilifeCity_JubilifeTV_3F_Text_PresidentCollectsRareStones:
	.string "DEVON's PRESIDENT likes to collect\n"
	.string "rare stones.$"

JubilifeCity_JubilifeTV_3F_Text_PresidentsSonAlsoCollectsRareStones:
	.string "I think the PRESIDENT's son also\n"
	.string "collects rare stones.$"

