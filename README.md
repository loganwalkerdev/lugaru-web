# Lugaru-Web

**Lugaru in the browser — preserved, stabilized, and playable with no original game files required.**

### [▶ Play Lugaru-Web in your browser](https://loganwalkerdev.github.io/lugaru-web/)

Lugaru-Web is an independent open-source browser version of **Lugaru**, built on the
OSS Lugaru codebase and Ryan Deering's Loupgarenne WebAssembly work.

The goal of the project is to preserve the original Lugaru experience while making
it reliable, maintainable, and easy to play in a modern browser.

Version **1.0.0** is the project's preservation-focused first release. It concentrates
on browser compatibility, historical behavior, campaign reliability, rendering and
performance fixes, persistence, controls, and documentation rather than adding a new
gameplay layer.

## Play

No installation or original commercial game files are required.

**Browser build:**

https://loganwalkerdev.github.io/lugaru-web/

The complete open-source game data required by this build is included with the project.

### Browser support

The current Web build has been tested primarily in:

- **Google Chrome**
- **Mozilla Firefox**

Firefox with a wired DualSense controller is the most extensively tested controller
configuration. Current Chrome testing has also successfully exposed and used the
controller through the browser Gamepad API.

Other modern browsers may work, but they have not received the same level of release
validation. Safari has not yet been release-tested.

## Included content

Lugaru-Web includes the open-source Lugaru game and its bundled campaign and challenge content:

- **Original Campaign**
- **Tutorial**
- **Challenges**
- **Temple**
- **Empire**
- **Seven Tasks**

The project also restores and repairs historical content where repository archaeology
showed that later changes had unintentionally altered its intended appearance or
behavior.

## What v1.0 adds

Lugaru-Web keeps the original game at its core while repairing and modernizing the
browser port.

Highlights include:

- WebAssembly/WebGL browser build
- browser save persistence through IndexedDB
- persistent settings and keyboard/mouse rebinding
- fullscreen and browser-friendly resolution handling
- keyboard and mouse gameplay
- controller gameplay, camera, menu, and campaign navigation support
- restored historical character turning, body tilt, and head behavior
- fixes for browser-specific texture and rendering problems
- major Web performance fixes for dense fire effects and foliage
- campaign progression and map-content repairs
- restored historical Temple clothing/fur assets and behavior
- safer image and clothing compositing
- self-contained Web deployment with game data served from the same origin
- extensive preservation, architecture, and browser-port documentation

Many of these changes intentionally affect only the Web build when the original native
behavior should remain unchanged.

## Controls

### Keyboard and mouse

Lugaru's existing keyboard and mouse controls remain supported and can be rebound from
the in-game **Options → Controls** menu.

Mouse movement controls the camera and the existing pointer-based menus.

### Controller

Lugaru-Web adds controller support while preserving keyboard and mouse input.

The currently implemented layout includes:

| Control | Action |
| --- | --- |
| Left stick | Movement |
| Right stick | Camera |
| Cross / A | Jump / Confirm |
| Circle / B | Crouch / contextual Back |
| Square / X | Draw or sheath weapon |
| Triangle / Y | Advance supported Tutorial stages |
| L1 / Left shoulder | Throw / pick up / contextual weapon action |
| R1 / Right shoulder | Attack |
| Options / Start | Pause / resume |
| D-pad | Precision menu cursor movement |

Controller support currently uses SDL's standardized GameController interface. Broader
controller, Bluetooth, sensitivity, glyph/prompt, and controller-rebinding work remains
future polish rather than part of the v1.0 compatibility guarantee.

Account-name entry still requires a keyboard.

## Known Web limitations

Lugaru-Web intentionally documents remaining browser limitations instead of hiding them.

- The original motion-blur effect is currently unavailable in the Web build because the
  inherited implementation does not render correctly through the current WebGL path.
- Level loading remains synchronous and can briefly block the browser while a level is
  being loaded.
