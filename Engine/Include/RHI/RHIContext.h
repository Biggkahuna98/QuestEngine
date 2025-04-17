#pragma once

//#include "GraphicsDevice.h"

namespace QE
{
	class GraphicsDevice;


	class RHIContext
	{
	public:
		RHIContext(GraphicsDevice* device);
		virtual ~RHIContext() = default;
	protected:
		GraphicsDevice* m_Device;
	};
}