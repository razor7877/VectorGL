#ifndef CUBEMAP_HPP
#define CUBEMAP_HPP

#include <vector>
#include <string>

#include <utilities/glad.h>

// A simple class used to easily build an OpenGL cubemap from image files passed as constructor arguments
// It may be used to for example draw a skybox

class Cubemap
{
public:
	/// <summary>
	/// The OpenGL handle for the cubemap texture
	/// </summary>
	GLuint texID = 0;

	/// <summary>
	/// A vector containing the file paths for the face images, if cubemap was loaded from image files
	/// </summary>
	std::vector<std::string> faces;

	Cubemap();

	/// <summary>
	/// A constructor that builds a cubemap from a path that contains 6 PNG files named: right left top bottom front back .png
	/// </summary>
	/// <param name="facesPath">The path to the PNG files</param>
	Cubemap(std::string facesPath);

	/// <summary>
	/// A constructor that builds a cubemap directly from any 6 given images path
	/// </summary>
	/// <param name="faces">A vector contaning the paths to 6 images</param>
	Cubemap(std::vector<std::string> faces);

	/// <summary>
	/// Creates a cubemap object from an OpenGL cubemap texture handle
	/// </summary>
	/// <param name="cubemap">An OpenGL handle for a cubemap</param>
	Cubemap(GLuint cubemap);

	Cubemap(GLenum format, int width, int height, bool mipMapFiltering = false);

	~Cubemap();

	/// <summary>
	/// Binds the cubemap texture
	/// </summary>
	void bind();

	/// <summary>
	/// Generates mip levels for the cubemap
	/// </summary>
	void generateMipMaps();

private:
	void createCubemapFromFaces();
};

#endif