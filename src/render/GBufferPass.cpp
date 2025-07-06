#include <random>

#include "render/GBufferPass.hpp"
#include "Renderer.hpp"

GBufferPass::GBufferPass() : RenderPass()
{

}

GBufferPass::~GBufferPass()
{
    this->renderTarget.release();
}


void GBufferPass::execute(Renderer& renderer, const Scene& scene, float deltaTime)
{
    this->renderTarget->bind();
    this->renderTarget->clear();

    // Use G-buffer shader
    Shader* gBufferShader = renderer.shaderManager.getShader(ShaderType::GBUFFER);
    gBufferShader->use();

    for (MeshComponent* mesh : scene.sortedSceneData.meshes)
        mesh->drawGeometry(gBufferShader);

    this->renderTarget->unbind();
}
