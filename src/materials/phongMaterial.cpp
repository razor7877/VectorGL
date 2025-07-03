#include "materials/phongMaterial.hpp"
#include "logger.hpp"

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

PhongMaterial::PhongMaterial(Shader* shaderProgram) : Material(shaderProgram)
{
	this->PhongMaterial::init();
}

PhongMaterial::PhongMaterial(Shader* shaderProgram, const std::shared_ptr<Texture>& texture) : Material(shaderProgram)
{
	this->PhongMaterial::init();
	this->addDiffuseMap(texture);
}

PhongMaterial::PhongMaterial(Shader* shaderProgram, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, float shininess, const std::shared_ptr<Texture>& texture)
	: Material(shaderProgram)
{
	this->ambientColor = ambient;
	this->diffuseColor = diffuse;
	this->specularColor = specular;
	this->shininess = shininess;
	this->diffuseTexture = texture;
}

PhongMaterial::~PhongMaterial() = default;

void PhongMaterial::init()
{
	this->shaderProgram->setInt(PhongMaterial::TEXTURE_DIFFUSE, 0);
	this->shaderProgram->setInt(PhongMaterial::TEXTURE_SPECULAR, 1);
	this->shaderProgram->setInt(PhongMaterial::TEXTURE_NORMAL, 2);
	this->shaderProgram->setInt(PhongMaterial::TEXTURE_HEIGHT, 3);
	this->shaderProgram->setInt(PhongMaterial::TEXTURE_EMISSIVE, 4);
}

void PhongMaterial::sendToShader()
{
	this->shaderProgram
		->setVec3(PhongMaterial::AMBIENT_COLOR, this->ambientColor)
		->setVec3(PhongMaterial::DIFFUSE_COLOR, this->diffuseColor)
		->setVec3(PhongMaterial::SPECULAR_COLOR, this->specularColor)
		->setFloat(PhongMaterial::SHININESS, this->shininess);

	// Send all data relevant to textures
	this->shaderProgram->setInt(PhongMaterial::USE_DIFFUSE_MAP, this->useDiffuseMap);
	this->shaderProgram->setInt(PhongMaterial::USE_SPECULAR_MAP, this->useSpecularMap);
	this->shaderProgram->setInt(PhongMaterial::USE_NORMAL_MAP, this->useNormalMap);
	this->shaderProgram->setInt(PhongMaterial::USE_HEIGHT_MAP, this->useHeightMap);
	this->shaderProgram->setInt(PhongMaterial::USE_EMISSIVE_MAP, this->useEmissiveMap);

	// Bind all the textures needed
	if (this->useDiffuseMap)
	{
		glActiveTexture(GL_TEXTURE0);
		this->diffuseTexture->bindTexture();
	}

	if (this->useSpecularMap)
	{
		glActiveTexture(GL_TEXTURE1);
		this->specularTexture->bindTexture();
	}

	if (this->useNormalMap)
	{
		glActiveTexture(GL_TEXTURE2);
		this->normalTexture->bindTexture();
	}

	if (this->useHeightMap)
	{
		glActiveTexture(GL_TEXTURE3);
		this->heightTexture->bindTexture();
	}

	if (this->useEmissiveMap)
	{
		glActiveTexture(GL_TEXTURE4);
		this->emissiveTexture->bindTexture();
	}
}

bool PhongMaterial::getIsTransparent()
{
	return false;
}

void PhongMaterial::addTextures(const std::vector<std::shared_ptr<Texture>>& textures)
{
	// We sort the textures from the vector into their own members in the mesh for easier use later
	for (const auto & texture : textures)
	{
		switch (texture->type)
		{
			case TextureType::TEXTURE_DIFFUSE:
				this->addDiffuseMap(texture);
				break;

			case TextureType::TEXTURE_SPECULAR:
				this->addSpecularMap(texture);
				break;

			case TextureType::TEXTURE_NORMAL:
				this->addNormalMap(texture);
				break;

			case TextureType::TEXTURE_HEIGHT:
				this->addHeightMap(texture);
				break;

			case TextureType::TEXTURE_EMISSIVE:
				this->addEmissiveMap(texture);

			default:
				Logger::logError("Got incorrect texture type while adding textures to PhongMaterial!", "phongMaterial.cpp");
				break;
		}
	}
}

void PhongMaterial::addDiffuseMap(const std::shared_ptr<Texture> &diffuseTexture)
{
	this->useDiffuseMap = true;
	this->diffuseTexture = diffuseTexture;
}

void PhongMaterial::addSpecularMap(const std::shared_ptr<Texture> &specularTexture)
{
	this->useSpecularMap = true;
	this->specularTexture = specularTexture;
}

void PhongMaterial::addNormalMap(const std::shared_ptr<Texture> &normalTexture)
{
	this->useNormalMap = true;
	this->normalTexture = normalTexture;
}

void PhongMaterial::addHeightMap(const std::shared_ptr<Texture> &heightTexture)
{
	this->useHeightMap = true;
	this->heightTexture = heightTexture;
}

void PhongMaterial::addEmissiveMap(const std::shared_ptr<Texture> &emissiveTexture)
{
	this->useEmissiveMap = true;
	this->emissiveTexture = emissiveTexture;
}
