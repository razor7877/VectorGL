#include <iostream>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

#include "utilities/resourceLoader.hpp"
#include "utilities/stb_image.h"
#include "components/meshComponent.hpp"
#include "logger.hpp"

ResourceLoader ResourceLoader::instance;

ResourceLoader& ResourceLoader::getInstance()
{
	return ResourceLoader::instance;
}

Entity* ResourceLoader::loadModelFromFilepath(std::string path, Shader* shaderProgram)
{
	Assimp::Importer import;
	const aiScene* scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_PreTransformVertices);

	if (scene == nullptr)
	{
		Logger::logError("Error while trying to load file path " + path + " - Extension might be incorrect");
		return nullptr;
	}

	std::string sceneName = std::string(scene->mName.C_Str());

	// We create an entity that will contains all the necessary data
	Entity* modelEntity = new Entity();
	modelEntity->setLabel(sceneName);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << std::endl;
		return nullptr;
	}

	this->directory = path.substr(0, path.find_last_of('/'));

	this->processNode(scene->mRootNode, scene, shaderProgram, modelEntity);

	return modelEntity;
}

ResourceLoader::ResourceLoader()
{

}

void ResourceLoader::processNode(aiNode* node, const aiScene* scene, Shader* shaderProgram, Entity* parent)
{
	// Process all the node's meshes (if any)
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		Entity* newMesh = processMesh(mesh, scene, shaderProgram, parent);

		std::string nodeName = std::string(node->mName.C_Str());
		newMesh->setLabel(nodeName);

		newMesh->setParent(parent);
		parent->addChild(newMesh);
	}

	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		processNode(node->mChildren[i], scene, shaderProgram, parent);
	}
}

Entity* ResourceLoader::processMesh(aiMesh* mesh, const aiScene* scene, Shader* shaderProgram, Entity* parent)
{
	std::vector<float> vertices;
	std::vector<float> texCoords;
	std::vector<float> normals;
	std::vector<unsigned int> indices;

	std::vector<Texture*> textures;

	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		vertices.push_back(mesh->mVertices[i].x);
		vertices.push_back(mesh->mVertices[i].y);
		vertices.push_back(mesh->mVertices[i].z);

		if (mesh->HasNormals())
		{
			normals.push_back(mesh->mNormals[i].x);
			normals.push_back(mesh->mNormals[i].y);
			normals.push_back(mesh->mNormals[i].z);
		}

		if (mesh->HasTextureCoords(0))
		{
			texCoords.push_back(mesh->mTextureCoords[0][i].x);
			texCoords.push_back(mesh->mTextureCoords[0][i].y);
		}
		else
		{
			texCoords.push_back(0.0f);
			texCoords.push_back(0.0f);
		}
	}

	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++)
		{
			indices.push_back(face.mIndices[j]);
		}
	}

	glm::vec3 diffuseColor;
	bool useDiffuseColor = false;

	// Load all the textures needed
	if (mesh->mMaterialIndex >= 0)
	{
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

		std::vector<Texture*> diffuseMaps = loadMaterialTextures(scene, material, aiTextureType_DIFFUSE, "texture_diffuse");
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

		std::vector<Texture*> specularMaps = loadMaterialTextures(scene, material, aiTextureType_SPECULAR, "texture_specular");
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

		std::vector<Texture*> normalMaps = loadMaterialTextures(scene, material, aiTextureType_NORMALS, "texture_normal");
		textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());

		std::vector<Texture*> heightMaps = loadMaterialTextures(scene, material, aiTextureType_HEIGHT, "texture_height");
		textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

		if (diffuseMaps.size() > 1 || specularMaps.size() > 1 || normalMaps.size() > 1 || heightMaps.size() > 1)
			Logger::logWarning("Loading model that contains more textures than the shader allows (diffuse/specular/normal/height)");

		if (diffuseMaps.size() == 0)
		{
			aiColor4D diffuseColorVec;
			if (aiGetMaterialColor(material, AI_MATKEY_COLOR_DIFFUSE, &diffuseColorVec) == AI_SUCCESS)
			{
				diffuseColor = glm::vec3(diffuseColorVec.r, diffuseColorVec.g, diffuseColorVec.b);
				useDiffuseColor = true;
			}
		}

		std::vector<Texture*> metalnessMaps = loadMaterialTextures(scene, material, aiTextureType_METALNESS, "texture_metallic");
		textures.insert(textures.end(), metalnessMaps.begin(), metalnessMaps.end());

		std::vector<Texture*> roughnessMaps = loadMaterialTextures(scene, material, aiTextureType_DIFFUSE_ROUGHNESS, "texture_roughness");
		textures.insert(textures.end(), roughnessMaps.begin(), roughnessMaps.end());

		std::vector<Texture*> aoMaps = loadMaterialTextures(scene, material, aiTextureType_AMBIENT_OCCLUSION, "texture_ao");
		textures.insert(textures.end(), aoMaps.begin(), aoMaps.end());
	}

	Entity* entity = new Entity();
	MeshComponent* meshComponent = entity->addComponent<MeshComponent>();
	meshComponent->setupMesh(vertices, texCoords, normals, indices, textures, shaderProgram);

	if (useDiffuseColor)
		meshComponent->setDiffuseColor(diffuseColor);

	return entity;
}

