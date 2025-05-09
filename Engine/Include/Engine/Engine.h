#pragma once

#include "Core/Core.h"
#include "Core/Log.h"
#include "Core/Window.h"
#include "RHI/GraphicsDevice.h"
#include "RHI/GraphicsContext.h"

namespace QE
{
	class QUEST_API Engine final
	{
	public:
		Engine() {};
		~Engine();
		Engine(const Engine&) = delete;
		void operator=(Engine const&) = delete;

		//static Engine& Get();
		//static Engine* GetPtr();

		void Initialize();
		void Shutdown();

		void Run();

		void SetWindowShouldClose(bool shouldClose);

		Window& GetWindow();
		Window* GetWindowPtr();
		InputManager& GetInput();
		InputManager* GetInputPtr();
		GraphicsDevice& GetGraphicsDevice();
		GraphicsDevice* GetGraphicsDevicePtr();
	private:
		bool m_Running = false;

		std::unique_ptr<Window> m_Window;
		InputManager* m_InputManager = nullptr; // active input manager from the active window, updated here for convenience

		std::unique_ptr<GraphicsDevice> m_GraphicsDevice;
		std::unique_ptr<GraphicsContext> m_GraphicsContext;
	};

	extern Engine g_Engine;
}