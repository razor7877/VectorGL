#pragma once

#include "RenderPass.hpp"

/// <summary>
/// The pass responsible for calculating SSAO
/// </summary>
class SSAOPass : public RenderPass
{
public:
    /// <summary>
    /// The scaling factor to scale the resolution of the SSAO map relative to the window base resolution
    /// </summary>
    static constexpr float SSAO_SCALE_FACTOR = 0.75;

    /// <summary>
    /// The render target for rendering the SSAO
    /// </summary>
    std::unique_ptr<RenderTarget> ssaoTarget;

    SSAOPass() = delete;
    SSAOPass(Renderer& renderer, const RenderPass& gBufferPass);
    ~SSAOPass() override;
    void execute(Renderer& renderer, const Scene& scene, float deltaTime) override;

private:
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
    /// The G-buffer pass
    /// </summary>
    const RenderPass& gBufferPass;
};