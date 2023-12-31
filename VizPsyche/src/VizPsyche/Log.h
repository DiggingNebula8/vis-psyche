#pragma once

#include<memory>
#include"Core.h"
#include"spdlog/spdlog.h"

namespace VizPsyche
{
	class VizPsyche_API Log
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
#define VP_CORE_TRACE(...)		::VizPsyche::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define VP_CORE_INFO(...)		::VizPsyche::Log::GetCoreLogger()->info(__VA_ARGS__)
#define VP_CORE_WARN(...)		::VizPsyche::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define VP_CORE_ERROR(...)		::VizPsyche::Log::GetCoreLogger()->error(__VA_ARGS__)
#define VP_CORE_CRITICAL(...)	::VizPsyche::Log::GetCoreLogger()->critical(__VA_ARGS__)

//client
#define VP_TRACE(...)		::VizPsyche::Log::GetClientLogger()->trace(__VA_ARGS__)
#define VP_INFO(...)		::VizPsyche::Log::GetClientLogger()->info(__VA_ARGS__)
#define VP_WARN(...)		::VizPsyche::Log::GetClientLogger()->warn(__VA_ARGS__)
#define VP_ERROR(...)		::VizPsyche::Log::GetClientLogger()->error(__VA_ARGS__)
#define VP_CRITICAL(...)	::VizPsyche::Log::GetClientLogger()->critical(__VA_ARGS__)


