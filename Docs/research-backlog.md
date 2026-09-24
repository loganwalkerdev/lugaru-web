# Lugaru-Web Research Backlog

This is not a feature roadmap. It is a queue of questions whose answers would materially improve architecture, historical accuracy, preservation, or future design decisions.

## A. Source/architecture

### A1. Build the full code mental map

Trace, with exact files/functions/types:

```text
SDL/platform input
→ player input state
→ Person movement/facing
→ animation
→ skeleton/ragdoll
→ combat/weapons
→ AI perception
→ level/map
→ rendering/audio/HUD
```

For each subsystem record:

- owning files/types;
- major data dependencies;
- global state;
- cross-system calls;
- assumptions about `players[0]`;
- Classic-sensitive behavior;
- useful extension seams;
- provenance where meaningful.

### A2. B-13 player rotation regression

Reproduce at 60, 120, 144+ Hz across:

- OSS upstream;
- Loupgarenne main;
- `feature/wasm-support` baseline;
- current Lugaru-Web.

Trace facing/interpolation/timestep behavior before making a fix.

### A3. Motion blur on Web

Investigate why Ryan disabled motion blur in browser builds.

Read the known relevant regions of `GameDraw.cpp`, settings/menu code, framebuffer/post-process requirements, and GL4ES/WebGL constraints.

Goal: either restore it correctly or hide/disable the option honestly on Web.

### A4. Web key rebinding — fixed (2026-09-24)

**VERIFIED SOURCE FINDING / RUNTIME-VERIFIED:** B-7 was caused by `Menu::setKeySelected()` attempting `SDL_CreateThread()` with the Web build's non-threaded SDL backend. Failure cleared `waiting` but left `keyselect` set. Commit `a7bfd51` adds Web-only nonblocking capture; Chrome testing confirmed keyboard/mouse rebinding, Escape cancellation, subsequent selection, gameplay use, and save/reload persistence.

### A4a. Native rebinding thread affinity / portability

**OPEN QUESTION:** The native `setKeySelected_thread()` still calls `SDL_WaitEvent()` from a worker, although SDL documents that this function must run on the thread that initialized video. It also mutates shared menu state. Native behavior was intentionally left unchanged by B-7. Determine the practical impact across supported native platforms before proposing a separate fix; no native runtime failure has been established by this investigation.

### A5. Native build health

Resolve/understand the current native Linux SDL2 include-path/build issue so behavior can be compared against browser builds more easily.

---

## B. Gameplay-system seams

### B1. Corpse dragging prototype study

Map:

- ragdoll ownership;
- stable joints/constraints;
- collision behavior;
- corpse smell/perception references;
- transition between dead/ragdolled/animated states.

Do not implement until the physical ownership path is understood.

### B2. AI perception map

Document exact code paths for:

- sight;
- occlusion;
- hearing;
- posture/height;
- smell;
- corpse/blood detection;
- alert state;
- pathfinding/investigation.

Use this to evaluate body discovery and AI communication without inventing a parallel stealth system.

### B3. Combat event extraction

Identify authoritative points for:

- hit;
- counter/reversal;
- disarm;
- knockdown;
- death;
- thrown-weapon hit;
- environmental kill.

Goal: a semantic event seam with no gameplay behavior change.

### B4. PersonType audit

List every direct rabbit/wolf check and classify:

- should remain species-specific;
- belongs in `PersonType` data/capability;
- accidental legacy coupling;
- unknown until new species work begins.

### B5. Tutorial coupling audit

Count and classify `Tutorial::active` / stage references in gameplay code.

Separate:

- behavior genuinely required by the historical tutorial;
- checks that could later become semantic tutorial events;
- code that must remain untouched for preservation.

---

## C. Multiplayer

### C1. `players[0]` census

Find all assumptions that the first Person is the only human player.

Classify by:

- input;
- camera;
- HUD;
- audio;
- targeting;
- scoring;
- account/progress;
- menu/tutorial logic.

### C2. Local two-human design

Before networking, design the smallest local two-human prototype and determine:

- camera model;
- input ownership;
- spawn ownership;
- per-player state;
- what simulation state actually needs replication later.

### C3. Networking experiment

After local multiplayer works, evaluate WebRTC data channels + host authority.

Do not choose deterministic lockstep unless testing proves the old simulation suitable.

---

## D. Editor/content

### D1. Browser editor viability

Enable/access the existing debug map editor in the Web build and verify:

- object placement;
- player/enemy placement;
- path points;
- weapons;
- save/load through IDBFS;
- JSON compatibility;
- reload/persistence.

### D2. First custom-content experiment

Duplicate a small level, alter terrain/objects, add an existing enemy/weapon, and load it as a new level.

This is a better first content test than building a new editor UI.

### D3. Campaign metadata

