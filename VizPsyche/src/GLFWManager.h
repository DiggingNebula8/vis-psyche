#pragma once

#include"Commons.h"

class GLFWManager
{
public:
	GLFWManager(unsigned int width, unsigned int height, const std::string& title);
	~GLFWManager();

	bool WindowShouldClose();
	void SwapBuffersAndPollEvents();
	GLFWwindow* GetWindow() const;

private:
	GLFWwindow* window;
	static void FramebufferSizeCallback(GLFWwindow* window, int width, int height);
	void Init(unsigned int width, unsigned int height, const std::string& title);
	void Shutdown();

};