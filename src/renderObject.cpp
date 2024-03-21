#include <iostream>

#include <glm/ext/matrix_transform.hpp>

#include "renderObject.hpp"

RenderObject::RenderObject()
{
	this->parent = {};
	this->shaderProgramID = {};
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

RenderObject& RenderObject::rotateObject(float degrees, glm::vec3 rotationPoint)
{
	modelMatrix = glm::rotate(modelMatrix, glm::radians(degrees), rotationPoint);
	return *this;
}

RenderObject& RenderObject::rotateObject(float degrees, float x, float y, float z)
{
	modelMatrix = glm::rotate(modelMatrix, glm::radians(degrees), glm::vec3(x, y, z));
	return *this;
}

RenderObject& RenderObject::translateObject(glm::vec3 translation)
{
	modelMatrix = glm::translate(modelMatrix, translation);
	return *this;
}

RenderObject& RenderObject::translateObject(float x, float y, float z)
{
	modelMatrix = glm::translate(modelMatrix, glm::vec3(x, y, z));
	return *this;
}

RenderObject& RenderObject::scaleObject(glm::vec3 scaleVec)
{
	modelMatrix = glm::scale(modelMatrix, scaleVec);
	return *this;
}

RenderObject& RenderObject::scaleObject(float scaleX, float scaleY, float scaleZ)
{
	modelMatrix = glm::scale(modelMatrix, glm::vec3(scaleX, scaleY, scaleZ));
	return *this;
}
