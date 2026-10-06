SandgemTown_PokemonResearchLab_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, SandgemTown_PokemonResearchLab_OnTransition
	map_script MAP_SCRIPT_ON_WARP_INTO_MAP_TABLE, SandgemTown_PokemonResearchLab_OnWarp
	map_script MAP_SCRIPT_ON_FRAME_TABLE, SandgemTown_PokemonResearchLab_OnFrame
	.byte 0

	@ State descriptions for VAR_DEX_UPGRADE_JOHTO_STARTER_STATE
	@ 1: Beat Elite Four, Dex upgrade ready
	@ 2: Received National Dex
	@ 3: Left lab after receiving National Dex
	@ 4: Entered lab after catching all Hoenn mons
	@ 5: Birch told player to choose Johto starter
	@ 6: Chose Johto starter

	@ State descriptions for VAR_BIRCH_LAB_STATE
	@ 1: Birch's assistant is leading the player in from Sandgem Town
	@ 5: Received pokedex

SandgemTown_PokemonResearchLab_OnTransition:
	call Common_EventScript_SetupRivalGfxId
	call Common_EventScript_SetupAssistantGfxId
	call ProfBirch_EventScript_UpdateLocation
	goto_if_ge VAR_DEX_UPGRADE_JOHTO_STARTER_STATE, 6, SandgemTown_PokemonResearchLab_EventScript_SetAfterJohtoStarterLayout
	goto_if_ge VAR_DEX_UPGRADE_JOHTO_STARTER_STATE, 4, SandgemTown_PokemonResearchLab_EventScript_SetJohtoStarterLayout
	goto_if_eq VAR_DEX_UPGRADE_JOHTO_STARTER_STATE, 3, SandgemTown_PokemonResearchLab_EventScript_CheckReadyForJohtoStarter
	end

SandgemTown_PokemonResearchLab_EventScript_CheckReadyForJohtoStarter::
	specialvar VAR_RESULT, HasAllHoennMons
	goto_if_eq VAR_RESULT, TRUE, SandgemTown_PokemonResearchLab_EventScript_SetReadyForJohtoStarter
	setobjectmovementtype LOCALID_BIRCHS_LAB_RIVAL, MOVEMENT_TYPE_WANDER_UP_AND_DOWN
	setobjectxyperm LOCALID_BIRCHS_LAB_RIVAL, 5, 10
	end

SandgemTown_PokemonResearchLab_EventScript_SetReadyForJohtoStarter::
	setvar VAR_DEX_UPGRADE_JOHTO_STARTER_STATE, 4
	goto SandgemTown_PokemonResearchLab_EventScript_SetJohtoStarterLayout

SandgemTown_PokemonResearchLab_EventScript_SetJohtoStarterLayout::
	setmaplayoutindex LAYOUT_SANDGEM_TOWN_POKEMON_RESEARCH_LAB_WITH_TABLE
	end

SandgemTown_PokemonResearchLab_EventScript_SetAfterJohtoStarterLayout::
	setmaplayoutindex LAYOUT_SANDGEM_TOWN_POKEMON_RESEARCH_LAB_WITH_TABLE
	setobjectmovementtype LOCALID_BIRCHS_LAB_RIVAL, MOVEMENT_TYPE_WANDER_UP_AND_DOWN
	setobjectxyperm LOCALID_BIRCHS_LAB_RIVAL, 5, 10
	end

SandgemTown_PokemonResearchLab_OnWarp:
	map_script_2 VAR_BIRCH_LAB_STATE, 1, SandgemTown_PokemonResearchLab_EventScript_SetObjectPosForMeetBirch
	map_script_2 VAR_DEX_UPGRADE_JOHTO_STARTER_STATE, 1, SandgemTown_PokemonResearchLab_EventScript_SetObjectPosForDexUpgrade
	map_script_2 VAR_DEX_UPGRADE_JOHTO_STARTER_STATE, 2, SandgemTown_PokemonResearchLab_EventScript_SetObjectPosForDexUpgrade
	map_script_2 VAR_DEX_UPGRADE_JOHTO_STARTER_STATE, 3, SandgemTown_PokemonResearchLab_EventScript_AddRivalObject
	map_script_2 VAR_DEX_UPGRADE_JOHTO_STARTER_STATE, 6, SandgemTown_PokemonResearchLab_EventScript_AddRivalObject
	map_script_2 VAR_DEX_UPGRADE_JOHTO_STARTER_STATE, 4, SandgemTown_PokemonResearchLab_EventScript_SetObjectPosForJohtoStarters
	map_script_2 VAR_DEX_UPGRADE_JOHTO_STARTER_STATE, 5, SandgemTown_PokemonResearchLab_EventScript_SetObjectPosForJohtoStarters
	.2byte 0

@ The player comes in at 6, 11 with the assistant just ahead of them
SandgemTown_PokemonResearchLab_EventScript_SetObjectPosForMeetBirch::
	turnobject LOCALID_PLAYER, DIR_NORTH
	addobject LOCALID_BIRCHS_LAB_ASSISTANT
	setobjectxy LOCALID_BIRCHS_LAB_ASSISTANT, 6, 10
	turnobject LOCALID_BIRCHS_LAB_ASSISTANT, DIR_NORTH
	end

