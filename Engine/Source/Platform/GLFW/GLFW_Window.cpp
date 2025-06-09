#include "GLFW_Window.h"

#include "Core/Log.h"

// FIX LATER WITH EVENTS
#include "Core/Events/EngineEvents.h"
#include "Engine/Engine.h"
#include "Core/Events/EventManager.h"

namespace QE
{
	static void GLFWErrorCallback(int error, const char* description)
	{
		LOG_ERROR_TAG("GLFW", "GLFW Error: ({0}): {1}", error, description);
	}

	GLFW_Window::GLFW_Window(std::string_view windowName, int width, int height)
		: Window(windowName, width, height)
	{
		if (!s_IsBackendInitialized)
		{
			if (!glfwInit())
			{
				LOG_DEBUG_TAG("GLFW_Window", "Failed to initialize GLFW");
			}
			glfwSetErrorCallback(GLFWErrorCallback);
			s_IsBackendInitialized = true;
		}

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
		m_Window = glfwCreateWindow(width, height, windowName.data(), nullptr, nullptr);

		if (!m_Window)
		{
			LOG_DEBUG_TAG("GLFW_Window", "[{}] - Failed to initialize GLFW window", windowName);
		}
		// Void pointer to this instance so I can access members in the callbacks
		glfwSetWindowUserPointer(m_Window, static_cast<void*>(this));
		s_WindowCount++;

		LOG_INFO_TAG("GLFW_Window", "Created GLFW window: [{0}] ({1}x{2})", windowName, width, height);

		if (m_IsProcessingMouseInput)
			glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

		glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int height)
			{
				g_Engine.GetGraphicsDevice().UpdateWindowSize(width, height);
				WindowResizeEvent event;
				event.Width = width;
				event.Height = height;
				GetGlobalEventManager()->QueueEvent<WindowResizeEvent>(event);
			}
		);

		glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window)
			{
				WindowCloseEvent event{};
				GetGlobalEventManager()->QueueEvent<WindowCloseEvent>(event);
				g_Engine.SetWindowShouldClose(true);
			}
		);

		glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
			{
				auto inputManager = static_cast<GLFW_Window*>(glfwGetWindowUserPointer(window))->GetInputManagerPtr();
				switch (action)
				{
				case GLFW_PRESS:
				{
					inputManager->UpdateKeyState(static_cast<KeyCode>(key), KeyState::Pressed);
					break;
				}
				case GLFW_RELEASE:
				{
					inputManager->UpdateKeyState(static_cast<KeyCode>(key), KeyState::Released);
					break;
				}
				case GLFW_REPEAT:
				{
					inputManager->UpdateKeyState(static_cast<KeyCode>(key), KeyState::Held);
					break;
				}
				}
			}
		);

		glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, int button, int action, int mods)
			{
				auto inputManager = static_cast<GLFW_Window*>(glfwGetWindowUserPointer(window))->GetInputManagerPtr();
				switch (action)
				{
				case GLFW_PRESS:
				{
					inputManager->UpdateMouseButtonState(static_cast<MouseCode>(button), KeyState::Pressed);
					break;
				}
				case GLFW_RELEASE:
				{
					inputManager->UpdateMouseButtonState(static_cast<MouseCode>(button), KeyState::Released);
					break;
				}
				}
			}
		);

		glfwSetScrollCallback(m_Window, [](GLFWwindow* window, double xOffset, double yOffset)
			{
				//LOG_DEBUG_TAG("GLFW_Window", "Scroll: {0}, {1}", xOffset, yOffset);
				if (static_cast<GLFW_Window*>(glfwGetWindowUserPointer(window))->m_IsProcessingMouseInput)
				{
					MouseScrollEvent event;
					event.MouseXOffset = xOffset;
					event.MouseYOffset = yOffset;
					//GetGlobalEventManager()->FireEvent(event);
					GetGlobalEventManager()->QueueEvent<MouseScrollEvent>(event);
				}
			}
		);

		glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double xPos, double yPos)
			{
				auto inputManager = static_cast<GLFW_Window*>(glfwGetWindowUserPointer(window))->GetInputManagerPtr();
				inputManager->UpdateMousePosition(xPos, yPos);
				if (static_cast<GLFW_Window*>(glfwGetWindowUserPointer(window))->m_IsProcessingMouseInput)
				{
					MouseMoveEvent event;
					event.MouseX = (float)xPos;
					event.MouseY = (float)yPos;
					//LOG_DEBUG("Move event from window: XPos={}, YPos={}", event.MouseX, event.MouseY);
					//GetGlobalEventManager()->FireEvent(event);
					GetGlobalEventManager()->QueueEvent<MouseMoveEvent>(event);
				}
			}
		);
	}

	GLFW_Window::~GLFW_Window()
	{
		glfwDestroyWindow(m_Window);
		s_WindowCount--;

		LOG_INFO_TAG("GLFW_Window", "Destroyed GLFW window: {0}", m_WindowName);
		LOG_DEBUG_TAG("GLFW_Window", "S_WindowCount: {0}", s_WindowCount);
			
		if (s_WindowCount == 0)
		{
			glfwTerminate();
			s_IsBackendInitialized = false;

			LOG_DEBUG_TAG("GLFW_Window", "Terminated GLFW");
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
		LOG_DEBUG_TAG("GLFW_Window", "Paused GLFW window: {0}", m_WindowName);

		int width = 0, height = 0;
		glfwGetFramebufferSize(m_Window, &width, &height);
		while (width == 0 || height == 0)
		{
			glfwGetFramebufferSize(m_Window, &width, &height);
			glfwWaitEvents();
		}

		LOG_DEBUG_TAG("GLFW_Window", "GLFW window: {0} is no longer paused", m_WindowName);
	}

	void GLFW_Window::ToggleMouseInputProcessing()
	{
		if (m_IsProcessingMouseInput)
		{
			// Mouse showing
			glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			m_IsProcessingMouseInput = false;
		} else
		{
			// No mouse showing
			glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			m_IsProcessingMouseInput = true;
		}
	}
}
