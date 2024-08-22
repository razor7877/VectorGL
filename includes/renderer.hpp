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

	float meshSortingTime = 0.0f;
	float physicsUpdateTime = 0.0f;
	float shadowPassTime = 0.0f;
	float gBufferPassTime = 0.0f;
	float ssaoPassTime = 0.0f;
	float renderPassTime = 0.0f;
	float outlinePassTime = 0.0f;
	float blitPassTime = 0.0f;

	Renderer();

	/// <summary>
	/// Returns a list of raw pointers to the entities in the renderer
	/// </summary>
	/// <returns>A vector containing all the raw entities pointers in the renderer</returns>
	std::vector<Entity*> getEntities();

	/// <summary>
	/// Returns the framebuffer that the renderer draws into
	/// </summary>
	/// <returns>A GLuint pointing to the render texture on the GPU</returns>
	GLuint getRenderTexture();

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
	void resizeFramebuffers(glm::vec2 newSize);

private:
	static constexpr unsigned int SHADOW_MAP_WIDTH = 2048;
	static constexpr unsigned int SHADOW_MAP_HEIGHT = 2048;

	static constexpr float SSAO_SCALE_FACTOR = 0.5;

	/// <summary>
	/// The entities stored in the renderer
	/// </summary>
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

	/// <summary>
	/// The G-buffer for screen space effects
	/// </summary>
	RenderTarget gBuffer;

	/// <summary>
	/// The render target for rendering the SSAO
	/// </summary>
	RenderTarget ssaoTarget;

	/// <summary>
	/// The render target for rendering the blurred SSAO texture
	/// </summary>
	RenderTarget ssaoBlurTarget;

	/// <summary>
	/// The noise texture for SSAO sampling
	/// </summary>
	std::unique_ptr<Texture> ssaoNoiseTexture;

	/// <summary>
	/// The kernel for SSAO sampling
	/// </summary>
	std::vector<glm::vec3> ssaoKernel;
	
	/// <summary>
	/// The noise values for SSAO sampling
	/// </summary>
	std::vector<glm::vec3> ssaoNoise;

	/// <summary>
	/// The quad for the SSAO rendering
	/// </summary>
	std::unique_ptr<Entity> ssaoQuad;

	/// <summary>
	/// The quad for the SSAO blur rendering
	/// </summary>
	std::unique_ptr<Entity> ssaoBlurQuad;

	/// <summary>
	/// Debug lines vertices
	/// </summary>
	std::vector<float> lineVerts;

	/// <summary>
	/// Debug lines vertices
	/// </summary>
	std::vector<float> storedLineVerts;

	// Creates a framebuffer with the size specified
	void createFramebuffers(glm::vec2 windowSize);

	/// <summary>
	/// The pass responsible for generating the shadow map
	/// </summary>
	/// <param name="meshes">A vector containing all meshes to be rendered onto the shadow map</param>
	void shadowPass(std::vector<MeshComponent*>& meshes);

	/// <summary>
	/// The pass responsible for rendering position/normal/albedo informations to the G buffer textures
	/// </summary>
	void gBufferPass(std::vector<MeshComponent*>& meshes);

	/// <summary>
	/// The pass responsible for calculating SSAO
	/// </summary>
	void ssaoPass(std::vector<MeshComponent*>& meshes);

	/// <summary>
	/// The main pass, responsible for rendering all the objects in the scene
	/// </summary>
	/// <param name="deltaTime">The time elapsed since the last frame</param>
	/// <param name="renderables">A vector containing all the entities that are rendered to the screen</param>
	/// <param name="nonRenderables">A vector containing all the entities that are not rendered to the screen</param>
	void renderPass(float deltaTime, std::map<MaterialType, std::vector<Entity*>>& renderables, std::vector<Entity*>& nonRenderables, std::map<MaterialType, std::vector<Entity*>>& transparentRenderables);
	
	/// <summary>
	/// The outline pass, responsible for rendering the outline of selected objects
	/// </summary>
	/// <param name="outlineRenderables"></param>
	void outlinePass(std::vector<Entity*>& outlineRenderables);

	/// <summary>
	/// The final pass, reponsible for resolving the multisampled framebuffer texture to the final texture for display
	/// </summary>
	void blitPass();

	/// <summary>
	/// A recursive function for traversing the scene graph and sorting all the entities before render
	/// </summary>
	void getMeshesRecursively(
		std::vector<Entity*> entities,
		std::map<MaterialType,
		std::vector<Entity*>>& renderables,
		std::vector<Entity*>& outlineRenderables,
		std::vector<MeshComponent*>& meshes,
		std::vector<Entity*>& nonRenderables,
		std::map<MaterialType, std::vector<Entity*>>& transparentRenderables
	);
};

#endif