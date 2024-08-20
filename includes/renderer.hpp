#ifndef RENDERER_HPP
#define RENDERER_HPP

#include <vector>
#include <map>
#include <memory>

#include "entity.hpp"
#include "lights/lightManager.hpp"
#include "shaderManager.hpp"
#include "renderTarget.hpp"
#include "components/cameraComponent.hpp"
#include "components/meshComponent.hpp"
#include "components/lights/directionalLightComponent.hpp"
#include "physics/physicsWorld.hpp"

extern std::vector<float> lineVerts;

/// <summary>
/// The renderer is responsible for storing and managing the scene data and setting up it's own framebuffer
/// </summary>
class Renderer
{
public:
	ShaderManager shaderManager;
	CameraComponent* currentCamera;
	DirectionalLightComponent* directionalLight;
	PhysicsWorld* physicsWorld = new PhysicsWorld();

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
	/// <returns>True if the entity was successfully removed, false otherwise</return>
	bool removeEntity(std::unique_ptr<Entity> objectPtr);

	/// <summary>
	/// Removes an entity from the renderer using a raw pointer
	/// </summary>
	/// <param name="rawObjectPtr">The raw pointer to the object</param>
	/// <returns>True if the entity was successfully removed, false otherwise</returns>
	bool removeEntity(Entity* rawObjectPtr);

	/// <summary>
	/// Initializes the renderer data, this needs to be done once before the render loop
	/// </summary>
	/// <param name="windowSize">The window size in pixels</param>
	void init(glm::vec2 windowSize);

	void addLine(glm::vec3 startPos, glm::vec3 endPos, bool store);

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
	static constexpr unsigned int SHADOW_MAP_WIDTH = 2048;
	static constexpr unsigned int SHADOW_MAP_HEIGHT = 2048;

	std::vector<std::unique_ptr<Entity>> entities;

	/// <summary>
	/// The render target in which everything is rendered
	/// </summary>
	RenderTarget multiSampledTarget;

	/// <summary>
	/// The final render target, the multisampled target is resolved into this one that can be used for displaying to screen
	/// </summary>
	RenderTarget finalTarget;

	/// <summary>
	/// The depth map for shadow mapping
	/// </summary>
	RenderTarget depthMap;

	// Creates a framebuffer with the size specified
	void createFramebuffer(glm::vec2 windowSize);

	/// <summary>
	/// The pass responsible for generating the shadow map
	/// </summary>
	/// <param name="meshes">A vector containing all meshes to be rendered onto the shadow map</param>
	void shadowPass(std::vector<MeshComponent*>& meshes);

	/// <summary>
	/// The main pass, responsible for rendering all the objects in the scene
	/// </summary>
	/// <param name="deltaTime">The time elapsed since the last frame</param>
	/// <param name="renderables">A vector containing all the entities that are rendered to the screen</param>
	/// <param name="nonRenderables">A vector containing all the entities that are not rendered to the screen</param>
	void renderPass(float deltaTime, std::map<MaterialType, std::vector<Entity*>>& renderables, std::vector<Entity*>& nonRenderables);
	
	/// <summary>
	/// The outline pass, responsible for rendering the outline of selected objects
	/// </summary>
	/// <param name="outlineRenderables"></param>
	void outlinePass(std::vector<Entity*>& outlineRenderables);

	/// <summary>
	/// The final pass, reponsible for resolving the multisampled framebuffer texture to the final texture for display
	/// </summary>
	void blitPass();

	void getMeshesRecursively(std::vector<Entity*> entities, std::map<MaterialType, std::vector<Entity*>>& renderables, std::vector<Entity*>& outlineRenderables, std::vector<MeshComponent*>& meshes, std::vector<Entity*>& nonRenderables);
};

#endif