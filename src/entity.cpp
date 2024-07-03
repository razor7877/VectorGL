#include <iostream>

#include "entity.hpp"

Entity::Entity()
{

}

Entity::~Entity()
{
	for (auto& [type, component] : this->components)
	{
		delete component;
	}
}

void Entity::update(float deltaTime)
{
	for (auto& [type, component] : this->components)
	{
		component->update();
	}
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
	std::cout << "Parent " << this->label << " now has child " << child->getLabel() << std::endl;
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