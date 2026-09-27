# Lugaru-Web Research Session — 2026-09-18

## Purpose

This document captures the major technical, historical, preservation, and design conclusions from the 2026-09-18 Lugaru-Web research/design session. It is intentionally broad and somewhat redundant. Stable conclusions should migrate into the focused documentation files; this document exists so useful threads are not lost.

---

# Core realization

Lugaru-Web has become a much clearer project than "get Lugaru running in a browser."

The project now has three mutually reinforcing goals:

1. **Preservation** — keep Original Lugaru and important community history accessible and accurately attributed.
2. **Modernization** — make the game behave like a polished browser-native application without altering historical gameplay by accident.
3. **Continuation** — use the original game's systemic combat/physics/stealth foundation for new modes and mechanics, especially a lightweight multiplayer Arena.

These do not require separate engine forks.

The strongest architectural principle to emerge today is:

> One codebase, one WASM build, multiple content packages, multiple gameplay rulesets, and separable presentation layers.

---

# A. Historical/technical lineage

## Original Lugaru

- David Rosen created the original game and its unusual combat/physics/systemic identity.
- Original toolchain: CodeWarrior/OpenGL/FMOD/QuickTime on Mac; later Windows and Linux ports came from other contributors.
- Modern source is not a frozen copy of Rosen's exact release.
- A large portion of legacy gameplay thinking remains concentrated in `Person.cpp` and `GameTick.cpp`.
- Do not attribute individual weird comments or behaviors to Rosen without source history.

## OSS Lugaru

- Modernized build/platform support and organization.
- SDL2/CMake/platform work.
- Multiple campaigns and preservation work.
- JSON map loading.
- Historical mod relicensing and asset cleanup.
- Source tree gained clearer domain directories while gameplay core remained coupled.

## Ryan Deering / Loupgarenne

- Focused on unlocked framerate and frame-rate/timestep behavior, plus UI/build/refactor work.
- Created a real, committed but unmerged WebAssembly branch.
- Browser commit: `b340204635e86a4385ef88dc45e2696e2213d9ab` (`wasm support`).
- Browser work used Emscripten + GL4ES + browser-specific platform glue + packaged assets + persistence.
- Lugaru-Web correctly starts from Ryan's work rather than claiming the first WASM port.

## Lugaru-Web

- Preserved Ryan's browser commit as `baseline/wasm-b340204`.
- Validated a playable browser baseline.
- Began turning the experimental port into a reliable product.
- Already fixed browser settings behavior, main-loop timing, persistence architecture, animation bounds safety, and a WASM memory bug.

## Redagito/Lugaru

- Useful comparative refactor/reference repository.
- More aggressive modularization and globals cleanup.
- Still retains large `Person`/`GameTick` concentrations and substantial global coupling.
- Lesson: directory cleanup is useful but does not by itself solve gameplay architecture.
- Treat it as reference material rather than a merge base.

---

# B. Browser engineering status remembered today

## Known-good build approach

The working browser build uses normal Emscripten CMake invocation rather than relying on Ryan's custom toolchain path:

```bash
rm -rf build-web
emcmake cmake -S . -B build-web \
  -DCMAKE_BUILD_TYPE=Release \
  -DUSE_CDN_DATA=OFF
cmake --build build-web -j$(nproc)
cd build-web/bin
python3 -m http.server 8000
```

Open `http://localhost:8000/lugaru.html`.

A CMake/Emscripten shared-library warning remains a baseline/toolchain-version concern rather than a blocker.

## Browser issue state discussed

- B-1 Web settings overridden after configuration load — fixed.
- B-2 main-loop timing / timeout scheduling — fixed.
- B-3 localStorage persistence replaced with IDBFS — fixed.
- B-4 animation bounds checking restored — fixed.
- B-5 case-insensitive asset fallback incomplete — open.
- B-6 motion blur option exposed but unsupported/forced off on Web — investigate.
- B-7 key rebinding was suspected to fail under Web due thread usage — fixed and Chrome-verified on 2026-09-24; see section V below.
- B-8 no controller support — open at this session; subsequently implemented through foundation (section W), gameplay/look (section X), and menu/UI plus tutorial follow-ups (section Y). Physical tutorial QA remains pending; menu/UI physical QA passed.
- B-9 size-optimized build flags may be worth benchmarking against speed optimization.
- B-10 no ASYNCIFY, so synchronous level loads may block the tab.
- B-11 `Account::active()` auto-creates `Player` globally — investigate/history compare.
- B-12 Sprite delete/erase WASM out-of-bounds — fixed.
- B-13 player direction snap/high-refresh regression — open at this session; subsequently resolved by the 2026-09-27 character-restoration milestone (section Z).
- CI macOS dependency setup issue — low priority.

Other browser/runtime observations:

- pointer-lock errors can be noisy;
- Chrome audio autoplay produces expected AudioContext warnings;
- resolution iterator has a latent invalid-current-resolution risk;
- native Linux SDL2 include/build health should be fixed to improve comparison testing.

---

# C. Lugaru 2 / Overgrowth research

## Direct lineage

Wolfire announced Overgrowth in 2008 as **"the new Lugaru 2."**

This makes old Lugaru 2 material legitimate design archaeology for understanding what Wolfire wanted to evolve from the first game.

## Phoenix / movement

David Rosen's June 2006 Lugaru 2 movement post described:

- walking;
- running;
- crouching;
- sneaking;
- rolling;
- jumping;
- seamless transitions;
- animation derived from surface/physics information and character state/personality.

The animation system had recently been rewritten at that time.

Relevance:

- movement quality and transition correctness mattered early;
- current facing/rotation behavior should be understood, not papered over;
- the lineage values responsive animation tied to simulation.

## Physics

Rosen later explained that original Lugaru ragdolls were particle-and-stick systems and explicitly described their gameplay role: manipulating fights so different bodies hit soft or hard surfaces.

Overgrowth explored richer physics because the old system had limitations.

Relevance:

- Lugaru physics are not merely dated visuals;
- do not replace them casually;
- compose new mechanics with them where possible.

## Species

Early Lugaru 2 art explored foxes, tigers, panthers. Aubrey Serr later joked that these were "eaten by the Whaleman." Early Overgrowth planning instead listed rabbits, wolves, cats, rats, and dogs.

Relevance:

- broader species have authentic historical lineage;
- this does not mean cut species should be treated as canon;
- a future fox/cat/rat/etc. cosmetic model is thematically defensible;
- start species as identity before introducing class mechanics.

## Mission structure

Wolfire explicitly contrasted Lugaru's kill-everyone structure with possible Overgrowth missions:

- reconnaissance;
- stealth;
- escape;
- race;
- assassination;
- survival.

They also emphasized freedom of approach inside levels and avoiding premature rigid design commitments.

Relevance:

- Expanded Lugaru can gain mission variety without becoming heavily scripted;
- objective layers can sit on top of existing systems;
- implement/playtest/cut is preferable to giant speculative systems.

## Ruins

Aubrey Serr's writing about Lugaru ruins emphasized their abstract/mysterious quality and their gameplay role.

Relevance to Arena/new maps:

- geometry should serve movement/combat first;
- sightlines, height, jump routes, hard surfaces, cover, edges, and readability matter more than decorative realism.

## Multiplayer

Early Overgrowth material discussed cooperative multiplayer. Later Overgrowth had local multiplayer and eventually an online multiplayer prototype.

Relevance:

- multiplayer fits the lineage;
- the current Lugaru code still needs substantial architectural work first;
- local multi-human control should precede networking.

## Mature Overgrowth input as a reference

Later Overgrowth supported:

