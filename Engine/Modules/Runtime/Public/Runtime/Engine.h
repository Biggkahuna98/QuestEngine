#pragma once

#include "Runtime/RuntimeExport.h"
#include "Runtime/Application.h"

#include "Core/Window.h"

#include "Render/Renderer.h"

#include <memory>

namespace Quest
{
    // Top-level orchestrator. Lives above Core/RHI/Render so it can own and drive the systems
    // it coordinates (currently the Renderer; audio/physics/etc. later). Owns one Renderer and
    // sequences its frame loop inside Run().
    class RUNTIME_API Engine final
    {
    public:
        Engine();
        ~Engine();

        Application* GetApplication();
        Window* GetWindow();
        Renderer* GetRenderer();

        void SetApplication(Application* app);
        void SetWindow(Window* window);

        void Startup();
        void Run();
        void Shutdown();
    private:
        std::unique_ptr<Application> m_Application;
        std::unique_ptr<Renderer> m_Renderer;
        Window* m_Window = nullptr;

        bool running = true;
    };

    RUNTIME_API Engine* CreateEngine();
    RUNTIME_API void DestroyEngine();
    RUNTIME_API Engine* GetEngine();
}
