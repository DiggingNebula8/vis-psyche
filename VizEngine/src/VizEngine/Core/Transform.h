#pragma once

#include "VizEngine/Core.h"
#include "glm.hpp"
#include "gtc/matrix_transform.hpp"
#include "gtc/quaternion.hpp"

namespace VizEngine
{
	struct VizEngine_API Transform
	{
		glm::vec3 Position = glm::vec3(0.0f);
		glm::vec3 Rotation = glm::vec3(0.0f); // Euler angles in radians
		glm::vec3 Scale = glm::vec3(1.0f);

		Transform() = default;

		Transform(const glm::vec3& position)
			: Position(position) {}

		Transform(const glm::vec3& position, const glm::vec3& rotation)
			: Position(position), Rotation(rotation) {}

		Transform(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale)
			: Position(position), Rotation(rotation), Scale(scale) {}

		glm::mat4 GetModelMatrix() const
		{
			glm::mat4 model = glm::mat4(1.0f);
			
			// Apply transformations: Scale -> Rotate -> Translate
			model = glm::translate(model, Position);
			model = glm::rotate(model, Rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
			model = glm::rotate(model, Rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
			model = glm::rotate(model, Rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
			model = glm::scale(model, Scale);
			
			return model;
		}

		// Convenience methods for rotation in degrees
		void SetRotationDegrees(const glm::vec3& degrees)
		{
			Rotation = glm::radians(degrees);
		}

		glm::vec3 GetRotationDegrees() const
		{
			return glm::degrees(Rotation);
		}
	};
}

