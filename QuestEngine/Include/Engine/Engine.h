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
	private:
		Engine() {};

		bool m_Running = false;

		std::unique_ptr<Window> m_Window;
	};
}