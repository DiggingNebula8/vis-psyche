#pragma once

#ifdef VP_PLATFORM_WINDOWS

extern VizPsyche::Application* CreateApplication();

int  main(int argc, char** argv)
{
	auto app = VizPsyche::CreateApplication();
	app->Run();
	delete app;
}

#endif
