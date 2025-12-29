# Chapter 6: Multiple Objects & Scene Management

## The Problem

Look at our current `Application::Run()`. It works, but it's limited:

```cpp
auto pyramidMesh = Mesh::CreatePyramid();
Transform pyramidTransform;
// ... render one pyramid
```

What if we want:
- 10 pyramids at different positions?
- A mix of pyramids, cubes, and planes?
- The ability to add/remove objects at runtime?

### The Naive Approach (Don't Do This)

```cpp
// This doesn't scale!
auto pyramid1 = Mesh::CreatePyramid();
Transform transform1;

auto pyramid2 = Mesh::CreatePyramid();
Transform transform2;

auto cube1 = Mesh::CreateCube();
Transform transform3;

// ... and so on for 100 objects?
```

Problems:
- **Copy-paste nightmare** - Repetitive, error-prone
- **Fixed at compile time** - Can't add objects dynamically
- **Hard to manage** - What if you want to delete object #47?

---

## The SceneObject Concept

We need a struct that bundles everything that makes an "object" in our scene:

```cpp
struct SceneObject
{
    std::shared_ptr<Mesh> MeshPtr;     // What to draw (geometry)
    Transform ObjectTransform;          // Where to draw it
    glm::vec4 Color = glm::vec4(1.0f); // Per-object tint color
    bool Active = true;                 // Enable/disable rendering
};
```

### Why `shared_ptr` for Mesh?

**Key insight:** The mesh (geometry data) can be shared, but the transform must be unique.

Consider 100 trees in a forest:
- They all use the **same** tree mesh (vertices, indices)
- They each have **different** positions/rotations/scales

```cpp
// Create one mesh, share it
auto treeMesh = std::make_shared<Mesh>(Mesh::CreatePyramid());

// 100 objects, one mesh
for (int i = 0; i < 100; i++)
{
    SceneObject tree;
    tree.MeshPtr = treeMesh;  // All share the same geometry
    tree.ObjectTransform.Position = glm::vec3(i * 2.0f, 0.0f, 0.0f);  // Different positions
    scene.push_back(tree);
}
```

This is **much** more efficient than creating 100 separate meshes!

---

## The Scene Class

A container that manages our collection of objects:

```cpp
// VizEngine/Core/Scene.h

class VizEngine_API Scene
{
public:
    // Add an object and return reference for further configuration
    SceneObject& AddObject(std::shared_ptr<Mesh> mesh);
    
    // Remove object by index
    void RemoveObject(size_t index);
    
    // Clear all objects
    void Clear();
    
    // Update all objects (for animations, etc.)
    void Update(float deltaTime);
    
    // Render all active objects
    void Render(Renderer& renderer, Shader& shader, const Camera& camera);
    
    // Access
    std::vector<SceneObject>& GetObjects();
    const std::vector<SceneObject>& GetObjects() const;
    size_t GetObjectCount() const;
    
private:
    std::vector<SceneObject> m_Objects;
};
```

### Implementation

```cpp
// VizEngine/Core/Scene.cpp

SceneObject& Scene::AddObject(std::shared_ptr<Mesh> mesh)
{
    SceneObject obj;
    obj.MeshPtr = mesh;
    obj.ObjectTransform = Transform{};  // Default transform
    obj.Color = glm::vec4(1.0f);        // White (no tint)
    obj.Active = true;
    
    m_Objects.push_back(std::move(obj));
    return m_Objects.back();
}

void Scene::RemoveObject(size_t index)
{
    if (index < m_Objects.size())
    {
        m_Objects.erase(m_Objects.begin() + index);
    }
}

void Scene::Render(Renderer& renderer, Shader& shader, const Camera& camera)
{
    shader.Bind();
    
    for (auto& obj : m_Objects)
    {
        if (!obj.Active) continue;  // Skip disabled objects
        if (!obj.MeshPtr) continue; // Safety check
        
        // Calculate MVP for this object
        glm::mat4 mvp = camera.GetViewProjectionMatrix() 
                      * obj.ObjectTransform.GetModelMatrix();
        
        shader.SetMatrix4fv("u_MVP", mvp);
        shader.Set4f("u_Color", obj.Color);
        
        obj.MeshPtr->Bind();
        renderer.Draw(obj.MeshPtr->GetVertexArray(), 
                      obj.MeshPtr->GetIndexBuffer(), 
                      shader);
    }
}
```

