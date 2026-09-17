# WebAssembly Porting Notes

**Last updated:** 2026-09-17

This document tracks WebAssembly-specific defects, inherited behavior changes, browser compatibility issues, and related implementation notes for Lugaru Web.

`PROJECT.md` contains the high-level issue summary. This file contains the technical detail needed to investigate, test, and resolve each item.

Issue IDs should remain stable once assigned.

---

## Status Definitions

* **Open** — confirmed or strongly supported by available evidence and still unresolved.
* **Investigating** — actively being reproduced, tested, or traced.
* **Fixed** — corrected in the development branch.
* **Deferred** — understood but intentionally postponed.
* **Unable to reproduce** — previously observed but not currently reproducible.

An issue should move to **Investigating** when active work begins rather than jumping directly from Open to Fixed.

## Evidence Definitions

Each issue should identify the strongest evidence currently available:

* **Source inspection** — behavior is supported by code inspection but has not necessarily been reproduced at runtime.
* **Runtime observation** — behavior has been directly observed in a running local build.
* **Source inspection + runtime observation** — both code and observed behavior support the issue.
* **CI observation** — behavior has been directly observed in an automated build or test environment.

Source-derived issues should not be described as runtime-confirmed until they have actually been reproduced.

---

## B-1 — Web Settings Overridden After Configuration Load

**Status:** Open
**Priority:** High
**Evidence:** Source inspection

### Location

```text
Source/User/Settings.cpp
LoadSettings()
```

### Current Behavior

The WebAssembly build applies browser-specific settings after normal configuration loading.

The inherited block includes behavior equivalent to:

```cpp
#ifdef __EMSCRIPTEN__
    ismotionblur = 0;
    alwaysblur = 0;
    disablevsync = 1;
    set_vsync(0);
    kContextWidth = 1920;
    kContextHeight = 1080;
    newscreenwidth = 1920;
    newscreenheight = 1080;
#endif
```

Because this occurs after configuration parsing, previously saved values can be overwritten every time settings are loaded.

### Impact

Potential effects include:

* saved VSync preferences being ignored,
* saved resolution being ignored,
* a hardcoded 1920×1080 resolution being applied regardless of browser canvas or display,
* browser-specific defaults behaving as forced settings rather than initial defaults.

`DefaultSettings()` also sets `disablevsync = 0`, which conflicts with the later Web-specific override.

### Expected Direction

Browser-specific values should be established as defaults where necessary, not reapplied after user configuration has been loaded.

B-1 and B-2 are directly related and should likely be resolved together.

---

## B-2 — Browser Main Loop Uses Timeout Scheduling Instead of Animation Frames

**Status:** Open
**Priority:** High
**Evidence:** Source inspection
**Related:** B-1

### Location

```text
Source/main.cpp
set_vsync()
WebAssembly main-loop timing configuration
```

### Current Behavior

B-1 forces:

```text
disablevsync = 1
```

which results in:

```text
set_vsync(0)
```

and ultimately causes Emscripten to use zero-delay timeout scheduling rather than browser animation-frame scheduling.

### Impact

Possible effects include:

* unnecessary CPU usage,
* increased battery consumption,
* poor frame pacing,
* timing behavior that does not follow the browser's refresh cycle.

### Expected Direction

Use Emscripten animation-frame scheduling where appropriate.

Settings handling must be corrected at the same time so the main-loop fix is not immediately overridden by `LoadSettings()`.

---

## B-3 — Browser Save Persistence Can Silently Discard Progress

**Status:** Open
**Priority:** High
**Evidence:** Source inspection

### Location

Primary browser-persistence code is located in:

```text
Source/Utils/Folders.cpp
Source/User/Account.cpp
Source/User/Settings.cpp
```

Relevant browser storage behavior includes the `localStorage` save/restore glue, account save operations, and settings save operations.

### Current Behavior

Browser persistence currently uses JavaScript `localStorage`.

Binary account data is Base64 encoded before storage.

The restore path performs a sanity check on the stored account data. When that validation fails, the `lugaru_users` entry can be removed from `localStorage`.