- controller menu navigation;
- per-controller rebinding;
- sensitivity/deadzone control;
- mouse-button rebinding.

This is a useful user-facing target for Lugaru-Web's input modernization.

---

# D. Product/ruleset direction

## Lugaru-Web as umbrella product

Lugaru-Web should contain the preserved game while also becoming the name of the larger browser-native continuation.

Potential elevator pitch:

> Lugaru in your browser. The complete original game, preserved and modernized, with an entirely new drop-in multiplayer experience built around its combat.

## No engine fork

Do not split development into separate `classic-engine` and `expanded-engine` worlds.

Use one engine with content/rules capabilities.

## Conceptual rulesets

- `Classic`
- `Expanded`
- `Arena`

Exact implementation is intentionally undecided until the architecture map is further along.

Avoid hundreds of scattered `if (classicMode)` branches.

## Historical defaults

Original, Temple, Empire, Seven Tasks, and historical community content should default to Classic rules.

Users may later choose Expanded replay for historical content, with a warning that it changes intended gameplay/balance.

## Universal vs. gameplay-changing changes

Universal:

- bug fixes;
- controller;
- rebinding;
- analog input;
- deadzones;
- high-refresh fixes;
- browser persistence;
- fullscreen/widescreen;
- accessibility.

Ruleset-controlled:

- corpse dragging;
- new movement verbs;
- new combat mechanics;
- new AI communication;
- fundamentally new weapon behavior.

Decision test:

> Does this materially change the decisions and possibilities available to someone playing historical content?

---

# E. Corpse dragging and systemic mechanics

Corpse dragging became a concrete example of the desired design philosophy.

## Why it fits

- dead/ragdolled characters already remain physical Persons/Skeletons;
- corpses already interact with perception, including smell/blood logic;
- moving a body therefore naturally changes stealth state;
- no scripted "hide the body" zone is necessary.

## Likely implementation shape

- enter drag interaction;
- select nearby eligible ragdoll;
- constrain suitable joints/body points near player;
- keep the rest of ragdoll physical;
- release on command/interruption.

## Rules

- off by default in Classic;
- available in Expanded;
- optional Expanded replay can make old maps behave differently by design.

## Related systemic expansion ideas

- body discovery;
- AI communication/shouts;
- investigate last-heard position;
- noise makers/thrown objects;
- dead vs. unconscious distinction;
- fire/environmental propagation later where technically appropriate.

The repeated principle is that one mechanic should feed several existing systems.

---

# F. AI/stealth observations

Existing stealth is richer than it appears on the surface.

Observed/recalled channels include:

- environmental sound radius/volume;
- rabbit hearing multiplier;
- distance;
- stance/animation height affecting detection;
- line of sight / occlusion;
- wolf smell;
- bloody corpse smell/stimuli.

This means Expanded stealth should deepen the existing model rather than replace it.

---

# G. Input and multiplayer

The tiny SDL/raw input path is a high-value refactor target because it blocks several future features at once.

## Semantic actions

Potential action layer:

```text
Move
Look
Jump
Crouch
Attack
Throw
DrawWeapon
Interact
Pause
```

Benefits:

- key rebinding;
- controller support;
- analog movement;
- deadzones/sensitivity;
- menu navigation;
- local multiplayer;
- network input;
- replay capture.

## Multiplayer order

1. action input;
2. assign inputs to Persons;
3. remove `players[0]` assumptions;
4. two local humans;
5. camera/UI ownership;
6. networking;
7. lobby/room flow.

Host-authoritative networking is the current conceptual preference.

---

# H. Arena concept

The most compelling continuation mode remains a STRAFTAT-like low-friction party/Arena game using Lugaru mechanics.

Desired qualities:

- browser-native;
- 1v1 first, then 2–4;
- compact maps;
- short rounds;
- fast rematches;
- random/rotating weapons/maps;
- simple room codes/lobbies;
- generous cosmetic customization;
- no monetization-driven scarcity necessary.

The inspiration is the ease of getting into a match and emergent replayability, not copying another game's actual systems.

---

# I. Cosmetics/species

Current engine already has clothing and RGB tint behavior that can seed a proper customizer.

Potential customization:

- fur;
- clothes;
- colors;
- masks;
- hats;
- species/model;
- player name/name color.

`PersonType` already abstracts some rabbit/wolf data but many hardcoded checks remain.

Start with visually different species on compatible mechanics/skeletons where feasible. Avoid turning every animal into a class before the architecture is ready.

---

# J. UI/UX and presentation

Proposed modern front page:

```text
LUGARU
CAMPAIGNS
CHALLENGES
ARENA
CUSTOMIZE
CREATE
OPTIONS
```

Visual direction:

- gothic/cathedral/horror-adjacent;
- stone/ink/scratch texture language;
- hostile animal imagery;
- low-res/pixel-ish counters where it suits the game;
- modern navigation beneath the aesthetic;
- avoid generic web-app/SaaS appearance.

Campaign cards can expose authorship and dates directly.

Illustrated card art is preferred long-term over generic screenshots; fantasy loading-screen illustration is a useful mood reference.

## Classic presentation

Classic UI/presentation and Classic gameplay should remain separable.

One possible model is a modern campaign browser where selecting Original can enter the preserved old title/menu/tutorial/challenges experience.

Do not decide the final UX before the menu/campaign architecture is mapped.

---

# K. Tutorial

The current tutorial has a recognizable stage/state subsystem but awareness leaks throughout gameplay code, especially `Person.cpp`.

Historical tutorial should stay intact under Classic.

Expanded mechanics should use a separate Lugaru-Web Training mode if necessary.

Future tutorial checks should ideally consume semantic gameplay events rather than adding more hardcoded stage branches into combat code.

---

# L. Maps/editor/Create

Current map loading is structured JSON and includes objects, hotspots, players, path points, environment, dialogue, etc.

Lugaru historically already has an in-game map editor.

Therefore:

- do not begin by building a new editor from scratch;
- first expose/stabilize the existing editor in Web;
- verify browser-local save/load;
- make a tiny custom map experiment;
- later build friendlier CREATE UX around what actually works.

Potential future flow:

```text
CREATE
→ New Arena
→ place props/spawns/weapons
→ save
→ playtest
→ export/share
```

---

# M. Campaign/challenge/leaderboard ideas

Historical campaigns should become first-class campaign cards rather than hidden files.

Challenges can eventually separate:

- Original;
- Historical Community;
- Lugaru-Web.

Leaderboards are technically feasible but global boards require a backend and cannot trust the open-source browser client.

Potential progression:

- local PBs;
- community/unverified global boards;
- run metadata by ruleset/build;
- replay/input evidence later.

Ruleset must be part of leaderboard identity.

## Replays

Potentially valuable for:

- verification;
- WR playback;
- ghosts;
- Arena;
- bug reports;
- regressions.

Deterministic input-only replay may not work with old timing/physics. Snapshot-assisted replay may be needed.

---

# N. Community archaeology: what survives beyond OSS

Research today established that the historical Lugaru community was significantly larger than the three bundled campaigns.

## Known map/content leads

- eyeCube MapPack;
- Levbot Map Pack 1;
- Levbot Map Pack 2;
- rudel_ic's nine-map pack: Arena, House, Hunt, Trap, Bigtower, Tower, Bridge, Coward, Stairs;
- BunnyWithStick maps: Forest Silly Forest, Winter Wanderers, Hill Grove, Desert Wanderers, Cave;
- Philtron R.'s 14 standalone Challenge maps;
- at least two additional map-pack forum threads explicitly preserved as leads by OSS.

## Surviving campaigns outside bundled OSS

