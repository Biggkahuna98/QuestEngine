#include "VkLoader.h"
#include <stb_image.h>
#include <iostream>

#include "VkInit.h"
#include "VkTypes.h"
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/quaternion.hpp"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Core/Log.h"
#include "VkGraphicsDevice.h" // gross

namespace QE
{
	std::optional<std::vector<std::shared_ptr<MeshAsset>>> LoadGltfMeshesAssimp(VkGraphicsDevice* engine, std::filesystem::path filePath)
	{
		LOG_DEBUG("Loading GLTF: {}", filePath.string());

		std::string _fp = QE_RESOURCES_FOLDER;
		_fp += filePath.string();
		LOG_ERROR("File path appended: {}", _fp);

		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(_fp,
			aiProcess_CalcTangentSpace |
			aiProcess_Triangulate |
			aiProcess_JoinIdenticalVertices |
			aiProcess_SortByPType
		);

		if (scene == nullptr)
		{
			LOG_ERROR("Failed to load GLTF:\n\t File: {}\n\t {}", filePath.string(), importer.GetErrorString());
			return std::nullopt;
		}

		std::vector<std::shared_ptr<MeshAsset>> meshes;

		if (!scene->HasMeshes())
			LOG_ERROR("No meshes found");

		// Log debug info
		LOG_DEBUG("Mesh count: {}", scene->mNumMeshes);

		std::vector<uint32_t> indices;
		std::vector<Vertex> vertices;

		meshes.reserve(scene->mNumMeshes);

		// For each mesh
		for (int i = 0; i < scene->mNumMeshes; i++)
		{
			LOG_DEBUG("Mesh {}: {}", i, scene->mMeshes[i]->mName.C_Str());
			LOG_DEBUG("\tVertex count: {}", scene->mMeshes[i]->mNumVertices);
			LOG_DEBUG("\tFace count: {}", scene->mMeshes[i]->mNumFaces);
			//LOG_DEBUG("\tIndice count: {}", scene->mMeshes[i]->mFaces->mNumIndices);

			indices.clear();
			vertices.clear();

			MeshAsset newMesh;

			aiMesh* mesh = scene->mMeshes[i];

			newMesh.Name = mesh->mName.C_Str();

			// Get indices
			//indices.reserve(mesh->mFaces->mNumIndices);
			int numFaces = mesh->mNumFaces;
			for (int index = 0; index < numFaces; index++)
			{
				aiFace face = mesh->mFaces[index];

				int indecesCount = face.mNumIndices;
				for (int idx = 0; idx < indecesCount; idx++)
				{
					indices.push_back(face.mIndices[idx]);
				}
			}

			// Get the vertices
			vertices.reserve(mesh->mNumVertices);
			for (int index = 0; index < mesh->mNumVertices; index++)
			{
				Vertex newVtx;
				newVtx.Position = { mesh->mVertices[index].x, mesh->mVertices[index].y, mesh->mVertices[index].z };
				newVtx.Normal = { mesh->mNormals[index].x, mesh->mNormals[index].y, mesh->mNormals[index].z };
				newVtx.Color = glm::vec4{ 1.0f };

				// Tex coords (uvs)
				aiVector3D uvs = mesh->mTextureCoords[0][index];
				newVtx.uv_x = uvs.x;
				newVtx.uv_y = uvs.y;

				vertices.push_back(newVtx);
			}

			LOG_DEBUG("Loaded Vertices Size: {}", vertices.size());
			LOG_DEBUG("Loaded Indices Size: {}", indices.size());

			// Create the mesh asset
			GPUMeshBuffer meshBuffer = engine->UploadMesh(indices, vertices);
			newMesh.MeshBuffer = meshBuffer;

			meshes.push_back(std::make_shared<MeshAsset>(std::move(newMesh)));
		}

		return meshes;
	}
}