Inspect the current multiple-campaign code and data to determine the smallest metadata layer needed for modern campaign cards.

---

## E. Historical archaeology

### E1. Full mod census

Create a catalog covering:

- campaigns;
- standalone maps;
- map packs;
- skins/clothing;
- models;
- skeletons;
- animations;
- weapons;
- textures;
- music/audio;
- utilities;
- converters;
- Blender plugins;
- mod installers;
- documentation.

### E2. Philtron 14-map recovery

Recover the exact forum archive/file and match it to the documented CC BY-SA grant.

### E3. rudel_ic / Wolf Mathwig archive

Recover:

- nine-map pack;
- X2Solid;
- Brainfart;
- Blender plugins;
- animation/skeleton tools;
- readmes/source;
- original site/forum posts.

Then locate a current public contact channel tied to his development identity.

### E4. Lotus Wolf / Weapon Shop

Reconstruct:

- Weapon Shop inventory;
- downloadable filenames;
- Google Sites archive;
- Reluctant Assassin package history;
- contest/video/account breadcrumbs;
- any public current contact.

### E5. tannim

Read the full OSS issue history around the "waiting for a public license statement" note and trace old competition/forum/video accounts.

### E6. BunnyWithStick, Levbot, eyeCube

Start from exact original forum/wiki records and profile IDs. Search outward through archived websites, signatures, filenames, and mirrors.

### E7. Lugaru Downloader

Find the installer/downloader itself, then inspect for a hardcoded or remote catalog. Recover URLs and filenames even when payloads are dead.

### E8. ModDB and old mirrors

Build a deduplicated index rather than downloading random files without provenance.

---

## F. Licensing/restoration

### F1. Ancestral Tales rights audit

Separate:

- author-owned maps/campaign data;
- stock Lugaru assets;
- modified stock assets;
- unknown third-party textures.

Determine whether a clean restored distribution can be produced by replacing only questionable assets.

### F2. Stock asset hash database

Hash canonical stock/OSS `Data` assets and historical releases when available.

Use it to automate old-mod classification.

### F3. Relicensing contact template

Prepare a short, respectful message that:

- names exact works;
- asks whether the creator controls their original contribution;
- requests CC BY-SA 4.0;
- asks about third-party assets;
- offers preferred attribution wording;
- explains historical preservation purpose.

### F4. Restoration metadata format

Define a small `RESTORATION.md` template and catalog schema before producing modified historical packages.

---

## G. Lugaru 2 / Overgrowth history

### G1. Build a chronological research table

For each important concept record:

```text
Original Lugaru
→ early Lugaru 2 proposal/prototype
→ early Overgrowth
→ later/final Overgrowth
→ documented reason for change, if known
→ relevance to Lugaru-Web
```

### G2. Mine Wolfire 2005–2008

Focus on the pre-Overgrowth Lugaru 2 period:

- Phoenix;
- animation;
- movement;
- early races;
- physics;
- editor/tooling;
- multiplayer;
- level/ruin concepts.

### G3. Mine 2008–2010 early Overgrowth

Focus on concepts closest to Lugaru 2:

- mission types;
- stealth/non-lethal approaches;
- combat redesign;
- animation/physics integration;
- editor architecture;
- co-op/gamepad plans;
- species changes.

### G4. Later Overgrowth comparison

Identify which early ideas survived, changed, or disappeared by the mature game.

Abandoned ideas may be more relevant to Lugaru-Web than final Overgrowth because Lugaru-Web is extending Lugaru, not recreating Overgrowth.

---

## H. Quirky-system archaeology follow-ups — 2026-09-23

The broad source/data pass on campaign flow, dialogue, hotspots, scoring, tutorial behavior, AI perception, environment/music, ragdolls, blood, fire, weapons, and challenge maps has moved many questions from "unknown architecture" to "known behavior with targeted verification remaining."

### H1. Rendering-owned AI occlusion regression test

Current source updates `Person::occluded` from `GameDraw.cpp` using camera/viewer-to-NPC collision checks, while `Person::doAI()` uses `occluded` to gate portions of visual perception before performing its own head-to-target line-of-sight test.

Build a controlled test map and vary:

- camera position without moving Turner;
- intervening geometry between camera and NPC but not NPC and Turner;
- intervening geometry between NPC and Turner;
- first-person/alternate debug camera conditions if available;
- 60 Hz vs. high refresh.

Determine whether camera occlusion can materially change detection in ordinary gameplay. Do not refactor this coupling before reproducing the behavior.

### H2. Hotspot radius semantics

Current trigger checks compare squared distance to raw `Hotspot::size`, while editor visualization appears to treat `size` like a linear radius.

Create known hotspots with values such as `1`, `4`, `9`, `16`, `25`, and measure the real trigger distance. Then compare with editor display and historical author expectations.

