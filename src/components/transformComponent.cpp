#include <glm/ext/matrix_transform.hpp>

#include "components/transformComponent.hpp"
#include "entity.hpp"

TransformComponent::TransformComponent(Entity* parent) : Component(parent)
{
	this->position = glm::vec3(0);
	this->rotation = glm::vec3(0);
	this->scale = glm::vec3(1.0f);
	this->modelMatrix = glm::mat4(1.0f);
}

TransformComponent::~TransformComponent()
{
	//if (this->parent != nullptr)
	//{
	//	this->parent->removeChild(this);
	//}

	//// We need to copy the list of children, since deleting the children will modify the original vector
	//std::vector<RenderObject*> children = this->children;
	//for (RenderObject* child : children)
	//{
	//	delete child;
	//}
}

void TransformComponent::start() { }

void TransformComponent::update() { }

glm::mat4 TransformComponent::getModelMatrix()
{
	return this->modelMatrix;
}

glm::vec3 TransformComponent::getPosition()
{
	return this->position;
}

glm::vec3 TransformComponent::getRotation()
{
	return this->rotation;
}

glm::vec3 TransformComponent::getScale()
{
	return this->scale;
}

void TransformComponent::updateModelMatrix()
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

	// A node should inherit the transform of the parent entity
	// The parent is the entity that contains this component, we want the entity above
	if (this->parent->getParent() != nullptr)
		this->modelMatrix = this->parent->getParent()->transform->getModelMatrix() * this->modelMatrix;

	// Since children inherit of the parent transform, they need to be updated too
	for (Entity* child : this->parent->getChildren())
		child->transform->updateModelMatrix();
}

void TransformComponent::setModelMatrix(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale)
{
	this->position = position;
	this->rotation = rotation;
	this->scale = scale;

	this->updateModelMatrix();
}

TransformComponent* TransformComponent::rotateObject(glm::vec3 rotation)
{
	this->rotation = rotation;
	this->updateModelMatrix();
	return this;
}

TransformComponent* TransformComponent::rotateObject(float x, float y, float z)
{
	this->rotation = glm::vec3(x, y, z);
	this->updateModelMatrix();
	return this;
}

TransformComponent* TransformComponent::translateObject(glm::vec3 translation)
{
	this->position += translation;
	this->updateModelMatrix();
	return this;
}

TransformComponent* TransformComponent::translateObject(float x, float y, float z)
{
	this->position += glm::vec3(x, y, z);
	this->updateModelMatrix();
	return this;
}

TransformComponent* TransformComponent::scaleObject(glm::vec3 scaleVec)
{
	this->scale += scaleVec;
	this->updateModelMatrix();
	return this;
}

TransformComponent* TransformComponent::scaleObject(float scaleX, float scaleY, float scaleZ)
{
	this->scale += glm::vec3(scaleX, scaleY, scaleZ);
	this->updateModelMatrix();
	return this;
}

TransformComponent* TransformComponent::setRotation(glm::vec3 rotation)
{
	this->rotation = rotation;
	this->updateModelMatrix();
	return this;
}

TransformComponent* TransformComponent::setRotation(float x, float y, float z)
{
	this->rotation = glm::vec3(x, y, z);
	this->updateModelMatrix();
	return this;
}

TransformComponent* TransformComponent::setPosition(glm::vec3 translation)
{
	this->position = translation;
	this->updateModelMatrix();
	return this;
}

TransformComponent* TransformComponent::setPosition(float x, float y, float z)
{
	this->position = glm::vec3(x, y, z);
	this->updateModelMatrix();
	return this;
}

TransformComponent* TransformComponent::setScale(glm::vec3 scaleVec)
{
	this->scale = scaleVec;
	this->updateModelMatrix();
	return this;
}

TransformComponent* TransformComponent::setScale(float x, float y, float z)
{
	this->scale = glm::vec3(x, y, z);
	this->updateModelMatrix();
	return this;
}
