#include "vepch.h"
#include "Log.h"

#include <spdlog/sinks/stdout_color_sinks.h>

namespace VizEngine
{
	std::shared_ptr<spdlog::logger> Log::s_CoreLogger;
	std::shared_ptr<spdlog::logger> Log::s_ClientLogger;

	void Log::Init()
	{
		// Set the global log pattern
		// [%Y-%m-%d %H:%M:%S.%e]: Date and time of the log message
		// - %Y-%m-%d: Year-Month-Day
		// - %H:%M:%S.%e: Hour:Minute:Second.Milliseconds
		// This provides precise timing information for each log entry.

		// [thread %t]: Thread ID
		// - %t: Thread identifier
		// This is useful in multi-threaded applications to identify which thread generated the log message.

		// [%^%l%$]: Log level with color
		// - %^: Start color range
		// - %l: Log level (info, warn, error, etc.)
		// - %$: End color range
		// The log level part is colorized to quickly distinguish between different types of log messages.

		// %v: The actual log message
		spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%e] [thread %t] [%^%l%$] %v");

		// Initialize Core Logger
		s_CoreLogger = spdlog::stdout_color_mt("VizPsyche");

		// Initialize Client Logger
		s_ClientLogger = spdlog::stdout_color_mt("Client");

		// Default log level setup
		SetCoreLogLevel(LogLevel::Trace);
		SetClientLogLevel(LogLevel::Debug);

		// Log initialization messages
		VE_CORE_INFO("Core Logger initialised");
		VE_CORE_INFO("Client Logger initialised");
	}

	void Log::SetCoreLogLevel(LogLevel level)
	{
		s_CoreLogger->set_level(EnumToLogLevel(level));
	}

	void Log::SetClientLogLevel(LogLevel level)
	{
		s_ClientLogger->set_level(EnumToLogLevel(level));
	}

	spdlog::level::level_enum Log::EnumToLogLevel(LogLevel level)
	{
		switch (level)
		{
		case LogLevel::Trace: return spdlog::level::trace;
		case LogLevel::Debug: return spdlog::level::debug;
		case LogLevel::Info: return spdlog::level::info;
		case LogLevel::Warn: return spdlog::level::warn;
		case LogLevel::Error: return spdlog::level::err;
		case LogLevel::Critical: return spdlog::level::critical;
		case LogLevel::Off: return spdlog::level::off;
		default: return spdlog::level::info; // Default case
		}
	}
}