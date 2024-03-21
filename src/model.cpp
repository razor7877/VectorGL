#include <iostream>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

#include "model.hpp"

Model::Model()
{

}

Model::Model(std::string path, GLuint shaderProgramID)
{
	this->shaderProgramID = shaderProgramID;
	loadModel(path);
}

Model::~Model()
{
	for (RenderObject* mesh : this->children)
		delete mesh;

	for (auto& [path, texture] : this->loadedTextures)
		delete texture;
}

void Model::drawObject()
{
	for (RenderObject* mesh : this->children)
	{
		mesh->drawObject();
	}
}

void Model::setupObject()
{
	for (RenderObject* mesh : this->children)
	{
		mesh->setupObject();
	}
}

std::string Model::getLabel() { return "Model"; }

void Model::loadModel(std::string path)
{
	Assimp::Importer import;
	const aiScene* scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << std::endl;
		return;
	}

	directory = path.substr(0, path.find_last_of('/'));

	processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode* node, const aiScene* scene)
{
	// Process all the node's meshes (if any)
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		Mesh* newMesh = processMesh(mesh, scene);
		newMesh->setParent(this);
		this->children.push_back(newMesh);
	}

	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		processNode(node->mChildren[i], scene);
	}
}

Mesh* Model::processMesh(aiMesh* mesh, const aiScene* scene)
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

	Mesh* newMesh = new Mesh(vertices, texCoords, normals, indices, textures, shaderProgramID);

	return newMesh;
}

std::vector<Texture*> Model::loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName)
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

Model* Model::rotateModel(float degrees, glm::vec3 rotationPoint)
{
	for (RenderObject* mesh : this->children)
	{
		mesh->rotateObject(degrees, rotationPoint);
	}
	return this;
}

Model* Model::rotateModel(float degrees, float x, float y, float z)
{
	for (RenderObject* mesh : this->children)
	{
		mesh->rotateObject(degrees, x, y, z);
	}
	return this;
}

Model* Model::translateModel(glm::vec3 translation)
{
	for (RenderObject* mesh : this->children)
	{
		mesh->translateObject(translation);
	}
	return this;
}

Model* Model::translateModel(float x, float y, float z)
{
	for (RenderObject* mesh : this->children)
	{
		mesh->translateObject(x, y, z);
	}
	return this;
}

Model* Model::scaleModel(glm::vec3 scaleVec)
{
	for (RenderObject* mesh : this->children)
	{
		mesh->scaleObject(scaleVec);
	}
	return this;
}

Model* Model::scaleModel(float scaleX, float scaleY, float scaleZ)
{
	for (RenderObject* mesh : this->children)
	{
		mesh->scaleObject(scaleX, scaleY, scaleZ);
	}
	return this;
}