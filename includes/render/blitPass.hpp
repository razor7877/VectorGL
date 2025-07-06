#pragma once

#include "renderPass.hpp"

/// <summary>
/// The blit pass serves to blit a render target to any other target or texture
/// </summary>
class BlitPass : public RenderPass
{
public:
    BlitPass();
    ~BlitPass() override;

    void execute(Renderer& renderer, const Scene& scene, float deltaTime) override;
    static void execute(RenderTarget& from, RenderTarget& to, Renderer& renderer, const Scene& scene, float deltaTime);
};
