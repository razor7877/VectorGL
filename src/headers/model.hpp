#ifndef MODEL_HPP
#define MODEL_HPP

#include <vector>
#include <string>

#include <assimp/scene.h>

#include "renderObject.hpp"
#include "mesh.hpp"

// A model instance can be any sort of object such a complex 3D objects that may contain multiple meshes, each with their own textures and materials
// The model serves to load 3D models from files, and also serves to group up a single object's meshes together
class Model : public virtual renderObject
{
public:
	std::vector<Mesh> meshes;
	Model();
	Model(std::string path, GLuint shaderProgramID);

	void renderObject::drawObject() override;
	void renderObject::setupObject() override;
	renderObjectType renderObject::getType() override;

	// Rotates the object's model matrix using a vec3 or xyz floats
	void rotateModel(float degrees, glm::vec3 rotationPoint);
	void rotateModel(float degrees, float x, float y, float z);

	// Translate the object's model matrix using a vec3 or xyz floats
	void translateModel(glm::vec3 translation);
	void translateModel(float x, float y, float z);

	// Scales the object's model matrix using a vec3 or xyz floats
	void scaleModel(glm::vec3 scaleVec);
	void scaleModel(float scaleX, float scaleY, float scaleZ);

private:
	
	std::string directory;

	void loadModel(std::string path);
	void processNode(aiNode* node, const aiScene* scene);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);
	std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
};

#endif