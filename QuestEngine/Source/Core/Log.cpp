#include "Core/Log.h"

#include <vector>
#include <string>
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/basic_file_sink.h"


namespace QE
{
	// Create a default engine logger that is guaranteed to exist
	Log::Log()
	{
		std::vector<spdlog::sink_ptr> sinks =
		{
			std::make_shared<spdlog::sinks::basic_file_sink_mt>("logs/EngineLog.txt", true),
			std::make_shared<spdlog::sinks::stdout_color_sink_mt>()
		};

		sinks[0]->set_pattern("[%T] [%l] [%n]: %v");
		sinks[1]->set_pattern("%^[%T] [%n]: %v%$");

		auto logger = std::make_shared<spdlog::logger>("Engine", begin(sinks), end(sinks));
		logger->set_level(spdlog::level::trace);
		AddLogger("Engine", logger);
		logger->log(spdlog::level::info, "Engine logger initialized");
	}

	void Log::AddLogger(const std::string_view loggerName, std::shared_ptr<spdlog::logger> logger)
	{
		m_Loggers.emplace(loggerName, logger);
	}

	void Log::AddLogger(const std::string_view loggerName)
	{
		std::string loggerPath = "logs/";
		loggerPath.append(loggerName.data()).append(".txt");

		std::vector<spdlog::sink_ptr> sinks =
		{
			std::make_shared<spdlog::sinks::basic_file_sink_mt>(loggerPath, true),
			std::make_shared<spdlog::sinks::stdout_color_sink_mt>()
		};

		sinks[0]->set_pattern("[%T] [%l] [%n]: %v");
		sinks[1]->set_pattern("%^[%T] [%n]: %v%$");

		auto logger = std::make_shared<spdlog::logger>(loggerName.data(), begin(sinks), end(sinks));
		logger->set_level(spdlog::level::trace);
		AddLogger(loggerName, logger);
		logger->log(spdlog::level::info, "{} logger initialized", loggerName.data());
	}

	std::shared_ptr<spdlog::logger> Log::GetLogger(const std::string_view loggerName)
	{
		auto it = m_Loggers.find(std::string(loggerName));
		if (it != m_Loggers.end())
		{
			return it->second;
		}
		return m_Loggers["Engine"];
	}

	spdlog::logger* Log::_GetLoggerAsPointer(const std::string_view loggerName)
	{
		auto it = m_Loggers.find(std::string(loggerName));
		if (it != m_Loggers.end())
		{
			return it->second.get();
		}
		return m_Loggers["Engine"].get();
	}

	void Log::SetLoggerLevel(const std::string_view loggerName, const Level level)
	{
		auto logger = _GetLoggerAsPointer(loggerName);
		logger->set_level(static_cast<spdlog::level::level_enum>(level));
	}
}