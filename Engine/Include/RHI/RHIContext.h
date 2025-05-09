#pragma once

//#include "GraphicsDevice.h"
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
		GraphicsDevice* m_Device;
	};
}