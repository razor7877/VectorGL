#include "headers/renderer.hpp"

void Renderer::begin()
{

}

void Renderer::render()
{
	for (Mesh mesh : meshes)
	{
		mesh.drawObject();
	}
}

void Renderer::end()
{

}