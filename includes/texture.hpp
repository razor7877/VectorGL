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
	TEXTURE_EMISSIVE = 8,
};

// A helper class to easily load and use 2D textures
class Texture
{
public:
	bool isHDR = false;
	GLuint texID = 0;
	TextureType type;
	std::string path;

	Texture();

	/// <summary>
	/// Creates a new texture from a file path
	/// </summary>
	/// <param name="filename">A path to the file to load</param>
	/// <param name="textureType">The type of texture to create</param>
	/// <param name="isHDR">Whether this is an HDR texture</param>
	/// <param name="stbiFlipOnLoad">Whether the image should be flipped on load</param>
	Texture(std::string filename, TextureType textureType, bool isHDR = false, bool stbiFlipOnLoad = false);

	/// <summary>
	/// Creates a new texture using a data buffer
	/// </summary>
	/// <param name="textureType">The type of texture to create</param>
	/// <param name="width">The width of the texture</param>
	/// <param name="height">The height of the texture</param>
	/// <param name="format">The format of the texture</param>
	/// <param name="textureData">A pointer to the texture data, which must correspond in size to the width/height/format specified</param>
	Texture::Texture(TextureType textureType, int width, int height, GLenum format, void* textureData);

	/// <summary>
	/// Creates a new texture using an existing OpenGL texture handle
	/// </summary>
	/// <param name="texture">The OpenGL texture handle</param>
	Texture(GLuint texture, TextureType textureType);

	~Texture();

	void bindTexture();

private:
	void createTexture(std::string filename, TextureType textureType, bool stbiFlipOnLoad = false);
	void createHDRTexture(std::string filename, TextureType textureType, bool stbiFlipOnLoad = false);
};

#endif