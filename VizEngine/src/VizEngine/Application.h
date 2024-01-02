#pragma once

#include"Core.h"

namespace VizEngine
{
	class VizEngine_API Application
	{
	public:
		Application();
		virtual ~Application();

		int Run();
	};

	Application* CreateApplication();
}


