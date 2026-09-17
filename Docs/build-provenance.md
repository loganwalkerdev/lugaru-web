# WebAssembly Build Provenance

This document records the environment and commands used to reproduce the known-good Lugaru Web browser build.

It describes the build verified on **2026-09-16** from Ryan Deering's inherited WebAssembly branch.

## Source Baseline

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

## Host Environment

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

## Development Tools

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

## Base Development Packages

The initial WSL environment was prepared with:

```bash
sudo apt install -y git build-essential cmake pkg-config
```

---

## Emscripten Setup

The verified build uses **Emscripten 6.0.9**.

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

## Repository Configuration

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

## Emscripten Toolchain Selection

The inherited WebAssembly branch contains:

```text
.devcontainer/toolchain-emscripten.cmake
```

That custom toolchain file was **not used** for the verified build.

Instead, the build used Emscripten's standard `emcmake` wrapper.

This was deliberate because the inherited toolchain file manually constructs Emscripten compiler paths and was unnecessary with the installed `emsdk` environment.

---

## Configure

From the repository root:

```bash
rm -rf build-web

emcmake cmake -S . -B build-web \
  -DCMAKE_BUILD_TYPE=Release \
  -DUSE_CDN_DATA=OFF
```

`USE_CDN_DATA=OFF` packages the Lugaru data with the local WebAssembly build rather than using the inherited external CDN configuration.

Configuration completed successfully.

---

## CMake Warning

CMake 4.2.3 emitted the following warning repeatedly during configuration:

```text
This version of cmake (4.2.3) does not support emscripten shared libraries.
Use cmake < 4.2.0 or cmake > 4.3.3 if you need shared library support
```

The warning did not prevent configuration or compilation of the verified build.

It should be reevaluated if the project's library structure changes, the CMake version changes, or a future build begins failing around shared-library handling.

---

## Build

The configured project was compiled with:

```bash
cmake --build build-web -j$(nproc)
```

The build completed successfully with:

```text
[100%] Built target lugaru
```

---

## Browser Build Dependencies

The inherited WebAssembly configuration uses Emscripten-provided or fetched versions of the project's dependencies.

### GL4ES

GL4ES provides the compatibility layer used by Lugaru's legacy fixed-function OpenGL renderer.

The inherited branch pins GL4ES to:

```text
8b424fa5aeea8a2445961ae98c326dc6061e109a
```

The pin should remain fixed until an update is deliberately tested.

### jsoncpp

The inherited branch fetches:

```text
jsoncpp 1.9.5
```

### Emscripten Ports

The Web build uses Emscripten support for dependencies including:

```text
SDL2
PNG
JPEG
Ogg
Vorbis
```

---

## Relevant Web Build Configuration

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

The current optimization profile primarily favors binary size.

Performance-oriented alternatives should only replace it after measurement.

---

## Output

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

## Observed Artifact Hashes

The hashes were recorded from `build-web/bin` with:

```bash
sha256sum lugaru.html lugaru.js lugaru.wasm lugaru.data
```

Observed SHA-256 values from the verified local build:

```text
de9ec90017b947abdcc4f8c12f0ae9347ee423349ce5204863ccf2de17b22d04  lugaru.html
a0ddc434e0c160e51ee5ba3d957bf41eecacc51a1453b7b78dcd03e4fa847b21  lugaru.js
17e5d44b791d8328e103924b12556ab640afcbe4ed568bf6b789f72cbc1e5a93  lugaru.wasm
0e466ec74170e2690b2aa91180f9f217d1c671b29fd49f225c53e3905433b563  lugaru.data
```

These hashes identify the exact artifacts produced by the verified build.

They are **not** treated as guaranteed future outputs. Changes in source, toolchain behavior, dependency state or build metadata may produce different hashes even when the resulting build is functionally equivalent.

---

## Local Testing

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

## Initial Verification

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
docs/porting-notes.md
```

---

## Reproducing the Baseline

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
cd ~/projects/lugaru-web

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
cd ~/projects/lugaru-web
git switch main
```

---

## Updating This Document

Update this file when a change affects the reproducible build environment, including:

* required tool versions,
* dependency revisions,
* CMake configuration,
* Emscripten flags,
* generated output structure,
* asset packaging,
* or the supported build procedure.

Runtime defects, implementation investigations and issue resolutions should be recorded in `docs/porting-notes.md` instead.
