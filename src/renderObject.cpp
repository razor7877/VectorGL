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