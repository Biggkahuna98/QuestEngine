#include "Engine/Engine.h"

#include "imgui.h"
#include "Platform/PlatformUtility.h"
#include "Core/Events/EventManager.h"

namespace QE
{
	// The global engine
	Engine g_Engine{};

	Engine::Engine()
		: m_GameApplication(nullptr)
	{

	}

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
		m_TestCamera = std::make_unique<TestCamera>();
		m_GraphicsDevice->SetCamera(m_TestCamera.get());

		//m_Camera.Velocity = glm::vec3(0.f);
		//m_Camera.Position = glm::vec3(30.f, -00.f, -085.f);

		//m_Camera.Pitch = 0;
		//m_Camera.Yaw = 0;

		m_Running = true;
	}

	void Engine::Shutdown()
	{
		m_GameApplication->Shutdown();

		m_GraphicsDevice.reset();
	}

	void Engine::Run()
	{
		EventManager* g_EventManager = GetGlobalEventManager();
		constexpr bool RunGraphics = true;
		float deltaTime = 0.0f; // time between current frame and last frame
		float lastFrame = 0.0f; // time of last frame
		while (m_Running)
		{
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

			m_TestCamera->Update(deltaTime);

			// Great value headless mode, will definitely fix later on
			if (RunGraphics) m_GraphicsDevice->BeginFrame();
			ImGui::ShowDemoWindow();
			m_TestCamera->DrawDebugInfo();

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

	TestCamera *Engine::GetCamera()
	{
		return m_TestCamera.get();
	}

}

