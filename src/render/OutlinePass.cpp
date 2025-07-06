#include <stdexcept>

#include "render/OutlinePass.hpp"
#include "Renderer.hpp"

OutlinePass::OutlinePass() : RenderPass()
{

}

OutlinePass::~OutlinePass()
{
    this->renderTarget.release();
}

void OutlinePass::execute(Renderer& renderer, const Scene& scene, float deltaTime)
{
    throw std::runtime_error("OutlinePass::execute() should not be called without a render target!");
}

void OutlinePass::execute(RenderTarget& outTarget, Renderer& renderer, const Scene& scene, float deltaTime)
{
    outTarget.bind();

    glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
    // Disable depth test before drawing outlines
    glDisable(GL_DEPTH_TEST);
    // Use outline shader
    Shader* outlineShader = renderer.shaderManager.getShader(ShaderType::OUTLINE);
    outlineShader->use();

    for (Entity* outlinedEntity : scene.sortedSceneData.outlineRenderList)
    {
        auto* mesh = outlinedEntity->getComponent<MeshComponent>();
        glm::vec3 originalScale = outlinedEntity->getTransform()->getScale();
        Shader* originalShader = mesh->material->shaderProgram;

        mesh->material->shaderProgram = outlineShader;

        outlinedEntity->getTransform()->setScale(originalScale * 1.1f);
        // TODO: Fix shadow bug. Whenever an object is outlined, the shadow on the next frame end up bigger even though the object was scaled back down, maybe depth or stencil issue?
        outlinedEntity->update(0);
        outlinedEntity->getTransform()->setScale(originalScale);

        mesh->material->shaderProgram = originalShader;
    }

    // Re-enable depth test after drawing outlines
    glStencilFunc(GL_ALWAYS, 1, 0xFF);
    glEnable(GL_DEPTH_TEST);
    // Re-enable stencil writes or buffer won't be cleared properly on next frame
    glStencilMask(0xFF);

    outTarget.unbind();
}

