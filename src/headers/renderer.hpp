#ifndef RENDERER_HPP
#define RENDERER_HPP

#include <vector>

#include "mesh.hpp"

class Renderer
{
public:
	std::vector<Mesh> meshes;

	void begin();
	void render();
	void end();
};

#endif