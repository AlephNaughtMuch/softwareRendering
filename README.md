# Software Renderer (SDL + C)

A **CPU-based software renderer written in C**, using **SDL2 only for windowing and display output**.  
The project focuses on building a complete rasterization pipeline from the ground up — covering projection math, triangle rasterization, depth buffering, lighting, texture mapping, and performance optimization — before moving to GPU rendering.

This is part of an ongoing exploration into **rendering engineering / graphics programming**, coming from a VFX/film technical background.

---

## ✨ Current Features

### Core Pipeline
- SDL2 window + renderer initialization (RGBA32 pixel format)
- CPU color buffer rendering pipeline
- Framebuffer clearing and presentation
- Locked 60 FPS frame timing with event-driven input handling

### Geometry & Mesh
- Mesh and triangle data types with dynamic arrays for complex geometry
- OBJ file loader — reads arbitrary meshes from disk
- Hardcoded cube mesh as default fallback
- Per-face and per-triangle color support

### Transformations
- Full matrix-based transformation pipeline
- Scale, rotation (per-axis), and translation matrices
- World matrix combining all transforms (matrix-to-matrix multiplication)
- NDC / projection matrix
- Object → world → screen space pipeline with correct Y-axis orientation

### Rendering Modes (interactive toggle)
- Wireframe rendering
- Filled flat-shaded triangles
- Wireframe + filled combined
- Textured rendering

### Rasterization
- Line drawing (screen-space)
- Triangle wireframe rendering
- Filled triangle rasterization (flat-top / flat-bottom split)
- Slope-based scanline fill with divide-by-zero safeguards

### Backface Culling
- Vector math (cross product, dot product, normals)
- Backface culling with interactive toggle between culling modes

### Depth Buffering
- Per-pixel Z-buffer implementation for correct depth testing (replaces painter's algorithm)
- Z-buffer applied to both textured and non-textured rendering paths
- Static maximum triangle buffer (no dynamic allocation per frame)

### Lighting
- Directional light source (flat shading model)
- Per-face light intensity based on surface normal alignment

### Texture Mapping
- PNG texture loading via the `upng` library (minimal, dependency-free)
- UV coordinates loaded directly from OBJ files
- V coordinate inversion to account for OBJ top-left UV origin
- UV coordinate interpolation using barycentric weights
- Perspective-correct UV mapping (perspective divide)
- Safeguards for degenerate pixels during perspective divide

---

## 🧠 Concepts Covered

### Software Rendering Pipeline
- Manual color buffer allocation and management
- CPU raster operations and framebuffer presentation
- CPU → GPU texture upload via SDL

### 3D Math
- Perspective projection: `x' = (fov * x) / z`, `y' = (fov * y) / z`
- Camera offset handling to avoid division by zero
- Screen-space coordinate transforms
- Matrix math: scale, rotation, translation, world, projection
- Vector math: cross products, dot products, surface normals
- Barycentric coordinate interpolation for UV mapping
- Perspective divide for correct texture projection

### Image Processing
- PNG decoding and pixel format handling (RGBA32)
- UV space to texel space mapping
- Perspective-correct interpolation across triangle surfaces

### Performance
- Tight raster loops to minimise unnecessary iteration
- Memory bandwidth considerations
- Frame timing and event loop responsiveness

---

## ⚙️ Build Requirements

- C compiler (GCC / Clang recommended)
- SDL2 development libraries

### Linux / macOS:
```bash
gcc src/main.c src/display.c src/vector.c -lSDL2 -lm -o renderer
```
Or simply:
```bash
make
```

---

## 🚀 Running

```bash
./renderer
```

### Controls

| Key | Action |
|-----|--------|
| `1` | Wireframe only |
| `2` | Filled triangles |
| `3` | Wireframe + filled |
| `4` | Textured |
| `C` | Toggle backface culling |
| `ESC` | Exit |

---

## 📂 Project Structure

```
.
├── src/
│   ├── main.c
│   ├── display.c / display.h
│   ├── vector.c / vector.h
│   ├── mesh.c / mesh.h
│   ├── triangle.c / triangle.h
│   ├── texture.c / texture.h
│   ├── light.c / light.h
│   ├── matrix.c / matrix.h
│   └── upng.c / upng.h       # Minimal PNG decoding library
├── Makefile
└── README.md
```

---

## 🛠 Known Limitations

- No clipping against the view frustum
- No mesh instancing
- CPU only — no GPU acceleration

These are intentional at this stage. The goal is to understand every part of the pipeline before moving to a GPU-based implementation.

---

## 🎯 Planned Next Steps

### Rendering Features
- View frustum clipping
- Camera system with movement controls
- Gouraud / Phong shading models

### Performance
- Dirty rectangle updates
- SIMD optimisations
- Improved frame timing control

### Graphics Programming Progression
- Software renderer → OpenGL pipeline transition
- Shader experimentation (GLSL)
- Physically-based shading concepts

---

## 💼 Why This Project Exists

Coming from a **VFX/film lighting & compositing technical background**, this project builds low-level graphics foundations toward a transition into:

- Rendering engineering
- GPU graphics programming
- Real-time NPR / stylized rendering pipelines

Understanding the CPU renderer demystifies GPU pipelines and shader math from first principles.

---

## 📚 Learning Sources

- Pikuma graphics programming courses
- Classic software renderer tutorials
- Real-time rendering literature
- [upng](https://github.com/elanthis/upng/) — minimal PNG decoding library by elanthis