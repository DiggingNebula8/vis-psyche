#pragma once

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

class GLFWManager
{
public:
	GLFWManager(unsigned int width, unsigned int height, const std::string& title);
	~GLFWManager();

	void ProcessInput();
	bool WindowShouldClose();
	void SwapBuffersAndPollEvents();
	GLFWwindow* GetWindow() const;

private:
	GLFWwindow* window;
	static void FramebufferSizeCallback(GLFWwindow* window, int width, int height);
	static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	void Init(unsigned int width, unsigned int height, const std::string& title);
	void Shutdown();

};