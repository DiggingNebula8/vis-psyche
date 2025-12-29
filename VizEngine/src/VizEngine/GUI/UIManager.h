#pragma once

#include <iostream>
#include <string>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "VizEngine/Core.h"

namespace VizEngine
{
	class VizEngine_API UIManager
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
}