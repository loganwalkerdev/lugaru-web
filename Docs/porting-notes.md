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

**Status:** Fixed
**Priority:** High
**Evidence:** Source inspection + browser runtime validation

### Location

```text
Source/User/Settings.cpp
Source/main.cpp
LoadSettings()
DefaultSettings()
Web resolution enumeration
```

### Previous Behavior

The inherited WebAssembly build applied browser-specific settings after normal configuration loading.

The Web-only block in `LoadSettings()` forced behavior equivalent to:

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

Because this occurred after configuration parsing, previously saved VSync and resolution values were overwritten every time settings were loaded.

The Web build also received only one resolution from SDL/Emscripten display-mode enumeration:

```text
1920 x 1080
```

As a result, the resolution option appeared in the menu but could not cycle to another value.

### Resolution

The Web-specific post-load overrides for VSync and resolution were removed.

The Web build still disables motion blur after configuration loading because that rendering feature is unsupported on Web:

```cpp
#ifdef __EMSCRIPTEN__
    // Motion blur is unsupported in the Web build.
    ismotionblur = 0;
    alwaysblur = 0;
#endif
```

Browser-specific values are now treated as defaults rather than forced settings.

`DefaultSettings()` continues to use:

```text
1920 x 1080
```

as the default Web render resolution, but a saved user-selected resolution is no longer overwritten during configuration loading.

In the tested Web build, SDL/Emscripten exposed only the current canvas size through display-mode enumeration, so the Web build now provides a small set of internal 16:9 render resolutions:

```text
1280 x 720
1600 x 900
1920 x 1080
2560 x 1440
```

Native builds continue to populate the resolution list from SDL display modes.

### Validation

Browser runtime testing confirmed:

- VSync can be changed between On and Off,
- the selected VSync setting survives a reload,
- the Resolution option cycles through all four Web render resolutions,
- the selected resolution survives a reload,
- selecting `2560 x 1440` results in a canvas drawing buffer of `2560 x 1440`,
- selecting `1280 x 720` results in a canvas drawing buffer of `1280 x 720`,
- the visible canvas continues filling the browser viewport because `shell.html` scales it independently through CSS.

The Web build therefore retains 1920×1080 as its default while allowing users to choose and persist another supported internal render resolution.

### Impact

Browser configuration now behaves like normal user configuration rather than being silently overwritten during every load.

Saved VSync and resolution preferences are respected, while unsupported motion blur remains disabled intentionally.

---

## B-2 — Browser Main Loop Uses Timeout Scheduling Instead of Animation Frames

**Status:** Fixed
**Priority:** High
**Evidence:** Source inspection + browser runtime validation
**Related:** B-1

### Location

```text
Source/Game.hpp
Source/main.cpp
set_vsync()
emscripten_main_loop()
```

### Previous Behavior

The inherited Web build forced:

```text
disablevsync = 1
```

through B-1, causing:

```cpp
set_vsync(false);
```

to select Emscripten zero-delay timeout scheduling rather than browser animation-frame scheduling.

An additional initialization-order bug caused the Web startup path to execute:

```cpp
g_emscriptenMainLoopRunning = true;
set_vsync(!disablevsync);
emscripten_set_main_loop(emscripten_main_loop, 0, 1);
```

This marked the loop as running and attempted to change its timing before `emscripten_set_main_loop()` had actually created the loop.

Chrome consequently reported:

```text
emscripten_set_main_loop_timing: Cannot set timing mode for main loop since a main loop does not exist! Call emscripten_set_main_loop first to set one up.
```

### Resolution

The Emscripten main loop is now registered without attempting to configure its timing first:

```cpp
emscripten_set_main_loop(emscripten_main_loop, 0, 1);
```

Timing initialization is deferred until the first actual execution of `emscripten_main_loop()`:

```cpp
if (!g_emscriptenMainLoopRunning) {
    g_emscriptenMainLoopRunning = true;
    set_vsync(!disablevsync);
}
```

At that point the Emscripten loop exists, so `set_vsync()` can safely configure its timing.

B-1 was fixed at the same time so the browser no longer forces VSync off after loading configuration.

With VSync enabled, the Web build now uses:

```text
EM_TIMING_RAF
```

and follows the browser's animation-frame scheduling.

If the user explicitly disables VSync, the existing timeout-based scheduling behavior remains available intentionally.

### Validation

After the initialization-order change:

- the previous `emscripten_set_main_loop_timing` startup warning no longer appears,
- the game starts and runs normally,
- VSync can be changed from the settings menu,
- the VSync preference survives browser reloads,
- the default Web configuration can use browser animation-frame scheduling instead of being forced to timeout scheduling.

### Impact

The Web build no longer attempts to configure main-loop timing before the loop exists and is no longer forced into timeout scheduling by its settings loader.

Browser frame scheduling now follows the user's VSync preference, with animation-frame scheduling available as the normal default behavior.

---

## B-3 — Browser Save Persistence Can Silently Discard Progress

**Status:** Fixed
**Priority:** High
**Evidence:** Source inspection and browser runtime testing

### Location

Primary browser-persistence code is located in:

```text
Misc/web-persistence.js
Source/User/Account.cpp
Source/User/Settings.cpp
```

Relevant browser storage behavior includes the IDBFS persistence layer and legacy `localStorage` migration path.

### Current Behavior

Browser persistence uses Emscripten IDBFS backed by IndexedDB.

The IDBFS filesystem is mounted at `/home/web_user` and loaded before the game starts. Files written beneath that mount are persisted automatically.