SandgemTown_PokemonResearchLab_EventScript_SetObjectPosForDexUpgrade::
	turnobject LOCALID_PLAYER, DIR_NORTH
	addobject LOCALID_BIRCHS_LAB_BIRCH
	addobject LOCALID_BIRCHS_LAB_RIVAL
	turnobject LOCALID_BIRCHS_LAB_RIVAL, DIR_NORTH
	setobjectxy LOCALID_BIRCHS_LAB_RIVAL, 5, 5
	turnobject LOCALID_BIRCHS_LAB_BIRCH, DIR_SOUTH
	setobjectxy LOCALID_BIRCHS_LAB_BIRCH, 6, 4
	turnobject LOCALID_BIRCHS_LAB_AIDE, DIR_EAST
	setobjectxy LOCALID_BIRCHS_LAB_AIDE, 10, 10
	end

SandgemTown_PokemonResearchLab_EventScript_AddRivalObject::
	addobject LOCALID_BIRCHS_LAB_RIVAL
	end

SandgemTown_PokemonResearchLab_EventScript_SetObjectPosForJohtoStarters::
	addobject LOCALID_BIRCHS_LAB_CYNDAQUIL_BALL
	addobject LOCALID_BIRCHS_LAB_TOTODILE_BALL
	addobject LOCALID_BIRCHS_LAB_CHIKORITA_BALL
	addobject LOCALID_BIRCHS_LAB_RIVAL
	turnobject LOCALID_BIRCHS_LAB_CYNDAQUIL_BALL, DIR_SOUTH
	setobjectxy LOCALID_BIRCHS_LAB_CYNDAQUIL_BALL, 8, 4
	turnobject LOCALID_BIRCHS_LAB_TOTODILE_BALL, DIR_SOUTH
	setobjectxy LOCALID_BIRCHS_LAB_TOTODILE_BALL, 9, 4
	turnobject LOCALID_BIRCHS_LAB_CHIKORITA_BALL, DIR_SOUTH
	setobjectxy LOCALID_BIRCHS_LAB_CHIKORITA_BALL, 10, 4
	turnobject LOCALID_BIRCHS_LAB_BIRCH, DIR_SOUTH
	setobjectxy LOCALID_BIRCHS_LAB_BIRCH, 6, 4
	turnobject LOCALID_BIRCHS_LAB_RIVAL, DIR_EAST
	setobjectxy LOCALID_BIRCHS_LAB_RIVAL, 5, 5
	end

SandgemTown_PokemonResearchLab_OnFrame:
	map_script_2 VAR_BIRCH_LAB_STATE, 1, SandgemTown_PokemonResearchLab_EventScript_MeetBirch
	map_script_2 VAR_DEX_UPGRADE_JOHTO_STARTER_STATE, 1, SandgemTown_PokemonResearchLab_EventScript_UpgradeToNationalDex
	map_script_2 VAR_DEX_UPGRADE_JOHTO_STARTER_STATE, 4, SandgemTown_PokemonResearchLab_EventScript_ChooseJohtoStarter
	.2byte 0

@ The assistant leads the player up to Birch (6, 4). He comments on the
@ starter, offers a nickname, hands over the POKéDEX and sends the player home.
SandgemTown_PokemonResearchLab_EventScript_MeetBirch::
	lockall
	applymovement LOCALID_BIRCHS_LAB_ASSISTANT, SandgemTown_PokemonResearchLab_Movement_AssistantLeadToBirch
	applymovement LOCALID_PLAYER, SandgemTown_PokemonResearchLab_Movement_PlayerFollowToBirch
	waitmovement 0
	msgbox SandgemTown_PokemonResearchLab_Text_AssistantBroughtPlayer, MSGBOX_DEFAULT
	closemessage
	applywaitmovement LOCALID_BIRCHS_LAB_ASSISTANT, Common_Movement_WalkInPlaceFasterLeft
	bufferleadmonspeciesname STR_VAR_1
	msgbox SandgemTown_PokemonResearchLab_Text_MonHasGrown, MSGBOX_YESNO
	call_if_eq VAR_RESULT, YES, SandgemTown_PokemonResearchLab_EventScript_NicknameLeadMon
	msgbox SandgemTown_PokemonResearchLab_Text_TakeThisPokedex, MSGBOX_DEFAULT
	call SandgemTown_PokemonResearchLab_EventScript_ReceivePokedex
	msgbox SandgemTown_PokemonResearchLab_Text_GoHomeToMother, MSGBOX_DEFAULT
	closemessage
	@ The assistant waits on Route 202 for the catching tutorial, then comes back
	setvar VAR_BIRCH_LAB_STATE, 5
	setflag FLAG_ADVENTURE_STARTED
	setvar VAR_TWINLEAF_RIVAL_STATE, 4
	@ Back home, the rival's mom brings a TOWN MAP and Mom has RUNNING SHOES
	setvar VAR_TWINLEAF_TOWN_STATE, 3
	releaseall
	end

SandgemTown_PokemonResearchLab_EventScript_NicknameLeadMon::
	setvar VAR_0x8004, 0
	call Common_EventScript_NameReceivedPartyMon
	return

@ Up to 7, 5, beside the spot in front of Birch
SandgemTown_PokemonResearchLab_Movement_AssistantLeadToBirch:
	walk_up
	walk_up
	walk_up
	walk_up
	walk_right
	walk_up
	walk_in_place_faster_up
	step_end

@ Two steps behind the assistant, up to 6, 5 in front of Birch
SandgemTown_PokemonResearchLab_Movement_PlayerFollowToBirch:
	delay_16
	delay_16
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	step_end

SandgemTown_PokemonResearchLab_EventScript_Assistant::
	msgbox SandgemTown_PokemonResearchLab_Text_AssistantPokedexFills, MSGBOX_NPC
	end

