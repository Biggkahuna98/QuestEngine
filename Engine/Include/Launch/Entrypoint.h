#pragma once

// ONLY INCLUDE THIS ONE TIME IN ONE .CPP FILE

#include "Core/Core.h"
#include "Core/Log.h"
#include "Engine/Engine.h"

extern "C" QUEST_API void InitializeEngineEntrypoint();
extern "C" QUEST_API void RunEngine();
extern "C" QUEST_API void ShutdownEngineEntrypoint();