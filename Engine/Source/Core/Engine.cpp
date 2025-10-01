#include "Core/Engine.h"

#include "imgui.h"
#include "Platform/PlatformUtility.h"
#include "Core/Events/EventManager.h"
#include "Core/Profiling.h"
#include <chrono>

namespace QE
{
	// The global engine
	Engine g_Engine{};

	Engine* GetEngine()
	{
		return &g_Engine;
	}

	void Engine::Initialize()
	{
		// Default init
		m_GameApplication = nullptr;

		// Create Window
		//m_Window = CreateWindowFactory("Quest Engine", 3840, 2160);
		m_Window = CreateWindowFactory("Quest Engine", 2560, 1440);
		m_InputManager = m_Window->GetInputManagerPtr(); // This is the *ACTIVE* input manager from the active window

		// Initialize graphics device and context
		m_GraphicsDevice = CreateGraphicsDeviceFactory(m_Window.get());
		m_GraphicsContext = m_GraphicsDevice->CreateGraphicsContext();
		m_TestCamera = std::make_unique<FlyCamera>();
		m_GraphicsDevice->SetCamera(m_TestCamera.get());

		m_Renderer = std::make_unique<Renderer>();

		m_Running = true;
	}

	void Engine::Shutdown()
	{
		m_GameApplication->Shutdown();

		// Delete renderer first
		m_Renderer.reset();

		m_GraphicsContext.reset();
		m_GraphicsDevice->ShutdownAndCleanup();
	}

	void Engine::Run()
	{
		EventManager* g_EventManager = GetGlobalEventManager();
		constexpr bool RunGraphics = true;
		float deltaTime = 0.0f; // time between current frame and last frame
		float lastFrame = 0.0f; // time of last frame
		RHIStats Stats{};
		while (m_Running)
		{
			auto startTime = std::chrono::high_resolution_clock::now();
			float currentFrameTime = static_cast<float>(GetTime());
			deltaTime = currentFrameTime - lastFrame;
			lastFrame = currentFrameTime;

			// Flush (dispatch) all pending events
			g_EventManager->Flush();

			m_Window->GetInputManager().ProcessTransitions();
			m_Window->ProcessEvents();

			if (m_InputManager->IsKeyPressed(Escape))
			{
				m_Running = false;
			}
			if (m_InputManager->IsKeyPressed(P))
				m_Window->ToggleMouseInputProcessing();

			m_TestCamera->Update(deltaTime);

			// Great value headless mode, will definitely fix later on
			if (RunGraphics) m_GraphicsDevice->BeginFrame();
			// Draw stats
			{
				ImGui::Begin("Engine Stats");
					ImGui::Text("FPS: %.2f", ImGui::GetIO().Framerate);
					ImGui::Text("Frametime: %.2f ms", Stats.Frametime);
					ImGui::Text("Triangle Count: %i", Stats.TriangleCount);
					ImGui::Text("Draws: %i", Stats.DrawCallCount);
				ImGui::End();
			}

			m_TestCamera->DrawDebugInfo();

			m_GameApplication->Update();

			if (RunGraphics) m_GraphicsDevice->EndFrame();

			if (RunGraphics) m_GraphicsDevice->PresentFrame();

			// Get engine stats
			auto endTime = std::chrono::high_resolution_clock::now();
			auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
			Stats = m_GraphicsDevice->GetStats();
			Stats.Frametime = elapsed.count() / 1000.0f;

			PROFILE_MARK_FRAME();
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

	Renderer & Engine::GetRenderer()
	{
		return *m_Renderer;
	}

	Renderer * Engine::GetRendererPtr()
	{
		return m_Renderer.get();
	}

	FlyCamera *Engine::GetCamera()
	{
		return m_TestCamera.get();
	}

}

