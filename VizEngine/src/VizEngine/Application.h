#pragma once

#include "Core.h"

namespace VizEngine
{
	// Forward declaration
	struct EngineConfig;

	/**
	 * Base class for game applications.
	 * Inherit from this class and override lifecycle methods to implement your game.
	 */
	class VizEngine_API Application
	{
	public:
		Application();
		virtual ~Application();

		// Non-copyable
		Application(const Application&) = delete;
		Application& operator=(const Application&) = delete;

		/**
		 * Called once before the game loop starts.
		 * Use for loading assets, creating the scene, and initial setup.
		 */
		virtual void OnCreate() {}

		/**
		 * Called every frame.
		 * @param deltaTime Time in seconds since the last frame.
		 * Use for game logic, camera controllers, physics updates.
		 */
		virtual void OnUpdate(float deltaTime) { (void)deltaTime; }

		/**
		 * Called every frame after OnUpdate.
		 * Use for rendering the scene.
		 */
		virtual void OnRender() {}

		/**
		 * Called every frame after OnRender.
		 * Use for ImGui panels and debug UI.
		 */
		virtual void OnImGuiRender() {}

		/**
		 * Called once after the game loop ends.
		 * Use for cleanup (though RAII handles most cases).
		 */
		virtual void OnDestroy() {}
	};

	/**
	 * Factory function implemented by client applications.
	 * @param config Engine configuration that the application can modify.
	 * @return A new Application instance.
	 */
	Application* CreateApplication(EngineConfig& config);
}