If the mismatch is real, preserve Classic behavior and decide whether modern Create UI should display both the stored value and effective trigger radius.

### H3. `ChooseNext = 2` object-state test

Source reading shows stealth loading preserves current world objects while rebuilding Persons/dialogues/hotspots/weapons/path data and preserving Turner position/yaw. Reproduce `Lugaru1 -> Lugaru2` in-game and compare with directly loading `Lugaru2`.

Record:

- which physical objects differ;
- whether destructible/object runtime state survives;
- decals/sprites that survive or are cleared;
- player inventory/weapon consequences;
- camera/player placement;
- whether any current map JSON object differences are intentionally ignored in campaign flow.

This should become a regression fixture because it is a rare but architecturally important transition path.

### H4. AI constant provenance

`guardtype = 1` and `attacktype = 3` remain declared but have no live references in the current tree, while active chase/combat behavior uses the oddly named `attacktypecutoff = 4`.

Trace git history and older source releases to determine:

- whether guard/attack states once had behavior;
- when `attacktypecutoff` became the active attack state;
- whether numeric values changed during source cleanup;
- whether old maps/tools ever serialized these states.

Do not rename/reassign the constants until provenance is known.

### H5. Dialogue opcode archaeology

Current bundled JSON establishes live use of types `1–5`, `10`, `11–14`, `21/23/25/26`, `30–32/34–36`, `41`, and `51`. Continue historical searches for:

- any map using type `40`, `42–49`, or `52–59`;
- negative scene sounds `-5` and `-6`;
- nonzero `participantaction`;
- a map/tool/readme that documents the numeric ranges explicitly;
- the provenance of the sole type-10 dialogue in `jendraz1`.

The `jendraz1` type-10 scene currently appears unreachable through ordinary dialogue ticking and should be treated as suspicious historical data, not automatically deleted.

### H6. Legacy hotspot archaeology

Bundled JSON only uses hotspot types `-111`, `-1`, `0`, `1`, and `20`, despite broader historical documentation/source semantics.

Search historical maps/campaigns for:

- types `2–10` following different Person indices;
- types `11–19` kill targets;
- types `21–40` dialogue launchers;
- malformed/creative values used by community maps;
- size-zero `-1` hotspots and whether they were intentionally disabled placeholders.

This will help define a compatibility validator for imported maps.

### H7. Campaign branching test/history

`CampaignLevel` supports multiple `nextlevel` choices, but all bundled campaigns are linear and the current world-map renderer visually connects levels sequentially rather than drawing arbitrary graph edges.

Find or construct a minimal branching campaign to determine:

- whether selection/progress/account storage still handles multiple choices correctly;
- whether world-map highlighting and connection lines represent branches correctly;
- whether old community campaigns ever exercised branching;
- whether historical documentation's intended branching was ever completed.

### H8. Map objective mode history

`mapkillsomeone` and `mapkillmost` remain defined but are unused by the bundled corpus; almost every map is `mapkilleveryone`, with four `mapgosomewhere` maps relying on exit hotspots.

Trace older releases/mod docs for maps or code paths that implemented modes 2/3. Determine whether these are abandoned objective types, incomplete features, or conversion casualties.

### H9. Challenge/campaign timing history

`setCampaignFinalTime()` exists but has no caller in the current source; campaign `time` currently stores the most recent level time rather than an accumulated run time.

Use git history and older binaries/docs to determine whether full-campaign timing ever worked. Do not build leaderboard semantics on the dormant field until this is resolved.

### H10. Weapon-fire provenance

`Weapon::onfire` has rendering/state support but no live assignment path was found outside initialization.

Search older source, mods, and history to determine whether burning weapons were once functional or merely planned. Keep any future flaming-weapon feature labeled Expanded unless historical evidence proves restoration.

### H11. Tutorial compatibility fixture

Because `Tutorial::active` alters damage, scoring, AI anger/perception, effects, rendering, and combat-stage behavior across many files, build a stage-by-stage automated/manual checklist before extracting tutorial seams.

At minimum verify:

- damage suppression;
- `cananger` gating;
- attack/counter stages;
- weapon stages;
- tutorial NPC rendering;
- suppressed blood/impact/speech effects;
- stage transitions and timer behavior;
- final exit back to menus.

### H12. Regression fixture pack for historical quirks

Create tiny maps/campaigns that isolate:

- all-dead completion;
- `-1` exit hotspot completion;
- kill-target hotspot completion;
- dialogue-completion win;
- type-20 and type-50 hostility transitions;
- type-41 immediate/special dialogue;
- participant teleport/no-restore behavior;
- patrol pause/loop behavior;
- pathfind return after search;
- corpse/body discovery;
- wolf blood/corpse smell;
- stealth loading;
- hotspot effective radius;
- tutorial active behavior.

These should become Classic compatibility tests before large schema/refactor work.
