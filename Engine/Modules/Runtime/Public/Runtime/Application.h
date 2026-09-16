#pragma once
#include "Runtime/RuntimeExport.h"

namespace Quest
{
    class Engine;

    // The game/consumer entry point. Implemented in a downstream module (e.g. Sandbox) that
    // is loaded by the Launch executable; the Engine owns the concrete instance and drives
    // its lifecycle.
    class RUNTIME_API Application
    {
    public:
        virtual ~Application() = default;

        // Called before startup
        void SetEngine(Engine* engine) { m_Engine = engine; }

        // Lifecycle functions
        virtual void Startup() = 0;
        virtual void Update() = 0;
        virtual void Shutdown() = 0;
    protected:
        Engine* m_Engine = nullptr;
    };
}
