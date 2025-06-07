#include "Assets/AssetLoader.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <stb_image.h>

// Need access to the graphics device
#include "Engine/Engine.h"

namespace QE
{
	void ProcessNode(aiNode* node, const aiScene* scene, Model* model);
	MeshHandle ProcessMesh(aiMesh* mesh, const aiScene* scene);

    std::optional<Model> LoadModel(const std::string &path)
    {
        LOG_DEBUG("Loading Model: {}", path);

		std::string _fp = QE_RESOURCES_FOLDER;
		_fp += path;
		LOG_DEBUG("File path appended: {}", _fp);

		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(_fp,
			aiProcess_CalcTangentSpace |
			aiProcess_Triangulate |
			aiProcess_JoinIdenticalVertices |
			aiProcess_SortByPType |
			aiProcess_FlipUVs |
			aiProcess_GenSmoothNormals
		);

		if (scene == nullptr)
		{
			LOG_ERROR("Failed to load Model:\n\t File: {}\n\t {}", path, importer.GetErrorString());
			return std::nullopt;
		}

		Model model;

    	ProcessNode(scene->mRootNode, scene, &model);

		return model;
    }

	void ProcessNode(aiNode* node, const aiScene* scene, Model* model)
    {
    	// Process each mesh located at the current node
		for (unsigned int i = 0; i < node->mNumMeshes; i++)
		{
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];

			LOG_DEBUG("Mesh {}: {}", i, mesh->mName.C_Str());
			LOG_DEBUG("\tVertex count: {}", mesh->mNumVertices);
			LOG_DEBUG("\tFace count: {}", mesh->mNumFaces);
			LOG_DEBUG("\tIndice count: {}", mesh->mFaces->mNumIndices * mesh->mNumFaces);

			model->Meshes.push_back(ProcessMesh(mesh, scene));
		}
    	// After mesh processing, recursively process children nodes
    	for (unsigned int i = 0; i < node->mNumChildren; i++)
    	{
    		ProcessNode(node->mChildren[i], scene, model);
    	}
    }

	MeshHandle ProcessMesh(aiMesh* mesh, const aiScene* scene)
    {
    	std::vector<uint32_t> indices;
    	std::vector<Vertex> vertices;
    	// Textures here once it exists

    	// Go through each mesh's vertices
    	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    	{
    		Vertex newVtx;
    		newVtx.Position.x = mesh->mVertices[i].x;
    		newVtx.Position.y = mesh->mVertices[i].y;
    		newVtx.Position.z = mesh->mVertices[i].z;

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

    	/*std::vector<std::uint8_t> verticesbuff;
    	verticesbuff.resize(vertices.size() * sizeof(Vertex));
    	memcpy(verticesbuff.data(), vertices.data(), verticesbuff.size());
    	BufferDescription verticesDesc = {
    		BufferType::Vertex,
			BufferUsage::Default,
			verticesbuff,
			vertices.size() * sizeof(Vertex),
			vertices.size()
		};
    	newMesh.VertexBuffer = g_Engine.GetGraphicsDevice().CreateBuffer(verticesDesc);

    	//RawBuffer indicesbuff = RawBuffer(indices.data(),indices.size(), indices.size() * sizeof(std::uint32_t));
    	std::vector<std::uint8_t> indicesbuff(indices.size());
    	indicesbuff.resize(indices.size() * sizeof(std::uint32_t));
    	memcpy(indicesbuff.data(), indices.data(), indicesbuff.size());
    	BufferDescription indicesDesc = {
    		BufferType::Index,
			BufferUsage::Default,
			indicesbuff,
			indices.size() * sizeof(std::uint32_t),
			indices.size()
		};
    	newMesh.IndexBuffer = g_Engine.GetGraphicsDevice().CreateBuffer(indicesDesc);*/

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
    	desc.ImageSize = imageSize;

    	stbi_image_free(pixels);

    	TextureHandle texture =  g_Engine.GetGraphicsDevice().CreateTexture(desc);

    	return texture;
	}

}