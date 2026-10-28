# Branches and upstream

| Branch | What it is |
|---|---|
| `context-content` | the conversion -- **this is the work**, and the default |
| `master` | tracks [pret/pokefirered](https://github.com/pret/pokefirered) `master`, untouched |
| `symbols` | tracks upstream `symbols`, untouched |

`origin` is this fork; `upstream` is pret. `git pull upstream master` brings in their fixes, and the untouched
tracking branches are what make that painless. DAEMONS' `setup.sh` sets the `upstream` remote for you.

## Why a fork beside the design, not inside it

The DAEMONS repository holds the design and the tools, and **never vendors this engine**: the engine carries
Nintendo-derived graphics, and the design repository promises to distribute nothing copyrighted. So the engine is a
sibling repository, linked into a DAEMONS checkout as the gitignored symlink `engineGba/`. The pattern, its tradeoffs
and the alternatives are written up in
[`docs/two-repo-pattern.md`](https://github.com/CodeMusic/DAEMONS/blob/main/docs/two-repo-pattern.md) there.

## The Game Boy build

[CodeMusic/pokered-daemons](https://github.com/CodeMusic/pokered-daemons) is where DAEMONS began, on pret's pokered. It
still compiles and runs the vertical slice, and is the source the port tools read from; it is not updated further.
DAEMONS moved here on 2026-09-03, after a one-day spike asked whether abilities, item descriptions and a real
scripting language were worth rebuilding 334 files for (the design bible's 9.3).
