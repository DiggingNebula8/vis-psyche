#pragma once

#include "VizEngine/Core.h"
#include "VizEngine/Core/SceneObject.h"
#include "VizEngine/Core/Camera.h"
#include "VizEngine/OpenGL/Renderer.h"
#include "VizEngine/OpenGL/Shader.h"
#include <vector>
#include <memory>

namespace VizEngine
{
	/**
	 * Scene manages a collection of SceneObjects.
	 * 
	 * Provides a container-like interface for managing objects in the scene.
	 * This is a simple approach suitable for learning and small scenes.
	 * For production use with thousands of objects, consider Entity Component Systems.
	 */
	class VizEngine_API Scene
	{
	public:
		Scene() = default;
		~Scene() = default;

		// Prevent copying (scenes own their object list)
		Scene(const Scene&) = delete;
		Scene& operator=(const Scene&) = delete;

		// Allow moving
		Scene(Scene&&) noexcept = default;
		Scene& operator=(Scene&&) noexcept = default;

		// =====================================================================
		// Modification
		// =====================================================================

		/**
		 * Add a new object to the scene.
		 * @param mesh The mesh to use (shared ownership)
		 * @param name Optional display name for UI
		 * @return Reference to the created object for further configuration
		 */
		SceneObject& Add(std::shared_ptr<Mesh> mesh, const std::string& name = "Object");

		/**
		 * Remove an object by index.
		 * @param index The index of the object to remove
		 */
		void Remove(size_t index);

		/**
		 * Clear all objects from the scene.
		 */
		void Clear();

		// =====================================================================
		// Access (container-like)
		// =====================================================================

		/** Access object by index (no bounds checking). */
		SceneObject& operator[](size_t index) { return m_Objects[index]; }
		const SceneObject& operator[](size_t index) const { return m_Objects[index]; }

		/** Access object by index with bounds checking. */
		SceneObject& At(size_t index) { return m_Objects.at(index); }
		const SceneObject& At(size_t index) const { return m_Objects.at(index); }

		/** Number of objects in the scene. */
		size_t Size() const { return m_Objects.size(); }

		/** Check if scene is empty. */
		bool Empty() const { return m_Objects.empty(); }

		// =====================================================================
		// Iteration (enables range-based for loops)
		// =====================================================================

		auto begin() { return m_Objects.begin(); }
		auto end() { return m_Objects.end(); }
		auto begin() const { return m_Objects.begin(); }
		auto end() const { return m_Objects.end(); }

		// =====================================================================
		// Scene Operations
		// =====================================================================

		/**
		 * Update all objects (placeholder for future animation/physics).
		 * @param deltaTime Time since last frame in seconds
		 */
		void Update(float deltaTime);

		/**
		 * Render all active objects in the scene.
		 * @param renderer The renderer to use for draw calls
		 * @param shader The shader program to use
		 * @param camera The camera for view/projection matrices
		 */
		void Render(Renderer& renderer, Shader& shader, const Camera& camera);

	private:
		std::vector<SceneObject> m_Objects;
	};
}

