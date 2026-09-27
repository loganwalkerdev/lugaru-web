# WebAssembly Porting Notes

**Last updated:** 2026-09-24

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

### Implementation Notes

Several details of the Web persistence implementation are intentional and should be preserved unless the surrounding build configuration is changed.

- `Module['preRun']` uses bracket notation intentionally. With the current `--closure 1` build, using `Module.preRun` allowed Closure Compiler to rename the property while the Emscripten runtime continued looking for `preRun`, preventing the persistence startup hook from running.
- `Misc/web-persistence.js` is registered as a `LINK_DEPENDS` dependency of the `lugaru` target. Without this dependency, editing the pre-JS file did not reliably cause CMake to relink the generated Web application.
- The initial `FS.syncfs(true)` is protected by an Emscripten run dependency. This ensures that IndexedDB contents are loaded into the mounted filesystem before the C++ game startup code accesses configuration or account files.
- Existing IndexedDB files take precedence over legacy `localStorage` data. Migration only occurs when the corresponding IDBFS file does not already exist.
- Invalid or malformed legacy account data is intentionally left untouched. The previous implementation could delete the stored value after failed validation, which made recovery impossible.

### Validation

The replacement persistence system was manually tested using isolated browser origins.

Testing confirmed that:

- account progress stored through IDBFS survives a page reload,
- an existing IndexedDB save is not overwritten by legacy `localStorage`,
- a valid legacy `localStorage` save migrates successfully when no IndexedDB save exists,
- migrated account data loads normally after migration,
- malformed legacy data is not silently deleted,
- the persistence initialization completes before normal game startup.

Disposable localhost ports were used during migration testing so the primary development origin and its existing browser data were not modified.

### Future Save UX

B-3 addresses the browser persistence backend and destructive legacy-save behavior. It does not change the game's higher-level save cadence.

Account state is still written according to the game's existing save triggers rather than after every relevant player action. During testing, creating an account alone was not sufficient to persist it; normal gameplay later triggered `Account::saveFile()`.

This behavior is currently acceptable, but save timing should be revisited during a future browser UI/UX pass. Possible improvements include saving at additional safe transition points or exposing clearer save-state feedback without changing the underlying IDBFS persistence model.

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

## B-7 — Browser Key Rebinding Failed at SDL Thread Creation

**Status:** Fixed
**Priority:** Medium
**Evidence:** VERIFIED SOURCE FINDING + RUNTIME-VERIFIED (Chrome, 2026-09-24)
**Fix:** `a7bfd51` — `fix: restore web key rebinding`
**Verified:** 2026-09-24

### Root Cause and Previous Behavior

`Source/Menu/Menu.cpp::setKeySelected()` attempted `SDL_CreateThread()` with the Web build's non-threaded SDL backend. Chrome reproduced the error `Unable to create thread: Threads are not supported on this platform`. Failure cleared `waiting` but left `keyselect` set: the control displayed `_`, replacement keys did nothing, and another control could not be selected. Escape behaved normally; refreshing restored the original binding display, so the failed capture state was not persisted.

### Resolution

Web-only nonblocking SDL capture replaces thread creation, with menu/input/main-loop guards against reusing captured input for navigation or hotkeys. Keyboard scancodes, mouse-button encoding, control ordering, Escape cancellation, and settings/save behavior are preserved. Capture clears both `waiting` and `keyselect`; native threaded capture is intentionally unchanged. Pthreads and Asyncify remain disabled.

### Validation

The Web target built successfully after retrying an Emscripten cache-lock sandbox failure with cache access; the successful build log contained no compiler/linker warnings.

Chrome runtime testing confirmed normal capture, disappearance of the thread error, keyboard and mouse rebinding, clean Escape cancellation, subsequent control selection, rebound actions in gameplay, and persistence after leaving Options and reloading. Held-hotkey, focus-transition, and browser-reserved-key coverage remains unverified.

Native SDL event-thread portability remains a separate open question in research backlog A4a. Historical origin and investigation details are recorded in research-session section V.

