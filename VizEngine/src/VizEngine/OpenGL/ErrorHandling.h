#pragma once

#include "Commons.h"
#include "VizEngine/Core.h"



namespace VizEngine
{
	class VizEngine_API ErrorHandling
	{
	public:
		static void GLAPIENTRY ErrorHandler(GLenum source,
			GLenum type,
			GLuint id,
			GLenum severity,
			GLsizei length,
			const GLchar* message,
			const void* userParam);

		static void HandleErrors();
	};
}
