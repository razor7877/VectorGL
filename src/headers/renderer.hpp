#ifndef RENDERER_HPP
#define RENDERER_HPP

#include <vector>
#include <map>

#include "mesh.hpp"

class Renderer
{
public:
	// Store a vector of mesh pointers that refers to all the meshes to be drawn
	std::vector<Mesh*> meshes;
	// Associates each shader used in the meshes vector with its corresponding meshes
	std::map<int, std::vector<Mesh*>> shaderMap;

	// Initializes the renderer data, done before the render loop
	void init();
	// Draws the renderer's content
	void render();
	void end();
};

#endif