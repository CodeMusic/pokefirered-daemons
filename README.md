# CONTEXT / CONTENT — the GBA build

A total conversion of **Pokémon FireRed and LeafGreen**, built on
[pret/pokefirered][pret].

The creatures are **daemons** — background processes, in the Unix sense, and the
older sense underneath it. The type chart is not a balance table; it is an
argument about how a mind resolves what it is given. Everything else follows
from that.

**This repository is an engine, and it is not yet the engine.** The design lives
next door in [**CodeMusic/DAEMONS**][daemons] — a design bible, a changelog, the
lineage of the ideas. The Game Boy build is [**CodeMusic/pokered-daemons**][gb],
and that is where the vertical slice actually is.

## Why this fork exists

It is a **spike, not a decision.** It was forked on 2026-09-02 to answer one
question, recorded in the bible as §9.3:

> Are abilities, item descriptions and a real scripting language worth
> rebuilding 334 files for?

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

Until that question is answered, **neither engine is deleted** and work is not
ported back out of this one.

## It builds two ROMs

| Target | Output | Edition |
|---|---|---|
| `make firered` | `pokefirered.gba` | **CONTENT** |
| `make leafgreen` | `pokeleafgreen.gba` | **CONTEXT** |

The edition split survived the port unchanged, which was the first good sign:
`firered` / `leafgreen` maps onto the Game Boy build's `_RED` / `_BLUE` with no
design work at all. Both disassemblies ship the same game twice and differ by a
build flag.

```sh
make firered           # build CONTENT
make leafgreen         # build CONTEXT
shasum -c firered.sha1 # prove the toolchain against the retail ROM
```

From the DAEMONS repo, `./bindDaemons.sh` builds and launches either edition;
`--classic` reaches the Game Boy build instead.

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
`DAEMONS/setup.sh` rebuilds it for you.

### Debug mode

**pokefirered ships no debug build** — unlike pokeemerald there is no menu to
port — so this one is ours:

```sh
make firered_debug     # -> pokefirered_debug.gba
make leafgreen_debug   # -> pokeleafgreen_debug.gba
```

A new game starts with six daemons chosen for their **abilities**, one of each
**kind** of item so the description window can be read, all eight badges, and
999999. **Hold B** to walk through grass unmolested.

Four field hotkeys, held with **L**:

| | |
|---|---|
| **L + R** | heal the party where you stand — a POKéCENTER on demand |
| **L + SELECT** | restock: balls, medicine, the four inputs, and money |
| **L + UP / DOWN** | walk the song table and play it — ours are at the far end |

They are hotkeys rather than a menu on purpose. A proper submenu means window
templates, a task, a callback and a tilemap — a day of UI for tools that exist
to save time.

It is scaffolding for the question above rather than a general debug menu.
`DAEMONS_DEBUG=1` suffixes `BUILD_NAME`, so it is a **separate ROM with its own
save** and a debug run never touches a real playthrough. It is deliberately not
part of `make all`, and **with `DAEMONS_DEBUG=0` both retail builds still match
their `.sha1` byte for byte** — the scaffolding is inert when it is off.

## What is ported so far

| | |
|---|---|
| The chart's vocabulary | all fifteen type names, plus `HARDENED` and `OPAQUE` |
| The chart's one change | `CONTEXT ↔ LATENT`, mutual 2× |
| 33 daemon names | every rename carried across |
| 12 item names | the box ladder, the four inputs, `INTERRUPT`, `RESOLVER`, `CC-7` |
| 12 item descriptions | **new writing** — Gen 1 had nowhere to put them |
| 30 Index categories and entries | reflowed from six short lines to three long ones |
| 14 town names | Blanche, Slate, Doldrum, Halftone, Ardor, Verdigris… |
| 66 sprites | front and back, **coloured by type** |
| 4 music tracks | re-emitted as MIDI, because nothing can be copied |
| The eight MARKS | as badge names, which is where Gen 3 keeps them |

**Colour is by type**, which makes it information rather than decoration — a
daemon is coloured by what it *is*, so the palette is another way of reading the
chart. Four of the anchors come from the Review Board's humours; the rest are
claims the bible now makes.

Everything else — the maps, the dialogue, the intro, the title screen, the
trainers — is untouched upstream. **The tools that did the porting live in
`DAEMONS/tools/`**, not here.

## What is not ported, and why

- **`CONSENSUS`.** §2.5 added it as a move on the Game Boy, where it had to be
  *inserted* before STRUGGLE because the AI treats any ID above STRUGGLE as
  "not a move". Gen 3 has 355 moves to Gen 1's 166 and no such assert, so here
  it is an append — but it still needs a slot, a name, an animation and a
  battle script, and none of that is a rename.
- **Item descriptions for anything we did not rename.** Twelve are written. The
  rest are still upstream's.

The MARKS *are* ported now. An earlier version of this file said they were not:
badges have no bag slot and no description in Gen 3, which is true, but they do
have **names**, as individual symbols in `src/strings.c`. "Not an item" is not
the same as "has no name".

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
[gb]: https://github.com/CodeMusic/pokered-daemons
