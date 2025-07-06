#ifndef RENDERER_HPP
#define RENDERER_HPP

#include <memory>

#include "ShaderManager.hpp"
#include "RenderTarget.hpp"
#include "physics/PhysicsWorld.hpp"
#include "Scene.hpp"
#include "render/BlitPass.hpp"
#include "render/DebugRenderPass.hpp"
#include "render/GBufferPass.hpp"
#include "render/MainRenderPass.hpp"
#include "render/OutlinePass.hpp"
#include "render/StaticShadowPass.hpp"
#include "render/SSAOPass.hpp"

/// <summary>
/// The renderer is responsible for storing and managing the scene data and setting up its own framebuffer
/// </summary>
class Renderer
{
public:
	ShaderManager shaderManager;

	double frameRenderTime = 0.0f;
	double meshSortingTime = 0.0f;
	double physicsUpdateTime = 0.0f;
	double shadowPassTime = 0.0f;
	double gBufferPassTime = 0.0f;
	double ssaoPassTime = 0.0f;
	double renderPassTime = 0.0f;
	double outlinePassTime = 0.0f;
	double blitPassTime = 0.0f;
	double debugPassTime = 0.0f;

	bool enableDebugDraw = true;

	Renderer();
	~Renderer();

	/// <summary>
	/// Returns the framebuffer that the renderer draws into
	/// </summary>
	/// <returns>A GLuint pointing to the render texture on the GPU</returns>
	[[nodiscard]] GLuint getRenderTexture() const;

	[[nodiscard]] GLuint getSkyRenderTexture() const;

	/// <summary>
	/// Initializes the renderer data, this needs to be done once before the render loop
	/// </summary>
	/// <param name="lastWindowSize">The window size in pixels</param>
	void init(glm::vec2 lastWindowSize);

	/// <summary>
	/// Draws the scene and updates all the entities
	/// </summary>
	/// <param name="scene">The scene to be rendered</param>
	/// <param name="physicsWorld">The physics world associated with the scene</param>
	/// <param name="deltaTime">The time elapsed since the last frame</param>
	void render(Scene& scene, const PhysicsWorld& physicsWorld, float deltaTime);

	/// <summary>
	/// Stops the renderer, this cleans up all the resources it contains (not implemented yet)
	/// </summary>
	void end();

	/// <summary>
	/// Resizes the existing framebuffer with the size specified
	/// </summary>
	/// <param name="newSize">The new size in pixels</param>
	void resizeFramebuffers(glm::vec2 newSize) const;

	[[nodiscard]] glm::vec2 getRenderSize() const;

private:
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

	std::unique_ptr<StaticShadowPass> staticShadowPass;
	std::unique_ptr<GBufferPass> gBufferPass;
	std::unique_ptr<SSAOPass> ssaoPass;
	std::unique_ptr<MainRenderPass> mainRenderPass;
	std::unique_ptr<DebugRenderPass> debugRenderPass;
	std::unique_ptr<OutlinePass> outlinePass;
	std::unique_ptr<BlitPass> blitPass;

	// Creates a framebuffer with the size specified
	void createRenderTargets(glm::vec2 windowSize);
};

#endif