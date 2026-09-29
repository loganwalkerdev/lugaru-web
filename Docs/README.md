# Lugaru-Web Documentation Index

These documents capture the technical, historical, preservation, and design understanding of Lugaru-Web, including the September 23 archaeology continuation, controller implementation updates, character restoration, and the 2026-09-28 Web sprite performance fix.

They are intentionally separated by purpose:

- `history-and-lineage.md` — project ancestry, authorship, Lugaru 2/Overgrowth research, and what each lineage layer contributed.
- `architecture.md` — a living mental map of the current codebase, including the campaign/world-map/dialogue/hotspot objective machinery, important couplings, and likely extension seams.
- `design-direction.md` — preservation/product principles, Classic/Expanded/Arena rulesets, UI direction, mechanics, multiplayer, and long-term product identity.
- `preservation-and-modding.md` — historical community content, archaeological leads, mod/tool preservation, licensing workflow, and restored-edition policy.
- `research-backlog.md` — unresolved questions and concrete research/code-reading tasks.
- `research-session-2026-09-18.md` — working notes from the September 18 session, September 23 archaeology continuation, and subsequent implementation/evidence updates. This is the broad capture document; the other files are the cleaner long-lived references.
- `porting-notes.md` — browser-port implementation, build and runtime evidence, including controller gameplay/menu UI and tutorial validation limits.

## Documentation conventions

Stable documentation should distinguish between:

- **Verified history** — supported by source history, public Wolfire material, OSS records, or repository evidence.
- **Code finding** — observed in the current source tree or a reference repository.
- **Decision** — current Lugaru-Web direction.
- **Idea** — plausible future work, not a commitment.
- **Open question** — something still requiring code archaeology, historical research, testing, or licensing verification.

The project should prefer evidence over folklore. When attribution matters, use repository history, `git blame`, original forum/blog posts, and preserved licensing statements rather than assuming every old line or behavior came directly from David Rosen.

## 2026-09-23 archaeology continuation

The September 23 continuation is integrated primarily into `architecture.md` and `research-session-2026-09-18.md`, with unresolved runtime/provenance checks moved to `research-backlog.md` and compatibility implications added to `preservation-and-modding.md` and `design-direction.md`.

## Controller implementation updates

Controller gameplay and pointer-based menu/UI support are physically verified in Firefox with wired DualSense, DS4Windows disabled. Triangle tutorial advance and stage-3 right-stick recognition are source/automated-validated, with physical QA pending. Current evidence is consolidated in [porting notes B-8](porting-notes.md#b-8--controller-gameplay-menuui-and-tutorial-support), design seams in `architecture.md`, and remaining compatibility, tuning, and scope work in [backlog A6](research-backlog.md#a6-controller-support-completed-scope-and-remaining-work).


## Character restoration milestone

Historical yaw, tilt, and head behavior are finalized defaults with successful combined physical QA. [Architecture section 5](architecture.md#5-movement-facing-animation-and-high-refresh-behavior) owns the design, [B-13](porting-notes.md#b-13--player-direction-changes-snap-instead-of-rotating) the source/runtime evidence and separately preserved scheduler patch, and [history and lineage](history-and-lineage.md#exact-preservation-checkpoints-2026-09-27) the literal-1.2 side branch and canonical TalkIdle decision. Latest wired Chrome controller success and earlier exposure failures are distinguished in B-8; broader compatibility remains open.

## Web sprite performance milestone

The shipped Seven Tasks task3 fire/sprite slowdown is resolved for R1. [Porting notes B-15](porting-notes.md#b-15--dense-fire-sprite-rendering-slows-web) holds the A/B reproduction, experiment measurements, production renderer behavior, and manual QA; [backlog A8](research-backlog.md#a8-dense-fire-spawner-web-slowdown--resolved-for-r1-2026-09-28) records closure. Native sprite rendering and historical particle behavior remain preserved.

## Campaign progression fix

The Empire Sky Palace delayed-exit progression bug is resolved for R1 (`97121db`). [Porting notes B-16](porting-notes.md#b-16--delayed-spatial-exit-loses-campaign-progression-empire-sky-palace) holds the mechanism, fix, review, QA, and residual risk; [architecture 20.3](architecture.md#delayed-completion-occupancy-vs-recorded-win) the state semantics; [backlog A9](research-backlog.md#a9-empire-sky-palace-campaign-progression--resolved-for-r1-2026-09-29) closure and the separate open Emperor dialogue-trigger polish item (A9a).
