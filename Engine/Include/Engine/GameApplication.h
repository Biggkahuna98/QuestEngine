#pragma once

#include "Core/Core.h"

namespace QE
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
}