#pragma once

#include "RHIContext.h"
#include <memory>

namespace QE
{
	class GraphicsContext : public RHIContext
	{
		public:
			GraphicsContext(GraphicsDevice* device);
			virtual ~GraphicsContext() = default;
	};

	//std::unique_ptr<GraphicsContext> CreateGraphicsContextFactory(GraphicsDevice* device);
}