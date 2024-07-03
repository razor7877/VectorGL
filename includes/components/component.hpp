#pragma once

class Component
{
public:
	Component() {}

	virtual void start() = 0;
	virtual void update() = 0;

protected:

};