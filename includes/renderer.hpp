#ifndef RENDERER_HPP
#define RENDERER_HPP

#include <vector>
#include <map>
#include <memory>

#include "entity.hpp"
#include "lights/lightManager.hpp"
#include "shaderManager.hpp"
#include "renderTarget.hpp"

/// <summary>
/// The renderer is responsible for storing and managing the scene data and setting up it's own framebuffer
/// </summary>
class Renderer
{
public:
	ShaderManager shaderManager;
	CameraComponent* currentCamera;

	Renderer();

	/// <summary>
	/// Returns a list of raw pointers to the entities in the renderer
	/// </summary>
	/// <returns>A vector containing all the raw entities pointers in the renderer</returns>
	std::vector<Entity*> GetEntities();

	/// <summary>
	/// Returns the framebuffer that the renderer draws into
	/// </summary>
	/// <returns>A GLuint pointing to the render texture on the GPU</returns>
	GLuint GetRenderTexture();

	/// <summary>
	/// Adds an entity to the renderer
	/// </summary>
	/// <param name="objectPtr">The entity to be added</param>
	void addEntity(std::unique_ptr<Entity> objectPtr);

	/// <summary>
	/// Removes an entity from the renderer
	/// </summary>
	/// <param name="objectPtr">The entity to be removed</param>
	void removeEntity(std::unique_ptr<Entity> objectPtr);

	void removeEntity(Entity* rawObjectPtr);

	/// <summary>
	/// Initializes the renderer data, this needs to be done once before the render loop
	/// </summary>
	/// <param name="windowSize">The window size in pixels</param>
	void init(glm::vec2 windowSize);

	/// <summary>
	/// Draws the scene and updates all the entities
	/// </summary>
	/// <param name="deltaTime">The time elapsed since the last frame</param>
	void render(float deltaTime);

	/// <summary>
	/// Stops the renderer, this cleans up all the resources it contains (not implemented yet)
	/// </summary>
	void end();

	/// <summary>
	/// Resizes the existing framebuffer with the size specified
	/// </summary>
	/// <param name="newSize">The new size in pixels</param>
	void resizeFramebuffer(glm::vec2 newSize);

private:
	std::vector<std::unique_ptr<Entity>> entities;

	RenderTarget multiSampledTarget;
	RenderTarget finalTarget;

	// Creates a framebuffer with the size specified
	void createFramebuffer(glm::vec2 windowSize);
};

#endif