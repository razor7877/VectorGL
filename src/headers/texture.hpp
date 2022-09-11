#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include <glad/glad.h>

// A helper class to easily load and use 2D textures
class Texture
{
public:
	unsigned int texID;

	Texture();

	Texture(const char* filename, GLuint colorModel, bool stbiFlipOnLoad);

	void bindTexture();
};

#endif