---

## B-8 — Controller Gameplay, Menu/UI, and Tutorial Support

**Status:** Milestones 1–5 implemented, Web-built, and physically runtime-verified (Firefox + wired DualSense, DS4Windows disabled). Tutorial additions are source/automated-validated; physical verification is pending. Compatibility, tuning, and scope exclusions remain open.
**Priority:** Medium
**Evidence:** Committed source, reported build/check results, isolated native SDL harness, and manual physical-controller runtime testing

### Implementation — Milestone 1 (2026-09-25)

**VERIFIED SOURCE FINDING:** Commit `e5939fe` (`feat: add controller input foundation`) on `investigate/controller-support` adds the foundation in `Source/Utils/Input.cpp`, `Source/Utils/Input.hpp`, and `Source/main.cpp`.

- `Input::InitController()` initializes `SDL_INIT_GAMECONTROLLER` and immediately checks for recognized controllers already available at startup.
- `Input::UpdateController()` calls `SDL_GameControllerUpdate()` and, while no controller is active, retries discovery at one-second intervals. One `SDL_GameController*` is active at a time.
- Device indices are used during enumeration/opening only. The active device is retained by its handle and `controllerInstance` (`SDL_JoystickID`), with attachment checked through `SDL_GameControllerGetAttached()`. Reconnection enumerates again rather than relying on a stale device index.
- Disconnect closes the handle and calls `clearControllerState()` to reset cached axes/buttons. `Input::ShutdownController()` closes the device, clears state, and releases the subsystem before `SDL_Quit()`.
- `Input::hasController()`, `Input::controllerAxis()`, and `Input::controllerButtonDown()` expose standardized SDL axes/buttons through the existing input area. Raw controller state remains separate from `keyDown`, `keyPressed`, and keyboard/mouse bindings.
- `SetUp()`, `DoUpdate()`, and `CleanUp()` own the lifecycle calls. No additional SDL event-queue consumer was introduced.

At milestone 1, no controller gameplay, camera, menu, or rebinding behavior was mapped. Subsequent milestones below add gameplay, look, UI, and tutorial support while preserving keyboard/mouse input; this remains narrower than a generalized input-system rewrite.

### Milestone 1 build and isolated harness validation

**Build validation (reported for the source milestone):** `cmake --build build-web -j2` succeeded, and `git diff --check` passed before the source commit.

**RUNTIME-VERIFIED — isolated native SDL virtual-controller harness:** no-controller startup, startup discovery, hot-plug, button/axis state reads, device-index changes, disconnect/reset, reconnect, and shutdown. This was not a full native Lugaru gameplay test.

### Milestone 1 physical runtime verification

**RUNTIME-VERIFIED — manual test:** Sony DualSense / PS5 controller, Windows host, Firefox, Lugaru-Web served from localhost, DS4Windows disabled, wired USB connection. Lugaru logged:

```text
Controller: detected device 0 (Standard Gamepad)
Controller: opened instance 0 (Standard Gamepad)
```

- Left-stick analog values were received across the SDL signed axis range, including intermediate values and small near-center values. The observed center noise informed the movement and look deadzones added below.
- Right-stick `rightx` and `righty` produced analog values in both positive and negative ranges.
- `lefttrigger` and `righttrigger` were observed as binary Web-axis values (`0` and `32767`), consistent with the previously identified bundled SDL Emscripten backend behavior; analog trigger precision is not established by this test.
- Shoulder and stick-click buttons produced clean down/up events. Additional controller buttons were exercised, without claiming exhaustive button coverage.
- Physical disconnect and reconnect were detected; the controller reopened and resumed reporting input. No stuck controller state was observed.
- Existing mouse controls still worked. At milestone 1, controller input remained intentionally unmapped to gameplay/menu behavior.

### Milestone 2 — Digital Classic movement (`4b8af1d`)

