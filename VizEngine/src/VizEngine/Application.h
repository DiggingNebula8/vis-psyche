#pragma once
#include <memory>
#include"Core.h"
#include"Events/Event.h"
#include"Window.h"

namespace VizEngine
{
	class VizEngine_API Application
	{
	public:
		Application();
		virtual ~Application();

		int Run();

		void OnEvent(Event& e);

	private:
		std::unique_ptr<Window> m_Window;
		bool m_Running = true;
	};

	Application* CreateApplication();
}


