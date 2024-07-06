#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include <string>

#include "utilities/glad.h"

// A helper class to easily load and use 2D textures
class Texture
{
public:
	unsigned int texID;
	std::string type;
	std::string path;

	Texture();
	~Texture();

	Texture(std::string filename, std::string type, bool stbiFlipOnLoad = false);
	Texture(int width, int height, GLenum format, void* textureData);

	void bindTexture();
};

#endif