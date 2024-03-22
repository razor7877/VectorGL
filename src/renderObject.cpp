#include <iostream>

#include <glm/ext/matrix_transform.hpp>

#include "renderObject.hpp"

RenderObject::RenderObject()
{
	this->shaderProgramID = {};

	this->parent = {};

	this->position = {};
	this->rotation = {};
	this->scale = glm::vec3(1.0f);
	this->modelMatrix = glm::mat4(1.0f);

	this->isVisible = true;
}

RenderObject::~RenderObject()
{
	if (this->parent != nullptr)
	{
		this->parent->removeChild(this);
	}
	
	// We need to copy the list of children, since deleting the children will modify the original vector
	std::vector<RenderObject*> children = this->children;
	for (RenderObject* child : children)
	{
		delete child;
	}
}

std::string RenderObject::getLabel()
{
	return this->label;
}

void RenderObject::setLabel(std::string label)
{
	this->label = label;
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
	std::cout << "Parent " << this->label << " now has child " << child->getLabel() << std::endl;
}

void RenderObject::removeChild(RenderObject* child)
{
	this->children.erase(std::remove(this->children.begin(), this->children.end(), child), this->children.end());
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
	return this->position;
}

glm::vec3 RenderObject::getRotation()
{
	return this->rotation;
}

glm::vec3 RenderObject::getScale()
{
	return this->scale;
}

void RenderObject::updateModelMatrix()
{
	this->modelMatrix = glm::mat4(1.0f);
	this->modelMatrix = glm::translate(this->modelMatrix, this->position); // Apply position

	const glm::mat4 transformX = glm::rotate(glm::mat4(1.0f),
		glm::radians(this->rotation.x),
		glm::vec3(1.0f, 0.0f, 0.0f));
	const glm::mat4 transformY = glm::rotate(glm::mat4(1.0f),
		glm::radians(this->rotation.y),
		glm::vec3(0.0f, 1.0f, 0.0f));
	const glm::mat4 transformZ = glm::rotate(glm::mat4(1.0f),
		glm::radians(this->rotation.z),
		glm::vec3(0.0f, 0.0f, 1.0f));

	// Y * X * Z
	const glm::mat4 rotationMatrix = transformY * transformX * transformZ;
	this->modelMatrix *= rotationMatrix; // Apply rotation

	this->modelMatrix = glm::scale(this->modelMatrix, this->scale); // Apply scaling

	// A node should inherit the transform of the parent
	if (this->parent != nullptr)
		this->modelMatrix = this->parent->getModelMatrix() * this->modelMatrix;

	// Since children inherit of the parent transform, they need to be updated too
	for (RenderObject* child : this->children)
		child->updateModelMatrix();
}

RenderObject* RenderObject::rotateObject(glm::vec3 rotation)
{
	this->rotation = rotation;
	this->updateModelMatrix();
	return this;
}

RenderObject* RenderObject::rotateObject(float x, float y, float z)
{
	this->rotation = glm::vec3(x, y, z);
	this->updateModelMatrix();
	return this;
}

RenderObject* RenderObject::translateObject(glm::vec3 translation)
{
	this->position += translation;
	this->updateModelMatrix();
	return this;
}

RenderObject* RenderObject::translateObject(float x, float y, float z)
{
	this->position += glm::vec3(x, y, z);
	this->updateModelMatrix();
	return this;
}

RenderObject* RenderObject::scaleObject(glm::vec3 scaleVec)
{
	this->scale += scaleVec;
	this->updateModelMatrix();
	return this;
}

RenderObject* RenderObject::scaleObject(float scaleX, float scaleY, float scaleZ)
{
	this->scale += glm::vec3(scaleX, scaleY, scaleZ);
	this->updateModelMatrix();
	return this;
}

RenderObject* RenderObject::setRotation(glm::vec3 rotation)
{
	this->rotation = rotation;
	this->updateModelMatrix();
	return this;
}

RenderObject* RenderObject::setRotation(float x, float y, float z)
{
	this->rotation = glm::vec3(x, y, z);
	this->updateModelMatrix();
	return this;
}

RenderObject* RenderObject::setPosition(glm::vec3 translation)
{
	this->position = translation;
	this->updateModelMatrix();
	return this;
}

RenderObject* RenderObject::setPosition(float x, float y, float z)
{
	this->position = glm::vec3(x, y, z);
	this->updateModelMatrix();
	return this;
}

RenderObject* RenderObject::setScale(glm::vec3 scaleVec)
{
	this->scale = scaleVec;
	this->updateModelMatrix();
	return this;
}

RenderObject* RenderObject::setScale(float x, float y, float z)
{
	this->scale = glm::vec3(x, y, z);
	this->updateModelMatrix();
	return this;
}
