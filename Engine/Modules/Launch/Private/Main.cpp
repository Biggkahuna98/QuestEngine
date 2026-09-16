#include <iostream>
#include <ostream>

#include "Core/Module/ModuleBoilerplate.h"
#include "Core/Platform/DynamicLibrary.h"
#include "Core/CoreModule.h"
#include "Runtime/Engine.h"
#include "Core/Log/Log.h"

using namespace Quest;

OVERRIDE_NEW_DELETE

using CreateApplicationFn = Quest::Application* (*)();
using DestroyApplicationFn = void (*)(Quest::Application*);
using VoidFn = void (*)();

int main(int argc, char** argv)
{
    // Startup Core module
    ModuleStartup();

    // Create and setup the engine
    Engine* engine = CreateEngine();
    engine->Startup();

    // Load Sandbox dynamically
    DynamicLibrary gameLib;
    if (!gameLib.Load("Sandbox.dll"))
    {
        std::cout << "Failed to load Sandbox.dll" << std::endl;
        exit(69420);
    }

    auto CreateApplication = gameLib.GetFunction<CreateApplicationFn>("CreateApplication");
    auto DestroyApplication = gameLib.GetFunction<DestroyApplicationFn>("DestroyApplication");

    if (!CreateApplication || !DestroyApplication)
    {
        std::cout << "Failed to find CreateApplication or DestroyApplication in Sandbox.dll" << std::endl;
        exit(69420);
    }

    Application* app = CreateApplication();
    engine->SetApplication(app);

    engine->Run();

    engine->Shutdown();
    DestroyEngine();

    gameLib.Unload();

    ModuleShutdown();

    // Shutdown engine logging service
    LogRegistry::Get().Shutdown();

    // Print out memory stats for debugging
    BasicMemoryStats stats = Memory::GetStats();
    std::cout << "Memory Stats: Allocs - " << stats.Allocs << ", Frees - " << stats.Frees << std::endl;

    return 0;
}