**VERIFIED SOURCE FINDING:** `Input::controllerMovement()` in `Source/Utils/Input.cpp` exposes left-stick directions using independent axis hysteresis: activate at magnitude `10000`, release at `7000`. `clearControllerState()` clears the direction latches on disconnect/reset. In `Game::Tick()` (`Source/GameTick.cpp`), controller directions are ORed with keyboard bindings into `Person::players[0]->forwardkeydown`, `backkeydown`, `leftkeydown`, and `rightkeydown`, under existing gameplay/dialogue guards.

Existing Classic speed, facing, animation, and diagonal logic remain authoritative. There is no proportional analog movement: the stick selects the existing digital/eight-direction combinations. Snapping movement/facing into those combinations is expected, not a milestone 2 defect. True analog movement/arbitrary facing is separate future work because it affects animation, facing, combat, and timing.

**Build validation:** `cmake --build build-web -j2` and `git diff --check` passed.

**RUNTIME-VERIFIED — isolated native SDL harness:** thresholds, hysteresis, diagonals, reversals, keyboard-state preservation, disconnect/reconnect, and lifecycle handling passed. This is not full native Lugaru gameplay validation.

**RUNTIME-VERIFIED — physical manual test:** wired DualSense, Firefox, DS4Windows disabled. Movement worked correctly: center noise caused no unintended movement; threshold/hysteresis start and stop behavior and diagonals worked; keyboard/controller movement coexisted; disconnect/reconnect left no stale movement. Mouse controls remained functional.

### Milestone 3 — Core gameplay actions (`2387ebf`)

**VERIFIED SOURCE FINDING:** fixed mappings in `Source/Utils/Input.cpp`:

| Physical control | SDL GameController button | Existing action |
| --- | --- | --- |
| Cross / bottom face | `SDL_CONTROLLER_BUTTON_A` | Jump |
| Circle / right face | `SDL_CONTROLLER_BUTTON_B` | Crouch |
| Square / left face | `SDL_CONTROLLER_BUTTON_X` | Draw / sheath |
| L1 | `SDL_CONTROLLER_BUTTON_LEFTSHOULDER` | Throw / pickup / contextual weapon interaction |
| R1 | `SDL_CONTROLLER_BUTTON_RIGHTSHOULDER` | Attack |

`isJumpDown()`, `isCrouchDown()`, `isDrawDown()`, `isThrowDown()`, and `isAttackDown()` combine controller and existing keyboard/mouse held state. Releasing either source does not release an action still held by the other. Existing jump/crouch transitions and draw/throw toggle logic remain authoritative, including contextual sneak/roll/reversal and weapon behavior.

`doAttacks()` consumes combined held attack while retaining `oldattackkey` logic. `Input::Tick()` derives `isAttackPressed()` from the combined transition for one input/physics tick. Both `Dialog::tick()` in `Source/Level/Dialog.cpp` (initiation) and `Game::Tick()` (advancement) consume this press edge, rather than relying only on `Person::attackkeydown`. Jump-based respawn uses combined jump input. Disconnect removes controller-derived action state without clearing keyboard/mouse state; combined attack history stays separate from controller lifecycle so a keyboard-held attack does not acquire a false new edge. Raw developer/text/key handling remains separate. Developer dialogue-camera controls remain keyboard-only. Challenge-completion confirmation was keyboard-only at milestone 3; milestone 5 adds controller continue/back through the existing result handlers.

**Build validation:** Web build and `git diff --check` passed.

**RUNTIME-VERIFIED — isolated SDL harness:** all five mappings, keyboard/controller handoffs, one-tick attack edges, raw-input isolation, unmapped controls, and disconnect/reconnect passed.

**RUNTIME-VERIFIED — physical Firefox/DualSense test:** all five mappings worked: Cross jump, Circle crouch and related existing contexts, Square draw/sheath, L1 throw/pickup, R1 combat and dialogue interaction/advancement. Held attack did not cause unintended repeated dialogue advancement. Keyboard/mouse and controller actions coexisted, and disconnect/reconnect left no stale action state.

