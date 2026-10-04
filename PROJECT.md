# Lugaru-Web Project

## Overview

Lugaru-Web is an effort to maintain and extend a browser-native version of **Lugaru**, based on the open-source Lugaru codebase and Ryan Deering's Loupgarenne WebAssembly work.

Ryan's WebAssembly build already ran the game in a modern browser before Lugaru-Web began. The project is therefore not a ground-up browser port.

**Lugaru-Web v1.0.0** is the preservation and browser-stabilization release built on that baseline. It concentrates on understanding and stabilizing the inherited WebAssembly changes, restoring historical behavior and content, browser reliability and usability, and documentation, rather than adding a new gameplay layer.

Large feature work should not come at the expense of understanding or maintaining the existing codebase.

---

## Inherited Baseline

The inherited browser baseline is:

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
    Current Lugaru-Web development branch.

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

## Verified Browser Baseline (historical, 2026-09-16)

The WebAssembly baseline was manually tested on **2026-09-16** in a Chromium-based browser (Google Chrome 153; see `Docs/build-provenance.md`).

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

It does not imply that every subsystem or campaign has been fully validated. `Docs/build-provenance.md` records that later testing found some gameplay inputs could stop responding after browser focus changes, so combat and full input behavior were not considered fully verified by this smoke test.

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
* jsoncpp,
* IDBFS (IndexedDB) for saved settings and progress,
* game data preloaded into `lugaru.data` and served from the same origin as the page.

GL4ES provides compatibility for Lugaru's legacy fixed-function OpenGL renderer. The pinned GL4ES revision is unchanged from the baseline; CMake applies one small local patch so that alpha-to-coverage reaches WebGL (B-18).

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
Docs/porting-notes.md
```

This file tracks only the current high-level state. Every item marked **Fixed** is committed; fixing commits are recorded in `Docs/porting-notes.md`. Issue IDs are stable; B-21 has not been assigned.

| ID   | Summary                                                                           | Status | Priority |
| ---- | --------------------------------------------------------------------------------- | ------ | -------- |
| B-1  | Web settings overridden after configuration load (`abb81d7`)                      | Fixed  | High     |
| B-2  | Browser main loop uses timeout scheduling as a consequence of B-1 (`abb81d7`)     | Fixed  | High     |
| B-3  | Browser persistence migrated from localStorage to IDBFS with safe legacy migration (`3d747d3`) | Fixed  | High     |
| B-4  | Animation bounds checking removed in shared code (`b222ddc`)                      | Fixed  | High     |
| B-5  | Asset case fallback: latent external-content compatibility risk                    | Closed — note retained | Low |
| B-6  | Web blur intentionally disabled; Options row unavailable (`fad80ac`)              | Fixed  | Low      |
| B-7  | Browser key rebinding replaced with Web-only nonblocking capture (`a7bfd51`)      | Fixed  | Medium   |
| B-8  | Controller gameplay/camera/menu UI Firefox/wired DualSense verified; tutorial physical QA and compatibility/tuning open | Partial | Medium |
| B-9  | Web build is optimized primarily for binary size                                  | Open   | Low      |
| B-10 | Synchronous level loading can block the browser main thread                       | Open   | Low      |
| B-11 | Account creation behavior was changed globally rather than only for Web builds    | Open   | Medium   |
| B-12 | WebAssembly runtime crash: memory access out of bounds (`7d4349b`)                | Fixed  | High     |
| B-13 | Historical yaw, tilt, and head behavior restored; combined physical QA passed (`0305947`) | Fixed  | Medium   |
| B-14 | Web NPOT mipmap compatibility fixed; snow and Fire loading-screen QA passed (`eea54f4`) | Fixed | Medium |
| B-15 | Fire-heavy Seven Tasks task3 Web sprite rendering optimized (`592e158`) | Fixed | High |
| B-16 | Delayed spatial exit lost Empire campaign progression; shared, not Web-specific (`97121db`) | Fixed | High |
| B-17 | Empire Sky Palace NPC clothing corrupted by inherited fire-texture path conversion (`6142c73`, `bf94eca`) | Fixed | Medium |
| B-18 | Dense Web foliage slowdown caused by GL4ES alpha-test rendering (`df72be9`) | Fixed | High |
| B-19 | Jump boost applied too quickly under fixed physics timestep (`21efae8`) | Fixed | High |
| B-20 | Temple tinted JPEG fur rendered with RGB striping (`85477f5`) | Fixed | Medium |

B-14 preserves native and Web POT behavior; only Web NPOT images requesting mipmaps fall back to `GL_LINEAR` without automatic generation. SnowFlake retains its original mipmap request and assets are unchanged. See [B-14 evidence](Docs/porting-notes.md#b-14--web-npot-textures-render-black-with-mipmapped-sampling). The separate fire/sprite slowdown is [resolved in backlog A8](Docs/research-backlog.md#a8-dense-fire-spawner-web-slowdown--resolved-for-r1-2026-09-28): B-15 uses client arrays on Web, preserves native rendering and historical particles, and passed project-owner production QA. See [B-15 evidence](Docs/porting-notes.md#b-15--dense-fire-sprite-rendering-slows-web).

B-16 closes the Empire progression blocker found during Web campaign QA: leaving the Sky Palace bridge exit during the completion delay no longer stalls routing or causes count-based death reloads to skip Rebel Outpost. The defect was inherited shared logic, not WASM-specific. See [B-16 evidence](Docs/porting-notes.md#b-16--delayed-spatial-exit-loses-campaign-progression-empire-sky-palace) and [backlog A9](Docs/research-backlog.md#a9-empire-sky-palace-campaign-progression--resolved-for-r1-2026-09-29).

B-17 closes an inherited OSS Empire content-conversion regression: two historically unresolved `fire.png` clothing references in Sky Palace had been remapped to the opaque `Fire.jpg` loading texture, corrupting two NPC appearances. The shipped map now omits those dead clothing layers and the obsolete conversion rule has been removed so the defect cannot be recreated. See [B-17 evidence](Docs/porting-notes.md#b-17--empire-sky-palace-npcs-receive-firejpg-as-clothing) and [backlog A9b](Docs/research-backlog.md#a9b-empire-sky-palace-npc-clothing-corruption--resolved-for-r1-2026-09-30).

B-18 closes a Web-specific dense-foliage performance regression exposed by Empire's Sky Palace tree cluster. Controlled A/B testing isolated the bottleneck to overlapping alpha-tested leaf triangles rather than object count or vertex submission. The Web renderer now sorts normal-pass tree leaves front-to-back and uses 4x MSAA alpha-to-coverage instead of GL4ES's legacy alpha-test discard path; the pinned GL4ES dependency is patched during CMake configuration so that alpha-to-coverage reaches WebGL. Native rendering and all original Empire foliage content remain unchanged. A fresh Emscripten 6.0.9 build reproduced the fix with correct cutouts and smooth runtime behavior. See [B-18 evidence](Docs/porting-notes.md#b-18--dense-web-foliage-causes-severe-slowdown) and [backlog A9c](Docs/research-backlog.md#a9c-empire-sky-palace-dense-foliage-web-slowdown--resolved-for-r1-2026-09-30).

---

## Build Status

### WebAssembly

The v1.0 Web release is built with:

```text
WSL2
Ubuntu 26.04.1
Emscripten 6.0.9
CMake 4.2.3
Release build directory: build-web-clean
```

The build produces:

```text
lugaru.html
lugaru.js
lugaru.wasm
lugaru.data
```

The four files must be deployed together. The current source and local build load `lugaru.data` from the same origin and directory as `lugaru.html`, not from Ryan Deering's server (`36407bd`); this has not yet been smoke-tested on the public deployment. The files must be served over HTTP rather than opened through `file://`.

