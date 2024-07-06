#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include <string>

#include "utilities/glad.h"

enum class TextureType
{
	TEXTURE_DIFFUSE,
	TEXTURE_SPECULAR,
	TEXTURE_NORMAL,
	TEXTURE_HEIGHT,
};

// A helper class to easily load and use 2D textures
class Texture
{
public:
	unsigned int texID;
	TextureType type;
	std::string path;

	Texture();
	~Texture();

	Texture(std::string filename, TextureType textureType, bool stbiFlipOnLoad = false);
	Texture::Texture(TextureType textureType, int width, int height, GLenum format, void* textureData);

	void bindTexture();
};

#endif