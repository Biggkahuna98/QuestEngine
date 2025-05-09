#pragma once

#include "Core.h"
#include <string>
#include <memory>
#include <unordered_map>

#include "spdlog/spdlog.h"

namespace QE
{
	extern std::string s_ClientLoggerName;
}

// Macros for logging
// Prefer to use the tagged macros for better readability/usability to track down errors
#ifdef QE_INTERNAL_USE_ONLY // These log macros assume that the logger is "Engine" for internal library use, otherwise the logger name must be specified in the client application
	#define LOG_TRACE(...) QE::Log::PrintMessage("Engine", QE::Log::Level::Trace, __VA_ARGS__)
	#define LOG_DEBUG(...) QE::Log::PrintMessage("Engine", QE::Log::Level::Debug, __VA_ARGS__)
	#define LOG_INFO(...) QE::Log::PrintMessage("Engine", QE::Log::Level::Info, __VA_ARGS__)
	#define LOG_WARN(...) QE::Log::PrintMessage("Engine", QE::Log::Level::Warning, __VA_ARGS__)
	#define LOG_ERROR(...) QE::Log::PrintMessage("Engine", QE::Log::Level::Error, __VA_ARGS__)
	#define LOG_FATAL(...) QE::Log::PrintMessage("Engine", QE::Log::Level::Fatal, __VA_ARGS__)

	#define LOG_TRACE_TAG(tag, ...) QE::Log::PrintMessageTag("Engine", QE::Log::Level::Trace, tag, __VA_ARGS__)
	#define LOG_DEBUG_TAG(tag, ...) QE::Log::PrintMessageTag("Engine", QE::Log::Level::Debug, tag, __VA_ARGS__)
	#define LOG_INFO_TAG(tag, ...) QE::Log::PrintMessageTag("Engine", QE::Log::Level::Info, tag, __VA_ARGS__)
	#define LOG_WARN_TAG(tag, ...) QE::Log::PrintMessageTag("Engine", QE::Log::Level::Warning, tag, __VA_ARGS__)
	#define LOG_ERROR_TAG(tag, ...) QE::Log::PrintMessageTag("Engine", QE::Log::Level::Error, tag, __VA_ARGS__)
	#define LOG_FATAL_TAG(tag, ...) QE::Log::PrintMessageTag("Engine", QE::Log::Level::Fatal, tag, __VA_ARGS__)
	
	#define LOG_ASSERT(failurePrefix, ...) QE::Log::PrintAssertMessage("Engine", failurePrefix, __VA_ARGS__)
#else
	#define LOG_TRACE(...) QE::Log::PrintMessage(QE::s_ClientLoggerName, QE::Log::Level::Trace, __VA_ARGS__)
	#define LOG_DEBUG(...) QE::Log::PrintMessage(QE::s_ClientLoggerName, QE::Log::Level::Debug, __VA_ARGS__)
	#define LOG_INFO(...) QE::Log::PrintMessage(QE::s_ClientLoggerName, QE::Log::Level::Info, __VA_ARGS__)
	#define LOG_WARN(...) QE::Log::PrintMessage(QE::s_ClientLoggerName, QE::Log::Level::Warning, __VA_ARGS__)
	#define LOG_ERROR(...) QE::Log::PrintMessage(QE::s_ClientLoggerName, QE::Log::Level::Error, __VA_ARGS__)
	#define LOG_FATAL(...) QE::Log::PrintMessage(QE::s_ClientLoggerName, QE::Log::Level::Fatal, __VA_ARGS__)

	#define LOG_TRACE_TAG(tag, ...) QE::Log::PrintMessageTag(QE::s_ClientLoggerName, QE::Log::Level::Trace, tag, __VA_ARGS__)
	#define LOG_DEBUG_TAG(tag, ...) QE::Log::PrintMessageTag(QE::s_ClientLoggerName, QE::Log::Level::Debug, tag, __VA_ARGS__)
	#define LOG_INFO_TAG(tag, ...) QE::Log::PrintMessageTag(QE::s_ClientLoggerName, QE::Log::Level::Info, tag, __VA_ARGS__)
	#define LOG_WARN_TAG(tag, ...) QE::Log::PrintMessageTag(QE::s_ClientLoggerName, QE::Log::Level::Warning, tag, __VA_ARGS__)
	#define LOG_ERROR_TAG(tag, ...) QE::Log::PrintMessageTag(QE::s_ClientLoggerName, QE::Log::Level::Error, tag, __VA_ARGS__)
	#define LOG_FATAL_TAG(tag, ...) QE::Log::PrintMessageTag(QE::s_ClientLoggerName, QE::Log::Level::Fatal, tag, __VA_ARGS__)

	#define LOG_ASSERT(failurePrefix, ...) QE::Log::PrintAssertMessage(QE::s_ClientLoggerName, failurePrefix, __VA_ARGS__)
