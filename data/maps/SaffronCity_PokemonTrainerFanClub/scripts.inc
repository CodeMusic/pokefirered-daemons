@ T-218: THE READING ROOM, in BRAZEN (docs/school.md 8). What is left of a faculty in the bought city: an
@ attendant who asks for a DIPLOMA out of habit, Crystal's journals and the reviewer's papers on the shelves
@ -- the real reward, since they let the player read before the Owl's scene what the machine will argue then --
@ three lecturers, and a keeper who installs REVEAL (T-220). Was the TRAINER FAN CLUB, whose vanilla postgame
@ fan-counting went with it; the journal that unlocks HEARSAY's entries is kept. DRAFT WORDING throughout.

SaffronCity_PokemonTrainerFanClub_MapScripts::
	.byte 0

ReadingRoom_EventScript_Gate::
	lockall
	applymovement LOCALID_READING_ROOM_ATTENDANT, Common_Movement_FacePlayer
	waitmovement 0
	msgbox ReadingRoom_Text_MayISee
	goto_if_unset FLAG_GOT_DIPLOMA, ReadingRoom_EventScript_TurnedAway
	msgbox ReadingRoom_Text_GoIn
	setvar VAR_READING_ROOM, 1
	releaseall
	end

ReadingRoom_EventScript_TurnedAway::
	msgbox ReadingRoom_Text_NoDiploma
	closemessage
	applymovement OBJ_EVENT_ID_PLAYER, ReadingRoom_Movement_StepBack
	waitmovement 0
	releaseall
	end

ReadingRoom_Movement_StepBack:
	walk_down
	step_end

ReadingRoom_EventScript_Attendant::
	goto_if_eq VAR_READING_ROOM, 0, ReadingRoom_EventScript_AttendantAsk
	msgbox ReadingRoom_Text_Faculty, MSGBOX_NPC
	end

ReadingRoom_EventScript_AttendantAsk::
	msgbox ReadingRoom_Text_MayISee, MSGBOX_NPC
	end

@ ---- the lecturers: a lecture is open to anyone, so any daemon can learn these (party_menu.c)
ReadingRoom_EventScript_LectureAttention::
	lock
	faceplayer
	msgbox ReadingRoom_Text_LectureAttention, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, ReadingRoom_EventScript_LectureDeclined
	msgbox ReadingRoom_Text_WhichDaemon
	setvar VAR_0x8005, MOVETUTOR_LOCK_ON
	call EventScript_ChooseMoveTutorMon
	goto_if_eq VAR_RESULT, FALSE, ReadingRoom_EventScript_LectureDeclined
	msgbox ReadingRoom_Text_LectureAttentionDone
	release
	end

ReadingRoom_EventScript_LectureCategories::
	lock
	faceplayer
	msgbox ReadingRoom_Text_LectureCategories, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, ReadingRoom_EventScript_LectureDeclined
	msgbox ReadingRoom_Text_WhichDaemon
	setvar VAR_0x8005, MOVETUTOR_BLEND
	call EventScript_ChooseMoveTutorMon
	goto_if_eq VAR_RESULT, FALSE, ReadingRoom_EventScript_LectureDeclined
	msgbox ReadingRoom_Text_LectureCategoriesDone
	release
	end

ReadingRoom_EventScript_LectureError::
	lock
	faceplayer
	msgbox ReadingRoom_Text_LectureError, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, ReadingRoom_EventScript_LectureDeclined
	msgbox ReadingRoom_Text_WhichDaemon
	setvar VAR_0x8005, MOVETUTOR_REBUT
	call EventScript_ChooseMoveTutorMon
	goto_if_eq VAR_RESULT, FALSE, ReadingRoom_EventScript_LectureDeclined
	msgbox ReadingRoom_Text_LectureErrorDone
	release
	end

ReadingRoom_EventScript_LectureDeclined::
	msgbox ReadingRoom_Text_LectureAnyTime
	release
	end

