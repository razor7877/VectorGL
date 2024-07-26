#include "materials/phongMaterial.hpp"

const std::string PhongMaterial::AMBIENT_COLOR = "material.ambient";
const std::string PhongMaterial::DIFFUSE_COLOR = "material.diffuse";
const std::string PhongMaterial::SPECULAR_COLOR = "material.specular";
const std::string PhongMaterial::SHININESS = "material.shininess";

const std::string PhongMaterial::USE_DIFFUSE_MAP = "material.use_diffuse_map";
const std::string PhongMaterial::USE_SPECULAR_MAP = "material.use_specular_map";
const std::string PhongMaterial::USE_NORMAL_MAP = "material.use_normal_map";
const std::string PhongMaterial::USE_HEIGHT_MAP = "material.use_height_map";
const std::string PhongMaterial::USE_EMISSIVE_MAP = "material.use_emissive_map";

const std::string PhongMaterial::TEXTURE_DIFFUSE = "material.texture_diffuse";
const std::string PhongMaterial::TEXTURE_SPECULAR = "material.texture_specular";
const std::string PhongMaterial::TEXTURE_NORMAL = "material.texture_normal";
const std::string PhongMaterial::TEXTURE_HEIGHT = "material.texture_height";
const std::string PhongMaterial::TEXTURE_EMISSIVE = "material.texture_emissive";

PhongMaterial::PhongMaterial()
{

}

PhongMaterial::PhongMaterial(std::shared_ptr<Texture> texture)
{
	this->addDiffuseMap(texture);
}

PhongMaterial::PhongMaterial(glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, float shininess, std::shared_ptr<Texture> texture)
{
	this->ambientColor = ambient;
	this->diffuseColor = diffuse;
	this->specularColor = specular;
	this->shininess = shininess;
	this->diffuseTexture = texture;
}

PhongMaterial::~PhongMaterial()
{

}

void PhongMaterial::sendToShader(Shader* shaderProgram)
{
	shaderProgram
		->setVec3(PhongMaterial::AMBIENT_COLOR, this->ambientColor)
		->setVec3(PhongMaterial::DIFFUSE_COLOR, this->diffuseColor)
		->setVec3(PhongMaterial::SPECULAR_COLOR, this->specularColor)
		->setFloat(PhongMaterial::SHININESS, this->shininess);

	// Send all data relevant to textures
	shaderProgram->setInt(PhongMaterial::USE_DIFFUSE_MAP, this->useDiffuseMap);
	shaderProgram->setInt(PhongMaterial::USE_SPECULAR_MAP, this->useSpecularMap);
	shaderProgram->setInt(PhongMaterial::USE_NORMAL_MAP, this->useNormalMap);
	shaderProgram->setInt(PhongMaterial::USE_HEIGHT_MAP, this->useHeightMap);
	shaderProgram->setInt(PhongMaterial::USE_EMISSIVE_MAP, this->useEmissiveMap);

	// Bind all the textures needed
	if (this->useDiffuseMap)
	{
		glActiveTexture(GL_TEXTURE0);
		shaderProgram->setInt(PhongMaterial::TEXTURE_DIFFUSE, 0);
		this->diffuseTexture->bindTexture();
	}

	if (this->useSpecularMap)
	{
		glActiveTexture(GL_TEXTURE1);
		shaderProgram->setInt(PhongMaterial::TEXTURE_SPECULAR, 1);
		this->specularTexture->bindTexture();
	}

	if (this->useNormalMap)
	{
		glActiveTexture(GL_TEXTURE2);
		shaderProgram->setInt(PhongMaterial::TEXTURE_NORMAL, 2);
		this->normalTexture->bindTexture();
	}

	if (this->useHeightMap)
	{
		glActiveTexture(GL_TEXTURE3);
		shaderProgram->setInt(PhongMaterial::TEXTURE_HEIGHT, 3);
		this->heightTexture->bindTexture();
	}

	if (this->useEmissiveMap)
	{
		glActiveTexture(GL_TEXTURE4);
		shaderProgram->setInt(PhongMaterial::TEXTURE_EMISSIVE, 4);
		this->emissiveTexture->bindTexture();
	}
}

void PhongMaterial::addTextures(std::vector<std::shared_ptr<Texture>> textures)
{
	// We sort the textures from the vector into their own members in the mesh for easier use later
	for (int i = 0; i < textures.size(); i++)
	{
		switch (textures[i]->type)
		{
			case TextureType::TEXTURE_DIFFUSE:
				this->addDiffuseMap(textures[i]);
				break;

			case TextureType::TEXTURE_SPECULAR:
				this->addSpecularMap(textures[i]);
				break;

			case TextureType::TEXTURE_NORMAL:
				this->addNormalMap(textures[i]);
				break;

			case TextureType::TEXTURE_HEIGHT:
				this->addHeightMap(textures[i]);
				break;

			case TextureType::TEXTURE_EMISSIVE:
				this->addEmissiveMap(textures[i]);
		}
	}
}

MaterialType PhongMaterial::getType()
{
	return MaterialType::PhongMaterial;
}

void PhongMaterial::addDiffuseMap(std::shared_ptr<Texture> diffuseTexture)
{
	this->useDiffuseMap = true;
	this->diffuseTexture = diffuseTexture;
}

void PhongMaterial::addSpecularMap(std::shared_ptr<Texture> specularTexture)
{
	this->useSpecularMap = true;
	this->specularTexture = specularTexture;
}

void PhongMaterial::addNormalMap(std::shared_ptr<Texture> normalTexture)
{
	this->useNormalMap = true;
	this->normalTexture = normalTexture;
}

void PhongMaterial::addHeightMap(std::shared_ptr<Texture> heightTexture)
{
	this->useHeightMap = true;
	this->heightTexture = heightTexture;
}

void PhongMaterial::addEmissiveMap(std::shared_ptr<Texture> emissiveTexture)
{
	this->useEmissiveMap = true;
	this->emissiveTexture = emissiveTexture;
}
