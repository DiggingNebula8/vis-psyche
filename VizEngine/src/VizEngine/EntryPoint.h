#pragma once

#ifdef VP_PLATFORM_WINDOWS

#include "Engine.h"
#include "Application.h"  // For CreateApplication declaration

int main(int argc, char** argv)
{
	(void)argc;
	(void)argv;

	VizEngine::Log::Init();

	VizEngine::EngineConfig config;
	auto app = VizEngine::CreateApplication(config);
	VizEngine::Engine::Get().Run(app, config);
	delete app;

	return 0;
}

#endif
