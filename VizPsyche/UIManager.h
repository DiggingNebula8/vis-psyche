#pragma once

#include<iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include"imgui.h"
#include"imgui_impl_glfw.h"
#include"imgui_impl_opengl3.h"

class UIManager
{
public:
	UIManager(GLFWwindow* window);
	~UIManager();

	void BeginFrame();
	void EndFrame();
	void Render();
	void StartWindow(const std::string& windowName);
	void EndWindow();
private:
	void Init(GLFWwindow* window);
	void Shutdown();
};