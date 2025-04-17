#pragma once

#include "RHI/GraphicsContext.h"

namespace QE
{
	class VkRHIContext : public GraphicsContext
	{
	public:
		VkRHIContext(GraphicsDevice* device) : GraphicsContext(device) {};
	private:
	};
}