#pragma once

#ifdef VP_PLATFORM_WINDOWS
	#ifdef VP_BUILD_DLL
		#define VizEngine_API __declspec(dllexport)
	#else
		#define VizEngine_API __declspec(dllimport)
	#endif
#else
	#error VizEngine only supports windows!
#endif
