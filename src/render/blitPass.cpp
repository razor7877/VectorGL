#include <stdexcept>

#include "render/blitPass.hpp"
#include "renderer.hpp"

BlitPass::BlitPass() : RenderPass()
{

}

BlitPass::~BlitPass()
{
    this->renderTarget.release();
}

void BlitPass::execute(Renderer& renderer, const Scene& scene, float deltaTime)
{
    throw std::runtime_error("BlitPass::execute() should not be called without specifying render targets!");
}

void BlitPass::execute(RenderTarget& from, RenderTarget& to, Renderer& renderer, const Scene& scene, float deltaTime)
{
    // Bind the second target that will contain the mixed multi sampled textures
    glBindFramebuffer(GL_READ_FRAMEBUFFER, from.framebuffer);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, to.framebuffer);

    glm::vec2 framebufferSize = from.size;
    // Resolve the multi sampled texture to the second target
    glScissor(0, 0, framebufferSize.x, framebufferSize.y);
    glBlitFramebuffer(0, 0, framebufferSize.x, framebufferSize.y, 0, 0, framebufferSize.x, framebufferSize.y, GL_COLOR_BUFFER_BIT, GL_NEAREST);
}