---

## Rendering Multiple Objects

### Before: Single Object

```cpp
// One object, one draw call
glm::mat4 mvp = camera.GetViewProjectionMatrix() * pyramidTransform.GetModelMatrix();
shader.SetMatrix4fv("u_MVP", mvp);
pyramidMesh->Bind();
renderer.Draw(pyramidMesh->GetVertexArray(), pyramidMesh->GetIndexBuffer(), shader);
```

### After: Multiple Objects

```cpp
// Many objects, loop through them
for (auto& obj : scene.GetObjects())
{
    if (!obj.Active) continue;
    
    glm::mat4 mvp = camera.GetViewProjectionMatrix() 
                  * obj.ObjectTransform.GetModelMatrix();
    
    shader.SetMatrix4fv("u_MVP", mvp);
    shader.Set4f("u_Color", obj.Color);
    
    obj.MeshPtr->Bind();
    renderer.Draw(obj.MeshPtr->GetVertexArray(), 
                  obj.MeshPtr->GetIndexBuffer(), 
                  shader);
}
```

The key difference: we calculate a **new MVP matrix for each object** because each has its own transform.

---

## Shared vs Owned Resources

Understanding resource ownership is crucial:

```
┌─────────────────────────────────────────────────────────────┐
│                         Scene                               │
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐         │
│  │SceneObject 1│  │SceneObject 2│  │SceneObject 3│         │
│  │             │  │             │  │             │         │
│  │ Transform   │  │ Transform   │  │ Transform   │         │
│  │ pos: 0,0,0  │  │ pos: 5,0,0  │  │ pos: -5,0,0 │         │
│  │             │  │             │  │             │         │
│  └──────┬──────┘  └──────┬──────┘  └──────┬──────┘         │
│         │                │                │                 │
└─────────┼────────────────┼────────────────┼─────────────────┘
          │                │                │
          │ shared_ptr     │ shared_ptr     │ shared_ptr
          │                │                │
          ▼                ▼                ▼
    ┌───────────┐    ┌───────────┐    ┌───────────┐
    │  Pyramid  │    │   Cube    │    │  Pyramid  │
    │   Mesh    │    │   Mesh    │    │   Mesh    │
    │ (shared)  │    │ (shared)  │    │   (same   │
    └───────────┘    └───────────┘    │  as #1!)  │
                                      └───────────┘
```

**Shared (via `shared_ptr`):**
- Mesh geometry (vertices, indices, GPU buffers)
- Multiple objects can reference the same mesh
- Mesh is deleted when last reference is gone

**Owned (per object):**
- Transform (position, rotation, scale)
- Color
- Active state
- Each object has its own copy

---

## Updating the Application

Here's how `Application::Run()` changes:

```cpp
int Application::Run()
{
    // ... initialization code ...
    
    // ═══════════════════════════════════════════════════
    // Create Shared Meshes
    // ═══════════════════════════════════════════════════
    auto pyramidMesh = std::make_shared<Mesh>(Mesh::CreatePyramid());
    auto cubeMesh = std::make_shared<Mesh>(Mesh::CreateCube());
    
    // ═══════════════════════════════════════════════════
    // Build Scene
    // ═══════════════════════════════════════════════════
    Scene scene;
    
    // Add a row of pyramids
    for (int i = 0; i < 5; i++)
    {
        auto& obj = scene.AddObject(pyramidMesh);
        obj.ObjectTransform.Position = glm::vec3((i - 2) * 3.0f, 0.0f, 0.0f);
        obj.Color = glm::vec4(1.0f, 0.5f + i * 0.1f, 0.2f, 1.0f);
    }
    
    // Add a cube in the middle
    auto& cube = scene.AddObject(cubeMesh);
    cube.ObjectTransform.Position = glm::vec3(0.0f, 2.0f, -5.0f);
    cube.ObjectTransform.Scale = glm::vec3(2.0f);
    
    // ═══════════════════════════════════════════════════
    // Main Loop
    // ═══════════════════════════════════════════════════
    while (!window.WindowShouldClose())
    {
        // ... input, deltaTime ...
        
        // Update scene (e.g., rotate all objects)
        scene.Update(deltaTime);
        
        // Render
        renderer.Clear(clearColor);
        scene.Render(renderer, shader, camera);
        
        // ... UI, swap buffers ...
    }
}
```

