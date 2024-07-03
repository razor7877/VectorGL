#include <iostream>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

#include "utilities/resourceLoader.hpp"
#include "components/meshComponent.hpp"

ResourceLoader ResourceLoader::instance;

ResourceLoader ResourceLoader::getInstance()
{
	return ResourceLoader::instance;
}

Entity* ResourceLoader::loadModelFromFilepath(std::string path, GLuint shaderProgramID)
{
	Assimp::Importer import;
	const aiScene* scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

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

	this->processNode(scene->mRootNode, scene, shaderProgramID, modelEntity);

	return modelEntity;
}

ResourceLoader::ResourceLoader()
{

}

void ResourceLoader::processNode(aiNode* node, const aiScene* scene, GLuint shaderProgramID, Entity* parent)
{
	// Process all the node's meshes (if any)
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		Entity* newMesh = processMesh(mesh, scene, shaderProgramID, parent);

		std::string nodeName = std::string(node->mName.C_Str());
		newMesh->setLabel(nodeName);

		newMesh->setParent(parent);
		parent->addChild(newMesh);
	}

	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		processNode(node->mChildren[i], scene, shaderProgramID, parent);
	}
}

Entity* ResourceLoader::processMesh(aiMesh* mesh, const aiScene* scene, GLuint shaderProgramID, Entity* parent)
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

		if (mesh->mTextureCoords[0])
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

	if (mesh->mMaterialIndex >= 0)
	{
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

		std::vector<Texture*> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

		std::vector<Texture*> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

		std::vector<Texture*> normalMaps = loadMaterialTextures(material, aiTextureType_NORMALS, "texture_normal");
		textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());

		std::vector<Texture*> heightMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_height");
		textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());
	}

	Entity* entity = new Entity();
	MeshComponent* meshComponent = entity->addComponent<MeshComponent>();
	meshComponent->setupMesh(vertices, texCoords, normals, indices, textures, shaderProgramID);

	return entity;
}

std::vector<Texture*> ResourceLoader::loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName)
{
	std::vector<Texture*> textures;
	std::cout << "loadMaterialTexture()\n";
	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString str;
		mat->GetTexture(type, i, &str);
		std::string path = directory + '/' + std::string(str.C_Str());

		Texture* texture;

		// Texture has not been loaded yet
		if (this->loadedTextures.count(path) == 0)
		{
			std::cout << "Loading texture path: " << path << "\n";
			texture = new Texture(path, typeName, false);
			texture->path = str.C_Str();
			this->loadedTextures[path] = texture;
		}
		else
		{
			std::cout << "Reusing texture\n";
			texture = this->loadedTextures[path];
		}

		textures.push_back(texture);
	}

	return textures;
}
