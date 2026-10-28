# Inside the tree

This is pret's disassembly with DAEMONS written into it. Most of the change -- names, the INDEX, the sprites, the music,
the maps, the dialogue -- arrived through tools, and those tools live in the
[DAEMONS repository](https://github.com/CodeMusic/DAEMONS/tree/main/tools), not here. Each takes `--write` and only
reports without it. The rest is DAEMONS' own C, below.

## DAEMONS' own source

Each file opens with a comment naming the ticket it answers (`T-` numbers, in DAEMONS'
[TODO](https://github.com/CodeMusic/DAEMONS/blob/main/docs/TODO.md)) and the rules it keeps.

| file | what it does |
|---|---|
| `src/daemons_time.c` | dawn, day, dusk and night, from the cartridge's clock or play time |
| `src/daemons_rtc.c` | reading a real cartridge clock |
| `src/daemons_weather.c` | weather by season, drawn from the date so it costs no save space |
| `src/daemons_holidays.c` | the real calendar's holidays, and a colour and a note for each month |
| `src/play.c` | PLAY: the HARP and the FLUTE, the SONGBOOK, the songs the world listens for |
| `src/notebook.c` | the NOTEBOOK and its sections -- the DREAM LOG, ITS DREAMS, THE LOG |
| `src/daemons_follower.c` | the daemon that walks behind you; its pictures are `graphics/pokemon/<x>/follower.png`, listed in `src/data/follower_gfx.h` |
| `src/daemons_feelings.c` | GLAD, MAD, SAD and CALM, read from play and kept in the daemon's own record |
| `src/daemons_three.c` | THE SUBTLE LAYER's three values: battle-only, derived each battle, never saved; every number in `include/daemons_three.h` |
| `src/daemons_reads.c` | a read on arriving somewhere, and the log that keeps whether it landed |
| `src/daemons_signature.c` | every routine's signature in battle: glyphs of bits and a three-note figure |
| `src/daemons_casino.c` | the GAME CORNER's reels, played as the companion's CASINO |
| `src/daemons_away.c` | a daemon AWAY on the companion, drawn washed out wherever the game draws it |
| `src/daemons_carried.c` | what the companion leaves for the game: a daemon evolved while carried, finished on load |
| `src/daemons_first_seen.c` | where the INDEX says each daemon was first met |
| `src/daemons_debug_events.c` | the debug build's GAME EVENTS ([The debug build](The-debug-build)) |

## What the companion writes

[daemons-companion](https://github.com/CodeMusic/daemons-companion) reads and writes a DAEMONS save without any of this
code. The layout it relies on -- the party's flags, the vars it writes at SYNC (SOUNDINGS, ITS DREAMS), what a carried
daemon brings home -- is exported from the DAEMONS repository by `tools/companion_export.py`, so the two never guess at
each other.

## The tools that write here

From the DAEMONS repository's root, each with `--write`:

| tool | writes |
|---|---|
| `port_names.py`, `port_index.py`, `port_item_text.py`, `port_vocab.py`, `port_dialogue.py` | the names, the INDEX, the item descriptions, the vocabulary, the dialogue |
| `gbasprite.py` | the daemons' sprites, coloured by type -- and, after every icon, `gbafollower.py`'s follower pictures |
| `port_music.py` | DAEMONS' tracks |
| `gbaregionmap.py`, `gbawater.py`, `gbalandmarks.py` | the region map, the water, the landmarks |

`tools/check_generators.py` there runs every tool in report mode and fails if one would change anything, so a
generated file never silently drifts from the tool that wrote it.

## How much of it is ours

The [README](https://github.com/CodeMusic/pokefirered-daemons#how-much-of-it-is-ours) keeps the count: species, the
INDEX, sprites, routines, abilities, items, trainers, places, maps and music, against pret's `master`.
