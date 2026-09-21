const u8 gTeachyTvString_TeachBattle[] = _("The first exchange");
const u8 gTeachyTvString_StatusProblems[] = _("When a process stalls");
const u8 gTeachyTvString_TypeMatchups[] = _("Reading the chart");
const u8 gTeachyTvString_CatchPkmn[] = _("Binding a stranger");
const u8 gTeachyTvString_AboutTMs[] = _("What a TM costs");
const u8 gTeachyTvString_RegisterItem[] = _("One item, one button");
const u8 gTeachyTvString_Cancel[] = _("CANCEL");

// --------------------------------------------------------------------------
// STREAM is CRYSTAL CLEAR's lectures (T-100): the series that was announced
// twice and never held, recorded instead. Her register -- dry, exact, a little
// impatient -- in place of vanilla's host, who shouted HELLO at the camera.
// --------------------------------------------------------------------------
const u8 gTeachyTvText_PokedudeSaysHello[] = _(
        "This is a recording.\p"
        "CRYSTAL CLEAR, for the lecture\n"
        "series. Take notes if you like.\p");
const u8 gTeachyTvText_BattleScript1[] = _(
        "The first exchange.\p"
        "A DAEMON appears. It is not yours.\n"
        "Neither of you was asked.\p"
        "Each side has ROUTINES and HP,\n"
        "and each side takes turns.\p"
        "Watch who acts first, and why.");
const u8 gTeachyTvText_BattleScript2[] = _(
        "That is all a battle is.\p"
        "When HP reaches zero, a DAEMON\n"
        "stops. It is not gone.\p"
        "A CHECKPOINT will restart it.\p"
        "That is the lecture.");
const u8 gTeachyTvText_StatusScript1[] = _(
        "When a process stalls.\p"
        "Leaking. Throttling. Suspension.\n"
        "Overheating. There are others.\p"
        "Any one of them can make a strong\n"
        "DAEMON useless for a whole battle.\p"
        "Level does not protect it.\p"
        "Watch what I do about it.");
const u8 gTeachyTvText_StatusScript2[] = _(
        "Leaking and throttling do not end\n"
        "when the battle does.\p"
        "A leaking DAEMON loses HP while\n"
        "you walk.\p"
        "Treat it with an item, or take it\n"
        "to a CHECKPOINT. Do not wait.\p"
        "That is the lecture.");
const u8 gTeachyTvText_MatchupsScript1[] = _(
        "Reading the chart.\p"
        "Every DAEMON has a type. So does\n"
        "every ROUTINE.\p"
        "GROWTH, for example, and FLOW.\p"
        "The damage depends on both: the\n"
        "type of the ROUTINE, and the type\l"
        "of what it hits.\p"
        "The chart does not care how strong\n"
        "you are. It is a table.\p"
        "I will show you one entry of it.");
const u8 gTeachyTvText_MatchupsScript2[] = _(
        "Before you act, ask two things.\p"
        "What does my ROUTINE do to that\n"
        "type?\p"
        "And what does its ROUTINE do to\n"
        "mine?\p"
        "Most USERS ask the first. Ask the\n"
        "second.\p"
        "That is the lecture.");
const u8 gTeachyTvText_CatchingScript1[] = _(
        "Binding a stranger.\p"
        "You will see a DAEMON you want.\n"
        "Wanting it is not a method.\p"
        "Here is the method.");
const u8 gTeachyTvText_CatchingScript2[] = _(
        "If the BOX fails, it is not\n"
        "personal. Throw again.\p"
        "Then write down where you found it.\n"
        "Nobody else will.\p"
        "That is the lecture.");
const u8 gTeachyTvText_TMsScript1[] = _(
        "What a TM costs.\p"
        "A TM teaches a DAEMON a ROUTINE it\n"
        "would not reach on its own.\p"
        "It writes over one it already has.\n"
        "Choose which, carefully.\p"
        "Open the TM CASE, and read what you\n"
        "have. All of it.");
const u8 gPokedudeText_TMTypes[] = _(
        "CRYSTAL: TMs have types, as\n"
        "ROUTINES do.\p"
        "Teach one to a DAEMON whose type\n"
        "suits it.\p"
        "RIPPLE suits a FLOW-type DAEMON.\p"
        "MINIBATCH is a ROUTINE most\n"
        "GROWTH-type DAEMON can learn.\p"
        "One more thing.");
