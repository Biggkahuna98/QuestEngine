#include "Core/Window.h"

#include "Platform/GLFW/GLFW_Window.h"

namespace QE
{
	bool Window::s_IsBackendInitialized = false;
	int Window::s_WindowCount = 0;

	Window::Window(std::string_view windowName, int width, int height)
		: m_WindowName(windowName.data()), m_ScreenWidth(width), m_ScreenHeight(height)
	{
	}

	std::unique_ptr<Window> CreateWindowFactory(std::string_view windowName, int width, int height)
	{
		return std::make_unique<GLFW_Window>(windowName, width, height);
	}
}