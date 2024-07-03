#pragma once

class Entity;

class Component
{
public:
	Entity* parent{};

	Component(Entity* parent) : parent(parent) {}

	virtual void start() = 0;
	virtual void update() = 0;
};