std::vector<Texture*> ResourceLoader::loadMaterialTextures(const aiScene* scene, aiMaterial* mat, aiTextureType type, std::string typeName)
{
	std::vector<Texture*> textures;

	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString str;
		mat->GetTexture(type, i, &str);
		std::string path = directory + '/' + std::string(str.C_Str());

		Texture* texture;

		// Texture has not been loaded yet
		if (this->loadedTextures.count(path) == 0)
		{
			// To map each value to the corresponding TextureType
			TextureType textureType;
			switch (type)
			{
				case aiTextureType_DIFFUSE:
					textureType = TextureType::TEXTURE_DIFFUSE;
					break;

				case aiTextureType_SPECULAR:
					textureType = TextureType::TEXTURE_SPECULAR;
					break;

				case aiTextureType_NORMALS:
					textureType = TextureType::TEXTURE_NORMAL;
					break;

				case aiTextureType_HEIGHT:
					textureType = TextureType::TEXTURE_HEIGHT;
					break;

				case aiTextureType_METALNESS:
					textureType = TextureType::TEXTURE_METALLIC;
					break;

				case aiTextureType_DIFFUSE_ROUGHNESS:
					textureType = TextureType::TEXTURE_ROUGHNESS;
					break;

				case aiTextureType_AMBIENT_OCCLUSION:
					textureType = TextureType::TEXTURE_AO;
					break;
			}

			// If this is not null, we have an embedded texture, for example in GLB models
			if (auto embeddedTexture = scene->GetEmbeddedTexture(str.C_Str()))
			{
				// This means we have a compressed texture that we need to decompress
				if (embeddedTexture->mHeight == 0)
				{
					Logger::logInfo(std::string("Loadding embedded compressed texture path " + path));

					int width = embeddedTexture->mWidth;
					int height = 1;
					int len = width * height;
					int channels = 3;

					GLenum format = GL_RGB;

					// Handle transparency layer for PNG data
					if (embeddedTexture->achFormatHint[0] == 'p' && embeddedTexture->achFormatHint[1] == 'n' && embeddedTexture->achFormatHint[2] == 'g')
					{
						format = GL_RGBA;
						channels = 4;
					}

					// Decompress image
					unsigned char* data = stbi_load_from_memory((const stbi_uc*)embeddedTexture->pcData, len, &width, &height, &channels, channels);
					// Create texture using image data
					texture = new Texture(textureType, width, height, format, data);
					stbi_image_free(data);
				}
				else // We have raw image data
				{
					Logger::logInfo(std::string("Loading embedded texture path " + path));

					GLenum format = GL_RGB;

					// Check the format hint for a transparency layer
					for (int i = 0; i < 4; i++)
						if (embeddedTexture->achFormatHint[i] == 'A' && embeddedTexture->achFormatHint[i] != '0')
							format = GL_RGBA;

					texture = new Texture(textureType, embeddedTexture->mWidth, embeddedTexture->mHeight, format, embeddedTexture->pcData);
				}
			}
			else // Not an embedded texture, load it from file system
			{
				Logger::logInfo(std::string("Loading texture path " + path));
				texture = new Texture(path, textureType, false);
				texture->path = str.C_Str();
				this->loadedTextures[path] = texture;
			}
		}
		else // Reuse the same texture
		{
			Logger::logInfo(std::string("Reusing texture path " + path));
			texture = this->loadedTextures[path];
		}

		textures.push_back(texture);
	}

	return textures;
}
