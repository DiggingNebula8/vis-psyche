#pragma once
#include"Window.h"
#include"LayerStack.h"
#include"Events/Event.h"
#include"Events/AppEvent.h"

namespace VizEngine
{
	class VizEngine_API Application
	{
	public:
		Application();
		virtual ~Application();

		int Run();

		void OnEvent(Event& e);

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* layer);

	private:
		bool OnWindowClose(WindowCloseEvent& e);
		std::unique_ptr<Window> m_Window;
		bool m_Running = true;
		LayerStack m_LayerStack;
	};

	Application* CreateApplication();
}


