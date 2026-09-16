#include "Runtime/Engine.h"

//#include "Render/Renderer.h"
#include "Core/Core.h"
#include "Core/Window.h"

namespace Quest
{
    // Global engine
    static Engine* g_Engine = nullptr;

    Engine* CreateEngine()
    {
        if (g_Engine == nullptr)
            g_Engine = new Engine();
        return g_Engine;
    }

    void DestroyEngine()
    {
        delete g_Engine;
        g_Engine = nullptr;
    }

    Engine* GetEngine()
    {
        return g_Engine;
    }

    Engine::Engine()
        : m_Renderer(std::make_unique<Renderer>())
    {
    }

    Engine::~Engine() = default;

    Application* Engine::GetApplication()
    {
        return m_Application.get();
    }

    Window* Engine::GetWindow()
    {
        return m_Window;
    }

    Renderer* Engine::GetRenderer()
    {
        return m_Renderer.get();
    }

    void Engine::SetApplication(Application* app)
    {
        m_Application.reset(app);
        m_Application->SetEngine(this);
        m_Application->Startup();
    }

    void Engine::SetWindow(Window* window)
    {
        m_Window = window;
    }

    void Engine::Startup()
    {
        QE_LOG(Engine, Info, "Starting Engine");
    }

    void Engine::Run()
    {
        // The window is created by the Application during SetApplication(), so it exists by now.
        m_Renderer->Initialize(m_Window);

        while (running)
        {
            running = m_Window->ProcessEvents();

            m_Renderer->BeginFrame();
            m_Application->Update();
            m_Renderer->EndFrame();
            m_Renderer->Present();
        }
    }

    void Engine::Shutdown()
    {
        QE_LOG(Engine, Info, "Shutting Down Engine");
        m_Renderer->Shutdown();
        m_Application->Shutdown();
        m_Application.reset();
    }
}