SandgemTown_PokemonResearchLab_EventScript_UpgradeToNationalDex::
	lockall
	delay 30
	msgbox SandgemTown_PokemonResearchLab_Text_OtherRegionsUpgradeToNational, MSGBOX_DEFAULT
	closemessage
	applywaitmovement LOCALID_BIRCHS_LAB_BIRCH, SandgemTown_PokemonResearchLab_Movement_BirchRetrievePokedexes
	delay 20
	playse SE_CLICK
	delay 10
	playse SE_CLICK
	delay 10
	playse SE_CLICK
	delay 10
	playse SE_CLICK
	delay 20
	applymovement LOCALID_BIRCHS_LAB_RIVAL, Common_Movement_WalkInPlaceFasterRight
	applymovement LOCALID_PLAYER, Common_Movement_WalkInPlaceFasterLeft
	waitmovement 0
	checkplayergender
	call_if_eq VAR_RESULT, MALE, SandgemTown_PokemonResearchLab_EventScript_MayUpgradeComment
	call_if_eq VAR_RESULT, FEMALE, SandgemTown_PokemonResearchLab_EventScript_BrendanUpgradeComment
	playse SE_PC_ON
	waitse
	delay 20
	applywaitmovement LOCALID_BIRCHS_LAB_BIRCH, SandgemTown_PokemonResearchLab_Movement_BirchReturnPokedex
	applymovement LOCALID_BIRCHS_LAB_RIVAL, Common_Movement_WalkInPlaceFasterUp
	applymovement LOCALID_PLAYER, Common_Movement_WalkInPlaceFasterUp
	waitmovement 0
	msgbox SandgemTown_PokemonResearchLab_Text_OkayAllDone, MSGBOX_DEFAULT
	playfanfare MUS_OBTAIN_ITEM
	message SandgemTown_PokemonResearchLab_Text_PokedexUpgradedToNational
	waitmessage
	waitfanfare
	setflag FLAG_SYS_NATIONAL_DEX
	special EnableNationalPokedex
	msgbox SandgemTown_PokemonResearchLab_Text_GrassyPatchWaiting2, MSGBOX_DEFAULT
	closemessage
	setflag FLAG_HIDE_TWINLEAF_TOWN_BIRCH
	setflag FLAG_HIDE_TWINLEAF_TOWN_RIVAL
	setvar VAR_DEX_UPGRADE_JOHTO_STARTER_STATE, 2
	setvar VAR_SCOTT_BF_CALL_STEP_COUNTER, 0
	setflag FLAG_SCOTT_CALL_BATTLE_FRONTIER
	releaseall
	end

SandgemTown_PokemonResearchLab_EventScript_MayUpgradeComment::
	msgbox SandgemTown_PokemonResearchLab_Text_MayUpgradeSoCool, MSGBOX_DEFAULT
	closemessage
	return

SandgemTown_PokemonResearchLab_EventScript_BrendanUpgradeComment::
	msgbox SandgemTown_PokemonResearchLab_Text_BrendanYouCanThankMe, MSGBOX_DEFAULT
	closemessage
	return

SandgemTown_PokemonResearchLab_Movement_BirchRetrievePokedexes:
	walk_left
	walk_in_place_faster_down
	delay_16
	delay_16
	walk_right
	walk_in_place_faster_down
	delay_16
	delay_16
	walk_right
	walk_down
	walk_down
	walk_down
	walk_down
	walk_right
	walk_right
	walk_right
	walk_in_place_faster_up
	step_end

SandgemTown_PokemonResearchLab_Movement_BirchReturnPokedex:
	walk_left
	walk_left
	walk_left
	walk_up
	walk_up
	walk_up
	walk_up
	walk_left
	walk_in_place_faster_down
	step_end

SandgemTown_PokemonResearchLab_EventScript_ChooseJohtoStarter::
	lockall
	applywaitmovement LOCALID_PLAYER, SandgemTown_PokemonResearchLab_Movement_PlayerEnterLabForJohtoStarter
	msgbox SandgemTown_PokemonResearchLab_Text_CompletedDexChoosePokemon, MSGBOX_DEFAULT
	setvar VAR_DEX_UPGRADE_JOHTO_STARTER_STATE, 5
	releaseall
	end

SandgemTown_PokemonResearchLab_Movement_PlayerEnterLabForJohtoStarter:
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	step_end

SandgemTown_PokemonResearchLab_EventScript_Aide::
	msgbox SandgemTown_PokemonResearchLab_Text_ProfStudiesHabitats, MSGBOX_NPC
	end

SandgemTown_PokemonResearchLab_EventScript_Cyndaquil::
	release
	goto_if_ge VAR_DEX_UPGRADE_JOHTO_STARTER_STATE, 6, SandgemTown_PokemonResearchLab_EventScript_AlreadyChoseJohtoStarter
	applywaitmovement LOCALID_BIRCHS_LAB_BIRCH, Common_Movement_WalkInPlaceFasterRight
	showmonpic SPECIES_CYNDAQUIL, 10, 3
	msgbox SandgemTown_PokemonResearchLab_Text_YoullTakeCyndaquil, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, SandgemTown_PokemonResearchLab_EventScript_TakeYourTime
	goto SandgemTown_PokemonResearchLab_EventScript_GiveCyndaquil
	end

SandgemTown_PokemonResearchLab_EventScript_Totodile::
	release
	goto_if_ge VAR_DEX_UPGRADE_JOHTO_STARTER_STATE, 6, SandgemTown_PokemonResearchLab_EventScript_AlreadyChoseJohtoStarter
	applywaitmovement LOCALID_BIRCHS_LAB_BIRCH, Common_Movement_WalkInPlaceFasterRight
	showmonpic SPECIES_TOTODILE, 10, 3
	msgbox SandgemTown_PokemonResearchLab_Text_YoullTakeTotodile, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, SandgemTown_PokemonResearchLab_EventScript_TakeYourTime
	goto SandgemTown_PokemonResearchLab_EventScript_GiveTotodile
	end

