#include "Camera.h"

namespace VizEngine
{
	Camera::Camera(float fov, float aspectRatio, float nearPlane, float farPlane)
		: m_Position(0.0f, 0.0f, 0.0f)
		, m_Pitch(0.0f)
		, m_Yaw(0.0f)
		, m_FOV(fov)
		, m_AspectRatio(aspectRatio)
		, m_NearPlane(nearPlane)
		, m_FarPlane(farPlane)
		, m_ViewMatrix(1.0f)
		, m_ProjectionMatrix(1.0f)
	{
		RecalculateViewMatrix();
		RecalculateProjectionMatrix();
	}

	void Camera::SetPosition(const glm::vec3& position)
	{
		m_Position = position;
		RecalculateViewMatrix();
	}

	void Camera::SetRotation(float pitch, float yaw)
	{
		m_Pitch = pitch;
		m_Yaw = yaw;
		RecalculateViewMatrix();
	}

	void Camera::SetFOV(float fov)
	{
		m_FOV = fov;
		RecalculateProjectionMatrix();
	}

	void Camera::SetAspectRatio(float aspectRatio)
	{
		m_AspectRatio = aspectRatio;
		RecalculateProjectionMatrix();
	}

	void Camera::SetClipPlanes(float nearPlane, float farPlane)
	{
		m_NearPlane = nearPlane;
		m_FarPlane = farPlane;
		RecalculateProjectionMatrix();
	}

	void Camera::Move(const glm::vec3& offset)
	{
		m_Position += offset;
		RecalculateViewMatrix();
	}

	void Camera::MoveForward(float amount)
	{
		m_Position += GetForward() * amount;
		RecalculateViewMatrix();
	}

	void Camera::MoveRight(float amount)
	{
		m_Position += GetRight() * amount;
		RecalculateViewMatrix();
	}

	void Camera::MoveUp(float amount)
	{
		m_Position += GetUp() * amount;
		RecalculateViewMatrix();
	}

	glm::vec3 Camera::GetForward() const
	{
		return glm::normalize(glm::vec3(
			cos(m_Pitch) * sin(m_Yaw),
			sin(m_Pitch),
			cos(m_Pitch) * cos(m_Yaw)
		));
	}

	glm::vec3 Camera::GetRight() const
	{
		return glm::normalize(glm::cross(GetForward(), glm::vec3(0.0f, 1.0f, 0.0f)));
	}

	glm::vec3 Camera::GetUp() const
	{
		return glm::normalize(glm::cross(GetRight(), GetForward()));
	}

	void Camera::RecalculateViewMatrix()
	{
		glm::vec3 forward = GetForward();
		glm::vec3 target = m_Position + forward;
		m_ViewMatrix = glm::lookAt(m_Position, target, glm::vec3(0.0f, 1.0f, 0.0f));
	}

	void Camera::RecalculateProjectionMatrix()
	{
		m_ProjectionMatrix = glm::perspective(glm::radians(m_FOV), m_AspectRatio, m_NearPlane, m_FarPlane);
	}
}


