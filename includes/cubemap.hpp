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
	unsigned int texID;
	std::vector<std::string> faces;

	Cubemap();
	~Cubemap();

	// A constructor that builds a cubemap from a path that contains 6 PNG files named
	// right left top bottom front back .png
	Cubemap(std::string facesPath);
	// A constructor that builds a cubemap directly from any 6 given images path
	Cubemap(std::vector<std::string> faces);

	void setupObject();
};

#endif