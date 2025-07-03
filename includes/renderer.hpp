#ifndef RENDERER_HPP
#define RENDERER_HPP

#include <vector>
#include <memory>

#include "entity.hpp"
#include "shaderManager.hpp"
#include "renderTarget.hpp"
#include "components/meshComponent.hpp"
#include "physics/physicsWorld.hpp"
#include "scene.hpp"

// TODO : Render pass system
/// <summary>
/// The renderer is responsible for storing and managing the scene data and setting up its own framebuffer
/// </summary>
class Renderer
{
public:
	static constexpr int SHADOW_CASCADE_LEVELS = 3;
	static constexpr float SHADOW_CASCADE_DISTANCES[SHADOW_CASCADE_LEVELS] = {
		0.05f,
		0.15f,
		0.35f
	};

	ShaderManager shaderManager;

	float frameRenderTime = 0.0f;
	float meshSortingTime = 0.0f;
	float physicsUpdateTime = 0.0f;
	float shadowPassTime = 0.0f;
	float gBufferPassTime = 0.0f;
	float ssaoPassTime = 0.0f;
	float renderPassTime = 0.0f;
	float outlinePassTime = 0.0f;
	float blitPassTime = 0.0f;
	float debugPassTime = 0.0f;

	bool enableDebugDraw = false;

	Renderer();
	~Renderer();

	/// <summary>
	/// Returns the framebuffer that the renderer draws into
	/// </summary>
	/// <returns>A GLuint pointing to the render texture on the GPU</returns>
	GLuint getRenderTexture() const;

	GLuint getSkyRenderTexture() const;

	/// <summary>
	/// Initializes the renderer data, this needs to be done once before the render loop
	/// </summary>
	/// <param name="lastWindowSize">The window size in pixels</param>
	void init(glm::vec2 lastWindowSize);

	/// <summary>
	/// Adds a line to draw for debugging raycasts etc.
	/// </summary>
	/// <param name="startPos">The start position of the line</param>
	/// <param name="endPos">The end position of the line</param>
	/// <param name="store">Whether the line data should persist between frames</param>
	void addLine(glm::vec3 startPos, glm::vec3 endPos, bool store);

	/// <summary>
	/// Draws the scene and updates all the entities
	/// </summary>
	/// <param name="scene">The scene to be rendered</param>
	/// <param name="physicsWorld">The physics world associated with the scene</param>
	/// <param name="deltaTime">The time elapsed since the last frame</param>
	void render(Scene& scene, PhysicsWorld& physicsWorld, float deltaTime);

	/// <summary>
	/// Stops the renderer, this cleans up all the resources it contains (not implemented yet)
	/// </summary>
	void end();

	/// <summary>
	/// Resizes the existing framebuffer with the size specified
	/// </summary>
	/// <param name="newSize">The new size in pixels</param>
	void resizeFramebuffers(glm::vec2 newSize) const;

private:
	/// <summary>
	/// The width in pixels of the shadow map
	/// </summary>
	static constexpr unsigned int SHADOW_MAP_WIDTH = 2048;

	/// <summary>
	/// The height in pixels of the shadow map
	/// </summary>
	static constexpr unsigned int SHADOW_MAP_HEIGHT = 2048;

	/// <summary>
	/// The scaling factor to scale the resolution of the SSAO map relative to the window base resolution
	/// </summary>
	static constexpr float SSAO_SCALE_FACTOR = 0.75;

	/// <summary>
	/// The render target in which everything is rendered
	/// </summary>
	std::unique_ptr<RenderTarget> multiSampledTarget;

	/// <summary>
	/// The final render target, the multisampled target is resolved into this one that can be used for displaying to screen
	/// </summary>
	std::unique_ptr<RenderTarget> finalTarget;

	/// <summary>
	/// The render target for the top-down view
	/// </summary>
	std::unique_ptr<RenderTarget> skyTarget;

	/// <summary>
	/// The depth map for shadow mapping
	/// </summary>
	std::unique_ptr<RenderTarget> depthMap;

	/// <summary>
	/// The G-buffer for screen space effects
	/// </summary>
	std::unique_ptr<RenderTarget> gBuffer;

	/// <summary>
	/// The render target for rendering the SSAO
	/// </summary>
	std::unique_ptr<RenderTarget> ssaoTarget;

	/// <summary>
	/// The render target for rendering the blurred SSAO texture
	/// </summary>
	std::unique_ptr<RenderTarget> ssaoBlurTarget;

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
	void createFramebuffers(glm::vec2 lastWindowSize);

	/// <summary>
	/// The pass responsible for generating the shadow map
	/// </summary>
	/// <param name="meshes">A vector containing all meshes to be rendered onto the shadow map</param>
	/// <param name="scene">The scene to use for generating the shadows</param>
	void shadowPass(const std::vector<MeshComponent*>& meshes, const Scene& scene);

	glm::mat4 getLightSpaceMatrix(const Scene& scene, float nearPlane, float farPlane) const;

	/// <summary>
	/// The pass responsible for rendering position/normal/albedo information to the G buffer textures
	/// </summary>
	void gBufferPass(const std::vector<MeshComponent*>& meshes);

	/// <summary>
	/// The pass responsible for calculating SSAO
	/// </summary>
	void ssaoPass(std::vector<MeshComponent*>& meshes);

	/// <summary>
	/// The main pass, responsible for rendering all the objects in the scene
	/// </summary>
	/// <param name="deltaTime">The time elapsed since the last frame</param>
	/// <param name="physicsWorld">A reference to the physics world</param>
	/// <param name="sceneData">The prepared scene data for rendering</param>
	void renderPass(float deltaTime, PhysicsWorld& physicsWorld, SortedSceneData& sceneData);
	
	/// <summary>
	/// The outline pass, responsible for rendering the outline of selected objects
	/// </summary>
	/// <param name="outlineRenderables"></param>
	void outlinePass(const std::vector<Entity*>& outlineRenderables);

	/// <summary>
	/// The final pass, reponsible for resolving the multisampled framebuffer texture to the final texture for display
	/// </summary>
	void blitPass() const;
};

#endif