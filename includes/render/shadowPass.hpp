#pragma once

#include "renderPass.hpp"

/// <summary>
/// The pass responsible for generating the shadow map
/// </summary>
class ShadowPass : public RenderPass
{
public:

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