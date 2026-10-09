@ Oreburgh Mine B1F, after Platinum's (x - 2), on Emerald's cave tileset.
@ The cave tileset has no north exits, so the way up to Oreburgh City is a
@ ladder at the top, and another ladder leads down to B2F.
@ Platinum's philosophy student (the Flame Plate, after the Arceus event)
@ isn't added.
OreburghMine_B1F_MapScripts::
	.byte 0

OreburghMine_B1F_EventScript_Worker::
	msgbox OreburghMine_B1F_Text_EveryoneKeepsTheirOwnMons, MSGBOX_NPC
	end

OreburghMine_B1F_EventScript_Twin::
	msgbox OreburghMine_B1F_Text_SomeRocksStartedMoving, MSGBOX_NPC
	end

OreburghMine_B1F_Text_EveryoneKeepsTheirOwnMons:
	.string "Everyone that works in the coal mine\n"
	.string "keeps their own POKéMON with them.\p"
	.string "You may be challenged to battles!\n"
	.string "Hahaha!$"

OreburghMine_B1F_Text_SomeRocksStartedMoving:
	.string "I found what I thought were some\n"
	.string "rocks, then they started moving!\p"
	.string "They were POKéMON that looked just\n"
	.string "like rocks!$"
