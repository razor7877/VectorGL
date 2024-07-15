#include <iostream>
#include <set>
#include <algorithm>

#include <glm/glm/ext/matrix_transform.hpp>

#include "main.hpp"
#include "renderer.hpp"
#include "shaderManager.hpp"
#include "entity.hpp"
#include "materials/pbrMaterial.hpp"

unsigned int sphereVAO = 0;
unsigned int indexCount;
void renderSphere()
{
	if (sphereVAO == 0)
	{
		glGenVertexArrays(1, &sphereVAO);

		unsigned int vbo, ebo;
		glGenBuffers(1, &vbo);
		glGenBuffers(1, &ebo);

		std::vector<glm::vec3> positions;
		std::vector<glm::vec2> uv;
		std::vector<glm::vec3> normals;
		std::vector<unsigned int> indices;

		const unsigned int X_SEGMENTS = 64;
		const unsigned int Y_SEGMENTS = 64;
		const float PI = 3.14159265359f;
		for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
		{
			for (unsigned int y = 0; y <= Y_SEGMENTS; ++y)
			{
				float xSegment = (float)x / (float)X_SEGMENTS;
				float ySegment = (float)y / (float)Y_SEGMENTS;
				float xPos = std::cos(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
				float yPos = std::cos(ySegment * PI);
				float zPos = std::sin(xSegment * 2.0f * PI) * std::sin(ySegment * PI);

				positions.push_back(glm::vec3(xPos, yPos, zPos));
				uv.push_back(glm::vec2(xSegment, ySegment));
				normals.push_back(glm::vec3(xPos, yPos, zPos));
			}
		}

		bool oddRow = false;
		for (unsigned int y = 0; y < Y_SEGMENTS; ++y)
		{
			if (!oddRow) // even rows: y == 0, y == 2; and so on
			{
				for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
				{
					indices.push_back(y * (X_SEGMENTS + 1) + x);
					indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
				}
			}
			else
			{
				for (int x = X_SEGMENTS; x >= 0; --x)
				{
					indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
					indices.push_back(y * (X_SEGMENTS + 1) + x);
				}
			}
			oddRow = !oddRow;
		}
		indexCount = static_cast<unsigned int>(indices.size());

		std::vector<float> data;
		for (unsigned int i = 0; i < positions.size(); ++i)
		{
			data.push_back(positions[i].x);
			data.push_back(positions[i].y);
			data.push_back(positions[i].z);
			if (normals.size() > 0)
			{
				data.push_back(normals[i].x);
				data.push_back(normals[i].y);
				data.push_back(normals[i].z);
			}
			if (uv.size() > 0)
			{
				data.push_back(uv[i].x);
				data.push_back(uv[i].y);
			}
		}
		glBindVertexArray(sphereVAO);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), &data[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
		unsigned int stride = (3 + 2 + 3) * sizeof(float);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));
	}

	glBindVertexArray(sphereVAO);
	glDrawElements(GL_TRIANGLE_STRIP, indexCount, GL_UNSIGNED_INT, 0);
}

Renderer::Renderer()
{

}

std::vector<Entity*> Renderer::GetEntities()
{
	return this->entities;
}

GLuint Renderer::GetRenderTexture()
{
	return this->finalTarget.renderTexture;
}

void Renderer::addEntity(Entity* objectPtr)
{
	this->entities.push_back(objectPtr);
}

void Renderer::removeEntity(Entity* objectPtr)
{
	this->entities.erase(std::remove(this->entities.begin(), this->entities.end(), objectPtr), this->entities.end());
}

void Renderer::resizeFramebuffer(glm::vec2 newSize)
{
	this->multiSampledTarget.resize(newSize);
	this->finalTarget.resize(newSize);
}

void Renderer::createFramebuffer(glm::vec2 windowSize)
{
	this->multiSampledTarget = RenderTarget(windowSize, true);
	this->finalTarget = RenderTarget(windowSize, false);
}