At milestone 3, right stick, Triangle, triggers, Options/Start, D-pad, and other unused controls remained unmapped. Milestone 4 adds right-stick look; milestone 5 adds Start/D-pad UI behavior; the tutorial follow-up maps Triangle only to tutorial advance. Triggers remain unmapped. The face-button layout is intentional and does not copy the historical Switch layout.

### Milestone 4 — Right-stick camera/look (`ab46b53`)

**VERIFIED SOURCE FINDING:** `Input::controllerLook()` uses `controllerLookAxis()` with an axial deadzone of `8000` (approximately 24%). Remaining travel is continuously rescaled from zero to full response, avoiding a speed jump at the boundary. `Game::TickOnce()` adds the resulting angular rate alongside mouse input, outside menus and dialogue/directing, before the existing pitch clamp:

- Maximum yaw rate: `120°/s`; maximum pitch rate: `90°/s`.
- Right increases yaw; down increases pitch, matching non-inverted mouse behavior.
- Pitch limits remain `[-70°, 90°]`.
- Controller scaling is independent of mouse sensitivity and inversion. Mouse and stick look combine without exclusive device selection.
- Attachment-checked axis reads remove controller-derived look on disconnect; centered reconnect has no retained look state. Triggers are not used for camera rotation.

**VERIFIED SOURCE FINDING — timing:** stick input represents angular velocity, not mouse displacement. The implementation integrates `look rate × realmultiplier` once per outer frame in `TickOnce()`, before physics substeps. `DoUpdate()` in `Source/main.cpp` sets `realmultiplier` from elapsed frame seconds before game-speed, difficulty, loading, and slow-motion modifiers. Sustained deflection therefore yields approximately the same angular rotation per real-world second across render rates. The existing 250 ms stall cap still applies; long stalls are not integrated as unrestricted elapsed time. A fixed increment per rendered frame would be refresh-rate dependent in this loop.

**Build validation:** Web build and `git diff --check` passed. The Web milestones required Emscripten cache access for linking; existing unused-variable warnings in `main.cpp` and `GameDraw.cpp` were not controller failures.

**RUNTIME-VERIFIED — isolated timing/camera harness:** 30, 60, 120, 144, and 240 Hz plus irregular timing; deadzone, signs, pitch limits, mouse combination/inversion, camera guards, disconnect/reconnect, and unmapped triggers passed. Milestone 3 action harness regression checks also passed. These are isolated checks, not physical browser measurements across those refresh rates or full native gameplay tests.

**RUNTIME-VERIFIED — physical Firefox/DualSense test:** camera control worked and felt mechanically correct. Centered stick and inside-deadzone movement caused no drift; horizontal/vertical look and diagonal/circular motion were smooth; pitch limits held. Mouse look remained functional and coexisted with controller look. Disconnect/reconnect left no stale camera movement. Left-stick movement and all milestone 3 buttons still worked; triggers and other intentionally unmapped controls had no gameplay effect.

**OPEN QUESTION — tuning:** physical testing found camera sensitivity somewhat slow for preference, not mechanically incorrect. Values remain `120°/s` yaw and `90°/s` pitch. Future tuning and controller-specific sensitivity configuration remain open.

### Milestone 5 — Pause and virtual-cursor UI (`8d549e4`)

**VERIFIED SOURCE FINDING:** controller UI reuses the existing pointer-oriented menu in `Source/Menu/Menu.cpp`, with no discrete-focus system or controller-specific campaign navigation:

- Options / `SDL_CONTROLLER_BUTTON_START` joins the existing pause/resume transitions in `Game::ProcessInput()`, including music/audio behavior. It opens Pause from gameplay and resumes from the actual pause menu (`mainmenu == 2`); it is not a universal Back button in nested menus.
- Cross / SDL A confirms the item selected by existing cursor hit-testing. The menu-local press joins the left-click activation dispatcher without changing `Input::MouseClicked()`, synthesizing SDL mouse events, or overwriting raw mouse buttons.
- Circle / SDL B chooses the existing contextual Back/Cancel/No/Resume action: Pause resumes; Options saves on Back; Controls returns to Options; Stereo applies its pending mode on Back; Delete chooses No; Accounts, Challenges, and campaign ending use their existing destinations. Circle does nothing at the root without an active game or on Difficulty (screen 8).
- Campaign cycling and world-map marker activation retain existing availability/progress checks and loading behavior. Challenge results separately accept Cross for existing continue and Circle for existing back behavior, preserving bookkeeping and destination handling.
- Square, Triangle, shoulders, triggers, and right stick have no menu action. Stereo decrement remains mouse-only. Controller confirm cannot start key rebinding. Account names require keyboard text entry; Circle can cancel the existing name-entry path and Web key capture. Native threaded key capture is unchanged.

