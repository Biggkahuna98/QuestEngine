#pragma once
#include "Core/Core.h"
#include "Graphics/RenderTypes.h"

#include <optional>

namespace QE
{
    QUEST_API std::optional<Model> LoadModel(const std::string& path, bool rotate90 = false, bool flipVertical = false, bool flipUVs = false);
    QUEST_API std::optional<TextureHandle> LoadTexture(const std::string& path);
}
