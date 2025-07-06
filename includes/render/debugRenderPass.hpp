#pragma once

#include "renderPass.hpp"
#include "physics/physicsWorld.hpp"

/// <summary>
/// The pass responsible for rendering all the debug helpers (bounding boxes, raycasts etc.)
/// </summary>
class DebugRenderPass : public RenderPass
{
public:
    /// <summary>
    /// Debug lines vertices
    /// </summary>
    std::vector<float> debugLines = std::vector<float>();

    /// <summary>
    /// Debug lines vertices
    /// </summary>
    std::vector<float> storedDebugLines = std::vector<float>();

    DebugRenderPass();
    ~DebugRenderPass() override;
    void execute(Renderer& renderer, const Scene& scene, float deltaTime) override;
    void execute(RenderTarget& outTarget, Renderer& renderer, const Scene& scene, float deltaTime) override;

    void addLine(glm::vec3 startPos, glm::vec3 endPos, bool store);
    void addLines(const std::vector<float>& lines, bool store);
};