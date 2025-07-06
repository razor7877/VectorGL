#include <random>

#include "render/SSAOPass.hpp"
#include "Renderer.hpp"
#include "materials/PBRMaterial.hpp"
#include "utilities/geometry.hpp"

SSAOPass::SSAOPass(Renderer& renderer, const RenderPass& gBufferPass)
    : RenderPass(), gBufferPass(gBufferPass)
{
    // Initialize SSAO kernels
	std::uniform_real_distribution<float> randomFloats(0.0, 1.0);
	std::default_random_engine generator;

	for (unsigned int i = 0; i < 64; ++i)
	{
		glm::vec3 sample(
			randomFloats(generator) * 2.0f - 1.0f, // x between -1:1
			randomFloats(generator) * 2.0f - 1.0f, // y between -1:1
			randomFloats(generator) // z between 0:1
		);

		sample = glm::normalize(sample);

		float scale = static_cast<float>(i) / 64.0f;
		// Lerp
		scale = 0.1f + (scale * scale) * (1.0f - 0.1f);
		sample *= scale;

		ssaoKernel.push_back(sample);
	}

	for (unsigned int i = 0; i < 16; i++)
	{
		glm::vec3 noise(
			randomFloats(generator) * 2.0f - 1.0f,
			randomFloats(generator) * 2.0f - 1.0f,
			0.0f
		);

		ssaoNoise.push_back(noise);
	}

	GLuint noiseTexture;
	glGenTextures(1, &noiseTexture);
	glBindTexture(GL_TEXTURE_2D, noiseTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, 4, 4, 0, GL_RGB, GL_FLOAT, &ssaoNoise[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	this->ssaoNoiseTexture = std::make_unique<Texture>(noiseTexture, TextureType::TEXTURE_2D);

	std::vector<float> quadVertices = Geometry::getQuadVertices();
	std::vector<float> quadTexCoords = Geometry::getQuadTexCoords();

	this->ssaoQuad = std::make_unique<Entity>("SSAO_Quad");
	auto* ssaoQuadMesh = ssaoQuad->addComponent<MeshComponent>();
	ssaoQuadMesh->setMaterial(std::make_unique<PBRMaterial>(renderer.shaderManager.getShader(ShaderType::SSAO)))
		.addVertices(quadVertices)
		.addTexCoords(quadTexCoords);

	ssaoQuadMesh->start();

	this->ssaoBlurQuad = std::make_unique<Entity>("SSAO_Blur_Quad");
	auto* ssaoBlurQuadMesh = ssaoBlurQuad->addComponent<MeshComponent>();
	ssaoBlurQuadMesh->setMaterial(std::make_unique<PBRMaterial>(renderer.shaderManager.getShader(ShaderType::SSAOBLUR)))
		.addVertices(quadVertices)
		.addTexCoords(quadTexCoords);

	ssaoBlurQuadMesh->start();
}

SSAOPass::~SSAOPass()
{
    this->renderTarget.release();
}


void SSAOPass::execute(Renderer& renderer, const Scene& scene, float deltaTime)
{
    this->ssaoTarget->bind();
    this->ssaoTarget->clear();

    Shader* ssaoShader = renderer.shaderManager.getShader(ShaderType::SSAO);
    // Setup required uniforms
    ssaoShader->use()
        ->setInt("gPosition", 0)
        ->setInt("gNormal", 1)
        ->setInt("texNoise", 2)
        ->setVec2("noiseScale", glm::vec2(this->ssaoTarget->size.x / 4.0f, this->ssaoTarget->size.y / 4.0f));

    for (unsigned int i = 0; i < 64; i++)
        ssaoShader->setVec3("samples[" + std::to_string(i) + "]", this->ssaoKernel[i]);

    // Bind the G buffer textures
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, this->gBufferPass.renderTarget->gPosition);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, this->gBufferPass.renderTarget->gNormal);

    glActiveTexture(GL_TEXTURE2);
    this->ssaoNoiseTexture->bindTexture();

    // Render SSAO to quad
    this->ssaoQuad->getComponent<MeshComponent>()->drawGeometry(ssaoShader);

    this->ssaoTarget->unbind();

    // Now we want to blur the result to correct the repeating noise pattern
    this->renderTarget->bind();
    this->renderTarget->clear();

    Shader* ssaoBlurShader = renderer.shaderManager.getShader(ShaderType::SSAOBLUR);
    ssaoBlurShader->use()
        ->setInt("ssaoInput", 0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, this->ssaoTarget->renderTexture);

    this->ssaoBlurQuad->getComponent<MeshComponent>()->drawGeometry(ssaoBlurShader);

    this->renderTarget->unbind();
}
