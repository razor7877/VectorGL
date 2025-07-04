#include <utilities/glad.h>
#include <utilities/stb_image.h>

#include "texture.hpp"
#include "logger.hpp"

Texture::Texture()
{
	this->texID = {};
}

Texture::Texture(const std::string& filename, TextureType textureType, bool isHDR, bool stbiFlipOnLoad)
{
	this->type = textureType;
	this->isHDR = isHDR;

	if (isHDR)
		this->createHDRTexture(filename, textureType, stbiFlipOnLoad);
	else
		this->createTexture(filename, textureType, stbiFlipOnLoad);
}

Texture::Texture(TextureType textureType, int width, int height, GLenum format, const void* textureData)
{
	this->type = textureType;
	this->width = width;
	this->height = height;

	// Create OpenGL texture
	glGenTextures(1, &texID);
	glBindTexture(GL_TEXTURE_2D, texID);

	// Sets parameters for texture wrapping and scaling
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, textureData);
	glGenerateMipmap(GL_TEXTURE_2D);
}

Texture::Texture(GLuint texture, TextureType textureType)
{
	this->texID = texture;
	this->type = textureType;
}

Texture::~Texture()
{
	glDeleteTextures(1, &this->texID);
}

void Texture::bindTexture() const
{
	if (this->type == TextureType::TEXTURE_3D)
		glBindTexture(GL_TEXTURE_2D_ARRAY, this->texID);
	else
		glBindTexture(GL_TEXTURE_2D, this->texID);
}

void Texture::createTexture(const std::string& filename, TextureType textureType, bool stbiFlipOnLoad)
{
	int width, height, nrChannels;

	stbi_set_flip_vertically_on_load(stbiFlipOnLoad);

	// Attempts loading image data from given filename
	if (unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrChannels, 0))
	{
		GLenum format = GL_RGB;
		if (nrChannels == 1)
			format = GL_RED;
		else if (nrChannels == 2)
			format = GL_RG;
		else if (nrChannels == 3)
			format = GL_RGB;
		else if (nrChannels == 4)
			format = GL_RGBA;

		this->format = format;

		// Create OpenGL texture
		glGenTextures(1, &this->texID);
		glBindTexture(GL_TEXTURE_2D, this->texID);

		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		// Sets parameters for texture wrapping and scaling
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
		Logger::logError(std::string("Failed to load texture: ") + filename, "texture.cpp");

	this->width = width;
	this->height = height;
}

void Texture::createHDRTexture(const std::string& filename, TextureType textureType, bool stbiFlipOnLoad)
{
	int width, height, nrChannels;

	stbi_set_flip_vertically_on_load(stbiFlipOnLoad);

	// Attempts loading image data from given filename
	if (float* data = stbi_loadf(filename.c_str(), &width, &height, &nrChannels, 0))
	{
		// Create OpenGL texture
		glGenTextures(1, &this->texID);
		glBindTexture(GL_TEXTURE_2D, this->texID);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, data);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
		Logger::logError(std::string("Failed to load HDR texture: ") + filename, "texture.cpp");

	this->format = GL_RGB;
	this->width = width;
	this->height = height;
}
