#ifndef RENDERER_HPP
#define RENDERER_HPP

#include <vector>
#include <map>

#include "entity.hpp"
#include "renderObject.hpp"
#include "lights/lightManager.hpp"
#include "lights/light.hpp"

// A renderer class, that does all that is needed for setting up and then drawing many things such as models, meshes, skyboxes etc.
// It contains a vector of renderObject, that stores the various objects to be drawn
class Renderer
{
public:
	// The renderer's light manager, used to handle lighting and various light types
	LightManager lightManager;

	Renderer();
	Renderer(GLuint lightShaderProgramID);

	// Returns the list of objects that the renderer contains
	std::vector<RenderObject*> GetObjects();
	std::vector<Entity*> GetEntities();
	// Returns the framebuffer that renderer draws into
	GLuint GetRenderTexture();

	// Adds an object (model, mesh, skybox...) to the renderer
	Renderer& addObject(RenderObject* objectPtr);
	// Removes an object from the renderer which stops it being drawn
	Renderer& removeObject(RenderObject* objectPtr);
	// Adds a light (dirlight, pointlight, spotlight ...) to the renderer's LightManager
	Renderer& addLight(Light* lightPtr);

	// Adds an object (model, mesh, skybox...) to the renderer
	void addEntity(Entity* objectPtr);
	// Removes an object from the renderer which stops it being drawn
	void removeEntity(Entity* objectPtr);

	// Initializes the renderer data, done before the render loop
	void init(glm::vec2 windowSize);
	// Draws the renderer's content
	void render(float deltaTime);
	void end();

	// Resizes the existing framebuffer with the size specified
	void resizeFramebuffer(glm::vec2 newSize);

private:
	std::vector<RenderObject*> objects;
	std::vector<Entity*> entities;

	// Associates each shader used in the meshes vector with its corresponding meshes
	std::map<int, std::vector<RenderObject*>> shaderMap;

	GLuint frameBuffer;
	GLuint depthBuffer;
	GLuint renderTexture;
	GLenum drawBuffers[1];
	glm::vec2 windowSize;

	// Creates a framebuffer with the size specified
	void createFramebuffer(glm::vec2 windowSize);
};

#endif