`Input::MoveControllerMenuCursor(realmultiplier)` runs in `DoUpdate()` after `DoMouse()`, once per outer frame rather than once per `Menu::Tick()`. It uses the existing continuous `8000` axial-deadzone response on the left stick, with maximum speed **320 logical pixels/second**. D-pad takes precedence for precision motion at **80 logical pixels/second**; diagonals have bounded speed. The logical **640×480** motion is scaled into the existing shared `mousecoordh`/`mousecoordv`, retaining fractional motion and clamping to screen bounds. Mouse and controller therefore move the same visible cursor. Elapsed-time integration provides approximately consistent speed across render rates, subject to the existing stall cap.

Controller press edges and `syncControllerContext()` consume held buttons through release across menu/gameplay/modal transitions. Cross used for Resume cannot immediately Jump, Circle cannot become Crouch, and held gameplay buttons cannot activate a newly entered UI context. Disconnect clears cached controller input, pending UI presses, suppression state, and cursor remainder without clearing keyboard/mouse state; centered/released reconnect has no stale movement or activation.

**Build validation:** Web build and `git diff --check` passed.

**RUNTIME-VERIFIED — isolated SDL/input harnesses:** cursor scaling, deadzone, 30/60/120/144/240 Hz and irregular timing, D-pad precision/diagonals, boundary clamping and slow movement away from edges; confirm edges; contextual Back destinations; pause/audio transitions; challenge results; modal cancellation; release suppression; raw-input isolation; disconnect/reconnect. Gameplay-action handoff and held-button regression checks also passed. These are not full native gameplay tests or physical browser measurements at those refresh rates.

**RUNTIME-VERIFIED — supplied physical QA:** Firefox, wired DualSense, DS4Windows disabled. Milestone 5 QA passed: pause/resume, virtual cursor, confirmation/contextual Back, campaign switching, world-map interaction, and challenge/menu flows. Earlier movement, actions/dialogue, camera, simultaneous keyboard/mouse use, and disconnect/reconnect remain physically verified. This report does not extend physical verification to the subsequent tutorial changes.

### Tutorial compatibility — source and automated validation; physical QA pending

**VERIFIED SOURCE FINDING:** `bd7b207` (`feat: add controller tutorial advance`) adds `Input::controllerTutorialAdvancePressed()` for Triangle / `SDL_CONTROLLER_BUTTON_Y`. It reuses controller press edges, transition suppression, and lifecycle reset, restricted to active tutorial gameplay outside menus, modal/text capture, console, freeze, and results. It joins the existing Tab handler in `Game::ProcessInput()` without global Tab emulation: if stage is not `51`, set `Tutorial::stagetime = Tutorial::maxtime`; preserve the existing sound. Stage 51 remains unskippable, and keyboard Tab semantics are unchanged.

**VERIFIED SOURCE FINDING:** `f00ec72` (`fix: recognize controller look in tutorial`) changes only stage-3 recognition in `Source/Tutorial.cpp`. The existing `deltah || deltav` mouse criterion is ORed with meaningful X or Y from `Input::controllerLook()`, the same post-deadzone signal used by the gameplay camera. There is no new Input helper, second deadzone calculation, synthetic mouse input, or write to mouse deltas. Existing tutorial-call guards exclude menus/pause, console freeze, and results; controller recognition additionally requires active gameplay, no input waiting, and no dialogue/directing. Stage timing, success accumulation, setup, and camera tuning are unchanged.

