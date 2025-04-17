#pragma once

#include <string>
#include <string_view>
#include <memory>

#include "Core/InputManager.h"

namespace QE
{
	class Window
	{
	public:
		Window(std::string_view windowName, int width, int height);
		virtual ~Window() = default;
		int GetScreenWidth() const { return m_ScreenWidth; }
		int GetScreenHeight() const { return m_ScreenHeight; }
		bool GetIsMinimized() const { return m_IsMinimized; }
		bool GetIsQuit() const { return m_IsQuit; }

		virtual void* GetNativeWindow() = 0;
		virtual void ProcessEvents() = 0;
		virtual void PauseWindow() = 0;

		InputManager& GetInputManager() { return m_InputManager; }
		InputManager* GetInputManagerPtr() { return &m_InputManager; }
	protected:
		std::string m_WindowName = "Quest Engine";
		int m_ScreenWidth = 0;
		int m_ScreenHeight = 0;

		InputManager m_InputManager;

		static bool s_IsBackendInitialized;
		static int s_WindowCount;
		bool m_IsMinimized = false;
		bool m_IsQuit = false;
	};

	std::unique_ptr<Window> CreateWindowFactory(std::string_view windowName, int width, int height);
}