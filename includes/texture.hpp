#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include <string>

#include "utilities/glad.h"

enum class TextureType
{
	TEXTURE_DIFFUSE = 0,
	TEXTURE_ALBEDO = 0,
	TEXTURE_SPECULAR = 1,
	TEXTURE_NORMAL = 2,
	TEXTURE_HEIGHT = 3,
	TEXTURE_METALLIC = 4,
	TEXTURE_ROUGHNESS = 5,
	TEXTURE_AO = 6,
	TEXTURE_OPACITY = 7,
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