#endif

namespace QE
{
	class QUEST_API Log
	{
	public:
		enum class Level : uint8_t
		{
			Trace = 0, Debug, Info, Warning, Error, Fatal
		};

		static void Init();

		static void AddLogger(const std::string_view loggerName);
		static void AddLogger(const std::string_view loggerName, std::shared_ptr<spdlog::logger> logger);
		static std::shared_ptr<spdlog::logger> GetLogger(const std::string_view loggerName);
		static spdlog::logger* _GetLoggerAsPointer(const std::string_view loggerName);

		static void SetLoggerLevel(const std::string_view loggerName, const Level level);
		static void FlushAllLoggers();

		template<typename... Args>
		static constexpr void PrintMessage(const std::string_view loggerName, const Log::Level level, fmt::format_string<Args...> format, Args&&... args);
		template<typename... Args>
		static constexpr void PrintMessageTag(const std::string_view loggerName, const Log::Level level, const std::string_view tag, fmt::format_string<Args...> format, Args&&... args);
		template<typename... Args>
		static constexpr void PrintAssertMessage(const std::string_view loggerName, const std::string_view failurePrefix, fmt::format_string<Args...> format, Args&&... args);

	private:
		static std::unordered_map<std::string, std::shared_ptr<spdlog::logger>> s_Loggers;
	};

	template<typename ...Args>
	inline constexpr void Log::PrintMessage(const std::string_view loggerName, const Log::Level level, fmt::format_string<Args...> format, Args&&... args)
	{
		auto logger = _GetLoggerAsPointer(loggerName);
		const std::string formatted = fmt::format(format, std::forward<Args>(args)...);
		switch (level)
		{
		case Level::Trace:
			logger->trace(formatted);
			break;
		case Level::Debug:
			logger->debug(formatted);
			break;
		case Level::Info:
			logger->info(formatted);
			break;
		case Level::Warning:
			logger->warn(formatted);
			break;
		case Level::Error:
			logger->error(formatted);
			break;
		case Level::Fatal:
			logger->critical(formatted);
			break;
		}
	}

	template<typename ...Args>
	inline constexpr void Log::PrintMessageTag(const std::string_view loggerName, const Log::Level level, const std::string_view tag, fmt::format_string<Args...> format, Args&&... args)
	{
		auto logger = _GetLoggerAsPointer(loggerName);
		const std::string formatted = fmt::format(format, std::forward<Args>(args)...);
		switch (level)
		{
		case Level::Trace:
			logger->trace("[{0}] - {1}", tag, formatted);
			break;
		case Level::Debug:
			logger->debug("[{0}] - {1}", tag, formatted);
			break;
		case Level::Info:
			logger->info("[{0}] - {1}", tag, formatted);
			break;
		case Level::Warning:
			logger->warn("[{0}] - {1}", tag, formatted);
			break;
		case Level::Error:
			logger->error("[{0}] - {1}", tag, formatted);
			break;
		case Level::Fatal:
			logger->critical("[{0}] - {1}", tag, formatted);
			break;
		}
	}

	template<typename... Args>
	inline constexpr void Log::PrintAssertMessage(const std::string_view loggerName, const std::string_view failurePrefix, fmt::format_string<Args...> format, Args&&... args)
	{
		auto logger = _GetLoggerAsPointer(loggerName);
		const std::string formatted = fmt::format(format, std::forward<Args>(args)...);
		logger->error("{0}: {1}", failurePrefix, formatted);
	}
}