#include "render/ReflectionPass.hpp"
#include "Renderer.hpp"
#include "materials/ReflectionMaterial.hpp"
#include "utilities/geometry.hpp"

ReflectionPass::ReflectionPass(Renderer& renderer, const RenderPass& gBufferPass)
    : RenderPass(), gBufferPass(gBufferPass)
{
	std::vector<float> quadVertices = Geometry::getQuadVertices();
	std::vector<float> quadTexCoords = Geometry::getQuadTexCoords();

	this->reflectionQuad = std::make_unique<Entity>("REFLECTION_Quad");
	auto* reflectionQuadMesh = reflectionQuad->addComponent<MeshComponent>();
	reflectionQuadMesh->setMaterial(std::make_unique<ReflectionMaterial>(renderer.shaderManager.getShader(ShaderType::REFLECTION)))
		.addVertices(quadVertices)
		.addTexCoords(quadTexCoords);

	reflectionQuadMesh->start();
}

ReflectionPass::~ReflectionPass()
{
    this->renderTarget.release();
}

void ReflectionPass::execute(Renderer& renderer, const Scene& scene, float deltaTime)
{
    this->renderTarget->bind();
    this->renderTarget->clear();

    Shader* reflectionShader = renderer.shaderManager.getShader(ShaderType::REFLECTION);
    // Setup required uniforms
    reflectionShader->use()
        ->setInt("gPosition", 0)
        ->setInt("gNormal", 1)
        ->setInt("gAlbedo", 2);

	// Bind the G buffer textures
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, this->gBufferPass.renderTarget->gPosition);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, this->gBufferPass.renderTarget->gNormal);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, this->gBufferPass.renderTarget->gAlbedo);

	// Render to quad
	this->reflectionQuad->getComponent<MeshComponent>()->drawGeometry(reflectionShader);

    this->renderTarget->unbind();
}
