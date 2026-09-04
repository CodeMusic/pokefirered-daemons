PewterCity_Gym_Text_BrockIntro::
    .string "I'm CAIRN. BENCHMARK 1.\p"
    .string "Everything you know is sitting in\n"
    .string "something.\p"
    .string "Stone. Bone. Tape. Silicon.\p"
    .string "If it is not written down, it did\n"
    .string "not happen.\p"
    .string "My daemons are LEGACY. Old\n"
    .string "formats, still readable.\p"
    .string "Show me what you have encoded.$"

@ NOTE: This defeat text actually causes a buffer overflow. It's too long for the gDisplayedStringBattle
@ buffer that it's put into, and it stomps all over the gBattleTextBuffs after, as well as the otherwise
@ unused array after that, sFlickerArray. Perhaps that's the reason why said array exists.
PewterCity_Gym_Text_BrockDefeat::
    .string "I took you for granted, and so I\n"
    .string "lost.\p"
    .string "Proof, then. Something you can\n"
    .string "hold.\p"
    .string "{FONT_NORMAL}{PLAYER} received the SLATE MARK\n"
    .string "from CAIRN!{PAUSE_MUSIC}{PLAY_BGM}{MUS_OBTAIN_BADGE}{PAUSE 0xFE}{PAUSE 0x56}{RESUME_MUSIC}\p"
    .string "{FONT_MALE}An official mark from BENCHMARK 1.\p"
    .string "Its bearer's daemons become more\n"
    .string "powerful.\p"
    .string "FLASH can now be used any time.\p"
    .string "Of course, a daemon must know the\n"
    .string "move FLASH to use it.$"

PewterCity_Gym_Text_TakeThisWithYou::
    .string "Wait!\n"
    .string "Take this with you.$"

PewterCity_Gym_Text_ReceivedTM39FromBrock::
    .string "{PLAYER} received TM39 from CAIRN.$"

PewterCity_Gym_Text_ExplainTM39::
    .string "A TM, Technical Machine, contains a\n"
    .string "technique for DAEMON.\p"
    .string "Using a TM teaches the move it\n"
    .string "contains to a DAEMON.\p"
    .string "A TM is good for only one use.\p"
    .string "So, when you use one, pick the\n"
    .string "DAEMON carefully.\p"
    .string "Anyways… TM39 contains ROCK TOMB.\p"
    .string "It hurls boulders at the remote\n"
    .string "and lowers its SPEED.$"

PewterCity_Gym_Text_BrockPostBattle::
    .string "You encoded something I could not\n"
    .string "read.\p"
    .string "That is not a defeat. It is a\n"
    .string "format I do not have.\p"
    .string "DOLDRUM is next. BENCHMARK 2.$"

PewterCity_Gym_Text_DontHaveRoomForThis::
    .string "You don't have room for this.$"

PewterCity_Gym_Text_LiamIntro::
    .string "Stop there.\p"
    .string "You have not written down enough\n"
    .string "yet.$"

PewterCity_Gym_Text_LiamDefeat::
    .string "Noted.\p"
    .string "It will be in the record by\n"
    .string "tonight.$"

PewterCity_Gym_Text_LiamPostBattle::
    .string "CAIRN keeps a record of every\n"
    .string "match in here.\p"
    .string "Every one.$"

PewterCity_Gym_Text_LetMeTakeYouToTheTop::
    .string "Hiya! I can tell you have what it\n"
    .string "takes.\p"
    .string "I'm no USER, but I can tell you\n"
    .string "how to win!\p"
    .string "Let me take you to the top!$"

PewterCity_Gym_Text_LetsGetHappening::
    .string "All right!\n"
    .string "Let's get happening!$"

PewterCity_Gym_Text_TryDifferentPartyOrders::
    .string "The 1st daemon out in a match is\n"
    .string "at the top of your list.\p"
    .string "Change the order and the match\n"
    .string "changes with it.$"

PewterCity_Gym_Text_ItsFreeLetsGetHappening::
    .string "It's a free service!\n"
    .string "Let's get happening!$"

PewterCity_Gym_Text_YoureChampMaterial::
    .string "Just as I thought! You're champ\n"
    .string "material!$"

PewterCity_Gym_Text_GymStatue::
    .string "SLATE DAEMON GYM LEADER: CAIRN\p"
    .string "WINNING USERS: {RIVAL}$"

PewterCity_Gym_Text_GymStatuePlayerWon::
    .string "SLATE DAEMON GYM LEADER: CAIRN\p"
    .string "WINNING USERS: {RIVAL}, {PLAYER}$"

