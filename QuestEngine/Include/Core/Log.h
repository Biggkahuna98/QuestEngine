#pragma once

#include <string>
#include <memory>
#include <unordered_map>

#include "spdlog/spdlog.h"

// Macros for logging
#define LOG(logger, level, ...) QE::Log::Get().PrintMessage(#logger, QE::Log::Level::##level, __VA_ARGS__)
#define LOG_TAG(logger, level, tag, ...) QE::Log::Get().PrintMessageTag(#logger, QE::Log::Level::##level, tag, __VA_ARGS__)
#define LOG_ASSERT(logger, failurePrefix, ...) QE::Log::Get().PrintAssertMessage(#logger, failurePrefix, __VA_ARGS__)

namespace QE
{
	class Log
	{
	public:
		enum class Level : uint8_t
		{
			Trace = 0, Debug, Info, Warning, Error, Fatal
		};

		static Log& Get()
		{
			static Log instance;
			return instance;
		}

		void AddLogger(const std::string_view loggerName, std::shared_ptr<spdlog::logger> logger);
		std::shared_ptr<spdlog::logger> GetLogger(const std::string_view loggerName);
		spdlog::logger* _GetLoggerAsPointer(const std::string_view loggerName);

		void SetLoggerLevel(const std::string_view loggerName, const Level level);

		template<typename... Args>
		constexpr void PrintMessage(const std::string_view loggerName, const Log::Level level, fmt::format_string<Args...> format, Args&&... args);
		template<typename... Args>
		constexpr void PrintMessageTag(const std::string_view loggerName, const Log::Level level, const std::string_view tag, fmt::format_string<Args...> format, Args&&... args);
		template<typename... Args>
		constexpr void PrintAssertMessage(const std::string_view loggerName, const std::string_view failurePrefix, fmt::format_string<Args...> format, Args&&... args);

	private:
		Log();
		Log(const Log&) = delete;
		Log& operator=(const Log&) = delete;

		std::unordered_map<std::string, std::shared_ptr<spdlog::logger>> m_Loggers;
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