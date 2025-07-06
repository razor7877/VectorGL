#pragma once

class Entity;

class Component
{
public:
	Entity* parent{};

	Component() = default;
	explicit Component(Entity* parent) : parent(parent) {}
	virtual ~Component() = default;

	virtual void start() = 0;
	virtual void update(float deltaTime) = 0;
};