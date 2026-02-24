# Software Renderer (SDL + C)

A small **CPU-based software renderer written in C**, using **SDL2 only
for windowing and display output**.  
The project focuses on learning **fundamental graphics programming
concepts** such as projection math, rasterization, buffer management,
and performance optimization before moving to GPU rendering.

This is part of my ongoing exploration into **rendering engineering /
technical art graphics programming**.

------------------------------------------------------------------------

## ✨ Current Features

-   SDL2 window + renderer initialization
-   CPU color buffer rendering pipeline
-   Perspective projection of 3D points → 2D screen space
-   Procedural 3D cube point cloud (9×9×9 grid)
-   Grid overlay rendering
-   Software rasterized rectangles
-   Event-driven input handling (Esc to quit)
-   Basic performance optimization of raster loops

------------------------------------------------------------------------

## 🧠 Concepts Covered So Far

### Software Rendering Pipeline

-   Manual color buffer allocation
-   CPU raster operations
-   Framebuffer clearing and presentation
-   CPU → GPU texture upload

### 3D Math Fundamentals

Perspective projection:

x’ = (fov \* x) / z  
y’ = (fov \* y) / z

-   Camera offset handling to avoid division by zero
-   Screen-space coordinate transforms

### Performance Lessons Learned

-   Importance of tight raster loops
-   Avoiding unnecessary full-screen iteration
-   Memory bandwidth considerations
-   Event loop responsiveness in SDL

------------------------------------------------------------------------

## ⚙️ Build Requirements

-   C compiler (GCC / Clang recommended)
-   SDL2 development libraries installed

### Linux example:

gcc main.c display.c vector.c -lSDL2 -o renderer

------------------------------------------------------------------------

## 🚀 Running

./renderer

Controls:

-   ESC → Exit application  
-   Window resize supported

------------------------------------------------------------------------

## 📂 Project Structure (Current)

. ├── main.c ├── display.c / display.h ├── vector.c / vector.h └──
README.md

------------------------------------------------------------------------

## 🛠 Known Limitations (Current Stage)

-   No depth buffering yet
-   No triangle rasterization
-   No shading model
-   CPU rasterization only (no GPU pipeline yet)
-   Minimal clipping / culling

This is intentional — the goal is to build the pipeline step-by-step.

------------------------------------------------------------------------

## 🎯 Planned Next Steps

### Rendering Features

-   Triangle rasterization
-   Depth buffer implementation
-   Wireframe + filled primitives
-   Camera transform system
-   Basic lighting model

### Performance

-   Dirty rectangle updates
-   SIMD optimizations (later)
-   Frame timing control

### Graphics Programming Progression

-   Software → OpenGL/Vulkan pipeline transition
-   Shader experimentation
-   Physically-based shading concepts

------------------------------------------------------------------------

## 💼 Why This Project Exists

Coming from a **VFX/film lighting & compositing technical background**,
I’m building stronger low-level graphics foundations to transition
toward:

-   Rendering engineering
-   Technical art (games)
-   GPU graphics programming

Understanding the CPU renderer helps demystify GPU pipelines and shader
math.

------------------------------------------------------------------------

## 📚 Inspiration / Learning Sources

-   Pikuma graphics programming courses
-   Classic software renderer tutorials
-   Real-time rendering literature