- Ancestral Tales;
- Reluctant Assassin;
- Tannim's Mini Campaign.

## Historical mod categories

- skins;
- clothing;
- high-res textures;
- alternate music;
- character/model changes;
- skeleton work;
- weapon packs;
- Blender converters/plugins;
- map repair tools;
- installers/downloaders.

## Historical sites/databases

- Wolfire forums;
- Wolfire Wiki;
- old Wolfire Lugaru page;
- ModDB;
- Wayback;
- OSS GitLab issues;
- personal sites;
- Lugaru Downloader/mod catalog.

---

# O. Licensing conclusions

## Key distinction

**Free to download is not the same as free to redistribute.**

An open-source/open-asset base game also does not automatically open-license every user-created map.

A map author may own the original arrangement/content they added even if every ingredient came from stock Lugaru.

## Clear historical content

OSS `CONTENT-LICENSE.txt` makes the current bundled rights unusually clear:

- original Wolfire/Lugaru HD assets: CC BY-SA 3.0;
- four OSS clothing textures: CC BY-SA 4.0;
- Temple contribution: CC BY-SA 3.0;
- Empire contribution: CC BY-SA 4.0;
- Seven Tasks contribution: CC BY-SA 4.0.

## Relicensing strategy

When an old creator is found, ask explicitly for CC BY-SA 4.0 covering named works/their original contribution.

Archive the reply and the scope of the permission.

Also ask whether custom assets were authored by them or taken from elsewhere.

## Unknown third-party assets

Even a correctly CC-licensed map can contain a texture/music/model the modder did not own.

The project should:

- inventory files;
- identify stock assets by hash;
- investigate unknown assets;
- classify unresolved provenance;
- replace problematic assets in the public restored edition;
- preserve the original artifact separately.

This is not theoretical: the OSS preservation issue records Temple becoming clean after non-free files were replaced, and Ancestral Tales raised exactly this kind of texture concern.

## Replacement philosophy

If a questionable asset is incidental (e.g. generic stone/wood texture), replacing it with a new clean equivalent can preserve map geometry/gameplay while fixing the rights chain.

AI-generated assets may be one practical source of replacement material, but provenance/terms for new assets should also be documented.

---

# P. Archive strategy

The project should "catch them all" historically without equating archival possession with public redistribution.

Potential categories:

```text
campaigns/
maps/
characters/
skins/
clothing/
models/
skeletons/
animations/
weapons/
textures/
audio/
tools/
documentation/
forum-archives/
provenance/
```

Every recovered item should eventually get metadata including:

- title;
- author/handle;
- date;
- source URL/thread;
- recovered source;
- exact filename/version;
- checksum;
- license;
- relicensing evidence;
- third-party asset status;
- contact attempts;
- compatibility;
- public redistribution status;
- restoration status.

Keep original archive copies unchanged. Any converted, fixed, cleaned, or JSON-modernized copy should be a separate derivative.

---

# Q. People/contact leads

## rudel_ic

Strongest identity lead.

Wolfire itself identifies `rudel_ic` as **Wolf Mathwig**, a German software developer who reverse-engineered Lugaru formats and built model/animation/skeleton tools.

This is both a licensing lead and an important technical-history lead.

## Lotus Wolf

Important because of:

- Reluctant Assassin;
- Weapon Shop;
- large forum presence;
- old site;
- contest/video breadcrumbs.

OSS reported difficulty reaching him.

## tannim

OSS was waiting for a public license statement. Historical contest/forum presence provides additional leads.

## BunnyWithStick

Known forum account and maps; identity/contact needs archaeology.

## Levbot / eyeCube

Weak public leads so far. Start from exact old URLs/profile IDs/files rather than broad username searches.

## Rémi Verschelde

Potentially valuable person to ask about preservation history because he coordinated the OSS mod-review/relicensing effort.

---

# R. Weapon/model/skin history

## Skins and clothing

Historical modding included fur/skin and clothing/decal replacement. Lugaru's own clothing tint/compositing architecture explains why this ecosystem existed.

This is directly relevant to a future non-destructive character customizer.

## Weapon Shop

A major thread/site existed around custom weapons, including a war hammer reference. Reconstructing the inventory is a high-priority archaeology task.

Historical versions likely replaced existing weapon slots/assets because arbitrary gameplay-code extensions were not originally available.

## Model/skeleton/animation tools

Wolf Mathwig's reverse-engineering work reconstructed enough binary formats to enable custom models, skeletons, and animations. His tool evolution (X2Solid → Brainfart → Blender Python tooling) should be preserved even if it no longer runs on modern Blender.

The tools themselves are part of Lugaru history.

---

# S. Preservation vs. restoration

A useful final vocabulary emerged today:

## Original archival package

Historical artifact kept unchanged for research/provenance.

## Restored Lugaru-Web edition

Publicly distributable package where:

- author rights are clear;
- stock assets are identified;
- questionable third-party assets are removed/replaced;
- technical conversions are documented;
- gameplay changes are minimized and disclosed.

## Historical gameplay mode

The original mechanics/rules expected by that content.

## Expanded replay

Optional modern mechanics applied to historical content with clear labeling.

These distinctions let the project preserve history without freezing development.

---

# T. Immediate next documentation/code steps

1. Keep this session capture intact.
2. Refine `architecture.md` while tracing exact source paths.
3. Build the historical/Overgrowth chronology with citations.
4. Create the community-content census/catalog.
5. Start with one archaeology target at a time: Philtron maps, Wolf Mathwig/rudel_ic tools/maps, then Lotus Wolf/Weapon Shop.
6. Use B-13 as the first deep architecture/provenance case study because it crosses timing, animation, player state, and branch lineage.
7. Defer large creative implementation until the browser foundation and mental map are stronger.

---

# Useful sources captured today

- https://www.wolfire.com/blog/2008/09/announcing-overgrowth/
- https://www.wolfire.com/blog/2006/06/lugaru-2-movement/
- https://www.wolfire.com/blog/2009/02/overgrowth-faq/
- https://www.wolfire.com/blog/2009/05/races-in-overgrowth/
- https://www.wolfire.com/blog/2009/02/the-ruins-of-lugaru/
- https://www.wolfire.com/blog/2009/05/creating-the-overgrowth-physics-engine/
- https://www.wolfire.com/blog/2009/07/wolfire-answers-community-questions/
- https://www.wolfire.com/blog/2018/05/What-s-new-at-Wolfire---Early-May/
- https://www.wolfire.com/blog/2021/12/Overgrowth-Online-Multiplayer-Prototype/
- https://www.wolfire.com/blog/2010/04/Reverse-Engineering-Binary-Files
- https://wiki.wolfire.com/Lugaru_Mods
- https://wiki.wolfire.com/Lugaru_FAQ
- https://wiki.wolfire.com/Map_Editing_in_Lugaru
- https://forums.wolfire.com/viewtopic.php?p=97502
- https://electriccartilage.wordpress.com/2017/06/30/postmortem-14-lugaru-challenge-maps-i-made/
- https://gitlab.com/osslugaru/lugaru/-/issues/31
- https://github.com/osslugaru/lugaru/blob/master/CONTENT-LICENSE.txt
- https://github.com/WolfireGames/lugaru
- https://github.com/redagito/Lugaru

---

# U. 2026-09-23 continuation — quirky-system archaeology

This continuation pass started from the September 18 dialogue/hotspot/campaign discoveries and deliberately asked what ordinary behaviors a player sees that are actually produced by unusual old source/data mechanisms.

The pass used the current Lugaru-Web source snapshot, all 137 bundled JSON maps, all four bundled campaign text files, and surviving Wolfire editing/player documentation. The findings below are code/data observations unless explicitly labeled historical documentation or interpretation.

