#pragma once

#include "RenderPass.hpp"

/// <summary>
/// The pass responsible for generating the main rendering of the scene
/// </summary>
class ReflectionPass : public RenderPass
{
public:
    ReflectionPass() = delete;
    ReflectionPass(Renderer& renderer, const RenderPass& gBufferPass);
    ~ReflectionPass() override;
    void execute(Renderer& renderer, const Scene& scene, float deltaTime) override;

private:
    RenderTarget reflectionRenderTarget;
    RenderTarget refractionRenderTarget;

    /// <summary>
    /// The quad for the SSAO rendering
    /// </summary>
    std::unique_ptr<Entity> reflectionQuad;

    /// <summary>
    /// The G-buffer pass
    /// </summary>
    const RenderPass& gBufferPass;
};