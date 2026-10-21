@ T-18 P2 (docs/private/plan-follower-2026-10-20.md): A on THE FOLLOWER. It is a sprite, not an object event, so
@ field_control_avatar.c asks Follower_GetInteractionScript after the people and before the signs. Follower_Talk
@ (src/daemons_follower.c) turns it to face the player, lifts a bubble over it and leaves the words in gStringVar4:
@ the line for its friendship (the five readings of the ladder: observation, not sentiment), then -- when one holds --
@ one mood: something it noticed nearby (a hidden item), the place (THE NEXUS, HALFTONE), the sky, the watch, the
@ season; outdoors only for the last three. Two lines a mood, taken in turn. EVERY WORD BELOW IS DRAFT.
EventScript_Follower::
	lockall
	copyvar VAR_PREV_TEXT_COLOR, VAR_TEXT_COLOR
	textcolor NPC_TEXT_COLOR_MON
	special Follower_Talk
	msgbox gStringVar4
	copyvar VAR_TEXT_COLOR, VAR_PREV_TEXT_COLOR
	releaseall
	end

@ ---- DRAFT: by friendship, 0-49, 50-99, 100-149, 150-219, 220-255 (the plan's own five)
Follower_Text_Tier0::
	.string "{STR_VAR_1} is a step behind.\n"
	.string "It is watching the way out.$"

Follower_Text_Tier1::
	.string "{STR_VAR_1} keeps the\n"
	.string "distance you set. No closer.$"

Follower_Text_Tier2::
	.string "{STR_VAR_1} stays near\n"
	.string "you without being told.$"

Follower_Text_Tier3::
	.string "{STR_VAR_1} has stopped\n"
	.string "watching the door.$"

Follower_Text_Tier4::
	.string "{STR_VAR_1} would not leave\n"
	.string "if you opened the box.$"

@ ---- DRAFT: it noticed something nearby (a hidden item within two tiles)
Follower_Text_Notice::
	.string "{STR_VAR_1} keeps looking\n"
	.string "at the ground near here.$"

@ ---- DRAFT: the place
Follower_Text_Nexus0::
	.string "{STR_VAR_1} is looking at the paint.$"

Follower_Text_Nexus1::
	.string "{STR_VAR_1} stands very still,\n"
	.string "as if it might dry that way.$"

Follower_Text_Halftone0::
	.string "{STR_VAR_1} looks the same as it did.\n"
	.string "Everything else does not.$"

Follower_Text_Halftone1::
	.string "{STR_VAR_1} keeps checking\n"
	.string "that you are still you.$"

@ ---- DRAFT: the sky (outdoors)
Follower_Text_Rain0::
	.string "{STR_VAR_1} shakes the\n"
	.string "rain off its casing.$"

Follower_Text_Rain1::
	.string "{STR_VAR_1} is counting\n"
	.string "the drops that land on it.$"

Follower_Text_Storm0::
	.string "{STR_VAR_1} counts the gap\n"
	.string "between the light and the sound.$"

Follower_Text_Storm1::
	.string "{STR_VAR_1} walks a little\n"
	.string "closer while it thunders.$"

Follower_Text_Snow0::
	.string "{STR_VAR_1} leaves a second\n"
	.string "line of prints beside yours.$"

Follower_Text_Snow1::
	.string "{STR_VAR_1} watches the\n"
	.string "snow fill in its tracks.$"

Follower_Text_Fog0::
	.string "{STR_VAR_1} stays where\n"
	.string "it can still see you.$"

Follower_Text_Fog1::
	.string "{STR_VAR_1} is listening\n"
	.string "for what it cannot see.$"

Follower_Text_Wind0::
	.string "{STR_VAR_1} leans into\n"
	.string "the wind when you do.$"

Follower_Text_Wind1::
	.string "{STR_VAR_1} turns its back to\n"
	.string "the wind and waits for you.$"

@ ---- DRAFT: the watch (outdoors; the day has none)
Follower_Text_Night0::
	.string "{STR_VAR_1} is quieter after dark.$"

Follower_Text_Night1::
	.string "{STR_VAR_1} keeps closer\n"
	.string "once the light goes.$"

Follower_Text_Dusk0::
	.string "{STR_VAR_1} looks back\n"
	.string "the way you came.$"

Follower_Text_Dusk1::
	.string "{STR_VAR_1} watches\n"
	.string "its shadow get longer.$"

Follower_Text_Dawn0::
	.string "{STR_VAR_1} watches\n"
	.string "the light come up.$"

Follower_Text_Dawn1::
	.string "{STR_VAR_1} was awake\n"
	.string "before you were.$"

@ ---- DRAFT: the season (outdoors, by day, under a clear sky)
Follower_Text_Winter0::
	.string "{STR_VAR_1} walks exactly\n"
	.string "in your footsteps.$"

Follower_Text_Winter1::
	.string "{STR_VAR_1} stands close\n"
	.string "whenever you stop.$"

Follower_Text_Spring0::
	.string "{STR_VAR_1} stops at\n"
	.string "everything that is new.$"

Follower_Text_Spring1::
	.string "{STR_VAR_1} sniffs at\n"
	.string "the grass, then at you.$"

Follower_Text_Summer0::
	.string "{STR_VAR_1} keeps to your shadow.$"

Follower_Text_Summer1::
	.string "{STR_VAR_1} walks slower\n"
	.string "in the heat. So do you.$"

Follower_Text_Autumn0::
	.string "{STR_VAR_1} stops to\n"
	.string "turn over a leaf.$"

Follower_Text_Autumn1::
	.string "{STR_VAR_1} is listening to\n"
	.string "the leaves under its feet.$"
