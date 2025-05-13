#include <Launch/Entrypoint.h>
#include "Core/Log.h"

#include "Engine/Engine.h"

#include <Windows.h>

using GetGameAppFn = QE::GameApplication* (*)();
using DeleteGameAppFn = void (*)(QE::GameApplication*);

int main(int argc, char** argv)
{
    // Initialize the engine
    InitializeEngineEntrypoint();

    // Load the Sandbox program dynamically
    HMODULE gameLibDLL = LoadLibrary("Sandbox.dll");
    if (gameLibDLL == nullptr)
    {
        LOG_ERROR("Failed to load Sandbox.dll");
        exit(1);
    }

    GetGameAppFn GetGameApp = reinterpret_cast<GetGameAppFn>(GetProcAddress(gameLibDLL, "CreateGameApplication"));
    DeleteGameAppFn DeleteGameApp = reinterpret_cast<DeleteGameAppFn>(GetProcAddress(gameLibDLL, "DestroyGameApplication"));

    if (!GetGameApp || !DeleteGameApp)
    {
        LOG_ERROR("Failed to load CreateGameApplication or DestroyGameApplication");
        exit(1);
    }

    QE::GetEngine()->SetGameApplication(GetGameApp());

    // Run the engine
    RunEngine();

    // Shutdown engine
    QE::GameApplication* app = QE::GetEngine()->GetGameApplication();
    ShutdownEngineEntrypoint();
    DeleteGameApp(app);

    // Free DLL
    FreeLibrary(gameLibDLL);

    return 0;
}