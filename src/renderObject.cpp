#include <iostream>

#include "renderObject.hpp"

RenderObject::RenderObject()
{
	this->shaderProgramID = {};
}

RenderObject::~RenderObject()
{
	std::cout << "RenderObject derived class did not override destructor" << std::endl;
}

RenderObject* RenderObject::getParent()
{
	return this->parent;
}

std::vector<RenderObject*> RenderObject::getChildren()
{
	return this->children;
}

void RenderObject::setParent(RenderObject* parent)
{
	this->parent = parent;
}

void RenderObject::addChild(RenderObject* child)
{
	this->children.push_back(child);
}