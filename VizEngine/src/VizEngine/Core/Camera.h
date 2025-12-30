#pragma once

#include "VizEngine/Core.h"
#include "glm.hpp"
#include "gtc/matrix_transform.hpp"

namespace VizEngine
{
	class VizEngine_API Camera
	{
	public:
		Camera(float fov = 45.0f, float aspectRatio = 1.0f, float nearPlane = 0.1f, float farPlane = 100.0f);

		// Setters
		void SetPosition(const glm::vec3& position);
		void SetRotation(float pitch, float yaw);
		void SetFOV(float fov);
		void SetAspectRatio(float aspectRatio);
		void SetClipPlanes(float nearPlane, float farPlane);

		// Getters
		const glm::vec3& GetPosition() const { return m_Position; }
		float GetPitch() const { return m_Pitch; }
		float GetYaw() const { return m_Yaw; }
		float GetFOV() const { return m_FOV; }

		// Matrix getters
		const glm::mat4& GetViewMatrix() const { return m_ViewMatrix; }
		const glm::mat4& GetProjectionMatrix() const { return m_ProjectionMatrix; }
		glm::mat4 GetViewProjectionMatrix() const { return m_ProjectionMatrix * m_ViewMatrix; }

		// Movement
		void Move(const glm::vec3& offset);
		void MoveForward(float amount);
		void MoveRight(float amount);
		void MoveUp(float amount);

		// Direction vectors
		glm::vec3 GetForward() const;
		glm::vec3 GetRight() const;
		glm::vec3 GetUp() const;

	private:
		void RecalculateViewMatrix();
		void RecalculateProjectionMatrix();

		glm::vec3 m_Position;
		float m_Pitch; // Up/down rotation (radians)
		float m_Yaw;   // Left/right rotation (radians)

		float m_FOV;
		float m_AspectRatio;
		float m_NearPlane;
		float m_FarPlane;

		glm::mat4 m_ViewMatrix;
		glm::mat4 m_ProjectionMatrix;
	};
}


