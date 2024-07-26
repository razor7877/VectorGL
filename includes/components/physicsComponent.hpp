#pragma once

#include <btBulletDynamicsCommon.h>

#include "components/component.hpp"

class PhysicsComponent : public virtual Component
{
public:
	PhysicsComponent(Entity* parent);
	~PhysicsComponent();

	void Component::start() override;
	void Component::update(float deltaTime) override;

	void setCollider(btRigidBody* rigidBody);

private:
	btRigidBody* rigidBody;
};