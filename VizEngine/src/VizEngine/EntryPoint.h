#pragma once

#ifdef VP_PLATFORM_WINDOWS

namespace VizEngine
{
	// Implemented by the client application (e.g., Sandbox)
	extern Application* CreateApplication();
}

int main(int argc, char** argv)
{
	VizEngine::Log::Init();
	auto app = VizEngine::CreateApplication();
	app->Run();
	delete app;
}

#endif
