#pragma once
#include "SandboxAPI.h"
#include "Engine/GameApplication.h"

extern "C" SANDBOX_API Quest::GameApplication* CreateGameApplication();
extern "C" SANDBOX_API void DestroyGameApplication(Quest::GameApplication* app);