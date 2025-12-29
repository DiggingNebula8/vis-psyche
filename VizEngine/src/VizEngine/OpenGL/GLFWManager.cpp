#include "GLFWManager.h"

namespace VizEngine
{
	GLFWManager::GLFWManager(unsigned int width, unsigned int height, const std::string& title)
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
			std::cerr << "Failed to initialize GLFW\n";
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
			std::cerr << "Failed to create GLFW window\n";
			glfwTerminate();
			exit(EXIT_FAILURE);
		}

		glfwMakeContextCurrent(m_Window);
		glfwSetFramebufferSizeCallback(m_Window, FramebufferSizeCallback);
		glfwSetKeyCallback(m_Window, KeyCallback);
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
		(void)window;
		glViewport(0, 0, width, height);
	}

	void GLFWManager::ProcessInput()
	{
		if (glfwGetKey(m_Window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		{
			glfwSetWindowShouldClose(m_Window, true);
		}
	}

	void GLFWManager::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		(void)window;
		(void)key;
		(void)scancode;
		(void)action;
		(void)mods;
		// Handle single key events here
	}
}
