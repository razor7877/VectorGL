#include <iostream>

#include <stb_image.h>

#include "headers/texture.hpp"

Texture::Texture()
{

}

Texture::Texture(const char* filename, GLuint colorModel, bool stbiFlipOnLoad = false)
{
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
	unsigned char* data = stbi_load(filename, &width, &height, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, colorModel, width, height, 0, colorModel, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}

	stbi_image_free(data);
}

void Texture::bindTexture()
{
	glBindTexture(GL_TEXTURE_2D, texID);
}