const u8 gPokedudeText_ReadTMDescription[] = _(
        "Do not stop at the type. Read the\n"
        "description.\p"
        "It says what kind of DAEMON could\n"
        "use the ROUTINE at all.\p"
        "SYLLOGISM, for example, is not a\n"
        "thing a fish learns.\p"
        "Try it on a DAEMON that argues.");
const u8 gTeachyTvText_TMsScript2[] = _(
        "I have talked for longer than I\n"
        "meant to.\p"
        "That is the lecture.");
const u8 gTeachyTvText_RegisterScript1[] = _(
        "One item, one button.\p"
        "Some KEY ITEMS you will use often,\n"
        "and opening the POOL is slow.\p"
        "Suppose I have a STREAM in my POOL.\p"
        "I will register it.");
const u8 gTeachyTvText_RegisterScript2[] = _(
        "Now it is registered.\p"
        "Once an item in the KEY ITEMS\n"
        "POCKET is registered, SELECT\l"
        "uses it, from anywhere.\p"
        "One button, and there I am again.\p"
        "I did not expect to be something\n"
        "people carry about.\p"
        "That is the lecture.");

// --------------------------------------------------------------------------
// Ours. Talk-only shows: two strings and a table row, no battle and no bag.
// The eight MARK shows are gated on the mark, so the list grows as the player
// is certified -- and each teaches its MARK's concept as a thing to DO in a
// battle, never as a thing to know. Craft rule 1 holds on television too.
// --------------------------------------------------------------------------

const u8 gTeachyTvString_Hosting[] = _("Still running elsewhere");
const u8 gTeachyTvString_Mark1[] = _("The SLATE MARK");
const u8 gTeachyTvString_Mark2[] = _("The SLOPE MARK");
const u8 gTeachyTvString_Mark3[] = _("The SENSE MARK");
const u8 gTeachyTvString_Mark4[] = _("The FIT MARK");
const u8 gTeachyTvString_Mark5[] = _("The SKEW MARK");
const u8 gTeachyTvString_Mark6[] = _("The FRAME MARK");
const u8 gTeachyTvString_Mark7[] = _("The HEAT MARK");
const u8 gTeachyTvString_Mark8[] = _("The TRUE MARK");

const u8 gTeachyTvText_TutorSaysHelloNewShow[] = _(
        "This is a recording.\p"
        "CRYSTAL CLEAR. A new lecture has\n"
        "been added to the list.\p");

const u8 gTeachyTvText_HostingScript1[] = _(
        "Where a DAEMON goes when you are\n"
        "not looking.\p"
        "You bind a DAEMON with a BOX.\p"
        "A BOX is not a pocket. It is a\n"
        "small machine, and it HOSTS.\p"
        "Six travel with you. The rest are\n"
        "hosted, and hosting is not storage.\p"
        "A hosted DAEMON is running. It is\n"
        "just not running here.$");

const u8 gTeachyTvText_HostingScript2[] = _(
        "So: you LATCH with a BOX, and\n"
        "the DAEMON is HOSTED on it.\p"
        "You can reach anything hosted\n"
        "from any CHECKPOINT.\p"
        "Bind more than you need.\p"
        "A party of one is one type, and\n"
        "the chart does not care how you\l"
        "feel.\p"
        "That is the lecture.$");

const u8 gTeachyTvText_Mark1Script1[] = _(
        "The SLATE MARK.\p"
        "CAIRN cuts his records into stone.\n"
        "Do you know why stone?\p"
        "A record has to hold still while\n"
        "people argue about it.\p"
        "Look at one of your DAEMONS.\p"
        "What you see is a picture. What is\n"
        "kept is a row of numbers.$");

const u8 gTeachyTvText_Mark1Script2[] = _(
        "Open the summary. As far as the\n"
        "record goes, that page is all\l"
        "there is.\p"
        "Two that look alike are not alike.\n"
        "The page is where you find out.\p"
        "When a DAEMON surprises you, read\n"
        "the page again.\p"
        "It was written down the whole time.\p"
        "That is the lecture.$");

const u8 gTeachyTvText_Mark2Script1[] = _(
        "The SLOPE MARK.\p"
        "Nothing improves all at once.\p"
        "It improves by the smallest step\n"
        "that goes downhill, again and\l"
        "again.$");

const u8 gTeachyTvText_Mark2Script2[] = _(
        "So do not save a DAEMON for the big\n"
        "fight. There is no big fight.\p"
        "There are two hundred small ones,\n"
        "and each one is a step.\p"
        "A DAEMON that battles often needs\n"
        "no shortcut.\p"
        "That is the lecture.$");

