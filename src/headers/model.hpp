#ifndef MODEL_HPP
#define MODEL_HPP

#include <vector>
#include <string>

#include <assimp/scene.h>

#include "renderObject.hpp"
#include "mesh.hpp"

class Model : public virtual renderObject
{
public:
	GLuint shaderProgramID;
	std::vector<Mesh> meshes;
	Model();
	Model(std::string path, GLuint shaderProgramID);

	void renderObject::drawObject() override;
	void renderObject::setupObject() override;
	renderObjectType renderObject::getType() override;

private:
	
	std::string directory;

	void loadModel(std::string path);
	void processNode(aiNode* node, const aiScene* scene);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);
	std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
};

#endif