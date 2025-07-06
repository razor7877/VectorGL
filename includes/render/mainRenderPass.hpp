#pragma once

#include "renderPass.hpp"

/// <summary>
/// The pass responsible for generating the main rendering of the scene
/// </summary>
class MainRenderPass : public RenderPass
{
public:

    MainRenderPass();
    ~MainRenderPass() override;
    void execute(Renderer& renderer, const Scene& scene, float deltaTime) override;
};