- Controller support has not yet been exhaustively tested across browsers, operating
  systems, Bluetooth configurations, and controller models.
- Some controller-oriented UI polish, including controller-specific prompts and
  rebinding, is still future work.

See [`Docs/porting-notes.md`](Docs/porting-notes.md) for detailed evidence, implementation
notes, and remaining browser-specific work.

## About Lugaru

**Lugaru** is a third-person action game originally created by David Rosen of
Wolfire Games.

The player controls Turner, an anthropomorphic rabbit whose search for those responsible
for the destruction of his village leads into a larger conflict involving the rabbit
republic and neighboring wolves.

Lugaru was open-sourced in 2010, leading to years of community maintenance, ports,
fixes, campaigns, and preservation work.

## Project lineage

Lugaru-Web exists because of several layers of previous work:

1. **Lugaru / Lugaru HD** — originally developed by David Rosen and Wolfire Games.
2. **OSS Lugaru** — community open-source maintenance and cross-platform development.
3. **Loupgarenne** — Ryan Deering's fork, including the WebAssembly work that provided
   the known-good browser baseline used by this project.
4. **Lugaru-Web** — preservation, browser stabilization, restoration, documentation,
   controller support, and continued development maintained by Logan Walker.

The existing Git history is retained so inherited work remains attributed to its
original authors.

Lugaru-Web is an independent community project and is **not affiliated with or endorsed
by Wolfire Games**.

## Documentation

The repository contains extensive documentation for both development and preservation
work:

- [`PROJECT.md`](PROJECT.md) — project scope, principles, status, and development direction
- [`Docs/README.md`](Docs/README.md) — documentation index
- [`Docs/build-provenance.md`](Docs/build-provenance.md) — reproducible Web build environment and dependency provenance
- [`Docs/porting-notes.md`](Docs/porting-notes.md) — browser defects, fixes, runtime evidence, and compatibility notes
- [`Docs/architecture.md`](Docs/architecture.md) — codebase and subsystem architecture
- [`Docs/history-and-lineage.md`](Docs/history-and-lineage.md) — project ancestry and historical research
- [`Docs/preservation-and-modding.md`](Docs/preservation-and-modding.md) — content preservation and modding research
- [`Docs/design-direction.md`](Docs/design-direction.md) — long-term design principles and possible future directions
- [`Docs/research-backlog.md`](Docs/research-backlog.md) — unresolved research and engineering questions

## Building from source

The Web build currently uses **Emscripten 6.0.9**, WebAssembly, SDL2, GL4ES, and WebGL.

For the exact tested browser build environment, dependency revisions, and reproducible
build information, see:

**[`Docs/build-provenance.md`](Docs/build-provenance.md)**

[`COMPILING.md`](COMPILING.md) also contains inherited/native compilation information.

## Licensing

The Lugaru source code is distributed under the **GNU General Public License version 2
or, at your option, any later version (GPLv2+)**.

See:

- [`COPYING.txt`](COPYING.txt) — GPLv2 license text
- [`CONTENT-LICENSE.txt`](CONTENT-LICENSE.txt) — game asset licenses, campaign attribution, and Lugaru-Web content modifications
- [`THIRD-PARTY-NOTICES.txt`](THIRD-PARTY-NOTICES.txt) — notices and licenses for third-party software used by the Web build
- [`AUTHORS`](AUTHORS) — project authorship and lineage

Game assets in `Data/` are distributed under their applicable
**CC BY-SA 3.0** and **CC BY-SA 4.0** licenses as documented in
`CONTENT-LICENSE.txt`.

## Upstream projects

**Loupgarenne:**

- https://github.com/ryandeering/loupgarenne

**OSS Lugaru:**

- https://gitlab.com/osslugaru/lugaru
- https://osslugaru.gitlab.io

**Wolfire Games:**

- http://www.wolfire.com

If you enjoy Lugaru, consider supporting its original developers through the official
commercial release of Lugaru HD.
