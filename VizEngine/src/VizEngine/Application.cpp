#include "Application.h"

namespace VizEngine
{
	// Out-of-line definitions for exported class with virtual destructor.
	// This ensures proper vtable generation in the DLL.
	Application::Application() = default;
	Application::~Application() = default;
}
