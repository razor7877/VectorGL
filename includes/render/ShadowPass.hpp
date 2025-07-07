#pragma once

#include "RenderPass.hpp"

/// <summary>
/// The pass responsible for generating the shadow map
/// </summary>
class ShadowPass : public RenderPass
{
public:
    static constexpr int SHADOW_CASCADE_LEVELS = 3;
    static constexpr float SHADOW_CASCADE_DISTANCES[SHADOW_CASCADE_LEVELS] = {
        0.05f,
        0.15f,
        0.35f
    };

    ShadowPass();
    ~ShadowPass() override;
    void execute(Renderer& renderer, const Scene& scene, float deltaTime) override;

private:
    /// <summary>
    /// The width in pixels of the shadow map
    /// </summary>
    static constexpr unsigned int SHADOW_MAP_WIDTH = 2048;

    /// <summary>
    /// The height in pixels of the shadow map
    /// </summary>
    static constexpr unsigned int SHADOW_MAP_HEIGHT = 2048;

    glm::mat4 getLightSpaceMatrix(Renderer& renderer, const Scene& scene, float nearPlane, float farPlane) const;
};