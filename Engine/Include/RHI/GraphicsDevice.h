#pragma once
#include "Core/Core.h"
#include "Core/Window.h"
//#include "GraphicsContext.h"
#include "RHI/ResourceTypes.h"
#include "Renderer/RenderTypes.h"
#include <memory>

namespace QE
{
	class GraphicsContext;

	class QUEST_API GraphicsDevice
	{
	public:
		GraphicsDevice(Window* window);
		virtual ~GraphicsDevice() = default;

		virtual void BeginFrame() = 0;
		virtual void EndFrame() = 0;
		virtual void PresentFrame() = 0;

		virtual void UpdateWindowSize(uint32_t width, uint32_t height) = 0;
		virtual std::unique_ptr<GraphicsContext> CreateGraphicsContext() = 0;

		virtual BufferHandle CreateBuffer(BufferDescription desc) = 0;

		// Temporary probably
		virtual void DrawVertexBuffer(BufferHandle vtx) = 0;
		virtual void DrawMesh(Mesh mesh) = 0;
		virtual void WaitForDeviceIdle() = 0;

		friend class GraphicsContext;
	};

	QUEST_API std::unique_ptr<GraphicsDevice> CreateGraphicsDeviceFactory(Window* window);
}