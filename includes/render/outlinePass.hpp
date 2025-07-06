#pragma once

#include "renderPass.hpp"
#include "physics/physicsWorld.hpp"

/// <summary>
/// The pass responsible for rendering the outlines of selected objects
/// </summary>
class OutlinePass : public RenderPass
{
public:
    OutlinePass();
    ~OutlinePass() override;
    void execute(Renderer& renderer, const Scene& scene, float deltaTime) override;
    void execute(RenderTarget& outTarget, Renderer& renderer, const Scene& scene, float deltaTime) override;
};