## U1. The world map is a menu, not a level

`Source/Level/Campaign.cpp` parses campaign text into `CampaignLevel` records containing map filename, description, `ChooseNext`, `nextlevel[]`, and fixed X/Y coordinates.

`Source/Menu/Menu.cpp` menu case 5 draws a 400×400 image and overlays markers/lines. It tries `Textures/<campaign>/World.png` first, then `Textures/World.png`.

Campaign positions are old 512×512 image coordinates converted into menu coordinates. Historical Wolfire docs confirm `LocationX/Y` were authored as pixel locations on `Data/Textures/World.png`.

The campaign data structure supports a vector of next levels, but **none of the bundled campaigns actually branches**:

```text
main.txt       27 levels: 26 entries NumNext=1, final NumNext=0
temple.txt     31 levels: 30 entries NumNext=1, final NumNext=0
empire.txt     41 levels: 40 entries NumNext=1, final NumNext=0
seventasks.txt 18 levels: 17 entries NumNext=1, final NumNext=0
```

Historical docs explicitly say branching was intended but not completed. The old world-map renderer also largely assumes sequential campaign indices, so a modern branching campaign would need testing/new presentation rather than relying on latent support.

## U2. `ChooseNext = 2` is a real in-place state swap

Earlier interpretation was "skip fiery loading presentation." Source inspection shows much more.

On `ChooseNext = 2`, `stealthloading` is enabled. During `LoadJsonLevel()` the engine:

- keeps existing map objects/terrain registrations/decals/sprites;
- skips loading the destination JSON object list;
- preserves player 0 coordinates/yaw/targetyaw;
- rebuilds Persons and weapons from the destination map;
- replaces dialogue, hotspots, path data, environment and skybox state;
- skips object center/radius/shadow rebuilds.

So it behaves like a **scene-state replacement inside the current geometry**.

Only one bundled campaign entry uses it: `Lugaru1 → Lugaru2`. This is the source mechanism behind the opening village's unusually seamless continuity.

Important wrinkle: `Lugaru2.json` contains 140 objects just like `Lugaru1.json`, but they are not loaded during the real stealth transition. Directly loading `Lugaru2` can therefore produce slightly different object state than reaching it through the campaign.

## U3. Original story continuity is built from duplicated maps

Selected JSON comparisons:

```text
1→2   140→140 objects; 122 exact at same index; stealth load ignores map 2 objects
2→3   140→140; all exact
10→11 95→95; all exact; hostile 1→0; dialogue added
17→18 152→152; 143 exact; hostile 0→1; 6→2 Persons
19→20 299→299; all exact
21→22 204→176; 0 exact at same index; grass→snow; center/radius/skybox differ
23→24 163→163; 154 exact; skybox changes
25→26 299→299; 276 exact
26→27 299→299; all exact
```

Interpretation: Lugaru fakes a continuous journey through a mixture of:

1. one true stealth/in-place swap;
2. immediate loads of identical duplicate maps;
3. immediate loads of near-duplicate maps;
4. world-map returns between different physical scenes that can share similar labels.

This is much closer to authored "state snapshots" than a persistent open-world location model.

## U4. Objective system is mostly flags and magic-number triggers

`Game.hpp` still exposes old map types:

```text
0 mapkilleveryone
1 mapgosomewhere
2 mapkillsomeone
3 mapkillmost
```

In current runtime behavior:

- all-dead + map type 0 can win;
- type `-1` hotspot sets the go-somewhere win;
- hotspot types `11–19` mark specific Person indices that all must die;
- dialogue types `30–39` win on dialogue completion.

Across all 137 bundled maps:

```text
map type 0: 133
map type 1: 4
map type 2: 0
map type 3: 0
```

The four type-1 maps also contain `-1` win hotspots, so the serialized map type is not itself what implements spatial completion.

Player death is the ordinary lose condition and reloads the level. No generalized fail-condition scripting layer was found.

## U5. Full bundled hotspot census

Across all 137 bundled JSON maps:

```text
-111 : 41 occurrences / 41 maps
-1   : 20 occurrences / 11 maps
0    : 3 occurrences / 2 maps
1    : 1 occurrence / 1 map
20   : 14 occurrences / 14 maps
```

No other hotspot type is currently used in bundled JSON.

Current source semantics:

```text
0      static contextual text
1–10   contextual text attached to Person[index]
11–19  kill Person[index-10] objective
-1     win when entered
20–40  launch Dialog[type-20]
```

All bundled `20` hotspots launch dialogue 0; no bundled map uses `21–40`.

Historical Wolfire docs say kill hotspots are `11–20`, while current source uses `11–19` because 20 begins the dialogue-trigger range. Keep that discrepancy documented.

### Weird hotspot details

- `-111` is a legacy binary-map demo marker. The old loader has `int indemo; // FIXME this should be removed`, and older versions infer it from type `-111`. Current JSON loader does not interpret it. All converted JSON examples are inert size-0 `text="mapname"` records.
- Some `-1` hotspots are also size 0, functioning as disabled/stale records alongside a real exit hotspot.
- Hotspot dialogue triggering happens in `GameDraw.cpp`, so rendering can start a conversation and mutate gameplay state.
- Trigger checks compare **squared distance** directly to `Hotspot::size`; editor visualization scatters particles out to roughly `size`. Effective trigger radius is therefore approximately `sqrt(size)`, not `size`, despite the visualization. This needs runtime/editor testing before any cleanup.

## U6. Dialogue type values are now verified rather than approximate

`Dialog::tick()` uses:

```cpp
playerId = type % 10;
special = type > 9;
```

Verified type-family behavior:

```text
1–9   manual/reusable interaction near Person last-digit index; non-hostile only
11–19 automatic one-shot proximity interaction; non-hostile
20–29 automatic one-shot proximity interaction; completion sets hostile=1
30–39 automatic one-shot proximity interaction; completion sets windialogue=true
41–49 one-shot interaction that bypasses normal hostility AND distance checks
50–59 one-shot proximity interaction; completion sets hostile=1 and all NPCs to attack state
```

The 41-range is more interesting than we thought: because distance is bypassed and every dialogue is ticked each frame, `type 41` commonly behaves like a **map-start autorun cutscene** when Person 1 exists.

### Bundled dialogue type census

```text
1:7   2:7   3:7   4:7   5:7
10:1
11:16 12:2  13:1  14:1
21:12 23:1  25:1  26:2
30:10 31:13 32:13 34:4 35:3 36:3
41:23
51:2
```

No other values occur.

The sole type-10 entry is `jendraz1` dialogue 4. Since `playerId = 0` fails the ordinary `playerId > 0` trigger condition and the map has no type-24 hotspot to launch dialogue 4, it appears unreachable/stale in the current path. It duplicates another guard line in that map, reinforcing that interpretation, but provenance should still be checked before deleting it.

## U7. Dialogue participants are direct Person indices and stay moved

A dialogue owns ten participant transform slots. `DialogScene::participantfocus` selects a Person index.

`Dialog::play()` iterates scenes, and for each focused participant it:

- assigns the stored participant position;
- assigns stored yaw/targetyaw;
- zeroes velocity;
- forces idle state.

There is no restoration step when the dialogue ends. The cutscene-authored position becomes the continuing gameplay position.

That old authoring model matches historical Wiki documentation describing participants as characters whose head direction was fixed during dialogue setup and who were teleported to recorded positions when the dialogue began.

## U8. Dormant dialogue script hooks

When advancing dialogue scenes, current code recognizes:

```text
scene sound -5 → create a type -1 win hotspot at the player, size 10
scene sound -6 → set hostile = 1
```

