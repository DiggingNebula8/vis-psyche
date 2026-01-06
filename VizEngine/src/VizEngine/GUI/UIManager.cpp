#include "UIManager.h"
#include "VizEngine/Events/Event.h"

#include <cstdarg>
#include <GLFW/glfw3.h>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

namespace VizEngine
{
	UIManager::UIManager(GLFWwindow* window)
	{
		Init(window);
	}

	UIManager::~UIManager()
	{
		Shutdown();
	}

	void UIManager::BeginFrame()
	{
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}

	void UIManager::Render()
	{
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}

	void UIManager::OnEvent(Event& e)
	{
		ImGuiIO& io = ImGui::GetIO();

		// If ImGui wants keyboard, consume keyboard events
		if (io.WantCaptureKeyboard && e.IsInCategory(EventCategoryKeyboard))
		{
			e.Handled = true;
		}

		// If ImGui wants mouse, consume mouse events
		if (io.WantCaptureMouse && e.IsInCategory(EventCategoryMouse))
		{
			e.Handled = true;
		}
	}

	void UIManager::StartWindow(const std::string& windowName)
	{
		ImGui::Begin(windowName.c_str());
	}

	void UIManager::StartFixedWindow(const std::string& windowName, float width, float height)
	{
		ImGui::SetNextWindowSize(ImVec2(width, height), ImGuiCond_FirstUseEver);
		ImGui::Begin(windowName.c_str(), nullptr, ImGuiWindowFlags_NoResize);
	}

	void UIManager::EndWindow()
	{
		ImGui::End();
	}

	// =========================================================================
	// ImGui Widget Wrappers
	// =========================================================================

	void UIManager::Text(const char* fmt, ...)
	{
		va_list args;
		va_start(args, fmt);
		ImGui::TextV(fmt, args);
		va_end(args);
	}

	void UIManager::Separator()
	{
		ImGui::Separator();
	}

	void UIManager::SameLine()
	{
		ImGui::SameLine();
	}

	bool UIManager::Button(const char* label)
	{
		return ImGui::Button(label);
	}

	bool UIManager::Checkbox(const char* label, bool* value)
	{
		return ImGui::Checkbox(label, value);
	}

	bool UIManager::SliderFloat(const char* label, float* value, float min, float max)
	{
		return ImGui::SliderFloat(label, value, min, max);
	}

	bool UIManager::DragFloat3(const char* label, float* values, float speed, float min, float max)
	{
		return ImGui::DragFloat3(label, values, speed, min, max);
	}

	bool UIManager::ColorEdit3(const char* label, float* color)
	{
		return ImGui::ColorEdit3(label, color);
	}

	bool UIManager::ColorEdit4(const char* label, float* color)
	{
		return ImGui::ColorEdit4(label, color);
	}

	bool UIManager::Selectable(const char* label, bool selected)
	{
		return ImGui::Selectable(label, selected);
	}

	void UIManager::Image(void* textureID, float width, float height)
	{
		// OpenGL textures are bottom-left origin, but ImGui expects top-left.
		// We flip the UV coordinates: ImVec2(0, 1) to ImVec2(1, 0).
		ImGui::Image(textureID, ImVec2(width, height), ImVec2(0, 1), ImVec2(1, 0));
	}

	// =========================================================================
	// Private Methods
	// =========================================================================

	void UIManager::Init(GLFWwindow* window)
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		(void)io;
		ImGui::StyleColorsDark();
		ImGui_ImplGlfw_InitForOpenGL(window, true);
		ImGui_ImplOpenGL3_Init("#version 460");
	}

	void UIManager::Shutdown()
	{
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}
}
