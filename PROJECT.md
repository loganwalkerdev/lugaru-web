# Lugaru Web Project

## Overview

Lugaru Web is an effort to maintain and extend a browser-native version of **Lugaru**, based on the open-source Lugaru codebase and Ryan Deering's Loupgarenne WebAssembly work.

A functional WebAssembly build already exists and has been verified to run the game in a modern browser. The project is therefore not a ground-up browser port.

The current objective is to preserve that working baseline, understand and stabilize the inherited WebAssembly changes, improve browser-specific reliability and usability, and establish a maintainable foundation for future development.

Large feature work should not come at the expense of understanding or maintaining the existing codebase.

---

## Current Baseline

The current browser baseline is:

```text
Loupgarenne feature/wasm-support
Commit: b340204635e86a4385ef88dc45e2696e2213d9ab
```

This baseline is preserved with:

```text
baseline/wasm-b340204
```

Relevant references:

```text
main
    Current Lugaru Web development branch.

loupgarenne-main
    Ryan Deering's Loupgarenne main branch.

upstream/master
    Local reference to WolfireGames/lugaru.
```

The canonical OSS Lugaru development project is maintained at:

```text
https://gitlab.com/osslugaru/lugaru
```

The existing Git history is retained so inherited work remains attributed to its original authors.

---

## Verified Browser Baseline

The WebAssembly baseline was manually tested on **2026-09-16** in a Chromium-based browser.

Surface-level testing confirmed:

* game startup and menus,
* tutorial and campaign levels,
* character models and animation,
* textures and terrain,
* keyboard controls,
* combat,
* audio,
* fullscreen,
* normal game progression.

This establishes `b340204` as the known-good browser baseline.

It does not imply that every subsystem or campaign has been fully validated.

---

## Current Web Architecture

The browser build currently uses:

* C++ Lugaru source,
* Emscripten and WebAssembly,
* SDL2,
* WebGL, with the build permitting up to WebGL 2,
* GL4ES,
* browser-compatible audio support,
* PNG, JPEG, Ogg and Vorbis support,
* jsoncpp.

GL4ES provides compatibility for Lugaru's legacy fixed-function OpenGL renderer.

Because the renderer is currently functional through GL4ES, replacing or substantially rewriting it is not a project objective unless profiling or compatibility testing demonstrates a concrete need.

Incremental improvements should be preferred over unnecessary architectural rewrites.

---

## Development Principles

### Preserve the Working Baseline

The browser build should remain playable throughout development whenever reasonably possible.

Changes to shared engine code should consider native behavior as well as browser behavior. Native builds can act as a reference when determining whether an issue is inherited from Lugaru or introduced by the WebAssembly port.

### Make Small, Reviewable Changes

Changes should address one logical concern whenever practical.

Before a change is committed, its diff should be reviewed, its purpose understood, the affected target built, and the relevant behavior tested.

Suggested or generated code is a proposal until it has been understood, reviewed, built, and tested.

### Preserve History and Attribution

Existing author attribution and upstream history should remain intact.

Imported patches or work derived from other projects should retain appropriate attribution and licensing information.

### Fix Defects Before Expanding Scope

Problems affecting stability, saved progress, input, timing, portability, or maintainability should generally take priority over new gameplay systems.

### Measure Before Optimizing

Performance changes should be based on reproducible measurements or profiling rather than assumptions.

---

## Known Browser Issues

Detailed investigation notes belong in:

```text
docs/porting-notes.md
```

This file tracks only the current high-level state.

| ID   | Summary                                                                           | Status | Priority |
| ---- | --------------------------------------------------------------------------------- | ------ | -------- |
| B-1  | Web settings overridden after configuration load                                  | Fixed  | High     |
| B-2  | Browser main loop uses timeout scheduling as a consequence of B-1                 | Fixed  | High     |
| B-3  | Browser persistence uses localStorage; validation failure silently discards saves | Open   | High     |
| B-4  | Animation bounds checking removed in shared code                                  | Fixed  | High     |
| B-5  | Asset filename case fallback is incomplete                                        | Open   | Medium   |
| B-6  | Unsupported motion blur option remains visible                                    | Open   | Low      |
| B-7  | Browser key rebinding relies on unavailable threading behavior                    | Open   | Medium   |
| B-8  | Controller input is not currently initialized or implemented                      | Open   | Medium   |
| B-9  | Web build is optimized primarily for binary size                                  | Open   | Low      |
| B-10 | Synchronous level loading can block the browser main thread                       | Open   | Low      |
| B-11 | Account creation behavior was changed globally rather than only for Web builds    | Open   | Medium   |
| B-12 | WebAssembly runtime crash: memory access out of bounds                            | Fixed  | High     |

---

## Build Status

### WebAssembly

A WebAssembly build has been successfully produced using:

