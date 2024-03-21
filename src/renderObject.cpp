#include <iostream>

#include <glm/ext/matrix_transform.hpp>

#include "renderObject.hpp"

RenderObject::RenderObject()
{
	this->parent = {};
	this->shaderProgramID = {};
	this->modelMatrix = {};
	this->isVisible = true;
}

RenderObject::~RenderObject()
{
	std::cout << "RenderObject derived class did not override destructor" << std::endl;
}

RenderObject* RenderObject::getParent()
{
	return this->parent;
}

std::vector<RenderObject*> RenderObject::getChildren()
{
	return this->children;
}

void RenderObject::setParent(RenderObject* parent)
{
	this->parent = parent;
}

void RenderObject::addChild(RenderObject* child)
{
	this->children.push_back(child);
}

bool RenderObject::getIsVisible()
{
	return this->isVisible;
}

void RenderObject::setIsVisible(bool isVisible)
{
	this->isVisible = isVisible;
}

glm::mat4 RenderObject::getModelMatrix()
{
	return this->modelMatrix;
}

glm::vec3 RenderObject::getPosition()
{
	return glm::vec3(
		this->modelMatrix[3][0],
		this->modelMatrix[3][1],
		this->modelMatrix[3][2]
	);
}

// TODO : Implement
glm::vec3 RenderObject::getRotation()
{
	return glm::vec3(1.0f);
}

// TODO : Implement
glm::vec3 RenderObject::getScale()
{
	return glm::vec3(
		this->modelMatrix[0][0],
		this->modelMatrix[1][0],
		this->modelMatrix[2][0]
	);
}

RenderObject* RenderObject::rotateObject(float degrees, glm::vec3 rotationPoint)
{
	this->modelMatrix = glm::rotate(this->modelMatrix, glm::radians(degrees), rotationPoint);
	return this;
}

RenderObject* RenderObject::rotateObject(float degrees, float x, float y, float z)
{
	this->modelMatrix = glm::rotate(this->modelMatrix, glm::radians(degrees), glm::vec3(x, y, z));
	return this;
}

RenderObject* RenderObject::translateObject(glm::vec3 translation)
{
	this->modelMatrix = glm::translate(this->modelMatrix, translation);
	return this;
}

RenderObject* RenderObject::translateObject(float x, float y, float z)
{
	this->modelMatrix = glm::translate(this->modelMatrix, glm::vec3(x, y, z));
	return this;
}

RenderObject* RenderObject::scaleObject(glm::vec3 scaleVec)
{
	this->modelMatrix = glm::scale(this->modelMatrix, scaleVec);
	return this;
}

RenderObject* RenderObject::scaleObject(float scaleX, float scaleY, float scaleZ)
{
	this->modelMatrix = glm::scale(this->modelMatrix, glm::vec3(scaleX, scaleY, scaleZ));
	return this;
}

RenderObject* RenderObject::setRotation(float degrees, glm::vec3 rotationPoint)
{
	this->modelMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(degrees), rotationPoint);
	return this;
}

RenderObject* RenderObject::setRotation(float degrees, float x, float y, float z)
{
	this->modelMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(degrees), glm::vec3(x, y, z));
	return this;
}

RenderObject* RenderObject::setPosition(glm::vec3 translation)
{
	this->modelMatrix[3][0] = translation.x;
	this->modelMatrix[3][1] = translation.y;
	this->modelMatrix[3][2] = translation.z;
	return this;
}

RenderObject* RenderObject::setPosition(float x, float y, float z)
{
	this->modelMatrix[3][0] = x;
	this->modelMatrix[3][1] = y;
	this->modelMatrix[3][2] = z;
	return this;
}

RenderObject* RenderObject::setScale(glm::vec3 scaleVec)
{
	this->modelMatrix[0][0] = scaleVec.x;
	this->modelMatrix[1][0] = scaleVec.y;
	this->modelMatrix[2][0] = scaleVec.z;
	return this;
}

RenderObject* RenderObject::setScale(float x, float y, float z)
{
	this->modelMatrix[0][0] = x;
	this->modelMatrix[1][0] = y;
	this->modelMatrix[2][0] = z;
	return this;
}
