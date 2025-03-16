#include "Engine/Engine.h"

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
			m_Window->ProcessEvents();
		}
	}
}