Exact environment information, dependency revisions, build commands, compiler configuration, known warnings, and the preserved 2026-09-16 baseline record are maintained in:

```text
Docs/build-provenance.md
```

Per-issue runtime validation is recorded in `Docs/porting-notes.md`. SHA-256 hashes and the source commit of the final published v1.0.0 artifacts are still to be recorded in `Docs/build-provenance.md` when the release is tagged.

### Native Builds

Native builds should remain available as comparison targets where practical. They are not part of the v1.0 Web release and are not release-validated.

A change to shared game code should not knowingly break a supported native target without a documented reason.

The repository's GitHub Actions workflows are inherited unchanged from Loupgarenne: they build native Linux, macOS and Windows targets under `lugaru-loupgarenne-*` names, and none builds the Web target. The inherited macOS workflow was observed failing before compilation because it attempts to install an x86_64 Homebrew environment on an Apple Silicon runner. This is considered a baseline CI configuration issue rather than a source-level regression. See CI-1 in `Docs/porting-notes.md`.

---

## Current Development Phase

### v1.0.0 — Preservation and Browser Stabilization Release

The baseline-preservation and stabilization work that began from `baseline/wasm-b340204` is complete for v1.0.0:

* inherited Web regressions in settings, frame scheduling, persistence, bounds checking, sprite deletion, key rebinding, and texture handling are fixed;
* historical character turning, tilt, head behavior, and jump timing are restored;
* Web sprite and foliage performance regressions are fixed without reducing historical content;
* Empire progression/clothing and Temple fur/clothing content are repaired or restored, with provenance recorded;
* the audited PNG/JPEG image-decode and clothing-compositing paths are hardened with size/dimension validation; known unchecked paths outside that audit remain documented, and unfixed, in backlog A11;
* controller support covers gameplay, camera, menus, campaign/world map, and challenges;
* the current source and local build are configured to load `lugaru.data` from the same origin as the page (`36407bd`); smoke testing of the public deployment is still pending.

The remaining work before publishing is release packaging and verification, not new gameplay. Major gameplay additions continue to wait until after v1.0.0; see **Near-Term Priorities**.

---

## Character restoration milestone — completed (2026-09-27)

