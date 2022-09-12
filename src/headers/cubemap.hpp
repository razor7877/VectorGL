#ifndef CUBEMAP_HPP
#define CUBEMAP_HPP

#include <vector>
#include <string>

#include <glad/glad.h>

class Cubemap
{
public:
	unsigned int texID;
	unsigned int shaderProgramID;

	Cubemap(std::vector<std::string> faces);
};

#endif