SandgemTown_PokemonResearchLab_EventScript_Chikorita::
	release
	goto_if_ge VAR_DEX_UPGRADE_JOHTO_STARTER_STATE, 6, SandgemTown_PokemonResearchLab_EventScript_AlreadyChoseJohtoStarter
	applywaitmovement LOCALID_BIRCHS_LAB_BIRCH, Common_Movement_WalkInPlaceFasterRight
	showmonpic SPECIES_CHIKORITA, 10, 3
	msgbox SandgemTown_PokemonResearchLab_Text_YoullTakeChikorita, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, SandgemTown_PokemonResearchLab_EventScript_TakeYourTime
	goto SandgemTown_PokemonResearchLab_EventScript_GiveChikorita
	end

SandgemTown_PokemonResearchLab_EventScript_AlreadyChoseJohtoStarter::
	msgbox SandgemTown_PokemonResearchLab_Text_BetterLeaveOthersAlone, MSGBOX_DEFAULT
	releaseall
	end

SandgemTown_PokemonResearchLab_EventScript_TakeYourTime::
	hidemonpic
	msgbox SandgemTown_PokemonResearchLab_Text_TakeYourTimeAllInvaluable, MSGBOX_DEFAULT
	releaseall
	end

SandgemTown_PokemonResearchLab_EventScript_GiveCyndaquil::
	bufferspeciesname STR_VAR_1, SPECIES_CYNDAQUIL
	setvar VAR_TEMP_TRANSFERRED_SPECIES, SPECIES_CYNDAQUIL
	givemon SPECIES_CYNDAQUIL, 5
	goto_if_eq VAR_RESULT, MON_GIVEN_TO_PARTY, SandgemTown_PokemonResearchLab_EventScript_SendCyndaquilToParty
	goto_if_eq VAR_RESULT, MON_GIVEN_TO_PC, SandgemTown_PokemonResearchLab_EventScript_SendCyndaquilToPC
	hidemonpic
	goto Common_EventScript_NoMoreRoomForPokemon
	end

SandgemTown_PokemonResearchLab_EventScript_SendCyndaquilToParty::
	call SandgemTown_PokemonResearchLab_EventScript_ReceivedJohtoStarter
	removeobject LOCALID_BIRCHS_LAB_CYNDAQUIL_BALL
	msgbox gText_NicknameThisPokemon, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, SandgemTown_PokemonResearchLab_EventScript_ReceivedCyndaquil
	call Common_EventScript_GetGiftMonPartySlot
	call Common_EventScript_NameReceivedPartyMon
	goto SandgemTown_PokemonResearchLab_EventScript_ReceivedCyndaquil
	end

SandgemTown_PokemonResearchLab_EventScript_SendCyndaquilToPC::
	call SandgemTown_PokemonResearchLab_EventScript_ReceivedJohtoStarter
	removeobject LOCALID_BIRCHS_LAB_CYNDAQUIL_BALL
	msgbox gText_NicknameThisPokemon, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, SandgemTown_PokemonResearchLab_EventScript_CyndaquilTransferredToPC
	call Common_EventScript_NameReceivedBoxMon
	goto SandgemTown_PokemonResearchLab_EventScript_CyndaquilTransferredToPC
	end

SandgemTown_PokemonResearchLab_EventScript_CyndaquilTransferredToPC::
	call Common_EventScript_TransferredToPC
	goto SandgemTown_PokemonResearchLab_EventScript_ReceivedCyndaquil
	end

SandgemTown_PokemonResearchLab_EventScript_ReceivedCyndaquil::
	hidemonpic
	msgbox SandgemTown_PokemonResearchLab_Text_GrassyPatchWaiting, MSGBOX_DEFAULT
	setvar VAR_DEX_UPGRADE_JOHTO_STARTER_STATE, 6
	releaseall
	end

SandgemTown_PokemonResearchLab_EventScript_GiveTotodile::
	bufferspeciesname STR_VAR_1, SPECIES_TOTODILE
	setvar VAR_TEMP_TRANSFERRED_SPECIES, SPECIES_TOTODILE
	givemon SPECIES_TOTODILE, 5
	goto_if_eq VAR_RESULT, MON_GIVEN_TO_PARTY, SandgemTown_PokemonResearchLab_EventScript_SendTotodileToParty
	goto_if_eq VAR_RESULT, MON_GIVEN_TO_PC, SandgemTown_PokemonResearchLab_EventScript_SendTotodileToPC
	hidemonpic
	goto Common_EventScript_NoMoreRoomForPokemon
	end

SandgemTown_PokemonResearchLab_EventScript_SendTotodileToParty::
	call SandgemTown_PokemonResearchLab_EventScript_ReceivedJohtoStarter
	removeobject LOCALID_BIRCHS_LAB_TOTODILE_BALL
	msgbox gText_NicknameThisPokemon, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, SandgemTown_PokemonResearchLab_EventScript_ReceivedTotodile
	call Common_EventScript_GetGiftMonPartySlot
	call Common_EventScript_NameReceivedPartyMon
	goto SandgemTown_PokemonResearchLab_EventScript_ReceivedTotodile
	end

