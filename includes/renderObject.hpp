#ifndef RENDEROBJECT_HPP
#define RENDEROBJECT_HPP

#include <glad/glad.h>

// An interface to be implemented by any object that should be drawn by a renderer
class RenderObject
{
public:
	GLuint shaderProgramID; // The ID of the shader used to draw the object

	RenderObject();

	virtual void drawObject() = 0;
	virtual void setupObject() = 0;
};

#endif