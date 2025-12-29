# Chapter 6: Current State & Code Reference

## What We've Built

As of now, VizPsyche has:

### Core Systems
- CMake build system (cross-platform ready)
- DLL architecture with proper exports
- Logging system (spdlog)
- Entry point abstraction

### OpenGL Abstractions
- VertexBuffer (VBO wrapper)
- IndexBuffer (IBO wrapper)
- VertexArray (VAO wrapper)
- VertexBufferLayout (attribute configuration)
- Shader (compile, link, uniforms, caching)
- Texture (load images, bind to slots)
- Renderer (clear, draw)
- GLFWManager (window, context, input)
- ErrorHandling (OpenGL debug callbacks)

### Engine Core
- Camera (view/projection matrices)
- Transform (position, rotation, scale)
- Mesh (geometry with factory methods)

### GUI
- UIManager (ImGui integration)

---

## File Reference

### Build Files

| File | Purpose |
|------|---------|
| `CMakeLists.txt` | Root build config |
| `VizEngine/CMakeLists.txt` | Engine library config |
| `Sandbox/CMakeLists.txt` | Test app config |

### Public API

| File | Purpose |
|------|---------|
| `VizEngine.h` | Single include for users |
| `Core.h` | Export macros, platform detection |
| `EntryPoint.h` | Defines main(), calls CreateApplication |
| `Application.h` | Base class for applications |
| `Log.h` | Logging macros |

### Core

| File | Purpose |
|------|---------|
| `Core/Camera.h` | View/projection matrix management |
| `Core/Transform.h` | Position, rotation, scale struct |
| `Core/Mesh.h` | Geometry abstraction with factories |

### OpenGL

| File | Purpose |
|------|---------|
| `OpenGL/VertexBuffer.h` | VBO wrapper |
| `OpenGL/IndexBuffer.h` | IBO wrapper |
| `OpenGL/VertexArray.h` | VAO wrapper |
| `OpenGL/VertexBufferLayout.h` | Vertex attribute configuration |
| `OpenGL/Shader.h` | Shader program wrapper |
| `OpenGL/Texture.h` | Texture wrapper |
| `OpenGL/Renderer.h` | High-level render commands |
| `OpenGL/GLFWManager.h` | Window and context |
| `OpenGL/ErrorHandling.h` | Debug output handling |
| `OpenGL/Commons.h` | Shared OpenGL includes |

### GUI

| File | Purpose |
|------|---------|
| `GUI/UIManager.h` | ImGui wrapper |

### Resources

| File | Purpose |
|------|---------|
| `resources/shaders/default.shader` | Combined vertex/fragment shader |
| `resources/textures/uvchecker.png` | Test texture |

---

## Class Diagram

```
VizEngine (namespace)
│
├── Application (base class, virtual destructor)
│   └── Run() → Main loop
│
├── Log (static)
│   ├── Init()
│   ├── GetCoreLogger()
│   └── GetClientLogger()
│
├── Camera
│   ├── SetPosition(), SetRotation()
│   ├── GetViewMatrix(), GetProjectionMatrix()
│   ├── GetViewProjectionMatrix()
│   └── Move*() helpers
│
├── Transform (struct)
│   ├── Position, Rotation, Scale
│   └── GetModelMatrix()
│
├── Mesh
│   ├── Bind(), Unbind()
│   ├── GetVertexArray(), GetIndexBuffer()
│   └── CreatePyramid(), CreateCube(), CreatePlane() [static factories]
│
├── OpenGL Wrappers
│   ├── VertexBuffer: Bind(), Unbind(), GetID()
│   ├── IndexBuffer: Bind(), Unbind(), GetCount()
│   ├── VertexArray: LinkVertexBuffer(), Bind(), Unbind()
│   ├── Shader: Bind(), Unbind(), Set*() uniforms
│   ├── Texture: Bind(), Unbind(), GetWidth(), GetHeight()
│   └── Renderer: Clear(), Draw()
│
├── GLFWManager
│   ├── ProcessInput()
│   ├── WindowShouldClose()
│   ├── SwapBuffersAndPollEvents()
│   └── GetWindow()
│
├── UIManager
│   ├── BeginFrame(), EndFrame()
│   ├── StartWindow(), EndWindow()
│   └── Render()
│
└── ErrorHandling (static)
    └── HandleErrors() → Sets up debug callback
```

---

## Key Design Patterns Used

### RAII (Resource Acquisition Is Initialization)
All OpenGL wrappers acquire resources in constructor, release in destructor.

### Factory Method
`Mesh::CreatePyramid()`, `Mesh::CreateCube()` create preconfigured objects.

### Singleton-ish
`Log` uses static methods and static logger instances.

### State Machine Wrapper
All OpenGL wrappers have `Bind()`/`Unbind()` to manage OpenGL's state machine.

---

## Memory Management

### Ownership Rules

| Object | Owned By | Lifetime |
|--------|----------|----------|
| VertexBuffer | Mesh | Until Mesh destroyed |
| IndexBuffer | Mesh | Until Mesh destroyed |
| VertexArray | Mesh | Until Mesh destroyed |
| Shader | Application | Until scope ends |
| Texture | Application | Until scope ends |
| Mesh (unique_ptr) | Application | Until scope ends |
| Camera | Application stack | Until function returns |
| Transform | Application stack | Until function returns |

### No Raw `new`/`delete`

We use:
- Stack allocation for small objects
- `std::unique_ptr` for owned heap objects
- RAII classes that clean up in destructors

---

## Performance Notes

### Current Optimizations
- Uniform location caching in Shader
- Move semantics prevent unnecessary copies
- Index buffers reduce vertex duplication

### Current Limitations (OK for learning)
- Single draw call per mesh
- No batching
- No frustum culling
- No LOD
- No instancing

---

## Debugging Tips

### OpenGL Errors
Debug output is enabled. Check console for messages like:
```
OpenGL Debug Message:
  Source:   API
  Type:     Error
  Severity: HIGH
  Message:  ...
```

### Shader Errors
Compilation errors print to console:
```
SHADER ERROR::COMPILATION ERROR: VERTEX
error: ...
```

### Logging
Use the logging macros:
```cpp
VP_CORE_INFO("Engine message");
VP_INFO("Application message");
VP_CORE_ERROR("Something went wrong!");
```

---

## Building & Running

```bash
# Generate project (first time)
cmake -B build -G "Visual Studio 17 2022"

# Build
cmake --build build --config Debug

# Run
./build/bin/Debug/Sandbox.exe
```

---

## What's Not Implemented Yet

Future chapters will cover:

- [ ] Scene graph (parent-child transforms)
- [ ] Multiple objects in scene
- [ ] Model loading (.obj, .fbx)
- [ ] Lighting (Phong, PBR)
- [ ] Shadow mapping
- [ ] Framebuffers and post-processing
- [ ] Input system abstraction
- [ ] Audio
- [ ] Physics integration
- [ ] Entity Component System

---

## Exercises for Practice

1. **Add a second object** - Create another Mesh and Transform, draw both
2. **Keyboard camera** - Move camera with WASD
3. **Mouse look** - Rotate camera with mouse
4. **New shader** - Create a wireframe shader
5. **New mesh** - Implement `Mesh::CreateSphere()`
6. **Material class** - Bundle Shader + Texture together