**Build validation / RUNTIME-VERIFIED — isolated harnesses only:** both changes passed the Web build and `git diff --check`. Triangle tests cover press/hold/release, stage 51 and sound, context guards, held entry/pause/resume, disconnect/reconnect, unchanged Tab and simultaneous input. Stage-3 tests cover mouse, horizontal/vertical stick input, deadzone boundaries, context exclusions, and clean reconnect. Camera timing/response and Triangle/menu regression checks passed. **OPEN QUESTION — physical verification:** neither tutorial addition has yet been reported as physically verified in Firefox/DualSense. Tutorial prompts remain keyboard-oriented.

### Browser observations and limits

**RUNTIME-VERIFIED:** Firefox exposed the wired DualSense through the standard Gamepad API; Emscripten/SDL exposed it to Lugaru as `Standard Gamepad`. Fullscreen is not required for controller input once the browser has exposed the device to the page.

**OPEN QUESTION — Bluetooth:** raw Bluetooth DualSense support is not verified. Exposure was inconsistent/not successfully established during the final runtime test.

**RUNTIME-VERIFIED — Chrome, updated 2026-09-27:** earlier tests on this development machine repeatedly returned null `navigator.getGamepads()` entries, including after renderer focus/fullscreen testing. The user now reports working wired DualSense input in current Chrome with the final character-restored Web build. The cause of the discrepancy is unknown: character restoration changed no controller/browser-input code, so no causal link is established. Firefox/wired DualSense remains the most extensively validated setup. This one Chrome result does not establish Bluetooth, other Chromium versions, other operating systems, or other controllers. No WebHID or Chrome-specific workaround was added.

### Historical context and remaining scope

**HISTORICAL EVIDENCE — prior research:** original/local Lugaru ancestry did not reveal inherited controller support. The 2019 `usineur/lugaru` Switch implementation (reference commit `9af3344`) remains useful historical reference. Its SDL GameController and left-stick digital movement approaches informed the preservation-safe direction; startup-only assumptions, its exact face-button mapping, and trigger-assisted camera rotation were not copied. Its fixed per-frame camera increments are unsuitable for the current Web loop's frame-rate-independent stick integration.

**HISTORICAL EVIDENCE — prior menu investigation:** the 2019 fork's `9af3344` added controller Start to pause handling; `7b81f2f` used cursor emulation and an A-as-mouse-click hack. Its fixed-per-frame cursor/camera increments and trigger-assisted motion are historical behavior, not the timing or binding contract for Lugaru-Web. Current UI reuses the pointer model but uses elapsed-time cursor integration, menu-local confirm, and contextual Back. The inspected historical tutorial handler remained Tab-only. The old face-button layout is reference material, not a requirement for the current mappings.

**RUNTIME-VERIFIED — progression:** the earlier observation that mouse clicking changes campaigns motivated pointer-path reuse. Milestone 5 physical QA now verifies controller campaign switching and world-map interaction through that same UI.