SandgemTown_PokemonResearchLab_EventScript_SendTotodileToPC::
	call SandgemTown_PokemonResearchLab_EventScript_ReceivedJohtoStarter
	removeobject LOCALID_BIRCHS_LAB_TOTODILE_BALL
	msgbox gText_NicknameThisPokemon, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, SandgemTown_PokemonResearchLab_EventScript_TotodileTransferredToPC
	call Common_EventScript_NameReceivedBoxMon
	goto SandgemTown_PokemonResearchLab_EventScript_TotodileTransferredToPC
	end

SandgemTown_PokemonResearchLab_EventScript_TotodileTransferredToPC::
	call Common_EventScript_TransferredToPC
	goto SandgemTown_PokemonResearchLab_EventScript_ReceivedTotodile
	end

SandgemTown_PokemonResearchLab_EventScript_ReceivedTotodile::
	hidemonpic
	msgbox SandgemTown_PokemonResearchLab_Text_GrassyPatchWaiting, MSGBOX_DEFAULT
	setvar VAR_DEX_UPGRADE_JOHTO_STARTER_STATE, 6
	releaseall
	end

SandgemTown_PokemonResearchLab_EventScript_GiveChikorita::
	bufferspeciesname STR_VAR_1, SPECIES_CHIKORITA
	setvar VAR_TEMP_TRANSFERRED_SPECIES, SPECIES_CHIKORITA
	givemon SPECIES_CHIKORITA, 5
	goto_if_eq VAR_RESULT, MON_GIVEN_TO_PARTY, SandgemTown_PokemonResearchLab_EventScript_SendChikoritaToParty
	goto_if_eq VAR_RESULT, MON_GIVEN_TO_PC, SandgemTown_PokemonResearchLab_EventScript_SendChikoritaToPC
	hidemonpic
	goto Common_EventScript_NoMoreRoomForPokemon
	end

SandgemTown_PokemonResearchLab_EventScript_SendChikoritaToParty::
	call SandgemTown_PokemonResearchLab_EventScript_ReceivedJohtoStarter
	removeobject LOCALID_BIRCHS_LAB_CHIKORITA_BALL
	msgbox gText_NicknameThisPokemon, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, SandgemTown_PokemonResearchLab_EventScript_ReceivedChikorita
	call Common_EventScript_GetGiftMonPartySlot
	call Common_EventScript_NameReceivedPartyMon
	goto SandgemTown_PokemonResearchLab_EventScript_ReceivedChikorita
	end

SandgemTown_PokemonResearchLab_EventScript_SendChikoritaToPC::
	call SandgemTown_PokemonResearchLab_EventScript_ReceivedJohtoStarter
	removeobject LOCALID_BIRCHS_LAB_CHIKORITA_BALL
	msgbox gText_NicknameThisPokemon, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, SandgemTown_PokemonResearchLab_EventScript_ChikoritaTransferredToPC
	call Common_EventScript_NameReceivedBoxMon
	goto SandgemTown_PokemonResearchLab_EventScript_ChikoritaTransferredToPC
	end

SandgemTown_PokemonResearchLab_EventScript_ChikoritaTransferredToPC::
	call Common_EventScript_TransferredToPC
	goto SandgemTown_PokemonResearchLab_EventScript_ReceivedChikorita
	end

SandgemTown_PokemonResearchLab_EventScript_ReceivedChikorita::
	hidemonpic
	msgbox SandgemTown_PokemonResearchLab_Text_GrassyPatchWaiting, MSGBOX_DEFAULT
	setvar VAR_DEX_UPGRADE_JOHTO_STARTER_STATE, 6
	releaseall
	end

SandgemTown_PokemonResearchLab_EventScript_ReceivedJohtoStarter::
	playfanfare MUS_OBTAIN_ITEM
	message SandgemTown_PokemonResearchLab_Text_ReceivedJohtoStarter
	waitmessage
	waitfanfare
	return

SandgemTown_PokemonResearchLab_EventScript_Birch::
	lock
	faceplayer
	goto_if_eq VAR_DEX_UPGRADE_JOHTO_STARTER_STATE, 5, SandgemTown_PokemonResearchLab_EventScript_CanHaveAnyOneOfRarePokemon
	goto_if_eq VAR_DEX_UPGRADE_JOHTO_STARTER_STATE, 2, SandgemTown_PokemonResearchLab_EventScript_GrassyPatchWaiting
	goto_if_unset FLAG_HAS_MATCH_CALL, SandgemTown_PokemonResearchLab_EventScript_TryRatePokedexOrRegister
	goto_if_unset FLAG_ENABLE_PROF_BIRCH_MATCH_CALL, EventScript_RegisterProfBirch
	goto SandgemTown_PokemonResearchLab_EventScript_TryRatePokedexOrRegister
	end

SandgemTown_PokemonResearchLab_EventScript_CanHaveAnyOneOfRarePokemon::
	msgbox SandgemTown_PokemonResearchLab_Text_CanHaveAnyOneOfRarePokemon, MSGBOX_DEFAULT
	release
	end

SandgemTown_PokemonResearchLab_EventScript_GrassyPatchWaiting::
	msgbox SandgemTown_PokemonResearchLab_Text_GrassyPatchWaiting2, MSGBOX_DEFAULT
	release
	end

SandgemTown_PokemonResearchLab_EventScript_TryRatePokedexOrRegister::
	goto_if_unset FLAG_HIDE_SANDGEM_TOWN_RESEARCH_LAB_UNKNOWN_0x380, ProfBirch_EventScript_RatePokedexOrRegister
	goto_if_eq VAR_DEX_UPGRADE_JOHTO_STARTER_STATE, 3, ProfBirch_EventScript_RatePokedexOrRegister
	goto_if_ge VAR_DEX_UPGRADE_JOHTO_STARTER_STATE, 6, ProfBirch_EventScript_RatePokedexOrRegister
	goto SandgemTown_PokemonResearchLab_EventScript_PokemonAwait

