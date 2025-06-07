#include "Assets/AssetLoader.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <stb_image.h>

// Need access to the graphics device
#include "Engine/Engine.h"
#include "gtx/quaternion.hpp"

namespace QE
{
	void ProcessNode(aiNode* node, const aiScene* scene, Model* model, bool rotate90);
	MeshHandle ProcessMesh(aiMesh* mesh, const aiScene* scene, bool rotate90);

    std::optional<Model> LoadModel(const std::string &path, bool rotate90, bool flipVerticals)
    {
        LOG_DEBUG("Loading Model: {}", path);

		std::string _fp = QE_RESOURCES_FOLDER;
		_fp += path;
		LOG_DEBUG("File path appended: {}", _fp);

		Assimp::Importer importer;
    	unsigned int pFlags = aiProcess_CalcTangentSpace |
			aiProcess_Triangulate |
			aiProcess_JoinIdenticalVertices |
			aiProcess_SortByPType |
			aiProcess_GenSmoothNormals;

    	if (flipVerticals)
    		pFlags |= aiProcess_FlipUVs;
		const aiScene* scene = importer.ReadFile(_fp, pFlags);

		if (scene == nullptr)
		{
			LOG_ERROR("Failed to load Model:\n\t File: {}\n\t {}", path, importer.GetErrorString());
			return std::nullopt;
		}

		Model model;

    	ProcessNode(scene->mRootNode, scene, &model, rotate90);

		return model;
    }

	void ProcessNode(aiNode* node, const aiScene* scene, Model* model, bool rotate90)
    {
    	// Process each mesh located at the current node
		for (unsigned int i = 0; i < node->mNumMeshes; i++)
		{
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];

			LOG_DEBUG("Mesh {}: {}", i, mesh->mName.C_Str());
			LOG_DEBUG("\tVertex count: {}", mesh->mNumVertices);
			LOG_DEBUG("\tFace count: {}", mesh->mNumFaces);
			LOG_DEBUG("\tIndice count: {}", mesh->mFaces->mNumIndices * mesh->mNumFaces);

			model->Meshes.push_back(ProcessMesh(mesh, scene, rotate90));
		}
    	// After mesh processing, recursively process children nodes
    	for (unsigned int i = 0; i < node->mNumChildren; i++)
    	{
    		ProcessNode(node->mChildren[i], scene, model, rotate90);
    	}
    }

	MeshHandle ProcessMesh(aiMesh* mesh, const aiScene* scene, bool rotate90)
    {
    	std::vector<uint32_t> indices;
    	std::vector<Vertex> vertices;
    	// Textures here once it exists

    	aiMatrix4x4 rotationMatrix;
    	aiMatrix4x4::RotationX(ai_real(AI_MATH_PI / 2.0f), rotationMatrix);
    	// Go through each mesh's vertices
    	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    	{
    		Vertex newVtx;
    		aiVector3D pos = mesh->mVertices[i];
    		if (rotate90)
    			pos = rotationMatrix * pos;
    		newVtx.Position.x = pos.x;
    		newVtx.Position.y = pos.y;
    		newVtx.Position.z = pos.z;

    		// Process normals
    		if (mesh->HasNormals())
    		{
				newVtx.Normal.x = mesh->mNormals[i].x;
    			newVtx.Normal.y = mesh->mNormals[i].y;
    			newVtx.Normal.z = mesh->mNormals[i].z;
    		}

    		// Texture coords
    		if (mesh->mTextureCoords[0])
    		{
				newVtx.uv_x = mesh->mTextureCoords[0][i].x;
    			newVtx.uv_y = mesh->mTextureCoords[0][i].y;
    		}

    		// Placeholder
    		newVtx.Color = glm::vec4{ 1.0f };

    		vertices.push_back(newVtx);
    	}

    	// Go through each face and get the indices
    	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    	{
    		aiFace face = mesh->mFaces[i];
    		for (unsigned int j = 0; j < face.mNumIndices; j++)
    			indices.push_back(face.mIndices[j]);
    	}

    	// Material processing


    	// Move the mesh uploading stuff elsewhere later
		MeshHandle newMesh = g_Engine.GetGraphicsDevice().CreateMesh(vertices, indices);

    	return newMesh;
    }

	std::optional<TextureHandle> LoadTexture(const std::string &path)
	{
    	LOG_DEBUG("Loading Texture: {}", path);

    	std::string _fp = QE_RESOURCES_FOLDER;
    	_fp += path;
    	LOG_DEBUG("File path appended: {}", _fp);

		int texWidth, texHeight, texChannels;
    	stbi_uc* pixels = stbi_load(_fp.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
    	size_t imageSize = texWidth * texHeight * STBI_rgb_alpha;

    	if (!pixels)
    	{
    		LOG_ERROR("Failed to load texture: {}", path);
    		return std::nullopt;
    	}

    	TextureDescription desc{};
    	std::vector<std::uint8_t> pixelsbuff;
    	pixelsbuff.resize(imageSize);
    	memcpy(pixelsbuff.data(), pixels, imageSize);
    	desc.Data = pixelsbuff;
    	desc.ImageWidth = texWidth;
    	desc.ImageHeight = texHeight;

    	stbi_image_free(pixels);

    	TextureHandle texture =  g_Engine.GetGraphicsDevice().CreateTexture(desc);

    	return texture;
	}

}
