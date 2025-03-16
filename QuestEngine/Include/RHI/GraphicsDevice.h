#pragma once
#include "Core/Window.h"



namespace QE
{
	class GraphicsDevice
	{
	public:
		GraphicsDevice(const Window& window);
		virtual ~GraphicsDevice() = default;

		virtual void BeginFrame() = 0;
		virtual void EndFrame() = 0;
		virtual void PresentFrame() = 0;

		virtual void UpdateWindowSize(uint32_t width, uint32_t height) = 0;

		virtual void WaitForDeviceIdle() = 0;
	};

	std::unique_ptr<GraphicsDevice> CreateGraphicsDeviceFactory(const Window& window);
}