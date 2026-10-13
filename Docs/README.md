# Lugaru-Web Documentation Index

These documents capture the technical, historical, preservation, and design understanding of Lugaru-Web, including maintained findings from the September 23 archaeology continuation, controller implementation, character restoration, the Web rendering/performance fixes, Empire and Temple content repairs, and the clothing/image audit completed for v1.0.

## Current status (v1.0.0 release preparation, 2026-10-13)

Lugaru-Web v1.0.0 is the preservation and browser-stabilization release. Every B-series issue currently marked **Fixed**, as listed in [`PROJECT.md`](../PROJECT.md#known-browser-issues), is committed, with fixing commits recorded in [`porting-notes.md`](porting-notes.md). The current v1.0 Web build procedure uses Emscripten 6.0.9 and `build-web-release`, and the Web build serves its game data from the same origin; the final clean release rebuild is still pending. See [`build-provenance.md`](build-provenance.md#current-v10-release-build). Open items are summarized in [`PROJECT.md`](../PROJECT.md#near-term-priorities).

Full release QA has covered all bundled campaigns (Original, Seven Tasks, Temple and Empire) and the challenges; Chrome and Firefox were separately browser smoke/performance tested. Fixes through `87c182a` are recorded in porting notes; later release fixes are summarized in [`PROJECT.md`](../PROJECT.md#v100-release-qa-fixes).

## Documents in this directory

They are intentionally separated by purpose:

- `history-and-lineage.md` — project ancestry, authorship, Lugaru 2/Overgrowth research, and what each lineage layer contributed.
- `architecture.md` — a living mental map of the current codebase, including the campaign/world-map/dialogue/hotspot objective machinery, important couplings, and likely extension seams.
- `design-direction.md` — preservation/product principles, Classic/Expanded/Arena rulesets, UI direction, mechanics, multiplayer, and long-term product identity.
- `preservation-and-modding.md` — historical community content, archaeological leads, mod/tool preservation, licensing workflow, restored-edition policy, and the evidence rule for changing inherited campaign data.
- `research-backlog.md` — unresolved questions and concrete research/code-reading tasks, plus dated closure records for resolved investigations.
- `porting-notes.md` — browser-port issues B-1 to B-20 and CI-1, plus the release campaign QA fixes: implementation, build and runtime evidence, fixing commits, and validation limits, including controller gameplay/menu UI and tutorial validation limits.
- `build-provenance.md` — the current v1.0 Web release build procedure and the preserved 2026-09-16 baseline build record (toolchain, dependency pins, commands, historical artifact hashes).

Historical and upstream documentation also lives here and should not automatically override current source or the documents above:

- `DEVTOOLS.txt` — OSS Lugaru in-game developer tools reference.
- `README.Temple.txt`, `README.Empire.txt`, `README.SevenTasks.txt` — original campaign readmes.

## Related root-level files

- `../README.md` — public project overview, play link, controls, and known Web limitations.
- `../PROJECT.md` — project scope, principles, current status, issue table, and near-term priorities.
- `../COMPILING.md` — build entry points (Web and inherited native instructions).
- `../RELEASE-NOTES.md` — Lugaru-Web release notes followed by the inherited OSS Lugaru release history.
- `../COPYING.txt`, `../CONTENT-LICENSE.txt`, `../THIRD-PARTY-NOTICES.txt`, `../AUTHORS` — source license, game-content licenses and attribution, third-party software notices, and authorship/lineage.

## Documentation conventions

Stable documentation should distinguish between:

- **Verified history** — supported by source history, public Wolfire material, OSS records, or repository evidence.
- **Code finding** — observed in the current source tree or a reference repository.
- **Decision** — current Lugaru-Web direction.
- **Idea** — plausible future work, not a commitment.
- **Open question** — something still requiring code archaeology, historical research, testing, or licensing verification.

The project should prefer evidence over folklore. When attribution matters, use repository history, `git blame`, original forum/blog posts, and preserved licensing statements rather than assuming every old line or behavior came directly from David Rosen.

Dated research and validation entries are preserved as written. Some use working labels from development (for example, "resolved for R1" in backlog headings); these are kept so existing links remain stable.

## Milestone summaries

### 2026-09-23 archaeology continuation

The raw September research-session notes are no longer part of the current documentation set. Maintained findings should be cited from the long-lived documents above; the original working notes remain recoverable from Git history. Not every idea or lead in those notes was carried forward, and no complete migration has been verified.

### Controller implementation

Controller gameplay, camera, and pointer-based menu/UI support (including campaign/world-map and challenge flows) are physically verified in Firefox with a wired DualSense, DS4Windows disabled; this is the most extensively tested configuration. Triangle tutorial advance and stage-3 right-stick recognition are source/automated-validated, with physical QA still open. Current evidence is consolidated in [porting notes B-8](porting-notes.md#b-8--controller-gameplay-menuui-and-tutorial-support), design seams in `architecture.md`, and remaining compatibility, tuning, and scope work in [backlog A6](research-backlog.md#a6-controller-support-completed-scope-and-remaining-work).

### Character restoration

Historical yaw, tilt, and head behavior are finalized defaults with successful combined physical QA (`0305947`). [Architecture section 5](architecture.md#5-movement-facing-animation-and-high-refresh-behavior) owns the design, [B-13](porting-notes.md#b-13--player-direction-changes-snap-instead-of-rotating) the source/runtime evidence and separately preserved scheduler patch, and [history and lineage](history-and-lineage.md#exact-preservation-checkpoints-2026-09-27) the literal-1.2 side branch and canonical TalkIdle decision. Latest wired Chrome controller success and earlier exposure failures are distinguished in B-8; broader compatibility remains open.

### Web sprite performance

The shipped Seven Tasks task3 fire/sprite slowdown is resolved (`592e158`). [Porting notes B-15](porting-notes.md#b-15--dense-fire-sprite-rendering-slows-web) holds the A/B reproduction, experiment measurements, production renderer behavior, and manual QA; [backlog A8](research-backlog.md#a8-dense-fire-spawner-web-slowdown--resolved-for-r1-2026-09-28) records closure. Native sprite rendering and historical particle behavior remain preserved.

### Campaign progression fix

The Empire Sky Palace delayed-exit progression bug is resolved (`97121db`). [Porting notes B-16](porting-notes.md#b-16--delayed-spatial-exit-loses-campaign-progression-empire-sky-palace) holds the mechanism, fix, review, QA, and residual risk; [architecture 20.3](architecture.md#delayed-completion-occupancy-vs-recorded-win) the state semantics; [backlog A9](research-backlog.md#a9-empire-sky-palace-campaign-progression--resolved-for-r1-2026-09-29) records closure, and [backlog A9a](research-backlog.md#a9a-throne-room-emperor-dialogue-activation-area--resolved-for-r1-2026-09-29) records the resolved Emperor dialogue-trigger polish.

### Web foliage rendering fix

The dense-foliage Web rendering regression is resolved (`df72be9`). [Porting notes B-18](porting-notes.md#b-18--dense-web-foliage-causes-severe-slowdown) documents the GL4ES alpha-test/overdraw regression, Web-specific rendering fix, dependency patch, and clean-build validation; [backlog A9c](research-backlog.md#a9c-empire-sky-palace-dense-foliage-web-slowdown--resolved-for-r1-2026-09-30) records the Empire Sky Palace investigation that exposed it.

### Clothing, skins, and Temple fur

The Temple tinted-fur striping (B-20, `85477f5`), FurBW restoration (`fef0364`), clothing preservation archaeology, and image/clothing compositing hardening (`babb5d0`, `117b2ec`) are complete. [Backlog A11](research-backlog.md#a11-clothing-skins-and-historical-asset-audit--resolved-2026-10-03) records the audit, ASan evidence, preserved compositor quirk, and the remaining out-of-scope follow-up candidates.

### Self-contained Web release

`36407bd` removed the inherited CDN option and the shell redirect to Ryan Deering's server; the Web release now loads `lugaru.data` from its own origin. See [build provenance](build-provenance.md#same-origin-game-data).

### Release campaign QA

Full Original, Seven Tasks and Temple playthroughs produced fixes for the Long-ten Giant encounter, minimap helper markers, combat music after End Game, and the post-completion campaign map. They also removed stale or duplicated dialogue from Temple and `task9`. [Porting notes](porting-notes.md#release-campaign-qa-fixes-2026-10-05--2026-10-06) hold the fixes and validation. [Preservation 19](preservation-and-modding.md#19-changing-inherited-campaign-data-v10-campaign-qa-review) records the evidence rule and the data deliberately left unchanged, including `svencouncil`, `svenskip`, helper actors and awkward but authored dialogue. Later release QA, including Empire, is summarized in [`PROJECT.md`](../PROJECT.md#v100-release-qa-fixes).
