#pragma once

#include<memory>
#include"Core.h"
#include"spdlog/spdlog.h"

namespace VizEngine
{
	class VizEngine_API Log
	{
	public:
		enum class LogLevel
		{
			Trace, Debug, Info, Warn, Error, Critical, Off
		};


		static void Init();

		static void SetCoreLogLevel(LogLevel level);
		static void SetClientLogLevel(LogLevel level);

		inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
		inline static std::shared_ptr<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }

	private:
		static spdlog::level::level_enum EnumToLogLevel(LogLevel level);
		static std::shared_ptr<spdlog::logger> s_CoreLogger;
		static std::shared_ptr<spdlog::logger> s_ClientLogger;
	};
}
//core
#define VP_CORE_TRACE(...)		::VizEngine::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define VP_CORE_INFO(...)		::VizEngine::Log::GetCoreLogger()->info(__VA_ARGS__)
#define VP_CORE_WARN(...)		::VizEngine::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define VP_CORE_ERROR(...)		::VizEngine::Log::GetCoreLogger()->error(__VA_ARGS__)
#define VP_CORE_CRITICAL(...)	::VizEngine::Log::GetCoreLogger()->critical(__VA_ARGS__)

//client
#define VP_TRACE(...)		::VizEngine::Log::GetClientLogger()->trace(__VA_ARGS__)
#define VP_INFO(...)		::VizEngine::Log::GetClientLogger()->info(__VA_ARGS__)
#define VP_WARN(...)		::VizEngine::Log::GetClientLogger()->warn(__VA_ARGS__)
#define VP_ERROR(...)		::VizEngine::Log::GetClientLogger()->error(__VA_ARGS__)
#define VP_CRITICAL(...)	::VizEngine::Log::GetClientLogger()->critical(__VA_ARGS__)


