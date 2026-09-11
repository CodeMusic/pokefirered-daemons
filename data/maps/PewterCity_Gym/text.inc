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
    .string "VERBOSE can now be used any time.\p"
    .string "Of course, a daemon must know the\n"
    .string "ROUTINE VERBOSE to use it.$"

PewterCity_Gym_Text_TakeThisWithYou::
    .string "Wait!\n"
    .string "Take this with you.$"

PewterCity_Gym_Text_ReceivedTM39FromBrock::
    .string "{PLAYER} received TM39 from CAIRN.$"

PewterCity_Gym_Text_ExplainTM39::
    .string "A TM holds one technique.\p"
    .string "Using it writes the ROUTINE into a\n"
    .string "daemon. Once.\p"
    .string "The TM does not survive that, so\n"
    .string "choose carefully.\p"
    .string "TM39 is WEDGE. It drops weight on\n"
    .string "the target and slows it.$"

PewterCity_Gym_Text_BrockPostBattle::
    .string "You encoded something I could not\n"
    .string "read.\p"
    .string "That is not a defeat. It is a\n"
    .string "format I do not have.\p"
    .string "DOLDRUM is next. BENCHMARK 2.$"

PewterCity_Gym_Text_DontHaveRoomForThis::
    .string "You have no room for this.$"

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
    .string "You look like you might get\n"
    .string "through.\p"
    .string "I do not battle. I watch.\p"
    .string "I can tell you how it goes.$"

PewterCity_Gym_Text_LetsGetHappening::
    .string "Right.\n"
    .string "Come on.$"

PewterCity_Gym_Text_TryDifferentPartyOrders::
    .string "The first daemon out is the one at\n"
    .string "the top of your list.\p"
    .string "Change the order and the match\n"
    .string "changes with it.$"

PewterCity_Gym_Text_ItsFreeLetsGetHappening::
    .string "It costs nothing.\n"
    .string "Come on.$"

PewterCity_Gym_Text_YoureChampMaterial::
    .string "I said so.\p"
    .string "I am usually right about this.$"

PewterCity_Gym_Text_GymStatue::
    .string "SLATE DAEMON GYM LEADER: CAIRN\p"
    .string "WINNING USERS: {RIVAL}$"

PewterCity_Gym_Text_GymStatuePlayerWon::
    .string "SLATE DAEMON GYM LEADER: CAIRN\p"
    .string "WINNING USERS: {RIVAL}, {PLAYER}$"

