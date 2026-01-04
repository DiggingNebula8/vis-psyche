#pragma once

#ifdef VP_PLATFORM_WINDOWS
	#ifdef VP_BUILD_DLL
		#define VizEngine_API __declspec(dllexport)
	#else
		#define VizEngine_API __declspec(dllimport)
	#endif
#elif defined(__GNUC__) || defined(__clang__)
	#define VizEngine_API __attribute__((visibility("default")))
#else
	#define VizEngine_API  // Fallback for unknown compilers
#endif
