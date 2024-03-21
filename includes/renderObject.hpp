#ifndef RENDEROBJECT_HPP
#define RENDEROBJECT_HPP

#include <vector>

#include <glad/glad.h>

// An interface to be implemented by any object that should be drawn by a renderer
// TODO : A render object should keep transform matrices and have methods to apply transforms (move/scale/rotate)
class RenderObject
{
public:
	// The ID of the shader used to draw the object
	GLuint shaderProgramID;

	RenderObject();
	virtual ~RenderObject() = 0;

	// A method that contains any API calls necessary to draw the object to screen
	virtual void drawObject() = 0;
	// A method that contains any API calls to send all necessary data to the GPU
	virtual void setupObject() = 0;
	// Returns a label to identify the object in the scene graph
	virtual std::string getLabel() = 0;

	// Returns the object's parent
	RenderObject* getParent();
	// Returns a vector containing the object's children
	std::vector<RenderObject*> getChildren();
	// Sets the parent of the object
	void setParent(RenderObject* parent);
	// Adds a child to the object
	void addChild(RenderObject* child);

private:
	RenderObject* parent;
	std::vector<RenderObject*> children;
};

#endif