void Renderer::init(glm::vec2 windowSize)
{
	this->shaderManager.initUniformBuffer();

	// Create the camera and set it up
	Entity* cameraEntity = new Entity("Camera");
	this->currentCamera = cameraEntity->addComponent<CameraComponent>();
	this->addEntity(cameraEntity);

	// Initializes the light manager if it was setup to send all required data to the shader
	if (LightManager::getInstance().shaderProgram->ID != 0)
		LightManager::getInstance().init();

	for (Entity* entity : this->entities)
		entity->start();

	this->createFramebuffer(windowSize);
}

void Renderer::render(float deltaTime)
{
	// We want to draw to the MSAA framebuffer
	this->multiSampledTarget.bind();

	// Update camera info
	glm::vec2 windowSize = this->multiSampledTarget.size;
	this->shaderManager.updateUniformBuffer(this->currentCamera->getViewMatrix(), this->currentCamera->getProjectionMatrix(windowSize.x, windowSize.y));

	// Send light data to shader
	LightManager::getInstance().sendToShader();

	// Render & update the scene
	for (Entity* entity : this->entities)
		entity->update(deltaTime);

	Shader* pbrShader = this->shaderManager.getShader(ShaderType::PBR);
	pbrShader->use();
	pbrShader->setBool("material.use_albedo_map", false);
	pbrShader->setBool("material.use_normal_map", false);
	pbrShader->setBool("material.use_metallic_map", false);
	pbrShader->setBool("material.use_roughness_map", false);
	pbrShader->setBool("material.use_ao_map", false);

	const int nrRows = 7;
	const int nrColumns = 7;
	float spacing = 2.5f;

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);

	if (PBRMaterial::irradianceMap != nullptr)
	{
		glActiveTexture(GL_TEXTURE6);
		pbrShader->setInt("irradianceMap", 6);
		PBRMaterial::irradianceMap->bind();
	}

	if (PBRMaterial::prefilterMap != nullptr)
	{
		glActiveTexture(GL_TEXTURE7);
		pbrShader->setInt("prefilterMap", 7);
		PBRMaterial::prefilterMap->bind();
	}

	if (PBRMaterial::brdfLut != nullptr)
	{
		glActiveTexture(GL_TEXTURE8);
		pbrShader->setInt("brdfLUT", 8);
		PBRMaterial::brdfLut->bindTexture();
	}

	pbrShader->setFloat("material.ao", 1.0f);

	glm::mat4 model = glm::mat4(1.0f);
	for (int row = 0; row < nrRows; ++row)
	{
		pbrShader->setFloat("material.metallic", (float)row / (float)nrRows);
		for (int col = 0; col < nrColumns; ++col)
		{
			// we clamp the roughness to 0.025 - 1.0 as perfectly smooth surfaces (roughness of 0.0) tend to look a bit off
			// on direct lighting.
			pbrShader->setFloat("material.roughness", glm::clamp((float)col / (float)nrColumns, 0.05f, 1.0f));

			model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(
				(float)(col - (nrColumns / 2)) * spacing,
				(float)(row - (nrRows / 2)) * spacing,
				-2.0f
			));
			pbrShader->setMat4("model", model);
			pbrShader->setMat3("normalMatrix", glm::transpose(glm::inverse(glm::mat3(model))));
			renderSphere();
		}
	}

	this->multiSampledTarget.unbind();

	// Bind the second target that will contain the mixed multisampled textures
	glBindFramebuffer(GL_READ_FRAMEBUFFER, this->multiSampledTarget.framebuffer);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, this->finalTarget.framebuffer);

	glm::vec2 framebufferSize = this->multiSampledTarget.size;
	// Resolve the multisampled texture to the second target
	glBlitFramebuffer(0, 0, framebufferSize.x, framebufferSize.y, 0, 0, framebufferSize.x, framebufferSize.y, GL_COLOR_BUFFER_BIT, GL_NEAREST);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::end()
{
	
}