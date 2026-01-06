#pragma once

#include <string>
#include "VizEngine/Core.h"

// Forward declarations
struct GLFWwindow;

namespace VizEngine
{
	// Forward declaration for event handling
	class Event;

	/**
	 * Manages ImGui integration with the engine.
	 * Provides wrapper methods so client applications don't need direct ImGui access.
	 * This avoids DLL boundary issues with ImGui's global context.
	 */
	class VizEngine_API UIManager
	{
	public:
		UIManager(GLFWwindow* window);
		~UIManager();

		// Non-copyable (owns ImGui context state)
		UIManager(const UIManager&) = delete;
		UIManager& operator=(const UIManager&) = delete;

		// Event handling - marks events as Handled if ImGui wants them
		void OnEvent(Event& e);

		// Frame lifecycle
		void BeginFrame();
		void Render();

		// Window helpers
		void StartWindow(const std::string& windowName);
		void StartFixedWindow(const std::string& windowName, float width, float height);
		void EndWindow();

		// =========================================================================
		// ImGui Widget Wrappers
		// These forward to ImGui internally so consumers don't need ImGui access
		// =========================================================================
		
		// Text and labels
		void Text(const char* fmt, ...);
		void Separator();
		void SameLine();

		// Input widgets
		bool Button(const char* label);
		bool Checkbox(const char* label, bool* value);
		bool SliderFloat(const char* label, float* value, float min, float max);
		bool DragFloat3(const char* label, float* values, float speed = 0.1f, float min = 0.0f, float max = 0.0f);
		
		// Color editors
		bool ColorEdit3(const char* label, float* color);
		bool ColorEdit4(const char* label, float* color);
		
		// Selection
		bool Selectable(const char* label, bool selected);
	
	// Images
	void Image(void* textureID, float width, float height);

	private:
		void Init(GLFWwindow* window);
		void Shutdown();
	};
}
