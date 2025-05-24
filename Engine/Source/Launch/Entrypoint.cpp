#include "Launch/Entrypoint.h"

#include <csignal>

void SignalHandler(int signal)
{
    // Flush all the loggers on crash
    QE::Log::FlushAllLoggers();

    exit(signal);
}

void TerminateHandler()
{
    // Same thing as Signal handler, flush loggers
    QE::Log::FlushAllLoggers();

    exit(1);
}

void InitializeEngineEntrypoint()
{
    using namespace QE;

    // Signal handlers
    std::signal(SIGSEGV, SignalHandler);
    std::signal(SIGABRT, SignalHandler);
    std::signal(SIGTERM, SignalHandler);
    std::signal(SIGINT, SignalHandler);

    // Terminate handler
    std::set_terminate(TerminateHandler);

    Log::Init();
    LOG_INFO("Hello from the Entrypoint for Quest Engine!");

    g_Engine.Initialize();
}

void RunEngine()
{
    using namespace QE;

    g_Engine.Run();
}

void ShutdownEngineEntrypoint()
{
    using namespace QE;

    g_Engine.Shutdown();
    Log::FlushAllLoggers();
}