#pragma once

#include "Core/CoreExport.h"
#include "Core/Log/Log.h"

namespace Quest
{
    class CORE_API BasicSink : public LogSinkBase
    {
    public:
        BasicSink() = default;
        ~BasicSink() override = default;

        void Log(LogCategoryBase* category, LogVerbosity verbosity, std::string_view message) override;
    };
}