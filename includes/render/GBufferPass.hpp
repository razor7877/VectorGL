#pragma once

#include "RenderPass.hpp"

/// <summary>
/// The G buffer pass for screen space effects
/// </summary>
class GBufferPass : public RenderPass
{
public:
    GBufferPass();
    ~GBufferPass() override;

    void execute(Renderer& renderer, const Scene& scene, float deltaTime) override;
};
