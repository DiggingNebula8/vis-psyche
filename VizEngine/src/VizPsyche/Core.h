#pragma once

#ifdef VP_PLATFORM_WINDOWS
	#ifdef  VP_BUILD_DLL
		#define VizPsyche_API __declspec(dllexport)
	#else
		#define VizPsyche_API __declspec(dllimport)
	#endif
#else
	#error MyEngine only supports windows!
#endif
