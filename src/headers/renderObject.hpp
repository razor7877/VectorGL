#ifndef RENDEROBJECT_HPP
#define RENDEROBJECT_HPP

// An interface to be implemented by any object that should be drawn by a renderer
class renderObject
{
public:
	GLuint shaderProgramID; // The ID of the shader used to draw the object

	virtual void drawObject() = 0;
};

#endif