None of the 862 bundled JSON dialogue scenes uses either value. Observed scene sounds are only:

```text
0,1,2,3,4,5,6,7,8,9,10,13,15,16
```

All 862 bundled scenes also serialize `participantaction = 0`.

These are useful format fossils: current schema still carries fields/opcodes whose authored use has disappeared from bundled content.

## U9. Historical `#` dialogue padding is real compatibility data

Current rendering strips `#` from dialogue speaker/text strings.

Historical Wolfire FAQ/dialogue docs say strings of exactly 10 or 13 characters could crash a map. The workaround was to append `#`, which would not display. The docs also give a 128-character text limit.

This explains the otherwise absurd hash-filled historical dialogue strings. Preserve them in original content unless a tested compatibility conversion deliberately normalizes them.

## U10. Text systems that look related are unrelated

There is no generic reusable `Person.name` field in current map JSON.

What the player sees comes from separate systems:

```text
DialogScene.name/text → conversation speaker + dialogue panel
Hotspot.text          → contextual bottom-screen message
Tutorial.cpp strings  → stage instruction UI
Awards/bonuses        → score/HUD text
```

So a future NPC-name system would be new architecture, not merely exposing an existing hidden field.

## U11. Global hostile is permission to aggro, not a faction system

No general faction/team id was found.

Classic relationship logic is mainly:

```text
global hostile flag
+ per-Person aitype
+ hardwired player-0 target assumptions
+ temporary ally index for get-help/get-weapon state
```

NPCs instantiate passive. On a hostile map they still patrol until perception/damage transitions them into combat.

This makes dialogue semantics meaningful:

- type 20s: turn the map hostile and let perception take over;
- type 50s: turn hostile and force all NPCs into immediate attack state.

`guardtype=1` and `attacktype=3` are defined in `Person.hpp` but have no other current references. The live combat/chase state is the oddly named `attacktypecutoff=4`.

## U12. Patrol waypoints and navigation nodes are different systems

Per-Person patrols use ordered waypoints and two waypoint modes:

```text
0 keep walking
1 pause (four seconds at arrival)
```

The map-global pathpoint graph is a separate obstacle-navigation aid used by `pathfindtype`, especially when returning toward a patrol/final target after losing the player.

Bundled corpus:

```text
224 Persons have authored patrol paths
1,144 keep-walking waypoint records
169 pause waypoint records
patrol length range: 2–20
24/137 maps have a global pathfinding graph
113/137 maps have none
```

Historical map-editing docs independently distinguish Shift-P patrol points from Control-P pathfinding points.

## U13. AI is more systemic than the current docs gave it credit for

### Hearing

Global environmental-sound events have position/volume/lifetime. Sleeping reduces perceived volume. Rabbits receive a much larger hearing threshold multiplier.

### Sight

AI combines distance, animation height/posture, facing, randomized update cadence, and a head-to-head object collision check. Low posture slows suspicion accumulation. Detection can first produce `searchtype` rather than perfect attack knowledge.

### Body/ragdoll discovery already partly exists

Passive AI's LOS loop considers not only Turner but also Persons whose skeleton is free or whose AI is non-passive. A visible dead/unconscious/ragdolled body can therefore cause search behavior around that body's position.

This materially changes the corpse-dragging design conversation: Expanded mode does **not** need to invent body discovery from zero. It can make an existing primitive behavior clearer/richer.

### Wolf smell

Wolves can smell:

- Turner;
- Turner at increased threshold when carrying a bloody weapon;
- dead bloodied Persons at a larger threshold.

The smell point is offset by normalized `windvector`, so environment wind has a small gameplay role.

### Search/escape

Search AI moves to remembered `lastseen`, checks perturbed nearby points, can reacquire through sight/hearing, and eventually switches to pathfinding back toward its patrol. Escaping increments `numescaped`, which later feeds the Escape Artist award.

### Primitive AI communication already exists

Injured rabbits can enter `gethelptype`, seek a passive ally, and when they reach one, put both into `searchtype` while transferring last-seen/check data.

That is a tiny genuine alarm-sharing system already in classic Lugaru.

## U14. Rendering can affect AI perception

One of the strangest couplings found in this pass:

`Person::occluded` is updated in `GameDraw.cpp` using camera/viewer-to-NPC collision tests for rendering/culling. `Person::doAI()` later checks the same field before running parts of passive/search sight logic.

The AI still performs an actual NPC-head-to-target-head collision test after that gate, but render-owned occlusion can decide whether perception work proceeds.

This deserves a targeted regression experiment using camera modes/angles before refactoring. It may be an old optimization whose side effects became gameplay.

## U15. Environment controls more than terrain art

Serialized environment values:

```text
0 snow
1 grass
2 desert
```

Corpus:

```text
snow   50 maps
grass  64 maps
desert 23 maps
```

`Setenvironment()` swaps terrain/object textures, footsteps, skybox image set, ambient audio, and wind vector. Snow additionally drives precipitation rendering.

Maps separately store view distance, fog fade start, skybox tint, skybox-light tint, and skybox-texture enablement.

So Lugaru's "weather/biome" system is a coarse global environment preset plus per-map lighting/fog values, not a generalized weather controller.

## U16. Music is an AI-state monitor

Base theme is selected only from environment:

```text
snow   → Music1Snow
 grass → Music1Grass
 desert→ Music1Desert
```

Any living NPC in attack/get-weapon/get-help/search state switches the game to the fight theme, except several sneak-attacked animations. Entering combat music plays the alarm sound. A one-second delay keeps combat music from dropping immediately when the threat clears.

This means even a searching NPC or an injured rabbit running to get help is enough to make the soundtrack report "combat/threat."

Dialogue itself does not own music cues.

## U17. Slow motion is cross-system simulation state

Auto slow motion can be set for about 0.2 seconds on certain finishing hits/deaths near the viewer/player.

When `slomo` is active:

- gameplay frame time is multiplied by default `slomospeed = 0.25`;
- renderer selects slow-motion blur where available and cancels camera shake;
- OpenAL pitch is reduced using default `slomofreq = 8012` relative to 44100 Hz.

Historical movement docs also explicitly mention automatic kill slow motion.

## U18. Fall damage is ragdoll impact physics

Fast downward jump/fall motion can force ragdoll at approximately `velocity.y < -30`.

Actual environmental damage then comes from `Skeleton::DoConstraints()` collision bounce/impulse against terrain/objects, with Person damage applied from the returned constraint damage.

So there is no simple fall-height damage table. Landing physics and surface/object contact matter.

Non-dead ragdolls can recover under specific velocity/clearance/input conditions.

## U19. There is a separate hard world-edge clamp

This finally answers the map-radius confusion cleanly.

`mapcenter/mapradius` are stored with the map and used for editor boundary visualization/editing.

Separately, Person movement clamps coordinates against a circle derived from the terrain midpoint and approximately:

```text
terrain half-size - viewdistance
```

That runtime containment is independent from `mapradius` and independent from exit hotspots.

Thus the game has three different concepts that can look like a "world border":

1. editor mapcenter/mapradius circle;
2. runtime terrain/viewdistance clamp;
3. authored `-1` win hotspot near a narrative exit.

## U20. Blood is both visual and mechanical

Blood state includes Person blood loss/bleeding, body/skin effects, weapon blood/drips, and terrain/object decals.

Mechanically:

- heavy blood loss can push injured rabbits into get-help/attack behavior;
- wolves can smell dead bloodied bodies;
- bloody weapons carried by Turner increase wolf smell sensitivity.

