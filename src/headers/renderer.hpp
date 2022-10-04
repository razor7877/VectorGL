#ifndef RENDERER_HPP
#define RENDERER_HPP

#include <vector>
#include <map>

#include "renderObject.hpp"
#include "lights/lightManager.hpp"
#include "lights/light.hpp"

// A renderer class, that does all that is needed for setting up and then drawing many things such as models, meshes, skyboxes etc.
// It contains a vector of renderObject, that stores the various objects to be drawn
class Renderer
{
public:
	// Store a vector of mesh pointers that refers to all the meshes to be drawn
	std::vector<RenderObject*> objects;
	// Associates each shader used in the meshes vector with its corresponding meshes
	std::map<int, std::vector<RenderObject*>> shaderMap;
	// The renderer's light manager, used to handle lighting and various light types
	LightManager lightManager;

	Renderer(GLuint lightShaderProgramID);

	Renderer addObject(RenderObject* objectPtr);
	Renderer addLight(Light* lightPtr);

	// Initializes the renderer data, done before the render loop
	void init();
	// Draws the renderer's content
	void render();
	void end();
};

#endif