#pragma once

#ifdef VP_PLATFORM_WINDOWS

extern VizEngine::Application* CreateApplication();

int  main(int argc, char** argv)
{
	VizEngine::Log::Init();
	auto app = VizEngine::CreateApplication();
	app->Run();
	delete app;
}

#endif
