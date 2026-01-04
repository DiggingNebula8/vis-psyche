#pragma once

#include <memory>
#include "Core.h"

namespace VizEngine
{
	// Forward declarations
	struct EngineConfig;
	class Event;

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
		 * Called when an event occurs (window resize, input, etc.).
		 * Use EventDispatcher to handle specific event types.
		 * @param e The event to handle
		 */
		virtual void OnEvent(Event& e) { (void)e; }

		/**
		 * Called once after the game loop ends.
		 * Use for cleanup (though RAII handles most cases).
		 */
		virtual void OnDestroy() {}
	};

	/**
	 * Factory function implemented by client applications.
	 * @param config Engine configuration that the application can modify.
	 * @return A new Application instance (ownership transferred via unique_ptr).
	 */
	std::unique_ptr<Application> CreateApplication(EngineConfig& config);
}