EventScript_RegisterProfBirch::
	msgbox MatchCall_Text_BirchRegisterCall, MSGBOX_DEFAULT
	closemessage
	delay 30
	playfanfare MUS_REGISTER_MATCH_CALL
	msgbox MatchCall_Text_RegisteredBirch, MSGBOX_DEFAULT
	waitfanfare
	closemessage
	delay 30
	setflag FLAG_ENABLE_PROF_BIRCH_MATCH_CALL
	setvar VAR_REGISTER_BIRCH_STATE, 2
	release
	end

SandgemTown_PokemonResearchLab_EventScript_ReceivePokedex::
	playfanfare MUS_OBTAIN_ITEM
	message SandgemTown_PokemonResearchLab_Text_ReceivedPokedex
	waitfanfare
	setflag FLAG_SYS_POKEDEX_GET
	setflag FLAG_RECEIVED_POKEDEX_FROM_BIRCH
	setvar VAR_CABLE_CLUB_TUTORIAL_STATE, 1
	return

SandgemTown_PokemonResearchLab_EventScript_PokemonAwait::
	msgbox SandgemTown_PokemonResearchLab_Text_CountlessPokemonAwait, MSGBOX_DEFAULT
	release
	end

SandgemTown_PokemonResearchLab_EventScript_Machine::
	msgbox SandgemTown_PokemonResearchLab_Text_SeriousLookingMachine, MSGBOX_SIGN
	end

SandgemTown_PokemonResearchLab_EventScript_Rival::
	lock
	faceplayer
	goto_if_eq VAR_DEX_UPGRADE_JOHTO_STARTER_STATE, 5, SandgemTown_PokemonResearchLab_EventScript_RivalFuturePlans
	goto_if_ge VAR_DEX_UPGRADE_JOHTO_STARTER_STATE, 6, SandgemTown_PokemonResearchLab_EventScript_RivalHaveYouGoneToBattleFrontier
	@ The rival is only here after the National Dex upgrade
	goto SandgemTown_PokemonResearchLab_EventScript_RivalTakeBreakFromFieldwork

SandgemTown_PokemonResearchLab_EventScript_RivalFuturePlans::
	checkplayergender
	call_if_eq VAR_RESULT, MALE, SandgemTown_PokemonResearchLab_EventScript_MayWhatNextImStayingHere
	call_if_eq VAR_RESULT, FEMALE, SandgemTown_PokemonResearchLab_EventScript_BrendanPreferCollectingSlowly
	release
	end

SandgemTown_PokemonResearchLab_EventScript_MayWhatNextImStayingHere::
	msgbox SandgemTown_PokemonResearchLab_Text_MayWhatNextImStayingHere, MSGBOX_DEFAULT
	return

SandgemTown_PokemonResearchLab_EventScript_BrendanPreferCollectingSlowly:
	msgbox SandgemTown_PokemonResearchLab_Text_BrendanPreferCollectingSlowly, MSGBOX_DEFAULT
	return

SandgemTown_PokemonResearchLab_EventScript_RivalHaveYouGoneToBattleFrontier::
	checkplayergender
	call_if_eq VAR_RESULT, MALE, SandgemTown_PokemonResearchLab_EventScript_MayHaveYouGoneToBattleFrontier
	call_if_eq VAR_RESULT, FEMALE, SandgemTown_PokemonResearchLab_EventScript_BrendanHaveYouGoneToBattleFrontier
	release
	end

SandgemTown_PokemonResearchLab_EventScript_MayHaveYouGoneToBattleFrontier::
	msgbox SandgemTown_PokemonResearchLab_Text_MayHaveYouGoneToBattleFrontier, MSGBOX_DEFAULT
	return

SandgemTown_PokemonResearchLab_EventScript_BrendanHaveYouGoneToBattleFrontier::
	msgbox SandgemTown_PokemonResearchLab_Text_BrendanHaveYouGoneToBattleFrontier, MSGBOX_DEFAULT
	return

SandgemTown_PokemonResearchLab_EventScript_RivalTakeBreakFromFieldwork::
	checkplayergender
	call_if_eq VAR_RESULT, MALE, SandgemTown_PokemonResearchLab_EventScript_MayTakeBreakFromFieldwork
	call_if_eq VAR_RESULT, FEMALE, SandgemTown_PokemonResearchLab_EventScript_BrendanTakeBreakFromFieldwork
	release
	end

SandgemTown_PokemonResearchLab_EventScript_MayTakeBreakFromFieldwork::
	msgbox SandgemTown_PokemonResearchLab_Text_MayTakeBreakFromFieldwork, MSGBOX_DEFAULT
	return

SandgemTown_PokemonResearchLab_EventScript_BrendanTakeBreakFromFieldwork::
	msgbox SandgemTown_PokemonResearchLab_Text_BrendanTakeBreakFromFieldwork, MSGBOX_DEFAULT
	return

SandgemTown_PokemonResearchLab_EventScript_PC::
	msgbox SandgemTown_PokemonResearchLab_Text_PCUsedForResearch, MSGBOX_SIGN
	end

SandgemTown_PokemonResearchLab_EventScript_Bookshelf::
	msgbox SandgemTown_PokemonResearchLab_Text_CrammedWithBooksOnPokemon, MSGBOX_SIGN
	end

