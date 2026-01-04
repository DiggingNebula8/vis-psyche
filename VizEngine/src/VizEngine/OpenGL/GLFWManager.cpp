#include "GLFWManager.h"
#include "VizEngine/Log.h"
#include "VizEngine/Core/Input.h"
#include "VizEngine/Events/ApplicationEvent.h"
#include "VizEngine/Events/KeyEvent.h"
#include "VizEngine/Events/MouseEvent.h"

namespace VizEngine
{
	GLFWManager::GLFWManager(unsigned int width, unsigned int height, const std::string& title)
		: m_Window(nullptr), m_Width(0), m_Height(0)
	{
		Init(width, height, title);
	}

	GLFWManager::~GLFWManager()
	{
		Shutdown();
	}

	void GLFWManager::Init(unsigned int width, unsigned int height, const std::string& title)
	{
		if (!glfwInit())
		{
			VP_CORE_ERROR("Failed to initialize GLFW");
			exit(EXIT_FAILURE);
		}
		// Set window hints
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
		glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);

		m_Window = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);
		if (!m_Window)
		{
			VP_CORE_ERROR("Failed to create GLFW window");
			glfwTerminate();
			exit(EXIT_FAILURE);
		}

		m_Width = static_cast<int>(width);
		m_Height = static_cast<int>(height);

		glfwMakeContextCurrent(m_Window);

		// Store this pointer for callbacks
		glfwSetWindowUserPointer(m_Window, this);

		glfwSetFramebufferSizeCallback(m_Window, FramebufferSizeCallback);
		glfwSetKeyCallback(m_Window, KeyCallback);
		glfwSetCharCallback(m_Window, CharCallback);
		glfwSetMouseButtonCallback(m_Window, MouseButtonCallback);
		glfwSetScrollCallback(m_Window, ScrollCallback);
		glfwSetCursorPosCallback(m_Window, CursorPosCallback);
		glfwSetWindowCloseCallback(m_Window, WindowCloseCallback);
		glfwSetWindowFocusCallback(m_Window, WindowFocusCallback);

		// Initialize input system
		Input::Init(m_Window);
	}

	void GLFWManager::Shutdown()
	{
		glfwDestroyWindow(m_Window);
		glfwTerminate();
	}

	bool GLFWManager::WindowShouldClose()
	{
		return glfwWindowShouldClose(m_Window);
	}

	void GLFWManager::SwapBuffersAndPollEvents()
	{
		glfwSwapBuffers(m_Window);
		glfwPollEvents();
	}

	GLFWwindow* GLFWManager::GetWindow() const
	{
		return m_Window;
	}

	void GLFWManager::FramebufferSizeCallback(GLFWwindow* window, int width, int height)
	{
		glViewport(0, 0, width, height);

		auto* manager = static_cast<GLFWManager*>(glfwGetWindowUserPointer(window));
		if (manager)
		{
			manager->m_Width = width;
			manager->m_Height = height;

			if (manager->m_EventCallback)
			{
				WindowResizeEvent event(width, height);
				manager->m_EventCallback(event);
			}
		}
	}

	void GLFWManager::ProcessInput()
	{
		// Update input state first
		Input::Update();

		// Handle escape key to close window
		if (Input::IsKeyPressed(KeyCode::Escape))
		{
			glfwSetWindowShouldClose(m_Window, true);
		}
	}

	void GLFWManager::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		(void)scancode;
		(void)mods;

		auto* manager = static_cast<GLFWManager*>(glfwGetWindowUserPointer(window));
		if (!manager || !manager->m_EventCallback)
			return;

		switch (action)
		{
			case GLFW_PRESS:
			{
				KeyPressedEvent event(static_cast<KeyCode>(key), false);
				manager->m_EventCallback(event);
				break;
			}
			case GLFW_RELEASE:
			{
				KeyReleasedEvent event(static_cast<KeyCode>(key));
				manager->m_EventCallback(event);
				break;
			}
			case GLFW_REPEAT:
			{
				KeyPressedEvent event(static_cast<KeyCode>(key), true);
				manager->m_EventCallback(event);
				break;
			}
		}
	}

	void GLFWManager::MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
	{
		(void)mods;

		auto* manager = static_cast<GLFWManager*>(glfwGetWindowUserPointer(window));
		if (!manager || !manager->m_EventCallback)
			return;

		switch (action)
		{
			case GLFW_PRESS:
			{
				MouseButtonPressedEvent event(static_cast<MouseCode>(button));
				manager->m_EventCallback(event);
				break;
			}
			case GLFW_RELEASE:
			{
				MouseButtonReleasedEvent event(static_cast<MouseCode>(button));
				manager->m_EventCallback(event);
				break;
			}
		}
	}

	void GLFWManager::ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
	{
		// Forward to Input system for polling support
		Input::ScrollCallback(window, xoffset, yoffset);

		// Fire event for event-driven handling
		auto* manager = static_cast<GLFWManager*>(glfwGetWindowUserPointer(window));
		if (manager && manager->m_EventCallback)
		{
			MouseScrolledEvent event(static_cast<float>(xoffset), static_cast<float>(yoffset));
			manager->m_EventCallback(event);
		}
	}

	void GLFWManager::CursorPosCallback(GLFWwindow* window, double xpos, double ypos)
	{
		auto* manager = static_cast<GLFWManager*>(glfwGetWindowUserPointer(window));
		if (manager && manager->m_EventCallback)
		{
			MouseMovedEvent event(static_cast<float>(xpos), static_cast<float>(ypos));
			manager->m_EventCallback(event);
		}
	}

	void GLFWManager::WindowCloseCallback(GLFWwindow* window)
	{
		auto* manager = static_cast<GLFWManager*>(glfwGetWindowUserPointer(window));
		if (manager && manager->m_EventCallback)
		{
			WindowCloseEvent event;
			manager->m_EventCallback(event);
		}
	}

	void GLFWManager::WindowFocusCallback(GLFWwindow* window, int focused)
	{
		auto* manager = static_cast<GLFWManager*>(glfwGetWindowUserPointer(window));
		if (manager && manager->m_EventCallback)
		{
			if (focused)
			{
				WindowFocusEvent event;
				manager->m_EventCallback(event);
			}
			else
			{
				WindowLostFocusEvent event;
				manager->m_EventCallback(event);
			}
		}
	}

	void GLFWManager::CharCallback(GLFWwindow* window, unsigned int codepoint)
	{
		auto* manager = static_cast<GLFWManager*>(glfwGetWindowUserPointer(window));
		if (manager && manager->m_EventCallback)
		{
			KeyTypedEvent event(static_cast<KeyCode>(codepoint));
			manager->m_EventCallback(event);
		}
	}
}
