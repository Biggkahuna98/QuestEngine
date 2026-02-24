#pragma once

#include "Core/Core.h"
#include "Core/Log.h"
#include "Core/Window.h"
//#include "RHI/DeviceManager.h"
#include "Graphics/Renderer.h"
#include "../Engine/GameApplication.h"
#include "Graphics/FlyCamera.h"

namespace Quest
{
	class QUEST_API Engine final
	{
	public:
		Engine() = default;
		~Engine() = default;
		Engine(const Engine&) = delete;
		void operator=(Engine const&) = delete;

		//static Engine& Get();
		//static Engine* GetPtr();

		void Initialize();
		void Shutdown();

		void Run();

		void SetWindowShouldClose(bool shouldClose);

		void SetGameApplication(GameApplication* gameApplication);

		Window& GetWindow();
		InputManager& GetInput();
		GameApplication* GetGameApplication();
		Renderer& GetRenderer();
		FlyCamera* GetCamera();
	private:
		bool m_Running = false;

		std::unique_ptr<Window> m_Window;
		InputManager* m_InputManager = nullptr; // active input manager from the active window, updated here for convenience

		//std::unique_ptr<GraphicsDevice> m_GraphicsDevice;
		/*struct GraphicsContext
		{
			std::unique_ptr<qrhi::DeviceManager> deviceManager;
			qrhi::DeviceHandle device;
		};
		GraphicsContext m_GraphicsContext;*/

		std::unique_ptr<Renderer> m_Renderer;

		GameApplication* m_GameApplication;

		std::unique_ptr<FlyCamera> m_TestCamera;
	};

	extern Engine g_Engine;

	extern "C" QUEST_API Engine* GetEngine();
}