#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include <string>

#include "utilities/glad.h"

enum class TextureType
{
	TEXTURE_DIFFUSE = 0, // Diffuse texture (Phong)
	TEXTURE_ALBEDO = 0, // Albedo texture (PBR)
	TEXTURE_SPECULAR = 1, // Specular map (Phong)
	TEXTURE_NORMAL = 2, // Normal map
	TEXTURE_HEIGHT = 3, // Height map
	TEXTURE_METALLIC = 4, // Metallic map (PBR)
	TEXTURE_ROUGHNESS = 5, // Roughness map (PBR)
	TEXTURE_AO = 6, // Ambient occlusion map
	TEXTURE_OPACITY = 7, // Opacity map
	TEXTURE_EMISSIVE = 8, // Emissive map
	TEXTURE_2D = 9, // Any 2D texture
	TEXTURE_3D = 10, // Any 3D texture (layered textures)
};

// A helper class to easily load and use 2D textures
class Texture
{
public:
	/// <summary>
	/// Whether this is an HDR texture or not
	/// </summary>
	bool isHDR = false;

	/// <summary>
	/// The OpenGL handle for the texture
	/// </summary>
	GLuint texID = 0;

	/// <summary>
	/// The OpenGL enum for the texture format
	/// </summary>
	GLenum format = 0;

	/// <summary>
	/// An enum representing the type of texture
	/// </summary>
	TextureType type;

	/// <summary>
	/// The path to the texture, if it was loaded from a file
	/// </summary>
	std::string path;

	/// <summary>
	/// The width of the texture in pixels
	/// </summary>
	float width = 0;

	/// <summary>
	/// The height of the texture in pixels
	/// </summary>
	float height = 0;

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
    Texture(TextureType textureType, int width, int height, GLenum format, void* textureData);

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
