#include <iostream>

#include <utilities/glad.h>
#include <utilities/stb_image.h>

#include "texture.hpp"

Texture::Texture()
{

}

Texture::Texture(std::string filename, std::string type, bool stbiFlipOnLoad)
{
	this->type = type;

	// Create OpenGL texture
	glGenTextures(1, &texID);
	glBindTexture(GL_TEXTURE_2D, texID);

	// Sets parameters for texture wrapping and scaling
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int width, height, nrChannels;

	stbi_set_flip_vertically_on_load(stbiFlipOnLoad);

	// Attempts loading image data from given filename
	unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrChannels, 0);
	if (data)
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

		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture: " << filename << std::endl;
	}

	stbi_image_free(data);
}

Texture::~Texture()
{
	glDeleteTextures(1, &this->texID);
}

void Texture::bindTexture()
{
	glBindTexture(GL_TEXTURE_2D, texID);
}