Legacy browser saves stored in `localStorage` are migrated to IDBFS only when no IndexedDB save already exists. Legacy account data is Base64 decoded during migration, and invalid legacy data is left untouched rather than deleted.

Existing IndexedDB data takes precedence over legacy `localStorage` data.

### Resolution

The previous `localStorage` save/restore implementation was removed.

The replacement persistence layer:

- mounts IDBFS before game startup,
- waits for the initial IndexedDB sync before allowing the game to run,
- automatically persists filesystem writes,
- migrates existing legacy config and account data when appropriate,
- preserves malformed legacy data instead of silently deleting it,
- avoids overwriting an existing IndexedDB save during migration.

Runtime testing confirmed that account progress persists across page reloads and that legacy `localStorage` saves migrate successfully to IndexedDB.

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

**Status:** Fixed
**Priority:** High
**Evidence:** Runtime debugging and source inspection

### Location

```text
Source/Graphic/Sprite.cpp
Sprite::Draw()
Sprite::DeleteSprite(int)
```

### Environment

```text
Google Chrome 153.0.8010.47
Windows 11 25H2
Emscripten 6.0.9
Lugaru Web baseline b340204
```

The crash was reproduced both on the inherited baseline build and after the B-4 animation bounds-check restoration, confirming that B-4 did not introduce the defect.

### Observed Behavior

During normal gameplay, particularly after several maps and combat encounters, the WebAssembly build would eventually terminate with:

```text
Uncaught RuntimeError: memory access out of bounds
```

A debug WebAssembly build using `-g3`, `-O0`, Emscripten assertions, and GL assertions produced a useful symbolic stack trace. The first meaningful game-side failure was:

```text
std::unique_ptr<Sprite>::release(...)
std::unique_ptr<Sprite>::operator=(...)
std::vector<std::unique_ptr<Sprite>>::erase(...)
Sprite::DeleteSprite(int)
Sprite::Draw()
```

Later SDL event errors and stack-overflow messages occurred after the initial failure and were treated as secondary effects rather than the root cause.

### Root Cause

`Sprite::Draw()` iterates forward through the global sprite vector.

Several blood-sprite collision paths called:

```cpp
DeleteSprite(i);
```

while execution was still inside the current sprite's update logic.

`DeleteSprite()` performs:

```cpp
sprites.erase(sprites.begin() + i);
```

Erasing the element shifts subsequent vector elements and invalidates the current element. Some control paths then continued using `sprites[i]`, resulting in invalid or unintended sprite access after the erase.

Source-history inspection showed that this deletion pattern predates the WebAssembly port and is inherited from earlier Lugaru code rather than being introduced by the browser port.

### Resolution

Blood-sprite collision paths now record the collision through `spritehit` without immediately erasing the sprite.

Deletion is deferred until the blood-sprite collision logic has finished:

```cpp
if (spritehit) {
    DeleteSprite(i);
    continue;
}
```

The `continue` prevents any further access to the erased sprite.

The original forward-loop iteration behavior was otherwise preserved.

An earlier experimental fix changed the loop to manually manage its index after deletion. Although it prevented the original memory trap, it caused reproducible gameplay freezes after map loading. That broader behavioral change was rejected in favor of the narrower fix.

### Validation

The fix was tested in a debug WebAssembly build that previously reproduced the crash consistently.

After the change:

- multiple maps and combat encounters completed without reproducing the memory-access trap,
- the earlier experimental freeze was no longer present,
- the normal Release WebAssembly build completed successfully,
- extended gameplay in the Release build also failed to reproduce the crash.

The Release build used the project's normal optimization configuration, including `-Oz`, LTO, Closure compilation, and disabled Emscripten assertions.

A native build sanity check was also attempted. Native compilation was blocked before reaching `Sprite.cpp` by a pre-existing Linux SDL2 CMake include-path problem:

```text
fatal error: SDL.h: No such file or directory
```

That native-build issue is separate from B-12 and does not affect the successful WebAssembly validation.

### Impact

The previously reproducible WebAssembly gameplay crash is no longer observed in either Debug or Release browser builds.

B-12 is considered fixed based on the identified unsafe erase pattern, the matching debug stack trace, and successful repeated runtime validation.

---

## B-13 — Player Direction Changes Snap Instead of Rotating

**Status:** Open
**Priority:** Medium
**Evidence:** External user report against the inherited Web fork; not yet independently reproduced

### Reported Behavior

A user of the inherited Web version reported that reversing the player's movement direction causes the character to snap immediately to the opposite facing direction.

The reported comparison with the original game was:

```text
Original: moving left and then right causes the character to rotate between directions.
Web fork: moving left and then right causes the character to snap immediately to the opposite direction.
```

The reporter also specifically noted playing at refresh rates above 100 Hz.

### Impact

The behavior removes or bypasses a visible part of the original character movement animation and makes direction changes appear abrupt.

If the report is reproducible, this represents a behavioral regression from the original game.

### Investigation Direction

Reproduce the behavior in the current Web build and compare it against a native/original build before assigning a cause.

Relevant areas to investigate include:

- player facing and rotation interpolation,
- movement-direction changes,
- frame-time-dependent rotation logic,
- differences between Wolfire upstream, Loupgarenne main, and the inherited WebAssembly branch.

The reported high-refresh-rate environment should also be tested to determine whether the behavior is frame-rate dependent.

Do not assume the issue is WebAssembly-specific until the behavior and source divergence have been identified.

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
