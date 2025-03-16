#pragma once

#include "RHI/GraphicsContext.h"

namespace QE
{
	class VkGraphicsContext : public GraphicsContext
	{
	public:
		VkGraphicsContext(GraphicsDevice* device);
		virtual ~VkGraphicsContext() override;
	};
}