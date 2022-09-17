#ifndef CUBEMAP_HPP
#define CUBEMAP_HPP

#include <vector>
#include <string>

#include <glad/glad.h>

class Cubemap
{
public:
	unsigned int texID;

	Cubemap();

	// A constructor that builds a cubemap from a path that contains 6 PNG files named
	// right left top bottom front back .png
	Cubemap(std::string facesPath);
	// A constructor that builds a cubemap directly from any 6 given images path
	Cubemap(std::vector<std::string> faces);
};

#endif