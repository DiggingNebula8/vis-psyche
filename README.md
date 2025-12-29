# VizPsyche

VizPsyche is an open-source 3D graphics engine written in C++17 using OpenGL 4.6. Designed for learning graphics programming, it provides clean abstractions over OpenGL primitives and includes an ImGui-based interface for real-time parameter editing.

[![Demo Video](https://img.youtube.com/vi/_n3hrrPEjSM/maxresdefault.jpg)](https://youtu.be/_n3hrrPEjSM)

---

## Table of Contents

- [Features](#features)
- [Project Structure](#project-structure)
- [Prerequisites](#prerequisites)
- [Building](#building)
- [Running](#running)
- [Usage](#usage)
- [Dependencies](#dependencies)
- [Troubleshooting](#troubleshooting)
- [License](#license)

---

## Features

- Modern OpenGL 4.6 rendering pipeline
- Abstraction classes: VertexBuffer, IndexBuffer, VertexArray, Shader, Texture, Renderer
- Unified shader format (vertex and fragment shaders in a single file)
- ImGui integration for runtime parameter adjustment
- spdlog-based logging with separate core and client loggers
- Engine/Application architecture for creating custom applications

---

## Project Structure

```
VizPsyche/
├── CMakeLists.txt              # Root CMake configuration
├── build.bat                   # Build script (supports debug/release/clean)
├── debug_build.bat             # Quick debug build script
│
├── VizEngine/                  # Core engine (builds as shared library)
│   ├── CMakeLists.txt
│   ├── include/                # Public headers (GLAD)
│   │   ├── glad/
│   │   └── KHR/
│   ├── src/
│   │   ├── VizEngine.h         # Main include header
│   │   ├── VizEngine/
│   │   │   ├── Application.cpp/h
│   │   │   ├── Core.h          # DLL export macros
│   │   │   ├── EntryPoint.h    # Main entry point
│   │   │   ├── Log.cpp/h
│   │   │   ├── GUI/
│   │   │   │   └── UIManager.cpp/h
│   │   │   └── OpenGL/
│   │   │       ├── ErrorHandling.cpp/h
│   │   │       ├── GLFWManager.cpp/h
│   │   │       ├── IndexBuffer.cpp/h
│   │   │       ├── Renderer.cpp/h
│   │   │       ├── Shader.cpp/h
│   │   │       ├── Texture.cpp/h
│   │   │       ├── VertexArray.cpp/h
│   │   │       ├── VertexBuffer.cpp/h
│   │   │       └── VertexBufferLayout.h
│   │   └── resources/
│   │       ├── shaders/
│   │       └── textures/
│   └── vendor/                 # Third-party libraries (git submodules)
│       ├── glfw/
│       ├── glm/
│       ├── imgui/
│       ├── spdlog/
│       └── stb_image/
│
└── Sandbox/                    # Example application
    ├── CMakeLists.txt
    └── src/
        └── SandboxApp.cpp
```

---

## Prerequisites

### Windows

1. **Visual Studio 2019, 2022, or 2026** with the "Desktop development with C++" workload installed.

2. **CMake 3.16 or later**. Either use the version bundled with Visual Studio, or download from https://cmake.org/download/.

3. **Git** for cloning the repository with submodules.

---

## Building

### Option 1: Build Scripts (Recommended)

Open a terminal in the repository root and run:

```
.\debug_build.bat
```

For release builds or clean rebuilds:

```
.\build.bat release
.\build.bat clean debug
.\build.bat clean release
```

The scripts automatically detect your installed Visual Studio version.

### Option 2: Manual CMake

```
mkdir build
cd build
cmake -G "Visual Studio 17 2022" -A x64 ..
cmake --build . --config Debug
```

Replace "Visual Studio 17 2022" with your installed version:
- Visual Studio 2019: `"Visual Studio 16 2019"`
- Visual Studio 2022: `"Visual Studio 17 2022"`
- Visual Studio 2026: `"Visual Studio 18 2026"`

### Option 3: Open in Visual Studio

After running CMake, open the generated solution:

```
cmake --open build
```

Or directly open `build\VizPsyche.sln`.

---

## Running

After a successful build, the output is located in:

```
build\bin\Debug\Sandbox.exe
build\bin\Debug\VizEngine.dll
build\bin\Debug\src\resources\
```

Run the application:

```
.\build\bin\Debug\Sandbox.exe
```

For release builds, replace `Debug` with `Release`.

---

## Usage

To create a custom application using VizEngine, inherit from `VizEngine::Application`:

```cpp
#include <VizEngine.h>

class MyApp : public VizEngine::Application
{
public:
    MyApp() {}
    ~MyApp() {}
};

VizEngine::Application* VizEngine::CreateApplication()
{
    return new MyApp();
}
```

The engine provides the entry point. Your application only needs to implement `CreateApplication()`.

---

## Dependencies

All dependencies are included as git submodules in `VizEngine/vendor/`.

| Library | Purpose |
|---------|---------|
| GLFW 3.3+ | Window creation and input handling |
| GLAD | OpenGL 4.6 function loader |
| GLM | Mathematics library (vectors, matrices, transformations) |
| Dear ImGui | Immediate mode graphical user interface |
| spdlog | Fast C++ logging library |
| stb_image | Single-header image loading |

To update submodules manually:

```
git submodule update --init --recursive
```

---

## Troubleshooting

### "Could not find any instance of Visual Studio"

CMake cannot locate Visual Studio. Verify that:
- Visual Studio is installed with the "Desktop development with C++" workload
- You are using a supported version (2019, 2022, or 2026)

### Submodule directories are empty

Run:

```
git submodule update --init --recursive
```

### Missing DLL error when running Sandbox.exe

Ensure you are running from the correct directory (`build\bin\Debug\` or `build\bin\Release\`). The build places `VizEngine.dll` alongside `Sandbox.exe`.

### OpenGL errors or black screen

- Update your GPU drivers
- Verify your GPU supports OpenGL 4.6
- Check the console output for shader compilation errors

### Linker errors (unresolved external symbols)

- Clean the build directory and rebuild: `.\build.bat clean debug`
- Verify all source files are listed in `VizEngine/CMakeLists.txt`

---

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.