```text
WSL2
Ubuntu
Emscripten 6.0.9
```

The build produces:

```text
lugaru.html
lugaru.js
lugaru.wasm
lugaru.data
```

The resulting build has been manually verified as playable through a local HTTP server.

Exact environment information, dependency revisions, build commands, compiler configuration and known warnings should be maintained in:

```text
docs/build-provenance.md
```

### Native Builds

Native builds should remain available as comparison targets where practical.

A change to shared game code should not knowingly break a supported native target without a documented reason.

The inherited macOS CI workflow currently fails before compilation because it attempts to install an x86_64 Homebrew environment on an Apple Silicon runner. This is considered a baseline CI configuration issue rather than a source-level regression.

---

## Current Development Phase

### Phase 0 — Baseline Preservation and Stabilization

The project is currently focused on understanding, documenting and stabilizing the inherited WebAssembly implementation.

Major gameplay additions should wait until the browser foundation is sufficiently understood and reliable.

See **Near-Term Priorities** for the active work sequence.

---

## Near-Term Priorities

Current priorities are approximately:

1. Document reproducible build provenance.
2. Create detailed WebAssembly porting notes.
3. Restore inherited shared-code behavior that was unintentionally changed, including animation bounds checking and account creation behavior.
4. Correct browser settings handling and frame scheduling together.
5. Investigate and harden browser persistence.
6. Verify and repair browser key rebinding.
7. Investigate asset filename case mismatches.
8. Add or integrate controller support.
9. Benchmark WebAssembly optimization settings.
10. Improve browser loading behavior if testing shows it is necessary.
11. Restore useful native CI coverage where practical.

Items not listed here remain tracked but are not currently scheduled.

The exact order may change when testing exposes dependencies or more serious defects.

---

## Determinism

Deterministic simulation may become useful for future reproducible game modes, testing, replay systems or other extensions.

The current game uses behavior including standard-library random generation seeded from wall-clock time, with relevant code beginning in:

```text
Source/Math/Random.hpp
```

Simulation timing would also require investigation before identical seeds could be expected to produce identical runs across different machines or frame rates.

This is not an immediate development priority. If systems requiring reproducibility are pursued, determinism should be treated as a prerequisite rather than a later addition.

---

## Future Direction

The project may eventually explore browser-native additions beyond the original Lugaru experience.

Possible areas include new challenge or combat modes, replay-oriented features, customization, and improved support for community-created content.

These are intentionally not defined as committed roadmap items. Specific features should be evaluated only after the browser build is stable and their technical requirements are understood.

---

## Scope Boundaries

The following are not current project objectives:

* rewriting Lugaru from scratch,
* replacing the renderer without demonstrated need,
* large unrelated engine refactors,
* implementing multiplayer,
* adding monetization systems,
* incorporating proprietary Overgrowth code or assets,
* prioritizing major gameplay expansion over baseline stabilization.

These boundaries can be revisited as the project matures.

---

## Licensing and Attribution

Lugaru source code and game content have separate licensing requirements.

The source code is distributed under **GNU GPL version 2 or later (GPLv2+)**.

Content under `Data/` is distributed under Creative Commons Attribution-ShareAlike licenses as documented by `CONTENT-LICENSE.txt`, including **CC BY-SA 3.0** and **CC BY-SA 4.0** content.

GL4ES is distributed under the MIT license and requires its applicable copyright and license notices to be preserved.

Additional dependency and contributor information should be maintained in:

```text
ATTRIBUTION.md
THIRD-PARTY.md
```

Any public distribution of compiled WebAssembly builds, including hosting them on a public web server, must satisfy the applicable GPL source-distribution requirements.

Modified or derived game assets must follow the applicable Creative Commons license.

Proprietary Overgrowth code or assets must not be incorporated into this project.

### Project Naming

The open-source licenses covering Lugaru's code and content do not themselves grant trademark rights.

The current project name describes its relationship to Lugaru rather than claiming to be an official Wolfire release. Long-term public branding and naming should be reviewed before the project is broadly promoted or distributed.

---

## Documentation

Project documentation should remain concise, technical and maintainable.

```text
PROJECT.md
    Stable project direction, current phase and scope.

docs/build-provenance.md
    Reproducible build environment, dependency versions and commands.

docs/porting-notes.md
    Detailed WebAssembly issues, testing observations and resolutions.

docs/decisions/
    Significant architectural decisions when additional explanation is useful.

CONTRIBUTING.md
    Contribution and commit practices if outside contribution becomes relevant.
```

Documentation should describe the project and its technical reasoning rather than serve as a transcript of development sessions.

---

## Project Direction

The immediate objective is:

**Turn the existing working Lugaru WebAssembly port into a documented, reproducible, stable and maintainable browser version of Lugaru.**

Future development should build on that foundation rather than replace it.
