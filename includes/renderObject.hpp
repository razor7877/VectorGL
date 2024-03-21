#ifndef RENDEROBJECT_HPP
#define RENDEROBJECT_HPP

#include <vector>

#include <glad/glad.h>

// An interface to be implemented by any object that should be drawn by a renderer
class RenderObject
{
public:
	GLuint shaderProgramID; // The ID of the shader used to draw the object

	RenderObject();
	virtual ~RenderObject() = 0;

	virtual void drawObject() = 0;
	virtual void setupObject() = 0;
	virtual std::string getLabel() = 0;

	RenderObject* getParent();
	std::vector<RenderObject*> getChildren();
	void setParent(RenderObject* parent);
	void addChild(RenderObject* child);

private:
	RenderObject* parent;
	std::vector<RenderObject*> children;
};

#endif