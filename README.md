# CONTEXT / CONTENT — the GBA build

A total conversion of **Pokémon FireRed and LeafGreen**, built on
[pret/pokefirered][pret].

The creatures are **daemons** — background processes, in the Unix sense, and the
older sense underneath it. The type chart is not a balance table; it is an
argument about how a mind resolves what it is given. Everything else follows
from that.

**This repository is the engine, and the game is here.** The design lives next
door in [**CodeMusic/DAEMONS**][daemons] — a design bible, a changelog, the
lineage of the ideas, and every tool that writes into this tree. How to play,
how to build and how a release is made are on the [**DAEMONS wiki**][wiki]. The
Game Boy build, [**CodeMusic/pokered-daemons**][gb], is where the vertical slice
was built; it is kept as a reference and is not updated further.

**No ROM is distributed.** Releases are BPS patches for a cartridge you own,
published on the [DAEMONS releases page][releases] (latest: **v11.304.6**,
2026-10-18).

## Why this fork exists

It began as a **spike** on 2026-09-02, to answer one question recorded in the
bible as §9.3:

> Are abilities, item descriptions and a real scripting language worth
> rebuilding 334 files for?

**The answer came back yes within a day, and the project pivoted here on
2026-09-03.**

Colour is not the argument. Three things are:

- **Abilities.** Gen 3 gives every species a passive. §2.5 had to invent a
  *move* — CONSENSUS — because the chart needed one to be legible. An ability is
  a second axis for exactly that work, and Gen 1 does not have one.
- **Item descriptions.** Gen 1 stores **none at all**. The boxes and the
  stones-that-are-inputs each got a name and silence.
- **A scripting language**, instead of assembly for every sign and guard.

Two things break, and they are recorded rather than waved away. **Invariant 5
stops being a constraint**: on a DMG the machine and the meaning agreed — the
player saw grey because that is what the hardware *was* — and on GBA greyscale
is a preference. And **the chart goes from 15 types to 17**, in a design whose
§2 calls the fifteen names a complete set.

**Neither engine is deleted.** The Game Boy build still compiles and still runs
the slice; it is a reference, not a casualty.

## It builds two editions

| Target | Output | Edition |
|---|---|---|
| `make firered` | `daemonsContent.gba` | **CONTENT** |
| `make leafgreen` | `daemonsContext.gba` | **CONTEXT** |
| `make firered_debug` | `daemonsContent_debug.gba` | CONTENT, the testing build |
| `make leafgreen_debug` | `daemonsContext_debug.gba` | CONTEXT, the testing build |

The ROMs are named for the editions, and the `.elf`, `.map`, `.sym` and mGBA's
`.sav` follow the same name, so every build keeps its own save.

The edition split survived the port unchanged, which was the first good sign:
`firered` / `leafgreen` maps onto the Game Boy build's `_RED` / `_BLUE` with no
design work at all. Both disassemblies ship the same game twice and differ by a
build flag. **The type chart is byte-identical in both** — it is the argument,
and an argument that changes by cartridge is not one.

From the DAEMONS repo, `./bindDaemons.sh` builds and launches either edition
(`context`, `--debug`); `--classic` reaches the Game Boy build instead.

**Check the exit code, never grep for `error:`.** agbcc prints its diagnostics
as `warning:` lines and then fails with a bare `Error 1`, so a filtered build
looks clean while the ROM silently keeps the previous binary:

```sh
make firered -j8 >/dev/null 2>&1; echo "exit=$?"
```

*`shasum -c firered.sha1` fails by design* — the ROM carries our content. It
proves the toolchain on a pristine `pret/pokefirered` checkout and is not a
regression check for this fork.

### Toolchain

**devkitARM is not required.** Homebrew's `arm-none-eabi-gcc` installs without
`sudo`, but it ships **no libc**, so the `MODERN=1` path dies on `string.h`.
`agbcc` brings its own headers and is the path that works here:

```sh
brew install arm-none-eabi-gcc libpng
git clone https://github.com/pret/agbcc /tmp/agbcc
cd /tmp/agbcc && ./build.sh && ./install.sh /path/to/pokefirered-daemons
```

`agbcc` installs **into this checkout**, so it does not survive a fresh clone.
`DAEMONS/setup.sh` clones this fork beside the design repo and rebuilds it for
you.

