# pokefirered-daemons

**The engine of [DAEMONS: CONTEXT / CONTENT](https://github.com/CodeMusic/DAEMONS)** -- a fork of
[pret/pokefirered](https://github.com/pret/pokefirered), and where the game is built. The creatures are daemons and the
type chart is an argument about consciousness; this tree is the GBA game that makes it.

This wiki is about **the fork**: what it is, how to build it, the debug build, and where DAEMONS' own code lives in the
tree. Everything about the game itself -- playing it, the world, the editions, the companion, the releases -- is on
**[the DAEMONS wiki](https://github.com/CodeMusic/DAEMONS/wiki)**, and the design is in
[`docs/vision.md`](https://github.com/CodeMusic/DAEMONS/blob/main/docs/vision.md) there.

## Pages

| page | what it covers |
|---|---|
| [Building](Building) | the toolchain (agbcc, no devkitARM), the four targets, checking the exit code, generated files |
| [The debug build](The-debug-build) | the testing ROM with its own save, START -> DEBUG, the field hotkeys, mGBA's keys |
| [Inside the tree](Inside-the-tree) | DAEMONS' own source files, the tools in the DAEMONS repo that write into this one, and what is ours |
| [Branches and upstream](Branches-and-upstream) | `context-content`, the untouched tracking branches, pulling pret's fixes |

## Two editions from one tree

| Target | Output | Edition |
|---|---|---|
| `make firered` | `daemonsContent.gba` | **CONTENT** |
| `make leafgreen` | `daemonsContext.gba` | **CONTEXT** |

The type chart is byte-identical in both: it is the argument, and an argument that changes by cartridge is not one.
What differs is which daemons you meet and what the INDEX says about them
([Editions](https://github.com/CodeMusic/DAEMONS/wiki/Editions)).

## No ROM here, ever

This repository is a disassembly; it holds no ROM. Releases are **BPS patches** for a cartridge you own, published on
the [DAEMONS releases page](https://github.com/CodeMusic/DAEMONS/releases) -- four to a release, CONTENT for *FireRed*
1.0 and Rev 1, CONTEXT for *LeafGreen* 1.0 and Rev 1
([Releases](https://github.com/CodeMusic/DAEMONS/wiki/Releases)).

## The family

| | |
|---|---|
| [CodeMusic/DAEMONS](https://github.com/CodeMusic/DAEMONS) ([wiki](https://github.com/CodeMusic/DAEMONS/wiki)) | the design: the bible, the changelog, and every tool that writes into this tree |
| **CodeMusic/pokefirered-daemons** | this: the GBA engine, where the game is |
| [CodeMusic/pokered-daemons](https://github.com/CodeMusic/pokered-daemons) | the Game Boy build, where the vertical slice was made; a reference now |
| [CodeMusic/daemons-companion](https://github.com/CodeMusic/daemons-companion) ([wiki](https://github.com/CodeMusic/daemons-companion/wiki)) | where a daemon goes when it is SENT out of the game: the app, the hub, the boards |
| [CodeMusic/daemons-playground](https://github.com/CodeMusic/daemons-playground) ([wiki](https://github.com/CodeMusic/daemons-playground/wiki)) | the game in a browser, for demos and beta tests on your own network |
| [CodeMusic/gpt-play-pokemon-firered-daemons](https://github.com/CodeMusic/gpt-play-pokemon-firered-daemons) | the harness that lets a model play it |

*Pokémon is Nintendo / Creatures / GAME FREAK. This fork is unaffiliated with and unendorsed by them. The disassembly is
pret's and its contributors'; the writing, music, design and art of DAEMONS are CodeMusic's.*