SandgemTown_PokemonResearchLab_EventScript_Book::
	msgbox SandgemTown_PokemonResearchLab_Text_BookTooHardToRead, MSGBOX_SIGN
	end

SandgemTown_PokemonResearchLab_EventScript_ScottAboardSSTidalCall::
	lockall
	pokenavcall SandgemTown_PokemonResearchLab_Text_ScottAboardSSTidalCall
	waitmessage
	clearflag FLAG_SCOTT_CALL_BATTLE_FRONTIER
	releaseall
	end

SandgemTown_PokemonResearchLab_Text_ReceivedPokedex:
	.string "{PLAYER} received the POKéDEX!$"

SandgemTown_PokemonResearchLab_Text_CountlessPokemonAwait:
	.string "PROF. BIRCH: Countless POKéMON\n"
	.string "await you!\p"
	.string "Argh, I'm getting the itch to get out\n"
	.string "and do fieldwork again!$"

SandgemTown_PokemonResearchLab_Text_SeriousLookingMachine:
	.string "It's a serious-looking machine.\n"
	.string "The PROF must use this for research.$"

SandgemTown_PokemonResearchLab_Text_PCUsedForResearch:
	.string "It's a PC used for research.\n"
	.string "Better not mess around with it.$"

SandgemTown_PokemonResearchLab_Text_CrammedWithBooksOnPokemon:
	.string "It's crammed with books on POKéMON.$"

SandgemTown_PokemonResearchLab_Text_BookTooHardToRead:
	.string "It's a book that's too hard to read.$"

SandgemTown_PokemonResearchLab_Text_ProfStudiesHabitats:
	.string "The PROF is studying the habitats\n"
	.string "and distribution of POKéMON.\p"
	.string "He's always saying there's no\n"
	.string "substitute for fieldwork!$"

SandgemTown_PokemonResearchLab_Text_OtherRegionsUpgradeToNational:
	.string "PROF. BIRCH: Now…\p"
	.string "{PLAYER}{KUN} and {RIVAL}, I've had the two\n"
	.string "of you help me study POKéMON.\p"
	.string "Thanks to your help, new facts\n"
	.string "are coming to light.\p"
	.string "It appears that in the HOENN region,\n"
	.string "there are also POKéMON from other\l"
	.string "regions.\p"
	.string "It goes to show how rich and varied\n"
	.string "the natural environments of HOENN\l"
	.string "happen to be.\p"
	.string "That's why I think it's necessary for\n"
	.string "me to upgrade your POKéDEX to\l"
	.string "the NATIONAL Mode.\p"
	.string "Here, let me see your POKéDEX units.$"

SandgemTown_PokemonResearchLab_Text_MayUpgradeSoCool:
	.string "MAY: Eheheh!\p"
	.string "It's so cool that even my POKéDEX\n"
	.string "is getting updated!\p"
	.string "It's because you went out and caught\n"
	.string "so many POKéMON, {PLAYER}{KUN}!$"

SandgemTown_PokemonResearchLab_Text_BrendanYouCanThankMe:
	.string "BRENDAN: I went out all over HOENN\n"
	.string "and checked out POKéMON.\p"
	.string "You can thank me for getting\n"
	.string "the NATIONAL Mode POKéDEX.\p"
	.string "Yep, you're lucky, {PLAYER}!$"

SandgemTown_PokemonResearchLab_Text_OkayAllDone:
	.string "PROF. BIRCH: Okay, all done!$"

SandgemTown_PokemonResearchLab_Text_PokedexUpgradedToNational:
	.string "{PLAYER}'s POKéDEX was upgraded\n"
	.string "to the NATIONAL Mode!$"

SandgemTown_PokemonResearchLab_Text_GrassyPatchWaiting2:
	.string "PROF. BIRCH: But listen.\n"
	.string "You've become the CHAMPION,\l"
	.string "but your POKéMON journey isn't over.\p"
	.string "There is no end to the road\n"
	.string "that is POKéMON.\p"
	.string "Somewhere, there is a grassy patch\n"
	.string "that's waiting for you!$"

SandgemTown_PokemonResearchLab_Text_MayTakeBreakFromFieldwork:
	.string "MAY: I think I'll take a short break\n"
	.string "from fieldwork.\p"
	.string "I think I'll help the PROF here for\n"
	.string "a while.$"

SandgemTown_PokemonResearchLab_Text_BrendanTakeBreakFromFieldwork:
	.string "BRENDAN: For the time being,\n"
	.string "I'm taking a break from fieldwork.\p"
	.string "I'll be helping out the PROF here\n"
	.string "for a while.$"

SandgemTown_PokemonResearchLab_Text_CompletedDexChoosePokemon:
	.string "PROF. BIRCH: Oh, {PLAYER}{KUN}!\n"
	.string "Let's have a look at your POKéDEX.\p"
	.string "… … … … … …\n"
	.string "… … … … … …\p"
	.string "Yes, there's no doubt about it.\p"
	.string "You really have completed the HOENN\n"
	.string "region's POKéDEX.\p"
	.string "That's more than just impressive.\p"
	.string "I have a gift for you.\p"
	.string "Consider it my show of appreciation\n"
	.string "for the fantastic work you've done.\p"
	.string "My gift is a rare POKéMON only found\n"
	.string "in another region!\p"
	.string "You can have any one of these\n"
	.string "three POKéMON!$"

SandgemTown_PokemonResearchLab_Text_CanHaveAnyOneOfRarePokemon:
	.string "PROF. BIRCH: These are rare POKéMON\n"
	.string "only found in another region!\p"
	.string "You can have any one of these\n"
	.string "three POKéMON!$"

