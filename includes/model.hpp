#ifndef MODEL_HPP
#define MODEL_HPP

#include <vector>
#include <string>

#include <assimp/scene.h>

#include "renderObject.hpp"
#include "mesh.hpp"

// A model instance can be any sort of object such a complex 3D objects that may contain multiple meshes, each with their own textures and materials
// The model serves to load 3D models from files, and also serves to group up a single object's meshes together
class Model : public virtual RenderObject
{
public:
	// We use a map to keep track of any textures that are already loaded and might be reused
	std::map<std::string, Texture*> loadedTextures;
	Model();
	Model(std::string path, GLuint shaderProgramID);
	~Model();

	void RenderObject::drawObject() override;
	void RenderObject::setupObject() override;

private:
	std::string directory;

	void loadModel(std::string path);
	void processNode(aiNode* node, const aiScene* scene);
	Mesh* processMesh(aiMesh* mesh, const aiScene* scene);
	std::vector<Texture*> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
};

#endif