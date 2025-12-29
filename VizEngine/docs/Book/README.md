# VizPsyche Engine Technical Book

A hands-on guide to building a 3D rendering engine from scratch.

## Table of Contents

### Part 1: Foundation
1. **[Introduction](00_Introduction.md)** - What we're building and why
2. **[Build System](01_BuildSystem.md)** - CMake, project structure, building
3. **[DLL Architecture](02_DLLArchitecture.md)** - Exports, namespaces, entry points

### Part 2: Graphics
4. **[OpenGL Fundamentals](03_OpenGLFundamentals.md)** - Pipeline, buffers, shaders, coordinates
5. **[Abstractions](04_Abstractions.md)** - RAII wrappers, Rule of 5, clean APIs

### Part 3: Engine
6. **[Engine Architecture](05_EngineArchitecture.md)** - Camera, Transform, Mesh, separation of concerns
7. **[Current State](06_CurrentState.md)** - Code reference, class diagram, what's next

---

## Reading Order

Read chapters in order. Each builds on the previous:

```
00 Introduction
      ↓
01 Build System ←── Understand how we compile
      ↓
02 DLL Architecture ←── Understand how engine/app interact
      ↓
03 OpenGL Fundamentals ←── Understand graphics basics
      ↓
04 Abstractions ←── Understand our C++ patterns
      ↓
05 Engine Architecture ←── Understand how it all fits
      ↓
06 Current State ←── Reference and exercises
```

---

## How to Use This Book

### While Coding
Keep the book open alongside the code. When you see a class, find its section.

### To Learn
Work through exercises at the end of each chapter.

### To Review
Use Chapter 6 (Current State) as a quick reference.

---

## Updates

This is a **living document**. As the engine grows, new chapters will be added:

- [ ] Lighting and Materials
- [ ] Model Loading
- [ ] Scene Management
- [ ] Input System
- [ ] Audio
- [ ] Physics

---

## Prerequisites

- Basic C++ (classes, templates, pointers)
- Basic linear algebra (vectors, matrices)
- Visual Studio 2022 or later
- CMake 3.16+

---

## Quick Start

```bash
# Clone and build
git clone <repo>
cd vis-psyche
cmake -B build
cmake --build build --config Debug
./build/bin/Debug/Sandbox.exe
```

Then start reading from [Chapter 0](00_Introduction.md)!

