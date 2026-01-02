#include "SandboxCore.h"
#include "SandboxGameApplication.h"

Quest::GameApplication* CreateGameApplication()
{
    return new SandboxGameApplication();
}

void DestroyGameApplication(Quest::GameApplication* app)
{
    delete app;
}