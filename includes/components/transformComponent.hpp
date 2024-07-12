#pragma once

#include <glm/glm.hpp>

#include "component.hpp"

class TransformComponent : public virtual Component
{
public:
	TransformComponent(Entity* parent);
	~TransformComponent();

	void Component::start() override;
	void Component::update() override;

	glm::mat4 getGlobalModelMatrix();
	glm::mat4 getModelMatrix();
	glm::vec3 getPosition();
	glm::vec3 getRotation();
	glm::vec3 getScale();

	void updateModelMatrix();
	void setModelMatrix(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale);

	// Rotates the object's model matrix using a vec3 (relative transform)
	TransformComponent* rotateObject(glm::vec3 rotation);
	// Rotates the object's model matrix using xyz floats (relative transform)
	TransformComponent* rotateObject(float x, float y, float z);

	// Translate the object's model matrix using a vec3 (relative transform)
	TransformComponent* translateObject(glm::vec3 translation);
	// Translate the object's model matrix using xyz floats (relative transform)
	TransformComponent* translateObject(float x, float y, float z);

	// Scales the object's model matrix using a vec3 (relative transform)
	TransformComponent* scaleObject(glm::vec3 scaleVec);
	// Scales the object's model matrix using xyz floats (relative transform)
	TransformComponent* scaleObject(float scaleX, float scaleY, float scaleZ);

	// Rotates the object's model matrix using a vec3 (absolute transform)
	TransformComponent* setRotation(glm::vec3 rotation);
	// Rotates the object's model matrix using xyz floats (absolute transform)
	TransformComponent* setRotation(float x, float y, float z);

	// Translate the object's model matrix using a vec 3 (absolute transform)
	TransformComponent* setPosition(glm::vec3 translation);
	// Translate the object's model matrix using xyz floats (absolute transform)
	TransformComponent* setPosition(float x, float y, float z);

	// Scales the object's model matrix using a vec3 (absolute transform)
	TransformComponent* setScale(glm::vec3 scaleVec);
	// Scales the object's model matrix using xyz floats (absolute transform)
	TransformComponent* setScale(float x, float y, float z);

private:
	glm::vec3 position;
	glm::vec3 rotation;
	glm::vec3 scale;
	glm::mat4 modelMatrix; // The object's model matrix (position in world)
};