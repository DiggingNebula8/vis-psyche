#pragma once

#ifdef VP_PLATFORM_WINDOWS

#include "Engine.h"
#include "Application.h"  // For CreateApplication declaration

int main()
{
	VizEngine::Log::Init();

	VizEngine::EngineConfig config;
	auto app = VizEngine::CreateApplication(config);
	VizEngine::Engine::Get().Run(std::move(app), config);

	return 0;
}

#endif
