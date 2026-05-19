# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

Mazer is a C++ maze game built with SDL2. The player navigates procedurally generated mazes, collects items, avoids enemies (whirligigs), and races to the finish within a time limit. Graphics emulate the CGA (Color Graphics Adapter) retro style.

## Build

**Linux (from `src/` directory):**
```sh
cd src
make
./mazer
```

**Cross-platform with CMake (from repo root):**
```sh
cmake -B build
cmake --build build
```

CMake requires SDL2. On Linux: `sudo apt install libsdl2-dev`. On Windows, SDL2 paths are configured in `CMakeSettings.json` for Visual Studio.

Tests live in `tests/`. Build and run with `cmake --build build --target mazer_tests && ./build/tests/mazer_tests`. There are no linting tools.

## Architecture

Source lives in `src/`. Key modules:

**`SYSTEM/`** — SDL2 initialization and lifecycle. `SYSTEM.cpp` sets up the window, renderer, and a simulated 16MB memory block. `Timer.cpp` handles frame rate timing.

**`CGA/`** — Retro graphics layer that emulates CGA color modes. `GraphicsMode` is the base rendering class (pixel drawing, primitives). `TextMode` extends it for character-based rendering. `MODEs.h` defines `MODE_A` (text) and `MODE_B` (graphics). `COLORS.h` defines the CGA palette.

**`maze.cpp/h`** — Maze generation algorithms and Dijkstra-based pathfinding. Generates the maze grid, places prizes/items, and computes distance maps used by whirligig AI and solution display.

**`mazer.cpp/h`** — Main game loop and all entity logic: player movement, whirligig AI, block placement, collision, scoring, and HUD rendering. This is the largest file (~1630 lines).

**`OEM437.cpp/h`** — Renders the OEM 437 extended ASCII character set (the classic DOS character set used for box-drawing and symbols).

**`moremath.cpp/h`** — Utility math: random number generation, range clamping, distance calculation.

**`KEYBOARD.h`** — Input state tracked as bit flags.

**`stack.h`** — Template stack used by maze generation.

**`bitIcons.h`** — Raw bitmap data for icons rendered in the HUD.

## Coding Standards

- **Classes**: `CapitalCase`
- **Functions**: `camelCase`
- **Variables**: `snake_case`
- **Private variables**: `_leading_underscore`
- **Constants**: `UPPER_CASE`