Persistence is primarily triggered through:

```text
Account::saveFile()
SaveSettings()
```

### Impact

A failed validation can silently destroy the browser's stored progress without presenting an error or recovery option.

Additional limitations include:

* browser storage limits,
* Base64 storage overhead,
* synchronous storage access,
* persistence tied to specific save calls,
* loss of changes if the browser closes before a relevant save operation,
* limited scalability if additional browser content is introduced.

### Expected Direction

Investigate migration to a browser-backed filesystem such as Emscripten IDBFS or another suitable persistent-storage approach.

Existing browser saves should be preserved or migrated where practical.

Before replacing the persistence system, document the current data format, sanity checks, save triggers, and failure conditions.

---

## B-4 — Animation access lost bounds checking

**Status:** Fixed
**Priority:** High
**Evidence:** Source inspection + native build + WebAssembly build + browser runtime comparison

**Location:** `Source/Objects/Person.hpp`

**Behavior:**
The inherited WebAssembly commit replaced checked `std::vector::at()` access with unchecked `operator[]` access in `currentFrame()` and `targetFrame()`.

**Resolution:**
Restored `.at()` for both animation and frame lookup, matching upstream checked-access semantics.

**Verification:**
- Native Release build completed successfully with GCC 15.2.0.
- WebAssembly Release build completed successfully with Emscripten 6.0.9.
- Browser gameplay remained functional with the change applied.
- A later `RuntimeError: memory access out of bounds` was reproduced on the untouched baseline with this patch stashed, confirming that crash is not introduced by B-4.
- Native runtime interaction could not be fully smoke-tested because the WSLg window would not accept input/focus.

**Introduced by:** inherited WebAssembly commit `b340204`

---

## B-5 — Asset Filename Case Fallback Is Incomplete

**Status:** Open
**Priority:** Medium
**Evidence:** Source inspection

### Location

The case-insensitive fallback is implemented around:

```text
Source/Utils/Folders.cpp
findCaseInsensitiveFile()
openMandatoryFile()
```

Other loading paths requiring review include:

```text
file_exists()
load_image()
Campaign.cpp ifstream usage
Dialog.cpp ifstream usage
LoadJsonLevel ifstream usage
```

### Current Behavior

The Web port contains a case-insensitive file lookup fallback through:

```text
findCaseInsensitiveFile()
```

However, that fallback is currently associated with `openMandatoryFile()` and is not consistently used by every asset-loading path.

### Impact

Linux and browser builds use case-sensitive filesystem behavior.

Filename/reference mismatches that may go unnoticed on case-insensitive filesystems can result in missing assets or failed content loads.

Community campaigns such as Temple, Empire, and Seven Tasks are useful candidates for testing because they contain additional independently authored content.

### Expected Direction

First identify actual filename mismatches.

Long-term, prefer correcting or normalizing filenames and references rather than relying indefinitely on runtime directory scanning.

---

## B-6 — Motion Blur Option Is Exposed but Unsupported on Web

**Status:** Open
**Priority:** Low
**Evidence:** Source inspection

### Location

Relevant settings and menu handling:

```text
Source/User/Settings.cpp
Source/Menu/Menu.cpp
Web-specific rendering conditionals
```

### Current Behavior

Motion blur is disabled for the Web build, while the corresponding option appears to remain available through the settings interface.

This has not yet been deliberately confirmed in browser QA.

### Impact

If confirmed, the UI exposes a control that has no effect.

### Reproduction Steps

1. Open the browser options menu.
2. Determine whether the motion blur option is visible.
3. Determine whether the option can be changed.
4. Determine whether changing it has any observable effect.

### Expected Direction

If the option is confirmed to be nonfunctional on Web, either support it or hide/disable it in Web builds.

---

## B-7 — Browser Key Rebinding Relies on SDL Thread Creation

**Status:** Open
**Priority:** Medium
**Evidence:** Source inspection

### Location

```text
Source/Menu/Menu.cpp
setKeySelected()
setKeySelected_thread()
```

### Current Behavior

Key rebinding uses an SDL thread to wait for the next selected input.

