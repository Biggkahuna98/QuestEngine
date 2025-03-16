#pragma once
#include "Core/Core.h"
#include "Core/Log.h"

#include "Core/Window.h"

namespace QE
{
	class Engine final
	{
	public:
		~Engine() = default;
		Engine(const Engine&) = delete;
		void operator=(Engine const&) = delete;

		static Engine& Get();
		static Engine* GetPtr();

		void Initialize();
		void Shutdown();

		void Run();

		Window& GetWindow();
		Window* GetWindowPtr();
		InputManager& GetInput();
		InputManager* GetInputPtr();
	private:
		Engine() = default;

		bool m_Running = false;

		std::unique_ptr<Window> m_Window;
		InputManager* m_InputManager; // active input manager from the active window, updated here for convenience
	};
}