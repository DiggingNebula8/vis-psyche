#pragma once

#include <iostream>
#include <string>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "VizEngine/Core.h"
#include "VizEngine/Events/Event.h"

namespace VizEngine
{
	class VizEngine_API GLFWManager
	{
	public:
		GLFWManager(unsigned int width, unsigned int height, const std::string& title);
		~GLFWManager();

		void ProcessInput();
		bool WindowShouldClose();
		void SwapBuffersAndPollEvents();
		GLFWwindow* GetWindow() const;

		void SetEventCallback(const EventCallbackFn& callback) { m_EventCallback = callback; }

		int GetWidth() const { return m_Width; }
		int GetHeight() const { return m_Height; }

	private:
		GLFWwindow* m_Window;
		int m_Width, m_Height;
		EventCallbackFn m_EventCallback;

		void Init(unsigned int width, unsigned int height, const std::string& title);
		void Shutdown();

		static void FramebufferSizeCallback(GLFWwindow* window, int width, int height);
		static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
		static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
		static void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
		static void CursorPosCallback(GLFWwindow* window, double xpos, double ypos);
		static void WindowCloseCallback(GLFWwindow* window);
	};
}