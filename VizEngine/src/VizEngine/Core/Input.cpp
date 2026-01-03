#include "Input.h"
#include <GLFW/glfw3.h>
#include <cstring>

namespace VizEngine
{
	// Array size constants
	static constexpr int MAX_KEYS = 512;
	static constexpr int MAX_MOUSE_BUTTONS = 8;

	// Static member definitions
	GLFWwindow* Input::s_Window = nullptr;
	bool Input::s_CurrentKeys[MAX_KEYS] = { false };
	bool Input::s_PreviousKeys[MAX_KEYS] = { false };
	bool Input::s_CurrentMouseButtons[MAX_MOUSE_BUTTONS] = { false };
	bool Input::s_PreviousMouseButtons[MAX_MOUSE_BUTTONS] = { false };
	glm::vec2 Input::s_MousePosition = glm::vec2(0.0f);
	glm::vec2 Input::s_LastMousePosition = glm::vec2(0.0f);
	bool Input::s_FirstMouse = true;
	float Input::s_ScrollDelta = 0.0f;

	void Input::Init(GLFWwindow* window)
	{
		s_Window = window;
		
		// Register scroll callback
		glfwSetScrollCallback(window, ScrollCallback);
		
		// Initialize mouse position
		double x, y;
		glfwGetCursorPos(window, &x, &y);
		s_MousePosition = glm::vec2(static_cast<float>(x), static_cast<float>(y));
		s_LastMousePosition = s_MousePosition;
	}

	void Input::Update()
	{
		// Copy current state to previous state
		std::memcpy(s_PreviousKeys, s_CurrentKeys, sizeof(s_CurrentKeys));
		std::memcpy(s_PreviousMouseButtons, s_CurrentMouseButtons, sizeof(s_CurrentMouseButtons));
		
		// Update current key states
		for (int key = 0; key < MAX_KEYS; key++)
		{
			s_CurrentKeys[key] = (glfwGetKey(s_Window, key) == GLFW_PRESS);
		}
		
		// Update current mouse button states
		for (int button = 0; button < MAX_MOUSE_BUTTONS; button++)
		{
			s_CurrentMouseButtons[button] = (glfwGetMouseButton(s_Window, button) == GLFW_PRESS);
		}
		
		// Update mouse position
		s_LastMousePosition = s_MousePosition;
		double x, y;
		glfwGetCursorPos(s_Window, &x, &y);
		s_MousePosition = glm::vec2(static_cast<float>(x), static_cast<float>(y));
		
		// Handle first mouse movement (avoid large delta on first frame)
		if (s_FirstMouse)
		{
			s_LastMousePosition = s_MousePosition;
			s_FirstMouse = false;
		}
		
		// Reset scroll delta (it accumulates via callback)
		// Note: We don't reset here - we reset after it's read
	}

	// --- Keyboard ---

	bool Input::IsKeyPressed(KeyCode key)
	{
		int k = static_cast<int>(key);
		if (k < 0 || k >= MAX_KEYS) return false;
		return s_CurrentKeys[k] && !s_PreviousKeys[k];
	}

	bool Input::IsKeyHeld(KeyCode key)
	{
		int k = static_cast<int>(key);
		if (k < 0 || k >= MAX_KEYS) return false;
		return s_CurrentKeys[k];
	}

	bool Input::IsKeyReleased(KeyCode key)
	{
		int k = static_cast<int>(key);
		if (k < 0 || k >= MAX_KEYS) return false;
		return !s_CurrentKeys[k] && s_PreviousKeys[k];
	}

	// --- Mouse Buttons ---

	bool Input::IsMouseButtonPressed(MouseCode button)
	{
		int b = static_cast<int>(button);
		if (b < 0 || b >= MAX_MOUSE_BUTTONS) return false;
		return s_CurrentMouseButtons[b] && !s_PreviousMouseButtons[b];
	}

	bool Input::IsMouseButtonHeld(MouseCode button)
	{
		int b = static_cast<int>(button);
		if (b < 0 || b >= MAX_MOUSE_BUTTONS) return false;
		return s_CurrentMouseButtons[b];
	}

	bool Input::IsMouseButtonReleased(MouseCode button)
	{
		int b = static_cast<int>(button);
		if (b < 0 || b >= MAX_MOUSE_BUTTONS) return false;
		return !s_CurrentMouseButtons[b] && s_PreviousMouseButtons[b];
	}

	// --- Mouse Position ---

	glm::vec2 Input::GetMousePosition()
	{
		return s_MousePosition;
	}

	glm::vec2 Input::GetMouseDelta()
	{
		return s_MousePosition - s_LastMousePosition;
	}

	// --- Scroll Wheel ---

	float Input::GetScrollDelta()
	{
		float delta = s_ScrollDelta;
		s_ScrollDelta = 0.0f;  // Reset after reading
		return delta;
	}

	void Input::ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
	{
		(void)window;
		(void)xoffset;
		s_ScrollDelta += static_cast<float>(yoffset);
	}
}
