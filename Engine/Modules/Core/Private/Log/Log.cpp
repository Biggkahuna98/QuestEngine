#include "Core/Log/Log.h"

namespace Quest
{
    const char* GetLogVerbosityString(LogVerbosity verbosity)
    {
        switch (verbosity)
        {
        case LogVerbosity::Fatal: return "Fatal";
        case LogVerbosity::Error: return "Error";
        case LogVerbosity::Warning: return "Warning";
        case LogVerbosity::Info: return "Info";
        case LogVerbosity::Verbose: return "Verbose";
        //case LogVerbosity::Debug: return "Debug";
        default: return "Unknown";
        }
    }

    LogCategoryBase::LogCategoryBase(const char* categoryName, LogVerbosity verbosity)
        : m_Name(categoryName)
        , m_Verbosity(verbosity)
    {
        LogRegistry::Get().Register(this);
    }

    LogCategoryBase::~LogCategoryBase()
    {
        LogRegistry::Get().Unregister(this);
    }

    void LogCategoryBase::SetVerbosity(LogVerbosity verbosity)
    {
        m_Verbosity = verbosity;
    }

    void LogCategoryBase::AddSink(LogSinkBase* sink)
    {
        m_Sinks.push_back(std::unique_ptr<LogSinkBase>(sink));
    }

    void LogCategoryBase::Log(LogVerbosity verbosity, std::string_view message)
    {
        if (verbosity >= m_Verbosity)
        {
            for (auto& sink : m_Sinks)
                sink->Log(this, verbosity, message);
        }
    }

    LogRegistry& LogRegistry::Get()
    {
        static LogRegistry instance;
        return instance;
    }

    void LogRegistry::Shutdown()
    {
        std::lock_guard<std::mutex> lock(m_Mutex);
        for (auto category : m_Categories)
            category->ClearSinks();
        std::vector<LogCategoryBase*>().swap(m_Categories);
    }

    void LogRegistry::Register(LogCategoryBase* category)
    {
        std::lock_guard<std::mutex> lock(m_Mutex);
        m_Categories.push_back(category);
    }

    void LogRegistry::Unregister(LogCategoryBase* category)
    {
        std::lock_guard<std::mutex> lock(m_Mutex);
        m_Categories.erase(std::remove(m_Categories.begin(), m_Categories.end(), category), m_Categories.end());
    }

    bool LogRegistry::SetVerbosity(const char* categoryName, LogVerbosity verbosity)
    {
        std::lock_guard<std::mutex> lock(m_Mutex);
        for (auto category : m_Categories)
        {
            if (strcmp(category->GetName(), categoryName) == 0)
            {
                category->SetVerbosity(verbosity);
                return true;
            }
        }
        return false;
    }

    LogCategoryBase* LogRegistry::GetCategory(const char* categoryName)
    {
        for (auto category : m_Categories)
        {
            if (strcmp(category->GetName(), categoryName) == 0)
                return category;
        }
        return nullptr;
    }
}
