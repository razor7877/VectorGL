#pragma once

#include <glm/glm.hpp>

#include "component.hpp"

class TransformComponent : public virtual Component
{
public:
	TransformComponent(Entity* parent);
	~TransformComponent();

	void Component::start() override;
	void Component::update(float deltaTime) override;

	glm::mat4 getGlobalModelMatrix();
	glm::mat4 getModelMatrix();
	glm::mat3 getNormalMatrix();
	glm::vec3 getPosition();
	glm::vec3 getRotation();
	glm::vec3 getScale();

	void updateModelMatrix();
	void setModelMatrix(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale);
	void setModelMatrix(glm::mat4 modelMatrix);

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
	/// <summary>
	/// The position in the world
	/// </summary>
	glm::vec3 position = glm::vec3(0.0f);

	/// <summary>
	/// The rotation in the world (Euler angles in degrees)
	/// </summary>
	glm::vec3 rotation = glm::vec3(0.0f);

	/// <summary>
	/// The scale in the world
	/// </summary>
	glm::vec3 scale = glm::vec3(1.0f);

	/// <summary>
	/// The object's model matrix combining the world space transformations
	/// </summary>
	glm::mat4 modelMatrix = glm::mat4(1.0f);

	/// <summary>
	/// The objett's model matrix when it it specified manually using an external matrix
	/// </summary>
	glm::mat4 manualModelMatrix = glm::mat4(1.0f);

	/// <summary>
	/// The transpose of the inverse of the model matrix, for lighting calculations
	/// </summary>
	glm::mat3 normalMatrix = glm::mat3(1.0f);

	/// <summary>
	/// Whether the model matrix should be calculated from the position/rotation/scale, or is specified directly
	/// </summary>
	bool useRawModelMatrix = false;
};