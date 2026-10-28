# Building

## The toolchain

**devkitARM is not required.** Homebrew's `arm-none-eabi-gcc` installs without `sudo`, but it ships **no libc**, so
the `MODERN=1` path dies on `string.h`. **agbcc** brings its own headers and is the path that works:

```sh
brew install arm-none-eabi-gcc libpng
git clone https://github.com/pret/agbcc /tmp/agbcc
cd /tmp/agbcc && ./build.sh && ./install.sh /path/to/pokefirered-daemons
```

agbcc installs **into this checkout**, so it does not survive a fresh clone. The easy way is the DAEMONS repository's
`./setup.sh`: it clones this fork beside the design repo, sets `upstream`, and builds agbcc for you. It is idempotent.

## The targets

| Target | Output | |
|---|---|---|
| `make firered` | `daemonsContent.gba` | **CONTENT** |
| `make leafgreen` | `daemonsContext.gba` | **CONTEXT** |
| `make firered_debug` | `daemonsContent_debug.gba` | CONTENT, the testing build |
| `make leafgreen_debug` | `daemonsContext_debug.gba` | CONTEXT, the testing build |

The ROMs are named for the editions, and the `.elf`, `.map`, `.sym` and mGBA's `.sav` follow the same name, so every
build keeps its own save. From the DAEMONS repository, `./bindDaemons.sh` builds an edition and launches it
(`context`, `--debug`; `--help` lists everything).

## Check the exit code. Never grep for `error:`

agbcc prints its diagnostics as `warning:` lines and then fails with a bare `Error 1`, so a filtered build looks clean
while the ROM silently keeps the previous binary:

```sh
make firered -j8 >/dev/null 2>&1; echo "exit=$?"
```

## `firered.sha1` fails, by design

The ROM carries our content, so `shasum -c firered.sha1` no longer matches. It proves the toolchain on a pristine
`pret/pokefirered` checkout and is not a regression check for this fork. DAEMONS' `tools/check_fresh_clone.py` is the
check that matters for a release: it clones this repository from GitHub, builds both editions and compares them byte
for byte with a local build.

## Edit the source, never the artifact

`json_data_rules.mk` generates several headers -- `items.h`, the region map's strings and entries, the wild
encounters, the heal locations and two constants files -- and every one is gitignored. A change written into a
generated header exists only in one working build; a fresh clone builds without it. Write the `.json`.

And most of what DAEMONS changed came through a tool in the DAEMONS repository, not by hand
([Inside the tree](Inside-the-tree)): change the tool's source, then run it with `--write`.
