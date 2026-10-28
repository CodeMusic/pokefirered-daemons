# The debug build

**pokefirered ships no debug build** -- unlike pokeemerald there is no menu to port -- so this one is ours.
`make firered_debug` and `make leafgreen_debug` set `DAEMONS_DEBUG=1`, which suffixes the build's name, so it is **a
separate ROM with its own save**, and a debug run never touches a real playthrough.

It began as scaffolding for the spike that moved DAEMONS to the GBA, and has grown into the field test's way of reaching
things without playing to them. It is for testing, not for judging how the game feels.

## The menu

**A new debug game starts as a normal one does**, so the opening can still be tested. **START -> DEBUG** opens the menu:

| | |
|---|---|
| **GAME EVENTS** | the kit a piece at a time, in story order, each row `+` held or `·` not, A giving or taking it. Its first row, EVERYTHING, is the old kit: six daemons picked for their abilities, one of each kind of item, all eight MARKS, 999999 |
| **HEAL**, **MART** | a full restore; money and any item, added or taken away |
| **JUMP** | places, without playing to them; **MORE** has a second page |
| **ENCOUNTER** | a daemon at a level, the season, the weather, and the date -- a holiday, or a month's 1st |
| **TIME** | AUTO, or force dawn, day, dusk or night -- the map relights at once |
| **SOUND** | any song or sound effect, the glitch, and the HARP's and FLUTE's scales |

GAME EVENTS also fakes what the companion writes into a save, to test the game's half without a companion: AWAY'S
SIGNAL steps a daemon away's SOUNDINGS through its four levels, and ITS DREAM fakes the next dream for ITS DREAMS. Its code is `src/daemons_debug_events.c`; the kit's pieces are in
`src/new_game.c`.

**Hold B** to walk through grass and past trainers.

## Field hotkeys

Held with **SELECT**:

| GBA | mGBA default keys | |
|---|---|---|
| **SELECT + A** | hold `Backspace`, press `X` | heal the party |
| **SELECT + B** | hold `Backspace`, press `Z` | restock items and money |
| **SELECT + UP** | hold `Backspace`, press `↑` | next song, and its number on screen |
| **SELECT + DOWN** | hold `Backspace`, press `↓` | put the map's own song back |

The message stays while SELECT is held and goes when it is released.

## mGBA's keys

**`L` is the keyboard `A`, which is not the GBA A button**:

| GBA | key | | GBA | key |
|---|---|---|---|---|
| A | `X` | | L | `A` |
| B | `Z` | | R | `S` |
| Start | `Enter` | | Select | `Backspace` |
| D-pad | arrow keys | | | |

L and R are the game's: the default BUTTON MODE is `LR`, with HELP moved into the START menu, so R says the last line
again on the map, R is PERSPECTIVE in battle, and L and R choose PLAY's octave.