const u8 gTeachyTvText_Mark3Script1[] = _(
        "The SENSE MARK. A question.\p"
        "What is on the battle screen right\n"
        "now?\p"
        "Not what happened. What is SHOWN.\p"
        "Two HP bars. Two names. A tile if\n"
        "something is wrong. And whose turn\l"
        "was faster.$");

const u8 gTeachyTvText_Mark3Script2[] = _(
        "You cannot act on a thing you did\n"
        "not notice.\p"
        "Most losses are not bad choices.\n"
        "They are unread screens.\p"
        "Before you press anything, name\n"
        "three things you can see.\p"
        "That is the lecture.$");

const u8 gTeachyTvText_Mark4Script1[] = _(
        "The FIT MARK.\p"
        "Here is a mistake I made, and made\n"
        "with some pride.\p"
        "I trained one DAEMON against one\n"
        "kind of opponent for a week.\p"
        "It beat that opponent every time.\p"
        "It then lost to a child on a route\n"
        "I had never walked.$");

const u8 gTeachyTvText_Mark4Script2[] = _(
        "A DAEMON shaped against one thing\n"
        "fits that thing exactly.\p"
        "Exactly is not the same as well.\p"
        "Battle where you do not usually go.\p"
        "ROUTINES that answer four kinds of\n"
        "trouble beat ones that answer one\l"
        "kind perfectly.\p"
        "That is the lecture.$");

const u8 gTeachyTvText_Mark5Script1[] = _(
        "The SKEW MARK. This one is\n"
        "uncomfortable.\p"
        "Every DAEMON you outscore leaves\n"
        "a little of itself in yours.\p"
        "You will not see it on any screen.\n"
        "It is there anyway.$");

const u8 gTeachyTvText_Mark5Script2[] = _(
        "So what you battle decides what\n"
        "your DAEMON becomes.\p"
        "Feed it one diet and it grows one\n"
        "way, whether or not you chose it.\p"
        "If a DAEMON turns out wrong, look\n"
        "at what you have been feeding it.\p"
        "Nobody tampered with it. You kept\n"
        "going to the same place.\p"
        "That is the lecture.$");

const u8 gTeachyTvText_Mark6Script1[] = _(
        "The FRAME MARK.\p"
        "A battle has more in it than the\n"
        "screen can hold.\p"
        "So the game picks. FIGHT shows you\n"
        "four ROUTINES. POOL shows you items.\p"
        "Whichever menu you opened decided\n"
        "what you were allowed to think of.$");

const u8 gTeachyTvText_Mark6Script2[] = _(
        "A menu is not the situation. It is\n"
        "a frame around part of it.\p"
        "DETACH is on that same screen, and\n"
        "most USERS never look at it.\p"
        "When you are stuck, back out one\n"
        "menu.\p"
        "The answer is often in the part\n"
        "that got cropped off.\p"
        "That is the lecture.$");

const u8 gTeachyTvText_Mark7Script1[] = _(
        "The HEAT MARK.\p"
        "The same DAEMON, the same ROUTINE,\n"
        "the same target.\p"
        "Does it do the same thing twice?\p"
        "It does not. It rolls.\p"
        "Damage wobbles. Accuracy misses.\n"
        "Sometimes it lands critical.$");

const u8 gTeachyTvText_Mark7Script2[] = _(
        "That wobble is not the game being\n"
        "unfair to you.\p"
        "A plan that only works on the best\n"
        "roll is not a plan.\p"
        "Leave room. Win on the ordinary\n"
        "roll, not the lucky one.\p"
        "That is the lecture.$");

const u8 gTeachyTvText_Mark8Script1[] = _(
        "The TRUE MARK. The last one, and I\n"
        "will be brief.\p"
        "A DAEMON that does what you said\n"
        "is not a DAEMON that did what you\l"
        "meant.\p"
        "You picked the ROUTINE. It aimed at\n"
        "the target you gave it.$");

const u8 gTeachyTvText_Mark8Script2[] = _(
        "A THRASHING DAEMON still obeys. It\n"
        "obeys in the wrong direction.\p"
        "A traded DAEMON obeys the MARKS you\n"
        "hold, not the words you use.\p"
        "When it goes wrong, ask what you\n"
        "actually asked for.\p"
        "That is the whole of it.\p"
        "That was the last lecture. There\n"
        "were to be more.$");
