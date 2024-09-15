#include <iostream>

#include "entity.hpp"
#include "components/physicsComponent.hpp"

Entity::Entity()
{
	this->transform = this->addComponent<TransformComponent>();
}

Entity::Entity(std::string label)
{
	this->transform = this->addComponent<TransformComponent>();
	this->label = label;
}

Entity::~Entity()
{
	for (auto& [type, component] : this->components)
		delete component;

	// Delete all the children as well
	for (Entity* child : this->children)
		delete child;
}

void Entity::start()
{
	for (auto& [type, component] : this->components)
		component->start();

	for (Entity* child : this->children)
		child->start();
}

void Entity::update(float deltaTime)
{
	if (!this->isEnabled)
		return;

	std::type_index physics = std::type_index(typeid(PhysicsComponent));
	if (this->components.count(physics))
		this->components[physics]->update(deltaTime);

	for (auto& [type, component] : this->components)
		component->update(deltaTime);
}

std::map<std::type_index, Component*> Entity::getComponents()
{
	return this->components;
}

TransformComponent* Entity::getTransform()
{
	return this->transform;
}

std::string Entity::getLabel()
{
	return this->label;
}

void Entity::setLabel(std::string label)
{
	this->label = label;
}

Entity* Entity::getParent()
{
	return this->parent;
}

std::vector<Entity*> Entity::getChildren()
{
	return this->children;
}

void Entity::setParent(Entity* parent)
{
	this->parent = parent;
}

void Entity::addChild(Entity* child)
{
	this->children.push_back(child);
}

void Entity::removeChild(Entity* child)
{
	this->children.erase(std::remove(this->children.begin(), this->children.end(), child), this->children.end());
}

bool Entity::getIsEnabled()
{
	return this->isEnabled;
}

void Entity::setIsEnabled(bool isEnabled)
{
	this->isEnabled = isEnabled;
}