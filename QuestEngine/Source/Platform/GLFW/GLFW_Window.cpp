#include "GLFW_Window.h"

#include "Core/Log.h"

namespace QE
{
	static void GLFWErrorCallback(int error, const char* description)
	{
		LOG_TAG("Engine", Error, "GLFW", "GLFW Error: ({0}): {1}", error, description);
	}

	GLFW_Window::GLFW_Window(std::string_view windowName, int width, int height)
		: Window(windowName, width, height)
	{
		if (!s_IsBackendInitialized)
		{
			if (!glfwInit())
			{
				LOG_TAG("Engine", Debug, "GLFW_Window", "Failed to initialize GLFW");
			}
			glfwSetErrorCallback(GLFWErrorCallback);
			s_IsBackendInitialized = true;
		}

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		m_Window = glfwCreateWindow(width, height, windowName.data(), nullptr, nullptr);
		if (!m_Window)
		{
			LOG_TAG("Engine", Debug, "GLFW_Window", "Failed to initialize GLFW window");
		}
		s_WindowCount++;

		LOG_TAG("Engine", Info, "GLFW_Window", "Created GLFW window: {0} ({1}x{2})", windowName, width, height);
	}

	GLFW_Window::~GLFW_Window()
	{
		glfwDestroyWindow(m_Window);
		s_WindowCount--;

		LOG_TAG("Engine", Info, "GLFW_Window", "Destroyed GLFW window: {0}", m_WindowName);
		LOG_TAG("Engine", Debug, "GLFW_Window", "S_WindowCount: {0}", s_WindowCount);
			
		if (s_WindowCount == 0)
		{
			glfwTerminate();
			s_IsBackendInitialized = false;

			LOG_TAG("Engine", Debug, "GLFW_Window", "Terminated GLFW");
		}
	}

	void* GLFW_Window::GetNativeWindow()
	{
		return m_Window;
	}

	void GLFW_Window::ProcessEvents()
	{
		glfwPollEvents();
	}

	void GLFW_Window::PauseWindow()
	{
		LOG_TAG("Engine", Debug, "GLFW_Window", "Paused GLFW window: {0}", m_WindowName);

		int width = 0, height = 0;
		glfwGetFramebufferSize(m_Window, &width, &height);
		while (width == 0 || height == 0)
		{
			glfwGetFramebufferSize(m_Window, &width, &height);
			glfwWaitEvents();
		}

		LOG_TAG("Engine", Debug, "GLFW_Window", "GLFW window: {0} is no longer paused", m_WindowName);
	}
}