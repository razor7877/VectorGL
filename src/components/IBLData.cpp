#include <glm/glm/ext/matrix_clip_space.hpp>
#include <glm/glm/ext/matrix_transform.hpp>

#include "components/IBLData.hpp"

IBLData::IBLData(Renderer& renderer, SkyboxComponent* skyComponent, Texture* hdrMap)
{
	// We start by querying all the necessary shaders
	Shader* hdrToCubemapShader = renderer.shaderManager.getShader(ShaderType::HDRTOCUBEMAP);
	Shader* irradianceShader = renderer.shaderManager.getShader(ShaderType::IRRADIANCE);
	Shader* prefilterShader = renderer.shaderManager.getShader(ShaderType::PREFILTER);
	Shader* brdfShader = renderer.shaderManager.getShader(ShaderType::BRDF);

	float boxVertices[] = { -1.0f,  1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f,  1.0f, -1.0f, -1.0f,  1.0f, -1.0f, -1.0f, -1.0f,  1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f, -1.0f, -1.0f,  1.0f, -1.0f, -1.0f,  1.0f,  1.0f, -1.0f, -1.0f,  1.0f, 1.0f, -1.0f, -1.0f, 1.0f, -1.0f,  1.0f, 1.0f,  1.0f,  1.0f, 1.0f,  1.0f,  1.0f, 1.0f,  1.0f, -1.0f, 1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f, -1.0f,  1.0f,  1.0f, 1.0f,  1.0f,  1.0f, 1.0f,  1.0f,  1.0f, 1.0f, -1.0f,  1.0f, -1.0f, -1.0f,  1.0f, -1.0f,  1.0f, -1.0f, 1.0f,  1.0f, -1.0f, 1.0f,  1.0f,  1.0f, 1.0f,  1.0f,  1.0f, -1.0f,  1.0f,  1.0f, -1.0f,  1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f, 1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f, 1.0f, -1.0f,  1.0f };

	// Create an entity that will contain a box mesh to display the HDR map
	std::unique_ptr<Entity> cubemapEntity = std::unique_ptr<Entity>(new Entity("HDR Cubemap"));
	MeshComponent* lightMesh = cubemapEntity->addComponent<MeshComponent>();
	lightMesh->setupMesh(boxVertices, sizeof(boxVertices), hdrToCubemapShader);
	lightMesh->addTexture(hdrMap);

	cubemapEntity->start();

	// We create a render target for rendering the HDR map to a cubemap
	GLuint captureFBO;
	GLuint captureRBO;
	glGenFramebuffers(1, &captureFBO);
	glGenRenderbuffers(1, &captureRBO);

	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 2048, 2048);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);

	this->environmentMap->bind();

	// The projection and view matrices to capture each face of the cube
	glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
	glm::mat4 captureViews[] =
	{
	   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
	   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
	   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
	   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
	   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
	   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
	};

	hdrToCubemapShader->use()
		->setInt("equirectangularMap", 0)
		->setMat4("projection", captureProjection);

	glActiveTexture(GL_TEXTURE0);
	hdrMap->bindTexture();

	// Convert the 2D map to a cubemap
	glViewport(0, 0, 2048, 2048);
	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	for (unsigned int i = 0; i < 6; ++i)
	{
		//defaultRenderer.shaderManager.updateUniformBuffer(captureProjection, captureViews[i]);
		hdrToCubemapShader->setMat4("view", captureViews[i]);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, this->environmentMap->texID, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		cubemapEntity->update(0);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Set the sky to use the new cubemap
	skyComponent->setCubemap(this->environmentMap);

	// Resize FBO to cubemap size
	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 32, 32);

	// Create irradiance map
	irradianceShader->use();
	irradianceShader->setInt("environmentMap", 0);
	irradianceShader->setMat4("projection", captureProjection);
	glActiveTexture(GL_TEXTURE0);
	this->environmentMap->bind();

	glViewport(0, 0, 32, 32);
	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	for (unsigned int i = 0; i < 6; i++)
	{
		irradianceShader->setMat4("view", captureViews[i]);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, this->irradianceMap->texID, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		lightMesh->setupMesh(boxVertices, sizeof(boxVertices), irradianceShader);
		lightMesh->start();
		cubemapEntity->update(0);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	prefilterShader->use();
	prefilterShader->setInt("environmentMap", 0);
	prefilterShader->setMat4("projection", captureProjection);
	glActiveTexture(GL_TEXTURE0);
	this->environmentMap->bind();

	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	unsigned int maxMipLevels = 5;
	for (unsigned int mip = 0; mip < maxMipLevels; ++mip)
	{
		// Resize framebuffer according to mip-level size
		unsigned int mipWidth = 1024 * std::pow(0.5, mip);
		unsigned int mipHeight = 1024 * std::pow(0.5, mip);
		glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth, mipHeight);
		glViewport(0, 0, mipWidth, mipHeight);

		float roughness = (float)mip / (float)(maxMipLevels - 1);
		prefilterShader->setFloat("roughness", roughness);

		for (unsigned int i = 0; i < 6; ++i)
		{
			prefilterShader->setMat4("view", captureViews[i]);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, this->prefilterMap->texID, mip);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			lightMesh->setupMesh(boxVertices, sizeof(boxVertices), prefilterShader);
			lightMesh->start();
			cubemapEntity->update(0);
		}
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	skyComponent->setCubemap(this->environmentMap);

	// Create BRDF map
	float quad_verts[] = {
	-1.0f, 1.0f, 0.0f, // Top left
	1.0f, 1.0f, 0.0f, // Top right
	-1.0f, -1.0f, 0.0f, // Bottom left

	1.0f, 1.0f, 0.0f, // Top right
	1.0f, -1.0f, 0.0f, // Bottom right
	-1.0f, -1.0f, 0.0f, // Bottom left
	};

	float quad_tex_coords[] = {
		0.0f, 0.0f, // Top left
		1.0f, 0.0f, // Top right
		0.0f, 1.0f, // Bottom left

		1.0f, 0.0f, // Top right
		1.0f, 1.0f, // Bottom right
		0.0f, 1.0f, // Bottom left
	};

	std::unique_ptr<Entity> quadEntity = std::unique_ptr<Entity>(new Entity("Quad"));
	MeshComponent* quadMesh = quadEntity->addComponent<MeshComponent>();
	quadMesh->setupMesh(quad_verts, sizeof(quad_verts), brdfShader);
	quadMesh->addTexCoords(quad_tex_coords, sizeof(quad_tex_coords));
	quadMesh->addTexture(hdrMap);

	//defaultRenderer.addEntity(quadEntity);
	quadMesh->start();

	unsigned int brdfLUTTexture;
	glGenTextures(1, &brdfLUTTexture);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, brdfLUTTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, 512, 512, 0, GL_RG, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, brdfLUTTexture, 0);

	glViewport(0, 0, 512, 512);
	glClearColor(0.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	quadEntity->update(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	this->brdfLut = new Texture(brdfLUTTexture, TextureType::TEXTURE_ALBEDO);
}

IBLData::~IBLData()
{
	delete irradianceMap;
	delete prefilterMap;
	delete brdfLut;
}