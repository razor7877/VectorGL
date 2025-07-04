#pragma once

#include <memory>

#include <assimp/scene.h>

#include "shader.hpp"
#include "entity.hpp"
#include "texture.hpp"

class ResourceLoader
{
public:
	static ResourceLoader& getInstance();

	std::unique_ptr<Entity> loadModelFromFilepath(const std::string& path, Shader* shaderProgram);
	
private:
	static ResourceLoader instance;
	static std::map<aiTextureType, TextureType> aiMatToTextureType;

	std::string directory;
	std::map<std::string, std::weak_ptr<Texture>> loadedTextures;

	ResourceLoader();
	ResourceLoader(ResourceLoader const&) = delete;
	ResourceLoader& operator=(ResourceLoader const&) = delete;

	void processNode(const aiNode* node, const aiScene* scene, Shader* shaderProgram, Entity* parent);
	Entity* processMesh(aiMesh* mesh, const aiScene* scene, Shader* shaderProgram, Entity* parent);
	std::vector<std::shared_ptr<Texture>> loadMaterialTextures(const aiScene* scene, const aiMaterial* mat, aiTextureType type, const std::string& typeName);
};