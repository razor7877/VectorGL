#include <glm/glm/ext/matrix_clip_space.hpp>
#include <glm/glm/ext/matrix_transform.hpp>

#include "components/IBLData.hpp"
#include "components/skyboxComponent.hpp"
#include "components/meshComponent.hpp"
#include "renderTarget.hpp"
#include "utilities/geometry.hpp"

IBLData::IBLData(Renderer& renderer, std::shared_ptr<Texture> hdrMap)
{
	// We start by querying all the necessary shaders
	Shader* hdrToCubemapShader = renderer.shaderManager.getShader(ShaderType::HDRTOCUBEMAP);
	Shader* irradianceShader = renderer.shaderManager.getShader(ShaderType::IRRADIANCE);
	Shader* prefilterShader = renderer.shaderManager.getShader(ShaderType::PREFILTER);
	Shader* brdfShader = renderer.shaderManager.getShader(ShaderType::BRDF);

	std::vector<float> boxVertices = Geometry::getCubeVertices();

	// Create an entity that will contain a box mesh to display the HDR map
	std::unique_ptr<Entity> cubemapEntity = std::make_unique<Entity>("HDR Cubemap");
	MeshComponent* lightMesh = cubemapEntity->addComponent<MeshComponent>();
	lightMesh->setupMesh(&boxVertices[0], boxVertices.size() * sizeof(float), hdrToCubemapShader);
	lightMesh->addTexture(hdrMap);

	cubemapEntity->start();

	// We create a render target for rendering the HDR map to a cubemap
	RenderTarget captureRT = RenderTarget(TargetType::TEXTURE_CUBEMAP, glm::vec2(512, 512), GL_RGB16F);
	captureRT.bind();

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
	captureRT.bind();
	for (unsigned int i = 0; i < 6; ++i)
	{
		hdrToCubemapShader->setMat4("view", captureViews[i]);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, this->environmentMap->texID, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		cubemapEntity->update(0);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Resize FBO to cubemap size
	captureRT.bind();
	captureRT.resize(glm::vec2(32, 32));

	// Create irradiance map
	irradianceShader->use();
	irradianceShader->setInt("environmentMap", 0);
	irradianceShader->setMat4("projection", captureProjection);
	glActiveTexture(GL_TEXTURE0);
	this->environmentMap->bind();

	captureRT.bind();
	for (unsigned int i = 0; i < 6; i++)
	{
		irradianceShader->setMat4("view", captureViews[i]);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, this->irradianceMap->texID, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		lightMesh->setupMesh(&boxVertices[0], boxVertices.size() * sizeof(float), irradianceShader);
		lightMesh->start();
		cubemapEntity->update(0);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// We first generate the mip levels for the prefiltered map before we draw into them
	this->prefilterMap->bind();
	this->prefilterMap->generateMipMaps();

	prefilterShader->use();
	prefilterShader->setInt("environmentMap", 0);
	prefilterShader->setMat4("projection", captureProjection);
	glActiveTexture(GL_TEXTURE0);
	this->environmentMap->bind();

	// We bind the framebuffer and start capturing each face of the cube for each mip level
	captureRT.bind();
	unsigned int maxMipLevels = 5;

	for (unsigned int mip = 0; mip < maxMipLevels; ++mip)
	{
		// Resize framebuffer according to mip-level size
		unsigned int mipWidth = 1024 * std::pow(0.5, mip);
		unsigned int mipHeight = 1024 * std::pow(0.5, mip);
		captureRT.resize(glm::vec2(mipWidth, mipHeight));

		float roughness = (float)mip / (float)(maxMipLevels - 1);
		prefilterShader->setFloat("roughness", roughness);

		for (unsigned int i = 0; i < 6; ++i)
		{
			prefilterShader->setMat4("view", captureViews[i]);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, this->prefilterMap->texID, mip);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			lightMesh->setupMesh(&boxVertices[0], boxVertices.size() * sizeof(float), prefilterShader);
			lightMesh->start();
			cubemapEntity->update(0);
		}
	}
	captureRT.unbind();

	// Create BRDF map
	std::vector<float> quadVertices = Geometry::getQuadVertices();
	std::vector<float> quadTexCoords = Geometry::getQuadTexCoords();

	std::unique_ptr<Entity> quadEntity = std::make_unique<Entity>("Quad");
	MeshComponent* quadMesh = quadEntity->addComponent<MeshComponent>();
	quadMesh->setupMesh(&quadVertices[0], quadVertices.size() * sizeof(float), brdfShader);
	quadMesh->addTexCoords(quadTexCoords);
	quadMesh->addTexture(hdrMap);

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

	captureRT.bind();
	captureRT.resize(glm::vec2(512, 512));
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, brdfLUTTexture, 0);

	glClearColor(0.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	quadEntity->update(0);
	captureRT.unbind();

	this->brdfLut = std::make_shared<Texture>(brdfLUTTexture, TextureType::TEXTURE_ALBEDO);

	cubemapEntity.reset();
}

IBLData::IBLData(Renderer& renderer, Cubemap* cubemap) : environmentMap(cubemap)
{
	// We start by querying all the necessary shaders
	Shader* irradianceShader = renderer.shaderManager.getShader(ShaderType::IRRADIANCE);
	Shader* prefilterShader = renderer.shaderManager.getShader(ShaderType::PREFILTER);
	Shader* brdfShader = renderer.shaderManager.getShader(ShaderType::BRDF);

	std::vector<float> boxVertices = Geometry::getCubeVertices();

	// Create an entity that will contain a box mesh to display the HDR map
	std::unique_ptr<Entity> cubemapEntity = std::make_unique<Entity>("HDR Cubemap");
	MeshComponent* lightMesh = cubemapEntity->addComponent<MeshComponent>();
	lightMesh->setupMesh(&boxVertices[0], boxVertices.size() * sizeof(float), irradianceShader);

	cubemapEntity->start();

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

	// We create a render target for rendering the HDR map to a cubemap
	RenderTarget captureRT = RenderTarget(TargetType::TEXTURE_CUBEMAP, glm::vec2(32, 32), GL_RGB16F);
	captureRT.bind();

	// Create irradiance map
	irradianceShader->use();
	irradianceShader->setInt("environmentMap", 0);
	irradianceShader->setMat4("projection", captureProjection);
	glActiveTexture(GL_TEXTURE0);
	this->environmentMap->bind();

	captureRT.bind();
	for (unsigned int i = 0; i < 6; i++)
	{
		irradianceShader->setMat4("view", captureViews[i]);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, this->irradianceMap->texID, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		cubemapEntity->update(0);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// We first generate the mip levels for the prefiltered map before we draw into them
	this->prefilterMap->bind();
	this->prefilterMap->generateMipMaps();

	prefilterShader->use();
	prefilterShader->setInt("environmentMap", 0);
	prefilterShader->setMat4("projection", captureProjection);
	glActiveTexture(GL_TEXTURE0);
	this->environmentMap->bind();

	// We bind the framebuffer and start capturing each face of the cube for each mip level
	captureRT.bind();
	unsigned int maxMipLevels = 5;

	for (unsigned int mip = 0; mip < maxMipLevels; ++mip)
	{
		// Resize framebuffer according to mip-level size
		unsigned int mipWidth = 1024 * std::pow(0.5, mip);
		unsigned int mipHeight = 1024 * std::pow(0.5, mip);
		captureRT.resize(glm::vec2(mipWidth, mipHeight));

		float roughness = (float)mip / (float)(maxMipLevels - 1);
		prefilterShader->setFloat("roughness", roughness);

		for (unsigned int i = 0; i < 6; ++i)
		{
			prefilterShader->setMat4("view", captureViews[i]);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, this->prefilterMap->texID, mip);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			lightMesh->setupMesh(&boxVertices[0], boxVertices.size() * sizeof(float), prefilterShader);
			lightMesh->start();
			cubemapEntity->update(0);
		}
	}
	captureRT.unbind();

	// Create BRDF map
	std::vector<float> quadVertices = Geometry::getQuadVertices();
	std::vector<float> quadTexCoords = Geometry::getQuadTexCoords();

	std::unique_ptr<Entity> quadEntity = std::make_unique<Entity>("Quad");
	MeshComponent* quadMesh = quadEntity->addComponent<MeshComponent>();
	quadMesh->setupMesh(&quadVertices[0], quadVertices.size() * sizeof(float), brdfShader);
	quadMesh->addTexCoords(quadTexCoords);

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

	captureRT.bind();
	captureRT.resize(glm::vec2(512, 512));
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, brdfLUTTexture, 0);

	glClearColor(0.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	quadEntity->update(0);
	captureRT.unbind();

	this->brdfLut = std::make_shared<Texture>(brdfLUTTexture, TextureType::TEXTURE_ALBEDO);
}

IBLData::~IBLData()
{
	delete this->irradianceMap;
	delete this->prefilterMap;
	this->brdfLut.reset();
}