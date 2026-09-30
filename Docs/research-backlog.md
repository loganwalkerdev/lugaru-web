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

### A2. B-13 character restoration — resolved (2026-09-27)

**VERIFIED SOURCE FINDING / RUNTIME-VERIFIED:** historical bounded yaw, bounded body tilt, and draw-order-safe historical head tracking are finalized as default behavior. Native OSS 1.2/current OSS comparisons and isolated Web A/Bs led to successful combined normal gameplay QA. Camera-relative movement intent works correctly. No verified remaining player-locomotion regression currently warrants another code change. See [B-13](porting-notes.md#b-13--player-direction-changes-snap-instead-of-rotating); manual code/docs commits are pending.

### A2a. NPC slope/seam and scheduler comparison — OPEN

**Priority / status:** low-priority open observation; active investigation closed after physical A/B testing at approximately 60 Hz. **RUNTIME-VERIFIED — user observation:** no clear or substantial overall difference was reliably distinguishable. OSS-style scheduling appeared possibly slightly smoother over NPC slope/seam traversal, but no reproducible gameplay failure or meaningful regression was established.

**VERIFIED SOURCE FINDING:** contact-corrected positions can feed subsequent AI steering, and several operations are subdivision-sensitive. This supports a possible interaction, not a verified cause. **Decision:** retain the fixed `1 ms` scheduler and unchanged NPC gameplay; no scheduler or local gameplay fix is justified. Preserve the external comparison patch and evidence in [B-13](porting-notes.md#scheduler-investigation-closed-low-priority-open-observation). Reopen only if a concrete reproduction emerges. Completed player restoration remains closed; camera follow remains a separate subsystem.



### A3. B-6 Web motion blur — resolved

**RUNTIME-VERIFIED — user physical A/B:** experimental enablement produced malformed slow-motion rendering, including an apparent resolution/viewport change and washed-out gray/white compositing. **Decision:** retain Web force-disable; close renderer investigation because exact historical restoration is disproportionate to this milestone. No modern replacement was introduced and native blur remains implemented.

**VERIFIED SOURCE FINDING:** the Web Options row now says `Blur Effects: Unavailable on Web`, and the shared dispatcher cannot toggle it. Native Options behavior is unchanged. This resolves the misleading control without claiming that WebGL fundamentally cannot implement blur. See [B-6](porting-notes.md#b-6--motion-blur-option-is-exposed-but-unsupported-on-web) for evidence and validation limits.

### A4. Web key rebinding — fixed (2026-09-24)

**VERIFIED SOURCE FINDING / RUNTIME-VERIFIED:** B-7 was caused by `Menu::setKeySelected()` attempting `SDL_CreateThread()` with the Web build's non-threaded SDL backend. Failure cleared `waiting` but left `keyselect` set. Commit `a7bfd51` adds Web-only nonblocking capture; Chrome testing confirmed keyboard/mouse rebinding, Escape cancellation, subsequent selection, gameplay use, and save/reload persistence.

### A4a. Native rebinding thread affinity / portability

**OPEN QUESTION:** The native `setKeySelected_thread()` still calls `SDL_WaitEvent()` from a worker, although SDL documents that this function must run on the thread that initialized video. It also mutates shared menu state. Native behavior was intentionally left unchanged by B-7. Determine the practical impact across supported native platforms before proposing a separate fix; no native runtime failure has been established by this investigation.

### A4b. B-5 asset filename case fallback — closed; compatibility note

**Priority:** Low. **VERIFIED SOURCE FINDING:** the tracked-reference audit found zero confirmed case-only mismatches; the inspected Web package preserves all tracked filenames/case exactly. The Emscripten-only `openMandatoryFile()` fallback repairs basenames only, leaving parent-directory casing and other loaders uncovered. Current OSS offers no broader handling.

**Decision:** latent compatibility risk for external/custom/historical content, not a current runtime bug. No shipped-content failure was reproduced and no code/data change is justified. Reopen only if a concrete custom/historical-content failure is reproduced. [Porting notes B-5](porting-notes.md#b-5--asset-filename-case-fallback-is-incomplete) record coverage, isolated harness validation, and audit limits.

### A5. Native build health

Resolve/understand the current native Linux SDL2 include-path/build issue so behavior can be compared against browser builds more easily.

### A6. Controller support: completed scope and remaining work

**VERIFIED SOURCE FINDING / RUNTIME-VERIFIED — completed physical scope:** foundation `e5939fe`, digital movement `4b8af1d`, core gameplay actions `2387ebf`, camera look `ab46b53`, and pause/menu UI `8d549e4` are implemented, Web-built, and verified with wired DualSense in Firefox (DS4Windows disabled). Physical checks cover discovery/lifecycle, movement/deadzones, five action mappings, attack-driven dialogue, look, pause/resume, virtual cursor and fine D-pad movement, confirm/contextual Back, campaign/world-map and challenge/menu flows, and simultaneous keyboard/mouse use. Isolated SDL/input/timing harnesses supplement these checks; they do not establish full native gameplay or physical multi-refresh-rate coverage. See [B-8](porting-notes.md#b-8--controller-gameplay-menuui-and-tutorial-support).

**VERIFIED SOURCE FINDING / automated validation:** Triangle tutorial advance (`bd7b207`) and stage-3 controller-look recognition (`f00ec72`) are implemented and passed Web builds and focused harness/regression checks. **Physical QA is pending for both.** The menu investigation is complete: the implementation reuses existing pointer/click semantics, not discrete focus or a separate campaign UI.

**OPEN QUESTION — validation and compatibility:**

- Physically verify Triangle tap/hold, stage 51, transition suppression, and tutorial stage 3 with horizontal/vertical look and centered noise.
- Raw Bluetooth DualSense verification; consistent exposure was not established in the final foundation test.
- Browser/controller matrix: earlier Chrome tests exposed no usable gamepads, but the user verified wired DualSense input in current Chrome with the final restored build. Cause unknown; no controller/browser-input change explains it. Firefox/wired DualSense remains most extensively tested; other Chromium versions, operating systems, controllers, and Bluetooth are not established by this result. No WebHID or Chrome-specific workaround exists.

**OPEN QUESTION — polish and feel:**

- Camera sensitivity: `120°/s` yaw and `90°/s` pitch work but felt slow for preference. Controller sensitivity/inversion settings UI and separate inversion are not implemented.
- Left-stick activation `10000` / release `7000` hysteresis works correctly; extended playtesting may motivate feel tuning. This is distinct from changing Classic locomotion.
- No controller glyph/prompt system; tutorial text still presents keyboard-oriented instructions.

**Intentional scope exclusions / future design:**

- Controller rebinding is absent; account/profile text entry still needs a keyboard. Existing cancellation does not provide controller-only text entry.
- True analog player movement/arbitrary facing: current discrete/approximately eight-direction movement is expected Classic behavior, not a controller defect. Investigate animation, facing, combat, and timing implications separately.
- **VERIFIED SOURCE FINDING / OPEN:** NPC steering generally produces continuous headings and shares the character executor. Character restoration is complete; residual NPC slope/seam behavior is tracked separately in A2a, not as a controller-input defect.
- Multiple-controller/player ownership, rumble/advanced DualSense features, trigger mappings, and controller stereo decrement remain unimplemented. Stereo decrement is mouse-only; no WebHID fallback exists.

**Next priority:** complete physical tutorial QA, then use extended playtesting to prioritize compatibility and tuning; do not treat every scope exclusion as a bug or a scheduled feature.

### A7. B-14 Web NPOT mipmapped textures — resolved

**VERIFIED SOURCE FINDING / RUNTIME-VERIFIED:** `eea54f4` adds a shared Web-only NPOT fallback while preserving native and Web POT behavior. The owner verified correct SnowFlake rendering with its original mipmap-requesting load and correct `Fire.jpg` appearance. No asset was changed. The active defect is closed; the exact failing runtime GL4ES state remains unproven, not a blocker requiring speculative work. [B-14](porting-notes.md#b-14--web-npot-textures-render-black-with-mipmapped-sampling) owns the policy, audit and evidence limits.

### A8. Dense fire-spawner Web slowdown — resolved for R1 (2026-09-28)

**RUNTIME-VERIFIED / VERIFIED SOURCE FINDING:** Seven Tasks `Data/Maps/task3.json` ("Desert Village - evening") has six historical fire objects. An exact temporary copy with only those objects removed ran smoothly, establishing the fire-generated sprite population as sufficient to trigger the Web slowdown. The historical fire spawn behavior was not a new Web regression. Per-sprite matrix and immediate-mode rendering through Web/GL4ES made the large flame and secondary smoke population expensive on the CPU.

**Decision / RUNTIME-VERIFIED:** `592e158` uses CPU billboards and client arrays on Web; native retains historical immediate mode. Normal production-build QA accepted the result, closing this shipped-content R1 blocker. Particle behavior and visual fidelity were preserved. The temporary `Data/Maps/task3_nofire.json` A/B control remains untracked test equipment. [Porting notes B-15](porting-notes.md#b-15--dense-fire-sprite-rendering-slows-web) record the experiment, approximate measurements, implementation, and regression checks. This issue is separate from the B-14 NPOT texture fix.

### A9. Empire Sky Palace campaign progression — resolved for R1 (2026-09-29)

**RUNTIME-VERIFIED / VERIFIED SOURCE FINDING:** on `jendraz1`, entering the bridge type `-1` exit hotspot recorded the campaign win immediately, but routing waited ~0.7 s and then re-checked live `winhotspot` occupancy. A fast player who left the trigger stalled the transition; a subsequent death reloaded `campaignlevels[getCampaignChoicesMade()]` (a choice count, not a node) into `jendraz2` with `actuallevel` still `0`, producing duplicate Throne Room completions and skipping `jendraz3`. Staying inside the trigger progressed correctly, so the hotspot data was not at fault. The susceptibility is shared/historical upstream logic, not a WASM-specific or established Web-introduced regression.

**Decision / RUNTIME-VERIFIED:** `97121db` latches `acceptedSpatialWin`, reloads ordinary campaign deaths from `actuallevel`, and resolves a death after a recorded win exactly once through existing routing. Fast-exit, stop-inside, Throne Room, death-before-win, and main-campaign Village QA passed; the post-win/pre-route death window is source-verified only. Empire's draw/fade pop-in remains HISTORICAL original behavior and is unrelated. [Porting notes B-16](porting-notes.md#b-16--delayed-spatial-exit-loses-campaign-progression-empire-sky-palace) hold the mechanism, fix, review, and residual risk.

**Follow-up runtime polish (2026-09-30):** the authored Sky Palace exit hotspot at `[652.0852, 160.3987, 888.6204]` is type `-1`, whose activation condition compares squared player distance against the hotspot `size`. Its original size `15.0` produced only a ~3.87-unit effective radius and felt unnecessarily precise at the bridge exit. Runtime testing settled on `100.0` (10-unit effective radius), which allows natural approaches to trigger the exit without extending excessively far back across the bridge. This is a map-local usability adjustment and does not change the B-16 campaign-progression logic.

### A9a. Throne Room Emperor dialogue activation area — resolved for R1 (2026-09-29)

**RUNTIME-VERIFIED / VERIFIED SOURCE FINDING:** on `jendraz2`, Emperor dialogue 1 is type `32`, so its normal special-dialogue activation depends on `distsq < 6` to participant 2. Because the Emperor stands elevated on the raised platform, the effective floor-level activation area is unusually small and can require the player to climb onto or near the dais before the dialogue begins. A type `21` hotspot correctly launches dialogue index 1 through the existing hotspot dialogue path and preserves its one-shot `gonethrough` behavior.

**Decision / RUNTIME-VERIFIED:** `2ae4c5e` introduced a map-local type `21` hotspot at Ash's authored dialogue staging position `[748.7458, 156.4227, 986.3483]`. Follow-up runtime tuning sets its size to `6.25` (2.5-unit effective radius). Decimus still activates first, the Emperor dialogue now triggers naturally from the floor, does not repeat, and campaign progression continues normally to the Rebel Outpost. No global dialogue-distance logic was changed; this is treated as localized polish for inherited map behavior rather than a Web-specific regression.

### A9b. Empire Sky Palace NPC clothing corruption — resolved for R1 (2026-09-30)

**RUNTIME-VERIFIED / VERIFIED SOURCE FINDING:** two NPCs in `jendraz1` rendered with a dark/red full-body texture instead of their intended clothing. The untouched 2008 Empire map contains two `:Data:Textures:fire.png` clothing references. The verified Lugaru v1.05 installer from the same era contains `Data/Textures/Fire.jpg` but no `fire.png`, and the Empire archive does not provide that missing texture. OSS later mapped `:Data:Textures:fire.png` to `Textures/Fire.jpg` during hardcoded-path conversion.

`Person::addClothes()` treats a three-channel RGB image as fully opaque when compositing clothing. The substituted JPEG therefore overwrote the underlying rabbit skin instead of behaving like a transparent clothing layer.

**Decision / RUNTIME-VERIFIED:** temporarily restoring the two paths to the historically missing `Textures/fire.png` immediately restored the normal NPC appearance, confirming the substitution as the cause. `6142c73` removes only those two dead `Fire.jpg` clothing layers from `jendraz1.json`; `bf94eca` removes the corresponding hardcoded-path rewrite so future conversions cannot recreate the regression. The final map renders correctly without relying on a missing asset. This is an inherited OSS content-conversion bug, not a Web-specific rendering defect. See [B-17](porting-notes.md#b-17--empire-sky-palace-npcs-receive-firejpg-as-clothing).

### A9c. Empire Sky Palace dense foliage Web slowdown — resolved for R1 (2026-09-30)

**RUNTIME-VERIFIED:** the Web build suffered substantial slowdown inside the dense tree cluster around the opening area of `jendraz1`, particularly while jumping or rotating the camera through overlapping foliage. The comparable native OSS build did not show the same severe slowdown, and areas of Sky Palace without dense trees remained smooth.

Controlled A/B testing isolated the cost to rasterizing the full overlapping leaf geometry rather than simply processing many tree objects. Removing leaves eliminated the slowdown; reducing leaf count by roughly half did not materially improve it; rendering the full leaf vertex count as points remained smooth; and disabling the translucent foliage second pass did not eliminate the defect. Front-to-back normal-pass leaf ordering produced a major improvement, while removing legacy alpha testing from that sorted path made the affected area smooth.

**VERIFIED SOURCE FINDING:** GL4ES fixed-pipeline emulation translates legacy alpha testing into fragment-shader discard logic. Its fixed-pipeline state path also tracked `GL_SAMPLE_ALPHA_TO_COVERAGE` without forwarding that enable to the underlying WebGL context. Direct WebGL alpha-to-coverage with a multisampled context restored correct leaf cutouts while retaining the performance improvement.

**Decision / RUNTIME-VERIFIED:** `df72be9` keeps all original foliage content while optimizing only the Web path. Web builds request 4x MSAA, render normal-pass tree leaves front-to-back, and use alpha-to-coverage instead of GL4ES legacy alpha testing. A small patch to the pinned GL4ES dependency forwards alpha-to-coverage to WebGL and is applied automatically by CMake. A completely fresh Emscripten 6.0.9 build reproduced the patch, correct foliage cutouts, and smooth Sky Palace performance with no obvious regressions. Native rendering behavior remains unchanged. See [B-18](porting-notes.md#b-18--dense-web-foliage-causes-severe-slowdown).

A follow-up CMake helper also made GL4ES patch application idempotent, and both repeat-build and fresh-build paths were verified successfully.

### A10. Jump mechanics regression under fixed timestep — resolved for R1 (2026-09-30)

**SOURCE-VERIFIED / RUNTIME-VERIFIED:** Lugaru `1.2`, current upstream, and Loupgarenne immediately before `1db2761` apply jump boost at `multiplier * 6`. Loupgarenne commit `1db2761` introduced `std::max(multiplier * 6.0f, 0.1f)` alongside the fixed 1 ms simulation timestep. At that timestep the floor applies stored jump power about 16.7 times faster than the original time-scaled rate, producing higher jumps and greatly reducing tap-versus-hold height variation.

A one-line A/B restoring `multiplier * 6.0f` while leaving the fixed scheduler unchanged restored the physically observed upstream OSS jump behavior. This confirms the `0.1f` floor as the regression rather than the scheduler itself. B-19 records the fix and validation.

Historical 1.05 binary parity remains unverified. Other multiplier-sensitive changes introduced by `1db2761`, including ragdoll constraint calculations, remain separate follow-up research.

### A11. Clothing, skins, and historical asset audit — active (2026-09-30)

A repository-wide audit covered bundled campaign clothing references, skin and fur assets, `Person::addClothes()`, devtool clothing commands, path-conversion rules, historical/easter-egg skins, and lineage from the original source through upstream, Loupgarenne, the WASM baseline, and lugaru-web.

**RESOLVED / B-20:** three Temple characters used `Fur.jpg` with non-unity tint, exposing the compositor's three-byte JPEG / four-byte tint-index mismatch and producing runtime-verified RGB striping. Commit `85477f5` adds a pixel-equivalent opaque RGBA `Fur.png` and redirects only those three affected references. Unity-tinted JPEG fur remains unchanged.

**VERIFIED:** the Loupgarenne/WASM lineage introduced no additional clothing regression. Empire clothing is correct after B-17, and intentional RGB full-body skins such as the bundled bloodied-character textures remain valid.

**OPEN — devtool safety:** `addClothes()` has no destination-size validation. The `clothes` command can therefore feed oversized or otherwise incorrectly sized PNG textures into the fixed 512×512 skin buffer. This is a separate hardening issue and should be tested with native sanitizers rather than intentionally triggered in the normal Web build.

**OPEN — preservation archaeology:** upstream made several Temple and Seven Tasks clothing/content edits during the 2016–17 asset-licensing work, including outfit removal, re-dressing, and tint changes. These changes are already present in Lugaru 1.2 and predate the Web port, so they should be evaluated as historical-preservation questions rather than current port regressions.

**SPECIAL / HISTORICAL:** `darko`, `lizardwolf`, and the orphaned `FurDavid` skin remain useful archaeology targets. `FurWolfLizard` uses the wolf texture layout; `FurDarko` and `FurDavid` use rabbit-layout skins. Their presence does not by itself establish a defect.

Further work should keep content-preservation decisions, devtool safety, and actual bundled-game regressions separate.

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

**VERIFIED SOURCE FINDING:** `Account::getCampaignChoicesMade()` is a choice count, not a node ID. B-16 (`97121db`) removed its use as a node index in the campaign death reload, but campaign menu entry (`Source/Menu/Menu.cpp`) still computes `actuallevel` from `campaignlevels[getCampaignChoicesMade() - 1].nextlevel[whichchoice]`. That relies on the linear-prefix assumption and is unproven for branching campaigns; a branching fixture should exercise it.

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