@ ---- the keeper installs REVEAL into the TOOLKIT: a flag, not a disc (reveal.c says why)
ReadingRoom_EventScript_Keeper::
	lock
	faceplayer
	goto_if_set FLAG_GOT_REVEAL, ReadingRoom_EventScript_KeeperAfter
	msgbox ReadingRoom_Text_KeeperGive
	playfanfare MUS_OBTAIN_TMHM
	message ReadingRoom_Text_RevealInstalled
	waitmessage
	waitfanfare
	setflag FLAG_GOT_REVEAL
	msgbox ReadingRoom_Text_KeeperExplain
	release
	end

ReadingRoom_EventScript_KeeperAfter::
	msgbox ReadingRoom_Text_KeeperAfter
	release
	end

@ ---- the shelves: a short look here, the whole page in the NOTEBOOK
ReadingRoom_EventScript_ShelfFields::
	lockall
	msgbox ReadingRoom_Text_ShelfFields
	goto_if_set FLAG_NOTEBOOK_LOOSE_FIELDS, ReadingRoom_EventScript_ShelfDone
	setflag FLAG_NOTEBOOK_LOOSE_FIELDS
	call ReadingRoom_EventScript_Copied
	releaseall
	end

ReadingRoom_EventScript_ShelfUnsent::
	lockall
	msgbox ReadingRoom_Text_ShelfUnsent
	goto_if_set FLAG_NOTEBOOK_LOOSE_UNSENT, ReadingRoom_EventScript_ShelfDone
	setflag FLAG_NOTEBOOK_LOOSE_UNSENT
	call ReadingRoom_EventScript_Copied
	releaseall
	end

ReadingRoom_EventScript_ShelfObserver::
	lockall
	msgbox ReadingRoom_Text_ShelfObserver
	goto_if_set FLAG_NOTEBOOK_LAB_OBSERVER, ReadingRoom_EventScript_ShelfDone
	setflag FLAG_NOTEBOOK_LAB_OBSERVER
	call ReadingRoom_EventScript_Copied
	releaseall
	end

ReadingRoom_EventScript_ShelfBox::
	lockall
	msgbox ReadingRoom_Text_ShelfBox
	goto_if_set FLAG_NOTEBOOK_LAB_BOX, ReadingRoom_EventScript_ShelfDone
	setflag FLAG_NOTEBOOK_LAB_BOX
	call ReadingRoom_EventScript_Copied
	releaseall
	end

ReadingRoom_EventScript_ShelfPosition::
	lockall
	msgbox ReadingRoom_Text_ShelfPosition
	goto_if_set FLAG_NOTEBOOK_REVIEW_POSITION, ReadingRoom_EventScript_ShelfDone
	setflag FLAG_NOTEBOOK_REVIEW_POSITION
	call ReadingRoom_EventScript_Copied
	releaseall
	end

ReadingRoom_EventScript_ShelfDraft::
	lockall
	msgbox ReadingRoom_Text_ShelfDraft
	goto_if_set FLAG_NOTEBOOK_REVIEW_DRAFT, ReadingRoom_EventScript_ShelfDone
	setflag FLAG_NOTEBOOK_REVIEW_DRAFT
	call ReadingRoom_EventScript_Copied
	releaseall
	end

ReadingRoom_EventScript_ShelfDone::
	releaseall
	end

ReadingRoom_EventScript_Copied::
	playse SE_SELECT
	goto_if_unset FLAG_GOT_NOTEBOOK, ReadingRoom_EventScript_CopiedOut
	msgbox ReadingRoom_Text_IntoNotebook
	return

ReadingRoom_EventScript_CopiedOut::
	msgbox ReadingRoom_Text_CopiedOut
	return

ReadingRoom_EventScript_Catalogue::
	msgbox ReadingRoom_Text_Catalogue, MSGBOX_SIGN
	end