The WebAssembly build does not currently enable pthread support.

If thread creation fails, the waiting state is cleared and the binding operation can silently end without useful feedback.

This failure mode has not yet been deliberately reproduced in browser QA.

### Impact

Key rebinding may appear to do nothing in the browser.

### Reproduction Steps

1. Attempt to rebind a keyboard control.
2. Record whether the UI enters a waiting state.
3. Press a replacement key.
4. Determine whether the new key is captured.
5. Determine whether the binding persists.
6. Repeat with several controls.
7. Record whether failure produces any visible error.

### Expected Direction

Replace thread-based waiting with an event-driven input state suitable for both browser and native builds where practical.

---

## B-8 — Controller Support Is Not Implemented in the Current Browser Build

**Status:** Open
**Priority:** Medium
**Evidence:** Source inspection

### Location

Relevant initialization and input handling include:

```text
Source/main.cpp
Source/Input.cpp
```

The inherited build initializes SDL video but does not currently initialize or integrate SDL GameController input into the existing input system.

### Current Behavior

Controller/gamepad input is not currently implemented in the browser build.

### Reference

An existing open-source Lugaru implementation may provide a useful starting point:

```text
usineur/lugaru
commit 9af3344
```

That work reportedly integrates controller buttons into the existing key state arrays with a relatively small change set.

### Expected Direction

Review the referenced implementation rather than copying it blindly.

If suitable, adapt controller support while preserving existing keyboard input behavior and test it through SDL2's browser Gamepad API mapping.

---

## B-9 — WebAssembly Build Is Optimized Primarily for Size

**Status:** Open
**Priority:** Low
**Evidence:** Source inspection

### Location

```text
CMakeLists.txt
WebAssembly compile and link options
```

### Current Behavior

The browser build uses size-oriented optimization including:

```text
-Oz
-flto
Closure optimization
```

The generated `.wasm` binary is relatively small compared with the packaged game data.

### Expected Direction

Benchmark before changing anything.

Useful comparisons may include:

```text
-Oz
-O2
-O3
```

Record at minimum:

* WASM size,
* JS size,
* startup time,
* representative frame-time behavior.

Do not change optimization settings based on assumptions alone.

---

## B-10 — Synchronous Level Loading Blocks the Browser Main Thread

**Status:** Open
**Priority:** Low
**Evidence:** Source inspection

### Location

Relevant areas include:

```text
CMakeLists.txt
WebAssembly link configuration
level-loading code paths
loading-screen rendering paths
```

The current build configuration does not enable Asyncify.

### Current Behavior

Level loading remains synchronous.

The build does not currently use Asyncify or another asynchronous loading architecture for these operations.

### Impact

Long-running loading operations can temporarily prevent the browser from presenting new frames or responding normally.

Existing loading-screen rendering may not visibly update until the blocking work completes.

### Expected Direction

Acceptable during initial stabilization unless testing shows severe stalls.

Revisit only after higher-priority correctness and persistence issues are resolved.

---

## B-11 — Account Creation Behavior Was Changed Globally

**Status:** Open
**Priority:** Medium
**Evidence:** Source inspection
**Introduced by:** inherited WebAssembly commit `b340204`

### Location

```text
Source/User/Account.cpp
Account::active()
```

### Current Behavior

Inherited changes cause a default `"Player"` account to be created automatically when no account exists.

This change is not restricted to WebAssembly builds.

### Impact

Native Lugaru behavior is changed as a side effect of the Web port.

The behavior may be appropriate for a browser build, but the current implementation applies it globally.

### Expected Direction

Compare against the original native behavior.

If automatic first-run account creation is needed only for Web builds, isolate that behavior appropriately rather than altering shared behavior without intent.

---

## B-12 — WebAssembly Runtime Crash: Memory Access Out of Bounds

**Status:** Open
**Priority:** High
**Evidence:** Runtime observation

### Location

Areas likely relevant to investigation include:

```text
Source/Objects/Person.cpp
Source/GameTick.cpp
Source/GameDraw.cpp
Source/Input.cpp
general WebAssembly runtime / animation / gameplay code paths
```

