#include "entity.hpp"

void Entity::update(float deltaTime)
{
	for (auto& [type, component] : this->components)
	{
		component->update();
	}
}

template <typename T>
void Entity::addComponent()
{
	static_assert(std::is_base_of<Component, T>::value, "T must derive from Component");

	if (this->components.count(T))
		return;

	this->components[T] = new(T);
}

template <typename T>
T Entity::getComponent()
{
	static_assert(std::is_base_of<Component, T>::value, "T must derive from Component");

	if (this->components.count(T))
		return this->components[T];

	return nullptr;
}