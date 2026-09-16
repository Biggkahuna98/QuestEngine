#include "Core/Core.h"
#include "Core/CoreModule.h"
#include "Core/Module/ModuleBoilerplate.h"

#include "Core/Log/BasicSink.h"

OVERRIDE_NEW_DELETE

QE_DEFINE_LOG_CATEGORY(Core);
QE_DEFINE_LOG_CATEGORY(Engine);

namespace Quest
{
    void ModuleStartup()
    {
        QE_LOG_ADD_SINK_TO_CATEGORY(Core, Quest::BasicSink);
        QE_LOG_ADD_SINK_TO_CATEGORY(Engine, Quest::BasicSink);

        // After the sinks, or it has nowhere to go.
        QE_LOG(Engine, Info, "Starting up the Core/Engine Module");
    }

    void ModuleShutdown()
    {
        QE_LOG(Engine, Info, "Shutting down the Core/Engine Module");
    }
}
