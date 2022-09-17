#ifndef RENDEROBJECT_HPP
#define RENDEROBJECT_HPP

enum class renderObjectType
{
	OBJECT_MESH,
};

// An interface to be implemented by any object that should be drawn by a renderer
class renderObject
{
public:
	GLuint shaderProgramID; // The ID of the shader used to draw the object

	virtual void drawObject() = 0;
	virtual renderObjectType getType() = 0;
};

#endif