#include "render/mainRenderPass.hpp"
#include "renderer.hpp"

MainRenderPass::MainRenderPass() : RenderPass()
{

}

MainRenderPass::~MainRenderPass()
{
    this->renderTarget.release();
}


void MainRenderPass::execute(Renderer& renderer, const Scene& scene, float deltaTime)
{
    // We now want to draw to the MSAA framebuffer
    this->renderTarget->bind();
    this->renderTarget->clear();

    glStencilMask(0x00);

    renderer.shaderManager.getShader(ShaderType::PBR)
        ->use()
        ->setVec2("windowSize", this->renderTarget->size);

    // We can simply update all entities that won't be rendered
    for (Entity* nonRenderable : scene.sortedSceneData.logicEntities)
        nonRenderable->update(deltaTime);

    for (PhysicsComponent* physics : scene.sortedSceneData.physicsComponents)
        physics->update(deltaTime);

    glEnable(GL_DEPTH_TEST);
    glStencilMask(0xFF);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    glStencilFunc(GL_ALWAYS, 1, 0xFF);

    // Entities that can be rendered are grouped by shader and then rendered together
    for (auto& [shader, meshes] : scene.sortedSceneData.renderList)
    {
        shader->use();

        for (Entity* renderable : meshes)
        {
            // We only write to the stencil mask if the entity should have an outline
            if (renderable->drawOutline)
                glStencilMask(0xFF);
            else
                glStencilMask(0x00);

            renderable->update(deltaTime);
        }
    }

    for (auto& [shader, meshesByDistance] : scene.sortedSceneData.transparentRenderList)
    {
        shader->use();

        for (auto it = meshesByDistance.rbegin(); it != meshesByDistance.rend(); ++it)
        {
            // We only write to the stencil mask if the entity should have an outline
            if (it->second->drawOutline)
                glStencilMask(0xFF);
            else
                glStencilMask(0x00);

            it->second->update(deltaTime);
        }
    }

    // Disable stencil writes
    glStencilMask(0x00);

    this->renderTarget->unbind();
}
