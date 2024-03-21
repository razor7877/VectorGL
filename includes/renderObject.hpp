#ifndef RENDEROBJECT_HPP
#define RENDEROBJECT_HPP

#include <vector>

#include <glm/glm.hpp>
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

	bool getIsVisible();
	void setIsVisible(bool isVisible);

	glm::mat4 getModelMatrix();
	glm::vec3 getPosition();
	glm::vec3 getRotation();
	glm::vec3 getScale();

	// Rotates the object's model matrix using a vec3 (relative transform)
	RenderObject* rotateObject(float degrees, glm::vec3 rotationPoint);
	// Rotates the object's model matrix using xyz floats (relative transform)
	RenderObject* rotateObject(float degrees, float x, float y, float z);

	// Translate the object's model matrix using a vec3 (relative transform)
	RenderObject* translateObject(glm::vec3 translation);
	// Translate the object's model matrix using xyz floats (relative transform)
	RenderObject* translateObject(float x, float y, float z);

	// Scales the object's model matrix using a vec3 (relative transform)
	RenderObject* scaleObject(glm::vec3 scaleVec);
	// Scales the object's model matrix using xyz floats (relative transform)
	RenderObject* scaleObject(float scaleX, float scaleY, float scaleZ);

	// Rotates the object's model matrix using a vec3 (absolute transform)
	RenderObject* setRotation(float degrees, glm::vec3 rotationPoint);
	// Rotates the object's model matrix using xyz floats (absolute transform)
	RenderObject* setRotation(float degrees, float x, float y, float z);

	// Translate the object's model matrix using a vec 3 (absolute transform)
	RenderObject* setPosition(glm::vec3 translation);
	// Translate the object's model matrix using xyz floats (absolute transform)
	RenderObject* setPosition(float x, float y, float z);

	// Scales the object's model matrix using a vec3 (absolute transform)
	RenderObject* setScale(glm::vec3 scaleVec);
	// Scales the object's model matrix using xyz floats (absolute transform)
	RenderObject* setScale(float x, float y, float z);

protected:
	RenderObject* parent;
	std::vector<RenderObject*> children;

	glm::mat4 modelMatrix; // The object's model matrix (position in world)
	bool isVisible; // Whether the object should be drawn or not
};

#endif