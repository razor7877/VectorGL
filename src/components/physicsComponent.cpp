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
	if (this->collider != nullptr && this->collider->world != nullptr)
		this->collider->world->removeRigidBody(this->collider->rigidBody.get());
}

void PhysicsComponent::start()
{

}

void PhysicsComponent::update(float deltaTime)
{
	if (this->collider == nullptr || this->collider->rigidBody == nullptr)
		return;

	// Extract basis (rotation) and origin (translation) from btTransform
	btTransform trans;
	this->collider->rigidBody->getMotionState()->getWorldTransform(trans);
	btMatrix3x3& basis = trans.getBasis();
	btVector3& origin = trans.getOrigin();

	glm::mat4 glmMat = this->parent->getTransform()->getModelMatrix();

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

	glmMat = glm::scale(glmMat, this->parent->getTransform()->getScale());

	this->parent->getTransform()->setModelMatrix(glmMat);
}

void PhysicsComponent::setCollider(std::unique_ptr<Collider> collider)
{
	this->collider = std::move(collider);
}