**OPEN QUESTION / remaining work:** physical tutorial QA; broader Chrome/Bluetooth compatibility; camera sensitivity and movement-threshold feel tuning; controller settings/inversion and glyphs/prompts; keyboard-only account naming and controller rebinding. True analog locomotion is a separate design problem; multiple-controller ownership, rumble/advanced DualSense features, trigger mappings, stereo decrement from controller, and WebHID remain outside implemented scope. NPC steering and shared character turning have now been investigated; residual slope/seam scheduler behavior remains open separately in B-13. See [backlog A6](research-backlog.md#a6-controller-support-completed-scope-and-remaining-work) for classification and follow-up priorities.

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

**Status:** Fixed — finalized default source and physical QA; manual commit pending
**Priority:** Medium
**Evidence:** VERIFIED SOURCE FINDING / VERIFIED GIT HISTORY / RUNTIME-VERIFIED

### Original symptom and provenance

The inherited Web fork was reported to snap on direction reversals instead of visibly turning, including a report above 100 Hz. Later physical comparisons found native OSS 1.2 and current OSS `42f148d` smooth, while Web remained rough after yaw alone was restored. Classic eight-direction movement intent is distinct from this regression in executing/presenting turns.

**VERIFIED GIT HISTORY:** Ryan commit `1db27611eec7fe4c3f4019e233235e3a80fb2460` replaced bounded yaw and tilt with exponential damping using full-frame `frameDeltaTime` inside repeated physics subdivisions. It also changed head normalization/convergence/application and guarded draw-time `DoHead()` updates with `isFirstPhysicsTickOfFrame`. Normal physics frames clear that flag before drawing, skipping target conversion and convergence. Zero-tick frames are an exception, not a reliable update contract. These identified regressions do not establish that every change in that commit is broken.

### Final restoration

**VERIFIED SOURCE FINDING:** `Source/GameTick.cpp` restores the existing historical yaw predicates and `500/1000/2000°/s` rates, and bounded body tilt at `150°/s`, using `multiplier`. `Source/Objects/Person.cpp` / `Person.hpp` restore historical head conversion, limits, `500°/s` yaw and `250°/s` pitch, and pitch-only head-joint application. Head behavior matched across OSS 1.2, current OSS, and pre-modernization `8cee3e3`.

The final head implementation prepares one per-actor pending update after fresh simulation and consumes it at the first eligible draw. It preserves state eligibility and target producers while preventing repeated/stereo mutation and retaining the previous pose on zero-physics frames. This draw-order adaptation is intentional. Character experiment macros and Ryan's alternate yaw/tilt/head paths are removed; canonical rabbit TalkIdle remains. Input, locomotion intent, target-heading generation, velocity integration, AI, animation progression, camera follow, and browser/controller systems are unchanged by this restoration.

### Validation

**RUNTIME-VERIFIED — user physical QA:** yaw materially improved turning; tilt substantially improved player locomotion presentation; head restoration worked well and resembled original Lugaru. Holding forward while rotating the camera continuously retargeted physical movement correctly. The combined restored build played normally with controller/gameplay behavior intact and no remaining perceptible player-locomotion problem. No verified remaining player-locomotion regression currently warrants another code change.

**Build/source validation:** isolated historical-law and head-equivalence checks preceded physical QA. The final default Release Web target is `build-web-character-final/bin/lugaru.html`, built without character or scheduler experiment definitions. Rebuilding after scheduler separation and `git diff --check` passed; bundled assets remain unchanged. Source behavior matches the physically validated restored character configuration. No fixing commit is recorded yet because staging/committing is reserved for manual review.

### Scheduler remains a separate experiment

**OPEN / RUNTIME-VERIFIED — user A/B observation:** fixed `1 ms` versus OSS-style count-and-divide scheduling produced essentially no player improvement, but slightly smoother NPC turning/contact on some slope seams and path-alignment situations. This is insufficient evidence to change the default. That scheduler A/B preceded the tilt/head restorations: fewer repeated full-frame damping calls and different simulated-time budgets complicated attribution. It was not proof of a scheduler-only mechanism.

The exact `Source/main.cpp` experiment was preserved as `/home/logan/projects/lugaru-web-oss-scheduler-experiment.patch`, outside the repository. It uses `LUGARU_EXPERIMENT_OSS_SCHEDULER`, selects `max(2, int(T * 200))` before gameplay modifiers, and preserves the current outer-frame/controller integration and `250 ms` stall cap. Its base is `8820f6125030127b1c7e8c328511c72a71d9299b`; patch SHA-256 is `e259a2f61248792b250462923666162b66182c8778dd333178ea4b2f58935448`. Application was checked against that baseline and reproduced the experimental file exactly. The patch was then reversed from the working tree and checked for clean reapplication. This local artifact is not part of the character commit; preserve it separately when moving the workspace.

See [backlog A2a](research-backlog.md#a2a-npc-slopeseam-and-scheduler-comparison--open). Camera follow remains separate and is not an unfinished locomotion blocker.


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
