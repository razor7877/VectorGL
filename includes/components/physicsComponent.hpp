#pragma once

#include "components/component.hpp"

class PhysicsComponent : public virtual Component
{
	PhysicsComponent(Entity* parent);
	~PhysicsComponent();

	void Component::start() override;
	void Component::update(float deltaTime) override;
};