**VERIFIED SOURCE FINDING / RUNTIME-VERIFIED:** bounded yaw, bounded body tilt, and historical head tracking adapted to fresh-simulation/draw ordering are now default behavior, committed as `0305947` (merged at `d21048e`). The user reports normal combined gameplay with controller behavior intact and no perceptible remaining player-locomotion problem. No verified remaining player-locomotion regression currently warrants another code change.

No experiment definitions are needed in normal builds. Current OSS rabbit TalkIdle is preserved. The unresolved OSS-scheduler experiment is saved separately outside the repository; the normal fixed scheduler is unchanged. See [B-13](Docs/porting-notes.md#b-13--player-direction-changes-snap-instead-of-rotating) for provenance, build evidence, and patch location, and [backlog A2a](Docs/research-backlog.md#a2a-npc-slopeseam-and-scheduler-comparison--open) for the remaining NPC observation. Camera follow is a separate subsystem only to revisit if a meaningful discrepancy is observed.

## Near-Term Priorities

### Completed stabilization sequence

The original stabilization priorities are complete: build provenance and porting notes are documented; inherited animation bounds checking is restored; Web settings, frame scheduling, persistence, and key rebinding are fixed; the asset-case investigation is closed as a low-priority compatibility note (B-5); and controller support is implemented — foundation (`e5939fe`), Classic digital movement (`4b8af1d`), gameplay actions (`2387ebf`), camera look (`ab46b53`), pause/menu UI (`8d549e4`), Triangle tutorial advance (`bd7b207`), and stage-3 right-stick recognition (`f00ec72`). See the issue table above and [B-8](Docs/porting-notes.md#b-8--controller-gameplay-menuui-and-tutorial-support) for evidence and limits.

### v1.0.0 release readiness

1. Reconcile release documentation and licensing files with the current source.
2. Audit and reconcile the inherited GitHub Actions build/release workflows before tagging: they still use Loupgarenne-era `lugaru-loupgarenne-*` naming, trigger releases on `*-loupgarenne` tags, and package native targets only. Lugaru-Web naming, tag behavior, and release packaging must be settled before the v1.0.0 tag is created.
3. Build the final release artifacts with Emscripten 6.0.9 from a clean `build-web-clean` configuration, and record the source commit and artifact hashes in `Docs/build-provenance.md`.
4. Smoke-test the public deployment, confirming that `lugaru.data` loads from the same origin.
5. Physically verify the Triangle tutorial advance and tutorial stage-3 right-stick recognition (B-8), or ship them documented as source/automated-validated only.
6. Optionally confirm the final Web `Blur Effects: Unavailable on Web` Options row physically (B-6 notes this as a short remaining manual UI check).

### After v1.0.0

1. Isolate the inherited global `"Player"` auto-account behavior to Web builds, or document why it should remain shared (B-11).
2. Controller compatibility and feel: broader browser/Bluetooth coverage, sensitivity/threshold tuning, settings/prompts (B-8, backlog A6). True analog locomotion remains a separate design question; Classic digital intent is unchanged.
3. Benchmark WebAssembly optimization settings (B-9).
4. Improve browser loading behavior if testing shows it is necessary (B-10).
5. Broader CI modernization beyond the pre-tag workflow audit: restore useful native CI coverage, fix the macOS dependency setup, add a Web build job, and add caching where practical (CI-1, backlog A5).

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

Licensing, dependency and contributor information is maintained in:

```text
COPYING.txt
    GNU GPL version 2 license text for the source code.

CONTENT-LICENSE.txt
    Game asset licenses, campaign attribution and Lugaru-Web content modifications.

THIRD-PARTY-NOTICES.txt
    Notices and licenses for third-party software used by the Web build, including GL4ES and jsoncpp.

AUTHORS
    Original developer, OSS contributors, Loupgarenne and Lugaru-Web authorship and lineage.
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
README.md
    Public overview, play link, controls and known Web limitations.

PROJECT.md
    Stable project direction, current phase and scope.

Docs/README.md
    Index of the technical, historical, preservation and design documentation.

Docs/build-provenance.md
    Current release build procedure, dependency versions and commands, and the preserved baseline build record.

Docs/porting-notes.md
    Detailed WebAssembly issues, testing observations and resolutions.

Docs/architecture.md
    Current engine mental model, important couplings, source archaeology and extension seams.

Docs/research-backlog.md
    Unresolved technical and historical questions.

COMPILING.md
    Build entry points for the Web build and inherited native instructions.

RELEASE-NOTES.md
    Lugaru-Web release notes, followed by inherited OSS Lugaru release history.
```

There is no separate contribution guide at present. `AGENTS.md` records the working rules used for automated-assistant contributions.

Documentation should describe the project and its technical reasoning rather than serve as a transcript of development sessions.

---

## Project Direction

The v1.0.0 objective is:

**Turn the existing working Lugaru WebAssembly port into a documented, reproducible, stable and maintainable browser version of Lugaru.**

Future development should build on that foundation rather than replace it.
