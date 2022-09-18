#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include <string>

#include <glad/glad.h>

// A helper class to easily load and use 2D textures
class Texture
{
public:
	unsigned int texID;
	std::string type;
	std::string path;

	Texture();

	Texture(std::string filename, std::string type, bool stbiFlipOnLoad = false);

	void bindTexture();
};

#endif