This is important for Expanded stealth: blood cleanup/tracking mechanics would be extending real underlying state, not inventing a purely cosmetic subsystem.

## U21. Fire propagates in a limited way

`firetype` objects begin burning. Persons can catch fire and can put themselves out through the rolling behavior.

Current collision code propagates fire both ways between a Person and nearby `firetype`/`bushtype` objects.

No general object-to-object spread loop was found. Weapon has an `onfire` field/render path, but no assignment that actually ignites a weapon was found in the current tree.

So Classic has stateful ignition and some propagation, but not a broad generic fire simulation.

## U22. Weapons are runtime world objects but save as Person inventory

Runtime weapon behavior is genuinely physical:

- ownership/ground state (`owner == -1` for free weapon);
- throw/drop physics;
- AI weapon seeking;
- sticking into bodies;
- pulling stuck weapons back out;
- blood state/dripping;
- held-weapon drop on death.

But map JSON serializes weapon types under a Person. There is no root-level free-weapon list.

Historical editor documentation explicitly says loose weapons disappear after save/reload.

That is a real Create-mode limitation, not merely bad UI.

## U23. Score is not just "points at level end"

`Bonuses.def` supplies base values for named combat events. `award_bonus()` only scores player 0.

Repeated bonuses are diminished by a per-bonus divisor. Most repeat counters increase by 1; the Solid Hit/combo family increases by only 0.15, so combo scoring falls off more slowly.

Tutorial zeroes bonus values entirely.

Challenge completion saves the score delta and time to challenge-specific Account arrays; campaign completion adds score to cumulative campaign score.

The campaign `time` field is only assigned the last completed level's time. `setCampaignFinalTime()` exists but has no caller in this tree, and campaign end UI shows score/high score only. Full-campaign time support therefore looks dormant/incomplete.

## U24. Awards are derived runtime classifications

Current source awards are based on stats accumulated while playing. Verified examples:

- Flawless: no damage + no blood loss;
- Take no prisoners: all NPCs fully dead (`dead == 2`);
- Merciful: all NPCs in the alternate dead/unconscious state (`dead == 1`);
- One with the shadows: no NPC responded and no thrown kill;
- Escape artist: at least one AI search expires/loss event;
- Gymnast: many flips with no flip failures;
- Blade slinger: throw-kill count equals enemy count;
- Brutal: after-kill attacks plus everyone fully dead;
- Aikido master: >80% reversal ratio and >3 reversals;
- Divide and conquer: maximum simultaneous alarm count equals one;
- Suicidal: >3 failed flips.

`Awards.def` still contains `Coward`, `Hyper`, `Rambo`, `Fast`, `Real fast`, and `Damn fast`, but current `award_awards()` never assigns them. Another set of dead/fossil definitions.

Historical Wiki descriptions are useful but should not override the current code when exact behavior matters.

## U25. Challenges are normal maps with different account plumbing

The challenge simulation is ordinary Lugaru. `map1` through `map14` use the same loader, combat, AI, hotspots, scoring and physics.

What is challenge-specific is the numeric loader/menu plus Account progress/highscore/fasttime.

Odd fossil: `LoadLevel(int)` accepts indices 0–15, i.e. `map1` through `map16`, while the menu exposes 14 levels and only 14 challenge JSON files are bundled.

## U26. Tutorial is effectively a special embedded ruleset

`Tutorial::active` is not an overlay flag.

Examples found across source:

- normal damage accumulation is disabled in `Person::DoDamage()`;
- bonus values are zeroed;
- AI anger/perception is gated by `cananger`;
- stages directly toggle enemy AI states and attack permission;
- many NPC blood/impact/speech paths are suppressed;
- tutorial NPCs use translucent/special rendering and attack highlighting;
- Skeleton impact effects/sounds are selectively suppressed for tutorial NPCs;
- combat/weapon branches contain explicit tutorial/stage checks;
- ordinary level transition logic is disabled while tutorial is active.

This strongly validates the existing decision: preserve the historical tutorial as-is under Classic. Do not make "clean tutorial architecture" a prerequisite for other work.

## U27. Binary-format fossils still visible in JSON/current source

The pass identified a useful list of compatibility archaeology:

```text
-111 demo-marker hotspots
#-padded dialogue strings
numeric dialogue script-opcode ranges
negative scene sound opcodes -5/-6
participantaction field (all bundled values 0)
unused map objective modes 2/3
unused guardtype/attacktype AI constants
branching nextlevel vector with no bundled branching
challenge loader slots 15/16 with no bundled files
Person-owned weapon persistence / loose weapon loss
mapcenter/mapradius editor boundary data distinct from runtime world edge
```

These should be cataloged before any schema normalization.

## U28. Design implications from the archaeology

The September 18 direction still holds, but several extension ideas are now better grounded:

### Corpse dragging

Existing AI already visually notices free/ragdolled Persons and wolves smell bloody dead bodies. Corpse dragging can manipulate **real existing perception inputs** rather than requiring a brand-new body-discovery subsystem.

### Semantic events

A gameplay event layer now has clear first consumers:

```text
objective completion
AI body noticed / player spotted / search lost
combat bonus/scoring
fight-music threat transitions
tutorial/training
Arena scoring/replays
```

It should observe authoritative existing transitions, not recreate their rules.

### Expanded objectives

Current objectives are a patchwork of all-dead checks, hotspot opcodes, dialogue type ranges, and target-kill magic numbers. Expanded content should avoid adding another magic-number range. A semantic objective layer can wrap Classic behavior while new content uses explicit objective definitions.

### Modern campaign browser

The legacy world map is already cleanly separate from gameplay. A modern campaign browser can therefore replace presentation while still feeding existing campaign map/choice data. Preserve the old world-map path as Classic presentation where practical.

### Create mode

The old editor already authors objects, Persons, patrol routes, pathfinding nodes, dialogues, hotspots, environment, and map bounds. Browser Create work should expose/stabilize these systems first. Persistent free weapon placement would be a genuine new map-format capability.

### AI cleanup caution

Do not pull AI perception out of rendering until the `occluded` coupling has been tested. Weird does not mean behaviorally irrelevant.

## U29. High-value follow-up experiments created by this pass

1. Reproduce AI perception with the same player/NPC positions but different camera visibility to quantify `occluded` gameplay effect.
2. Measure hotspot editor visualization versus actual trigger boundary to confirm the `distsq < size` mismatch in play.
3. Load `Lugaru2` directly and through `Lugaru1` stealth transition; compare object geometry/state to document the destination-object-list discrepancy.
4. Trace `guardtype` and `attacktype` through older source history/git blame to determine whether they were once live states.
5. Search historical binary maps/mod campaigns for dialogue scene sounds `-5/-6`, `participantaction != 0`, hotspot values 11–19/21–40, and true campaign branching.
6. Trace old source/history for `jendraz1` dialogue type 10 before classifying it permanently as dead data.
7. Verify whether the old map editor intended Hotspot `size` to be radius, squared radius, or simply an inconsistent debug visualization.
8. Trace `setCampaignFinalTime()` history to see whether campaign speedrun timing was abandoned or accidentally disconnected.
9. Verify weapon `onfire` provenance/history; current field appears dormant.
10. Build regression fixtures for `ChooseNext=2`, dialogue participant teleport persistence, hotspot win/dialog triggers, tutorial no-damage behavior, and AI corpse perception before refactoring those systems.

## U30. Historical sources revisited in this continuation

