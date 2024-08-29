#include <memory>
#include <vector>

#include "game/mainGameState.hpp"
#include "shader.hpp"
#include "entity.hpp"
#include "renderer.hpp"
#include "lightManager.hpp"
#include "components/meshComponent.hpp"
#include "components/skyboxComponent.hpp"
#include "components/physicsComponent.hpp"
#include "components/cameraComponent.hpp"
#include "components/lights/pointLightComponent.hpp"
#include "components/lights/directionalLightComponent.hpp"
#include "utilities/geometry.hpp"

void MainGameState::init()
{
	Shader* phongShader = this->renderer.shaderManager.getShader(ShaderType::PHONG);
	Shader* pbrShader = this->renderer.shaderManager.getShader(ShaderType::PBR);
	Shader* skyboxShader = this->renderer.shaderManager.getShader(ShaderType::SKYBOX);

	LightManager::getInstance().shaderProgram = pbrShader;

	//this->renderer.physicsWorld->enableDebugDraw = true;

	// Cube
	std::unique_ptr<Entity> cubeEntity = std::make_unique<Entity>("Cube");

	MeshComponent* cubeMesh = cubeEntity->addComponent<MeshComponent>();
	std::vector<float> cubeVertices = Geometry::getCubeVertices();
	cubeMesh->setMaterial(std::make_unique<PBRMaterial>(pbrShader))
		.addVertices(cubeVertices);

	PhysicsComponent* cubeCollider = cubeEntity->addComponent<PhysicsComponent>();
	this->renderer.physicsWorld->addBox(cubeCollider, glm::vec3(1.0f), glm::vec3(0.0f));

	this->renderer.addEntity(std::move(cubeEntity));

	cubeEntity = std::make_unique<Entity>("Cube");

	cubeMesh = cubeEntity->addComponent<MeshComponent>();
	cubeMesh->setMaterial(std::make_unique<PBRMaterial>(pbrShader))
		.addVertices(cubeVertices);

	cubeCollider = cubeEntity->addComponent<PhysicsComponent>();
	this->renderer.physicsWorld->addBox(cubeCollider, glm::vec3(1.0f), glm::vec3(0.0f));

	this->renderer.addEntity(std::move(cubeEntity));

	VertexData sphere = Geometry::getSphereVertices(100, 30);
	VertexDataIndices sphereOptimized = Geometry::optimizeVertices(sphere.vertices, sphere.normals);

	for (int x = 0; x < 13; x++)
	{
		for (int y = 0; y < 13; y++)
		{
			for (int z = 0; z < 13; z++)
			{
				std::unique_ptr<Entity> sphereEntity = std::make_unique<Entity>("Sphere");

				MeshComponent* sphereMesh = sphereEntity->addComponent<MeshComponent>();
				sphereMesh->setMaterial(std::make_unique<PBRMaterial>(pbrShader))
					.addVertices(sphereOptimized.vertices)
					.addIndices(sphereOptimized.indices)
					.addNormals(sphereOptimized.normals);

				sphereMesh->setDiffuseColor(glm::vec3((float)x / 13.0f, (float)y / 13.0f, 1.0f));
				sphereEntity->transform->setPosition(x * 3, y * 3, z * 3);

				PBRMaterial* pbrMat = dynamic_cast<PBRMaterial*>(sphereMesh->material.get());
				if (pbrMat != nullptr)
				{
					pbrMat->roughness = (float)y / 13.0f + 0.001f;
					pbrMat->metallic = (float)z / 13.0f;
				}

				this->renderer.addEntity(std::move(sphereEntity));
			}
		}
	}

	// Sphere
	for (int i = 0; i < 10; i++)
	{
		std::unique_ptr<Entity> sphereEntity = std::make_unique<Entity>("Sphere");

		MeshComponent* sphereMesh = sphereEntity->addComponent<MeshComponent>();
		sphereMesh->setMaterial(std::make_unique<PBRMaterial>(pbrShader))
			.addVertices(sphereOptimized.vertices)
			.addIndices(sphereOptimized.indices)
			.addNormals(sphereOptimized.normals);

		PhysicsComponent* sphereCollider = sphereEntity->addComponent<PhysicsComponent>();
		this->renderer.physicsWorld->addSphere(sphereCollider, 1.0f, glm::vec3(0.0f, 25.0f, 0.0f));

		this->renderer.addEntity(std::move(sphereEntity));
	}

	// Skybox
	std::unique_ptr<Entity> skyEntity = std::unique_ptr<Entity>(new Entity("Skybox"));
	SkyboxComponent* skyComponent = skyEntity->addComponent<SkyboxComponent>();
	skyComponent->setupSkybox(skyboxShader, this->renderer);
	this->renderer.addEntity(std::move(skyEntity));

	glm::vec3 lightPositions[] = {
		glm::vec3(-10.0f,  10.0f, 10.0f),
		glm::vec3(10.0f,  10.0f, 10.0f),
		glm::vec3(-10.0f, -10.0f, 10.0f),
		glm::vec3(10.0f, -10.0f, 10.0f),
	};

	glm::vec3 lightColors[] = {
		glm::vec3(300.0f, 300.0f, 300.0f),
		glm::vec3(300.0f, 300.0f, 300.0f),
		glm::vec3(300.0f, 300.0f, 300.0f),
		glm::vec3(300.0f, 300.0f, 300.0f)
	};

	for (int i = 0; i < 4; i++)
	{
		// Add point light
		std::unique_ptr<Entity> pointLightEntity = std::unique_ptr<Entity>(new Entity("Point light"));
		pointLightEntity->transform->setScale(glm::vec3(0.1f));
		pointLightEntity->transform->setPosition(lightPositions[i]);
		PointLightComponent* pointLightComponent = pointLightEntity->addComponent<PointLightComponent>();
		pointLightComponent->diffuseColor = lightColors[i];

		this->renderer.addEntity(std::move(pointLightEntity));
	}

	// Plane
	std::vector<float> quadVertices = Geometry::getQuadVertices();
	std::unique_ptr<Entity> planeEntity = std::make_unique<Entity>("Plane");

	MeshComponent* planeMesh = planeEntity->addComponent<MeshComponent>();
	planeMesh->setMaterial(std::make_unique<PBRMaterial>(pbrShader))
		.addVertices(quadVertices);
	planeEntity->transform->setPosition(0.0f, -5.0f, 0.0f);
	planeEntity->transform->setRotation(-90.0f, 0.0f, 0.0f);
	planeEntity->transform->setScale(glm::vec3(20.0f, 20.0f, 1.0f));

	this->renderer.addEntity(std::move(planeEntity));

	this->renderer.physicsWorld->addPlane(glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f));

	// After all needed objects have been added, initializes the renderer's data to set up every object's data
	this->renderer.init(glm::vec2(10));
	this->camera = this->renderer.currentCamera;
}

void MainGameState::cleanup()
{
	this->renderer.end();
}

void MainGameState::pause()
{

}

void MainGameState::resume()
{

}

void MainGameState::handleEvents(GameEngine* gameEngine)
{

}

void MainGameState::update(GameEngine* gameEngine, float deltaTime)
{
	this->renderer.render(deltaTime);
}

void MainGameState::draw(GameEngine* gameEngine)
{

}
