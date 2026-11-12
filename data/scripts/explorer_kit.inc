@ Using the Explorer Kit outdoors pulls up a rock wall and starts the mining minigame.
EventScript_UseExplorerKit::
	lockall
	msgbox Text_UsedExplorerKit, MSGBOX_DEFAULT
	closemessage
	setvar VAR_0x8004, 1  @ vertical pan
	setvar VAR_0x8005, 0  @ horizontal pan
	setvar VAR_0x8006, 8  @ num shakes
	setvar VAR_0x8007, 4  @ shake delay
	special ShakeCamera
	waitstate
	msgbox Text_RockWallPulledUp, MSGBOX_DEFAULT
	closemessage
	fadescreen FADE_TO_BLACK
	special StartMiningGame
	releaseall
	end

Text_UsedExplorerKit:
	.string "{PLAYER} used the EXPLORER KIT!$"

Text_RockWallPulledUp:
	.string "A rock wall was pulled up from\n"
	.string "the ground!$"
