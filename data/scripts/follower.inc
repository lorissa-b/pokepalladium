@ Run instead of the player's step when they try to walk into tall grass with a
@ follower and no POKéMON (see TryFollowerStopTallGrass). Only the rival follows
@ before the player has a POKéMON, so this is his line.
EventScript_FollowerStopsTallGrass::
	lockall
	playse SE_PIN
	applywaitmovement LOCALID_FOLLOWER, Common_Movement_ExclamationMark
	facefollower
	msgbox Text_FollowerStopsTallGrass, MSGBOX_DEFAULT
	releaseall
	end

Text_FollowerStopsTallGrass:
	.string "{RIVAL}: Whoa, hold it, {PLAYER}!\p"
	.string "Wild POKéMON live in tall grass!\n"
	.string "We can't go in there without any\l"
	.string "POKéMON of our own!$"
