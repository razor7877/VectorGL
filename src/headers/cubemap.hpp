#ifndef CUBEMAP_HPP
#define CUBEMAP_HPP

#include <glad/glad.h>

class Cubemap
{
public:
	unsigned int texID;

	Cubemap(const char* filename, GLuint colorModel, bool stbiFlipOnLoad);
};

#endif