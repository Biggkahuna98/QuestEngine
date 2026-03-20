#pragma once

#include "Core/Core.h"
#include "Core/Log.h"
#include "Core/Window.h"
#include "RHI/Context.h"
#include "RHI/QRHI.h"
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

		qrhi::ContextHandle GetGraphicsContext() { return m_GraphicsContext; }
		qrhi::DeviceHandle GetGraphicsDevice() { return m_GraphicsDevice; }
	private:
		bool m_Running = false;

		std::unique_ptr<Window> m_Window;
		InputManager* m_InputManager = nullptr; // active input manager from the active window, updated here for convenience

		qrhi::ContextHandle m_GraphicsContext;
		qrhi::DeviceHandle m_GraphicsDevice;

		std::unique_ptr<Renderer> m_Renderer;

		GameApplication* m_GameApplication;

		std::unique_ptr<FlyCamera> m_TestCamera;
	};

	extern Engine g_Engine;

	extern "C" QUEST_API Engine* GetEngine();
}