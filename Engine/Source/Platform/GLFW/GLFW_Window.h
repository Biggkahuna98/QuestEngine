#pragma once

#include "Core/Window.h"

#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"

namespace QE
{
	class GLFW_Window : public Window
	{
	public:
		GLFW_Window(std::string_view windowName, int width, int height);
		virtual ~GLFW_Window() override;

		virtual void* GetNativeWindow() override;
		virtual void ProcessEvents() override;
		virtual void PauseWindow() override;

		virtual void ToggleMouseInputProcessing() override;
	private:
		double m_LastXPos;
		double m_LastYPos;
		GLFWwindow* m_Window;
	};
}