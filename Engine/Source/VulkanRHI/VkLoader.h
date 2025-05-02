#pragma once

#include "VkTypes.h"
#include <unordered_map>
#include <string>
#include <vector>
#include <filesystem>
#include <optional>
#include <memory>

namespace QE
{
	struct GeoSurface // wtf is this
	{
		uint32_t StartIndex;
		uint32_t Count;
	};

	struct MeshAsset
	{
		std::string Name;

		//std::vector<GeoSurface> Surfaces;
		GPUMeshBuffer MeshBuffer;
	};

	class VkGraphicsDevice; // why need ??
	std::optional<std::vector<std::shared_ptr<MeshAsset>>> LoadGltfMeshesAssimp(VkGraphicsDevice* engine, std::filesystem::path filePath);
}