**Edit the source, never the artifact.** `json_data_rules.mk` generates several
headers (`items.h`, the region map's strings, the wild encounters …) and every
one is gitignored. A change written into a generated header exists only in one
working build; write the `.json`.

## What a player meets

Everything below is in both editions unless it says otherwise. Every new
in-game word is still a **DRAFT** until it is approved.

- **Every daemon is ours.** All 386 species are renamed, plus nine new ones in
  slots vanilla left unused; the INDEX's 395 entries are rewritten, and each
  edition says something different about every one of them.
- **Seventeen types**: the fifteen names of the chart, plus `HARDENED` and
  `OPAQUE` in the Steel and Dark slots. The matchup table's one change from
  vanilla is `CONTEXT ↔ LATENT`, mutual 2×. All 77 abilities are renamed.
- **The vocabulary of the world**: USERS, BINDING, the USERBOX up to the
  ROOTBOX, ROUTINES, BENCHMARKS and their MARKS, CHECKPOINTS, the POOL, DRIVERS
  in place of HMs, HALTED in place of fainted, RECOMPILE in place of evolution.
- **A clock that matters.** The cartridge's clock if it has one, play time if
  not: dawn, day, dusk and night tint the outdoor maps, the weekday is on the
  USER card, and the seasons repaint the world — CONTENT keeps a northern year
  and CONTEXT a southern one. On fifteen early roads, one grass slot belongs to
  the season and one to the hour.
- **Weather by season**: rain and storms in spring, storms in summer, fog and
  WIND in autumn, snow in winter, drawn from the date, so it costs no save
  space. Each weather lifts its own routines — SIGNAL in a storm, LATENT in
  fog, VECTOR in wind — tints the battle, and brings out daemons of its own;
  snow is hail. The chart itself never changes.
- **Holidays.** Three a month on the real calendar — one shared, one for each
  edition's hemisphere — 36 in all, under the world's own names. Each month has
  a colour and a note, on the day's banner and the USER card; a holiday brings
  its own light, sky and daemons, and a gift once a year. On two days a year
  the other edition's daemons are let in. THE YEAR keeps them in the NOTEBOOK.
- **PLAY**: an instrument in each edition — the **HARP** in CONTENT, the
  **FLUTE** in CONTEXT — played note by note on a STAFF, in any of twelve keys,
  each note in its own colour. Some things in the world listen. Seven songs are
  heard in the world and can be learned by playing them back, and each does
  something; the **SONGBOOK** keeps what you have played.
- **Music of its own**: four towns' themes, and each of the fourteen hidden
  groves its own loop.
- **Colour is friendship.** A daemon's colours fill as its friendship grows:
  nearly grey at first, the full ramp at the most. People stay in colour.
- **The GAME CORNER** plays as the companion's CASINO: reels of the daemons
  you have seen, and luck from the game's own day — a holiday, the sky, the
  hour, the season.
- **The NOTEBOOK**: the pages the player finds and writes, in sections, each
  saying where it was found — among them a DREAM LOG, opened with "I had a
  dream." at the dawn after a milestone.
- **The MARKS and what they pair with.** Each of the eight MARKS prints a
  virtue over its shadow; a MARK and what it pairs with, held together, each
  say how the other balances it, and when every pair is held a last note
  arrives.
- **CALLOW SCHOOL**: a TEXTBOOK, an EXAM that can be resumed, and a DIPLOMA.
- **NATIVE** on the INDEX's area page, in place of the size comparison: where
  a daemon belongs, and the hours, seasons and weathers it keeps.
- **SEND and AWAY**: a party daemon can be sent to the
  [companion][companion] — a phone or a board hosts it — and comes home with
  what it learned.

What is decided and **not built yet** is in the DAEMONS [TODO list][todo] —
among it routine animations that carry the game's own signature, its binary
and its notes, and the HARP and FLUTE taking the project's own voices, as the
companion already plays them.

## How much of it is ours

Measured against `upstream/master` (v11.302.3; music re-counted at v11.304.6):

| | |
|---|---|
| Species names | **all 386** renamed, plus 9 new in unused slots |
| INDEX | **395 entries** per edition, all rewritten, every one different between the two |
| Sprites | **1464 of 1469** vanilla sprites redrawn, plus about 100 new — daemons **coloured by type** |
| Moves | 335 of 354 renamed, plus 9 new, among them **CONSENSUS** |
| Abilities | all 77 renamed |
| Items | 216 renamed, **251 descriptions** rewritten, 7 new key items |
| Trainers | 68 of 107 classes renamed; 99 trainers renamed |
| Places | 57 region-map names; Kanto's towns are words that mean both a colour and a feeling |
| Maps | 329 of 425 maps changed, 23 new |
| Music | 18 new songs — four towns' and the fourteen groves' — and 12 vanilla slots rewritten, plus the HARP's and FLUTE's notes |

**Colour is by type**, which makes it information rather than decoration — a
daemon is coloured by what it *is*, so the palette is another way of reading the
chart.

**The tools that write into this tree live in `DAEMONS/tools/`**, not here:
the names, the INDEX, the sprites, the music, the maps and the dialogue all
come through one, and each takes `--write` and only reports without it.

## The debug build

**pokefirered ships no debug build** — unlike pokeemerald there is no menu to
port — so this one is ours. `DAEMONS_DEBUG=1` suffixes `BUILD_NAME`, so it is a
**separate ROM with its own save** and a debug run never touches a real
playthrough. It began as scaffolding for the spike and has grown into the field
test's way of reaching things without playing to them.

**A new debug game starts as a normal one does**, so the opening can still be
tested. **START → DEBUG** opens the menu:

| | |
|---|---|
| **GAME EVENTS** | the kit a piece at a time, in story order, each row `+` held or `·` not, A giving or taking it. Its first row, EVERYTHING, is the old kit: six daemons picked for their abilities, one of each kind of item, all eight MARKS, 999999 |
| **HEAL**, **MART** | a full restore; 999999 and any item, added or taken away |
| **JUMP** | places, without playing to them; **MORE** has a second page |
| **ENCOUNTER** | a daemon at a level, the season, the weather, and the date — a holiday, or a month's 1st |
| **TIME** | AUTO, or force dawn, day, dusk or night — the map relights at once |
| **SOUND** | any song or sound effect, the glitch, and the HARP's and FLUTE's scales |

**Hold B** to walk through grass and past trainers. And a few field hotkeys,
held with **SELECT**:

| GBA | mGBA default keys | |
|---|---|---|
| **SELECT + A** | hold `Backspace`, press `X` | heal the party |
| **SELECT + B** | hold `Backspace`, press `Z` | restock items and money |
| **SELECT + UP** | hold `Backspace`, press `↑` | next song, and its number on screen (SOUND reaches every song) |
| **SELECT + DOWN** | hold `Backspace`, press `↓` | put the map's own song back |

The message stays while SELECT is held and goes when it is released.

mGBA's stock keyboard map is worth writing down, because **`L` is the keyboard
`A`, which is not the GBA A button**:

| GBA | key | | GBA | key |
|---|---|---|---|---|
| A | `X` | | L | `A` |
| B | `Z` | | R | `S` |
| Start | `Enter` | | Select | `Backspace` |
| D-pad | arrow keys | | | |

L and R are the game's now: the default BUTTON MODE is `LR`, with HELP moved
into the START menu, so R says the last line again on the map, R is
PERSPECTIVE in battle, and L and R choose PLAY's octave.

## Branches

| Branch | What it is |
|---|---|
| `context-content` | the conversion — **this is the work**, and the default |
| `master` | tracks [pret/pokefirered][pret] `master`, untouched |
| `symbols` | tracks upstream `symbols`, untouched |

`origin` is this fork; `upstream` is pret. `git pull upstream master` brings in
their fixes, and the untouched tracking branches are what make that painless.

## Credit and licence

This is a fork of [pret/pokefirered][pret], and it inherits everything about
that project's posture: it is a disassembly, the work of many contributors over
many years, and none of the original game's content is ours. **Pokémon is
Nintendo / Creatures / Game Freak.** Nothing here is endorsed by or affiliated
with them.

The original writing, music, design and artwork of CONTEXT / CONTENT are by
[CodeMusic](https://github.com/CodeMusic). The upstream disassembly is credited
to pret and its contributors.

[pret]: https://github.com/pret/pokefirered
[daemons]: https://github.com/CodeMusic/DAEMONS
[wiki]: https://github.com/CodeMusic/DAEMONS/wiki
[releases]: https://github.com/CodeMusic/DAEMONS/releases
[todo]: https://github.com/CodeMusic/DAEMONS/blob/main/docs/TODO.md
[companion]: https://github.com/CodeMusic/daemons-companion
[gb]: https://github.com/CodeMusic/pokered-daemons
