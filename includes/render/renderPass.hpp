#pragma once

#include "renderTarget.hpp"
#include "scene.hpp"

class Renderer;

/// <summary>
/// A render pass represents any rendering step in the global rendering pipeline
/// The global pipeline generally consists of many successive passes
/// </summary>
class RenderPass
{
public:
    std::unique_ptr<RenderTarget> renderTarget;

    RenderPass() = default;
    virtual ~RenderPass() = default;
    virtual void execute(Renderer& renderer, const Scene& scene, float deltaTime) = 0;
};