The exact failing source location is not yet known.

### Environment

```text
Google Chrome 153.0.8010.47
Windows 11 25H2
Lugaru Web baseline b340204
```

The same crash was observed both:

* on the inherited baseline build using unchecked `[]` accessors, and
* after restoring checked `.at()` accessors for B-4.

This indicates the crash is **not introduced by B-4**.

### Observed Behavior

During gameplay, the browser eventually crashes with:

```text
Uncaught RuntimeError: memory access out of bounds
```

Observed console traces include WebAssembly frames such as:

```text
lugaru.wasm:0x53fc4
lugaru.wasm:0xb10af
lugaru.wasm:0xe722d
lugaru.wasm:0x148f4a
```

Additional traces observed during later crashes included functions such as:

```text
$func789
$func532
$func650
```

The crash occurs inside the WebAssembly module rather than as a surfaced C++ `std::out_of_range` exception.

### Impact

This is a high-severity runtime stability issue in the project's target platform.

Because it reproduces during real gameplay and terminates execution, it blocks confidence in browser playability even when normal startup, rendering, and early gameplay appear to work.

### Reproduction Steps

1. Build and serve the WebAssembly version normally.
2. Open the game in Chrome.
3. Start or load gameplay.
4. Play for a period of time, including normal movement, combat, and gameplay actions.
5. Observe whether the browser console eventually reports:

```text
Uncaught RuntimeError: memory access out of bounds
```

If reproduced again, record:

* what level or mode was active,
* what actions were being performed immediately before the crash,
* whether it occurred during combat, animation transitions, loading, or interaction,
* the full browser console trace.

### Current Interpretation

This appears to be a pre-existing WebAssembly/runtime bug rather than a regression caused by the B-4 bounds-check restoration.

The fact that the same memory trap occurs on the untouched baseline suggests an invalid memory access elsewhere in the codebase.

### Expected Direction

Investigate this as an independent runtime defect.

Useful next steps may include:

* reproducing it from a fresh load with a narrower scenario,
* determining whether it correlates with specific combat or animation states,
* comparing behavior before and after suspicious gameplay actions,
* using browser devtools and debug-friendly builds to narrow the failing code path,
* checking whether a native build can reproduce related instability.

Do not treat this issue as resolved by reverting B-4 unless direct evidence shows the restored bounds checks are responsible.

---

## CI-1 — macOS CI Fails During Dependency Setup

**Status:** Open
**Priority:** Low
**Evidence:** CI observation

### Location

```text
GitHub Actions macOS workflow
Homebrew dependency installation step
```

### Observed Behavior

The inherited macOS workflow attempts to install an x86_64 Homebrew environment on an Apple Silicon runner.

The workflow fails before Lugaru source compilation begins.

Observed error:

```text
Homebrew on macOS is only supported on Apple Silicon processors!
```

### Impact

macOS CI does not currently provide a usable native-build reference.

There is no current evidence from this failure that the Lugaru source itself fails to compile on macOS.

### Expected Direction

Update the workflow to use an architecture-compatible dependency setup or an appropriate runner.

Native CI restoration remains lower priority than browser correctness issues.

---

## Adding New Issues

When a new porting issue is confirmed, add a stable ID and include:

```text
ID
Summary
Status
Priority
Evidence
Relevant source location
Observed or inferred behavior
Impact
Reproduction steps when applicable
Expected direction
Introduced-by commit when known
Resolution commit once fixed
```

Avoid creating issue entries for speculative concerns without either source evidence or an observable symptom.

Do not describe source-inferred behavior as runtime-confirmed until it has been reproduced.

---

## Resolving Issues

When an issue is fixed:

1. Change its status to **Fixed**.
2. Record the fixing commit.
3. Describe the final behavior briefly.
4. Preserve the original symptom and investigation notes.
5. Update the high-level issue table in `PROJECT.md` if its status changes.

When an issue originates from a known inherited commit, preserve that provenance after resolution.

This file should preserve the reasoning behind fixes rather than only showing the current state.