---

## UI for Object Selection

With multiple objects, we need UI to select and manipulate them:

```cpp
// In your main loop, after ui.BeginFrame():

ui.StartWindow("Scene Objects");

// Object list
static int selectedObject = 0;
auto& objects = scene.GetObjects();

for (size_t i = 0; i < objects.size(); i++)
{
    char label[32];
    snprintf(label, sizeof(label), "Object %zu", i);
    
    if (ImGui::Selectable(label, selectedObject == (int)i))
    {
        selectedObject = (int)i;
    }
}

ImGui::Separator();

// Edit selected object
if (selectedObject >= 0 && selectedObject < (int)objects.size())
{
    auto& obj = objects[selectedObject];
    
    ImGui::Checkbox("Active", &obj.Active);
    ImGui::DragFloat3("Position", &obj.ObjectTransform.Position.x, 0.1f);
    ImGui::DragFloat3("Rotation", &obj.ObjectTransform.Rotation.x, 0.01f);
    ImGui::DragFloat3("Scale", &obj.ObjectTransform.Scale.x, 0.1f);
    ImGui::ColorEdit4("Color", &obj.Color.x);
    
    if (ImGui::Button("Delete"))
    {
        scene.RemoveObject(selectedObject);
        selectedObject = std::min(selectedObject, (int)scene.GetObjectCount() - 1);
    }
}

ImGui::Separator();

// Add new objects
if (ImGui::Button("Add Pyramid"))
{
    scene.AddObject(pyramidMesh);
}
ImGui::SameLine();
if (ImGui::Button("Add Cube"))
{
    scene.AddObject(cubeMesh);
}

ui.EndWindow();
```

---

## Performance Considerations

### Draw Calls

Each object = 1 draw call. For our learning engine, this is fine.

Production engines optimize with:
- **Batching** - Combine similar objects into one draw call
- **Instancing** - GPU draws multiple copies with one call
- **Frustum Culling** - Skip objects outside camera view

### Memory

With `shared_ptr`, we efficiently share mesh data:

| Scenario | Mesh Memory | Transform Memory |
|----------|-------------|------------------|
| 100 unique pyramids | 100× mesh size | 100× transform size |
| 100 pyramids (shared) | 1× mesh size | 100× transform size |

The shared approach uses ~100× less GPU memory for geometry!

---

## Key Takeaways

1. **SceneObject bundles rendering data** - Mesh + Transform + Color + Active flag
2. **Share geometry, own transforms** - `shared_ptr<Mesh>` with unique `Transform`
3. **Scene manages the collection** - Add, remove, update, render
4. **MVP per object** - Each object needs its own Model-View-Projection matrix
5. **UI enables editing** - Select, modify, delete objects at runtime

---

## Exercises

1. **Implement SceneObject and Scene** - Create the classes described above
2. **Create a grid of cubes** - 5×5 cubes with different colors
3. **Add object selection in UI** - Click list to select, show transform controls
4. **Implement object deletion** - Remove selected object, handle index updates
5. **Add rotation animation** - Make all pyramids spin at different speeds

---

> **Reference:** For implementation details, class diagrams, and debugging tips, see [Appendix A: Code Reference](A_Reference.md).

