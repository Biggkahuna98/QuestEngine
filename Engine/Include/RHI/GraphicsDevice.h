#pragma once
#include "Core/Core.h"
#include "Core/Window.h"
//#include "GraphicsContext.h"
#include "RHI/ResourceTypes.h"
#include <memory>
#include <span>

namespace QE
{
	class GraphicsContext;
	class TestCamera;

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
		virtual TextureHandle CreateTexture(TextureDescription desc) = 0;
		virtual MeshHandle CreateMesh(std::span<Vertex> vertices, std::span<uint32_t> indices) = 0;

		// Temporary probably
		virtual void DrawMesh(MeshHandle mesh, TextureHandle* texture = nullptr) = 0;
		virtual void WaitForDeviceIdle() = 0;
		virtual void SetCamera(TestCamera* camera) = 0;

		friend class GraphicsContext;
	};

	QUEST_API std::unique_ptr<GraphicsDevice> CreateGraphicsDeviceFactory(Window* window);
}