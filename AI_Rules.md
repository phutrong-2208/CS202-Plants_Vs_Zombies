# AI Agent Instructions & Architecture Guide: Plants vs. Zombies C++ / Raylib Engine

This document provides mandatory architectural patterns, coding rules, memory management standards, and implementation conventions for all AI agents working on this codebase.

---

## 1. Project Overview & Technology Stack

* **Language Standard:** C++17 (`set(CMAKE_CXX_STANDARD 17)`)
* **Graphics & Core Framework:** Raylib 5.5 (fetched via CMake `FetchContent`)
* **Build System:** CMake (minimum version 3.15)
* **Directory Structure:**
  * `lib/`: All C++ header files (`.hpp`), organized cleanly into domain subdirectories:
    * `lib/Core/`: Base engine utilities (`AssetManager`, `TextureManager`, `TexturePackage`, `ReanimParser`, `InputManager`, `TextManager`, `AnimationManager`)
    * `lib/Gameplay/`: Game entity logic (`Plants/`, `Zombies/`, `Animation/`)
    * `lib/Worlds/`: World, map grid, cell, and environment logic (`World`, `Map`, `DayMap`, `Grid`, `Cell`)
    * `lib/UI/`: User interface components (`SeedBank`, `ChoosePlants`, etc.)
    * `lib/Screens/`: Application state & screen management (`Screen`, `GameplayScreen`, etc.)
    * `lib/Common.hpp`: Master header including Raylib and C++ STL headers.
  * `src/`: Implementation files (`.cpp`) mirroring the `lib/` directory structure.
  * `assets/`: Textures, animation `.reanim` XML files, fonts, and audio assets.
  * `build/`: Target output directory for compiled binaries (`PvZMain.exe`).

---

## 2. Core Architectural Standards

### 🏛️ The Flyweight & Asset Resource Pattern
* **Flyweight Separation:** Gameplay entities (`Plant`, `Zombie`, etc.) must **never** hold redundant copies of texture buffers or parsed animation keyframe arrays.
* **`TexturePackage` & `TextureManager`:**
  * Assets are loaded once into centralized `TexturePackage` bundles owned by `TextureManager`.
  * Entities query `TextureManager` or receive raw pointers/references to `TexturePackage` and `Texture2D`.
  * **Rule:** Direct filesystem queries inside entity classes are strictly prohibited. All texture requests must pass through `TextureManager` / `AssetManager`.
* **Asset Paths:**
  * Compile definition `PROJECT_DIR` is set by CMake.
  * Always reference assets using portable paths relative to `PROJECT_DIR` or `assets/`.

### 🎞️ Reanim Engine Architecture
* **`ReanimParser` (Stateless Asset Data):**
  * Parses XML `.reanim` animation files into tracks (`ReanimTrack`), keyframes (`Frame`), and clips (`AnimClip`).
  * **Delta State Inheritance:** When parsing `<t></t>` keyframe elements with missing delta fields, the parser **must forward-fill missing values** by cloning data from previous keyframes. This prevents transform vectors from collapsing to default origin coordinates.
* **`AnimClip` (Clip Metadata):**
  * Sub-animations (e.g., `anim_idle`, `anim_shooting`, `anim_blink`) are parsed into `AnimClip` definitions:
    * `startTime`: Initial keyframe index for the clip.
    * `loopStart`: Frame index to rewind to when looping.
    * `endTime`: Terminal boundary index (exclusive) for the clip.
* **`ReanimInstance` (Runtime State Component):**
  * Attached to each animated entity to track local playback (`currentTime`, `speed`, `looping`, `clipLoopStart`, `clipEnd`).
  * Drives multi-state animation switching via `playClip(clipName)` (e.g., `playClip("idle")`, `playClip("shooting")`).
  * **Rule:** Never play `.reanim` files strictly linearly from frame 0 to end unless intended. Always respect active clip ranges (`clipLoopStart` to `clipEnd`).

---

## 3. Mathematical & Transform Standards

### 📐 Affine Matrix Transformations
When rendering Reanim skeleton tracks, matrices must be composed in exact order:
$$\text{Matrix} = \text{Scale} \times \text{Shear/Skew}(k_x, k_y) \times \text{Rotate} \times \text{Translate}$$

* Keyframe fields (`newX`, `newY`, `scaleX`, `scaleY`, `skewX`, `skewY`, `alpha`) represent local track transforms.
* Hierarchical parent-child transformation matrices must be aggregated recursively before dispatching to Raylib's `DrawTexturePro`.

---

## 4. Entity & World Systems Layout

* **5x9 Grid System:** The lawn layout (`Grid`) consists of 5 rows and 9 columns of `Cell` objects tracking occupancy and plant references.
* **Plant & Zombie Hierarchy:**
  * Subclasses inherit from base `Plant` / `Zombie` abstract classes.
  * Factory pattern (`PlantFactory`) instantiates entities dynamically based on type and metadata.
* **Screen State Flow:**
  * Screen transitions are managed through `GameLoop` executing `Screen` interfaces (`GameplayScreen`, etc.).

---

## 5. Coding & Implementation Guardrails

### ✅ MUST DO:
1. **Header Include Rules:** Use `#include <Common.hpp>` or clean relative module paths (e.g., `#include <Core/ReanimParser.hpp>`). Include guards (`#ifndef HEADER_NAME_HPP ... #endif`) must be present on every header.
2. **Memory Safety:** Use stack allocation or modern smart pointers (`std::unique_ptr`, `std::shared_ptr`) for runtime entities. Avoid unmanaged naked `new`/`delete` allocations.
3. **Clip Boundaries:** When calling `ReanimInstance::playClip()`, verify clip existence and preserve current frame timing appropriately.
4. **Build Verification:** After making any code changes, always compile using CMake to ensure zero compilation or linking errors.

### 🚫 FORBIDDEN ACTIONS:
1. **No Hardcoded Asset Reads:** Do NOT load textures directly inside `Plant` or `Zombie` constructors using Raylib's `LoadTexture()` directly. Use `TextureManager` / `AssetManager`.
2. **No Null Transforms on Blank Keyframes:** Do NOT drop blank delta fields in XML parsing without forward-filling state values from previous frames.
3. **No Direct Mutating of Flyweight Data:** Entities must NOT mutate shared `ReanimParser` or `TexturePackage` instances at runtime.
4. **No Hardcoded Absolute Paths:** Do NOT use hardcoded Windows absolute paths (e.g. `C:\...`) in code or assets. Use relative paths or `PROJECT_DIR`.