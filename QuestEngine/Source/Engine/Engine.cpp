#include "Engine/Engine.h"

#include "Platform/PlatformUtility.h"

namespace QE
{
	Engine& Engine::Get()
	{
		static Engine instance;
		return instance;
	}

	Engine* Engine::GetPtr()
	{
		static Engine instance;
		return &instance;
	}

	void Engine::Initialize()
	{
		// Create Window
		m_Window = CreateWindowFactory("Quest Engine", 1280, 720);

		m_Running = true;
	}

	void Engine::Shutdown()
	{

	}

	void Engine::Run()
	{
		while (m_Running)
		{
			m_Window->GetInputManager().ProcessTransitions();
			m_Window->ProcessEvents();

			if (m_Window->GetInputManager().IsKeyPressed(Key::Escape))
			{
				m_Running = false;
			}
		}
	}
}

