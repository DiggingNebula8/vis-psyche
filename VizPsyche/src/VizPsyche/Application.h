#pragma once

#include"Core.h"

namespace VizPsyche
{
	class VizPsyche_API Application
	{
	public:
		Application();
		virtual ~Application();

		void Run();
	};

	Application* CreateApplication();
}


