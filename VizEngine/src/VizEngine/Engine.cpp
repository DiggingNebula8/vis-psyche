#include "Engine.h"
#include "Application.h"
#include "Log.h"
#include "Events/Event.h"

#include "OpenGL/GLFWManager.h"
#include "OpenGL/Renderer.h"
#include "OpenGL/ErrorHandling.h"
#include "GUI/UIManager.h"
#include "Core/Input.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace VizEngine
{
	Engine& Engine::Get()
	{
		static Engine instance;
		return instance;
	}

	void Engine::Run(Application* app, const EngineConfig& config)
	{
		// Note: Caller retains ownership of app pointer (see EntryPoint.h)
		if (!app)
		{
			VP_CORE_ERROR("Engine::Run called with null application!");
			return;
		}

		m_App = app;  // Store for event routing

		if (!Init(config))
		{
			VP_CORE_ERROR("Engine initialization failed!");
			return;
		}

		m_Running = true;

		bool appCreated = false;
		try
		{
			// Application initialization
			app->OnCreate();
			appCreated = true;

			double prevTime = glfwGetTime();

			// Main game loop
			while (m_Running && !m_Window->WindowShouldClose())
			{
				// Delta time calculation
				double currentTime = glfwGetTime();
				m_DeltaTime = static_cast<float>(currentTime - prevTime);
				prevTime = currentTime;

				// Input phase
				m_Window->ProcessInput();
				m_UIManager->BeginFrame();

				// Application hooks
				app->OnUpdate(m_DeltaTime);
				app->OnRender();
				app->OnImGuiRender();

				// Present phase
				m_UIManager->Render();
				m_Window->SwapBuffersAndPollEvents();
				Input::EndFrame();  // Reset scroll delta after events polled
			}

			// Application cleanup (normal exit)
			app->OnDestroy();
		}
		catch (const std::exception& e)
		{
			VP_CORE_ERROR("Exception in engine loop: {}", e.what());
			if (appCreated)
			{
				app->OnDestroy();
			}
		}
		catch (...)
		{
			VP_CORE_ERROR("Unknown exception in engine loop");
			if (appCreated)
			{
				app->OnDestroy();
			}
		}

		Shutdown();
	}

	void Engine::Quit()
	{
		m_Running = false;
	}

	GLFWManager& Engine::GetWindow()
	{
		VP_CORE_ASSERT(m_Window, "Engine not initialized or already shut down!");
		return *m_Window;
	}

	Renderer& Engine::GetRenderer()
	{
		VP_CORE_ASSERT(m_Renderer, "Engine not initialized or already shut down!");
		return *m_Renderer;
	}

	UIManager& Engine::GetUIManager()
	{
		VP_CORE_ASSERT(m_UIManager, "Engine not initialized or already shut down!");
		return *m_UIManager;
	}

	bool Engine::Init(const EngineConfig& config)
	{
		// Guard against double initialization
		if (m_Window)
		{
			VP_CORE_WARN("Engine::Init called while already initialized!");
			return true;
		}

		VP_CORE_INFO("Initializing Engine...");

		// Create window
		m_Window = std::make_unique<GLFWManager>(
			config.Width, 
			config.Height, 
			config.Title.c_str()
		);

		// Initialize GLAD
		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		{
			VP_CORE_ERROR("Failed to initialize GLAD");
			m_Window.reset();
			return false;
		}

		// OpenGL state setup
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_DEPTH_TEST);

		// Wire event callback
		m_Window->SetEventCallback([this](Event& e) {
			OnEvent(e);
		});

		// Create subsystems
		m_UIManager = std::make_unique<UIManager>(m_Window->GetWindow());
		m_Renderer = std::make_unique<Renderer>();

		// Enable OpenGL debug output
		ErrorHandling::HandleErrors();

		VP_CORE_INFO("Engine initialized successfully");
		return true;
	}

	void Engine::OnEvent(Event& e)
	{
		// Give ImGui first chance to handle events
		m_UIManager->OnEvent(e);

		// Only forward to application if not consumed
		if (!e.Handled && m_App)
		{
			m_App->OnEvent(e);
		}
	}

	void Engine::Shutdown()
	{
		VP_CORE_INFO("Shutting down Engine...");

		// Reset subsystems in reverse order of creation
		m_Renderer.reset();
		m_UIManager.reset();
		m_Window.reset();
		m_App = nullptr;

		VP_CORE_INFO("Engine shutdown complete");
	}
}
