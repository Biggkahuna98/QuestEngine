#pragma once

#include "Core/Core.h"

namespace Quest
{
    class QUEST_API GameApplication
    {
    public:
        GameApplication() = default;
        virtual ~GameApplication() = default;

        virtual void Init() = 0;
        virtual void Shutdown() = 0;
        virtual void Update() = 0;
    };

    // Define this in the client
    GameApplication* CreateApplication();
}