- `https://wiki.wolfire.com/Map_Editing_in_Lugaru` — map editor, patrol/pathfinding distinction, campaign file/ChooseNext, world-map coordinates, loose weapon persistence.
- `https://wiki.wolfire.com/Lugaru%27s_Console_Guide` — map objective commands, hotspot authoring/types, view distance/fade controls.
- `https://wiki.wolfire.com/Dialogues` — old dialogue authoring workflow and participant teleport behavior.
- `https://wiki.wolfire.com/Lugaru_FAQ` — 10/13-character dialogue crash, invisible `#` padding workaround, old map/dialogue corruption advice.
- `https://wiki.wolfire.com/Lugaru_Character_Movement` — historical player-facing slow-motion behavior.
- `https://wiki.wolfire.com/Lugaru_Combat_Strategy` — historical scoring/attack-bonus descriptions.
- `https://wiki.wolfire.com/Lugaru_Walkthrough` — challenge strategies and historical award descriptions.

As before, surviving Wiki material is historical evidence, not automatically the source of truth for the modern OSS/Loupgarenne/Lugaru-Web tree. Exact current behavior above was taken from source/data first and compared against the historical documentation second.

---

# V. 2026-09-24 — Web key rebinding fixed (B-7)

**VERIFIED SOURCE FINDING:** `Menu::setKeySelected()` depended on `SDL_CreateThread()`, but the current Emscripten SDL2 port selects a non-threaded backend that rejects thread creation. The failure path cleared `waiting` without clearing `keyselect`, leaving the selected binding displayed as `_` without an active capture worker.

**HISTORICAL EVIDENCE:** Côme BERNIGAUD's June 2010 commit `5e808c5` explicitly describes introducing threaded key configuration; follow-up `e31b703` contains the implementation. This path predates Loupgarenne and the WASM port.

**VERIFIED SOURCE FINDING:** Commit `a7bfd51` adds Web-only `pollKeyCapture()` / `Menu::isKeyCaptureActive()` integration with the menu, `Game::ProcessInput()`, and Emscripten main loop. Capture uses nonblocking SDL polling, preserves control ordering and keyboard scancode/mouse-button encoding, discards queued selection presses, cancels on Escape, and clears `waiting` and `keyselect`. The captured press is consumed through release to avoid menu/hotkey reuse. No pthreads, Asyncify, broader input refactor, settings-format change, or save-cadence change was introduced. Native threaded capture was intentionally left unchanged.

**Build validation:** `cmake --build build-web -j2` completed successfully. An initial linker attempt was blocked by sandbox access to an Emscripten cache lock; the retry with cache access succeeded without compiler/linker warnings in its log. An isolated harness exercised capture-state behavior, but does not substitute for browser testing.

**RUNTIME-VERIFIED (Chrome, 2026-09-24):** Before the fix, selecting a control displayed `_` and logged `Unable to create thread: Threads are not supported on this platform`; replacement keys did nothing and another control could not be selected. Escape behaved normally, and refreshing restored the original display. After the fix, capture starts normally without that error; keyboard and mouse rebinding work, Escape cancels cleanly, another control can be selected, rebound actions work in gameplay, and leaving Options saves bindings that reload successfully through existing persistence.

**OPEN QUESTION:** Native worker-thread `SDL_WaitEvent()` usage conflicts with SDL's documented video-thread requirement and remains a separate portability investigation (backlog A4a). The Chrome verification does not establish native behavior or exhaustive coverage of held hotkeys, focus transitions, or browser-reserved shortcuts.


---

# W. 2026-09-25 — Controller input foundation completed (B-8)

**HISTORICAL EVIDENCE — prior research:** original/local Lugaru ancestry did not reveal inherited controller support. The 2019 `usineur/lugaru` Switch fork (`9af3344`) remains the concrete historical controller reference. Its SDL GameController approach informed this work; its startup-only assumptions were not copied blindly.

**VERIFIED SOURCE FINDING:** `e5939fe` (`feat: add controller input foundation`) implements milestone 1 in `Source/Utils/Input.cpp`, `Source/Utils/Input.hpp`, and `Source/main.cpp`. SDL GameController initialization, startup discovery, periodic discovery while absent, one active controller, raw standardized axes/buttons, disconnect reset, and reconnect use a handle/instance identity rather than retaining a device index. Controller state stays separate from keyboard/mouse bindings, with no additional SDL event-queue consumer. No gameplay, camera, menu, or rebinding mappings were added; this remains narrower than a generalized input rewrite.

**Build validation (reported):** `cmake --build build-web -j2` succeeded; `git diff --check` passed before the source commit.

**RUNTIME-VERIFIED — isolated native SDL harness:** virtual-controller tests covered no-controller startup, startup discovery, hot-plug, button/axis reads, device-index changes, disconnect/reset, reconnect, and shutdown. This was not a full native Lugaru gameplay test.

**RUNTIME-VERIFIED — physical manual test:** Windows, Firefox, localhost, wired Sony DualSense / PS5 controller, DS4Windows disabled. Lugaru detected device 0 and opened instance 0 as `Standard Gamepad`. Left-stick values included signed-range, intermediate, and near-center readings; both right-stick axes produced positive and negative analog values. Triggers reported `0` / `32767`, matching the previously identified bundled SDL Emscripten backend behavior. Shoulders and stick clicks produced clean down/up events; additional buttons were exercised. Physical disconnect/reconnect reopened the controller and resumed input without observed stuck state. Mouse controls still worked, and controller gameplay/menu input remained intentionally unmapped. Firefox did not require fullscreen once the Gamepad API exposed the device.

**Historical runtime observation (milestone 1; later Chrome success recorded in section Z):** raw Bluetooth DualSense support was not successfully established in the final test. Chrome continued returning null `navigator.getGamepads()` entries on this development machine, even after renderer focus and fullscreen testing; this is a separate browser/controller compatibility investigation, not an established SDL foundation failure. No WebHID or Chrome-specific workaround was added.

