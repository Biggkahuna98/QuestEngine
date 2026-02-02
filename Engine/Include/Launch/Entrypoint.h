#pragma once

// ONLY INCLUDE THIS ONE TIME IN ONE .CPP FILE

#include "Core/Core.h"
#include "Core/Engine.h"
#include "Engine/GameApplication.h"

extern "C" QUEST_API void InitializeEngineEntrypoint();
extern "C" QUEST_API void RunEngine();
extern "C" QUEST_API void ShutdownEngineEntrypoint();