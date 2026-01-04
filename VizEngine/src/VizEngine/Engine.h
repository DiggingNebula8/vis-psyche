#pragma once

#include <string>
#include <memory>
#include <cstdint>
#include "Core.h"

namespace VizEngine
{
	// Forward declarations
	class Application;
	class GLFWManager;
	class Renderer;
	class UIManager;

	/**
	 * Configuration for the Engine.
	 * Pass to CreateApplication() to customize window and rendering settings.
	 */
	struct EngineConfig
	{
		std::string Title = "VizPsyche";
		uint32_t Width = 800;
		uint32_t Height = 800;
		bool VSync = true;
	};

	/**
	 * Engine singleton that owns the game loop and core subsystems.
	 * Separates engine infrastructure from game logic.
	 */
	class VizEngine_API Engine
	{
	public:
		/**
		 * Get the singleton instance.
		 * Uses Meyer's singleton pattern for thread-safe lazy initialization.
		 */
		static Engine& Get();

		// Non-copyable
		Engine(const Engine&) = delete;
		Engine& operator=(const Engine&) = delete;

		/**
		 * Run the game loop with the given application.
		 * @param app The application to run (must not be null)
		 * @param config Engine configuration settings
		 */
		void Run(Application* app, const EngineConfig& config = {});

		/**
		 * Request the engine to quit at the end of the current frame.
		 */
		void Quit();

		// Subsystem accessors
		GLFWManager& GetWindow();
		Renderer& GetRenderer();
		UIManager& GetUIManager();

		/**
		 * Get the delta time (seconds) since the last frame.
		 */
		float GetDeltaTime() const { return m_DeltaTime; }

	private:
		Engine() = default;
		~Engine() = default;

		/**
		 * Initialize the engine and all subsystems.
		 * @return true if initialization succeeded
		 */
		bool Init(const EngineConfig& config);

		/**
		 * Shutdown the engine and clean up subsystems.
		 */
		void Shutdown();

		// Subsystems
		std::unique_ptr<GLFWManager> m_Window;
		std::unique_ptr<Renderer> m_Renderer;
		std::unique_ptr<UIManager> m_UIManager;

		float m_DeltaTime = 0.0f;
		bool m_Running = false;
	};
}
