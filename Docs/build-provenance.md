# WebAssembly Build Provenance

This document records the environment and commands used to build Lugaru-Web for the browser.

It has two parts:

1. **[Current v1.0 release build](#current-v10-release-build)** — the procedure for the Lugaru-Web v1.0.0 Web release.
2. **[Historical baseline verification (2026-09-16)](#historical-baseline-verification-2026-09-16)** — the preserved record of the first known-good build of Ryan Deering's inherited WebAssembly branch. Its commands, sizes and hashes describe the baseline tag `baseline/wasm-b340204`, not the current release.

---

## Current v1.0 release build

**Last updated:** 2026-10-13

### Release source

The final Lugaru-Web v1.0.0 Web artifacts were built on 2026-10-13 from the
clean `release/v1.0.0-prep` working tree at:

```text
1b81e1d48b1ad4d681258e04a1696536d794fa3e
```

That commit is the exact source for the final v1.0.0 Web artifacts and hashes
recorded below. A later documentation-only commit that records this provenance
does not alter these generated artifacts.

### Toolchain and environment

The v1.0 release build uses the same host environment family as the historical baseline:

```text
Host OS:      Windows 11 (WSL2)
Distribution: Ubuntu 26.04.1 LTS
Emscripten:   6.0.9
CMake:        4.2.3
Git:          2.53.0
Generator:    Unix Makefiles
EMCC_CFLAGS:  <unset>
```

Emscripten 6.0.9 is installed and activated through `emsdk` exactly as described in [Emscripten Setup](#emscripten-setup). Do not use `latest`.

The final v1.0.0 build directory is:

```text
build-web-release
```

### Configure and build

From the repository root, with the pinned SDK active and `EMCC_CFLAGS`
unset, the final v1.0.0 build was configured and built from a freshly deleted
`build-web-release` directory with:

```bash
source ~/emsdk/emsdk_env.sh
hash -r

rm -rf build-web-release

emcmake cmake -S . -B build-web-release \
  -DCMAKE_BUILD_TYPE=Release

cmake --build build-web-release -j2
```

The build completed successfully with:

```text
[100%] Built target lugaru
```

No data-hosting option is required. `USE_CDN_DATA` no longer exists (see [Same-origin game data](#same-origin-game-data)).

For final release artifacts, configuring a fresh directory is preferable to reusing an older one. A build directory configured before `36407bd` may still contain a stale `USE_CDN_DATA` cache entry; current `CMakeLists.txt` never reads it, so it has no effect.

The GL4ES dependency patch (below) is applied idempotently, so both fresh and repeated builds of the same directory succeed. Both paths were verified during B-18: an existing build tree rebuilt successfully, and a separate fresh build tree received the patch automatically. See [porting notes B-18](porting-notes.md#b-18--dense-web-foliage-causes-severe-slowdown).

### Output

The final v1.0.0 build produced:

```text
build-web-release/bin/lugaru.html
build-web-release/bin/lugaru.js
build-web-release/bin/lugaru.wasm
build-web-release/bin/lugaru.data
```

All four files must be deployed together, from the same directory.

Exact artifact sizes of the final v1.0.0 Web artifacts, built from
`1b81e1d48b1ad4d681258e04a1696536d794fa3e` on 2026-10-13:

```text
lugaru.html       7,783 bytes
lugaru.js       146,235 bytes
lugaru.wasm   2,145,306 bytes
lugaru.data  40,057,550 bytes
```

SHA-256 values for the final v1.0.0 Web artifacts:

```text
8bb851dd9cc20c3230e4e64af1e301167b14503eb69c0525c84171ac7d84c1ea  lugaru.html
3f8ea58f7dcaa6f175f70fbac2a6472049517d2445512614a7230ef96386e9cc  lugaru.js
9701db1abc9acb56cc5e35c519122a518c937c5acefa0440dea468ec18313ffb  lugaru.wasm
17938681b428e1b58216326d4c84f62d4ef9d16130802c9dd4581e3effdfbfa1  lugaru.data
```

These hashes identify the final v1.0.0 Web artifacts. Rebuilding with a
different source, toolchain or dependency state may produce different hashes.

### Final artifact smoke test

**RUNTIME-VERIFIED (2026-10-13):** the final artifacts above were served
locally from `build-web-release/bin` (see [Local testing](#local-testing)) and
passed a browser smoke test:

```text
title screen shows Lugaru-Web v1.0.0
red loading UI
gameplay startup
audio after first click
fullscreen enter/exit
windowed reload
assets load
```

This is a release smoke test, not exhaustive QA. Detailed runtime validation
belongs in [`porting-notes.md`](porting-notes.md).

### Same-origin game data

**VERIFIED SOURCE FINDING:** commit `36407bd` (`fix: make Web release self-contained`) removed the inherited `USE_CDN_DATA` CMake option and the production-hostname redirect in `Misc/shell.html` that loaded `lugaru.data` from `https://dev.ryandeering.ie/lugaru.data`. `Module.locateFile()` now returns the requested path unchanged, so `lugaru.data` is fetched from the same origin and directory as `lugaru.html`.

The Web release is therefore self-contained: it does not depend on Ryan Deering's server or any other external data host. The game data is packaged from the repository's `Data/` directory at build time.

**HISTORICAL EVIDENCE — earlier release-candidate build (2026-10-04):** the
following same-origin/MIME inspection was performed on an earlier
release-candidate build from `917bdbae63c28d91649023e8b374bb4e845d6667`, not
on the final v1.0.0 artifacts. Its sizes therefore differ from the final
artifacts above. It is retained as evidence for the `36407bd` same-origin
change. The final artifacts were served locally and smoke-tested separately
(see [Final artifact smoke test](#final-artifact-smoke-test)).

The generated `build-web-release/bin/lugaru.html` and `lugaru.js` from that
earlier build contained no `dev.ryandeering`, `DATA_CDN_URL`, or
`USE_CDN_DATA` references.

That earlier build was served locally on port 8785 and opened through the
WSL network address. Browser and HTTP-header inspection confirmed:

```text
lugaru.data   HTTP 200   Content-Length: 40238437   application/octet-stream
lugaru.wasm   HTTP 200   Content-Length: 2145378    application/wasm
lugaru.js     HTTP 200   Content-Length: 146235     text/javascript
```

The browser requested `lugaru.data` from the same host and port as
`lugaru.html`, and the startup console reported:

```text
Version Lugaru-Web v1.0.0 (Lugaru 1.3) -- Release build
```

**OPEN:** repeat the same-origin/MIME smoke test against the final public
deployment after GitHub Pages is configured.

Commit `36407bd` also renamed the shell title, SDL window title and startup
banner from Loupgarenne to Lugaru-Web, and set `LUGARU_VERSION_SUFFIX` to
`-lugaru-web`. Commit `8ba1cc6` then separated the Lugaru-Web project release
version from the inherited Lugaru engine version, producing the user-facing
identity `Lugaru-Web v1.0.0 (Lugaru 1.3)`.

### Current Web build configuration

`CMakeLists.txt` currently configures the Emscripten target with:

```text
Emscripten ports: SDL2, libpng, libjpeg, Ogg, Vorbis
GL4ES 8b424fa5aeea8a2445961ae98c326dc6061e109a, patched (see below)
jsoncpp 1.9.5 (FetchContent)
-Oz, -flto, --closure 1
-fexceptions, DISABLE_EXCEPTION_CATCHING=0
FULL_ES2=1, MAX_WEBGL_VERSION=2
ALLOW_MEMORY_GROWTH=1
ASSERTIONS=0, GL_ASSERTIONS=0
ENVIRONMENT=web
-lidbfs.js
--preload-file Data@/Data
--pre-js Misc/web-persistence.js
--shell-file Misc/shell.html
```

Changes from the historical baseline configuration that affect the build:

* **GL4ES patch:** `cmake/patches/gl4es-alpha-to-coverage.patch` forwards `GL_SAMPLE_ALPHA_TO_COVERAGE` to WebGL (B-18, `df72be9`). `cmake/apply-gl4es-patch.cmake` applies it only when the change is not already present (`9012637`). The GL4ES pin itself is unchanged.
* **Persistence:** IDBFS (`-lidbfs.js`) and `Misc/web-persistence.js` replace the inherited `localStorage` path (B-3, `3d747d3`). `web-persistence.js` is a `LINK_DEPENDS` of the `lugaru` target so edits trigger relinking.
* **Data hosting:** the CDN option was removed (`36407bd`), as described above.

The optimization profile still favors binary size; see [porting notes B-9](porting-notes.md#b-9--webassembly-build-is-optimized-primarily-for-size).

The inherited `.devcontainer/` (Fedora-based, native/MinGW oriented) and `.devcontainer/toolchain-emscripten.cmake` were not used for the release build; see [Emscripten Toolchain Selection](#emscripten-toolchain-selection).

### Local testing

Serve the release output over HTTP rather than `file://`:

```bash
python3 -m http.server 8785 \
  --bind 0.0.0.0 \
  --directory build-web-release/bin
```

For testing from the Windows host through WSL, obtain the current WSL address with:

```bash
hostname -I
```

Then open:

```text
http://<WSL-IP>:8785/lugaru.html
```

The WSL address can change between sessions.

Runtime defects, validation evidence and per-issue QA belong in [`porting-notes.md`](porting-notes.md), not here.

### CI

The retained GitHub Actions build workflow builds native Linux, macOS and
Windows comparison targets only. Before v1.0.0, its inherited
`lugaru-loupgarenne-*` artifact/install names were reconciled to
`lugaru-web-native-*`.

The inherited tag-driven native release workflow, which triggered on
`*-loupgarenne` tags, was removed in `8ba1cc6` because v1.0.0 is a Web-only
release and is being packaged and deployed separately.

No current workflow builds or tests the Web target. The v1.0 Web release is
therefore built and verified locally using the procedure above. Web CI and
automated Pages/release deployment remain post-v1.0 work. See
[porting notes CI-1](porting-notes.md#ci-1--macos-ci-fails-during-dependency-setup).

---

## Historical baseline verification (2026-09-16)

> **Historical record.** Everything below describes the build verified on **2026-09-16** from Ryan Deering's inherited WebAssembly branch, before any Lugaru-Web changes. It is preserved as provenance for `baseline/wasm-b340204`. Its commands are correct **for that tag** (including `-DUSE_CDN_DATA=OFF`, which existed then), and its sizes and hashes identify the baseline artifacts. They are not the current release procedure; see [Current v1.0 release build](#current-v10-release-build).

### Source Baseline

Verified source commit:

```text
b340204635e86a4385ef88dc45e2696e2213d9ab
```

Original commit:

```text
wasm support
Author: Ryan Deering
```

The repository preserves this state with the annotated Git tag:

```text
baseline/wasm-b340204
```

Later documentation commits do not change the source state used for this initial verification.

---

### Host Environment

The successful build was produced on:

```text
Host OS: Windows 11 Version 25H2
Windows build: 26200.9457

WSL: WSL2
Distribution: Ubuntu 26.04.1 LTS
Release: 26.04
Codename: resolute
Architecture: x86_64
```

The browser used for the initial smoke test was:

```text
Google Chrome
Version 153.0.8010.47
Official Build
64-bit
```

Browser revision:

```text
73934a44f61e6b3878d1943064c141a5a820f5f7
```

Because browser behavior can affect WebAssembly, input handling, focus behavior, fullscreen and rendering, the browser version is recorded as part of the verification environment.

---

### Development Tools

The verified environment included:

```text
Git         2.53.0
CMake       4.2.3
Emscripten  6.0.9
```

CMake 4.2.3 was the version installed in the verified Ubuntu 26.04.1 environment. It is recorded as part of the known-good environment rather than treated as a permanent project requirement.

If the CMake version changes, the version used for a verified build should be recorded so version-specific warnings and behavior remain interpretable.

Host GCC was installed through the base development environment but is not treated as a required WebAssembly toolchain pin because the Web build is compiled through Emscripten/Clang.

---

### Base Development Packages

The initial WSL environment was prepared with:

```bash
sudo apt install -y git build-essential cmake pkg-config
```

---

### Emscripten Setup

The verified build uses **Emscripten 6.0.9**. The v1.0 release build uses the same version and setup.

Install and activate that exact version:

```bash
cd ~

git clone https://github.com/emscripten-core/emsdk.git
cd emsdk

./emsdk install 6.0.9
./emsdk activate 6.0.9

source ./emsdk_env.sh
```

Verify the active compiler:

```bash
emcc --version
```

The expected version for this baseline is:

```text
Emscripten 6.0.9
```

When reproducing the build in a new shell, explicitly reactivate the pinned SDK:

```bash
cd ~/emsdk

./emsdk activate 6.0.9
source ./emsdk_env.sh

emcc --version
```

The reported version should be **6.0.9**.

Using `latest` is intentionally avoided because it would allow the toolchain to drift and reduce reproducibility.

---

### Repository Configuration

The working repository was originally cloned from Loupgarenne and configured with separate remotes for the inherited fork and Wolfire reference repository.

Relevant inherited branch:

```text
loupgarenne/feature/wasm-support
```

Verified WebAssembly commit:

```text
b340204635e86a4385ef88dc45e2696e2213d9ab
```

The frozen baseline should be referenced through the annotated tag:

```text
baseline/wasm-b340204
```

---

### Emscripten Toolchain Selection

The inherited WebAssembly branch contains:

```text
.devcontainer/toolchain-emscripten.cmake
```

That custom toolchain file was **not used** for the verified build.

Instead, the build used Emscripten's standard `emcmake` wrapper.

This was deliberate because the inherited toolchain file manually constructs Emscripten compiler paths and was unnecessary with the installed `emsdk` environment.

---

### Configure

From the repository root, at the baseline tag:

```bash
rm -rf build-web

emcmake cmake -S . -B build-web \
  -DCMAKE_BUILD_TYPE=Release \
  -DUSE_CDN_DATA=OFF
```

`USE_CDN_DATA=OFF` packaged the Lugaru data with the local WebAssembly build rather than using the inherited external CDN configuration. Even with the option off, the baseline `Misc/shell.html` still redirected `lugaru.data` to `https://dev.ryandeering.ie/lugaru.data` on any hostname other than `localhost`/`127.0.0.1`; local testing was therefore unaffected. Both the option and the redirect were removed in `36407bd`.

Configuration completed successfully.

---

### CMake Warning

CMake 4.2.3 emitted the following warning repeatedly during configuration:

```text
This version of cmake (4.2.3) does not support emscripten shared libraries.
Use cmake < 4.2.0 or cmake > 4.3.3 if you need shared library support
```

The warning did not prevent configuration or compilation of the verified build.

It should be reevaluated if the project's library structure changes, the CMake version changes, or a future build begins failing around shared-library handling.

---

### Build

The configured project was compiled with:

```bash
cmake --build build-web -j$(nproc)
```

The build completed successfully with:

```text
[100%] Built target lugaru
```

---

### Browser Build Dependencies

The inherited WebAssembly configuration uses Emscripten-provided or fetched versions of the project's dependencies.

#### GL4ES

GL4ES provides the compatibility layer used by Lugaru's legacy fixed-function OpenGL renderer.

The inherited branch pins GL4ES to:

```text
8b424fa5aeea8a2445961ae98c326dc6061e109a
```

The pin should remain fixed until an update is deliberately tested. (The current build keeps this pin and applies a small local patch; see [Current Web build configuration](#current-web-build-configuration).)

#### jsoncpp

The inherited branch fetches:

```text
jsoncpp 1.9.5
```

#### Emscripten Ports

The Web build uses Emscripten support for dependencies including:

```text
SDL2
PNG
JPEG
Ogg
Vorbis
```

---

### Relevant Web Build Configuration

The inherited WebAssembly branch enables or configures behavior including:

```text
WebAssembly output
WebGL support with a maximum version of 2
FULL_ES2=1
ALLOW_MEMORY_GROWTH=1
C++ exception support
browser-only execution environment
link-time optimization
size-oriented optimization
asset preloading
```

The optimization profile primarily favors binary size.

Performance-oriented alternatives should only replace it after measurement.

---

### Output

The successful build produced:

```text
build-web/bin/lugaru.html
build-web/bin/lugaru.js
build-web/bin/lugaru.wasm
build-web/bin/lugaru.data
```

Approximate sizes from the initial verified build were:

```text
lugaru.html     8.2 KB
lugaru.js       134 KB
lugaru.wasm     2.0 MB
lugaru.data     38 MB
```

These sizes are recorded as observed baseline values rather than strict expectations.

---

### Observed Artifact Hashes

The hashes were recorded from `build-web/bin` with:

```bash
sha256sum lugaru.html lugaru.js lugaru.wasm lugaru.data
```

Observed SHA-256 values from the verified local baseline build:

```text
de9ec90017b947abdcc4f8c12f0ae9347ee423349ce5204863ccf2de17b22d04  lugaru.html
a0ddc434e0c160e51ee5ba3d957bf41eecacc51a1453b7b78dcd03e4fa847b21  lugaru.js
17e5d44b791d8328e103924b12556ab640afcbe4ed568bf6b789f72cbc1e5a93  lugaru.wasm
0e466ec74170e2690b2aa91180f9f217d1c671b29fd49f225c53e3905433b563  lugaru.data
```

These hashes identify the exact artifacts produced by the 2026-09-16 baseline build. They do not describe any Lugaru-Web release build.

They are **not** treated as guaranteed future outputs. Changes in source, toolchain behavior, dependency state or build metadata may produce different hashes even when the resulting build is functionally equivalent.

---

### Local Testing

The generated files were served from:

```bash
cd build-web/bin
python3 -m http.server 8000
```

The game was opened in Chrome at:

```text
http://localhost:8000/lugaru.html
```

The files should be served over HTTP rather than opened directly through a `file://` URL.

---

### Initial Verification

On **2026-09-16**, the baseline build successfully demonstrated at least:

```text
game startup
menus
tutorial and campaign loading
character models and animation
textures and terrain
basic keyboard movement
audio
fullscreen
normal level loading and progression
```

This was a surface-level smoke test rather than exhaustive QA.

Subsequent testing found that some gameplay inputs may stop responding after browser focus changes. Combat and full input behavior should therefore not be considered fully verified by this initial test.

Detailed runtime defects belong in:

```text
Docs/porting-notes.md
```

---

### Reproducing the Baseline

First activate and verify the pinned Emscripten SDK:

```bash
cd ~/emsdk

./emsdk activate 6.0.9
source ./emsdk_env.sh

emcc --version
```

Confirm that the reported version is **6.0.9**.

Then switch to the preserved source baseline:

```bash
cd /path/to/lugaru-web

git checkout baseline/wasm-b340204
```

Checking out the annotated baseline tag places Git in a detached `HEAD` state.

This is intentional for verification. Project changes should not be made while checked out at the baseline tag.

Configure and build:

```bash
rm -rf build-web

emcmake cmake -S . -B build-web \
  -DCMAKE_BUILD_TYPE=Release \
  -DUSE_CDN_DATA=OFF

cmake --build build-web -j$(nproc)
```

Serve the generated build:

```bash
cd build-web/bin
python3 -m http.server 8000
```

Then open:

```text
http://localhost:8000/lugaru.html
```

After baseline verification, stop the local server and return to the active development branch:

```bash
cd /path/to/lugaru-web
git switch main
```

---

## Updating This Document

Update the [current release build](#current-v10-release-build) section when a change affects the reproducible build environment, including:

* required tool versions,
* dependency revisions or local dependency patches,
* CMake configuration,
* Emscripten flags,
* generated output structure,
* asset packaging or data hosting,
* or the supported build procedure.

Do not rewrite the historical baseline section; it records what was verified on 2026-09-16.

Runtime defects, implementation investigations and issue resolutions should be recorded in `Docs/porting-notes.md` instead.
