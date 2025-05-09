#include "SandboxCore.h"
#include "SandboxGameApplication.h"

QE::GameApplication* CreateGameApplication()
{
    return new SandboxGameApplication();
}

void DestroyGameApplication(QE::GameApplication* app)
{
    delete app;
}