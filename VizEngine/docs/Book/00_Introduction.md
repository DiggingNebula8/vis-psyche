# VizPsyche Engine: A Technical Journey

## What is VizPsyche?

VizPsyche is a 3D rendering engine built from scratch using C++17 and OpenGL 4.6. The name combines "Viz" (visualization) with "Psyche" (the mind/soul) - representing the goal of understanding the soul of graphics programming.

## Why Build an Engine from Scratch?

1. **Deep Understanding** - Using Unity or Unreal hides the fundamentals
2. **Complete Control** - Every line of code is yours to understand and modify
3. **Industry Knowledge** - Game studios value engineers who understand the "how"
4. **Problem Solving** - Debugging becomes easier when you know the internals

## What You'll Learn

By the end of this book, you'll understand:

- **Build Systems** - CMake, DLLs, linking, and project organization
- **Graphics Pipeline** - How pixels get from code to screen
- **OpenGL** - Modern OpenGL 4.6 with VAOs, VBOs, shaders
- **Engine Architecture** - How professional engines are structured
- **Memory Management** - RAII, move semantics, resource lifetime

## Prerequisites

- Basic C++ knowledge (classes, pointers, references)
- Basic math (vectors, matrices - we'll explain as we go)
- A Windows PC with Visual Studio 2022+

## Project Structure Overview

```
VizPsyche/
├── CMakeLists.txt          ← Root build configuration
├── VizEngine/              ← The engine (compiled as DLL)
│   ├── CMakeLists.txt
│   ├── src/
│   │   ├── VizEngine.h     ← Public API header
│   │   └── VizEngine/      ← Engine source code
│   ├── include/            ← Third-party headers (GLAD)
│   └── vendor/             ← Dependencies (GLFW, GLM, ImGui, spdlog)
├── Sandbox/                ← Test application (uses the engine)
│   ├── CMakeLists.txt
│   └── src/
│       └── SandboxApp.cpp
└── build/                  ← Generated build files
```

## How to Read This Book

Each chapter builds on the previous. Don't skip ahead - the concepts stack!

1. **Chapter 1: Build System** - CMake and project setup
2. **Chapter 2: DLL Architecture** - Why we separate engine and app
3. **Chapter 3: OpenGL Fundamentals** - The graphics pipeline
4. **Chapter 4: Abstractions** - Wrapping OpenGL in clean C++
5. **Chapter 5: Engine Architecture** - Proper game engine structure
6. **Chapter 6: Rendering** - Putting it all together

Let's begin!

