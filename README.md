# Software Renderer

> A CPU-based software rendering engine written in C, using SDL2 exclusively for windowing and display output.

This project is a ground-up exploration of fundamental graphics programming — building a rendering pipeline entirely on the CPU before touching GPU APIs. The goal is to deeply understand the math and mechanics that GPU pipelines abstract away.

---

## Table of Contents

- [Features](#features)
- [Requirements](#requirements)
- [Building](#building)
- [Running](#running)
- [Controls](#controls)
- [Project Structure](#project-structure)
- [Concepts Covered](#concepts-covered)
- [Known Limitations](#known-limitations)
- [Roadmap](#roadmap)
- [Changelog](#changelog)
- [Motivation](#motivation)
- [Learning Resources](#learning-resources)

---

## Features

- SDL2 window and renderer initialization
- CPU color buffer rendering pipeline
- Perspective projection of 3D points to 2D screen space
- Per-axis vector rotation (X, Y, Z)
- Mesh data type with dynamic array support for complex geometry
- OBJ file loading — read mesh data directly from disk
- Wireframe rendering via line drawing from screen-space points
- Filled triangle rasterization
- Per-face and per-triangle color properties
- Backface culling with interactive toggle
- Painter's algorithm — sorts faces by average depth for correct draw order
- Interactive switching between culling modes and render modes at runtime
- 60 FPS frame timing
- Grid overlay rendering
- Event-driven input handling

---

## Requirements

- C compiler — GCC or Clang recommended
- [SDL2](https://www.libsdl.org/) development libraries
- Math library (`-lm`, included in Makefile)

**Install SDL2 on common platforms:**

```bash
# Ubuntu / Debian
sudo apt install libsdl2-dev

# macOS (Homebrew)
brew install sdl2

# Arch Linux
sudo pacman -S sdl2
```

---

## Building

A `Makefile` is included. From the repo root:

```bash
make
```

To build manually:

```bash
gcc src/main.c src/display.c src/vector.c -lSDL2 -lm -o renderer
```

---

## Running

```bash
./renderer
```

---

## Controls

| Key | Action |
|-----|--------|
| `ESC` | Quit the application |
| *(runtime keys)* | Toggle culling mode |
| *(runtime keys)* | Toggle render mode (wireframe / filled / both) |

Window resizing is supported.

---

## Project Structure

```
softwareRendering/
├── src/
│   ├── main.c          # Entry point, game loop, input handling
│   ├── display.c       # Color buffer, SDL texture, draw functions
│   ├── display.h
│   ├── vector.c        # 3D vector math, rotation, cross/dot product
│   └── vector.h
├── Makefile
└── README.md
```

---

## Concepts Covered

### Software Rendering Pipeline

- Manual color buffer allocation and management
- CPU rasterization — pixels, rects, lines, filled triangles
- Framebuffer clearing and presentation via SDL texture upload
- 60 FPS frame pacing

### 3D Math

Perspective projection formula applied per point:

```
x' = (fov * x) / z
y' = (fov * y) / z
```

- Per-axis rotation (X, Y, Z) using rotation matrices
- Vector cross product and dot product for surface normal calculation
- Division-by-zero guard when calculating slopes and projections
- Screen-space coordinate transforms

### Geometry & Mesh

- `mesh` struct with dynamic array backing for vertices and faces
- `triangle` and `face` structs with per-element color properties
- OBJ file parser — loads vertex and face data from disk

### Visibility & Sorting

- Backface culling using surface normals and camera dot product
- Painter's algorithm — triangles sorted by average Z depth before drawing

### Performance

- Tight raster loop design
- Avoiding unnecessary full-screen iteration
- Memory bandwidth considerations
- Event loop responsiveness in SDL

---

## Known Limitations

This project is intentionally built incrementally:

- No depth buffer (z-buffering) — painter's algorithm used instead
- No shading or lighting model yet
- No clipping or frustum culling
- CPU-only (no GPU pipeline integration)
- Minimal OBJ support (vertices and faces only)

---

## Roadmap

### Rendering

- [ ] Depth buffer (z-buffer) to replace painter's algorithm
- [ ] Camera transform system
- [ ] Basic lighting model (flat, Gouraud, Phong)
- [ ] Texture mapping

### Performance

- [ ] Dirty rectangle updates
- [ ] SIMD optimizations (longer term)

### Graphics Progression

- [ ] Transition to OpenGL / Vulkan
- [ ] Shader experimentation
- [ ] Physically-based shading concepts

---

## Changelog

| Date | Milestone |
|------|-----------|
| 2026-03-23 | Painter's algorithm — sort faces by average depth for correct draw order |
| 2026-03-22 | Per-face and per-triangle color properties |
| 2026-03-22 | Interactive runtime toggle for culling modes and render modes |
| 2026-03-22 | Division-by-zero guard when calculating rasterization slopes |
| 2026-03-22 | Filled triangle rasterization |
| 2026-03-07 | Vector math operations (cross, dot, normalize); backface culling |
| 2026-03-01 | OBJ file loader — read mesh geometry from disk |
| 2026-02-27 | Groundwork for OBJ/mesh data loading |
| 2026-02-26 | Mesh struct; dynamic array for complex geometry |
| 2026-02-26 | Wireframe cube at 60 FPS, higher resolution, orthographic mode |
| 2026-02-26 | `draw_triangle` function; mesh and triangle data types; line rendering |
| 2026-02-23 | Math library integration; per-axis rotation for projected cube |
| 2026-02-22 | Vector typedefs; faster rect drawing; basic 3D→2D cube projection |
| 2026-02-22 | Color buffer, SDL texture pipeline, grid overlay, draw pixel/rect functions |
| 2026-02-22 | Display module split into `display.c` / `display.h` |
| 2026-02-22 | Initial SDL2 window, ESC quit, first render loop |
| 2026-02-22 | Initial commit — project scaffolding, `src/` folder, Makefile |

---

## Motivation

This project comes from a VFX/film background in lighting and compositing, with the goal of building stronger low-level graphics foundations to move toward:

- Rendering engineering
- Technical art (games)
- GPU graphics programming

Understanding the CPU renderer helps demystify the math behind GPU pipelines and shader programming.

---

## Learning Resources

- [Pikuma — 3D Computer Graphics Programming](https://pikuma.com/courses/raycasting-engine-tutorial-algorithm-javascript)
- Classic software renderer tutorials
- *Real-Time Rendering* (Akenine-Möller et al.)
