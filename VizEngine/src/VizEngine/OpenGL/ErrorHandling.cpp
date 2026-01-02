#include "ErrorHandling.h"
#include "VizEngine/Log.h"

namespace VizEngine
{
	// Error handling for OpenGL 4.3 and above https://docs.gl/gl4/glDebugMessageCallback
	void GLAPIENTRY ErrorHandling::ErrorHandler(GLenum source,
		GLenum type,
		GLuint id,
		GLenum severity,
		GLsizei length,
		const GLchar* message,
		const void* userParam)
	{
		// Suppress unused parameter warnings
		(void)length;
		(void)userParam;

		// Ignore notifications - they're just informational messages
		if (severity == GL_DEBUG_SEVERITY_NOTIFICATION)
			return;

		// Determine severity label
		const char* severityStr;
		switch (severity)
		{
			case GL_DEBUG_SEVERITY_HIGH:   severityStr = "HIGH";    break;
			case GL_DEBUG_SEVERITY_MEDIUM: severityStr = "MEDIUM";  break;
			case GL_DEBUG_SEVERITY_LOW:    severityStr = "LOW";     break;
			default:                       severityStr = "UNKNOWN"; break;
		}

		// Determine source label
		const char* sourceStr;
		switch (source)
		{
			case GL_DEBUG_SOURCE_API:             sourceStr = "API";             break;
			case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   sourceStr = "Window System";   break;
			case GL_DEBUG_SOURCE_SHADER_COMPILER: sourceStr = "Shader Compiler"; break;
			case GL_DEBUG_SOURCE_THIRD_PARTY:     sourceStr = "Third Party";     break;
			case GL_DEBUG_SOURCE_APPLICATION:     sourceStr = "Application";     break;
			case GL_DEBUG_SOURCE_OTHER:           sourceStr = "Other";           break;
			default:                              sourceStr = "Unknown";         break;
		}

		// Determine type label
		const char* typeStr;
		switch (type)
		{
			case GL_DEBUG_TYPE_ERROR:               typeStr = "Error";               break;
			case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: typeStr = "Deprecated Behavior"; break;
			case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  typeStr = "Undefined Behavior";  break;
			case GL_DEBUG_TYPE_PORTABILITY:         typeStr = "Portability";         break;
			case GL_DEBUG_TYPE_PERFORMANCE:         typeStr = "Performance";         break;
			case GL_DEBUG_TYPE_MARKER:              typeStr = "Marker";              break;
			case GL_DEBUG_TYPE_OTHER:               typeStr = "Other";               break;
			default:                                typeStr = "Unknown";             break;
		}

		VP_CORE_WARN("OpenGL Debug [{}, {}]: {} (Severity: {}): {}", 
			sourceStr, typeStr, id, severityStr, message);

		// Only exit on high severity errors
		if (severity == GL_DEBUG_SEVERITY_HIGH)
		{
			VP_CORE_ERROR("Fatal OpenGL error - shutting down.");
			exit(-1);
		}
	}

	void ErrorHandling::HandleErrors()
	{
		// Error handling for OpenGL 4.3 and above https://docs.gl/gl4/glDebugMessageCallback
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback(ErrorHandling::ErrorHandler, NULL);
	}
}
