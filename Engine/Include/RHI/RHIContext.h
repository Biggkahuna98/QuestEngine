#pragma once

#include "Core/Core.h"

namespace QE
{
	class GraphicsDevice;

	class QUEST_API RHIContext
	{
	public:
		RHIContext(GraphicsDevice* device);
		virtual ~RHIContext() = default;
	protected:
		// Non-owning pointer that references the device that created this context
		GraphicsDevice* m_Device;
	};
}