SandgemTown_PokemonResearchLab_Text_YoullTakeCyndaquil:
	.string "PROF. BIRCH: The FIRE POKéMON\n"
	.string "CYNDAQUIL caught your eye!\p"
	.string "You're as sharp as ever!\p"
	.string "So the CYNDAQUIL is your choice?$"

SandgemTown_PokemonResearchLab_Text_YoullTakeTotodile:
	.string "PROF. BIRCH: The WATER POKéMON\n"
	.string "TOTODILE is your choice!\p"
	.string "You know how to pick a good one.\p"
	.string "So, you'll take the TOTODILE?$"

SandgemTown_PokemonResearchLab_Text_YoullTakeChikorita:
	.string "PROF. BIRCH: The GRASS POKéMON\n"
	.string "CHIKORITA is your choice!\p"
	.string "You sure know what you're doing.\p"
	.string "So, you'll take the CHIKORITA?$"

SandgemTown_PokemonResearchLab_Text_TakeYourTimeAllInvaluable:
	.string "PROF. BIRCH: Take your time before\n"
	.string "you decide.\p"
	.string "They're all invaluable POKéMON.$"

@ Unused
SandgemTown_PokemonResearchLab_Text_ReceivedJohtoStarter:
	.string "{PLAYER} received the {STR_VAR_1}\n"
	.string "from PROF. BIRCH!$"

@ Unused
SandgemTown_PokemonResearchLab_Text_GrassyPatchWaiting:
	.string "PROF. BIRCH: Listen, {PLAYER}{KUN}.\n"
	.string "You've completed the HOENN POKéDEX,\l"
	.string "but your POKéMON journey isn't over.\p"
	.string "There is no end to the road\n"
	.string "that is POKéMON.\p"
	.string "Somewhere, there is a grassy patch\n"
	.string "that's waiting for you!$"

SandgemTown_PokemonResearchLab_Text_BetterLeaveOthersAlone:
	.string "You received the promised POKéMON.\n"
	.string "Better leave the others alone.$"

@ Unused
SandgemTown_PokemonResearchLab_Text_MayWhatNextImStayingHere:
	.string "MAY: {PLAYER}{KUN}, after this…\n"
	.string "What are you going to do?\p"
	.string "Are you going to keep battling\n"
	.string "and sharpening your skills?\p"
	.string "Or are you going to try filling\n"
	.string "the NATIONAL POKéDEX?\p"
	.string "I'm staying here to help the PROF.$"

SandgemTown_PokemonResearchLab_Text_BrendanPreferCollectingSlowly:
	.string "BRENDAN: Rather than collecting\n"
	.string "POKéMON, I prefer slowly and \l"
	.string "steadily raising the one I chose.$"

SandgemTown_PokemonResearchLab_Text_MayHaveYouGoneToBattleFrontier:
	.string "MAY: Oh, hi, {PLAYER}{KUN}!\n"
	.string "Have you gone to that place,\l"
	.string "the BATTLE FRONTIER?$"

SandgemTown_PokemonResearchLab_Text_BrendanHaveYouGoneToBattleFrontier:
	.string "BRENDAN: Hey, {PLAYER}!\n"
	.string "Have you gone out to that place,\l"
	.string "the BATTLE FRONTIER?$"

SandgemTown_PokemonResearchLab_Text_ScottAboardSSTidalCall:
	.string "… … … … … …\n"
	.string "… … … … … Beep!\p"
	.string "SCOTT: Hi, hi, {PLAYER}!\n"
	.string "It's me, SCOTT.\p"
	.string "I'm aboard the S.S. TIDAL now.\n"
	.string "It feels great to be on the sea!\p"
	.string "By the way…\p"
	.string "There's this place that I'd like to\n"
	.string "invite you to for a visit.\p"
	.string "If you're interested, board a ferry\n"
	.string "at either SLATEPORT or LILYCOVE.\p"
	.string "I'll fill you in on the details when\n"
	.string "we meet. I'll be waiting!\p"
	.string "… … … … … …\n"
	.string "… … … … … Click!$"

SandgemTown_PokemonResearchLab_Text_AssistantBroughtPlayer:
	.string "PROFESSOR!\n"
	.string "I brought {PLAYER}!$"

SandgemTown_PokemonResearchLab_Text_MonHasGrown:
	.string "PROF. BIRCH: Ah, {PLAYER}!\n"
	.string "Thanks for coming all this way.\p"
	.string "Let me take a look…\p"
	.string "Well, well! Your {STR_VAR_1} looks like\n"
	.string "it's grown since the lake!\p"
	.string "You two already make a fine team.\p"
	.string "Would you like to give your\n"
	.string "{STR_VAR_1} a nickname?$"

SandgemTown_PokemonResearchLab_Text_TakeThisPokedex:
	.string "PROF. BIRCH: Now, I'd like you to\n"
	.string "have this.\p"
	.string "It's a POKéDEX. It records data on\n"
	.string "every POKéMON you meet.$"

SandgemTown_PokemonResearchLab_Text_GoHomeToMother:
	.string "PROF. BIRCH: Before you continue on\n"
	.string "your journey, go home and talk to\l"
	.string "your mother.\p"
	.string "I'm sure she'll want to hear all\n"
	.string "about it!$"

SandgemTown_PokemonResearchLab_Text_AssistantPokedexFills:
	.string "Your POKéDEX fills in every time you\n"
	.string "meet a new POKéMON.\p"
	.string "Good luck, {PLAYER}!$"
