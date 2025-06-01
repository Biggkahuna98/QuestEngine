#include "Engine/Engine.h"

#include "imgui.h"
#include "Platform/PlatformUtility.h"

namespace QE
{
	// The global engine
	Engine g_Engine;

	Engine* GetEngine()
	{
		return &g_Engine;
	}

	Engine::~Engine()
	{
	}

	void Engine::Initialize()
	{
		// Create Window
		m_Window = CreateWindowFactory("Quest Engine", 1920, 1080);
		m_InputManager = m_Window->GetInputManagerPtr(); // This is the *ACTIVE* input manager from the active window

		// Initialize graphics device and context
		m_GraphicsDevice = CreateGraphicsDeviceFactory(m_Window.get());
		m_GraphicsContext = m_GraphicsDevice->CreateGraphicsContext();

		m_Running = true;
	}

	void Engine::Shutdown()
	{
		m_GameApplication->Shutdown();

		m_GraphicsDevice.reset();
	}

	void Engine::Run()
	{
		constexpr bool RunGraphics = true;
		while (m_Running)
		{
			m_Window->GetInputManager().ProcessTransitions();
			m_Window->ProcessEvents();

			if (m_InputManager->IsKeyPressed(Escape))
			{
				m_Running = false;
			}

			// Great value headless mode, will definitely fix later on
			if (RunGraphics) m_GraphicsDevice->BeginFrame();

			m_GameApplication->Update();

			if (RunGraphics) m_GraphicsDevice->EndFrame();

			if (RunGraphics) m_GraphicsDevice->PresentFrame();
		}
	}

	void Engine::SetWindowShouldClose(bool shouldClose)
	{
		m_Running = !shouldClose;
	}

	void Engine::SetGameApplication(GameApplication *gameApplication)
	{
		m_GameApplication = gameApplication;

		// Init game app
		m_GameApplication->Init();
	}

	Window& Engine::GetWindow()
	{
		return *m_Window;
	}

	Window* Engine::GetWindowPtr()
	{
		return m_Window.get();
	}

	InputManager& Engine::GetInput()
	{
		return m_Window->GetInputManager();
	}

	InputManager* Engine::GetInputPtr()
	{
		return &m_Window->GetInputManager();
	}

	GraphicsDevice& Engine::GetGraphicsDevice()
	{
		return *m_GraphicsDevice;
	}

	GraphicsDevice* Engine::GetGraphicsDevicePtr()
	{
		return m_GraphicsDevice.get();
	}

	GameApplication* Engine::GetGameApplication()
	{
		return m_GameApplication;
	}
}

