#pragma once

#include "VizEngine/Core.h"
#include "glm.hpp"

namespace VizEngine
{
	/**
	 * DirectionalLight represents a light source infinitely far away (like the sun).
	 * All rays are parallel, coming from a single direction.
	 */
	struct VizEngine_API DirectionalLight
	{
		glm::vec3 Direction = glm::vec3(-0.2f, -1.0f, -0.3f);  // Direction light is traveling
		
		glm::vec3 Ambient  = glm::vec3(0.2f, 0.2f, 0.2f);      // Base illumination
		glm::vec3 Diffuse  = glm::vec3(0.8f, 0.8f, 0.8f);      // Main light color
		glm::vec3 Specular = glm::vec3(1.0f, 1.0f, 1.0f);      // Highlight color

		DirectionalLight() = default;

		DirectionalLight(const glm::vec3& direction)
			: Direction(glm::normalize(direction)) {}

		DirectionalLight(const glm::vec3& direction, const glm::vec3& color)
			: Direction(glm::normalize(direction))
			, Ambient(color * 0.2f)
			, Diffuse(color * 0.8f)
			, Specular(color) {}

		// Get normalized direction (for shader)
		glm::vec3 GetDirection() const { return glm::normalize(Direction); }
	};

	/**
	 * PointLight represents a light source at a specific position that radiates in all directions.
	 * Intensity falls off with distance (attenuation).
	 */
	struct VizEngine_API PointLight
	{
		glm::vec3 Position = glm::vec3(0.0f, 5.0f, 0.0f);
		
		glm::vec3 Ambient  = glm::vec3(0.1f, 0.1f, 0.1f);
		glm::vec3 Diffuse  = glm::vec3(0.8f, 0.8f, 0.8f);
		glm::vec3 Specular = glm::vec3(1.0f, 1.0f, 1.0f);

		// Attenuation factors (how light falls off with distance)
		float Constant  = 1.0f;
		float Linear    = 0.09f;
		float Quadratic = 0.032f;

		PointLight() = default;

		PointLight(const glm::vec3& position)
			: Position(position) {}

		PointLight(const glm::vec3& position, const glm::vec3& color)
			: Position(position)
			, Ambient(color * 0.1f)
			, Diffuse(color * 0.8f)
			, Specular(color) {}
	};

	/**
	 * Material properties for Phong lighting model.
	 */
	struct VizEngine_API Material
	{
		glm::vec3 Ambient   = glm::vec3(1.0f);    // How much ambient light is reflected
		glm::vec3 Diffuse   = glm::vec3(1.0f);    // Main surface color
		glm::vec3 Specular  = glm::vec3(0.5f);    // Specular highlight color
		float Shininess     = 32.0f;               // Specular exponent (higher = smaller highlight)

		Material() = default;

		Material(const glm::vec3& color, float shininess = 32.0f)
			: Ambient(color)
			, Diffuse(color)
			, Specular(glm::vec3(0.5f))
			, Shininess(shininess) {}
	};
}

