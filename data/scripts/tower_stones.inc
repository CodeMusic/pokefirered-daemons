@  T-249. What a HALFTONE Tower stone says. field_control_avatar.c knows a stone by its metatile and puts which of
@  the six it is in VAR_0x8004, fixed by where it stands. Each line reads as an epitaph to one person in HALFTONE
@  and as a deletion record to the next, and never says which. DRAFT wording, all six.
EventScript_TowerStone::
	lockall
	switch VAR_0x8004
	case 0, EventScript_TowerStone0
	case 1, EventScript_TowerStone1
	case 2, EventScript_TowerStone2
	case 3, EventScript_TowerStone3
	case 4, EventScript_TowerStone4
	msgbox Text_TowerStone5
	releaseall
	end

EventScript_TowerStone0::
	msgbox Text_TowerStone0
	releaseall
	end

EventScript_TowerStone1::
	msgbox Text_TowerStone1
	releaseall
	end

EventScript_TowerStone2::
	msgbox Text_TowerStone2
	releaseall
	end

EventScript_TowerStone3::
	msgbox Text_TowerStone3
	releaseall
	end

EventScript_TowerStone4::
	msgbox Text_TowerStone4
	releaseall
	end

Text_TowerStone0::
	.string "Here was a record. It is kept here\n"
	.string "that it was.$"

Text_TowerStone1::
	.string "DELETED. Every copy has been told.$"

Text_TowerStone2::
	.string "The name is gone. The space it took\n"
	.string "is still kept.$"

Text_TowerStone3::
	.string "Not here. Kept so that nothing puts\n"
	.string "it back by mistake.$"

Text_TowerStone4::
	.string "Removed at the owner's request. The\n"
	.string "removal is permanent. So is this.$"

Text_TowerStone5::
	.string "It was here. Now only that is.$"
