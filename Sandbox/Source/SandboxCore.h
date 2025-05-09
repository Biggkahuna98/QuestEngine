#pragma once
#include "SandboxAPI.h"
#include "Engine/GameApplication.h"

extern "C" SANDBOX_API QE::GameApplication* CreateGameApplication();
extern "C" SANDBOX_API void DestroyGameApplication(QE::GameApplication* app);