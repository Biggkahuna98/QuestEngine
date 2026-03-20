#include "Core/Engine.h"

#include "imgui.h"
#include "Platform/PlatformUtility.h"
#include "Core/Events/EventManager.h"
#include "Core/Profiling.h"
#include <chrono>

#include <string_view>
#include <Core/Window.h>
#include <RHI/ShaderUtils.h>

namespace Quest
{
	// The global engine
	Engine g_Engine{};

	Engine* GetEngine()
	{
		return &g_Engine;
	}

	class EngineMessageCallback : public qrhi::MessageCallback
	{
	public:
		void Message(Severity level, std::string_view message) override
		{
			switch (level)
			{
				case Severity::Info: LOG_INFO("{}", message); break;
				case Severity::Warning: LOG_WARN("{}", message); break;
				case Severity::Error: LOG_ERROR("{}", message); break;
			}
		}
	};

	std::unique_ptr<qrhi::MessageCallback> g_RHIMessageCallback = std::make_unique<EngineMessageCallback>();

	void Engine::Initialize()
	{
		// Default init
		m_GameApplication = nullptr;

		// Create Window
		//m_Window = CreateWindowFactory("Quest Engine", 3840, 2160);
		//m_Window = CreateWindowFactory("Quest Engine", 2560, 1440);
		m_Window = CreateWindowFactory("Quest Engine", 1920, 1080);
		m_InputManager = m_Window->GetInputManagerPtr(); // This is the *ACTIVE* input manager from the active window

		// Initialize the context and device
		qrhi::CompileShader("static_triangle", "Shaders/static_triangle.slang");
		qrhi::CompileShader("vertex_buffer", "Shaders/vertex_buffer.slang");


		qrhi::ContextDesc contextDesc{};
		contextDesc.framesInFlight = qrhi::FramesInFlight::Two;
		contextDesc.messageCallback = g_RHIMessageCallback.get();
		contextDesc.window = m_Window.get();
		m_GraphicsContext = qrhi::CreateContext(contextDesc);

		m_GraphicsDevice = m_GraphicsContext->CreateDevice();
		qrhi::CommandListDesc commandListDesc{};
		commandListDesc.type = qrhi::QueueType::Graphics;
		m_GraphicsCommandList = m_GraphicsDevice->CreateCommandList(commandListDesc);

		qrhi::GraphicsPipelineDesc pipelineDesc{};
		qrhi::ShaderDesc shaderDesc{};
		shaderDesc.type = qrhi::ShaderType::Vertex;
		shaderDesc.name = "vertex_buffer.spv";
		qrhi::ShaderHandle vertexShader = m_GraphicsDevice->CreateShader(shaderDesc);
		pipelineDesc.vertexShader = vertexShader;
		m_GraphicsPipeline = m_GraphicsDevice->CreateGraphicsPipeline(pipelineDesc);

		const std::vector<qrhi::Vertex> vertices = {
			{{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
			{{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
			{{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}
		};

		qrhi::BufferDesc bufferDesc{};
		bufferDesc.type = qrhi::BufferType::Vertex;
		bufferDesc.sizeInBytes = sizeof(vertices[0]) * vertices.size();
		m_VertexBuffer = m_GraphicsDevice->CreateBuffer(bufferDesc);

		void* ptr = m_GraphicsDevice->MapBuffer(m_VertexBuffer.Get());
		memcpy(ptr, vertices.data(), sizeof(vertices[0]) * vertices.size());
		m_GraphicsDevice->UnmapBuffer(m_VertexBuffer.Get());

		m_TestCamera = std::make_unique<FlyCamera>();
		//m_GraphicsDevice->SetCamera(m_TestCamera.get());

		m_Renderer = std::make_unique<Renderer>();

		m_Running = true;
	}

	void Engine::Shutdown()
	{
		LOG_INFO("Shutting down engine...");
		m_GameApplication->Shutdown();

		// Delete renderer first
		m_Renderer.reset();

		// Probably not necessary for the device but it's ok
		m_GraphicsPipeline.Reset();
		m_GraphicsCommandList.Reset();
		m_GraphicsDevice.Reset();
		m_GraphicsContext->Shutdown();
		m_GraphicsContext.Reset();
	}

	void Engine::Run()
	{
		EventManager* g_EventManager = GetGlobalEventManager();
		constexpr bool RunGraphics = true;
		float deltaTime = 0.0f; // time between current frame and last frame
		float lastFrame = 0.0f; // time of last frame
		//RHIStats Stats{};
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

			m_GraphicsContext->BeginFrame();
			m_GraphicsCommandList->Open();
			qrhi::GraphicsState state{};
			state.vertexBuffer = m_VertexBuffer;
			state.pipeline = m_GraphicsPipeline.Get();
			m_GraphicsCommandList->SetGraphicsState(state);

			// Draw stats
			{
				/*ImGui::Begin("Engine Stats");
					ImGui::Text("FPS: %.2f", ImGui::GetIO().Framerate);
					ImGui::Text("Frametime: %.2f ms", Stats.Frametime);
					ImGui::Text("Triangle Count: %i", Stats.TriangleCount);
					ImGui::Text("Draws: %i", Stats.DrawCallCount);
				ImGui::End();*/
			}

			//m_TestCamera->DrawDebugInfo();

			m_GameApplication->Update();

			qrhi::DrawArguments drawArguments{};
			drawArguments.vertexCount = 3;
			drawArguments.instanceCount = 1;
			m_GraphicsCommandList->Draw(drawArguments);


			// Get engine stats
			auto endTime = std::chrono::high_resolution_clock::now();
			auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
			//Stats = m_GraphicsDevice->GetStats();
			//Stats.Frametime = elapsed.count() / 1000.0f;

			m_GraphicsCommandList->Close();
			m_GraphicsContext->EndFrame();
			m_GraphicsContext->PresentFrame();
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

	InputManager& Engine::GetInput()
	{
		return m_Window->GetInputManager();
	}

	GameApplication* Engine::GetGameApplication()
	{
		return m_GameApplication;
	}

	Renderer & Engine::GetRenderer()
	{
		return *m_Renderer;
	}

	FlyCamera *Engine::GetCamera()
	{
		return m_TestCamera.get();
	}

}