**Decision at milestone 1 (subsequently completed; see section X):** next implement `left stick -> deadzone/threshold logic -> existing Lugaru digital movement booleans`, preserving Classic movement semantics rather than proportional analog movement. [Backlog A6](research-backlog.md#a6-controller-support-completed-scope-and-remaining-work) tracks the current validation, compatibility, tuning, configuration, ownership, and advanced-feature work; [porting notes B-8](porting-notes.md#b-8--controller-gameplay-menuui-and-tutorial-support) records the detailed milestone evidence.


---

# X. 2026-09-25 — Controller movement, gameplay actions, and look completed

**VERIFIED SOURCE FINDING:** branch `investigate/controller-support` records the progression after foundation `e5939fe`:

| Milestone | Commit | Integration |
| --- | --- | --- |
| 2 — movement | `4b8af1d` | Left-stick hysteresis (`10000` activate / `7000` release) ORed into existing `Person` movement booleans |
| 3 — gameplay actions | `2387ebf` | Combined keyboard/mouse/controller held actions, attack press edges for combat/dialogue consumers, and jump-based respawn |
| 4 — camera look | `ab46b53` | Right-stick continuous response after `8000` axial deadzone; `120°/s` yaw / `90°/s` pitch integrated with `realmultiplier` once per outer frame |

**HISTORICAL EVIDENCE / decision:** the earlier ancestry search did not find inherited controller support; the 2019 `usineur/lugaru` reference informed SDL GameController use and digital movement. Lugaru-Web deliberately uses Cross jump, Circle crouch, Square draw/sheath, L1 throw/pickup, and R1 attack instead of copying its face-button layout. Trigger-assisted camera rotation was not copied. Fixed per-frame camera increments would depend on refresh rate in the current Web loop; the new look path uses elapsed time before gameplay modifiers and physics substeps, retaining the 250 ms stall cap.

**Build validation:** each milestone passed the Web build and `git diff --check`. **RUNTIME-VERIFIED — isolated harnesses:** movement thresholds/lifecycle; action mappings/handoffs/press edges; camera response/timing at 30/60/120/144/240 Hz and irregular intervals, plus action regression checks. These do not replace full native gameplay or physical browser refresh-rate tests.

**RUNTIME-VERIFIED — supplied physical test results:** wired DualSense in Firefox, DS4Windows disabled. Movement, all five gameplay actions, attack-driven dialogue without unintended held-repeat advancement, and camera look worked. Center noise/deadzones, diagonals, look pitch limits, simultaneous keyboard/mouse usage, and disconnect/reconnect without stale movement/action/look were verified. Later tests retained earlier milestone behavior. At milestone 4, triggers, Triangle, Options/Start, D-pad, and other unused inputs remained unmapped; section Y records the subsequent Start/D-pad and tutorial Triangle additions.

**Interpretation / open work:** eight-direction movement/facing snapping is expected for Classic digital movement; true analog movement/arbitrary facing remains a separate design problem. Camera look felt mechanically correct but somewhat slow for preference; current speeds are unchanged and sensitivity tuning/configuration remains open. Bluetooth and Chrome exposure were unverified/unresolved at that point, as recorded in section W; see section Z for later wired Chrome success.

**RUNTIME-VERIFIED — UI observation at milestone 4:** clicking the existing campaign selection UI changed campaigns. **Decision at that milestone (subsequently completed in section Y):** investigate and reuse the existing clickable UI for controller pause, confirm/back, and menu navigation. Controller campaign navigation was not yet implemented at that point. Detailed evidence is consolidated in [B-8](porting-notes.md#b-8--controller-gameplay-menuui-and-tutorial-support); current remaining work is in [A6](research-backlog.md#a6-controller-support-completed-scope-and-remaining-work).

---

# Y. 2026-09-26 — Controller menu/UI and tutorial compatibility completed

**VERIFIED SOURCE FINDING:** `git log` on `investigate/controller-support` records:

| Change | Commit | Integration |
| --- | --- | --- |
| Milestone 5 — pause/menu UI | `8d549e4` | Shared virtual cursor and existing pointer activation; Start pause/resume, Cross confirm, contextual Circle Back, challenge-result continue/back |
| Tutorial advance | `bd7b207` | Triangle press edge joins the existing tutorial Tab handler, with gameplay guards and release suppression; stage 51 remains unskippable |
| Tutorial camera objective | `f00ec72` | Stage 3 accepts mouse input or meaningful `Input::controllerLook()` X/Y, preserving the existing success/timing logic |

The menu investigation selected virtual cursor reuse rather than discrete focus or a hybrid UI. Left stick and fine D-pad motion share the mouse cursor and existing logical 640×480 hit-testing. Integration is once per outer frame using elapsed real time, not once per menu/physics tick. Start is restricted to gameplay/pause; Circle invokes existing contextual actions rather than global Escape. Campaign switching/world-map activation retain existing progress checks. Held controller presses are consumed through release across UI/gameplay/modal transitions. Name entry remains keyboard-based; controller rebinding and stereo secondary-click were not added.

**HISTORICAL EVIDENCE — prior investigation:** the 2019 `usineur/lugaru` fork provides digital stick movement, Start pause (`9af3344`), and cursor emulation/A-as-click (`7b81f2f`) precedent. Lugaru-Web did not copy its face-button layout, trigger-assisted camera behavior, or fixed-per-frame camera/cursor increments. The latter would be refresh-rate dependent in the current Web loop. The inspected historical tutorial shortcut remained Tab-only; current Triangle support is a narrow semantic addition, not inherited Tab emulation.

**Build validation / RUNTIME-VERIFIED — isolated harnesses:** all three changes passed Web builds and `git diff --check`. Tests covered cursor timing/scaling/deadzone/precision/bounds at multiple regular and irregular render rates; confirm edges, contextual destinations, pause/audio and challenge-result transitions; release suppression and lifecycle reset; tutorial advance/sound/stage 51/unchanged Tab; stage-3 mouse and horizontal/vertical stick recognition with shared deadzone/context checks. Camera, gameplay-action, menu, and Triangle regression checks passed as applicable. These checks do not establish full native gameplay or physical multi-refresh-rate behavior.

**RUNTIME-VERIFIED — supplied physical QA:** Firefox, wired DualSense, DS4Windows disabled. Milestone 5 passed pause/resume, shared virtual cursor, menu confirm/contextual Back, campaign switching, world-map interaction, and challenge/menu flows. Earlier controller detection, analog-axis readings, disconnect/reconnect, digital movement, all five gameplay actions, dialogue attack, camera look, and mixed keyboard/mouse input remain physically verified.

**OPEN QUESTION — tutorial physical QA:** no physical verification has yet been supplied for `bd7b207` or `f00ec72`. Their implementation and automated checks are established, but neither should be described as physically runtime-verified. Triangle keeps Tab semantics/stage 51 intact; stage 3 reuses the existing post-deadzone signal without synthetic mouse input or duplicated math. Tutorial text remains keyboard-oriented.

**Supplied runtime observation / OPEN QUESTION:** player movement retains Classic approximately eight-direction locomotion; enemy/NPC movement had also appeared quantized, with its source mechanism uninvestigated at that point (subsequently separated from digital player intent; see section Z). Do not infer a controller or Web regression from that observation. Current movement thresholds work; camera sensitivity and movement feel may receive later tuning. True analog locomotion remains a separate animation/facing/combat/timing design problem. Browser compatibility and other deliberate scope exclusions remain in backlog A6; the next validation priority is physical tutorial QA.


# Z. 2026-09-27 — Character restoration finalized; scheduler kept separate

**VERIFIED GIT HISTORY / VERIFIED SOURCE FINDING:** comparison used literal 1.2 and its shared preparation parent independently, Ryan's inherited `8edd5ee`, pre-modernization `8cee3e3`, and current OSS `42f148d`. The release-side-branch nuance and canonical TalkIdle decision are recorded in [history and lineage](history-and-lineage.md#exact-preservation-checkpoints-2026-09-27). The specific yaw/tilt/head changes in `1db2761` were isolated rather than treating all modernization as broken.

**RUNTIME-VERIFIED — user physical progression:** native 1.2 and current OSS felt smooth; bounded yaw improved Web turning but was insufficient alone. OSS-style scheduling had little player effect and small NPC slope/seam benefits. Bounded tilt substantially improved presentation; restored head tracking worked well and resembled original Lugaru. Forward movement correctly followed rotating camera-forward. Combined restored/default builds played normally with controller behavior intact and no perceptible remaining player-locomotion problem.

**Decision / finalization:** yaw, tilt, and draw-order-safe historical head behavior are normal code paths. Character A/B definitions and literal-1.2 dialogue fallback were removed; current OSS TalkIdle remains. The scheduler experiment was preserved as a verified external patch and removed from the character commit's diff. Default scheduling stays fixed at `1 ms`. [B-13](porting-notes.md#b-13--player-direction-changes-snap-instead-of-rotating) owns the exact implementation, patch provenance, and final build/check evidence. No verified remaining player-locomotion regression currently warrants another code change. Scheduler/NPC observations remain open, and camera follow is not part of this milestone.

**RUNTIME-VERIFIED — latest browser observation:** the user reports wired DualSense input working in current Chrome with the final Web build, despite earlier null-gamepad results. Character changes did not modify browser/controller input; the cause is unknown. Firefox/wired DualSense remains the most extensively validated combination. This is not evidence for Bluetooth, other Chromium versions, operating systems, or controllers. Tutorial-specific physical checks retain their previously recorded limits unless separately confirmed.
