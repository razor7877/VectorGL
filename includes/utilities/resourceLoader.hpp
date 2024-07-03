#pragma once

#include <glad/glad.h>
#include <assimp/scene.h>

#include "entity.hpp"
#include "texture.hpp"

class ResourceLoader
{
public:
	static ResourceLoader getInstance();

	Entity* loadModelFromFilepath(std::string path, GLuint shaderProgramID);
	
private:
	static ResourceLoader instance;

	std::string directory;
	std::map<std::string, Texture*> loadedTextures;

	ResourceLoader();

	void processNode(aiNode* node, const aiScene* scene, GLuint shaderProgramID, Entity* parent);
	Entity* processMesh(aiMesh* mesh, const aiScene* scene, GLuint shaderProgramID, Entity* parent);
	std::vector<Texture*> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
};