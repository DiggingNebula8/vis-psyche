#pragma once

#include "VizEngine/Core.h"
#include "VizEngine/Core/Transform.h"
#include "VizEngine/Core/Mesh.h"
#include "VizEngine/OpenGL/Texture.h"
#include "glm.hpp"
#include <memory>

namespace VizEngine
{
	/**
	 * SceneObject bundles everything needed to render an object in the scene.
	 * 
	 * Note: This is a simple, learning-focused approach. Production engines
	 * typically use Entity Component Systems (ECS) for better flexibility
	 * and performance. See Part XI (Chapters 37+) for ECS implementation.
	 */
	struct VizEngine_API SceneObject
	{
		std::shared_ptr<Mesh> MeshPtr;              // Geometry (shared - many objects can use same mesh)
		std::shared_ptr<Texture> TexturePtr;        // Optional per-object texture (nullptr = use default)
		Transform ObjectTransform;                   // Position, rotation, scale (unique per object)
		glm::vec4 Color = glm::vec4(1.0f);          // Per-object tint color
		float Roughness = 0.5f;                      // Material roughness (0 = smooth, 1 = rough)
		float Metallic = 0.0f;                       // Material metalness (0 = dielectric, 1 = metal)
		bool Active = true;                          // Enable/disable rendering
		std::string Name = "Object";                 // Display name for UI

		SceneObject() = default;

		SceneObject(std::shared_ptr<Mesh> mesh)
			: MeshPtr(mesh) {}

		SceneObject(std::shared_ptr<Mesh> mesh, const Transform& transform)
			: MeshPtr(mesh), ObjectTransform(transform) {}

		SceneObject(std::shared_ptr<Mesh> mesh, const Transform& transform, const glm::vec4& color)
			: MeshPtr(mesh), ObjectTransform(transform), Color(color) {}
	};
}

