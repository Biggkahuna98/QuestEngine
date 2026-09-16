#pragma once

#include "Core/CoreExport.h"
#include <cstdint>
#include <string_view>
#include <vector>
#include <mutex>
#include <format>

namespace Quest
{
    enum class LogVerbosity : uint8_t
    {
        Fatal = 0,
        Error,
        Warning,
        Info,
        Verbose,
        Count
    };

    const char* GetLogVerbosityString(LogVerbosity verbosity);

    class LogCategoryBase;

    class CORE_API LogSinkBase
    {
    public:
        LogSinkBase() = default;
        virtual ~LogSinkBase() = default;
        virtual void Log(LogCategoryBase* category, LogVerbosity verbosity, std::string_view message) = 0;
    };

    class CORE_API LogCategoryBase
    {
    public:
        LogCategoryBase(const char* categoryName, LogVerbosity verbosity);
        virtual ~LogCategoryBase();

        LogCategoryBase(const LogCategoryBase&) = delete;
        LogCategoryBase& operator=(const LogCategoryBase&) = delete;

        void SetVerbosity(LogVerbosity verbosity);
        LogVerbosity GetVerbosity() const { return m_Verbosity; }
        const char* GetName() const { return m_Name; }

        void AddSink(LogSinkBase* sink);
        void ClearSinks() { m_Sinks.clear(); }

        void Log(LogVerbosity verbosity, std::string_view message);
    private:
        const char* m_Name;
        LogVerbosity m_Verbosity;

        std::vector<std::unique_ptr<LogSinkBase>> m_Sinks;
    };

    class CORE_API LogRegistry
    {
    public:
        static LogRegistry& Get();

        void Shutdown();

        void Register(LogCategoryBase* category);
        void Unregister(LogCategoryBase* category);

        // True if set, false if no category match
        bool SetVerbosity(const char* categoryName, LogVerbosity verbosity);

        // Nullptr if not in the registry
        LogCategoryBase* GetCategory(const char* categoryName);
    private:
        LogRegistry() = default;
        std::mutex m_Mutex;
        std::vector<LogCategoryBase*> m_Categories;
    };

    template<typename... Args>
    inline constexpr void LogFormatandDispatch(LogCategoryBase& category, LogVerbosity verbosity, std::format_string<Args...> format, Args&&... args)
    {
        if (verbosity >= category.GetVerbosity())
        {
            std::string message = std::format(format, std::forward<Args>(args)...);
            category.Log(verbosity, message);
        }
    }
}

#define QE_DECLARE_LOG_CATEGORY(CategoryName, Verbosity) \
    struct LogCategory_##CategoryName : public Quest::LogCategoryBase { \
        LogCategory_##CategoryName() : Quest::LogCategoryBase(#CategoryName, Quest::LogVerbosity::Verbosity) {} \
    }; \
    extern LogCategory_##CategoryName Log_##CategoryName; \

#define QE_DECLARE_LOG_CATEGORY_EXTERN(API, CategoryName, Verbosity) \
    struct LogCategory_##CategoryName : public Quest::LogCategoryBase { \
        LogCategory_##CategoryName() : Quest::LogCategoryBase(#CategoryName, Quest::LogVerbosity::Verbosity) {} \
    }; \
    extern API LogCategory_##CategoryName Log_##CategoryName;

#define QE_DEFINE_LOG_CATEGORY(CategoryName) \
    LogCategory_##CategoryName Log_##CategoryName

#define QE_LOG(CategoryName, Verbosity, Format, ...) \
    do { \
        LogFormatandDispatch(Log_##CategoryName, Quest::LogVerbosity::Verbosity, Format, ##__VA_ARGS__); \
    } while (0)

// This could produce an error if the category doesn't exist... but it'll be ok.
#define QE_LOG_ADD_SINK_TO_CATEGORY(CategoryName, Sink) \
    Quest::LogRegistry::Get().GetCategory(#CategoryName)->AddSink(new Sink())