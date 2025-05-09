#pragma once

#include "Core/Core.h"
#include "RHIContext.h"
#include <memory>

namespace QE
{
	class QUEST_API GraphicsContext : public RHIContext
	{
		public:
			GraphicsContext(GraphicsDevice* device);
			virtual ~GraphicsContext() = default;
	};

	//std::unique_ptr<GraphicsContext> CreateGraphicsContextFactory(GraphicsDevice* device);
}