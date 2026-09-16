#include "Core/Log/BasicSink.h"

#include <iostream>

namespace Quest
{
    void BasicSink::Log(LogCategoryBase* category, LogVerbosity verbosity, std::string_view message)
    {
        std::cout << "[" << category->GetName() << "] " << GetLogVerbosityString(verbosity) << ": " << message << std::endl;
    }
}
