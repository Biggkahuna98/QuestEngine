#include "Core/Log.h"

#include <vector>
#include <string>
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/basic_file_sink.h"

std::unordered_map<std::string, std::shared_ptr<spdlog::logger>> QE::Log::s_Loggers;
std::string QE::s_ClientLoggerName = "Application";

namespace QE
{
	// Create a default engine logger that is guaranteed to exist
	void Log::Init()
	{
		AddLogger("Engine");
		AddLogger(s_ClientLoggerName);
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

	void Log::AddLogger(const std::string_view loggerName, std::shared_ptr<spdlog::logger> logger)
	{
		s_Loggers.emplace(loggerName, logger);
	}

	std::shared_ptr<spdlog::logger> Log::GetLogger(const std::string_view loggerName)
	{
		auto it = s_Loggers.find(std::string(loggerName));
		if (it != s_Loggers.end())
		{
			return it->second;
		}
		return s_Loggers["Engine"];
	}

	spdlog::logger* Log::_GetLoggerAsPointer(const std::string_view loggerName)
	{
		auto it = s_Loggers.find(std::string(loggerName));
		if (it != s_Loggers.end())
		{
			return it->second.get();
		}
		return s_Loggers["Engine"].get();
	}

	void Log::SetLoggerLevel(const std::string_view loggerName, const Level level)
	{
		auto logger = _GetLoggerAsPointer(loggerName);
		logger->set_level(static_cast<spdlog::level::level_enum>(level));
	}
}