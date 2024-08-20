#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>

#include "logger.hpp"
#include "entity.hpp"
#include "components/physicsComponent.hpp"

PhysicsComponent::PhysicsComponent(Entity* parent) : Component(parent)
{

}

PhysicsComponent::~PhysicsComponent()
{

}

void PhysicsComponent::start()
{

}

void PhysicsComponent::update(float deltaTime)
{
	if (this->rigidBody == nullptr)
		return;

	// Extract basis (rotation) and origin (translation) from btTransform
	btTransform trans;
	this->rigidBody->getMotionState()->getWorldTransform(trans);
	btMatrix3x3& basis = trans.getBasis();
	btVector3& origin = trans.getOrigin();

	glm::mat4 glmMat = this->parent->transform->getModelMatrix();

	// Set rotation part
	glmMat[0][0] = basis[0][0];
	glmMat[1][0] = basis[0][1];
	glmMat[2][0] = basis[0][2];
	glmMat[0][1] = basis[1][0];
	glmMat[1][1] = basis[1][1];
	glmMat[2][1] = basis[1][2];
	glmMat[0][2] = basis[2][0];
	glmMat[1][2] = basis[2][1];
	glmMat[2][2] = basis[2][2];

	// Set translation part
	glmMat[3][0] = origin.getX();
	glmMat[3][1] = origin.getY();
	glmMat[3][2] = origin.getZ();

	glmMat = glm::scale(glmMat, this->parent->transform->getScale());

	this->parent->transform->setModelMatrix(glmMat);
}

void PhysicsComponent::setCollider(btRigidBody* collider)
{
	this->rigidBody = collider;
}